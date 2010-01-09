#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

struct FarrItem
{
    enum EntryType
    {
        Unknown = 0,
        File = 1,
        Alias = 3,
        Url = 4,
    };

    FarrItem(const std::string& caption_, const std::string& group_, const std::string& path_, const std::string& iconPath_, EntryType entryType_)
        :caption(caption_),
         group(group_),
         path(path_),
         iconPath(iconPath_),
         entryType(entryType_)
    {}

    bool operator<(const FarrItem& farrItem) const
    {
        return (caption < farrItem.caption);
    }

    std::string caption;
    std::string group;
    std::string path;
    std::string iconPath;
    EntryType   entryType;
};

///////////////////////////////////////////////////////////////////////////////

typedef std::vector<FarrItem> FarrItems;

///////////////////////////////////////////////////////////////////////////////