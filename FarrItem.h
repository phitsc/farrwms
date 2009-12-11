#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

struct FarrItem
{
    FarrItem(const std::string& caption_, const std::string& group_, const std::string& path_, const std::string& iconPath_)
        :caption(caption_),
         group(group_),
         path(path_),
         iconPath(iconPath_)
    {}

    bool operator<(const FarrItem& farrItem) const
    {
        return (caption < farrItem.caption);
    }

    std::string caption;
    std::string group;
    std::string path;
    std::string iconPath;
};

///////////////////////////////////////////////////////////////////////////////

typedef std::vector<FarrItem> FarrItems;

///////////////////////////////////////////////////////////////////////////////