#pragma once

#include "msxml.h"

#include <vector>
#include <string>
#include <array>

///////////////////////////////////////////////////////////////////////////////

class Search
{
public:
	Search(const std::string& name, const std::string& searchUrl, const std::string& resultPattern, const std::string& resultOrder, const std::string& iconPath);

	bool hasName(const std::string& name) const;

	const std::string& getName() const { return _name; }
	const std::string& getSearchUrl() const { return _searchUrl; }
	const std::string& getResultPattern() const { return _resultPattern; }
	const std::string& getIconPath() const { return _iconPath; }

	enum Group
	{
		Caption,
		Description,
		Url,

		GroupCount
	};

	int getGroupIndex(Group group) const { return _groupIndices[group]; }

private:
	std::string _name;
	std::string _searchUrl;
	std::string _resultPattern;
	std::string _iconPath;

	typedef std::tr1::array<int, GroupCount> GroupIndices;
	GroupIndices _groupIndices;

	void initGroupIndices(const std::string& resultOrder);
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
