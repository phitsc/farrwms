#pragma once

#include "FarrItem.h"
#include "Search.h"
#include "msxml.h"
#include "XmlHttpEventSink.h"
#include "XmlHttpRequestEvents.h"

#include <string>
#include <regex>
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////

class FarrPlugin :
    private XmlHttpRequestEvents
{
public:
    FarrPlugin(const std::string& modulePath);
    ~FarrPlugin();

    void search(const char* rawSearchString);
    void stopSearch();
    bool isSearching() const;

    FarrItems::size_type getItemCount() const;
    const FarrItem& getItem(const FarrItems::size_type& index) const;

private:
	Searches _searches;

    void listSearches(const std::string& filter);
    void addSearchToResults(const Search& search, const std::string& filter);

    const Search* _currentSearch;
    bool _isSearching;

    MSXML2::IXMLHTTPRequestPtr _xmlHttpRequest;
    XmlHttpEventSink* _xmlHttpEventSink;

    // XmlHttpRequestEvents
    virtual void onHttpRequestResponse(const std::string& response);

    //
    std::string _farrAlias;
    std::string _iconPath;
    std::string _helpFile;

    // task vector for item retrieval by FARR 
    FarrItems _farrItems;

    void clearResults();

    // some helper functions
    void signalSearchStateChanged(bool isSearching, FarrItems::size_type itemCount = 0);

    static std::string replaceCharacterEntityReferences(const std::string& text);
    static std::string removeHttp(const std::string& url);
    static std::string replaceSubexpressions(const std::string& text, const std::tr1::cmatch& match);
	//static std::string fixLink(const std::string& link, const std::string& searchUrl);

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
