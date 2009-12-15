#pragma once

#include "msxml.h"

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////

class Search
{
public:
	Search(const std::string& name,
           const std::string& description,
           const std::string& searchUrl, 
           const std::string& resultPattern, 
           const std::string& farrCaption, 
           const std::string& farrGroup, 
           const std::string& farrPath, 
           const std::string& farrIconPath);

	bool hasName(const std::string& name) const;

	const std::string& getName() const { return _name; }
    const std::string& getDescription() const { return _description; }
	const std::string& getSearchUrl() const { return _searchUrl; }
	const std::string& getResultPattern() const { return _resultPattern; }
	const std::string& getFarrCaption() const { return _farrCaption; }
	const std::string& getFarrGroup() const { return _farrGroup; }
	const std::string& getFarrPath() const { return _farrPath; }
	const std::string& getFarrIconPath() const { return _farrIconPath; }

private:
	std::string _name;
    std::string _description;
	std::string _searchUrl;
	std::string _resultPattern;
	std::string _farrCaption;
	std::string _farrGroup;
	std::string _farrPath;
	std::string _farrIconPath;
};

///////////////////////////////////////////////////////////////////////////////

class Searches
{
public:
	typedef std::vector<Search> SearchCollection;
	typedef SearchCollection::const_iterator const_iterator;

	Searches(const std::string& searchesDirectory);
	~Searches();

	const_iterator begin() const { return _searches.begin(); }
	const_iterator end() const { return _searches.end(); }

private:
	void addSearch(const std::string& searchFile);
	static std::string findIconPath(const std::string& searchFile);

	typedef std::vector<Search> SearchCollection;
	SearchCollection _searches;

	MSXML2::IXMLDOMDocument2Ptr _document;
};

///////////////////////////////////////////////////////////////////////////////
