#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////

class IniFile;

///////////////////////////////////////////////////////////////////////////////

class Search
{
public:
	Search(const std::string& name);

    typedef std::map<std::string, std::string> Parameters;

    Parameters& addOption(const std::string& optionName);

	bool hasName(const std::string& name) const;

    const std::string& getName() const { return _name; }
    const std::string& getParameter(const std::string& optionName, const std::string& parameterName) const;
    const std::string getInfoAsHtml() const;

    typedef std::set<std::string> OptionNames;

    typedef OptionNames::const_iterator OptionNamesConstIterator;

    OptionNamesConstIterator optionNamesBegin() const { return _optionNames.begin(); }
    OptionNamesConstIterator optionNamesEnd() const { return _optionNames.end(); }

private:
    void getOptionInfoAsHtml(const std::string& optionName, std::stringstream& stream) const;

    typedef std::map<std::string, Parameters> Options;

    std::string _name;
    Options     _options;
    OptionNames _optionNames;
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

    static void assignProperty(Search::Parameters& parameters, const std::string& parameterName, const std::string& value)
    {
        if(value != "__UNDEF")
        {
            parameters[parameterName] = value;
        }
    }

	SearchCollection _searches;
};

///////////////////////////////////////////////////////////////////////////////
