#pragma once

#include <string>
#include <fstream>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

///////////////////////////////////////////////////////////////////////////////

class LogFile
{
public:
    bool enable()
    {
        if(!_file.is_open())
        {
            char logFilePath[MAX_PATH] = { 0 };
            if(S_OK == SHGetFolderPath(0, CSIDL_APPDATA, 0, SHGFP_TYPE_CURRENT, logFilePath))
            {
                PathAppend(logFilePath, "FarrWebMetaSearch");

                if(PathFileExists(logFilePath) == FALSE)
                {
                    CreateDirectory(logFilePath, 0);
                }

                PathAppend(logFilePath, "log.txt");
                _file.open(logFilePath);
                if(_file.is_open())
                {
                    _path = logFilePath;
                }
            }
        }

        return _file.is_open();
    }

    const std::string& getPath() const
    {
        return _path;
    }

    void disable()
    {
        _file.close();
    }

    void write(const std::string& message)
    {
        if(_file.is_open())
        {
            _file << message;
        }
    }

    void writeLine(const std::string& message)
    {
        if(_file.is_open())
        {
            write(message);
            _file << std::endl;
        }
    }

private:
    std::string   _path;
    std::ofstream _file;
};

///////////////////////////////////////////////////////////////////////////////
