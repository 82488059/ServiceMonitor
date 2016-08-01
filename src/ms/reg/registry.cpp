#include "stdafx.h"
#include <windows.h>
#include <atlbase.h>
#include <ms/reg/registry.h>
#include <ms/log/LogToFile.h>

namespace reg{
    CRegistry::CRegistry(LPCTSTR mainKey)
        : regkey_(NULL)
        , isOpen_(false)
        //, mainKey_(NULL)
    {
        regkey_ = new CRegKey;
        int len = lstrlen(mainKey);
        if (len >= MAX_PATH)
        {
            //throw("注册表项太长");
            lstrcpy(mainKey_, _T("SOFTWARE\\AudioService"));
            return;
        }
        lstrcpy(mainKey_, mainKey);

    }
    CRegistry::~CRegistry()
    {
        if (regkey_)
        {
            regkey_->Close();
            delete regkey_;
            regkey_ = NULL;
        }
    }
    bool CRegistry::Open()
    {
        if (isOpen_)
        {
            return isOpen_;
        }
        LSTATUS nREt = regkey_->Open(HKEY_LOCAL_MACHINE, mainKey_, KEY_READ | KEY_WRITE);
		if (nREt != ERROR_SUCCESS || nREt == ERROR_FILE_NOT_FOUND)//没有创建主键
		{
            AppErrorLog(_T("打开注册表失败！"));
            if (ERROR_SUCCESS != regkey_->Create(HKEY_LOCAL_MACHINE, mainKey_))
				return false;
            if (ERROR_SUCCESS != regkey_->Open(HKEY_LOCAL_MACHINE, mainKey_, KEY_READ | KEY_WRITE))
				return false;
		}
        AppErrorLog(_T("打开注册表成功！"));

        isOpen_ = true;
        return true;
    }

    bool CRegistry::SetValue(LPCTSTR value, LPCTSTR Name)
    {
        if (!Open ())
        {
            return false;
        }
        LSTATUS state = regkey_->SetStringValue(Name, value);
        return ERROR_SUCCESS == state;
    }

    bool CRegistry::_CreateKey(LPTSTR vKey)
    {
        return ERROR_SUCCESS == regkey_->Create(HKEY_LOCAL_MACHINE, vKey);
    }


    bool CRegistry::QueryValue(LPCTSTR pszValueName, TCHAR* szRegisterSerial, size_t nRegisterSerialSize)
    {
        if (NULL == pszValueName || szRegisterSerial == NULL)
        {
            return false;
        }
        if (!Open ())
        {
            return false;
        }
        ULONG l = nRegisterSerialSize;
        if (ERROR_SUCCESS != regkey_->QueryStringValue(pszValueName, szRegisterSerial, &l))
        {
            return false;
        }
        return true;
    }

}