#include "Search.h"
#include "FileList.h"
#include "Util.h"

#include <algorithm>
#include <functional> 

using namespace std::tr1::placeholders; 

///////////////////////////////////////////////////////////////////////////////

Searches::Searches(const std::string& searchesDirectory)
{
    _document = MSXMLHelper::createDomDocument();
    _document->async = VARIANT_FALSE;

	util::FileList searchesFiles(searchesDirectory, "*.xml", util::FileList::Files);

	std::for_each(searchesFiles.begin(), searchesFiles.end(), std::tr1::bind(&Searches::addSearch, this, _1));
}

///////////////////////////////////////////////////////////////////////////////

Searches::~Searches()
{
    _document.Release();
    _document = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Searches::addSearch(const std::string& searchFile)
{
    try
    {
        const VARIANT_BOOL ret = _document->load(searchFile.c_str());

		if(ret == VARIANT_TRUE)
		{
			MSXML2::IXMLDOMNodePtr searchNode = _document->selectSingleNode("search");

			MSXML2::IXMLDOMNodePtr searchUrlNode = searchNode->selectSingleNode("searchUrl");
			MSXML2::IXMLDOMNodePtr resultPatternNode = searchNode->selectSingleNode("resultPattern");
			MSXML2::IXMLDOMNodePtr captionDescriptionUrlNode = searchNode->selectSingleNode("captionDescriptionUrl");

			char searchFileName[MAX_PATH] = {0};
			util::String::copyString(searchFileName, MAX_PATH, searchFile);
			PathRemoveExtension(searchFileName);
			const std::string searchName = PathFindFileName(searchFileName);
			const std::string iconPath = findIconPath(searchFile);

			_searches.push_back(Search(searchName, 
									   static_cast<const char*>(searchUrlNode->text), 
									   static_cast<const char*>(resultPatternNode->text), 
									   static_cast<const char*>(captionDescriptionUrlNode->text),
									   iconPath));
		}
		else
        {     
            MSXML2::IXMLDOMParseErrorPtr parseError = _document->parseError;
            _bstr_t reason = parseError->reason;
            long line = parseError->line;
            long linePosition = parseError->linepos;

            std::stringstream stream;
            stream << "Loading XML-file " << searchFile << " failed. ";
            stream << std::string(reason);
            stream << "Error in line " << line << " at position " << linePosition;

            std::string temp = stream.str();
        }
    }
    catch(_com_error& e)
    {
        std::stringstream stream;
        stream << "Loading XML-file " << searchFile << " failed. ";
        stream << e.ErrorMessage();
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

Search::Search(const std::string& name, const std::string& searchUrl, const std::string& resultPattern, const std::string& resultOrder, const std::string& iconPath) :
	_name(name),
	_searchUrl(searchUrl),
	_resultPattern(resultPattern),
	_iconPath(iconPath)
{
	util::String::tolower(_name);

	initGroupIndices(resultOrder);
}

///////////////////////////////////////////////////////////////////////////////

bool Search::hasName(const std::string& name) const
{
	std::string temp(name);
	util::String::tolower(temp);

	return (_name == temp);
}

///////////////////////////////////////////////////////////////////////////////

void Search::initGroupIndices(const std::string& resultOrder)
{
	_groupIndices.assign(0);
	for(std::string::size_type index = 0; index < resultOrder.length(); ++index)
	{
		_groupIndices[index] = util::String::fromString<int>(resultOrder.substr(index, 1));
	}
}

///////////////////////////////////////////////////////////////////////////////
