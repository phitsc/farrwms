#pragma once

#include <set>

namespace config
{

class Item
{
    typedef std::set<Parameter> Parameters;
public:
    const std::string& getParameterValue(const std::string& name) const
    {
        const Parameters::const_iterator it = _parameters.find(name);
        const Parameter& parameter = *it;
        return parameter.getValue();
    }

private:
    Parameters _parameters;
};

}