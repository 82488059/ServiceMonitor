#pragma once 

#include <ms/Sington.hpp>
#include <ms/tstring.h>

namespace ms{
    namespace db{
        class DBConfig{

        public:
            ms::tstring ConnectStr;
            ms::tstring Server;
            ms::tstring DATABASE;
            ms::tstring UID;
            ms::tstring PWD;
            int nPort;
            DBConfig()
                : Server(_T("192.168.1.184"))
                , DATABASE(_T("Center"))
                , UID(_T("sa"))
                , PWD(_T("123456"))
                , nPort(1433)
            {

            }
        };
        typedef ms::Sington<DBConfig> GlobalDBConfig;
    } // namespace db
}// namespace ms