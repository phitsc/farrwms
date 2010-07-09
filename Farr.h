#pragma once

#include <string>
#include <windows.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace farr
{
    void setShowAllMode();
    void showLocalHtmlFile(const std::string& filePath);
    void showHtml(const std::string& html);
    void setNewSearch(const std::string& searchText);
    void setStatusText(const std::string& statusText);
    void reportError(const std::string& errorText);
    void displayAlertMessage(const std::string& text);
    long getMaxResults();

    enum MenuType
    {
        ContextMenu,
        Statusbar
    };

    struct MenuItem
    {
        MenuItem(const std::string& type_, const std::string& caption_, const std::string& hint_, const std::string& iconPath_, const std::string& command_) :
            type(type_), caption(caption_), hint(hint_), iconPath(iconPath_), command(command_)
        {}

        std::string type;
        std::string caption;
        std::string hint;
        std::string iconPath;
        std::string command;
    };
    typedef std::vector<MenuItem> MenuItems;

    void addMenuItems(MenuType menuType, const MenuItems& menuItems);
    void clearMenuItems();

    bool signalSearchStateChanged(bool isSearching, unsigned long itemCount = 0);

    HWND getFarrMainWindow();
    std::string getPluginAlias();
    std::string getReadMeFileName();
    std::string getFarrDirectory();
    std::string resolveFile(const std::string& path);

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
