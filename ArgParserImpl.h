#pragma once
#include "ArgParserTypes.h"
#include <map>
#include <string>
#include <vector>
#include <typeindex>
#include "MapSwitch.h"
#include <deque>

namespace CppArgParser
{
    
    namespace Private
    {
        
        typedef Types::Name Name;
        typedef Types::Args Args;
        typedef Types::Parameter Parameter;
        
        class ArgParserImpl
        {
        public:  
            ArgParserImpl(Name description);
            
            void add(Name name, void* valuePtr, std::type_index type, Name desc);

            void parse(int argc, char* argv[]);

            template<typename T>
            void registerType();
            
        protected:
            void parse_argsfirst(Args args);
            
            void print_help();

        private:
            template<typename T>
            void registerTypeMutations();
                
            Name m_name;
            Name m_desc;
            
            typedef std::vector<Parameter> Parameters;
            Parameters m_parameters;

            bool m_bHelp;

            MapSwitch<std::type_index, Parameter&, Args&> m_convertSwitch;
            MapSwitch<std::type_index, Parameter&, std::string&> m_decorateSwitch;
        };

        std::string demangle(const char* mangled);

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
