#include "stdafx.h"
#include <Windows.h>
#include <ms/service/service.h>



namespace service{

    bool ServiceInstall(LPCTSTR lpServiceName)
    {
        bool install = false;
        SC_HANDLE   schService;
        SC_HANDLE   schSCManager;

        TCHAR szPath[512];

        if (GetModuleFileName(NULL, szPath, 512) == 0)
        {
            _tprintf(_T("%s Unable to install: %d\n"), lpServiceName, GetLastError());
            return false;
        }

        schSCManager = OpenSCManager(
            NULL,                   // machine (NULL == local)
            NULL,                   // database (NULL == default)
            SC_MANAGER_ALL_ACCESS   // access required
            );
        if (schSCManager)
        {
            schService = CreateService(
                schSCManager,       // SCManager database
                lpServiceName,	    // name of service
                lpServiceName,		// name to display
                SERVICE_ALL_ACCESS,         // desired access
                SERVICE_WIN32_OWN_PROCESS,  // service type
                SERVICE_AUTO_START,         // start type
                SERVICE_ERROR_NORMAL,       // error control type
                szPath,                     // service's binary
                NULL,                       // no load ordering group
                NULL,                       // no tag identifier
                NULL,				        // dependencies
                NULL,                       // LocalSystem account
                NULL);                      // no password

            if (schService)
            {
                _tprintf(_T("installed OK.\n"));
                CloseServiceHandle(schService);
                install = true;
            }
            else
            {
                _tprintf(_T("%s CreateService failed - %d\n"), lpServiceName, GetLastError());
                install = false;
            }

            CloseServiceHandle(schSCManager);
        }
        else
        {
            _tprintf(_T("%s OpenSCManager failed - %d\n"), lpServiceName, GetLastError());
            install = false;
        }

        return install;
    }
    void ServiceRemove(LPCTSTR lpServiceName, SERVICE_STATUS & ServiceStatus)
    {
        SC_HANDLE   schService;
        SC_HANDLE   schSCManager;

        schSCManager = OpenSCManager(
            NULL,                   // machine (NULL == local)
            NULL,                   // database (NULL == default)
            SC_MANAGER_ALL_ACCESS   // access required
            );
        if (schSCManager)
        {
            schService = OpenService(schSCManager, lpServiceName, SERVICE_ALL_ACCESS);
            if (schService)
            {
                if (ControlService(schService, SERVICE_CONTROL_STOP, &ServiceStatus))
                {
                    _tprintf(_T("Stopping %s."), lpServiceName);
                    Sleep(1000);

                    while (QueryServiceStatus(schService, &ServiceStatus))
                    {
                        if (ServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
                        {
                            _tprintf(TEXT("."));
                            Sleep(1000);
                        }
                        else	break;
                    }

                    if (ServiceStatus.dwCurrentState == SERVICE_STOPPED)
                        _tprintf(_T("\n%s stopped.\n"), lpServiceName);
                    else
                        _tprintf(_T("\n%s failed to stop.\n"), lpServiceName);
                }

                // now remove the service
                if (DeleteService(schService))
                    _tprintf(_T("%s removed.\n"), lpServiceName);
                else
                    _tprintf(_T("DeleteService failed - %d\n"), GetLastError());


                CloseServiceHandle(schService);
            }
            else
                _tprintf(_T("OpenService failed - %d\n"), GetLastError());

            CloseServiceHandle(schSCManager);
        }
        else
            _tprintf(_T("OpenSCManager failed - %s\n"), GetLastError());
    }



}
