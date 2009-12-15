#include "Util.h"

#include <locale>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace util
{

///////////////////////////////////////////////////////////////////////////////

void String::UTF8Encode(const std::string& Text, std::string& Result)
{
    int InputChars = strlen(Text.c_str())+0;        // MSVCP strings tend to append extra nulls, so don't process them.

    // We need to first convert the ASCII input to Unicode before we can convert it to UTF-8...
    int UnicodeChars = MultiByteToWideChar(CP_ACP, 0, Text.c_str(), InputChars, 0, 0);
    LPWSTR UnicodeBuffer = (LPWSTR)alloca(UnicodeChars*sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, Text.c_str(), InputChars, UnicodeBuffer, UnicodeChars);

    // Now that we've got everything translated to Unicode, we can (finally) convert it to UTF-8.
    int UTF8Chars = WideCharToMultiByte(CP_UTF8, 0, UnicodeBuffer, UnicodeChars, 0, 0, 0, 0);
    LPSTR UTF8Buffer = (LPSTR)alloca(UTF8Chars);
    WideCharToMultiByte(CP_UTF8, 0, UnicodeBuffer, UnicodeChars, UTF8Buffer, UTF8Chars, 0, 0);

    // Store it in the std::string passed in.  Note that this string will be null terminated just like
    // any other C-style string, but it'll have the special encodings for multibyte characters.
    Result.assign(UTF8Buffer, UTF8Chars);
}

///////////////////////////////////////////////////////////////////////////////

void String::UTF8Decode(const std::string& UTF8, std::string& Result)
{
    const int InputBytes = strlen(UTF8.c_str())+0;        // MSVCP strings tend to append extra nulls, so don't process them.

    // Again, we need to convert the UTF-8 string to Unicode before we can convert it to 8-bit.
    int UnicodeChars = MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), InputBytes, 0, 0);
    LPWSTR UnicodeBuffer = (LPWSTR)alloca(UnicodeChars*sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), InputBytes, UnicodeBuffer, UnicodeChars);

    // Now that we've got everything translated to Unicode, we can convert it to 8-bit characters.
    int SingleByteChars = WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeChars, 0, 0, 0, 0);
    LPSTR SingleByteBuffer = (LPSTR)alloca(SingleByteChars);
    WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeChars, SingleByteBuffer, SingleByteChars, 0, 0);

    Result.assign(SingleByteBuffer, SingleByteChars);
}

///////////////////////////////////////////////////////////////////////////////

void String::tolower(std::string& toConvert)
{
    std::locale loc("");
    std::use_facet<std::ctype<char> >(loc).tolower(&toConvert[0], &toConvert[toConvert.length()]);
}

///////////////////////////////////////////////////////////////////////////////

bool String::equalsNoCase(const std::string& left, const std::string& right)
{
    return (_strcmpi(left.c_str(), right.c_str()) == 0);
}

///////////////////////////////////////////////////////////////////////////////

bool String::isAlpha(char character)
{
    std::locale loc("");
    return std::isalpha(character, loc);
}

///////////////////////////////////////////////////////////////////////////////

bool String::isAlphanumeric(char character)
{
    std::locale loc("");
    return std::isalnum(character, loc);
}

///////////////////////////////////////////////////////////////////////////////

bool String::isDigit(char character)
{
    std::locale loc("");
    return std::isdigit(character, loc);
}

///////////////////////////////////////////////////////////////////////////////

char String::getDecimalPoint()
{
    std::locale loc("");
    return std::use_facet<std::numpunct<char> >(loc).decimal_point();
}

///////////////////////////////////////////////////////////////////////////////

void String::removeTrailingSpaces(std::string& text)
{
    if(!text.empty() && (text[text.length() - 1] == ' '))
    {
        const std::string::size_type pos = text.find_last_not_of(' ');
        text.erase(pos + 1);
    }
}

///////////////////////////////////////////////////////////////////////////////

void String::removePreceedingSpaces(std::string& text)
{
    if(!text.empty() && (text[0] == ' '))
    {
        const std::string::size_type pos = text.find_first_not_of(' ');
        text.erase(0, pos);
    }
}

///////////////////////////////////////////////////////////////////////////////

void String::addTrailingSpace(std::string& text)
{
    if(!text.empty() && (text[text.length() - 1] != ' '))
    {
        text.push_back(L' ');
    }
}

///////////////////////////////////////////////////////////////////////////////

template<typename CollectionType>
void String::tokenize(const std::string& string, const std::string& delimiters, CollectionType& tokens)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = string.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = string.find_first_of(delimiters, lastPos);

    while(std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the collection
        tokens.push_back(string.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = string.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = string.find_first_of(delimiters, lastPos);
    }
}

///////////////////////////////////////////////////////////////////////////////

template<typename CollectionType>
void String::tokenize(const std::string& string, const char delimiter, CollectionType& tokens)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = string.find_first_not_of(delimiter, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = string.find_first_of(delimiter, lastPos);

    while(std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the collection
        tokens.push_back(string.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = string.find_first_not_of(delimiter, pos);
        // Find next "non-delimiter"
        pos = string.find_first_of(delimiter, lastPos);
    }
}

///////////////////////////////////////////////////////////////////////////////

String::StringPair String::split(const std::string& stringToSplit, char delimiter)
{
    const std::string::size_type pos = stringToSplit.find(delimiter);
    if(pos != std::string::npos)
    {
        return StringPair(stringToSplit.substr(0, pos), stringToSplit.substr(pos + 1));
    }
    else
    {
        return StringPair(stringToSplit, "");
    }
}

///////////////////////////////////////////////////////////////////////////////

bool String::containsSubstringNoCase(const std::string& value, const std::string& substring)
{
    std::string valueLower(value);
    tolower(valueLower);

    std::string substringLower(substring);
    tolower(substringLower);

    return (valueLower.find(substringLower) != std::string::npos);
}

///////////////////////////////////////////////////////////////////////////////

std::string String::getNumberWithOrdinal(const std::string& numberWithoutOrdinal)
{
    std::string result;

    if(!numberWithoutOrdinal.empty())
    {
        result = numberWithoutOrdinal;

        const long number = fromString<long>(numberWithoutOrdinal);
        if(number >= 21)
        {
            const long lastDigit = fromString<long>(numberWithoutOrdinal.substr(numberWithoutOrdinal.length() - 1));
            switch(lastDigit)
            {
            case 1:
                result += "st";
                break;

            case 2:
                result += "nd";
                break;

            case 3:
                result += "rd";
                break;

            default:
                result += "th";
                break;
            }
        }
        else if(number >= 0)
        {
            switch(number)
            {
            case 1:
                result += "st";
                break;

            case 2:
                result += "nd";
                break;

            case 3:
                result += "rd";
                break;

            default:
                result += "th";
                break;
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

std::string String::getUniqueID()
{
    UUID uniqueID;
    UuidCreate(&uniqueID);
    unsigned char* uniqueIDstring = 0;
    UuidToString(&uniqueID, &uniqueIDstring);
    const std::string result = (char*)uniqueIDstring;
    RpcStringFree(&uniqueIDstring);

    return result;
}

///////////////////////////////////////////////////////////////////////////////

std::string String::safeAssignCharPtr(const char* charPtr)
{
    if(charPtr != 0)
    {
        return std::string(charPtr);
    }
    else
    {
        return std::string();
    }
}

///////////////////////////////////////////////////////////////////////////////

std::string String::quoteSpaces(const std::string& text)
{
    std::string result(text);

    const std::string::size_type pos = text.find(' ');
    if(pos != std::string::npos)
    {
        result.insert(0, "\"");
        result.append("\"");
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////

void String::copyString(char* buffer, int maxLength, const std::string& string)
{
#pragma warning (push)
#pragma warning (disable : 4996)
    const std::string::size_type numberOfCharactersCopied = string.copy(buffer, maxLength - 1);
    buffer[numberOfCharactersCopied] = '\0';
#pragma warning (pop)
}

///////////////////////////////////////////////////////////////////////////////

void VersionInfo::initialize(const char* filename)
{
    typedef std::vector<std::string> Strings;

    DWORD handle;
    DWORD bufferSize = GetFileVersionInfoSize(filename, &handle);
    if(bufferSize > 0)
    {
        BYTE* buffer = new BYTE[bufferSize];
        BOOL ok = GetFileVersionInfo(filename, 0, bufferSize, buffer);
        if(ok)
        {
            char* info;
            UINT  len;

            // get file version
            ok = VerQueryValue(buffer, "\\StringFileInfo\\000004b0\\FileVersion", (void**)&info, &len);
            if(ok)
            {
                std::string fileVersion(info, len);
                Strings versionStrings;
                util::String::tokenize(fileVersion, ",.", versionStrings);

                if(versionStrings.size() > 0)
                {
                    std::stringstream stream;
                    stream << versionStrings[0];
                    stream >> _fileVersion.major;
                }

                if(versionStrings.size() > 1)
                {
                    std::stringstream stream;
                    stream << versionStrings[1];
                    stream >> _fileVersion.minor;
                }

                if(versionStrings.size() > 2)
                {
                    std::stringstream stream;
                    stream << versionStrings[2];
                    stream >> _fileVersion.service;
                }

                if(versionStrings.size() > 3)
                {
                    std::stringstream stream;
                    stream << versionStrings[3];
                    stream >> _fileVersion.build;
                }
            }

            // get special build
            ok = VerQueryValue(buffer, "\\StringFileInfo\\000004b0\\SpecialBuild", (void**)&info, &len);
            if(ok)
            {
                _specialBuild.assign(info, len - 1);
            }

            delete [] buffer;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
