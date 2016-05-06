#include "ArgParser.h"
#include "Test.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        CppArgParser::ArgParser args(argc, argv, "Test the app name override", "Foo");

        if (!args.valid())
        {
            return 1;
        };
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

