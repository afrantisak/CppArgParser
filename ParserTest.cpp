#include "ArgParser.h"
#include "Test.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>

int main(int argc, char* argv[])
{
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

        CppArgParser::ArgParser args(argc, argv, "Test the CppArgParser");

        // configure single instance arguments
        args.param(b,    "--b",      "bool");
        args.param(c,    "--c",      "char");
        args.param(uc,   "--uc",     "unsigned char");
        args.param(s,    "--s",      "short");
        args.param(us,   "--us",     "unsigned short");
        args.param(n,    "--n",      "int");
        args.param(un,   "--un",     "unsigned int");
        args.param(l,    "--l",      "long");
        args.param(ul,   "--ul",     "unsigned long");
        args.param(ll,   "--ll",     "long long");
        args.param(ull,  "--ull",    "unsigned long long");
        args.param(size, "--size",   "size_t");
        args.param(str,  "--str",    "std::string");

        // configure multiple instance arguments
        args.param(b_m,    "--b_m",      "bool (multiple instances)");
        args.param(c_m,    "--c_m",      "char (multiple instances)");
        args.param(uc_m,   "--uc_m",     "unsigned char (multiple instances)");
        args.param(s_m,    "--s_m",      "short (multiple instances)");
        args.param(us_m,   "--us_m",     "unsigned short (multiple instances)");
        args.param(n_m,    "--n_m",      "int (multiple instances)");
        args.param(un_m,   "--un_m",     "unsigned int (multiple instances)");
        args.param(l_m,    "--l_m",      "long (multiple instances)");
        args.param(ul_m,   "--ul_m",     "unsigned long (multiple instances)");
        args.param(ll_m,   "--ll_m",     "long long (multiple instances)");
        args.param(ull_m,  "--ull_m",    "unsigned long long (multiple instances)");
        args.param(size_m, "--size_m",   "size_t (multiple instances)");
        args.param(str_m,  "--str_m",    "std::string (multiple instances)");
        
        // parse
        if (!args.valid())
        {
            return 1;
        };

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
    catch (std::runtime_error& e)
    {
        if (e.what())
            std::cerr << "ERROR: " << e.what();
        return 1;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what();
        std::cerr << "Unhandled exception!" << std::endl;
        return 1;
    }
    
    return 0;
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
