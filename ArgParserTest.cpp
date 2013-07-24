#include "ArgParser.h"
#include <iostream>
#include <iomanip>
#include <string>

// shortcuts
struct ArgParserType
{
    typedef bool               B;
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

int main(int argc, char* argv[])
{
    try
    {
        // declare and set defaults
        ArgParserType::B    b    = 0;
        ArgParserType::C    c    = 0;
        ArgParserType::UC   uc   = 0;
        ArgParserType::S    s    = 0;
        ArgParserType::US   us   = 0;
        ArgParserType::N    n    = 0;
        ArgParserType::UN   un   = 0;
        ArgParserType::L    l    = 0;
        ArgParserType::UL   ul   = 0;
        ArgParserType::LL   ll   = 0;
        ArgParserType::ULL  ull  = 0;
        ArgParserType::Size size = 0;
        ArgParserType::Str  str  = "";

        // define arguments
        CppArgParser::ArgParser args("Test the CppArgParser");
        args.add("--b",      b,    "bool");
        args.add("--c",      c,    "char");
        args.add("--uc",     uc,   "unsigned char");
        args.add("--s",      s,    "short");
        args.add("--us",     us,   "unsigned short");
        args.add("--n",      n,    "int");
        args.add("--un",     un,   "unsigned int");
        args.add("--l",      l,    "long");
        args.add("--ul",     ul,   "unsigned long");
        args.add("--ll",     ll,   "long long");
        args.add("--ull",    ull,  "unsigned long long");
        args.add("--size",   size, "size_t");
        args.add("--str",    str,  "std::string");

        // parse
        args.parse(argc, argv);

        // output
        std::cout << "b:      " << b       << std::endl;
        std::cout << "c:      " << (int)c  << std::endl;
        std::cout << "uc:     " << (int)uc << std::endl;
        std::cout << "s:      " << s       << std::endl;
        std::cout << "us:     " << us      << std::endl;
        std::cout << "n:      " << n       << std::endl;
        std::cout << "un:     " << un      << std::endl;
        std::cout << "l:      " << l       << std::endl;
        std::cout << "ul:     " << ul      << std::endl;
        std::cout << "ll:     " << ll      << std::endl;
        std::cout << "ull:    " << ull     << std::endl;
        std::cout << "size:   " << size    << std::endl;
        std::cout << "str:    " << str     << std::endl;
    }
    catch (int n)
    {
        return n;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
        return 127;
    }

    return 0;
}
