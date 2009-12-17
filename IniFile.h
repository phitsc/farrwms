#pragma once

///////////////////////////////////////////////////////////////////////////////

#include "Util.h"

#include <fstream>
#include <map>
#include <string>

///////////////////////////////////////////////////////////////////////////////

struct IniParameterNotFoundException
{
};

///////////////////////////////////////////////////////////////////////////////

class IniFile
{
    typedef std::map<std::string, std::string> Parameters;

public:
    IniFile(const std::string& path)
    {
		 errno_t ret = fopen_s(&_file, path.c_str(), "r");
		 if(ret != 0)
		 {
			 _file = 0;
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

    const std::string& getParameterValue(const std::string& parameterName) const // throws IniParameterNotFoundException
    {
        const Parameters::const_iterator it = _parameters.find(parameterName);
        if(it != _parameters.end())
        {
            return it->second;
        }
        else
        {
            throw IniParameterNotFoundException();
        }
    }

private:
    void parseIniFile()
    {
        std::ifstream stream(_file);

        const int MAX_LINE_LENGTH = 512;
        char lineBuffer[MAX_LINE_LENGTH];

        while(!stream.eof())
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

                const std::string::size_type pos = line.find('=');
                if(pos != std::string::npos)
                {
                    const std::string parameterName = line.substr(0, pos);
                    const std::string parameterValue = line.substr(pos + 1);

                    _parameters.insert(std::make_pair(parameterName, parameterValue));
                }
            }
        }
    }

	FILE* _file;
    Parameters _parameters;
};

///////////////////////////////////////////////////////////////////////////////
