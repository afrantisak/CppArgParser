#include "ArgParser.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

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

int main(int argc, char* argv[])
{
  std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
  
  try
    {
        // declare single instance args
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
      
        // declare multiple instance args
        std::vector<ArgParserType::B>    b_m;
        std::vector<ArgParserType::C>    c_m;
        std::vector<ArgParserType::UC>   uc_m;
        std::vector<ArgParserType::S>    s_m;
        std::vector<ArgParserType::US>   us_m;
        std::vector<ArgParserType::N>    n_m;
        std::vector<ArgParserType::UN>   un_m;
        std::vector<ArgParserType::L>    l_m;
        std::vector<ArgParserType::UL>   ul_m;
        std::vector<ArgParserType::LL>   ll_m;
        std::vector<ArgParserType::ULL>  ull_m;
        std::vector<ArgParserType::Size> size_m;
        std::vector<ArgParserType::Str>  str_m;

        CppArgParser::ArgParser args("Test the CppArgParser");

        // configure single instance arguments
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

        // configure multiple instance arguments
        args.add("--b_m",      b_m,    "bool (multiple instances)");
        args.add("--c_m",      c_m,    "char (multiple instances)");
        args.add("--uc_m",     uc_m,   "unsigned char (multiple instances)");
        args.add("--s_m",      s_m,    "short (multiple instances)");
        args.add("--us_m",     us_m,   "unsigned short (multiple instances)");
        args.add("--n_m",      n_m,    "int (multiple instances)");
        args.add("--un_m",     un_m,   "unsigned int (multiple instances)");
        args.add("--l_m",      l_m,    "long (multiple instances)");
        args.add("--ul_m",     ul_m,   "unsigned long (multiple instances)");
        args.add("--ll_m",     ll_m,   "long long (multiple instances)");
        args.add("--ull_m",    ull_m,  "unsigned long long (multiple instances)");
        args.add("--size_m",   size_m, "size_t (multiple instances)");
        args.add("--str_m",    str_m,  "std::string (multiple instances)");

        // parse
        args.parse(argc, argv);

        // output
        dump("b:      ", b.m_b);
        dump("c:      ", (int)c);
        dump("uc:     ", (int)uc);
        dump("s:      ", s);
        dump("us:     ", us);
        dump("n:      ", n);
        dump("un:     ", un);
        dump("l:      ", l);
        dump("ul:     ", ul);
        dump("ll:     ", ll);
        dump("ull:    ", ull);
        dump("size:   ", size);
        dump("str:    ", str);

        dump("b_m:    ", b_m);
        dump("c_m:    ", c_m);
        dump("uc_m:   ", uc_m);
        dump("s_m:    ", s_m);
        dump("us_m:   ", us_m);
        dump("n_m:    ", n_m);
        dump("un_m:   ", un_m);
        dump("l_m:    ", l_m);
        dump("ul_m:   ", ul_m);
        dump("ll_m:   ", ll_m);
        dump("ull_m:  ", ull_m);
        dump("size_m: ", size_m);
        dump("str_m:  ", str_m);
    }
    catch (int n)
    {
        return n;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
        std::cerr << "Unhandled exception!" << std::endl;
        return 127;
    }
    
    return 0;
}
