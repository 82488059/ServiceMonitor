//#define _WIN32_DCOM

#include "stdafx.h"
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <nb30.h>
#include <ms/Dev/GetHDSerial2.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Netapi32.lib")

using namespace std;
namespace dev{

    int GetMacAddress(char *mac)
    {
        NCB ncb;

        typedef struct _ASTAT_
        {
            ADAPTER_STATUS adapt;
            NAME_BUFFER NameBuff[30];
        } ASTAT, *PASTAT;
        ASTAT Adapter;
        //    ADAPTER_STATUS Adapter;

        LANA_ENUM lana_enum;


        UCHAR uRetCode;
        memset(&ncb, 0, sizeof(ncb));
        memset(&lana_enum, 0, sizeof(lana_enum));

        ncb.ncb_command = NCBENUM;
        ncb.ncb_buffer = (unsigned char *)&lana_enum;
        ncb.ncb_length = sizeof(LANA_ENUM);
        uRetCode = Netbios(&ncb);
        if (uRetCode != NRC_GOODRET)
            return uRetCode;

        for (int lana = 0; lana < lana_enum.length; lana++)
        {
            ncb.ncb_command = NCBRESET;
            ncb.ncb_lana_num = lana_enum.lana[lana];
            uRetCode = Netbios(&ncb);
            if (uRetCode == NRC_GOODRET)
                break;
        }
        if (uRetCode != NRC_GOODRET)
            return uRetCode;

        memset(&ncb, 0, sizeof(ncb));
        ncb.ncb_command = NCBASTAT;
        ncb.ncb_lana_num = lana_enum.lana[0];
#pragma warning(push)
#pragma warning(disable:4996)
        strcpy((char*)ncb.ncb_callname, "*");
        ncb.ncb_buffer = (unsigned char *)&Adapter;
        ncb.ncb_length = sizeof(Adapter);
        uRetCode = Netbios(&ncb);
        if (uRetCode != NRC_GOODRET)
            return uRetCode;
        sprintf(mac, "%02X-%02X-%02X-%02X-%02X-%02X",
            Adapter.adapt.adapter_address[0],
            Adapter.adapt.adapter_address[1],
            Adapter.adapt.adapter_address[2],
            Adapter.adapt.adapter_address[3],
            Adapter.adapt.adapter_address[4],
            Adapter.adapt.adapter_address[5]);
#pragma warning(pop)

        return 0;
    }
    BOOL GetDiskSerial(char* pBuf, size_t bufSize)
    {
        HRESULT hres;

        // Step 1: --------------------------------------------------
        // Initialize COM. ------------------------------------------

        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            cout << "Failed to initialize COM library. Error code = 0x"
                << hex << hres << endl;
            return FALSE;                  // Program has failed.
        }

        // Step 2: --------------------------------------------------
        // Set general COM security levels --------------------------
        // Note: If you are using Windows 2000, you need to specify -
        // the default authentication credentials for a user by using
        // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
        // parameter of CoInitializeSecurity ------------------------

        hres = CoInitializeSecurity(
            NULL,
            -1,                          // COM authentication
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities 
            NULL                         // Reserved
            );

        if (FAILED(hres))
        {
            cout << "Failed to initialize security. Error code = 0x"
                << hex << hres << endl;
            CoUninitialize();
            return FALSE;                    // Program has failed.
        }

        // Step 3: ---------------------------------------------------
        // Obtain the initial locator to WMI -------------------------

        IWbemLocator *pLoc = NULL;

        hres = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, (LPVOID *)&pLoc);

        if (FAILED(hres))
        {
            cout << "Failed to create IWbemLocator object."
                << " Err code = 0x"
                << hex << hres << endl;
            CoUninitialize();
            return FALSE;                 // Program has failed.
        }

        // Step 4: -----------------------------------------------------
        // Connect to WMI through the IWbemLocator::ConnectServer method

        IWbemServices *pSvc = NULL;

        // Connect to the root\cimv2 namespace with
        // the current user and obtain pointer pSvc
        // to make IWbemServices calls.
        hres = pLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
            NULL,                    // User name. NULL = current user
            NULL,                    // User password. NULL = current
            0,                       // Locale. NULL indicates current
            NULL,                    // Security flags.
            0,                       // Authority (e.g. Kerberos)
            0,                       // Context object 
            &pSvc                    // pointer to IWbemServices proxy
            );

        if (FAILED(hres))
        {
            cout << "Could not connect. Error code = 0x"
                << hex << hres << endl;
            pLoc->Release();
            CoUninitialize();
            return FALSE;                // Program has failed.
        }
#ifdef _DEBUG
        cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;
#endif // _DEBUG

        // Step 5: --------------------------------------------------
        // Set security levels on the proxy -------------------------

        hres = CoSetProxyBlanket(
            pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            NULL,                        // Server principal name 
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities 
            );

        if (FAILED(hres))
        {
            cout << "Could not set proxy blanket. Error code = 0x"
                << hex << hres << endl;
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return FALSE;               // Program has failed.
        }

        // Step 6: --------------------------------------------------
        // Use the IWbemServices pointer to make requests of WMI ----

        // For example, get the name of the operating system
        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_PhysicalMedia"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

        if (FAILED(hres))
        {
            cout << "Query for physical media failed."
                << " Error code = 0x"
                << hex << hres << endl;
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return FALSE;               // Program has failed.
        }

        // Step 7: -------------------------------------------------
        // Get the data from the query in step 6 -------------------

        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;
        BOOL rVal = FALSE;
        while (pEnumerator)
        {
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                &pclsObj, &uReturn);

            if (0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr))
            {
#ifdef _DEBUG
                wcout << "Serial Number : " << vtProp.bstrVal << endl;
#endif // _DEBUG
                char* ps = _com_util::ConvertBSTRToString(vtProp.bstrVal);
                strcpy_s(pBuf, bufSize, ps);
                HexStrToStr(pBuf, pBuf, bufSize);
                delete[] ps;
                rVal = TRUE;

                VariantClear(&vtProp);
                break;
            }
            VariantClear(&vtProp);
        }

        // Cleanup
        // ========

        pSvc->Release();
        pLoc->Release();
        pEnumerator->Release();
        pclsObj->Release();
        CoUninitialize();
        return rVal;
    }
    BOOL HexStrToStr(char* hexStr, char*pBuf, size_t bufSize)
    {
        int l = strlen(hexStr);
        if (l % 2)
        {
            return FALSE;
        }
        char numStr[3] = "00";
        char* p = numStr + 2;
        for (int i = 0; i < bufSize && i < l; ++i)
        {
            numStr[0] = hexStr[2 * i];
            numStr[1] = hexStr[2 * i + 1];
            int a = strtol(numStr, &p, 16);
            //cout << a;
            if (i % 2)
            {
                pBuf[i - 1] = a;
            }
            else
            {
                pBuf[i + 1] = a;
            }
        }
        return TRUE;
    }

    // int main(int argc, char **argv)
    // {
    //     GetDiskSerial();
    // 
    //     char szAddr[20];
    //     GetMacAddress(szAddr);
    //     cout << "\nNet adapter MAC Address:" << szAddr << endl << endl;
    // 
    //     return 0;   // Program successfully completed.
    // }
}