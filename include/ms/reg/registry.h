#pragma once
//#include <Windows.h>
#include <string>

namespace ATL{
    class CRegKey;
}
namespace reg{

    class CRegistry{
        CRegistry();
    public:
        CRegistry(LPCTSTR mainKey);
        ~CRegistry();
        bool QueryValue(LPCTSTR pszValueName, TCHAR* szRegisterSerialBuf, size_t nRegisterSerialSize);
        bool SetValue(LPCTSTR value, LPCTSTR pszValueName);

        // std::string QueryValue(LPCTSTR lpRun, LPCTSTR pszValueName);


    private:
        bool _CreateKey(LPTSTR vKey);
        bool Open();
        ATL::CRegKey* regkey_;
        bool isOpen_;
        TCHAR mainKey_[MAX_PATH];
    };

//     bool QueryValue(LPCTSTR lpRun, LPCTSTR vKey, char* szRegisterSerialBuf, size_t nRegisterSerialSize);
//     std::string QueryValue(LPCTSTR lpRun, LPCTSTR vKey);
// 
//     bool CreateKey(LPCTSTR lpRun);
//     bool SetValue(LPCTSTR lpRun, LPCTSTR vKey);

}
