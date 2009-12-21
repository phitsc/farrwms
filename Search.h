#pragma once

#include <vector>
#include <string>
#include <map>

///////////////////////////////////////////////////////////////////////////////

class IniFile;

///////////////////////////////////////////////////////////////////////////////

class Search
{
public:
	Search(const std::string& name);

    void addItem(const std::string& optionName,
                 const std::string& description,
                 const std::string& searchUrl, 
                 const std::string& resultPattern, 
                 const std::string& farrCaption, 
                 const std::string& farrGroup, 
                 const std::string& farrPath, 
                 const std::string& farrIconPath);

	bool hasName(const std::string& name) const;

    const std::string& getName() const { return _name; }

    const std::string& getDescription(const std::string& optionName) const { return getItem(optionName, _descriptions); }
	const std::string& getSearchUrl(const std::string& optionName) const { return getItem(optionName, _searchUrls); }
	const std::string& getResultPattern(const std::string& optionName) const { return getItem(optionName, _resultPatterns); }
	const std::string& getFarrCaption(const std::string& optionName) const { return getItem(optionName, _farrCaptions); }
	const std::string& getFarrGroup(const std::string& optionName) const { return getItem(optionName, _farrGroups); }
	const std::string& getFarrPath(const std::string& optionName) const { return getItem(optionName, _farrPaths); }
	const std::string& getFarrIconPath(const std::string& optionName) const { return getItem(optionName, _farrIconPaths); }

private:
    typedef std::map<std::string, std::string> Strings;

    const std::string& getItem(const std::string& optionName, const Strings& collection) const
    {
        const Strings::const_iterator it = collection.find(optionName);
        if(it != collection.end())
        {
            return it->second;
        }
        else
        {
            static std::string empty;
            return empty;
        }
    }

    std::string _name;
    Strings _descriptions;
	Strings _searchUrls;
	Strings _resultPatterns;
	Strings _farrCaptions;
	Strings _farrGroups;
	Strings _farrPaths;
	Strings _farrIconPaths;
};

///////////////////////////////////////////////////////////////////////////////

class Searches
{
public:
	typedef std::vector<Search> SearchCollection;
	typedef SearchCollection::const_iterator const_iterator;

	Searches(const std::string& searchesDirectory);

	const_iterator begin() const { return _searches.begin(); }
	const_iterator end() const { return _searches.end(); }

private:
	void addSearch(const std::string& searchFile);
    static void addItemToSearch(Search& search, const IniFile& iniFile, const std::string& categoryName, const std::string& iconPath);
    static bool isValidRegularExpression(const std::string& searchName, const std::string& categoryName, const std::string& pattern);
	static std::string findIconPath(const std::string& searchFile);

	typedef std::vector<Search> SearchCollection;
	SearchCollection _searches;
};

///////////////////////////////////////////////////////////////////////////////
