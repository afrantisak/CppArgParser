#include "ArgParserImpl.h"
#include "ArgParser.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cxxabi.h>
#include <boost/lexical_cast.hpp>

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
            //if (valueStr.size())
            //    std::cout << "from value \"" << valueStr << "\" ";
            std::cout << "to type \"" << demangle(param.m_type.name()) << "\" ";
            std::cout << "for parameter \"" << param.m_name << "\"" << std::endl;
            throw 1;
        }
        
        void throwRequiredMissing(Parameter param)
        {
            std::cout << "ERROR: " << param.m_name << " is required" << std::endl;
            throw 1;
        }
    
        void throwMultipleNotAllowed(Parameter param)
        {
            std::cout << "ERROR: " << param.m_name << " does not allow multiple occurrences" << std::endl;
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
        void debug(Parameter& param, Args args, std::string desc)
        {
            std::cout << "Cvt debug " << param.m_name << " (" << desc << "): ";
            while (!args.empty())
            {
                std::string arg = args.front();
                std::cout << arg << " ";
                args.pop_front();
            }
            std::cout << std::endl;
        }
        
        template<typename T>
        struct Convert
        {
            static void impl(Parameter& param, Args& args)
            {
                if (!args.size())
                    throwRequiredMissing(param);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                param.set(boost::lexical_cast<T>(value));
            }
        };
        
        template<typename T>
        struct Convert<std::vector<T>>
        {
            static void impl(Parameter& param, Args& args)
            {
                if (!args.size())
                    throwRequiredMissing(param);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                try
                {
                    T t = boost::lexical_cast<T>(value);
                    param.as<std::vector<T>>().push_back(t);
                }
                catch (boost::bad_lexical_cast& e)
                {
                    // fake out the thrower
                    Parameter paramFake = param;
                    paramFake.m_type = typeid(T);
                    throwFailedConversion(paramFake, value);
                }
            }
        };
        
        template<>
        struct Convert<char>
        {
            static void impl(Parameter& param, Args& args)
            {
                if (!args.size())
                    throwRequiredMissing(param);
                if (param.m_set)
                    throwMultipleNotAllowed(param);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                if (value.size() > 1)
                    throwFailedConversion(param, value);
                param.set(value[0]);
            }
        };

        template<>
        struct Convert<unsigned char>
        {
            static void impl(Parameter& param, Args& args)
            {
                if (!args.size())
                    throwRequiredMissing(param);
                if (param.m_set)
                    throwMultipleNotAllowed(param);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                if (value.size() > 1)
                    throwFailedConversion(param, value);
                param.set(value[0]);
            }
        };

        template<>
        struct Convert<bool>
        {
            static void impl(Parameter& param, Args& args)
            {
                param.set(true);
                return;
            }
        };

        template<>
        struct Convert<Bool>
        {
            static void impl(Parameter& param, Args& args)
            {
                std::vector<std::string> trueValues, falseValues;
                trueValues.push_back("1");
                trueValues.push_back("T");
                trueValues.push_back("True");
                trueValues.push_back("Y");
                trueValues.push_back("Yes");
                falseValues.push_back("0");
                falseValues.push_back("F");
                falseValues.push_back("False");
                falseValues.push_back("N");
                falseValues.push_back("No");
                // bool requires the --arg=value syntax, otherwise if the flag is present, 
                // the value will be true.
                if (args.size() && args[0][0] == '=')
                {
                    std::string value = args[0].substr(1);
                    for (auto valid: trueValues)
                    {
                        if (value == valid)
                        {
                            Bool b(true);
                            param.set(b);
                            args.pop_front();
                            return;
                        }
                    }
                    for (auto valid: falseValues)
                    {
                        if (value == valid)
                        {
                            Bool b(false);
                            param.set(b);
                            args.pop_front();
                            return;
                        }
                    }
                    throwFailedConversion(param, value);
                }
                else
                {
                    Bool b(true);
                    param.set(b);
                    return;
                }
            }
        };

        template<typename T>
        struct Decorate
        {
            static void impl(Parameter& param, std::string& decorator)
            {
                decorator = "arg";
            }
        };

        template<>
        struct Decorate<bool>
        {
            static void impl(Parameter& param, std::string& decorator)
            {
                decorator = "";
            }
        };

        template<>
        struct Decorate<Bool>
        {
            static void impl(Parameter& param, std::string& decorator)
            {
                decorator = "[=arg(=1)]";
            }
        };

    };//namespace Types

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
            
            throwUnknownParameter(arg);
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
        Name name = getOptional(param.m_name);
        if (!name.size())
        {
            std::cout << " <" << param.m_name << ">";
            required.push_back(param);
        }
        else
        {
            optional.push_back(param);
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
