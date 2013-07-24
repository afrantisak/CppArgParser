#pragma once
#include <string>
#include <boost/program_options.hpp>

namespace CppArgParser
{
    
    namespace Private
    {

        class ArgParserImpl
        {
        public:  
            typedef std::string Name;
            
            ArgParserImpl(Name description);
            
            void add(Name name, void* valuePtr, const std::type_info* typePtr, Name desc)
            {
                m_options.push_back(Option(name, "", valuePtr, typePtr, desc));
            }

            // process the arguments and check for errors
            void parse(int argc, char* argv[]);
            
        private:
            struct Option
            {
                Option(Name name, Name abbrev, void* valuePtr, const std::type_info* infoPtr, Name desc)
                :   m_name(name), m_abbrev(abbrev), m_valuePtr(valuePtr), m_infoPtr(infoPtr), m_desc(desc)
                {
                }
                
                Name m_name;
                Name m_abbrev;
                void* m_valuePtr;
                const std::type_info* m_infoPtr;
                Name m_desc;
            };
            
            template<typename T>
            friend bool optionAddImpl(const Option& option, 
                                      boost::program_options::options_description& desc, const Name& name);
                
            template<typename T>
            friend bool optionConvertImpl(const Option& option, 
                                          const boost::program_options::variable_value& value);

            const boost::program_options::variable_value& value(const Name& name) const;
            
            // if this is an optional argument (i.e. the name begins with "-" or "--")
            // then return the name WITHOUT the dashes.  If it is a required argument, return empty string.
            Name getOptional(const Name& name);
                
            void optionAdd(boost::program_options::options_description& desc, const Option& option, Name name);
            
            void optionConvert(const Option& option, Name name);
            
            Name m_name;
            Name m_desc;
            
            typedef std::vector<Option> Options;
            Options m_options;
            
            boost::program_options::options_description m_po_visible;
            boost::program_options::options_description m_po_required;
            boost::program_options::options_description m_po_all;
            boost::program_options::positional_options_description m_po_positional;
            boost::program_options::variables_map m_po_map;
        };

    };//namespace Private
    
};//namespace CppArgParser
