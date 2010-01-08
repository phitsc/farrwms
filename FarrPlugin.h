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

    const std::string& getAlias() const { return _farrAlias; }

private:
	Searches _searches;

    void listSearches(const std::string& filter);
    void listOptions(const std::string& searchName, const std::string& filter);
    void listCachedItems(const std::string& filter);
    void addSearchToResults(const Search& search, const std::string& filter);
    void addOptionToResults(const Search& search, const std::string& optionName, const std::string& filter);
    void addItemToResults(const FarrItem& farrItem, const std::string& filter);

    const Search* _currentSearch;
    std::string   _currentOptionName;
    std::string   _currentSearchTerm;
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

    FarrItems _farrItemCache;

    void clearResults();

    // some helper functions
    void signalSearchStateChanged(bool isSearching, FarrItems::size_type itemCount = 0);

    static void splitSearch(const std::string& searchString, std::string& searchName, std::string& optionName, std::string& searchTerm, bool& hasOption);
    static std::string replaceCharacterEntityReferences(const std::string& text);
    static std::string removeHttp(const std::string& url);
    static std::string replaceSubexpressions(const std::string& text, const std::tr1::cmatch& match);
    static std::string replaceVariable(const std::string& text, const std::string& variable, const std::string& value);
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
