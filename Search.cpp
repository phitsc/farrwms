#include "Search.h"
#include "FileList.h"
#include "Util.h"
#include "IniFile.h"

#include <algorithm>
#include <functional> 

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

            search.addItem("", 
                           iniFile.getParameterValue("", "description"),
						   iniFile.getParameterValue("", "searchUrl"), 
						   iniFile.getParameterValue("", "resultPattern"), 
						   iniFile.getParameterValue("", "farrCaption"),
						   iniFile.getParameterValue("", "farrGroup"),
						   iniFile.getParameterValue("", "farrPath"),
						   iconPath);

            _searches.push_back(search);
		}
    }
    catch(IniParameterNotFoundException& e)
    {
        OutputDebugString(e.what());
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
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasName(const std::string& name) const
{
	std::string temp(name);
	util::String::tolower(temp);

	return (_name == temp);
}

///////////////////////////////////////////////////////////////////////////////
