#pragma once
#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace CppArgParser
{
    
    typedef std::string Name;
    typedef std::deque<std::string> Args;
        
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
    
    class failed_conversion {};
    class required_missing {};
    class too_many {};
    
    template<typename T>
    struct ParamTraits
    {
        ParamTraits() : m_count(0) {}
        void convert(std::string name, T& t, Args& args)
        {
            if (!args.size())
                throw required_missing();
            if (m_count)
                throw too_many();
            std::string value = args[0];
            args.pop_front();
            if (value[0] == '=')
            {
                value = value.substr(1);
            }
            t = lexical_cast<T>(value);
            m_count++;
        }

        std::string value_description()
        {
            return "arg";
        }
        
        private:
            int m_count;
    };
    
    template<typename T>
    struct ParamTraits<std::vector<T>>
    {
        void convert(std::string name, std::vector<T>& v, Args& args)
        {
            if (!args.size())
                throw required_missing();
            std::string value = args[0];
            args.pop_front();
            if (value[0] == '=')
            {
                value = value.substr(1);
            }
            T t = lexical_cast<T>(value);
            v.push_back(t);
        }
        
        std::string value_description()
        {
            return "arg";
        }
    };
    
    template<>
    struct ParamTraits<bool>
    {
        void convert(std::string name, bool& t, Args& args);
        std::string value_description()
        {
            return "";
        }
    };

    struct Bool
    {
        Bool() : m_b(false) {} // HACK?
        Bool(bool b) : m_b(b) {}
        bool m_b;
    };
    
    template<>
    struct ParamTraits<Bool>
    {
    public:
        ParamTraits();
        void convert(std::string name, Bool& t, Args& args);
        std::string value_description()
        {
            return "[=arg(=1)]";
        }
        
    private:
        std::vector<std::string> m_trueValues, m_falseValues;
    };

    struct Parameter
    {
        std::vector<Name> m_names;
        Name m_desc;
        Name m_decorator;
        
        std::string getName() const
        {
            std::string names;
            
            if (!m_names.size())
                return names;

            for (auto nameIter = m_names.begin(); nameIter != m_names.end() - 1; ++nameIter)
            {
                names += *nameIter + ", ";
            }

            return names + *m_names.rbegin();
        }
    };

    typedef std::vector<Parameter> Parameters;

    class ArgParser
    {
    public:
        // default name is taken from argv[0]
        ArgParser(int argc, char* argv[], Name app_name = std::string());
        
        template<typename T>
        void param(T& value, Name name, Name desc = Name());
        
        template<typename T>
        void param(T& value, std::vector<Name> names, Name desc = Name());
        
        template<typename T>
        T param(Name name, Name desc = Name());

        template<typename T>
        T param(std::vector<Name> names, Name desc = Name());

        bool help(Name description, std::ostream& os = std::cout);
        
        void print_help(Name app_name, Name app_description, std::ostream& os);

        bool fail_remaining();

    private:        
        Name m_app_name;
        Parameters m_parameters;
        Args m_args;
    };

    inline 
    ArgParser::ArgParser(int argc, char* argv[], Name app_name)
    :   m_app_name(app_name),
        m_parameters(),
        m_args()
    {
        for (int argn = 0; argn < argc; argn++)
        {
            m_args.push_back(argv[argn]);
        }
        
        if (!m_app_name.size())
        {
            m_app_name = m_args.front();
            m_app_name = m_app_name.substr(m_app_name.find_last_of("\\/") + 1);
        }
        m_args.pop_front();    
    }

    template<typename T>
    void ArgParser::param(T& value, Name name, Name desc)
    {
        std::vector<Name> names;
        names.push_back(name);
        param(value, names, desc);
    }

    template<typename T>
    void ArgParser::param(T& value, std::vector<Name> names, Name desc)
    {
        ParamTraits<T> type;
        Parameter param = { names, desc, type.value_description() };
        m_parameters.push_back(param);

        for (auto name : names)
        {
            Args args = m_args;
            size_t argCount = m_args.size();
            for (auto argIter = m_args.begin(); argIter != m_args.end(); ++argIter)
            {
                try
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
                catch (bad_lexical_cast&)
                {
                    std::stringstream strm;
                    strm << name << " failed conversion";
                    throw std::runtime_error(strm.str());
                }
                catch (failed_conversion&)
                {
                    std::stringstream strm;
                    strm << name << " failed conversion";
                    throw std::runtime_error(strm.str());
                }
                catch (required_missing&)
                {
                    std::stringstream strm;
                    strm << name << " is required";
                    throw std::runtime_error(strm.str());
                }
                catch (too_many&)
                {
                    std::stringstream strm;
                    if (names.size() > 1)
                    {
                        strm << "(" << param.getName() << ")";
                    }
                    else
                    {
                        strm << name;
                    }
                    strm << " does not allow multiple occurrences";
                    throw std::runtime_error(strm.str());
                }
            }
            m_args = args;
        }
    }

    template<typename T>
    T param(Name name, Name desc = Name())
    {
        T t;
        param(t, name, desc);
        return t;
    }

    template<typename T>
    T param(std::vector<Name> names, Name desc = Name())
    {
        T t;
        param(t, names, desc);
        return t;
    }

    inline
    bool ArgParser::help(Name description, std::ostream& os)
    {
        bool bHelp = false;
        param(bHelp, "--help", "show this help message");
        if (bHelp)
        {
            print_help(m_app_name, description, os);
            return true;
        }

        fail_remaining();

        return false;
    }

    inline
    void ArgParser::print_help(Name app_name, Name app_description, std::ostream& os)
    {
        Parameters optional;
        Parameters required;
        
        os << "Usage: " << app_name; 
        for (auto param: m_parameters)
        {
            auto name = param.getName();
            if (name.size() && name[0] == '-')
            {
                optional.push_back(param);
            }
            else
            {
                os << " <" << name << ">";
                required.push_back(param);
            }
        }        
        if (optional.size())
            os << " [options]";
        os << std::endl;
        os << std::endl;
        
        if (app_description.size())
        {
            os << app_description << std::endl;
            os << std::endl;
        }

        if (required.size())
        {
            os << "Required parameters:" << std::endl;
            for (auto param: required)
            {
                os << "  " << param.getName() << ": " << param.m_desc << std::endl;
            }
        }

        if (optional.size())
        {
            os << "Optional parameters:" << std::endl;
            int max = 0;
            for (auto param: optional)
            {
                // get the decorator (HACKY)
                std::string decorator = param.m_decorator;
                int cur = param.getName().size() + 1 + decorator.size();
                if (max < cur)
                    max = cur;
            }
            for (auto param: optional)
            {
                std::string decorator = param.m_decorator;
                decorator = param.getName() + " " + decorator;
                os << "  " << std::left << std::setw(max + 2) << decorator << param.m_desc << std::endl;
            }
            os << std::endl;
        }
    }

    inline
    bool ArgParser::fail_remaining()
    {
        for (auto& arg : m_args)
        {
            std::stringstream strm;
            strm << "ArgParser unknown name " << "\"" << arg << "\"";
            throw std::runtime_error(strm.str());
        }
    }

    inline
    void ParamTraits<bool>::convert(std::string name, bool& t, Args& args)
    {
        t = true;
        return;
    }

    inline
    ParamTraits<CppArgParser::Bool>::ParamTraits()
    {
        m_trueValues.push_back("1");
        m_trueValues.push_back("T");
        m_trueValues.push_back("True");
        m_trueValues.push_back("Y");
        m_trueValues.push_back("Yes");
        m_falseValues.push_back("0");
        m_falseValues.push_back("F");
        m_falseValues.push_back("False");
        m_falseValues.push_back("N");
        m_falseValues.push_back("No");
    }

    inline
    void ParamTraits<CppArgParser::Bool>::convert(std::string name, Bool& t, Args& args)
    {
        // bool requires the --arg=value syntax, otherwise if the flag is present, 
        // the value will be true.
        if (args.size() && args[0][0] == '=')
        {
            std::string value = args[0].substr(1);
            for (auto valid: m_trueValues)
            {
                if (value == valid)
                {
                    args.pop_front();
                    t = true;
                    return;
                }
            }
            for (auto valid: m_falseValues)
            {
                if (value == valid)
                {
                    args.pop_front();
                    t = false;
                    return;
                }
            }
            throw failed_conversion();
        }
        else
        {
            t = true;
            return;
        }
    }

};// namespace CppArgParser

inline
std::istream& operator>>(std::istream& is, CppArgParser::Bool& v)
{
    is >> v.m_b;
    return is;
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
