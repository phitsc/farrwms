#include "FarrPlugin.h"
#include "Util.h"
#include "Farr.h"

#include "config/Search.h"
#include "config/ParameterNames.h"

#include <sstream>

#include <algorithm>
#include <functional> 
#include <regex>

#include <shlwapi.h>

using namespace std::placeholders; 

namespace
{
    const std::string ValidOptionCharacters("+/-");
}

///////////////////////////////////////////////////////////////////////////////

FarrPlugin::FarrPlugin(const std::string& modulePath) :
    _currentSearch(0),
    _isSearching(false),
    _iconPath(modulePath + "\\icons\\"),
    _helpFile(modulePath + "\\" + farr::getReadMeFileName()),
	_searches(modulePath + "\\searches\\"),
    _searchesPath(modulePath + "\\searches\\")
{
    _xmlHttpRequest.CreateInstance(L"Msxml2.XMLHTTP.3.0");

    _xmlHttpEventSink = new XmlHttpEventSink(_xmlHttpRequest, *this);
}

///////////////////////////////////////////////////////////////////////////////

FarrPlugin::~FarrPlugin()
{
    _xmlHttpRequest->onreadystatechange = 0;
    delete _xmlHttpEventSink;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::clearResults()
{
    _farrItems.clear();
}

///////////////////////////////////////////////////////////////////////////////

FarrItems::size_type FarrPlugin::getItemCount() const
{
    return _farrItems.size();
}

///////////////////////////////////////////////////////////////////////////////

const FarrItem& FarrPlugin::getItem(const FarrItems::size_type& index) const
{
    return _farrItems[index];
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::showSearchInfo(const std::string& searchName)
{
    Searches::const_iterator it = std::find_if(_searches.begin(), _searches.end(), std::bind(&config::Search::hasName, _1, searchName));
    if(it != _searches.end())
    {
        const config::Search& search = *it;

        const std::string infoFileName = _searchesPath + search.getName() + ".html";
        if(PathFileExists(infoFileName.c_str()))
        {
            farr::showLocalHtmlFile(infoFileName);
        }
        else
        {
            const std::string searchInfoAsHtml = ""; //search.getInfoAsHtml();
            farr::showHtml(searchInfoAsHtml);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::search(const char* rawSearchString)
{
    if(_farrAlias.empty())
    {
        _farrAlias = farr::getPluginAlias() + " ";
    }

    _isSearching = farr::signalSearchStateChanged(true);

    clearResults();

    //
    std::string searchString(rawSearchString);
    // remove plugin alias
    const bool hasSpaceAfterAlias = (searchString.length() == _farrAlias.length());
    searchString.erase(0, _farrAlias.length());

    _currentSearchTerm = "";
    bool hasSubsearch = false;

    if(!processCommand(searchString))
    {
        std::string searchName;
        std::string subsearchName;
        splitSearch(searchString, searchName, subsearchName, _currentSearchTerm, hasSubsearch);

        if(!searchName.empty())
	    {
            const Searches::const_iterator it = std::find_if(_searches.begin(), _searches.end(), std::bind(&config::Search::hasName, _1, searchName));
		    if(it != _searches.end())
		    {
                const config::Search& search = *it;
                _currentSearch = search.hasSubsearch(subsearchName) ? &search.getSubsearch(subsearchName) : &search;

                const bool isFeed = (_currentSearch->getParameterValue(config::param::LoadFirst) == "true");
                if(isFeed)
                {
                    _logFile.writeLine("Search name: '" + searchName + "' Subsearch name: '" + subsearchName + "' Search term: '" + _currentSearchTerm + "' Has Subsearch: " + (hasSubsearch ? "yes" : "no") + " Is feed: yes");

                    if(!_currentSearchTerm.empty())
                    {
                        listCachedItems(_currentSearchTerm);
                    }
                    else if(!hasSubsearch || (hasSubsearch && !subsearchName.empty() && search.hasSubsearch(subsearchName)))
                    {
                        _farrItemCache.clear();

                        const std::string searchUrl = util::String::escapeUrl(_currentSearch->getParameterValue(config::param::Source));

                        _logFile.writeLine("Search URL: '" + searchUrl + "'");

                        sendRequest(searchUrl);

                        return; // searching continues
                    }
                    else if(!subsearchName.empty() || hasSubsearch)
                    {
                        listSubsearches(searchName, subsearchName);
                    }
                }
                else
                {
                    _logFile.writeLine("Search name: '" + searchName + "' Subsearch name: '" + subsearchName + "' Search term: '" + _currentSearchTerm + "' Has Subsearch: " + (hasSubsearch ? "yes" : "no") + " Is feed: no");

                    if(!_currentSearchTerm.empty())
                    {
                        const bool containsSearchTermVariable = (_currentSearch->getParameterValue(config::param::Source).find("%SEARCHTERM%") != std::string::npos);

                        Variables variables;
                        variables["%SEARCHTERM%"] = _currentSearchTerm;

                        const std::string searchUrl = util::String::escapeUrl(containsSearchTermVariable ? replaceVariables(_currentSearch->getParameterValue(config::param::Source), variables) : (_currentSearch->getParameterValue(config::param::Source) + _currentSearchTerm));

                        _logFile.writeLine("Search URL: '" + searchUrl + "'");

                        sendRequest(searchUrl);

                        return; // searching continues
                    }
                    else if(!subsearchName.empty() || hasSubsearch)
                    {
                        listSubsearches(searchName, subsearchName);
                    }
                }
            }
	    }
        else
        {
            listSearches(searchString);

            if(hasSpaceAfterAlias && searchString.empty())
            {
                farr::setStatusText(std::string("Type ? and hit enter to show help file. ") + ((_farrItems.size() > 1) ? (util::String::toString(_farrItems.size()) + " searches.") : ""));
            }
            else
            {
                farr::setStatusText(std::string("Hit Shift + Enter to display info about search. ") + ((_farrItems.size() > 1) ? (util::String::toString(_farrItems.size()) + " searches.") : ""));
            }
        }
    }

    if((long)_farrItems.size() > farr::getMaxResults())
    {
        farr::MenuItems menuItems;
        menuItems.push_back(farr::MenuItem("item", "Show more results", "Show more results", _iconPath + "Down_small.ico", "dosearch " + _farrAlias + "!showAllItems"));
        farr::addMenuItems(farr::Statusbar, menuItems);
    }

    _isSearching = farr::signalSearchStateChanged(false, getItemCount());
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::stopSearch()
{
    _xmlHttpRequest->abort();

    _isSearching = farr::signalSearchStateChanged(false);

    _logFile.writeLine("Stopped search");
}

///////////////////////////////////////////////////////////////////////////////

bool FarrPlugin::isSearching() const
{
    return _isSearching;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::onHttpRequestResponse(const std::string& /*responseText*/)
{
    clearResults();

    Variables variables;
    variables["%SEARCHTERM%"] = _currentSearchTerm;
    variables["%SEARCHURL%"] = replaceVariables(_currentSearch->getParameterValue(config::param::Source), variables);
    variables["%PLUGINALIAS%"] = farr::getPluginAlias();

    //const std::string resultPattern = replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "resultPattern"), variables);

    //_logFile.writeLine("Result pattern: '" + resultPattern + "'");

    //const std::regex expression(resultPattern);
    //std::sregex_iterator it(responseText.begin(), responseText.end(), expression);
    //std::sregex_iterator end;
    //for( ; it != end; ++it)
    //{
    //    const std::smatch match = *it;
    //    
    //    const std::string caption = getValue("farrCaption", match, variables);
    //    const std::string group = getValue("farrGroup", match, variables);
    //    const std::string url = getValue("farrPath", match, variables);

    //    FarrItem farrItem(caption, group, removeHttp(url), _currentSearch->getParameterValue(config::param::Icon), FarrItem::Url);

    //    for(unsigned long index = 1; index <= Searches::MaxContextMenuItemCount; ++index)
    //    {
    //        std::string contextType = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getParameterValue(_currentSubsearchName, "contextType" + util::String::toString(index)), variables)));
    //        const std::string contextCaption = getValue("contextCaption" + util::String::toString(index), match, variables);
    //        const std::string contextPath = getValue("contextPath" + util::String::toString(index), match, variables);
    //        if(contextType.empty() && (contextCaption.empty() || contextPath.empty()))
    //        {
    //            break;
    //        }
    //        else if(contextType.empty())
    //        {
    //            contextType = "item";
    //        }

    //        if(((contextType == "item") || (contextType == "submenu")) && contextCaption.empty())
    //        {
    //            break;
    //        }

    //        const std::string contextHint = getValue("contextHint" + util::String::toString(index), match, variables);
    //        const std::string contextIconPath = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "contextIcon" + util::String::toString(index)), variables)));

    //        farrItem.contextItems.push_back(ContextItem(contextType, contextCaption, contextHint, contextPath, contextIconPath));
    //    }

    //    _farrItems.push_back(farrItem);
    //}

    //_farrItemCache = _farrItems;

    //// add statusbar icons
    //farr::MenuItems menuItems;

    //for(unsigned long index = 1; index <= Searches::MaxStatusIconCount; ++index)
    //{
    //    const std::string statusCaptionPattern = replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "statusCaptionPattern" + util::String::toString(index)), variables);
    //    if(statusCaptionPattern.empty())
    //    {
    //        break;
    //    }

    //    const std::regex expression(statusCaptionPattern);
    //    std::smatch match;
    //    if(std::regex_search(responseText, match, expression))
    //    {
    //        const std::string statusCaption = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "statusCaption" + util::String::toString(index)), variables)));
    //        const std::string statusPath = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "statusPath" + util::String::toString(index)), variables)));
    //        if(!statusCaption.empty() && !statusPath.empty())
    //        {
    //            const std::string statusHint = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "statusHint" + util::String::toString(index)), variables)));
    //            const std::string statusIcon = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getParameter(_currentSubsearchName, "statusIcon" + util::String::toString(index)), variables)));

    //            menuItems.push_back(farr::MenuItem("item", statusCaption, statusHint, statusIcon, statusPath));
    //        }
    //    }
    //}

    //if((long)_farrItems.size() > farr::getMaxResults())
    //{
    //    menuItems.push_back(farr::MenuItem("item", "Show more results", "Show more results", _iconPath + "Down_small.ico", "dosearch " + _farrAlias + "!showAllItems"));
    //}

    //farr::addMenuItems(farr::Statusbar, menuItems);

    //
    std::stringstream stream;
    stream << "Found " << _farrItems.size() << " results";
    _logFile.writeLine(stream.str());

    _isSearching = farr::signalSearchStateChanged(false, getItemCount());
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::getValue(const std::string& /*parameterName*/, const std::smatch& /*match*/, const Variables& /*variables*/)
{
    //std::smatch secondMatch;
    //const std::string input = _currentSearch->getParameter(_currentSubsearchName, parameterName + "Input");
    //const std::string pattern = _currentSearch->getParameter(_currentSubsearchName, parameterName + "Pattern");
    //const std::string inputString = (input.empty() || pattern.empty()) ? "" : replaceCharacterEntityReferences(match.format(replaceVariables(input, variables)));
    //if(!input.empty() && !pattern.empty())
    //{
    //    std::regex regex(replaceCharacterEntityReferences(match.format(replaceVariables(pattern, variables))));
    //    std::regex_search(inputString, secondMatch, regex);
    //}

    //const std::smatch& matchToUse = (!input.empty() && !pattern.empty()) ? secondMatch : match;
    //return replaceCharacterEntityReferences(matchToUse.format(replaceVariables(_currentSearch->getParameter(_currentSubsearchName, parameterName), variables)));

    return "";
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::onHttpRequestFailed(const std::string& statusText)
{
    clearResults();

    const std::string errorText = "FarrWebMetaSearch Error!\n" + statusText;
    farr::displayAlertMessage(errorText);

    _isSearching = farr::signalSearchStateChanged(false, getItemCount());
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listHelpItems()
{
    _farrItems.push_back(FarrItem("Show help", "Open help file in an external browser.", _helpFile, _iconPath + "Help.ico", FarrItem::File));

    const std::string alias2 = _farrAlias + "!about";
    _farrItems.push_back(FarrItem("About", "", alias2, _iconPath + "About.ico", FarrItem::Alias));
}

///////////////////////////////////////////////////////////////////////////////

extern HINSTANCE dllInstanceHandle;

void FarrPlugin::listAboutItems()
{
    util::VersionInfo versionInfo(dllInstanceHandle);
    const std::string version = versionInfo.getFileVersion().getAsString() + " - " + versionInfo.getSpecialBuild();
    _farrItems.push_back(FarrItem(version, "Visit FarrWebMetaSearch Web Site", "farrwms.objecttechnology.com", _iconPath + "FarrWebMetaSearch.ico", FarrItem::Url));

    _farrItems.push_back(FarrItem("Philipp Tschannen (phitsc)", "Idea & Programming", "phitsc@gmail.com", _iconPath + "Philipp.ico", FarrItem::Unknown));

    _farrItems.push_back(FarrItem("Carroll - (hamradio on donationcoder.com forums)", "Icon Design", "www.hamradiousa.net", _iconPath + "Hamradio.ico", FarrItem::Url));

    _farrItems.push_back(FarrItem("Donate", "Support this plugin", "www.donationcoder.com/Forums/bb/index.php?action=dlist;sa=search;search=149485;fields=uid", _iconPath + "Donate.ico", FarrItem::Url));
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listCommandItems()
{
    if(_logFile.isEnabled())
    {
        _farrItems.push_back(FarrItem("Disable logging", "", _farrAlias + "!disableLogging", _iconPath + "DisableLogging.ico", FarrItem::Alias));
    }
    else
    {
        _farrItems.push_back(FarrItem("Enable logging", "", _farrAlias + "!enableLogging", _iconPath + "EnableLogging.ico", FarrItem::Alias));
    }

    if(PathFileExists(_logFile.getPath().c_str()))
    {
        _farrItems.push_back(FarrItem("Open log file in external editor", "", _logFile.getPath(), "", FarrItem::File));
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listSearches(const std::string& filter)
{
	std::for_each(_searches.begin(), _searches.end(), std::bind(&FarrPlugin::addSearchToResults, this, _1, filter));
    _farrItemCache = _farrItems;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::addSearchToResults(const config::Search& search, const std::string& filter)
{
    const std::string& searchName = search.getName();

    if(filter.empty() || util::String::containsSubstringNoCase(searchName, filter))
    {
        _farrItems.push_back(FarrItem(searchName, search.getParameterValue(config::param::Description), _farrAlias + search.getName(), _searchesPath + search.getParameterValue(config::param::Icon), FarrItem::Alias));
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listSubsearches(const std::string& searchName, const std::string& filter)
{
    if(!searchName.empty())
    {
        Searches::const_iterator it = std::find_if(_searches.begin(), _searches.end(), std::bind(&config::Search::hasName, _1, searchName));
        if(it != _searches.end())
        {
            const config::Search& search = (*it);

            std::for_each(search.subsearchesBegin(), search.subsearchesEnd(), std::bind(&FarrPlugin::addSubsearchToResults, this, search, _1, filter));
            _farrItemCache = _farrItems;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listCachedItems(const std::string& filter)
{
    std::for_each(_farrItemCache.begin(), _farrItemCache.end(), std::bind(&FarrPlugin::addItemToResults, this, _1, filter));
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::addSubsearchToResults(const config::Search& search, const config::SubsearchPtr& subsearch, const std::string& filter)
{
    const bool isHidden = (subsearch->getParameterValue(config::param::IsHidden) == "true");
    if(!isHidden && (filter.empty() || util::String::startsWithNoCase(subsearch->getName(), filter) || util::String::startsWithNoCase(subsearch->getAbbreviation(), filter)))
    {
        const std::string caption = search.getName() + " +" + subsearch->getName() + (!subsearch->getAbbreviation().empty() ? " (" + subsearch->getAbbreviation() + ")" : "");
        _farrItems.push_back(FarrItem(caption, subsearch->getParameterValue(config::param::Description), _farrAlias + search.getName() + " +" + subsearch->getName(), subsearch->getParameterValue(config::param::Icon), FarrItem::Alias));
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::addItemToResults(const FarrItem& farrItem, const std::string& filter)
{
    if(filter.empty() || util::String::containsSubstringNoCase(farrItem.caption, filter))
    {
        _farrItems.push_back(farrItem);
    }
}

///////////////////////////////////////////////////////////////////////////////

extern HINSTANCE dllInstanceHandle;

///////////////////////////////////////////////////////////////////////////////

bool FarrPlugin::processCommand(const std::string& searchString)
{
    if(!searchString.empty())
    {
        if(searchString[0] == '?')
        {
            listHelpItems();

            return true;
        }
        else if(std::regex_match(searchString, std::regex("!about(?: )?")))
        {
            listAboutItems();

            return true;
        }
        else if(std::regex_match(searchString, std::regex("!showAllItems(?: )?")))
        {
            farr::setShowAllMode();
            farr::clearMenuItems();
            listCachedItems("");
        }
        else if(std::regex_match(searchString, std::regex("!enableLogging(?: )?")))
        {
            if(_logFile.enable())
            {
                const std::string message = "FarrWebMetaSearch\nLogging enabled.";
                farr::displayAlertMessage(message);
            }
            else
            {
                const std::string errorText = "FarrWebMetaSearch Error!\nCreating log file failed.";
                farr::displayAlertMessage(errorText);
            }

            farr::setNewSearch(_farrAlias);
        }
        else if(std::regex_match(searchString, std::regex("!disableLogging(?: )?")))
        {
            _logFile.disable();

            const std::string message = "FarrWebMetaSearch\nLogging disabled.";
            farr::displayAlertMessage(message);

            farr::setNewSearch(_farrAlias);
        }
        else if(searchString[0] == '!')
        {
            listCommandItems();

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::sendRequest(const std::string& searchUrl)
{
    if(PathIsURL(searchUrl.c_str()))
    {
        _xmlHttpRequest->open("GET", searchUrl.c_str(), VARIANT_TRUE);
        _xmlHttpRequest->onreadystatechange = _xmlHttpEventSink;
        _xmlHttpRequest->send();
    }
    else
    {
        const std::string filePath = farr::resolveFile(searchUrl);
        std::ifstream file(filePath.c_str());
        if(file.is_open())
        {
            std::stringstream text;

            std::string line;
            while(std::getline(file, line))
            {
                text << line << std::endl;
            }

            onHttpRequestResponse(text.str());
        }
        else
        {
            _isSearching = farr::signalSearchStateChanged(false, getItemCount());

            const std::string errorText = "FarrWebMetaSearch Error!\n'" + filePath + "'\n not found.";
            farr::displayAlertMessage(errorText);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::splitSearch(const std::string& searchString, std::string& searchName, std::string& optionName, std::string& searchTerm, bool& hasSubsearch)
{
 	const std::string::size_type pos1 = searchString.find(' ');
	if(pos1 != std::string::npos)
	{
		searchName = searchString.substr(0, pos1);
        const std::string potentialOptionAndSearchTerm = searchString.substr(pos1 + 1);

        if(!potentialOptionAndSearchTerm.empty() && (potentialOptionAndSearchTerm.substr(0, 1).find_first_of(ValidOptionCharacters) == 0))
        {
            hasSubsearch = true;

            const std::string::size_type pos2 = potentialOptionAndSearchTerm.find(' ');
            if(pos2 != std::string::npos)
            {
                optionName = potentialOptionAndSearchTerm.substr(1, pos2 - 1);
                searchTerm = potentialOptionAndSearchTerm.substr(pos2 + 1);
            }
            else
            {
                optionName = potentialOptionAndSearchTerm.substr(1);
            }
        }
        else
        {
            searchTerm = potentialOptionAndSearchTerm;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::replaceCharacterEntityReferences(const std::string& text)
{
    std::string temp(text);

    // the numeric ones
	std::string::size_type pos = temp.find("&#");
	while(pos != std::string::npos)
	{
		const std::string::size_type pos2 = temp.find(';', pos + 2);
		if(pos2 == std::string::npos)
		{
			break;
		}

		const std::string charCode = temp.substr(pos + 2, pos2 - (pos + 2));
		const unsigned char character = static_cast<const unsigned char>(util::String::fromString<int>(charCode));
		if(character >= 32 && character < 255)
		{
			temp.replace(pos, pos2 - pos + 1, std::string(1, character));
		}

		pos = temp.find("&#", pos + 1);
	}

    // the special ones
    temp = std::regex_replace(temp, std::regex("&quot;"), std::string("\""));
    temp = std::regex_replace(temp, std::regex("&amp;"), std::string("&"));
    temp = std::regex_replace(temp, std::regex("&apos;"), std::string("'"));
    temp = std::regex_replace(temp, std::regex("&lt;"), std::string("<"));
    temp = std::regex_replace(temp, std::regex("&gt;"), std::string(">"));
    temp = std::regex_replace(temp, std::regex("&reg;"), std::string("®"));
    temp = std::regex_replace(temp, std::regex("&copy;"), std::string("©"));
    temp = std::regex_replace(temp, std::regex("&nbsp;"), std::string(" "));
    temp = std::regex_replace(temp, std::regex("&nbsp;"), std::string(" "));
    temp = std::regex_replace(temp, std::regex("&deg;"), std::string("°"));

    return temp;
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::removeHttp(const std::string& url)
{
    std::string temp(url);

    temp = std::regex_replace(temp, std::regex("http://"), std::string(""));

    return temp;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::replaceVariable(std::string& text, const Variables::value_type& variableAndValue)
{
    text = std::regex_replace(text, std::regex(variableAndValue.first), variableAndValue.second);
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::replaceVariables(const std::string& text, const Variables& variables)
{
    std::string temp(text);
    std::for_each(variables.begin(), variables.end(), std::bind(&FarrPlugin::replaceVariable, std::ref(temp), _1));
    return temp;
}

///////////////////////////////////////////////////////////////////////////////

//std::string FarrPlugin::fixLink(const std::string& link, const std::string& searchUrl)
//{
//	if(!link.empty() && link[0] == '/')
//	{
//		// relative path. add domain.
//		if(searchUrl.find("http://") == 0)
//		{
//			const std::string::size_type pos = searchUrl.find('/', 7); // start after http://
//			if(pos != std::string::npos)
//			{
//				return searchUrl.substr(0, pos) + link;
//			}
//		}
//	}
//
//	return link;
//}

///////////////////////////////////////////////////////////////////////////////
