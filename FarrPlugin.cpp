#include "FarrPlugin.h"
#include "JrPlugin_GenericShell.h"
#include "JrPlugin_MyPlugin.h"

#include <sstream>

///////////////////////////////////////////////////////////////////////////////

FarrPlugin::FarrPlugin(const std::string& modulePath) :
    _iconPath(modulePath + "\\icons\\"),
    _helpFile(modulePath + "\\" + ThisPlugin_ReadMeFile)
{
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::clearResults()
{
    _farrItems.clear();
}

///////////////////////////////////////////////////////////////////////////////

FarrItems::size_type FarrPlugin::getItemCount() const
{
    return _farrItems.size();
}

///////////////////////////////////////////////////////////////////////////////

const FarrItem& FarrPlugin::getItem(const FarrItems::size_type& index) const
{
    return _farrItems[index];
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::search(const char* rawSearchString)
{
    if(_farrAlias.empty())
    {
        _farrAlias = getPluginAlias() + " ";
    }

    //
    std::string searchString(rawSearchString);
    // remove rmilk alias
    searchString.erase(0, _farrAlias.length());

    DWORD start = GetTickCount();


    DWORD delta = GetTickCount() - start;

    std::stringstream stream;
    stream << "Found " << _farrItems.size() << " items in " << (delta / 1000) << " seconds.";
    setStatusText(stream.str());
}

///////////////////////////////////////////////////////////////////////////////

extern HINSTANCE dllInstanceHandle;

//void FarrPlugin::showAbout(const std::string& /*searchString*/)
//{
//    clearResults();
//
//    util::VersionInfo versionInfo(dllInstanceHandle);
//    const std::string version = versionInfo.getFileVersion().getAsString() + " - " + versionInfo.getSpecialBuild();
//    _farrItems.push_back(FarrItem(version, "Visit FarrMilk Web Site", "farrmilk.objecttechnology.com", _iconPath + "FarrMilk.ico", new CallTriggerFunctionCommand(this, &FarrPlugin::goweb, "http://farrmilk.objecttechnology.com")));
//
//    _farrItems.push_back(FarrItem("Philipp Tschannen (phitsc)", "Idea & Programming", "phitsc@gmail.com", _iconPath + "Philipp.ico", new CallTriggerFunctionCommand(this, &FarrPlugin::goweb, "mailto:phitsc@gmail.com")));
//
//    _farrItems.push_back(FarrItem("Carroll - (hamradio on donationcoder.com forums)", "Icon Design", "www.hamradiousa.net", _iconPath + "Hamradio.ico", new CallTriggerFunctionCommand(this, &FarrPlugin::goweb, "http://www.hamradiousa.net/")));
//
//    _farrItems.push_back(FarrItem("Donate", "Support this plugin", "www.donationcoder.com/Forums/bb/index.php?action=dlist;sa=search;search=149485;fields=uid", _iconPath + "Donate.ico", new CallTriggerFunctionCommand(this, &FarrPlugin::goweb, "http://www.donationcoder.com/Forums/bb/index.php?action=dlist;sa=search;search=149485;fields=uid")));
//}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::launchFile(const std::string& path)
{
    SHELLEXECUTEINFO info = { sizeof(SHELLEXECUTEINFO) };
    info.lpFile = path.c_str();
    ShellExecuteEx(&info);
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::setStatusText(const std::string& statusText)
{
    callbackfp_set_strval(hostptr, "statusbar", (TCHAR*)statusText.c_str());
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::setNewSearch(const std::string& searchText)
{
    callbackfp_set_strval(hostptr, "setsearch", (char*)searchText.c_str());
}

///////////////////////////////////////////////////////////////////////////////

HWND FarrPlugin::getFarrMainWindow()
{
    HWND mainFarrWindow = 0;
    callbackfp_get_strval(hostptr, "Handle.MainForm", (char*)&mainFarrWindow, sizeof(HWND));

    return mainFarrWindow;
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::setShowAllMode()
{
    callbackfp_set_strval(hostptr, "setshowallmode", "");
}

///////////////////////////////////////////////////////////////////////////////

void FarrPlugin::showHtmlFileInternal(const std::string& filePath)
{
    const std::string alias = "htmlviewurl file://" + filePath;
    callbackfp_set_strval(hostptr, "launch", (char*)alias.c_str());
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::getPluginAlias()
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
