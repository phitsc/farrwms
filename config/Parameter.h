#pragma once

#include <string>

namespace config
{

class Parameter
{
public:
    Parameter(const std::string& name) :
        _name(name)
    {}

    Parameter(const std::string& name, const std::string& value) :
        _name(name), 
        _value(value)
    {}

    const std::string& getValue() const
    {
        return _value;
    }

    bool operator<(const Parameter& other) const
    {
        return (_name < other._name);
    }

private:
    std::string _name;
    std::string _value;
};

}