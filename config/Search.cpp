#include "Search.h"
#include "Keywords.h"

#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>

namespace config
{

Search::Search(const std::string& searchFile) :
    Subsearch(extractName(searchFile), "")
{
    std::ifstream file(searchFile.c_str());
    if(file.is_open())
    {
        Subsearch* search = this;

        std::string line;
        while(std::getline(file, line))
        {
            boost::trim(line);
            if(line.empty() || boost::starts_with(line, ";"))
            {
                // skip empty lines and comments
                continue;
            }

            if(boost::starts_with(line, keyword::Subsearch))
            {
                std::tr1::smatch matches;
                if(std::tr1::regex_match(line, matches, std::tr1::regex(keyword::Subsearch + std::string("\\s+(\\w+)\\s*(\\(\\w+\\))?"))))
                {
                    _subsearches.insert(Subsearch(matches.str(1), matches.str(2)));
                }
            }
        }
    }
}

std::string Search::extractName(const std::string& searchFile)
{
    const std::string::size_type posSlash = searchFile.find_last_of("/\\");
    const std::string::size_type posPoint = searchFile.find_last_of(".");

    assert(posPoint != std::string::npos);

    if(posSlash != std::string::npos)
    {
        return searchFile.substr(posSlash + 1, posPoint - posSlash - 1);
    }
    else
    {
        return searchFile.substr(0, posPoint);
    }
}

}