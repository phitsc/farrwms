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

    const std::string pattern = iniFile.getParameterValue(categoryName, "resultPattern", "");

    if(isValidRegularExpression(search.getName(), categoryName, pattern))
    {
        search.addItem(categoryName, 
                       iniFile.getParameterValue(categoryName, "description", ""),
                       iniFile.getParameterValue(categoryName, "searchUrl", ""), 
                       pattern, 
                       iniFile.getParameterValue(categoryName, "farrCaption", ""),
                       iniFile.getParameterValue(categoryName, "farrGroup", ""),
                       iniFile.getParameterValue(categoryName, "farrPath", ""),
                       (PathFileExists(categoryIconPath) == TRUE) ? categoryIconPath : iconPath);
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
        stream << searchName << " [" << categoryName << "]. Invalid Regular Expression. " << e.what() << "(" << e.code() << ")" << std::endl;

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

void Search::addItem(const std::string& optionName,
                     const std::string& description,
                     const std::string& searchUrl, 
                     const std::string& resultPattern, 
                     const std::string& farrCaption, 
                     const std::string& farrGroup, 
                     const std::string& farrPath, 
                     const std::string& farrIconPath)
{
    _descriptions[optionName] = description;
	_searchUrls[optionName] = searchUrl;
	_resultPatterns[optionName] = resultPattern;
    _farrCaptions[optionName] = farrCaption;
    _farrGroups[optionName] = farrGroup;
    _farrPaths[optionName] = farrPath;
	_farrIconPaths[optionName] = farrIconPath;

    _optionNames.insert(optionName);
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasName(const std::string& name) const
{
	std::string temp(name);
	util::String::tolower(temp);

	return (_name == temp);
}

///////////////////////////////////////////////////////////////////////////////
