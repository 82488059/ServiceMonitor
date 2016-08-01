#include "stdafx.h"
#include <windows.h>
#include <ms/path/path.h>

namespace ms{
namespace path{

    ms::tstring GetExeFilePath()
    {
        TCHAR buf[MAX_PATH];
        DWORD size = ::GetModuleFileName(NULL, buf, MAX_PATH);
        if (0 == size)
        {
            return ms::tstring();
        }
        for (int i = size; i > 0; --i)
        {
            if ('\\' == buf[i])
            {
                buf[i+1] = 0;
                break;
            }
        }

        return buf;
    }
    ms::tstring GetExeFileName()
    {
        TCHAR buf[MAX_PATH];
        DWORD size = ::GetModuleFileName(NULL, buf, MAX_PATH);
        if (0 == size)
        {
            return ms::tstring();
        }
        TCHAR *p = &buf[size];
        for (int i = size; i > 0; --i, --p)
        {
            if ('\\' == *p)
            {
                ++p;
                break;
            }
        }
        return p;
    }

    const ms::tstring& CPath::GetExecutePath()
    {
        if (_executepath.empty ())
        {
            _executepath = GetExeFilePath();
        }
        return _executepath;
    }
    const ms::tstring& CPath::GetExecuteName()
    {
        if (_executename.empty ())
        {
            _executename = GetExeFileName();
        }
        return _executename;
    }
    
    CPath::CPath()
    {
    }

    CPath::~CPath()
    {
    }

    ms::tstring GetExecutePath()
    {
        return ms::path::Path::GetInstance()->GetExecutePath();

    }
    ms::tstring GetExecuteName()
    {
        return ms::path::Path::GetInstance()->GetExecuteName();
    }
}
}