#include "Farr.h"
#include "JrPlugin_GenericShell.h"
#include "JrPlugin_MyPlugin.h"
#include <Shlwapi.h>

///////////////////////////////////////////////////////////////////////////////

namespace farr
{

///////////////////////////////////////////////////////////////////////////////

void setShowAllMode()
{
    callbackfp_set_strval(hostptr, "setshowallmode", "");
}

///////////////////////////////////////////////////////////////////////////////

void showLocalHtmlFile(const std::string& filePath)
{
    const std::string alias = "htmlviewurl file://" + filePath;
    callbackfp_set_strval(hostptr, "launch", (char*)alias.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void showHtml(const std::string& html)
{
    const std::string alias = "showhtml " + html;
    callbackfp_set_strval(hostptr, "launch", (char*)alias.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void setNewSearch(const std::string& searchText)
{
    callbackfp_set_strval(hostptr, "setsearch", (char*)searchText.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void addMenuItems(MenuType menuType, const MenuItems& menuItems)
{
    std::string menuStrings;

    MenuItems::const_iterator it = menuItems.begin();
    const MenuItems::const_iterator end = menuItems.end();
    for( ; it != end; ++it)
    {
        const MenuItem& menuItem = *it;
        menuStrings += "type=" + menuItem.type + "|caption=" + menuItem.caption + "|icon=" + menuItem.iconPath + "|hint=" + menuItem.hint + "|launch=" + menuItem.command + "\n";
    }

    if(!menuStrings.empty())
    {
        callbackfp_set_strval(hostptr, (menuType == ContextMenu) ? "addmenu.contextmenu" : "addmenu.statusbar", (char*)menuStrings.c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////

void clearMenuItems()
{
    callbackfp_set_strval(hostptr, "clearmenus", "");
}

///////////////////////////////////////////////////////////////////////////////

long getMaxResults()
{
    const int BufferLength = 100;
    char buffer[BufferLength] = { 0 };
    callbackfp_get_strval(hostptr, "resolve:%MAXRESULTSDEF%", (char*)buffer, BufferLength);
    
    return atoi(buffer);
}

///////////////////////////////////////////////////////////////////////////////

void setStatusText(const std::string& statusText)
{
    callbackfp_set_strval(hostptr, "statusbar", (TCHAR*)statusText.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void reportError(const std::string& errorText)
{
    callbackfp_set_strval(hostptr, "reporterror", (TCHAR*)errorText.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void displayAlertMessage(const std::string& text)
{
    callbackfp_set_strval(hostptr, "DisplayAlertMessage", (TCHAR*)text.c_str());
}

///////////////////////////////////////////////////////////////////////////////

bool signalSearchStateChanged(bool isSearching, unsigned long itemCount)
{
    callbackfp_notifysearchstatechanged(hostptr, itemCount, isSearching ? E_SearchState_Searching : E_SearchState_Stopped);

    return isSearching;
}

///////////////////////////////////////////////////////////////////////////////

HWND getFarrMainWindow()
{
    HWND mainFarrWindow = 0;
    callbackfp_get_strval(hostptr, "Handle.MainForm", (char*)&mainFarrWindow, sizeof(HWND));

    return mainFarrWindow;
}

///////////////////////////////////////////////////////////////////////////////

std::string getPluginAlias()
{
    const int MaxLength = 100;
    char buffer[MaxLength] = { 0 };
    callbackfp_get_strval(hostptr, "plugin_alias", (char*)&buffer, MaxLength);

    const std::string pluginAlias(buffer);
    if(pluginAlias.empty())
    {
        return ThisPlugin_FARR_DefaultAlias;
    }
    else
    {
        return pluginAlias;
    }
}

///////////////////////////////////////////////////////////////////////////////

std::string getReadMeFileName()
{
    return ThisPlugin_ReadMeFile;
}

///////////////////////////////////////////////////////////////////////////////

std::string getFarrDirectory()
{
    char modulePath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, modulePath, MAX_PATH);
    PathRemoveFileSpec(modulePath);

    return modulePath;
}

///////////////////////////////////////////////////////////////////////////////

std::string resolveFile(const std::string& path)
{
    std::string tempPath = (path.find("..\\") == 0) ? (getFarrDirectory() + "\\" + path) : path;

    const int BufferLength = MAX_PATH;
    
    char buffer[BufferLength] = { 0 };
    callbackfp_get_strval(hostptr, std::string("resolvefile:" + tempPath).c_str(), (char*)buffer, BufferLength);

    char buffer2[BufferLength] = { 0 };
    if(PathCanonicalize(buffer2, buffer))
    {
        return std::string(buffer2);
    }
    else
    {
        return std::string(buffer);
    }
}

///////////////////////////////////////////////////////////////////////////////

}