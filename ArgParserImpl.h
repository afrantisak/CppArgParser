#pragma once
#include <map>
#include <string>
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <boost/program_options.hpp>
#include "MapSwitch.h"

namespace CppArgParser
{
    
    namespace Private
    {

        class ArgParserImpl
        {
        public:  
            typedef std::string Name;
            
            ArgParserImpl(Name description);
            
            void add(Name name, void* valuePtr, std::type_index type, Name desc)
            {
                m_options.push_back(Option(name, "", valuePtr, type, desc));
            }

            // process the arguments and check for errors
            void parse(int argc, char* argv[]);
            
            struct Option
            {
                Option(Name name, Name abbrev, void* valuePtr, std::type_index type, Name desc)
                :   m_name(name), m_abbrev(abbrev), m_valuePtr(valuePtr), m_type(type), m_desc(desc)
                {
                }
                
                Name m_name;
                Name m_abbrev;
                void* m_valuePtr;
                std::type_index m_type;
                Name m_desc;
            };

        private:            
            const boost::program_options::variable_value& value(const Name& name) const;
            
            // if this is an optional argument (i.e. the name begins with "-" or "--")
            // then return the name WITHOUT the dashes.  If it is a required argument, return empty string.
            Name getOptional(const Name& name);
                
            void optionAdd(boost::program_options::options_description& desc, const Option& option, Name name);
            
            void optionConvert(Option& option, Name name);
            
            Name m_name;
            Name m_desc;
            
            typedef std::vector<Option> Options;
            Options m_options;
            
            boost::program_options::options_description m_po_visible;
            boost::program_options::options_description m_po_required;
            boost::program_options::options_description m_po_all;
            boost::program_options::positional_options_description m_po_positional;
            boost::program_options::variables_map m_po_map;
            
            typedef MapSwitch<std::type_index, const Option&, boost::program_options::options_description&, const std::string&> AddSwitch;
            AddSwitch m_addSwitch;
            
            typedef MapSwitch<std::type_index, Option&, const boost::program_options::variable_value&> ConvertSwitch;
            ConvertSwitch m_convertSwitch;

            template<typename T>
            void handleType();
        };

    };//namespace Private
    
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
