#include "stdafx.h"
#include <ms/conf/LoadIni.h>

#include <Windows.h>

// bool LoadConfig(const char* appName, const char* KeyName, CString& rval, const char* path, const char* def)
// {
// 	if (NULL == appName  || NULL == KeyName || appName[0] == 0 || KeyName[0] == 0)
// 	{
// 		return false;
// 	}
// 	enum{MAXSIZE = 256};
// 	TCHAR strRval[MAXSIZE];
// 	DWORD l = GetPrivateProfileString(appName, KeyName, def, strRval, MAXSIZE, path);
// 	if (MAXSIZE-1 == l)
// 	{
// 		return false;
// 	}
// 	else if (MAXSIZE-2 == l)
// 	{
// 		return false;
// 	}
// 	else if (0 == l)
// 	{
// 		return false;
// 	}
// 	rval = strRval;
// 	return !rval.IsEmpty();
// }


bool LoadConfig(const TCHAR* appName, const TCHAR* KeyName, long& rval, const TCHAR* path, const long def)
{
	if (NULL == appName  || NULL == KeyName || appName[0] == 0 || KeyName[0] == 0)
	{
		return false;
	}
	rval = GetPrivateProfileInt(appName, KeyName, def, path);
	return true;
}

bool LoadConfig(const TCHAR* appName, const TCHAR* KeyName, bool& outVal, const TCHAR* path, const bool derault /*= true*/, const TCHAR* default)
{
	if (NULL == appName  || NULL == KeyName || appName[0] == 0 || KeyName[0] == 0)
	{
		return false;
	}
	long rval = GetPrivateProfileInt(appName, KeyName, (int)default, path);
	outVal = (rval == 1);
	return true;
}