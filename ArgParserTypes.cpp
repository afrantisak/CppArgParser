#include "ArgParserTypes.h"
#include <cxxabi.h>

std::string CppArgParser::Types::demangle(const char* mangled)
{
    int status;
    char* p = abi::__cxa_demangle(mangled, 0, 0, &status);
    std::string ret(p);
    std::free(p);
    return ret;
}

void CppArgParser::Types::throwFailedConversion(Parameter param, std::string valueStr)
{
    std::cout << "ERROR: ArgParser failed conversion ";
    //if (valueStr.size())
    //    std::cout << "from value \"" << valueStr << "\" ";
    std::cout << "to type \"" << demangle(param.m_type.name()) << "\" ";
    std::cout << "for parameter \"" << param.m_name << "\"" << std::endl;
    throw 1;
}

void CppArgParser::Types::throwRequiredMissing(Parameter param)
{
    std::cout << "ERROR: " << param.m_name << " is required" << std::endl;
    throw 1;
}

void CppArgParser::Types::throwMultipleNotAllowed(Parameter param)
{
    std::cout << "ERROR: " << param.m_name << " does not allow multiple occurrences" << std::endl;
    throw 1;
}

void CppArgParser::Types::throwUnknownParameter(std::string name)
{
    std::cout << "ERROR: ArgParser unknown name ";
    std::cout << "\"" << name << "\"" << std::endl;
    throw 1;
}

void CppArgParser::Types::debug(Parameter& param, Args args)
{
    std::cout << "Cvt debug " << param.m_name << " (" << demangle(param.m_type.name()) << ") [" << args.size() << "]: ";
    while (!args.empty())
    {
        std::string arg = args.front();
        std::cout << "\"" << arg << "\" ";
        args.pop_front();
    }
    std::cout << std::endl;
}

void CppArgParser::Types::Type<bool>::convert(Parameter& param, Args& args)
{
    param.as<bool>() = true;
    return;
}

CppArgParser::Types::Type<CppArgParser::Bool>::Type()
{
    m_trueValues.push_back("1");
    m_trueValues.push_back("T");
    m_trueValues.push_back("True");
    m_trueValues.push_back("Y");
    m_trueValues.push_back("Yes");
    m_falseValues.push_back("0");
    m_falseValues.push_back("F");
    m_falseValues.push_back("False");
    m_falseValues.push_back("N");
    m_falseValues.push_back("No");
}

void CppArgParser::Types::Type<CppArgParser::Bool>::convert(Parameter& param, Args& args)
{
    // bool requires the --arg=value syntax, otherwise if the flag is present, 
    // the value will be true.
    if (args.size() && args[0][0] == '=')
    {
        std::string value = args[0].substr(1);
        for (auto valid: m_trueValues)
        {
            if (value == valid)
            {
                args.pop_front();
                param.as<bool>() = true;
                return;
            }
        }
        for (auto valid: m_falseValues)
        {
            if (value == valid)
            {
                args.pop_front();
                param.as<bool>() = false;
                return;
            }
        }
        throwFailedConversion(param, value);
    }
    else
    {
        param.as<bool>() = true;
        return;
    }
}

std::string CppArgParser::Types::Type<bool>::decorate()
{
    return "";
}

std::string CppArgParser::Types::Type<CppArgParser::Bool>::decorate()
{
    return "[=arg(=1)]";
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
