#include "FarrPlugin.h"
#include "Util.h"
#include "Farr.h"

#include <sstream>

#include <algorithm>
#include <functional> 
#include <regex>

using namespace std::tr1::placeholders; 

///////////////////////////////////////////////////////////////////////////////

FarrPlugin::FarrPlugin(const std::string& modulePath) :
    _currentSearch(0),
    _isSearching(false),
    _iconPath(modulePath + "\\icons\\"),
    _helpFile(modulePath + "\\" + farr::getReadMeFileName()),
	_searches(modulePath + "\\searches\\")
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

    std::string searchName;
    _currentOptionName = "";
    _currentSearchTerm = "";
    bool hasOption = false;

    if(!processCommand(searchString))
    {
        splitSearch(searchString, searchName, _currentOptionName, _currentSearchTerm, hasOption);

        if(!searchName.empty())
	    {
		    Searches::const_iterator it = std::find_if(_searches.begin(), _searches.end(), std::tr1::bind(&Search::hasName, _1, searchName));
		    if(it != _searches.end())
		    {
			    _currentSearch = &(*it);

                const bool isFeed = _currentSearch->getIsFeed(_currentOptionName);
                if(isFeed)
                {
                    if(_currentSearchTerm.empty())
                    {
                        _farrItemCache.clear();

                        const std::string searchUrl = _currentSearch->getSearchUrl(_currentOptionName);

                        _xmlHttpRequest->open("GET", searchUrl.c_str(), VARIANT_TRUE);
                        _xmlHttpRequest->onreadystatechange = _xmlHttpEventSink;
                        _xmlHttpRequest->send();

                        return; // searching continues
                    }
                    else
                    {
                        listCachedItems(_currentSearchTerm);
                    }
                }
                else
                {
                    if(!_currentSearchTerm.empty())
                    {
                        const bool containsSearchTermVariable = (_currentSearch->getSearchUrl(_currentOptionName).find("%SEARCHTERM%") != std::string::npos);

                        Variables variables;
                        variables["%SEARCHTERM%"] = _currentSearchTerm;

                        const std::string searchUrl = containsSearchTermVariable ? replaceVariables(_currentSearch->getSearchUrl(_currentOptionName), variables) : (_currentSearch->getSearchUrl(_currentOptionName) + _currentSearchTerm);

                        _xmlHttpRequest->open("GET", searchUrl.c_str(), VARIANT_TRUE);
                        _xmlHttpRequest->onreadystatechange = _xmlHttpEventSink;
                        _xmlHttpRequest->send();

                        return; // searching continues
                    }
                    else if(!_currentOptionName.empty() || hasOption)
                    {
                        listOptions(searchName, _currentOptionName);
                    }
                }
            }
	    }
        else
        {
            listSearches(searchString);

            if(hasSpaceAfterAlias && searchString.empty())
            {
                farr::setStatusText("Type ? and hit enter to show help file.");
            }
        }
    }

    _isSearching = farr::signalSearchStateChanged(false, getItemCount());
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::stopSearch()
{
    _xmlHttpRequest->abort();
    clearResults();

    _isSearching = farr::signalSearchStateChanged(false);
}

///////////////////////////////////////////////////////////////////////////////

bool FarrPlugin::isSearching() const
{
    return _isSearching;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::onHttpRequestResponse(const std::string& responseText)
{
    clearResults();

    Variables variables;
    variables["%SEARCHTERM%"] = _currentSearchTerm;
    variables["%SEARCHURL%"] = replaceVariables(_currentSearch->getSearchUrl(_currentOptionName), variables);

    const std::string resultPattern = replaceVariables(_currentSearch->getResultPattern(_currentOptionName), variables);

    //std::stringstream stream;
    //stream << "search term: '" << _currentSearchTerm << "'\n";
    //stream << "regex: '" << resultPattern << "'\n";

    //OutputDebugString(stream.str().c_str());

    const std::tr1::regex expression(resultPattern);
    std::tr1::sregex_iterator it(responseText.begin(), responseText.end(), expression);
    std::tr1::sregex_iterator end;
    for( ; it != end; ++it)
    {
        const std::tr1::smatch match = *it;

        const std::string caption = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getFarrCaption(_currentOptionName), variables)));
        const std::string group = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getFarrGroup(_currentOptionName), variables)));
        const std::string url = replaceCharacterEntityReferences(match.format(replaceVariables(_currentSearch->getFarrPath(_currentOptionName), variables)));

        _farrItems.push_back(FarrItem(caption, group, removeHttp(url), _currentSearch->getFarrIconPath(_currentOptionName), FarrItem::Url));
    }

    _farrItemCache = _farrItems;

    if((long)_farrItems.size() > farr::getMaxResults())
    {
        farr::addStatusBarMenuItem("Show more results", "Show more results", _iconPath + "Down_small.ico", "dosearch " + _farrAlias + "!showAllItems");
    }

    _isSearching = farr::signalSearchStateChanged(false, getItemCount());
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

void FarrPlugin::listSearches(const std::string& filter)
{
	std::for_each(_searches.begin(), _searches.end(), std::tr1::bind(&FarrPlugin::addSearchToResults, this, _1, filter));
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::addSearchToResults(const Search& search, const std::string& filter)
{
    const std::string& searchName = search.getName();

    if(filter.empty() || util::String::containsSubstringNoCase(searchName, filter))
    {
        _farrItems.push_back(FarrItem(searchName, search.getDescription(""), _farrAlias + search.getName(), search.getFarrIconPath(""), FarrItem::Alias));
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listOptions(const std::string& searchName, const std::string& filter)
{
    if(!searchName.empty())
    {
        Searches::const_iterator it = std::find_if(_searches.begin(), _searches.end(), std::tr1::bind(&Search::hasName, _1, searchName));
        if(it != _searches.end())
        {
            const Search& search = (*it);

            std::for_each(search.optionNamesBegin(), search.optionNamesEnd(), std::tr1::bind(&FarrPlugin::addOptionToResults, this, search, _1, filter));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::listCachedItems(const std::string& filter)
{
    std::for_each(_farrItemCache.begin(), _farrItemCache.end(), std::tr1::bind(&FarrPlugin::addItemToResults, this, _1, filter));
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::addOptionToResults(const Search& search, const std::string& optionName, const std::string& filter)
{
    if(filter.empty() || util::String::containsSubstringNoCase(optionName, filter))
    {
        const std::string caption = search.getName() + " - " + optionName;
        _farrItems.push_back(FarrItem(caption, search.getDescription(optionName), _farrAlias + search.getName() + " +" + optionName, search.getFarrIconPath(optionName), FarrItem::Alias));
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
        else if(searchString.find("!about") == 0)
        {
            listAboutItems();

            return true;
        }
        else if(searchString.find("!showAllItems") == 0)
        {
            farr::setShowAllMode();
            listCachedItems("");
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::splitSearch(const std::string& searchString, std::string& searchName, std::string& optionName, std::string& searchTerm, bool& hasOption)
{
 	const std::string::size_type pos1 = searchString.find(' ');
	if(pos1 != std::string::npos)
	{
		searchName = searchString.substr(0, pos1);
        const std::string potentialOptionAndSearchTerm = searchString.substr(pos1 + 1);

        if(!potentialOptionAndSearchTerm.empty() && (potentialOptionAndSearchTerm.substr(0, 1).find_first_of("+/-") == 0))
        {
            hasOption = true;

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
		const char character = static_cast<const char>(util::String::fromString<int>(charCode));
		if(character >= 32 && character < 127)
		{
			temp.replace(pos, pos2 - pos + 1, std::string(1, character));
		}

		pos = temp.find("&#", pos + 1);
	}

    // the special ones
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&quot;"), std::string("\""));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&amp;"), std::string("&"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&apos;"), std::string("'"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&lt;"), std::string("<"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&gt;"), std::string(">"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&reg;"), std::string("®"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#174;"), std::string("®"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&copy;"), std::string("©"));

    return temp;
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::removeHttp(const std::string& url)
{
    std::string temp(url);

    temp = std::tr1::regex_replace(temp, std::tr1::regex("http://"), std::string(""));

    return temp;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::replaceVariable(std::string& text, const Variables::value_type& variableAndValue)
{
    text = std::tr1::regex_replace(text, std::tr1::regex(variableAndValue.first), variableAndValue.second);
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::replaceVariables(const std::string& text, const Variables& variables)
{
    std::string temp(text);
    std::for_each(variables.begin(), variables.end(), std::tr1::bind(&FarrPlugin::replaceVariable, std::tr1::ref(temp), _1));
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
