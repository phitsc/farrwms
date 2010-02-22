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
    const std::string& getParameterValue(const std::string& name) const
    {
        const Parameters::const_iterator it = _parameters.find(name);
        const Parameter& parameter = *it;
        return parameter.getValue();
    }

protected:
    void insertParameter(const std::string& name, const std::string& value);

private:
    Parameters _parameters;
};

}