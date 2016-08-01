#pragma once
#include <string>
// #include <tchar>
#include <tchar.h>
namespace ms{
    typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> >
        tstring;
}