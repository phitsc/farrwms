#include "Searches.h"
#include "FileList.h"
#include "Util.h"
#include "config/ParseError.h"

#include <algorithm>
#include <functional> 
#include <regex>

using namespace std::tr1::placeholders; 

///////////////////////////////////////////////////////////////////////////////

Searches::Searches(const std::string& searchesDirectory)
{
	util::FileList searchesFiles(searchesDirectory, "*.conf", util::FileList::Files);

	std::for_each(searchesFiles.begin(), searchesFiles.end(), std::tr1::bind(&Searches::addSearch, this, _1));
}

///////////////////////////////////////////////////////////////////////////////

void Searches::addSearch(const std::string& searchFile)
{
    try
    {
        config::Search search(searchFile);
        _searches.insert(search);
    }
    catch(config::ParseError& /*error*/)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
