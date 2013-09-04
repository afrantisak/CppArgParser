#include "ArgParser.h"
#include "ArgParserImpl.h"

using namespace CppArgParser;

ArgParser::ArgParser(int argc, char* argv[])
    :   m_implPtr(new Private::ArgParserImpl(argc, argv))
{    
}

ArgParser::~ArgParser()
{
}

bool ArgParser::help(Name description)
{
    return m_implPtr->help(description);
}

void ArgParser::addImpl(Name name, void* valuePtr, std::type_index type, Name desc)
{
    // HACK
    std::string decorator;
    decorator = "arg";
    if (type == typeid(Bool))
        decorator = "[=arg(=1)]";
    m_implPtr->add(name, valuePtr, type, desc, decorator);
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
