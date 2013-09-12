#include "ArgParser.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        CppArgParser::ArgParser args(argc, argv, "Foo");

        if (args.help("Test the app name override"))
        {
            return 1;
        };
    }
    catch (int n)
    {
        return n;
    }
    catch (std::runtime_error& e)
    {
        if (e.what())
            std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
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
