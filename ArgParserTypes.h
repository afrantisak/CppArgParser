#pragma once
#include <typeindex>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <deque>
#include <sstream>

#ifdef USE_BOOST_LEXICAL_CAST
#include <boost/lexical_cast.hpp>
#endif

namespace CppArgParser
{
    
    struct Bool
    {
        Bool() : m_b(false) {} // HACK?
        Bool(bool b) : m_b(b) {}
        bool m_b;
    };
    
    namespace Types
    {

        typedef std::string Name;
            
        struct Parameter
        {
            Parameter(Name name, Name abbrev, void* valuePtr, std::type_index type, Name desc, Name decorator)
            :   m_name(name), m_abbrev(abbrev), m_valuePtr(valuePtr), m_type(type), m_desc(desc), m_decorator(decorator), m_set(false)
            {
            }
            
            Name m_name;
            Name m_abbrev;
            void* m_valuePtr;
            std::type_index m_type;
            Name m_desc;
            Name m_decorator;
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
        std::string demangle(const char* mangled);
        
#ifndef USE_BOOST_LEXICAL_CAST
        template<typename T>
        T lexical_cast(const Parameter& param, std::string value)
        {
            try
            {
                std::istringstream strm(value);
                strm.exceptions(std::istringstream::failbit | std::istringstream::badbit);
                T t;
                strm >> t;
                if (!strm.eof())
                    throwFailedConversion(param, value);
                return t;
            }
            catch (std::istringstream::failure&)
            {
                throwFailedConversion(param, value);
            }
        }
#else
        template<typename T>
        T lexical_cast(const Parameter& param, std::string value)
        {
            try
            {
                return boost::lexical_cast<T>(value);
            }
            catch (boost::bad_lexical_cast& e)
            {
                throwFailedConversion(param, value);
            }
        }
#endif        
        template<>
        inline std::string lexical_cast<std::string>(const Parameter& param, std::string value)
        {
            return value;
        }
        
        template<>
        inline char lexical_cast<char>(const Parameter& param, std::string value)
        {
            if (value.size() != 1)
                throwFailedConversion(param, value);
            return value[0];
        }
        
        template<>
        inline unsigned char lexical_cast<unsigned char>(const Parameter& param, std::string value)
        {
            if (value.size() != 1)
                throwFailedConversion(param, value);
            return value[0];
        }
        
        template<typename T>
        struct Type
        {
            void convert(Parameter& param, Args& args)
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
                T t = lexical_cast<T>(param, value);
                param.as<T>() = t;
            }

            std::string decorate()
            {
                return "arg";
            }
        };
        
        template<typename T>
        struct Type<std::vector<T>>
        {
            void convert(Parameter& param, Args& args)
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
            
            std::string decorate()
            {
                return "arg";
            }
        };
        
        template<>
        struct Type<bool>
        {
            void convert(Parameter& param, Args& args);
            std::string decorate();
        };

        template<>
        struct Type<Bool>
        {
        public:
            Type();
            void convert(Parameter& param, Args& args);
            std::string decorate();
            
        private:
            std::vector<std::string> m_trueValues, m_falseValues;
        };

    };//namespace Types

};//namespace CppArgParser

std::istream& operator>>(std::istream& is, CppArgParser::Bool& v);
        
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
