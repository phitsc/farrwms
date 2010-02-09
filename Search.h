#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>

#include "Util.h"

///////////////////////////////////////////////////////////////////////////////

class IniFile;

///////////////////////////////////////////////////////////////////////////////

class Search
{
public:
	Search(const std::string& name);

    typedef std::map<std::string, std::string> Parameters;

    Parameters& addSubsearch(const std::string& optionName, const std::string& abbreviation, long sortOrder);

	bool hasName(const std::string& name) const;
    bool hasSubsearch(const std::string& subSearchNameOrAbbreviation) const;

    const std::string& getName() const { return _name; }
    const std::string& getParameter(const std::string& subsearchName, const std::string& parameterName) const;
    const std::string getInfoAsHtml() const;

    typedef std::set<std::string> SubsearchNames;

    struct Subsearch
    {
        Subsearch(const std::string& name_, const std::string& abbreviation_, long sortOrder_) : name(name_), abbreviation(abbreviation_), sortOrder(sortOrder_)
        {}

        bool operator<(const Subsearch& subsearch) const
        {
            if((sortOrder != 0) && (subsearch.sortOrder != 0))
            {
                if(sortOrder == subsearch.sortOrder)
                {
                    return (name < subsearch.name);
                }
                else
                {
                    return (sortOrder < subsearch.sortOrder);
                }
            }
            else if((sortOrder != 0) && (subsearch.sortOrder == 0))
            {
                return true;
            }
            else if((sortOrder == 0) && (subsearch.sortOrder != 0))
            {
                return false;
            }
            else
            {
                return (name < subsearch.name);
            }
        }

        bool equalsName(const std::string& subsearchName) const
        {
            return (util::String::tolower_copy(name) == util::String::tolower_copy(subsearchName));
        }

        bool equalsNameOrAbbreviation(const std::string& subsearchNameOrAbbreviation) const
        {
            return ((util::String::tolower_copy(name) == util::String::tolower_copy(subsearchNameOrAbbreviation)) || (util::String::tolower_copy(abbreviation) == util::String::tolower_copy(subsearchNameOrAbbreviation)));
        }

        std::string name;
        std::string abbreviation;
        long        sortOrder;

        Parameters parameters;
    };

    typedef std::set<Subsearch> Subsearches;
    typedef Subsearches::const_iterator SubsearchesConstIterator;

    SubsearchesConstIterator subsearchesBegin() const { return _subsearches.begin(); }
    SubsearchesConstIterator subsearchesEnd() const { return _subsearches.end(); }

private:
    void getSubsearchInfoAsHtml(const Subsearch& subsearch, std::stringstream& stream) const;

    std::string _name;
    Subsearches _subsearches;
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

    enum
    {
        MaxContextMenuItemCount = 50,
        MaxStatusIconCount = 15
    };

private:
	void addSearch(const std::string& searchFile);
    static void addItemToSearch(Search& search, const IniFile& iniFile, const std::string& categoryName, const std::string& iconPath);
    static bool isValidRegularExpression(const std::string& searchName, const std::string& optionName, const std::string& pattern);
	static std::string findIconPath(const std::string& searchFile);

    static bool assignProperty(Search::Parameters& parameters, const std::string& parameterName, const IniFile& iniFile, const std::string& categoryName);
    static void assignIconProperty(Search::Parameters& parameters, const std::string& parameterName, const IniFile& iniFile, const std::string& categoryName, const std::string& searchIconPath);
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
