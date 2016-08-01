#include "stdafx.h"
#include <ms/dev/ElevationPrivilege.h>
#include <windows.h>

namespace dev{
    bool ElevationPrivilege()
    {
        HANDLE hToken;
        LUID sedebugnameValue;
        TOKEN_PRIVILEGES tkp;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            return   false;
        }
        if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
        {
            CloseHandle(hToken);
            return false;
        }
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = sedebugnameValue;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
        {
            CloseHandle(hToken);
            return false;
        }ERROR_NOT_ALL_ASSIGNED;
        DWORD ge =GetLastError();
        return true;
    }


}

