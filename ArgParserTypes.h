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
            :   m_name(name), m_abbrev(abbrev), m_valuePtr(valuePtr), m_type(type), m_desc(desc), m_decorator(decorator)
            {
            }
            
            Name m_name;
            Name m_abbrev;
            void* m_valuePtr;
            std::type_index m_type;
            Name m_desc;
            Name m_decorator;
            
            template<typename T>
            T& as()
            {
                return *(reinterpret_cast<T*>(m_valuePtr));
            }

        };
        
        typedef std::deque<std::string> Args;
                
        void throwFailedConversion(std::string name);
        void throwRequiredMissing(std::string name);
        void throwMultipleNotAllowed(std::string name);
        void throwUnknownParameter(std::string name);
        
        std::string demangle(const char* mangled);
        
        class bad_lexical_cast {};
#ifndef USE_BOOST_LEXICAL_CAST
        template<typename T>
        T lexical_cast(std::string value)
        {
            try
            {
                std::istringstream strm(value);
                strm.exceptions(std::istringstream::failbit | std::istringstream::badbit);
                T t;
                strm >> t;
                if (!strm.eof())
                    throw bad_lexical_cast();
                return t;
            }
            catch (std::istringstream::failure&)
            {
                throw bad_lexical_cast();
            }
        }
#else
        template<typename T>
        T lexical_cast(std::string value)
        {
            try
            {
                return boost::lexical_cast<T>(value);
            }
            catch (boost::bad_lexical_cast& e)
            {
                throw bad_lexical_cast();
            }
        }
#endif        
        template<>
        inline std::string lexical_cast<std::string>(std::string value)
        {
            return value;
        }
        
        template<>
        inline char lexical_cast<char>(std::string value)
        {
            if (value.size() != 1)
                throw bad_lexical_cast();
            return value[0];
        }
        
        template<>
        inline unsigned char lexical_cast<unsigned char>(std::string value)
        {
            if (value.size() != 1)
                throw bad_lexical_cast();
            return value[0];
        }
        
        template<typename T>
        struct Type
        {
            Type() : m_count(0) {}
            void convert(Parameter& param, Args& args)
            {
                if (!args.size())
                    throwRequiredMissing(param.m_name);
                if (m_count)
                    throwMultipleNotAllowed(param.m_name);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                T t = lexical_cast<T>(value);
                param.as<T>() = t;
                m_count++;
            }

            std::string decorate()
            {
                return "arg";
            }
            
            private:
                int m_count;
        };
        
        template<typename T>
        struct Type<std::vector<T>>
        {
            void convert(Parameter& param, Args& args)
            {
                if (!args.size())
                    throwRequiredMissing(param.m_name);
                std::string value = args[0];
                args.pop_front();
                if (value[0] == '=')
                {
                    value = value.substr(1);
                }
                Parameter paramFake = param;
                paramFake.m_type = typeid(T);
                T t = lexical_cast<T>(value);
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
