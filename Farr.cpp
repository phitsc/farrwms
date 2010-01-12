#include "Farr.h"
#include "JrPlugin_GenericShell.h"
#include "JrPlugin_MyPlugin.h"

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

void setNewSearch(const std::string& searchText)
{
    callbackfp_set_strval(hostptr, "setsearch", (char*)searchText.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void addStatusBarMenuItem(const std::string& caption, const std::string& hint, const std::string& launchCommand)
{
    const std::string menuStrings = "type=item|caption=" + caption + "|hint=" + hint + "|launch=" + launchCommand;
    callbackfp_set_strval(hostptr, "addmenu.statusbar", (char*)menuStrings.c_str());
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

}