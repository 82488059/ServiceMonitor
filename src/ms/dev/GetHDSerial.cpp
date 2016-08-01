
#include "stdafx.h"

#include <ms/dev/GetHDSerial.h>
#include <ms/dev/GetHDSerial1.h>
#include <ms/dev/GetHDSerial2.h>
#include <ms/dev/ElevationPrivilege.h>

namespace dev{

const std::string& GetHDSerial()
{
    static std::string serial;
    if (!serial.empty ())
    {
        return serial;
    }
    char buf[256];
    memset(buf, 0, 256);
    if (GetDiskSerial(buf, 256))
    {
        serial = buf;
        serial.erase(0, serial.find_first_not_of(" "));
    }

    if (serial.empty())
    {
        dev::ElevationPrivilege();
        CGetHDSerial1 get;
        serial = get.GetHDSerial();
        serial.erase(0, serial.find_first_not_of(" "));
    }

    return serial;
}

}
