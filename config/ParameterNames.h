#pragma once

#include <set>
#include <string>

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
            const char* Expression  = "expression";
            const char* Caption     = "caption";
            const char* Group       = "group";
            const char* Path        = "path";
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
            _parameterNames.insert(param::Expression);
            _parameterNames.insert(param::Caption);
            _parameterNames.insert(param::Group);
            _parameterNames.insert(param::Path);
        }

        ParameterNameCollection _parameterNames;
    };
}