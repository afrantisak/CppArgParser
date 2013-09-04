#pragma once
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>

namespace CppArgParser
{
    
    struct Bool
    {
        Bool() : m_b(false) {} // HACK?
        Bool(bool b) : m_b(b) {}
        bool m_b;
    };
    
    namespace Private
    {
        class ArgParserImpl;
    };
    
    class ArgParser
    {
    public:  
        typedef std::string Name;
        
        ArgParser(int argc, char* argv[]);
        ~ArgParser();
        
        template<typename T>
        void add(Name name, T& value, Name desc = Name())
        {
            addImpl(name, &value, typeid(T), desc);
        }

        bool help(Name description);
        
    private:
        std::unique_ptr<Private::ArgParserImpl> m_implPtr;
        
        void addImpl(Name name, void* valuePtr, std::type_index type, Name desc);
    };

};// namespace CppArgParser

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
