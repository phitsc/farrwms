#pragma once

#include "Parameter.h"
#include "Item.h"

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace config
{

class Subsearch : public Item
{
public:
    Subsearch(const std::string& name, const std::string& abbreviation) :
        Item(name),
        _abbreviation(abbreviation)
    {}

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
        return (getName() < other.getName());
    }

    bool processConfigLine(const std::string& line, Item*& currentItem);

private:
    bool processParameterConfigLine(const std::string& line, Item* currentItem);
    bool processItemConfigLine(const std::string& line, Item*& currentItem);

    Item* addItem(const std::string& itemName);

    std::string _abbreviation;

    typedef std::tr1::shared_ptr<Item> ItemPtr;
    typedef std::vector<ItemPtr> Items;
    typedef std::map<std::string, Items> ItemsCollection;
    ItemsCollection _itemsCollection;
};

typedef std::tr1::shared_ptr<Subsearch> SubsearchPtr;

class Search : public Subsearch
{
    typedef std::set<SubsearchPtr> Subsearches;
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
        SubsearchPtr subsearch = *it;
        return *subsearch;
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
    static bool processEnd(Subsearch*& currentSubsearch, Item*& currentItem);
    static std::string extractName(const std::string& searchFile);
    static void throwError(const std::string& fileName, unsigned long lineNumber, const std::string& message);

private:
    Subsearches _subsearches;
};

}