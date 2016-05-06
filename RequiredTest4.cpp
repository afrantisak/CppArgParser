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
        CppArgParser::ArgParser args(argc, argv, "Test the CppArgParser");

        std::array<int, 4> values4;
        args.param(values4, "a", "four ints");
        
        std::vector<int> valuesN;
        args.param(valuesN, "b", "N ints");

        // parse
        if (!args.valid())
        {
            return 1;
        };

        dump("values4:  ", values4);
        dump("valuesN:  ", valuesN);
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

