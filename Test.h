#include "ArgParser.h"
#include <string>

// list of built-in types we support
struct ArgParserType
{
    typedef CppArgParser::Bool B;
    typedef char               C;
    typedef unsigned char      UC;
    typedef short              S;
    typedef unsigned short     US;
    typedef int                N;
    typedef unsigned int       UN;
    typedef long               L;
    typedef unsigned long      UL;
    typedef long long          LL;
    typedef unsigned long long ULL;
    typedef size_t             Size;
    typedef std::string        Str;
};

template<typename T>
void dump(std::string name, T t)
{
    std::cout << name << t << std::endl;
}

template<>
void dump(std::string name, CppArgParser::Bool t)
{
    std::cout << name << t.m_b << std::endl;
}

template<typename T>
void dump(std::string name, std::vector<T> t_m)
{
    std::cout << name;
    for (auto t: t_m)
        std::cout << t << ", ";
    std::cout << std::endl;
}

template<>
void dump(std::string name, std::vector<CppArgParser::Bool> t_m)
{
    std::cout << name;
    for (auto t: t_m)
        std::cout << t.m_b << ", ";
    std::cout << std::endl;
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
