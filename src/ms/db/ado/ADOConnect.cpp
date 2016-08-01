#include "StdAfx.h"

#include <ms/db/ado/ADOConnect.h>
#include <Windows.h>
#include <ms/log/LogToFile.h>
#include <ms/tstring.h>
#include <tchar.h>
//////////////////////////////////////////////////////////////////////
// StdAfx.h中加入 #import "E:\vs2008\BuildNcProgram\BuildNcProgram\dll\msado15.dll" no_namespace rename("EOF","adoEOF")rename("BOF","adoBOF") //导入ADO动态连接库
// 
//////////////////////////////////////////////////////////////////////
namespace ms{
namespace db{

CAdoConnect::CAdoConnect()
{
	::CoInitialize(NULL);    //初始化OLE/COM库环境
}

CAdoConnect::CAdoConnect(const DBConfig& config)
    : m_szServer(config.Server)
    , m_szDATABASE(config.DATABASE)
    , m_szUID(config.UID)
    , m_szPWD(config.PWD)
    , m_szConnectStr(_T(""))
    , m_nPort(config.nPort)
{
    ::CoInitialize(NULL);    //初始化OLE/COM库环境
}


CAdoConnect::~CAdoConnect(void)
{
    //关闭记录集和连接
    if (m_pRecordset != NULL)
    {
        if (m_pRecordset->GetState() == adStateOpen)
        {
            m_pRecordset->Close();
        }
        if (m_pRecordset)
        {
            m_pRecordset.Release();
            m_pRecordset = NULL;
        }
    }
    if (m_pConnection != NULL)
    {
        if (m_pConnection->GetState() == adStateOpen)
        {
            m_pConnection->Close();
        }
        m_pConnection.Release();
        m_pConnection = NULL;
    }
    ::CoUninitialize();    //释放程序占用的COM资源
}
BOOL CAdoConnect::InitWith(const DBConfig& config)
{
	m_szServer = config.Server;
	m_szDATABASE = config.DATABASE;
	m_szUID = config.UID;
	m_szPWD = config.PWD;
	m_szConnectStr = config.ConnectStr;
	m_nPort = config.nPort;
	return TRUE;
}

// 连接数据库
BOOL CAdoConnect::OnInitAdoConnection(void)
{
    try
    {
        HRESULT hr = m_pConnection.CreateInstance("ADODB.Connection");//创建Connection 对象
        if (SUCCEEDED(hr))
        {
            m_pConnection->CursorLocation = adUseServer;
            // "Provider=SQLOLEDB.1;Server=数据库IP地址;UID=sa;PWD=你的sa登陆密码;DATABASE=数据库名称;";
            //_bstr_t strConnect = "driver={SQL Server};Server=192.168.0.117;DATABASE=TEST;UID=uid;PWD=passward;";                    
            if (m_szConnectStr.empty())
            {
//                 m_szConnectStr.Format(_T("driver={SQL Server};Server=%s,%d;DATABASE=%s;UID=%s;PWD=%s;")
//                     , m_szServer, m_nPort, m_szDATABASE, m_szUID, m_szPWD);
                
                TCHAR szPort[10];
                _itot_s(m_nPort, szPort, 10);
                
                m_szConnectStr =
                    _T("driver={SQL Server};Server=") + m_szServer + _T(",")
                    + szPort + _T(";DATABASE=") + m_szDATABASE
                    + _T(";UID=") + m_szUID + _T(";PWD=") + m_szPWD + _T(";");
            }
            //m_szConnectStr = "driver={SQL Server};Server=127.0.0.1,1433;DATABASE=EOSDB;UID=sa;PWD=12345;";
            _bstr_t strConnect = m_szConnectStr.c_str ();
            // "driver={SQL Server};Server=数据库IP地址;DATABASE=数据库名称;UID=sa;PWD=你的sa登陆密码;";
            HRESULT hr = m_pConnection->Open(strConnect, "", "", adModeUnknown);
            if (SUCCEEDED(hr))
            {
                return TRUE;
            }
            else
            {
                // AfxMessageBox("建立数据库连接失败!");
                return FALSE;
            }
        }
        // AfxMessageBox("建立ADODB.Connection失败!");
        return FALSE;
    }
    catch (const _com_error& e)
    {
        AppErrorLog(ms::tstring(e.Description()));//显示错误信息
        return FALSE;
    }
    catch (...)
    {
        return FALSE;
    }

    return FALSE;
}

// 执行Insert UpDate
BOOL CAdoConnect::ExecuteSQL(_bstr_t bstrSQL, long lOptions)
{
    try
    {
        //是否已经连接到数据库
        if (m_pConnection == NULL)
        {
            OnInitAdoConnection();
        }
        m_pRecordset = m_pConnection->Execute(bstrSQL, NULL, lOptions);
        return TRUE;
    }
    catch (const _com_error& e)
    {
        AppErrorLog(ms::tstring(e.Description()));
        return FALSE;
    }
    catch (...)
    {
        return FALSE;
    }
}
BOOL CAdoConnect::ExecuteSQL(const ms::tstring& sql, long lOptions)
{
    return ExecuteSQL(_bstr_t(sql.c_str ()), lOptions);
}

BOOL CAdoConnect::ExecuteSQL(const TCHAR* sql, long lOptions)
{
    return ExecuteSQL(_bstr_t(sql), lOptions);
}

_RecordsetPtr& CAdoConnect::GetRecordSet(const ms::tstring& SQL)
{
    return GetRecordSet(_bstr_t(SQL.c_str ()));
}
_RecordsetPtr& CAdoConnect::GetRecordSet(const TCHAR* pSQL)
{
    return GetRecordSet(_bstr_t(pSQL));
}

//执行查询
_RecordsetPtr& CAdoConnect::GetRecordSet(_bstr_t bstrSQL)
{
    try
    {
        //连接数据库 如果Connection对象为空则重新连接数据库
        if (m_pConnection == NULL)
        {
            OnInitAdoConnection();
        }
        if (NULL == m_pRecordset)
        {
            //创建记录集对象
            HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
            if (FAILED(hr))
            {
                return m_pRecordset;
            }
        }
        //取得表中的记录
        m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
    }
    catch (const _com_error& e)
    {
        //异常 显示错误信息
        AppErrorLog(ms::tstring(e.Description()));
        //AfxMessageBox(e.Description());
    }
    return m_pRecordset;
}
BOOL CAdoConnect::GetValue(const ms::tstring& index, ms::tstring& val)
{
    _variant_t var = m_pRecordset->GetCollect(_variant_t(index.c_str()));
    if (var.vt != VT_NULL && var.vt != VT_EMPTY)
    {
        val = _bstr_t(var);
        return TRUE;
    }
    return FALSE;
}

BOOL CAdoConnect::GetVersion(ms::tstring& ver)
{
	if (!ExecuteSQL(_T("select @@Version as version")))
	{
		return FALSE;
	}
    _variant_t var = m_pRecordset->GetCollect("version");
    if (var.vt != VT_NULL && var.vt != VT_EMPTY)
    {
        ver = _bstr_t(var);
        return TRUE;
    }
    return FALSE;

}
// select * from sysobjects where name = 'T_ALERT'
BOOL CAdoConnect::ExistTable(const ms::tstring& tableName)
{
    ms::tstring sql = _T("select * from sysobjects where name = '") + tableName + _T("'");
	if (!ExecuteSQL(_bstr_t(sql.c_str ())))
	{
		return FALSE;
	}
	_variant_t var = m_pRecordset->GetCollect("name");
	if (var.vt != VT_NULL && var.vt != VT_EMPTY)
	{
		return TRUE;
	}
	return FALSE;

}

} // namespace db
} // namespace ms