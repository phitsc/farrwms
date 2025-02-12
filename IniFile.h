#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Util.h"

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////

struct IniParameterNotFoundException : std::invalid_argument
{
    IniParameterNotFoundException(const std::string& message) :
      std::invalid_argument(message)
    {}
};

///////////////////////////////////////////////////////////////////////////////

class IniFile
{
    typedef std::map<std::string, std::string> Parameters;
    typedef std::map<std::string, Parameters> ParametersCollection;

public:
    IniFile(const std::string& path) :
      _path(path)
    {
		 errno_t ret = fopen_s(&_file, path.c_str(), "r");
		 if(ret != 0)
		 {
             _file = 0;

             std::stringstream stream;
             stream << "Loading '" << path << "' failed.\n";

             OutputDebugString(stream.str().c_str());
		 }
         else
         {
             parseIniFile();
         }
    }

    ~IniFile()
    {
		if(isOpen())
		{
			fclose(_file);
		}
    }

    bool isOpen() const
    {
        return (_file != 0);
    }

    typedef std::set<std::string> SectionNames;
    typedef SectionNames::const_iterator ConstSectionIterator;
    ConstSectionIterator sectionsBegin() const { return _sectionNames.begin(); }
    ConstSectionIterator sectionsEnd() const { return _sectionNames.end(); }

    const std::string& getParameterValue(const std::string& sectionName, const std::string& parameterName) const // throws IniParameterNotFoundException
    {
        ParametersCollection::const_iterator it1 = _parametersCollection.find(sectionName);
        if(it1 != _parametersCollection.end())
        {
            const Parameters& parameters = it1->second;
            const Parameters::const_iterator it2 = parameters.find(parameterName);
            if(it2 != parameters.end())
            {
                return it2->second;
            }
        }

        std::stringstream stream;
        stream << "Parameter '" << parameterName << "' in section '" << sectionName << " in '" << _path << "' not found.\n";

        throw IniParameterNotFoundException(stream.str());
    }

    const std::string& getParameterValue(const std::string& sectionName, const std::string& parameterName, const std::string& defaultValue) const
    {
        ParametersCollection::const_iterator it1 = _parametersCollection.find(sectionName);
        if(it1 != _parametersCollection.end())
        {
            const Parameters& parameters = it1->second;
            const Parameters::const_iterator it2 = parameters.find(parameterName);
            if(it2 != parameters.end())
            {
                return it2->second;
            }
        }

        return defaultValue;
    }

private:
    void parseIniFile()
    {
        std::ifstream stream(_file);

        const int MAX_LINE_LENGTH = 4096;
        char lineBuffer[MAX_LINE_LENGTH];

        std::string sectionName;
        _sectionNames.insert(sectionName);

        while(!stream.eof() && !stream.fail())
        {
            stream.getline(lineBuffer, MAX_LINE_LENGTH);
            std::string line = lineBuffer;
            util::String::removePreceedingSpaces(line);
            if(!line.empty())
            {
                if(line[0] == ';')
                {
                    continue;
                }
                else if(line[0] == '[')
                {
                    const std::string::size_type pos = line.find(']', 1);
                    sectionName = line.substr(1, (pos != std::string::npos) ? (pos - 1) : std::string::npos);
                    util::String::tolower(sectionName);
                    _sectionNames.insert(sectionName);

                    continue;
                }

                const std::string::size_type pos = line.find('=');
                if(pos != std::string::npos)
                {
                    const std::string parameterName = line.substr(0, pos);
                    const std::string parameterValue = line.substr(pos + 1);

                    _parametersCollection[sectionName].insert(std::make_pair(parameterName, parameterValue));
                }
            }
        }
    }

	FILE* _file;
    ParametersCollection _parametersCollection;
    SectionNames _sectionNames;
    const std::string _path;

    void operator=(const IniFile&);
};

///////////////////////////////////////////////////////////////////////////////
