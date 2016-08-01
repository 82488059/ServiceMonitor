#pragma once
#include <windows.h>
#include <winsvc.h>
#include <vector>
#include <ms/tstring.h>

namespace ms{

namespace service{
    
    class sc{



    public:

        bool Open()
        {
            scm_ = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
            return NULL != scm_;
        }
        void Close()
        {
            if (pessp_)
            {
                free(pessp_);
                pessp_ = NULL;
            }
            if (NULL != scm_)
            {
                CloseServiceHandle(scm_);
                scm_ = NULL;
            }
        }

        bool EnumServices()
        {
            if (pessp_)
            {
                free(pessp_);
                pessp_ = NULL;
            }
            DWORD need = 0;
            DWORD ret = 0;
            EnumServicesStatusEx(scm_, SC_ENUM_PROCESS_INFO, SERVICE_WIN32
                , SERVICE_STATE_ALL, (PBYTE)pessp_, 0
                , &need, &ret, NULL, NULL);

            if (need == 0) 
                return false;
            pessp_ = (LPENUM_SERVICE_STATUS_PROCESS)malloc(need);
            ZeroMemory(pessp_, need);
            if (!EnumServicesStatusEx(scm_, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (PBYTE)pessp_, need, &need, &essp_size_, NULL, NULL))
            {
                free(pessp_);
                return false;
            }
            return true;
        }
        bool GetInfoByServiceName(const ms::tstring& servicename, std::vector<ENUM_SERVICE_STATUS_PROCESS>& esspv)
        {
            if (NULL == pessp_)
            {
                if (!Open())
                {
                    return false;
                }
                if (!EnumServices())
                {
                    return false;
                }
            }
            bool rval = false;
            for (DWORD i = 0; i < essp_size_; i++)
            {
                if (0 == StrCmp(servicename.c_str(), pessp_[i].lpServiceName))
                {
                    ENUM_SERVICE_STATUS_PROCESS essp;
                    memcpy(&essp, &pessp_[i], sizeof(essp));
                    esspv.push_back(essp);
                    rval = true;
                }
            }
            return rval;
        }
        bool GetInfoByDisplayName(const ms::tstring& displayName, std::vector<ENUM_SERVICE_STATUS_PROCESS>& esspv)
        {
            if (NULL == pessp_)
            {
                if (!Open())
                {
                    return false;
                }
                if (!EnumServices())
                {
                    return false;
                }
            }
            bool rval = false;
            for (DWORD i = 0; i < essp_size_; i++)
            {
                if (0 == StrCmp(displayName.c_str(), pessp_[i].lpDisplayName))
                {
                    ENUM_SERVICE_STATUS_PROCESS essp;
                    memcpy(&essp, &pessp_[i], sizeof(essp));
                    esspv.push_back(essp);
                    rval = true;
                }
            }
            return rval;
        }
        enum State{ sc_start, sc_stop, sc_delete };
        bool SetServerState(const ms::tstring& displayName, State state)
        {
            if (NULL == scm_)
            {
                if (!Open())
                {
                    return false;
                }
            }
            SC_HANDLE shDefineService = OpenService(scm_, displayName.c_str(), SERVICE_ALL_ACCESS);
            if (NULL == shDefineService)
            {
                return false;
            }
            BOOL rval = FALSE;
            switch (state)
            {
            case State::sc_start:
                rval = StartService(shDefineService, 0, NULL);
                break;

            case State::sc_stop:
                SERVICE_STATUS ss;
                rval = ControlService(shDefineService, SERVICE_CONTROL_STOP, &ss);
                break;

            case State::sc_delete:
                rval = DeleteService(shDefineService);
                break;

            default:
                break;
            }
            CloseServiceHandle(shDefineService);
            return !!rval;
        }
        bool ServiceInstall(const ms::tstring& szPath, const ms::tstring& lpDisplayName, const ms::tstring& lpServiceName)
        {
            if (NULL == scm_)
            {
                if (!Open())
                {
                    return false;
                }
            }
            SC_HANDLE schService = CreateService(
                scm_,       // SCManager database
                lpServiceName.c_str(),	    // name of service
                lpDisplayName.c_str(),		// name to display
                SERVICE_ALL_ACCESS,         // desired access
                SERVICE_WIN32_OWN_PROCESS,  // service type
                SERVICE_AUTO_START,         // start type
                SERVICE_ERROR_NORMAL,       // error control type
                szPath.c_str(),                     // service's binary
                NULL,                       // no load ordering group
                NULL,                       // no tag identifier
                NULL,				        // dependencies
                NULL,                       // LocalSystem account
                NULL);                      // no password
            bool rval = false;
            if (NULL != schService)
            {
                rval = true;
            }
            CloseServiceHandle(schService);
            return rval;
        }
        bool GetStatus(ms::tstring& displayName, SERVICE_STATUS & status)
        {
            if (NULL == scm_)
            {
                if (!Open())
                {
                    return false;
                }
            }
            SC_HANDLE shDefineService = OpenService(scm_, displayName.c_str(), SERVICE_ALL_ACCESS);
            if (NULL == shDefineService)
            {
                return false;
            }
            if (QueryServiceStatus(shDefineService, &status))
            {
                CloseServiceHandle(shDefineService);
                return true;
            }
            CloseServiceHandle(shDefineService);
            return false;
        }

        bool GetConfig(const ms::tstring& displayName, LPQUERY_SERVICE_CONFIG& status)
        {
            if (NULL == scm_)
            {
                if (!Open())
                {
                    return false;
                }
            }
            SC_HANDLE shDefineService = OpenService(scm_, displayName.c_str(), SERVICE_ALL_ACCESS);
            if (NULL == shDefineService)
            {
                return false;
            }
            if (pqsc_)
            {
                free(pqsc_);
                pqsc_ = NULL;
            }
            DWORD needsize = 0;
            bool rval = false;
            if (!QueryServiceConfig(shDefineService, status, 0, &needsize))
            {
                pqsc_ = (LPQUERY_SERVICE_CONFIG)malloc(needsize);
                if (QueryServiceConfig(shDefineService, pqsc_, needsize, &needsize))
                {
                    status = pqsc_;
                    rval = true;
                }
            }
            CloseServiceHandle(shDefineService);
            return rval;
        }
    public:
        sc() : scm_{ NULL }, pessp_{ NULL }, essp_size_{ 0 }, pqsc_{NULL}
        {}
        ~sc()
        {
            if (pessp_)
            {
                free(pessp_);
                pessp_ = NULL;
            }
            if (pqsc_)
            {
                free(pqsc_);
                pqsc_ = NULL;
            }
            if (NULL != scm_)
            {
                CloseServiceHandle(scm_);
                scm_ = NULL;
            }
        }

    private:
        SC_HANDLE scm_{ NULL };
        DWORD essp_size_{0};
        LPENUM_SERVICE_STATUS_PROCESS pessp_{ NULL };
        LPQUERY_SERVICE_CONFIG pqsc_{ NULL };
//         std::vector<ENUM_SERVICE_STATUS_PROCESS> essp_;

    };


}
}