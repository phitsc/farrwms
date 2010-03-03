#pragma once

#include <set>
#include <string>

namespace config
{
    namespace item
    {
        namespace
        {
            const char* Result      = "result";
        }
    }

    class ItemNames
    {
    public:
        static const ItemNames& get()
        {
            static ItemNames _this;
            return _this;
        }

        typedef std::set<std::string> ItemNameCollection;
        typedef ItemNameCollection::const_iterator const_iterator;

        const_iterator begin() const
        {
            return _itemNames.begin();
        }

        const_iterator end() const
        {
            return _itemNames.end();
        }

    private:
        ItemNames()
        {
            _itemNames.insert(item::Result);
        }

        ItemNameCollection _itemNames;
    };
}