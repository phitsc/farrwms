#pragma once

#include "FarrItem.h"

#include <string>
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////

class FarrPlugin
{
public:
    FarrPlugin(const std::string& modulePath);

    void search(const char* rawSearchString);

    FarrItems::size_type getItemCount() const;
    const FarrItem& getItem(const FarrItems::size_type& index) const;

private:
    void clearResults();

    //
    std::string _farrAlias;
    std::string _iconPath;
    std::string _helpFile;

    // task vector for item retrieval by FARR 
    FarrItems _farrItems;

    static void launchFile(const std::string& path);
    static void setStatusText(const std::string& statusText);
    static void setNewSearch(const std::string& searchText);
    static void setShowAllMode();
    static HWND getFarrMainWindow();
    static void showHtmlFileInternal(const std::string& filePath);
    static std::string getPluginAlias();

    class TemporaryStatusText
    {
    public:
        TemporaryStatusText(const std::string& statusText)
        {
            setStatusText(statusText);
        }

        ~TemporaryStatusText()
        {
            setStatusText("");
        }
    };
};

///////////////////////////////////////////////////////////////////////////////
