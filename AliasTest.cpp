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

        // configure an aliased argument
        std::vector<std::string> aliases;
        aliases.push_back("--a1");
        aliases.push_back("--a2");
        aliases.push_back("-a");
        ArgParserType::N value = 0;
        args.param(value, aliases, "int (aliased)");
        
        // do the same thing but this time allow multiple instances
        std::vector<std::string> aliasesB;
        aliasesB.push_back("--b1");
        aliasesB.push_back("--b2");
        aliasesB.push_back("-b");
        std::vector<ArgParserType::N> values;
        args.param(values, aliasesB, "int (aliased vector)");

        // parse
        if (!args.valid())
        {
            return 1;
        };

        dump("alias:  ", value);
        dump("aliasB:  ", values);
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

