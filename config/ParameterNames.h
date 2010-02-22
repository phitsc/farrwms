#pragma once

#include <set>

namespace config
{
    namespace param
    {
        namespace
        {
            const char* LoadFirst   = "loadFirst";
            const char* Source      = "source";
            const char* Icon        = "icon";
            const char* Description = "description";
            const char* IsHidden    = "isHidden";
        }
    }

    class ParameterNames
    {
    public:
        static const ParameterNames& get()
        {
            static ParameterNames _this;
            return _this;
        }

        typedef std::set<std::string> ParameterNameCollection;
        typedef ParameterNameCollection::const_iterator const_iterator;

        const_iterator begin() const
        {
            return _parameterNames.begin();
        }

        const_iterator end() const
        {
            return _parameterNames.end();
        }

    private:
        ParameterNames()
        {
            _parameterNames.insert(param::LoadFirst);
            _parameterNames.insert(param::Source);
            _parameterNames.insert(param::Icon);
            _parameterNames.insert(param::Description);
            _parameterNames.insert(param::IsHidden);
        }

        ParameterNameCollection _parameterNames;
    };
}