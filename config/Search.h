#pragma once

#include "Parameter.h"
#include "Item.h"

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

namespace config
{

class Subsearch : public Item
{
public:
    Subsearch(const std::string& name, const std::string& abbreviation) :
      _name(name),
      _abbreviation(abbreviation)
    {}

    const std::string& getName() const
    {
        return _name;
    }

    const std::string& getAbbreviation() const
    {
        return _abbreviation;
    }

    bool hasName(const std::string& name) const
    {
        return (getName() == name);
    }

    bool operator<(const Subsearch& other) const
    {
        return (_name < other._name);
    }

private:
    std::string _name;
    std::string _abbreviation;

    typedef std::tr1::shared_ptr<Item> ItemPtr;
    typedef std::vector<ItemPtr> Items;
    Items _items;
};

class Search : public Subsearch
{
    typedef std::set<Subsearch> Subsearches;
public:
    Search(const std::string& searchFile);

    bool hasSubsearch(const std::string& name) const
    {
        using namespace std::tr1::placeholders;
        return (std::find_if(_subsearches.begin(), _subsearches.end(), std::tr1::bind(&Subsearch::hasName, _1, name)) != _subsearches.end());
    }

    const Subsearch& getSubsearch(const std::string& name) const
    {
        using namespace std::tr1::placeholders;
        const Subsearches::const_iterator it = std::find_if(_subsearches.begin(), _subsearches.end(), std::tr1::bind(&Subsearch::hasName, _1, name));
        return *it;
    }

    bool operator<(const Search& search) const
    {
        return (getName() < search.getName());
    }

    typedef Subsearches::const_iterator const_iterator;
    const_iterator subsearchesBegin() const
    {
        return _subsearches.begin();
    }

    const_iterator subsearchesEnd() const
    {
        return _subsearches.end();
    }

private:
    static std::string extractName(const std::string& searchFile);

private:
    Subsearches _subsearches;
};

}