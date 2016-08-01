#pragma once

namespace service{
    bool ServiceInstall(LPCTSTR lpServiceName);

    void ServiceRemove(LPCTSTR lpServiceName, SERVICE_STATUS & ServiceStatus);


}
