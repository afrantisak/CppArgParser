#include "ArgParserImpl.h"
#include "ArgParserTypes.h"
#include "ArgParser.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cxxabi.h>

using namespace CppArgParser::Private;

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

ArgParserImpl::ArgParserImpl(int argc, char* argv[])
:   m_name(),
    m_desc(),
    m_parameters(),
    m_args()
{
    for (int argn = 0; argn < argc; argn++)
    {
        m_args.push_back(argv[argn]);
    }
    
    m_name = m_args.front();
    m_name = m_name.substr(m_name.find_last_of("\\/") + 1);
    m_args.pop_front();    

    m_parameters.push_back(Parameter("--help", "", &m_bHelp, typeid(bool), "show this help message", ""));
}

void ArgParserImpl::add(Name name, void* valuePtr, std::type_index type, Name desc, Name decorator)
{
    m_parameters.push_back(Parameter(name, "", valuePtr, type, desc, decorator));
}

bool ArgParserImpl::help(Name description)
{
    m_desc = description;
    for (auto& arg : m_args)
    {
        if (arg == "--help")
        {
            print_help();
            return true;
        }
    }

    return false;
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
            std::string decorator = param.m_decorator;
            int cur = param.m_name.size() + 1 + decorator.size();
            if (max < cur)
                max = cur;
        }
        for (auto param: optional)
        {
            std::string decorator = param.m_decorator;
            decorator = param.m_name + " " + decorator;
            std::cout << "  " << std::left << std::setw(max + 8) << decorator << param.m_desc << std::endl;
        }
        std::cout << std::endl;
    }
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
