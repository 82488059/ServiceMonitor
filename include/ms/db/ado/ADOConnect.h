#pragma once
//#import "E:\vs2008\BuildNcProgram\BuildNcProgram\dll\msado15.dll" no_namespace rename("EOF","adoEOF")rename("BOF","adoBOF") //导入ADO动态连接库
#import "c:\Program Files\Common Files\System\ado\msado15.dll"no_namespace \
    rename("EOF", "adoEOF")rename("BOF", "adoBOF")
#include "DBConfig.h"
// #include <string>
#include <ms/tstring.h>


namespace ms{
namespace db{
class CAdoConnect
{
private:
    ms::tstring m_szConnectStr;
    ms::tstring m_szServer;
    ms::tstring m_szDATABASE;
    ms::tstring m_szUID;
    ms::tstring m_szPWD;
    int m_nPort;
private:
    _ConnectionPtr m_pConnection;     // 连接对象指针
    _RecordsetPtr  m_pRecordset;      // 记录集对象指针

public:
	CAdoConnect();
    CAdoConnect(const DBConfig& config);

    virtual~CAdoConnect(void);

public:
	BOOL InitWith(const DBConfig& config);
    // 连接数据库
    BOOL OnInitAdoConnection(void);
    // 执行Insert UpDate
    BOOL ExecuteSQL(_bstr_t bstrSQL, long lOptions = adCmdText);
    BOOL ExecuteSQL(const ms::tstring&, long lOptions = adCmdText);
    BOOL ExecuteSQL(const TCHAR*, long lOptions = adCmdText);
    // 执行查询得到记录集
    _RecordsetPtr& GetRecordSet(const ms::tstring& SQL);
    _RecordsetPtr& GetRecordSet(const TCHAR* pSQL);
    _RecordsetPtr& GetRecordSet(_bstr_t bstrSQL);
    // 取数据库版本信息
    BOOL GetVersion(ms::tstring& ver);
    // 取值
    BOOL GetValue(const ms::tstring& index, ms::tstring& val);
    template <class T>
    BOOL GetValue(const ms::tstring& index, T& val)
    {
        _variant_t var = m_pRecordset->GetCollect(_variant_t(index.c_str ()));
        if (var.vt != VT_NULL && var.vt != VT_EMPTY)
        {
            val = var;
            return TRUE;
        }
        return FALSE;
    }
//     template <class T>
//     BOOL GetValue(const char* index, T& val)
//     {
//         _variant_t var = m_pRecordset->GetCollect(_variant_t(index));
//         if (var.vt != VT_NULL && var.vt != VT_EMPTY)
//         {
//             val = _bstr_t(var);
//             return TRUE;
//         }
//         return FALSE;
//     }

	// 表名是否存在
	// select * from sysobjects where name = 'T_ALERT'
    BOOL ExistTable(const ms::tstring& tableName);
    // 操作符重载
public:
    _RecordsetPtr& operator->()
    {
        return m_pRecordset;
    }
};
} // namespace db
} // ms