#pragma once

#include "Parameter.h"

#include <set>
#include <string>

namespace config
{

class Item
{
    typedef std::set<Parameter> Parameters;
public:
    Item(const std::string& name) :
        _name(name)
    {
    }

    const std::string& getName() const
    {
        return _name;
    }

    const std::string& getParameterValue(const std::string& name) const
    {
        const Parameters::const_iterator it = _parameters.find(name);
        if(it != _parameters.end())
        {
            const Parameter& parameter = *it;
            return parameter.getValue();
        }
        else
        {
            static std::string empty;
            return empty;
        }
    }

    bool operator==(const Item& item) const
    {
        return (getName() == item.getName());
    }

    void addParameter(const std::string& name, const std::string& value);

private:
    std::string _name;

    Parameters  _parameters;
};

}