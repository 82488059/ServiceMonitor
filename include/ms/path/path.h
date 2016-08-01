#pragma once
#include <ms/tstring.h>
#include <ms/Sington.hpp>
namespace ms{
namespace path{

    class CPath
    {
    public:
        CPath();
        ~CPath();
        const ms::tstring& GetExecutePath();
        const ms::tstring& GetExecuteName();
    private:
        ms::tstring _executepath;
        ms::tstring _executename;
    };
    typedef ms::Sington<CPath> Path;

    ms::tstring GetExecutePath();
    ms::tstring GetExecuteName();

}
}