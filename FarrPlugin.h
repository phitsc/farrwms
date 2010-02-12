#pragma once

#include "FarrItem.h"
#include "Search.h"
#include "msxml.h"
#include "XmlHttpEventSink.h"
#include "XmlHttpRequestEvents.h"
#include "LogFile.h"

#include <string>
#include <map>
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

    void showSearchInfo(const std::string& searchName);

    const std::string& getAlias() const { return _farrAlias; }

private:
	Searches _searches;

    bool processCommand(const std::string& searchString);
    void sendRequest(const std::string& searchUrl);

    void listHelpItems();
    void listAboutItems();
    void listCommandItems();

    void listSearches(const std::string& filter);
    void listSubsearches(const std::string& searchName, const std::string& filter);
    void listCachedItems(const std::string& filter);
    void addSearchToResults(const Search& search, const std::string& filter);
    void addSubsearchToResults(const Search& search, const Search::Subsearch& subsearch, const std::string& filter);
    void addItemToResults(const FarrItem& farrItem, const std::string& filter);

    const Search* _currentSearch;
    std::string   _currentSubsearchName;
    std::string   _currentSearchTerm;
    bool _isSearching;

    LogFile _logFile;

    MSXML2::IXMLHTTPRequestPtr _xmlHttpRequest;
    XmlHttpEventSink* _xmlHttpEventSink;

    // XmlHttpRequestEvents
    virtual void onHttpRequestResponse(const std::string& response);
    virtual void onHttpRequestFailed(const std::string& statusText);

    //
    std::string _farrAlias;
    std::string _iconPath;
    std::string _searchesPath;
    std::string _helpFile;

    // task vector for item retrieval by FARR 
    FarrItems _farrItems;

    FarrItems _farrItemCache;

    void clearResults();

    typedef std::map<std::string, std::string> Variables;

    std::string getValue(const std::string& parameterName, const std::tr1::smatch& match, const Variables& variables);

    static void splitSearch(const std::string& searchString, std::string& searchName, std::string& optionName, std::string& searchTerm, bool& hasSubsearch);
    static std::string replaceCharacterEntityReferences(const std::string& text);
    static std::string removeHttp(const std::string& url);
    static void        replaceVariable(std::string& text, const Variables::value_type& variableAndValue);
    static std::string replaceVariables(const std::string& text, const Variables& variables);
	//static std::string fixLink(const std::string& link, const std::string& searchUrl);
};

///////////////////////////////////////////////////////////////////////////////
