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
    const std::string optionName = (pos == std::string::npos) ? categoryName : categoryName.substr(0, pos);
    const std::string abbreviation = (pos == std::string::npos) ? "" : categoryName.substr(pos + 1);

    char categoryIconPath[MAX_PATH] = { 0 };
    PathAppend(categoryIconPath, iconPath.c_str());
    PathRemoveExtension(categoryIconPath);
    PathAppend(categoryIconPath, std::string("-" + optionName).c_str());
    PathAddExtension(categoryIconPath, ".ico");

    const std::string pattern = iniFile.getParameterValue(categoryName, "resultPattern", "__UNDEF");

    if(isValidRegularExpression(search.getName(), optionName, pattern))
    {
        Search::Parameters& parameters = search.addSubsearch(optionName, abbreviation);
        assignProperty(parameters, "description", iniFile.getParameterValue(categoryName, "description", "__UNDEF"));
        assignProperty(parameters, "contributor", iniFile.getParameterValue(categoryName, "contributor", "__UNDEF"));
        assignProperty(parameters, "searchUrl", iniFile.getParameterValue(categoryName, "searchUrl", "__UNDEF"));
        assignProperty(parameters, "isFeed", iniFile.getParameterValue(categoryName, "isFeed", "false"));
        assignProperty(parameters, "resultPattern", pattern);
        assignProperty(parameters, "farrCaption", iniFile.getParameterValue(categoryName, "farrCaption", "__UNDEF"));
        assignProperty(parameters, "farrGroup", iniFile.getParameterValue(categoryName, "farrGroup", "__UNDEF"));
        assignProperty(parameters, "farrPath", iniFile.getParameterValue(categoryName, "farrPath", "__UNDEF"));
        assignProperty(parameters, "farrIconPath", (PathFileExists(categoryIconPath) == TRUE) ? categoryIconPath : iconPath);

        for(unsigned long index = 1; index <= 9; ++index)
        {
            if(assignProperty(parameters, "contextCaption" + util::String::toString(index), iniFile.getParameterValue(categoryName, "contextCaption" + util::String::toString(index), "__UNDEF")))
            {
                assignProperty(parameters, "contextHint" + util::String::toString(index), iniFile.getParameterValue(categoryName, "contextHint" + util::String::toString(index), "__UNDEF"));
                assignProperty(parameters, "contextPath" + util::String::toString(index), iniFile.getParameterValue(categoryName, "contextPath" + util::String::toString(index), "__UNDEF"));

                std::string iconName = iniFile.getParameterValue(categoryName, "contextIcon" + util::String::toString(index), "__UNDEF");
                if(iconPath != "__UNDEF")
                {
                    char contextIconPath[MAX_PATH] = { 0 };
                    PathAppend(contextIconPath, iconPath.c_str());
                    PathRemoveFileSpec(contextIconPath);
                    PathAppend(contextIconPath, iconName.c_str());
                    if(PathFileExists(contextIconPath) == TRUE)
                    {
                        assignProperty(parameters, "contextIcon" + util::String::toString(index), contextIconPath);
                    }
                }
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
///////////////////////////////////////////////////////////////////////////////

Search::Search(const std::string& name) :
	_name(name)
{
	util::String::tolower(_name);
}

///////////////////////////////////////////////////////////////////////////////

Search::Parameters& Search::addSubsearch(const std::string& optionName, const std::string& abbreviation)
{
    return _subsearches.insert(Subsearch(optionName, abbreviation)).first->parameters;
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
    }

    {
        const Subsearches::const_iterator it = std::find_if(_subsearches.begin(), _subsearches.end(), std::tr1::bind(&Subsearch::equalsName, _1, ""));
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

void Search::getSubsearchInfoAsHtml(const Subsearch& option, std::stringstream& stream) const
{
    if(!option.name.empty())
    {
        stream << "<li><i>" << option.name;
        if(!option.abbreviation.empty())
        {
            stream << " (" << option.abbreviation << ")";
        }
        
        stream << "</i>";

        const Parameters::const_iterator it2 = option.parameters.find("description");
        if(it2 != option.parameters.end())
        {
            stream << " : " << it2->second; 
        }

        stream << "</li>";
    }
}

///////////////////////////////////////////////////////////////////////////////
