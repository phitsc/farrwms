#include "FarrPlugin.h"
#include "JrPlugin_GenericShell.h"
#include "JrPlugin_MyPlugin.h"

#include <sstream>
#include <regex>

///////////////////////////////////////////////////////////////////////////////

FarrPlugin::FarrPlugin(const std::string& modulePath) :
    _iconPath(modulePath + "\\icons\\"),
    _helpFile(modulePath + "\\" + ThisPlugin_ReadMeFile)
{
    _xmlHttpRequest.CreateInstance(L"Msxml2.XMLHTTP.3.0");
}

///////////////////////////////////////////////////////////////////////////////

FarrPlugin::~FarrPlugin()
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

    if(!searchString.empty())
    {
        clearResults();

        setStatusText("searching...");

        DWORD start = GetTickCount();

        const std::string searchUrl = "http://social.msdn.microsoft.com/Search/en-GB?query=" + searchString;

        _xmlHttpRequest->open("GET", searchUrl.c_str(), false);
        _xmlHttpRequest->send();

        const std::string responseText = _xmlHttpRequest->responseText;

        std::string pattern("<a onmousedown=\".*\" href=\".*\">(.*)</a>\r\n<br/>\r\n<div class=\"ResultDescription\">(.*)</div>\r\n<div class=\"ResultUrl\">(.*)</div>");

        const std::tr1::cregex_iterator::regex_type regexType(pattern);

        std::tr1::cregex_iterator it(responseText.c_str(), responseText.c_str() + responseText.length(), regexType);
        std::tr1::cregex_iterator end;
        for( ; it != end; ++it)
        {
            const std::tr1::cmatch match = *it;
            const std::string heading = replaceNcrs(match[1]);
            const std::string group = replaceNcrs(match[2]);
            const std::string url = removeHttp(replaceNcrs(match[3]));

            _farrItems.push_back(FarrItem(heading, group, url, ""));
        }


        DWORD delta = GetTickCount() - start;

        std::stringstream stream;
        stream << "Found " << _farrItems.size() << " items in " << (delta / 1000) << " seconds.";
        setStatusText(stream.str());
    }
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

std::string FarrPlugin::replaceNcrs(const std::string& text)
{
    std::string temp(text);

    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#33;"), std::string("!"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#39;"), std::string("'"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#40;"), std::string("("));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#41;"), std::string(")"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#47;"), std::string("/"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#58;"), std::string(":"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#61;"), std::string("="));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#63;"), std::string("?"));
    temp = std::tr1::regex_replace(temp, std::tr1::regex("&#124;"), std::string("|"));

    return temp;
}

///////////////////////////////////////////////////////////////////////////////

std::string FarrPlugin::removeHttp(const std::string& url)
{
    std::string temp(url);

    temp = std::tr1::regex_replace(temp, std::tr1::regex("http://"), std::string(""));

    return temp;
}

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
