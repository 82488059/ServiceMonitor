#pragma once
#include <tchar.h>
#include <stdlib.h>
#include <ms/tstring.h>

//stdlib.h
class cstr_camp{
public:
	cstr_camp(const TCHAR* p):str(p){}
	~cstr_camp(){;}

	operator int()
	{
        return _tstoi(str);
	}
// 	operator unsigned int()
// 	{
//         return _tstoi(str);
// 	}
    operator long()
    {
        return _tstol(str);
    }
    operator ms::tstring()
    {
        return str;
    }
    operator short()
    {
        return _tstoi(str);
    }
private:
    const TCHAR * str;
};

cstr_camp str_camp(const TCHAR* str);
