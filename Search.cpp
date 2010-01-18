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
    char categoryIconPath[MAX_PATH];
    PathAppend(categoryIconPath, iconPath.c_str());
    PathRemoveExtension(categoryIconPath);
    PathAppend(categoryIconPath, std::string("-" + categoryName).c_str());
    PathAddExtension(categoryIconPath, ".ico");

    const std::string pattern = iniFile.getParameterValue(categoryName, "resultPattern", "__UNDEF");

    if(isValidRegularExpression(search.getName(), categoryName, pattern))
    {
        Search::Parameters& parameters = search.addOption(categoryName);
        assignProperty(parameters, "description", iniFile.getParameterValue(categoryName, "description", "__UNDEF"));
        assignProperty(parameters, "contributor", iniFile.getParameterValue(categoryName, "contributor", "__UNDEF"));
        assignProperty(parameters, "searchUrl", iniFile.getParameterValue(categoryName, "searchUrl", "__UNDEF"));
        assignProperty(parameters, "isFeed", iniFile.getParameterValue(categoryName, "isFeed", "false"));
        assignProperty(parameters, "resultPattern", pattern);
        assignProperty(parameters, "farrCaption", iniFile.getParameterValue(categoryName, "farrCaption", "__UNDEF"));
        assignProperty(parameters, "farrGroup", iniFile.getParameterValue(categoryName, "farrGroup", "__UNDEF"));
        assignProperty(parameters, "farrPath", iniFile.getParameterValue(categoryName, "farrPath", "__UNDEF"));
        assignProperty(parameters, "farrIconPath", (PathFileExists(categoryIconPath) == TRUE) ? categoryIconPath : iconPath);
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

bool Searches::isValidRegularExpression(const std::string& searchName, const std::string& categoryName, const std::string& pattern)
{
    try 
    { 
        std::tr1::regex rx(pattern); 
    } 
    catch(const std::tr1::regex_error& e) 
    { 
        std::stringstream stream;
        stream << searchName << " [" << categoryName << "]. Invalid Regular Expression. " << e.what() << " (" << e.code() << ")" << std::endl;

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

Search::Parameters& Search::addOption(const std::string& optionName)
{
    _optionNames.insert(optionName);
    return _options[optionName];
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasName(const std::string& name) const
{
	std::string temp(name);
	util::String::tolower(temp);

	return (_name == temp);
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Search::getParameter(const std::string& optionName, const std::string& parameterName) const
{
    {
        const Options::const_iterator it = _options.find(optionName);
        if(it != _options.end())
        {
            const Parameters& parameters = it->second;

            const Parameters::const_iterator it2 = parameters.find(parameterName);
            if(it2 != parameters.end())
            {
                const std::string& value = it2->second;
                return value;
            }
        }
    }

    {
        const Options::const_iterator it = _options.find("");
        if(it != _options.end())
        {
            const Parameters& parameters = it->second;

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

    if(_optionNames.size() > 1)
    {
        stream << "<h3>Subsearches</h3>";
        stream << "<ul>";
        std::for_each(_optionNames.begin(), _optionNames.end(), std::tr1::bind(&Search::getOptionInfoAsHtml, this, _1, std::tr1::ref(stream)));
        stream << "</ul>";
    }

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////

void Search::getOptionInfoAsHtml(const std::string& optionName, std::stringstream& stream) const
{
    if(!optionName.empty())
    {
        stream << "<li><i>" << optionName << "</i>";

        const Options::const_iterator it = _options.find(optionName);
        if(it != _options.end())
        {
            const Parameters& parameters = it->second;
            const Parameters::const_iterator it2 = parameters.find("description");
            if(it2 != parameters.end())
            {
                stream << ": " << it2->second; 
            }
        }

        stream << "</li>";
    }
}

///////////////////////////////////////////////////////////////////////////////
