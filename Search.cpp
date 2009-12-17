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
	util::FileList searchesFiles(searchesDirectory, "*.ini", util::FileList::Files);

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

			_searches.push_back(Search(searchName,
                                       iniFile.getParameterValue("description"),
									   iniFile.getParameterValue("searchUrl"), 
									   iniFile.getParameterValue("resultPattern"), 
									   iniFile.getParameterValue("farrCaption"),
									   iniFile.getParameterValue("farrGroup"),
									   iniFile.getParameterValue("farrPath"),
									   iconPath));
		}
    }
    catch(IniParameterNotFoundException&)
    {
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

Search::Search(const std::string& name,
               const std::string& description,
               const std::string& searchUrl, 
               const std::string& resultPattern, 
               const std::string& farrCaption, 
               const std::string& farrGroup, 
               const std::string& farrPath, 
               const std::string& farrIconPath) :
	_name(name),
    _description(description),
	_searchUrl(searchUrl),
	_resultPattern(resultPattern),
    _farrCaption(farrCaption),
    _farrGroup(farrGroup),
    _farrPath(farrPath),
	_farrIconPath(farrIconPath)
{
	util::String::tolower(_name);
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasName(const std::string& name) const
{
	std::string temp(name);
	util::String::tolower(temp);

	return (_name == temp);
}

///////////////////////////////////////////////////////////////////////////////
