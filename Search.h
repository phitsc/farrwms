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

    Parameters& addOption(const std::string& optionName, const std::string& abbreviation);

	bool hasName(const std::string& name) const;

    const std::string& getName() const { return _name; }
    const std::string& getParameter(const std::string& optionName, const std::string& parameterName) const;
    const std::string getInfoAsHtml() const;

    typedef std::set<std::string> OptionNames;

    struct Option
    {
        Option(const std::string& name_, const std::string& abbreviation_) : name(name_), abbreviation(abbreviation_)
        {}

        bool operator<(const Option& option) const
        {
            return (name < option.name);
        }

        bool equalsName(const std::string& optionName) const
        {
            return (name == optionName);
        }

        bool equalsNameOrAbbreviation(const std::string& optionNameOrAbbreviation) const
        {
            return ((name == optionNameOrAbbreviation) || (abbreviation == optionNameOrAbbreviation));
        }

        std::string name;
        std::string abbreviation;

        Parameters parameters;
    };

    typedef std::set<Option> Options;
    typedef Options::const_iterator OptionsConstIterator;

    OptionsConstIterator optionsBegin() const { return _options.begin(); }
    OptionsConstIterator optionsEnd() const { return _options.end(); }

private:
    void getOptionInfoAsHtml(const Option& option, std::stringstream& stream) const;

    std::string _name;
    Options     _options;
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
    static bool isValidRegularExpression(const std::string& searchName, const std::string& optionName, const std::string& pattern);
	static std::string findIconPath(const std::string& searchFile);

    static bool assignProperty(Search::Parameters& parameters, const std::string& parameterName, const std::string& value)
    {
        if(value != "__UNDEF")
        {
            parameters[parameterName] = value;

            return true;
        }
        else
        {
            return false;
        }
    }

	SearchCollection _searches;
};

///////////////////////////////////////////////////////////////////////////////
