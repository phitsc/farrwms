#pragma once

#include "config/Search.h"

#include <set>

///////////////////////////////////////////////////////////////////////////////

class Searches
{
public:
    typedef std::set<config::Search> SearchCollection;
	typedef SearchCollection::const_iterator const_iterator;

	Searches(const std::string& searchesDirectory);

	const_iterator begin() const { return _searches.begin(); }
	const_iterator end() const { return _searches.end(); }

private:
	void addSearch(const std::string& searchFile);
 //   static void addItemToSearch(Search& search, const IniFile& iniFile, const std::string& categoryName, const std::string& iconPath);
 //   static bool isValidRegularExpression(const std::string& searchName, const std::string& optionName, const std::string& pattern);
	//static std::string findIconPath(const std::string& searchFile);

 //   static bool assignProperty(Search::Parameters& parameters, const std::string& parameterName, const IniFile& iniFile, const std::string& categoryName);
 //   static void assignIconProperty(Search::Parameters& parameters, const std::string& parameterName, const IniFile& iniFile, const std::string& categoryName, const std::string& searchIconPath);
 //   static bool assignProperty(Search::Parameters& parameters, const std::string& parameterName, const std::string& value)
 //   {
 //       if(value != "__UNDEF")
 //       {
 //           parameters[parameterName] = value;

 //           return true;
 //       }
 //       else
 //       {
 //           return false;
 //       }
 //   }

	SearchCollection _searches;
};

///////////////////////////////////////////////////////////////////////////////
