#pragma once
#include <functional>
#include <unordered_map>

template<typename Key, typename... Args>
class MapSwitch
{
public:
    typedef std::function<void(const Args&... args)> Func;
    
    MapSwitch()
        :   m_impl(),
            m_default()
    {
    }
    
    void add(Key key, Func func)
    {
        m_impl.insert(std::make_pair(key, func));
    }

    void def(Func func)
    {
        m_default = func;
    }

    void operator()(Key key, const Args&... args)
    {
        auto iFunc = m_impl.find(key);
        if (iFunc != m_impl.end())
        {
            iFunc->second(args...);
        }
        else
        {
            m_default(args...);
        }
    }

private:
    typedef std::unordered_map<Key, Func> Impl;
    Impl m_impl;
    Func m_default;
};

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
