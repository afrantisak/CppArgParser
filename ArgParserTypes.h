#pragma once
#include "ArgParser.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <deque>
#include <boost/lexical_cast.hpp>

namespace CppArgParser
{
    
    namespace Types
    {

        typedef std::string Name;
            
        struct Parameter
        {
            Parameter(Name name, Name abbrev, void* valuePtr, std::type_index type, Name desc)
            :   m_name(name), m_abbrev(abbrev), m_valuePtr(valuePtr), m_type(type), m_desc(desc), m_set(false)
            {
            }
            
            Name m_name;
            Name m_abbrev;
            void* m_valuePtr;
            std::type_index m_type;
            Name m_desc;
            bool m_set;
            
            template<typename T>
            T& as()
            {
                m_set = true;
                return *(reinterpret_cast<T*>(m_valuePtr));
            }

        };
        
        typedef std::deque<std::string> Args;
                
        void throwUnsupportedType(Parameter param);
        void throwFailedConversion(Parameter param, std::string valueStr = std::string());
        void throwRequiredMissing(Parameter param);
        void throwMultipleNotAllowed(Parameter param);
        void throwUnknownParameter(std::string name);
        
        void debug(Parameter& param, Args args);
        
        template<typename T>
        T lexical_cast(const Parameter& param, std::string value)
        {
            try
            {
                //std::stringstream(value) >> t;
                return boost::lexical_cast<T>(value);
            }
            catch (boost::bad_lexical_cast& e)
            {
                throwFailedConversion(param, value);
            }
        }
        
        template<typename T>
        struct Convert
        {
            static void impl(Parameter& param, Args& args)
            {
                //debug(param, args);
                if (!args.size())
                    throwRequiredMissing(param);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                T t = lexical_cast<T>(param, value);
                param.as<T>() = t;
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
                Parameter paramFake = param;
                paramFake.m_type = typeid(T);
                T t = lexical_cast<T>(paramFake, value);
                param.as<std::vector<T>>().push_back(t);
            }
        };
        
        template<>
        struct Convert<char>
        {
            static void impl(Parameter& param, Args& args);
        };

        template<>
        struct Convert<unsigned char>
        {
            static void impl(Parameter& param, Args& args);
        };

        template<>
        struct Convert<bool>
        {
            static void impl(Parameter& param, Args& args);
        };

        template<>
        struct Convert<Bool>
        {
            static void impl(Parameter& param, Args& args);
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
            static void impl(Parameter& param, std::string& decorator);
        };

        template<>
        struct Decorate<Bool>
        {
            static void impl(Parameter& param, std::string& decorator);
        };

    };//namespace Types

};//namespace CppArgParser

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
