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

template<typename T, size_t N>
void dump(std::string name, std::array<T, N> t_m)
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

