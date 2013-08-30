#include "ArgParserImpl.h"
#include "ArgParserTypes.h"
#include "ArgParser.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cxxabi.h>

using namespace CppArgParser::Private;

std::ostream& operator<<(std::ostream& os, const CppArgParser::Bool& v)
{
    os << v.m_b;
    return os;
}        

std::istream& operator>>(std::istream& is, CppArgParser::Bool& v)
{
    is >> v.m_b;
    return is;
}        

std::string CppArgParser::Private::demangle(const char* mangled)
{
    int status;
    char* p = abi::__cxa_demangle(mangled, 0, 0, &status);
    std::string ret(p);
    std::free(p);
    return ret;
}

namespace CppArgParser
{
    
    namespace Private
    {
        template<typename T>
        void ArgParserImpl::registerType()
        {
            m_convertSwitch.add(typeid(T), &CppArgParser::Types::Convert<T>::impl);
            m_decorateSwitch.add(typeid(T), &CppArgParser::Types::Decorate<T>::impl);
        }

        template<typename T>
        void ArgParserImpl::registerTypeMutations()
        {
            registerType<T>();
            registerType<std::vector<T>>();
        }
    };
};//namespace CppArgParser

ArgParserImpl::ArgParserImpl(Name desc)
:   m_name(),
    m_desc(desc),
    m_parameters(),
    m_convertSwitch(),
    m_decorateSwitch()
{
    // register each type that we handle
    registerTypeMutations<Bool>();
    registerTypeMutations<bool>();
    registerTypeMutations<char>();
    registerTypeMutations<unsigned char>();
    registerTypeMutations<short>();
    registerTypeMutations<unsigned short>();
    registerTypeMutations<int>();
    registerTypeMutations<unsigned int>();
    registerTypeMutations<long>();
    registerTypeMutations<unsigned long>();
    registerTypeMutations<long long>();
    registerTypeMutations<unsigned long long>();
    registerTypeMutations<size_t>();
    registerTypeMutations<std::string>();
    
    add("--help", &m_bHelp, typeid(bool), "show this help message");
}

void ArgParserImpl::add(Name name, void* valuePtr, std::type_index type, Name desc)
{
    m_parameters.push_back(Parameter(name, "", valuePtr, type, desc));
}

void ArgParserImpl::parse(int argc, char* argv[])
{
    Args args;
    for (int argn = 0; argn < argc; argn++)
    {
        args.push_back(argv[argn]);
    }
    
    m_name = args.front();
    m_name = m_name.substr(m_name.find_last_of("\\/") + 1);
    args.pop_front();
    
    parse_argsfirst(args);
    
    if (m_bHelp)
    {
        print_help();
    }
}

void ArgParserImpl::parse_argsfirst(Args args)
{
    // args first - less intuitive matching?
    Parameters::iterator paramIter = m_parameters.end();
    std::string arg;
    try
    {
        while (!args.empty())
        {
            arg = args.front();
            args.pop_front();
            
            // check for exact match of optional parameter
            paramIter = std::find_if(m_parameters.begin(), m_parameters.end(), [arg](const Parameter& param)
            {
                return arg == param.m_name;
            });
            if (paramIter != m_parameters.end())
            {
                m_convertSwitch(paramIter->m_type, *paramIter, args);
                continue;
            }
            
            // look for tokens that start with optional parameter and have = following
            paramIter = std::find_if(m_parameters.begin(), m_parameters.end(), [arg](const Parameter& param)
            {
                return arg.size() > param.m_name.size() 
                    && arg.substr(0, param.m_name.size()) == param.m_name
                    && arg[param.m_name.size()] == '=';
            });
            if (paramIter != m_parameters.end())
            {
                args.push_front(arg.substr(paramIter->m_name.size()));
                m_convertSwitch(paramIter->m_type, *paramIter, args);
                continue;
            }
            
            Types::throwUnknownParameter(arg);
        }
    }
    catch (boost::bad_lexical_cast& e)
    {
        if (paramIter != m_parameters.end())
            throwFailedConversion(*paramIter, arg);
    }
}

void ArgParserImpl::print_help()
{
    Parameters optional;
    Parameters required;
    std::cout << "Usage: " << m_name; 
    for (auto param: m_parameters)
    {
        if (param.m_name.size() && param.m_name[0] == '-')
        {
            optional.push_back(param);
        }
        else
        {
            std::cout << " <" << param.m_name << ">";
            required.push_back(param);
        }
    }        
    if (optional.size())
        std::cout << " [options]";
    std::cout << std::endl;
    std::cout << std::endl;
    
    if (m_desc.size())
    {
        std::cout << m_desc << std::endl;
        std::cout << std::endl;
    }

    if (required.size())
    {
        std::cout << "Required parameters:" << std::endl;
        for (auto param: required)
        {
            std::cout << "  " << param.m_name << ": " << param.m_desc << std::endl;
        }
    }

    if (optional.size())
    {
        std::cout << "Optional parameters:" << std::endl;
        int max = 0;
        for (auto param: optional)
        {
            // get the decorator (HACKY)
            std::string decorator;
            m_decorateSwitch(param.m_type, param, decorator);
            int cur = param.m_name.size() + 1 + decorator.size();
            if (max < cur)
                max = cur;
        }
        for (auto param: optional)
        {
            std::string decorator;
            m_decorateSwitch(param.m_type, param, decorator);
            decorator = param.m_name + " " + decorator;
            std::cout << "  " << std::left << std::setw(max + 8) << decorator << param.m_desc << std::endl;
        }
        std::cout << std::endl;
    }

    throw 0;
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
