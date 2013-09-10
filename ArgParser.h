#pragma once
#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <sstream>

namespace CppArgParser
{
    
    struct Bool
    {
        Bool() : m_b(false) {} // HACK?
        Bool(bool b) : m_b(b) {}
        bool m_b;
    };
    
    struct Parameter
    {
        typedef std::string Name;
        Parameter(Name name, Name abbrev, Name desc, Name decorator)
        :   m_name(name), m_abbrev(abbrev), m_desc(desc), m_decorator(decorator)
        {
        }
        
        Name m_name;
        Name m_abbrev;
        Name m_desc;
        Name m_decorator;
    };

    typedef std::deque<std::string> Args;
            
    void throwFailedConversion(std::string name);
    void throwRequiredMissing(std::string name);
    void throwMultipleNotAllowed(std::string name);
    void throwUnknownParameter(std::string name);
    
    std::string demangle(const char* mangled);
    
    class bad_lexical_cast {};

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
        void convert(std::string name, T& t, Args& args)
        {
            if (!args.size())
                throwRequiredMissing(name);
            if (m_count)
                throwMultipleNotAllowed(name);
            std::string value = args[0];
            args.pop_front();
            if (value[0] == '=')
            {
                value = value.substr(1);
            }
            try
            {
                t = lexical_cast<T>(value);
                m_count++;
            }
            catch (bad_lexical_cast& e)
            {
                throwFailedConversion(name);
            }
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
        void convert(std::string name, std::vector<T>& v, Args& args)
        {
            if (!args.size())
                throwRequiredMissing(name);
            std::string value = args[0];
            args.pop_front();
            if (value[0] == '=')
            {
                value = value.substr(1);
            }
            try
            {
                T t = lexical_cast<T>(value);
                v.push_back(t);
            }
            catch (bad_lexical_cast& e)
            {
                throwFailedConversion(name);
            }
        }
        
        std::string decorate()
        {
            return "arg";
        }
    };
    
    template<>
    struct Type<bool>
    {
        void convert(std::string name, bool& t, Args& args);
        std::string decorate();
    };

    template<>
    struct Type<Bool>
    {
    public:
        Type();
        void convert(std::string name, Bool& t, Args& args);
        std::string decorate();
        
    private:
        std::vector<std::string> m_trueValues, m_falseValues;
    };

    class ArgParser
    {
    public:  
        typedef std::string Name;
        
        ArgParser(int argc, char* argv[]);
        ~ArgParser();
        
        template<typename T>
        void param(Name name, T& value, Name desc = Name());
        
        bool fail_remaining();

        bool help(Name description);
        
    private:        
        void print_help(Name description);

        Name m_name;
        
        typedef std::vector<Parameter> Parameters;
        Parameters m_parameters;

        bool m_bHelp;

        typedef std::deque<std::string> Args;
        Args m_args;
    };

    template<typename T>
    void ArgParser::param(Name name, T& value, Name desc)
    {
        Type<T> type;
        Parameter param(name, "", desc, type.decorate());
        m_parameters.push_back(param);

        Args args = m_args;
        size_t argCount = m_args.size();
        for (auto argIter = m_args.begin(); argIter != m_args.end(); ++argIter)
        {
            std::string arg = *argIter;
            if (arg == name)
            {
                args.pop_front();
                type.convert(name, value, args);
            }
            else if (arg.size() > name.size() 
                && arg.substr(0, name.size()) == name
                && arg[name.size()] == '=')
            {
                args[0] = arg.substr(name.size());
                type.convert(name, value, args);
            }
        }
        m_args = args;
    }

};// namespace CppArgParser

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
