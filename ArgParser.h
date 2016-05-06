#pragma once
#include <string>
#include <memory>
#include <deque>
#include <array>
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
    
    class required_missing {};
    class too_many {};
    class too_many_required_silent {};
    class not_enough {};
    class syntax_error {};
    
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

        void end()
        {
        }

        std::string value_description()
        {
            return "arg";
        }
        
        size_t expected()
        {
            return 1;
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
        
        void end()
        {
        }

        std::string value_description()
        {
            return "arg";
        }

        size_t expected()
        {
            return -1;
        }
    };
    
    template<typename T, size_t N>
    struct ParamTraits<std::array<T, N>>
    {
        ParamTraits()
            : m_count(0)
        {            
        }
        
        void convert(std::string name, std::array<T, N>& v, Args& args)
        {
            if (!args.size())
                throw required_missing();
            //std::cout << "m_count: " << m_count << std::endl;
            if (m_count == N)
                throw too_many_required_silent();
            std::string value = args[0];
            args.pop_front();
            if (value[0] == '=')
            {
                value = value.substr(1);
            }
            T t = lexical_cast<T>(value);
            v[m_count++] = t;
        }
        
        void end()
        {
            if (m_count != N)
            {
                throw not_enough();
            }
        }
        
        std::string value_description()
        {
            return "arg";
        }
        
        size_t expected()
        {
            return N;
        }
        
    private:
        int m_count;
    };
    
    template<>
    struct ParamTraits<bool>
    {
        void convert(std::string name, bool& t, Args& args)
        {
            t = true;
            return;
        }
        
        std::string value_description()
        {
            return "";
        }
        
        void end()
        {
        }

        size_t expected()
        {
            return 1;
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

        void convert(std::string name, Bool& t, Args& args)
        {
            // "Bool" allows the --arg=value syntax.  without it, the value will be true.
            // "bool" does not allow --arg=value
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
                throw bad_lexical_cast();
            }
            else
            {
                t = true;
                return;
            }
        }

        std::string value_description()
        {
            return "[=arg(=1)]";
        }

        void end()
        {
        }

        size_t expected()
        {
            return 1;
        }
        
    private:
        std::vector<std::string> m_trueValues, m_falseValues;
    };

    struct Parameter
    {
        std::vector<Name> m_names;
        Name m_desc;
        Name m_decorator;
        size_t m_expected;
        
        std::string getName() const
        {
            return getName(m_names);
        }
        
        static std::string getName(std::vector<Name> names)
        {
            std::string full;
            
            if (!names.size())
                return full;

            for (auto nameIter = names.begin(); nameIter != names.end() - 1; ++nameIter)
            {
                full += *nameIter + ", ";
            }

            return full + *names.rbegin();
        }
    };

    typedef std::vector<Parameter> Parameters;

    class ArgParser
    {
    public:
        // if you don't supply a name then it is taken from argv[0]
        ArgParser(int argc, char* argv[], 
                  Name app_description = std::string(), Name app_name = std::string(), 
                  std::ostream& os = std::cout);
        
        template<typename T>
        void param(T& value, Name name, Name desc = Name(), bool visible_in_help = true);
        
        template<typename T>
        void param(T& value, std::vector<Name> names, Name desc = Name(), bool visible_in_help = true);
        
        template<typename T>
        T param(Name name, Name desc = Name(), bool visible_in_help = true);

        template<typename T>
        T param(std::vector<Name> names, Name desc = Name(), bool visible_in_help = true);
        
        bool valid();
        
        void print_help(Name app_name, Name app_description, std::ostream& os);

    private:
        Name m_app_description;
        Name m_app_name;
        std::ostream& m_os;
        std::stringstream m_errors;
        Parameters m_parameters;
        Args m_args;
        bool m_help_requested;
        bool m_valid;
    };

    inline 
    ArgParser::ArgParser(int argc, char* argv[], Name app_description, Name app_name, std::ostream& os)
    :   m_app_description(app_description),
        m_app_name(app_name),
        m_os(os),
        m_errors(),
        m_parameters(),
        m_args(),
        m_help_requested(false),
        m_valid(true)
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
        
        param(m_help_requested, "--help", "show this help message", false);
    }

    template<typename T>
    void ArgParser::param(T& value, Name name, Name desc, bool visible_in_help)
    {
        std::vector<Name> names;
        names.push_back(name);
        param(value, names, desc, visible_in_help);
    }

    template<typename T>
    void ArgParser::param(T& value, std::vector<Name> names, Name desc, bool visible_in_help)
    {
        if (names.size() == 0) // TODO make sure all names are unique and non-empty
        {
            m_errors << "every parameter must have a unique name" << std::endl;
            m_valid = false;
        }        
        try
        {
            ParamTraits<T> type;
            if (visible_in_help)
            {
                Parameter param = { names, desc, type.value_description(), type.expected() };
                m_parameters.push_back(param);
            }

            for (auto name : names)
            {
                Args args = m_args;
                size_t argCount = m_args.size();
                bool done = false;
                for (auto argIter = m_args.begin(); argIter != m_args.end(); ++argIter)
                {
                    if (!m_valid || done) 
                        break;
                    std::string arg;
                    try
                    {
                        arg = *argIter;
                        //std::cout << "ARG: " << arg;
                        //std::cout << "  args:[";
                        //for (auto argIter2 = m_args.begin(); argIter2 != m_args.end(); ++argIter2)
                        //{
                        //    std::cout <<*argIter2 << ", ";
                        //}
                        //std::cout << "]" << std::endl;
                        if (arg == name)
                        {
                            // "--param value" for optional params
                            //   "param value" for required params (TODO: don't support this)
                            args.pop_front();
                            type.convert(name, value, args);
                        }
                        else if (arg.size() > name.size() 
                            && arg.substr(0, name.size()) == name
                            && arg[name.size()] == '=')
                        {
                            // "--opt-param=value" for optional params
                            //       "param=value" for required params (TODO: maybe keep this?)
                            args[0] = arg.substr(name.size());
                            type.convert(name, value, args);
                        }
                        else if (name.size() && name[0] != '-' 
                            && args.size() && args[0].size()
                            && args[0][0] != '-')
                        {
                            // "value" (for required params)
                            type.convert(name, value, args);
                        }
                    }
                    catch (bad_lexical_cast&)
                    {
                        m_errors << name << " failed conversion" << std::endl;
                        m_valid = false;
                    }
                    catch (required_missing&)
                    {
                        m_errors << name << " is required" << std::endl;
                        m_valid = false;
                    }
                    catch (too_many&)
                    {
                        m_errors << Parameter::getName(names) << ": too many instances" << std::endl;
                        m_valid = false;
                    }
                    catch (too_many_required_silent&)
                    {
                        if (names[0][0] == '-')
                        {
                            m_errors << Parameter::getName(names) << ": too many instances" << std::endl;
                            m_valid = false;
                        }
                        else
                        {
                            // give someone else a chance to look at this argument
                            //args.push_front(arg);
                            //std::cout << "BLAH" << std::endl;
                            m_args = args;
                            return;
                            done = true;
                            break;
                        }
                    }
                    catch (not_enough&)
                    {
                        m_errors << Parameter::getName(names) << ": not enough instances" << std::endl;
                        m_valid = false;
                    }
                    catch (syntax_error&)
                    {
                        m_errors << Parameter::getName(names) << ": syntax error" << std::endl;
                        m_valid = false;
                    }
                }
                m_args = args;
            }
            type.end();
        }
        catch (not_enough&)
        {
            m_errors << Parameter::getName(names) << ": not enough instances" << std::endl;
            m_valid = false;
        }
    }

    template<typename T>
    T ArgParser::param(Name name, Name desc, bool visible_in_help)
    {
        T t;
        param(t, name, desc, visible_in_help);
        return t;
    }

    template<typename T>
    T ArgParser::param(std::vector<Name> names, Name desc, bool visible_in_help)
    {
        T t;
        param(t, names, desc, visible_in_help);
        return t;
    }

    inline
    bool ArgParser::valid()
    {
        if (m_help_requested)
        {
            print_help(m_app_name, m_app_description, m_os);
            return false;
        }

        for (auto& arg : m_args)
        {
            m_errors << "ArgParser unknown name " << "\"" << arg << "\"" << std::endl;
            m_valid = false;
        }
        
        if (!m_valid)
        {
            // show only the first error for now
            std::string errors = m_errors.str();
            auto eol = errors.find('\n');
            if (eol != std::string::npos)
                errors = errors.substr(0, eol + 1);
            throw std::runtime_error(errors);
        }

        return m_valid;
    }

    inline
    void ArgParser::print_help(Name app_name, Name app_description, std::ostream& os)
    {
        Parameters optional;
        Parameters required;
        
        std::vector<std::string> aliases;
        aliases.push_back("--help");
        Parameter param = {aliases, "show this help message", ""};
        m_parameters.push_back(param);
        
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
                if (param.m_expected > 1)
                {
                    if (param.m_expected < 4)
                    {
                        for (int i = 1; i < param.m_expected; ++i)
                        {
                            os << " <" << name << ">";
                        }
                    }
                    else
                    {
                        os << "*";
                        if (param.m_expected == -1)
                            os << "n";
                        else
                            os << param.m_expected;
                    }
                }
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

};// namespace CppArgParser

inline
std::istream& operator>>(std::istream& is, CppArgParser::Bool& v)
{
    is >> v.m_b;
    return is;
}        
        
