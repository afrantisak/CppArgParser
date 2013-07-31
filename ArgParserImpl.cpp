#include "ArgParserImpl.h"
#include <algorithm>
#include <cxxabi.h>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/errors.hpp>

using namespace CppArgParser::Private;

namespace CppArgParser
{
    
    namespace Private
    {
        
        std::string demangle(const char* mangled)
        {
            int status;
            char* p = abi::__cxa_demangle(mangled, 0, 0, &status);
            std::string ret(p);
            std::free(p);
            return ret;
        }

        void throwUnsupportedType(Parameter param)
        {
            std::cout << "ERROR: ArgParser unsupported type ";
            std::cout << "(" << demangle(param.m_type.name()) << ") ";
            std::cout << "for parameter \"" << param.m_name << "\"" << std::endl;
            throw 1;
        }
        
        void throwFailedConversion(Parameter param, std::string valueStr = std::string())
        {
            std::cout << "ERROR: ArgParser failed conversion ";
            if (valueStr.size())
                std::cout << "from value \"" << valueStr << "\" ";
            std::cout << "to type \"" << demangle(param.m_type.name()) << "\" ";
            std::cout << "for parameter \"" << param.m_name << "\"" << std::endl;
            throw 1;
        }
        
        void throwRequiredMissing(Parameter param)
        {
            std::cout << "ERROR: " << param.m_name << " is required" << std::endl;
            throw 1;
        }
    
        void throwUnknownParameter(std::string name)
        {
            std::cout << "ERROR: ArgParser unknown name ";
            std::cout << "\"" << name << "\"" << std::endl;
            throw 1;
        }
        
    };//namespace Private
    
    namespace Types
    {

        template<typename T>
        struct Add
        {
            static void impl(const Parameter& param, BpoOptsDesc& desc, const std::string& name)
            {
                desc.add_options()(name.c_str(), Bpo::value<T>(), param.m_desc.c_str());    
            }
        };

        template<>
        struct Add<bool>
        {
            static void impl(const Parameter& param, BpoOptsDesc& desc, const std::string& name)
            {
                desc.add_options()(name.c_str(), param.m_desc.c_str());    
            }
        };
        #if 0
        template<typename T>
        struct Add<std::vector<T>>
        {
            static void impl(const Parameter& param, BpoOptsDesc& desc, const std::string& name)
            {
                desc.add_options()(name.c_str(), Bpo::value<std::vector<T>>(), param.m_desc.c_str());
            }
        };
        #endif
        template<typename T>
        struct Cvt
        {
            static void impl(const Parameter& param, const BpoVarValue& value)
            {
                if (!value.empty())
                {
                    *reinterpret_cast<T*>(param.m_valuePtr) = value.as<T>();
                }
            }
        };
        
        template<>
        struct Cvt<bool>
        {
            static void impl(const Parameter& param, const BpoVarValue& value)
            {
                if (!value.empty())
                {
                    if (value.as<std::string>().size())
                        *reinterpret_cast<bool*>(param.m_valuePtr) = boost::lexical_cast<bool>(value.as<std::string>());
                    else
                        *reinterpret_cast<bool*>(param.m_valuePtr) = true;
                }
            }
        };
        #if 0
        template<typename T>
        struct Cvt<std::vector<T>>
        {
            static void impl(const Parameter& param, const BpoVarValue& value)
            {
                if (!value.empty())
                {
                    *reinterpret_cast<std::vector<T>*>(param.m_valuePtr) = value.as<std::vector<T>>();
                }
            }
        };
        #endif
    };//namespace Types

};//namespace CppArgParser

template<typename T>
void ArgParserImpl::registerType()
{
    m_addSwitch.add(typeid(T), &CppArgParser::Types::Add<T>::impl);
    m_cvtSwitch.add(typeid(T), &CppArgParser::Types::Cvt<T>::impl);
    m_addSwitch.add(typeid(std::vector<T>), &CppArgParser::Types::Add<std::vector<T>>::impl);
    m_cvtSwitch.add(typeid(std::vector<T>), &CppArgParser::Types::Cvt<std::vector<T>>::impl);
}

ArgParserImpl::ArgParserImpl(Name desc)
:   m_name(),
    m_desc(desc),
    m_parameters(),
    m_po_visible("Optional parameters"),
    m_po_required("Required parameters"),
    m_po_all("All parameters"),
    m_po_positional(),
    m_po_map(),
    m_addSwitch(),
    m_cvtSwitch()
{
    // register each type that we handle
    registerType<bool>();
    registerType<char>();
    registerType<unsigned char>();
    registerType<short>();
    registerType<unsigned short>();
    registerType<int>();
    registerType<unsigned int>();
    registerType<long>();
    registerType<unsigned long>();
    registerType<long long>();
    registerType<unsigned long long>();
    registerType<size_t>();
    registerType<std::string>();
}

void ArgParserImpl::add(Name name, void* valuePtr, std::type_index type, Name desc)
{
    m_parameters.push_back(Parameter(name, "", valuePtr, type, desc));
}

void ArgParserImpl::parse(int argc, char* argv[])
{
    m_po_all.add_options()("help", "show this help message");
    m_po_visible.add_options()("help", "show this help message");
    
    m_name = argv[0];
    m_name = m_name.substr(m_name.find_last_of("\\/") + 1);
    
    // Declare the supported parameters
    for (auto param: m_parameters)
    {
        Name name = getOptional(param.m_name);
        if (name.size())
        {
            // add optional parameters
            try
            {
                m_addSwitch(param.m_type, param, m_po_all, name);
                m_addSwitch(param.m_type, param, m_po_visible, name);
            }
            catch (std::bad_function_call&)
            {
                throwUnsupportedType(param);
            }
        }
        else
        {
            // add required parameters
            m_po_all.add_options()(param.m_name.c_str(), param.m_desc.c_str());    
            m_po_required.add_options()(param.m_name.c_str(), param.m_desc.c_str());    
            m_po_positional.add(param.m_name.c_str(), 1);
        }
    }

    // process the command line
    try
    {
        Bpo::store(Bpo::command_line_parser(argc, argv).options(m_po_all).positional(m_po_positional).run(), m_po_map);
        Bpo::notify(m_po_map);    
    }
    catch (Bpo::validation_error& e)
    {
        auto paramIter = std::find_if(m_parameters.begin(), m_parameters.end(), [&](const Parameter& param)
        {
            return getOptional(param.m_name) == e.get_option_name();
        });
        throwFailedConversion(*paramIter);
    }
    catch (Bpo::unknown_option& e)
    {
        throwUnknownParameter(e.get_option_name());
    }
    
    // automatically handle --help
    if (m_po_map.count("help")) {
        std::cout << "Usage: " << m_name; 
        for (auto param: m_parameters)
        {
            Name name = getOptional(param.m_name);
            if (!name.size())
            {
                std::cout << " <" << param.m_name << ">";
            }
        }        
        if (m_po_visible.options().size())
            std::cout << " [options]";
        std::cout << std::endl;
        std::cout << std::endl;
        
        if (m_desc.size())
        {
            std::cout << m_desc << std::endl;
            std::cout << std::endl;
        }

        if (m_po_required.options().size())
            std::cout << m_po_required << std::endl;

        if (m_po_visible.options().size())
            std::cout << m_po_visible << std::endl;

        throw 0;
    }
    
    // check for required args
    for (auto param: m_parameters)
    {
        Name name = getOptional(param.m_name);
        if (!name.size())
        {
            if (m_po_map.count(param.m_name.c_str()) == 0)
            {
                throwRequiredMissing(param);
            }
        }
    }

    // do the conversions
    for (auto param: m_parameters)
    {
        try
        {
            Name name = getOptional(param.m_name);
            const BpoVarValue& value = m_po_map.operator[](name.c_str());
            if (!name.size())
                name = param.m_name;
            m_cvtSwitch(param.m_type, param, value);
        }
        catch (std::bad_function_call&)
        {
            throwUnsupportedType(param);
        }
    }
}

Name ArgParserImpl::getOptional(const Name& name)
{
    if (name.size() && name[0] == '-')
    {
        Name sNice(name.substr(1));
        if (sNice.size() && sNice[0] == '-')
            sNice = sNice.substr(1);
        return sNice;
    }
    
    return Name();
}

/*
Copyright (c) 2013 Aaron Frantisak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
