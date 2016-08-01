#pragma once
#include <ms/str_camp/str_camp.h>
#include <tchar.h>
#include <Windows.h>
// bool LoadConfig(const char* appName, const char* KeyName, CString& outVal, const char* path, const char* default = "");
bool LoadConfig(const TCHAR* appName, const TCHAR* KeyName, long& outVal, const TCHAR* path, const long default = 0);
bool LoadConfig(const TCHAR* appName, const TCHAR* KeyName, bool& outVal, const TCHAR* path, const bool default = true);

template<class T>
bool LoadConfig(const TCHAR* appName, const TCHAR* KeyName, T& outVal, const TCHAR* path, const T& def)
{
	enum{MAXSIZE = 256};
	TCHAR strRval[MAXSIZE];
	int l = GetPrivateProfileString(appName, KeyName, _T(""), strRval, MAXSIZE, path);
	if (0 == strRval[0])
	{
		outVal = def;
		return false;
	}
	outVal = str_camp(strRval);
	return true;
}

template<class T>
bool LoadConfig(const TCHAR* appName, const TCHAR* KeyName, T& outVal, const ms::tstring& path, const T& def)
{
    enum{ MAXSIZE = 256 };
    TCHAR strRval[MAXSIZE];
    int l = GetPrivateProfileString(appName, KeyName, _T(""), strRval, MAXSIZE, path.c_str());
    if (0 == strRval[0])
    {
        outVal = def;
        return false;
    }
    outVal = str_camp(strRval);
    return true;
}
