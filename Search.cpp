#include "Search.h"
#include "FileList.h"
#include "Util.h"
#include "IniFile.h"

#include <algorithm>
#include <functional> 
#include <regex>

using namespace std::tr1::placeholders; 

///////////////////////////////////////////////////////////////////////////////

Searches::Searches(const std::string& searchesDirectory)
{
	util::FileList searchesFiles(searchesDirectory, "*.conf", util::FileList::Files);

	std::for_each(searchesFiles.begin(), searchesFiles.end(), std::tr1::bind(&Searches::addSearch, this, _1));
}

///////////////////////////////////////////////////////////////////////////////

void Searches::addSearch(const std::string& searchFile)
{
    try
    {
        IniFile iniFile(searchFile);
        if(iniFile.isOpen())
		{
			char searchFileName[MAX_PATH] = {0};
			util::String::copyString(searchFileName, MAX_PATH, searchFile);
			PathRemoveExtension(searchFileName);
			const std::string searchName = PathFindFileName(searchFileName);
			const std::string iconPath = findIconPath(searchFile);

			Search search(searchName);

            std::for_each(iniFile.sectionsBegin(), iniFile.sectionsEnd(), std::tr1::bind(&Searches::addItemToSearch, std::tr1::ref(search), std::tr1::ref(iniFile), _1, std::tr1::ref(iconPath)));

            _searches.push_back(search);
		}
    }
    catch(IniParameterNotFoundException& e)
    {
        OutputDebugString(e.what());
    }
}

///////////////////////////////////////////////////////////////////////////////

void Searches::addItemToSearch(Search& search, const IniFile& iniFile, const std::string& categoryName, const std::string& iconPath)
{
    const std::string::size_type pos = categoryName.find('|');
    const std::string subsearchName = (pos == std::string::npos) ? categoryName : categoryName.substr(0, pos);
    const std::string abbreviation = (pos == std::string::npos) ? "" : categoryName.substr(pos + 1);

    char categoryIconPath[MAX_PATH] = { 0 };
    PathAppend(categoryIconPath, iconPath.c_str());
    PathRemoveExtension(categoryIconPath);
    PathAppend(categoryIconPath, std::string("-" + subsearchName).c_str());
    PathAddExtension(categoryIconPath, ".ico");

    const std::string pattern = iniFile.getParameterValue(categoryName, "resultPattern", "__UNDEF");

    if(isValidRegularExpression(search.getName(), subsearchName, pattern))
    {
        const std::string sortOrder = iniFile.getParameterValue(categoryName, "sortOrder", "__UNDEF");

        Search::Parameters& parameters = search.addSubsearch(subsearchName, abbreviation, (sortOrder == "__UNDEF") ? 0 : util::String::fromString<long>(sortOrder));
        assignProperty(parameters, "description", iniFile, categoryName);
        assignProperty(parameters, "contributor", iniFile, categoryName);
        assignProperty(parameters, "searchUrl", iniFile, categoryName);
        assignProperty(parameters, "isFeed", iniFile.getParameterValue(categoryName, "isFeed", "false"));
        assignProperty(parameters, "resultPattern", pattern);

        assignProperty(parameters, "farrCaptionInput", iniFile, categoryName);
        assignProperty(parameters, "farrCaptionPattern", iniFile, categoryName);
        assignProperty(parameters, "farrCaption", iniFile, categoryName);

        assignProperty(parameters, "farrGroupInput", iniFile, categoryName);
        assignProperty(parameters, "farrGroupPattern", iniFile, categoryName);
        assignProperty(parameters, "farrGroup", iniFile, categoryName);

        assignProperty(parameters, "farrPathInput", iniFile, categoryName);
        assignProperty(parameters, "farrPathPattern", iniFile, categoryName);
        assignProperty(parameters, "farrPath", iniFile, categoryName);

        assignProperty(parameters, "farrIconPath", (PathFileExists(categoryIconPath) == TRUE) ? categoryIconPath : iconPath);
        assignProperty(parameters, "isHidden", iniFile, categoryName);

        for(unsigned long index = 1; index <= MaxContextMenuItemCount; ++index)
        {
            const bool hasType    = assignProperty(parameters, "contextType" + util::String::toString(index), iniFile, categoryName);
            const bool hasCaption = assignProperty(parameters, "contextCaption" + util::String::toString(index), iniFile, categoryName);
            if(hasType || hasCaption)
            {
                assignProperty(parameters, "contextCaptionInput" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "contextCaptionPattern" + util::String::toString(index), iniFile, categoryName);

                assignProperty(parameters, "contextHintInput" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "contextHintPattern" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "contextHint" + util::String::toString(index), iniFile, categoryName);

                assignProperty(parameters, "contextPathInput" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "contextPathPattern" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "contextPath" + util::String::toString(index), iniFile, categoryName);

                assignIconProperty(parameters, "contextIcon" + util::String::toString(index), iniFile, categoryName, iconPath);
            }
            else
            {
                break;
            }
        }

        for(unsigned long index = 1; index <= MaxStatusIconCount; ++index)
        {
            if(assignProperty(parameters, "statusCaptionPattern" + util::String::toString(index), iniFile, categoryName))
            {
                assignProperty(parameters, "statusCaption" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "statusHint" + util::String::toString(index), iniFile, categoryName);
                assignProperty(parameters, "statusPath" + util::String::toString(index), iniFile, categoryName);
                assignIconProperty(parameters, "statusIcon" + util::String::toString(index), iniFile, categoryName, iconPath);
            }
            else
            {
                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

std::string Searches::findIconPath(const std::string& searchFile)
{
	char iconPath[MAX_PATH] = {0};
	util::String::copyString(iconPath, MAX_PATH, searchFile);

	PathRenameExtension(iconPath, ".ico");
	if(PathFileExists(iconPath))
	{
		return iconPath;
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////

bool Searches::isValidRegularExpression(const std::string& searchName, const std::string& optionName, const std::string& pattern)
{
    try 
    { 
        std::tr1::regex rx(pattern); 
    } 
    catch(const std::tr1::regex_error& e) 
    { 
        std::stringstream stream;
        stream << searchName << " [" << optionName << "]. Invalid Regular Expression. " << e.what() << " (" << e.code() << ")" << std::endl;

        OutputDebugString(stream.str().c_str());

        //std::tr1::regex_constants::error_paren

        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Searches::assignProperty(Search::Parameters& parameters, const std::string& parameterName, const IniFile& iniFile, const std::string& categoryName)
{
    return assignProperty(parameters, parameterName, iniFile.getParameterValue(categoryName, parameterName, "__UNDEF"));
}

///////////////////////////////////////////////////////////////////////////////

void Searches::assignIconProperty(Search::Parameters& parameters, const std::string& parameterName, const IniFile& iniFile, const std::string& categoryName, const std::string& searchIconPath)
{
    const std::string iconName = iniFile.getParameterValue(categoryName, parameterName, "__UNDEF");
    if(iconName != "__UNDEF")
    {
        char iconPath[MAX_PATH] = { 0 };
        PathAppend(iconPath, searchIconPath.c_str());
        PathRemoveFileSpec(iconPath);
        PathAppend(iconPath, iconName.c_str());
        if(PathFileExists(iconPath) == TRUE)
        {
            assignProperty(parameters, parameterName, iconPath);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Search::Search(const std::string& name) :
	_name(name)
{
	util::String::tolower(_name);
}

///////////////////////////////////////////////////////////////////////////////

Search::Parameters& Search::addSubsearch(const std::string& optionName, const std::string& abbreviation, long sortOrder)
{
    return _subsearches.insert(Subsearch(optionName, abbreviation, sortOrder)).first->parameters;
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasName(const std::string& name) const
{
	std::string temp(name);
	util::String::tolower(temp);

	return (_name == temp);
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasSubsearch(const std::string& subSearchNameOrAbbreviation) const
{
    const Subsearches::const_iterator it = std::find_if(_subsearches.begin(), _subsearches.end(), std::tr1::bind(&Subsearch::equalsNameOrAbbreviation, _1, subSearchNameOrAbbreviation));
    return (it != _subsearches.end());
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Search::getParameter(const std::string& subsearchName, const std::string& parameterName) const
{
    const Subsearches::const_iterator it = std::find_if(_subsearches.begin(), _subsearches.end(), std::tr1::bind(&Subsearch::equalsNameOrAbbreviation, _1, subsearchName));
    if(it != _subsearches.end())
    {
        const Parameters& parameters = it->parameters;

        const Parameters::const_iterator it2 = parameters.find(parameterName);
        if(it2 != parameters.end())
        {
            const std::string& value = it2->second;
            return value;
        }
    }

    static std::string empty;
    return empty;
}

///////////////////////////////////////////////////////////////////////////////

const std::string Search::getInfoAsHtml() const
{
    std::stringstream stream;
    stream << "<body><h2>" << _name << "</h2>";
    
    const std::string& contributor = getParameter("", "contributor");
    if(!contributor.empty())
    {
        stream << "<i>by " << contributor << "</i></br></br>";
    }

    stream << getParameter("", "description");

    if(_subsearches.size() > 1)
    {
        stream << "<h3>Subsearches</h3>";
        stream << "<ul>";
        std::for_each(_subsearches.begin(), _subsearches.end(), std::tr1::bind(&Search::getSubsearchInfoAsHtml, this, _1, std::tr1::ref(stream)));
        stream << "</ul>";
    }

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////

void Search::getSubsearchInfoAsHtml(const Subsearch& subsearch, std::stringstream& stream) const
{
    const bool isHidden = (getParameter(subsearch.name, "isHidden") == "true");
    if(!subsearch.name.empty() && !isHidden)
    {
        stream << "<li><i>" << subsearch.name;
        if(!subsearch.abbreviation.empty())
        {
            stream << " (" << subsearch.abbreviation << ")";
        }
        
        stream << "</i>";

        const Parameters::const_iterator it2 = subsearch.parameters.find("description");
        if(it2 != subsearch.parameters.end())
        {
            stream << " : " << it2->second; 
        }

        stream << "</li>";
    }
}

///////////////////////////////////////////////////////////////////////////////
