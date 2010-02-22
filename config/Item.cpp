#include "Item.h"

#include <boost/algorithm/string.hpp>

namespace config
{
    void Item::insertParameter(const std::string& name, const std::string& value)
    {
        _parameters.insert(Parameter(name, boost::trim_copy(value)));
    }
}