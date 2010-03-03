#pragma once

#include <stdexcept>
#include <string>

namespace config
{

class ParseError : public std::runtime_error
{
public:
    ParseError(const std::string& message) :
      std::runtime_error(message)
    {}
};

}