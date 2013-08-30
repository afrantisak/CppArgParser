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
            void set(const T& v)
            {
                *(reinterpret_cast<T*>(m_valuePtr)) = v;
                m_set = true;
            }
            
            template<typename T>
            void get(T& v) const
            {
                v = *(reinterpret_cast<const T*>(m_valuePtr));
            }
            
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
        
        void debug(Parameter& param, Args args, std::string desc);
        
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

