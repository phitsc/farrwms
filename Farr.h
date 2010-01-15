#pragma once

#include <string>
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////

namespace farr
{
    void setShowAllMode();
    void showLocalHtmlFile(const std::string& filePath);
    void setNewSearch(const std::string& searchText);
    void setStatusText(const std::string& statusText);
    void reportError(const std::string& errorText);
    void displayAlertMessage(const std::string& text);
    void addStatusBarMenuItem(const std::string& caption, const std::string& hint, const std::string& icon, const std::string& command);
    long getMaxResults();

    bool signalSearchStateChanged(bool isSearching, unsigned long itemCount = 0);

    HWND getFarrMainWindow();
    std::string getPluginAlias();
    std::string getReadMeFileName();

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

}

///////////////////////////////////////////////////////////////////////////////
