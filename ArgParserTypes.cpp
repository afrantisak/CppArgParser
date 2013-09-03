#include "ArgParserTypes.h"
#include "ArgParserImpl.h"

using namespace CppArgParser::Private;

void CppArgParser::Types::throwUnsupportedType(Parameter param)
{
    std::cout << "ERROR: ArgParser unsupported type ";
    std::cout << "(" << demangle(param.m_type.name()) << ") ";
    std::cout << "for parameter \"" << param.m_name << "\"" << std::endl;
    throw 1;
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
    std::cout << "Cvt debug " << param.m_name << " (" << demangle(param.m_type.name()) << "): ";
    while (!args.empty())
    {
        std::string arg = args.front();
        std::cout << arg << " ";
        args.pop_front();
    }
    std::cout << std::endl;
}

void CppArgParser::Types::Type<char>::convert(Parameter& param, Args& args)
{
    typedef char T;
    if (!args.size())
        throwRequiredMissing(param);
    if (param.m_set)
        throwMultipleNotAllowed(param);
    std::string value = args[0];
    args.pop_front();
    if (value[0] == '=')
    {
        value = value.substr(1);
    }
    if (value.size() > 1)
        throwFailedConversion(param, value);
    param.as<T>() = value[0];

}

void CppArgParser::Types::Type<unsigned char>::convert(Parameter& param, Args& args)
{
    typedef unsigned char T;
    if (!args.size())
        throwRequiredMissing(param);
    if (param.m_set)
        throwMultipleNotAllowed(param);
    std::string value = args[0];
    args.pop_front();
    if (value[0] == '=')
    {
        value = value.substr(1);
    }
    if (value.size() > 1)
        throwFailedConversion(param, value);
    param.as<T>() = value[0];
}

void CppArgParser::Types::Type<bool>::convert(Parameter& param, Args& args)
{
    typedef bool T;
    param.as<T>() = true;
    return;
}

void CppArgParser::Types::Type<CppArgParser::Bool>::convert(Parameter& param, Args& args)
{
    std::vector<std::string> trueValues, falseValues;
    trueValues.push_back("1");
    trueValues.push_back("T");
    trueValues.push_back("True");
    trueValues.push_back("Y");
    trueValues.push_back("Yes");
    falseValues.push_back("0");
    falseValues.push_back("F");
    falseValues.push_back("False");
    falseValues.push_back("N");
    falseValues.push_back("No");
    // bool requires the --arg=value syntax, otherwise if the flag is present, 
    // the value will be true.
    if (args.size() && args[0][0] == '=')
    {
        std::string value = args[0].substr(1);
        for (auto valid: trueValues)
        {
            if (value == valid)
            {
                typedef bool T;
                Bool b(true);
                param.as<T>() = b.m_b;
                args.pop_front();
                return;
            }
        }
        for (auto valid: falseValues)
        {
            if (value == valid)
            {
                typedef bool T;
                Bool b(false);
                param.as<T>() = b.m_b;
                args.pop_front();
                return;
            }
        }
        throwFailedConversion(param, value);
    }
    else
    {
        typedef bool T;
        Bool b(true);
        param.as<T>() = b.m_b;
        return;
    }
}

void CppArgParser::Types::Type<char>::decorate(Parameter& param, std::string& decorator)
{
    decorator = "arg";
}

void CppArgParser::Types::Type<unsigned char>::decorate(Parameter& param, std::string& decorator)
{
    decorator = "arg";
}

void CppArgParser::Types::Type<bool>::decorate(Parameter& param, std::string& decorator)
{
    decorator = "";
}

void CppArgParser::Types::Type<CppArgParser::Bool>::decorate(Parameter& param, std::string& decorator)
{
    decorator = "[=arg(=1)]";
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