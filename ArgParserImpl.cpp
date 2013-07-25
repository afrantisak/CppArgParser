#include "ArgParserImpl.h"
#include <typeinfo>
#include <boost/lexical_cast.hpp>

namespace po = boost::program_options;
using namespace CppArgParser::Private;

namespace CppArgParser
{
    
    namespace Private
    {
    
        template<typename T>
        void optionAddImpl(const ArgParserImpl::Option& option, po::options_description& desc, const std::string& name)
        {
            desc.add_options()(name.c_str(), po::value<T>(), option.m_desc.c_str());    
        }

        template<>
        void optionAddImpl<bool>(const ArgParserImpl::Option& option, po::options_description& desc, const std::string& name)
        {
            desc.add_options()(name.c_str(), option.m_desc.c_str());    
        }

        void optionAddUnsupported(const ArgParserImpl::Option& option, po::options_description& desc, const std::string& name)
        {
            std::cout << "ERROR: ArgParser unsupported type ";
            std::cout << "(" << option.m_infoPtr->name() << ") ";
            std::cout << "for option \"" << option.m_name << "\"" << std::endl;
            throw 1;
        }

        template<typename T>
        void optionConvertImpl(const ArgParserImpl::Option& option, const po::variable_value& value)
        {
            if (!value.empty())
            {
                *reinterpret_cast<T*>(option.m_valuePtr) = value.as<T>();
            }
        }
        
        template<>
        void optionConvertImpl<bool>(const ArgParserImpl::Option& option, const po::variable_value& value)
        {
            if (!value.empty())
            {
                if (value.as<std::string>().size())
                    *reinterpret_cast<bool*>(option.m_valuePtr) = boost::lexical_cast<bool>(value.as<std::string>());
                else
                    *reinterpret_cast<bool*>(option.m_valuePtr) = true;
            }
        }
        
        void optionConvertUnsupported(const ArgParserImpl::Option& option, const po::variable_value& value)
        {
            std::cout << "ERROR: ArgParser unsupported conversion ";
            std::cout << "(" << option.m_infoPtr->name() << ") ";
            std::cout << "for option \"" << option.m_name << "\"" << std::endl;
            throw 1;
        }
        
        template<typename T>
        void ArgParserImpl::handleType()
        {
            m_addSwitch.add(&typeid(T), &CppArgParser::Private::optionAddImpl<T>);
            m_convertSwitch.add(&typeid(T), &CppArgParser::Private::optionConvertImpl<T>);
        }

    };//namespace Private

};//namespace CppArgParser

ArgParserImpl::ArgParserImpl(Name desc)
:   m_name(),
    m_desc(desc),
    m_options(),
    m_po_visible("Allowed options"),
    m_po_required("Required parameters"),
    m_po_all("All options"),
    m_po_positional(),
    m_po_map(),
    m_addSwitch(),
    m_convertSwitch()
{
    handleType<bool>();
    handleType<char>();
    handleType<unsigned char>();
    handleType<short>();
    handleType<unsigned short>();
    handleType<int>();
    handleType<unsigned int>();
    handleType<long>();
    handleType<unsigned long>();
    handleType<long long>();
    handleType<unsigned long long>();
    handleType<size_t>();
    handleType<std::string>();
    m_addSwitch.def(&Private::optionAddUnsupported);
    m_convertSwitch.def(&Private::optionConvertUnsupported);
}

void ArgParserImpl::optionAdd(boost::program_options::options_description& desc, const Option& option, Name name)
{
    m_addSwitch(option.m_infoPtr, option, desc, name);
}

void ArgParserImpl::optionConvert(Option& option, Name name)
{
    m_convertSwitch(option.m_infoPtr, option, value(name));
}

void ArgParserImpl::parse(int argc, char* argv[])
{
    m_po_all.add_options()("help", "show this help message");
    m_po_visible.add_options()("help", "show this help message");
    
    m_name = argv[0];
    
    // Declare the supported options.
    for (auto option: m_options)
    {
        Name name = getOptional(option.m_name);
        if (name.size())
        {
            optionAdd(m_po_all, option, name);
            optionAdd(m_po_visible, option, name);
        }
        else
        {
            m_po_all.add_options()(option.m_name.c_str(), option.m_desc.c_str());    
            m_po_required.add_options()(option.m_name.c_str(), option.m_desc.c_str());    
            m_po_positional.add(option.m_name.c_str(), 1);
        }
    }
    
    po::store(po::command_line_parser(argc, argv).options(m_po_all).positional(m_po_positional).run(), m_po_map);
    po::notify(m_po_map);    
    
    if (m_po_map.count("help")) {
        std::cout << "Usage: " << m_name; 
        for (auto option: m_options)
        {
            Name name = getOptional(option.m_name);
            if (name.size())
            {
            }
            else
            {
                std::cout << " <" << option.m_name << ">";
            }
        }
        
        std::cout << " [options]" << std::endl;
        if (m_desc.size())
            std::cout << m_desc << std::endl;
        std::cout << std::endl;

        std::cout << m_po_required << std::endl;

        std::cout << m_po_visible << std::endl;
        throw 0;
    }
    
    // check for required args
    for (auto option: m_options)
    {
        Name name = getOptional(option.m_name);
        if (name.size())
        {
        }
        else
        {
            if (m_po_map.count(option.m_name.c_str()) == 0)
            {
                std::cout << "ERROR: " << option.m_name << " is required" << std::endl;
                throw 1;
            }
        }
    }

    // do the conversions
    for (auto option: m_options)
    {
        Name name = getOptional(option.m_name);
        try
        {
            if (name.size() == 0)
                name = option.m_name;
            optionConvert(option, name);
        }
        catch (boost::bad_any_cast)
        {
            std::cout << "ERROR: ArgParser failed conversion ";
            std::cout << "from value \"" << value(name).as<std::string>() << "\" ";
            std::cout << "to type \"" << option.m_infoPtr->name() << "\" ";
            std::cout << "for option \"" << option.m_name << "\"" << std::endl;
            throw 1;
        }
    }
}

const po::variable_value& ArgParserImpl::value(const Name& name) const 
{
    return m_po_map.operator[](name.c_str());
}
    
ArgParserImpl::Name ArgParserImpl::getOptional(const Name& name)
{
    // TODO: convert spaces/underscores to dashes
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
