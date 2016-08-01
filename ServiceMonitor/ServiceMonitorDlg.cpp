
// ServiceMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "ServiceMonitorDlg.h"
#include "afxdialogex.h"

#include <ms/service/sc.h>
#include <ms/path/path.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()
// WM_TRAYNOTIFY

// CServiceMonitorDlg 对话框



CServiceMonitorDlg::CServiceMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServiceMonitorDlg::IDD, pParent)
    , m_szPath(_T(""))
    , m_serviceName(_T(""))
    , m_displayName(_T(""))
{
    memset(&m_notifyicon, 0, sizeof(m_notifyicon)); // Initialize NOTIFYICONDATA struct
    m_notifyicon.cbSize = sizeof(m_notifyicon);
    m_notifyicon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServiceMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SERVICES, m_servicesListCtrl);
    DDX_Control(pDX, IDC_BT_START, m_btStart);
    DDX_Control(pDX, IDC_BT_STOP, m_btStop);
    DDX_Control(pDX, IDC_LIST_LOG, m_logListCtrl);
    DDX_Control(pDX, IDC_BT_REMOVE, m_btRemove);
    DDX_Text(pDX, IDC_EDIT_PATH, m_szPath);
    DDX_Control(pDX, IDC_BT_RESTART, m_btRestart);
    DDX_Text(pDX, IDC_EDIT_SERVICE_NAME, m_serviceName);
    DDX_Text(pDX, IDC_EDIT_SERVICE_DISPLAYNAME, m_displayName);
    DDX_Control(pDX, IDC_BT_INSTALL, m_btInstall);
}

BEGIN_MESSAGE_MAP(CServiceMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_TRAYNOTIFY, &CServiceMonitorDlg::OnTrayNotify)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BT_START, &CServiceMonitorDlg::OnBnClickedBtStart)
    ON_BN_CLICKED(IDC_BT_STOP, &CServiceMonitorDlg::OnBnClickedBtStop)
    ON_LBN_SELCHANGE(IDC_LIST_SERVICES, &CServiceMonitorDlg::OnLbnSelchangeListServices)
    ON_BN_CLICKED(IDC_BT_REMOVE, &CServiceMonitorDlg::OnBnClickedBtRemove)
    ON_BN_CLICKED(IDC_BT_MIN, &CServiceMonitorDlg::OnBnClickedBtMin)
    ON_BN_CLICKED(IDC_BT_INSTALL, &CServiceMonitorDlg::OnBnClickedBtInstall)
    ON_BN_CLICKED(IDC_BT_RESTART, &CServiceMonitorDlg::OnBnClickedBtRestart)
    ON_BN_CLICKED(IDC_BT_REFRESH, &CServiceMonitorDlg::OnBnClickedBtRefresh)
    ON_BN_CLICKED(IDC_BT_SELECT, &CServiceMonitorDlg::OnBnClickedBtSelect)
END_MESSAGE_MAP()


// CServiceMonitorDlg 消息处理程序

BOOL CServiceMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO:  在此添加额外的初始化代码
    
    // 管理的服务名称
    //CString serviceName = _T("Apache2.4"); 
    CString serviceName = _T("AudioService");
    m_appHeadName = _T("ServiceMonitor-");
    m_displayName = serviceName;
    m_serviceName = serviceName;
    // 程序名称
    CString appName = m_appHeadName + m_serviceName;
    SetWindowText(appName);
    m_notifyicon.hWnd = GetSafeHwnd();
    m_notifyicon.uCallbackMessage = WM_TRAYNOTIFY;
    // Set tray icon and tooltip
    m_notifyicon.hIcon = m_hIcon;
    // Set tray notification tip information
    _tcsncpy_s(m_notifyicon.szTip, appName, appName.GetLength());
    Shell_NotifyIcon(NIM_ADD, &m_notifyicon);

    RefreshWindowsName();


    if (m_sc.Open())
    {
        if (m_sc.EnumServices())
        {
            if (m_sc.GetInfoByServiceName(ms::tstring(m_serviceName), m_esspv))
            {
                for (ENUM_SERVICE_STATUS_PROCESS& var : m_esspv)
                {
                    m_servicesListCtrl.AddString(var.lpDisplayName);
                }
            }
        }
    }
    m_btStop.EnableWindow(FALSE);
    m_btStart.EnableWindow(FALSE);
    m_btRemove.EnableWindow(FALSE);
    m_btInstall.EnableWindow(FALSE);

    m_logListCtrl;
    
    //m_szPath = ms::path::GetExecutePath().c_str() + m_serviceName + _T(".exe");
    UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServiceMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServiceMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServiceMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CServiceMonitorDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    UINT uMsg = (UINT)lParam;
    switch (uMsg)
    {
    case WM_RBUTTONUP:
    {
        //右键处理
        CMenu menuTray;
        CPoint point;
        int id;
        GetCursorPos(&point);

        menuTray.LoadMenu(IDR_MENU_TRAY);
        id = menuTray.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
#if 0
        CString strInfo;
        strInfo.Format(L"menuid %d", id);
        LPCTSTR strtmp;
        strtmp = strInfo.GetBuffer(0);
        MessageBox(strtmp, L"test");
#endif
        switch (id){
        case IDR_TRAY_EXIT:
            OnOK();
            break;
        case IDR_TRAY_RESTORE:
            //窗口前端显示
            SetForegroundWindow();
            ShowWindow(SW_SHOWNORMAL);
            break; 
            
        case IDR_TRAY_MSC:
            ShellExecute(NULL, _T("open"), _T("services.msc"), NULL, NULL, SW_SHOWNORMAL);
            break;
        default:
            break;
        }
        break;
    }
    case WM_LBUTTONDBLCLK:
        SetForegroundWindow();
        ShowWindow(SW_SHOWNORMAL);
        break;
    default:
        break;
    }
    return 0;
}


void CServiceMonitorDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO:  在此处添加消息处理程序代码
    if (nType == SIZE_MINIMIZED){
        ShowWindow(SW_HIDE);
    }
}


void CServiceMonitorDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO:  在此处添加消息处理程序代码
    m_notifyicon.hIcon = NULL;
    Shell_NotifyIcon(NIM_DELETE, &m_notifyicon);
}

bool CServiceMonitorDlg::ServiceStart()
{
    if (m_displayName.IsEmpty())
    {
        return false;
    }
    ResetButtonStatus(0);
    if (m_sc.SetServerState(ms::tstring(m_displayName), ms::service::sc::sc_start))
    {
        m_logListCtrl.AddString(m_displayName + _T("启动成功！"));
        ResetButtonStatus(SERVICE_RUNNING);
        return true;
    }
    else
    {
        m_logListCtrl.AddString(m_displayName + _T("启动失败！"));
        ResetButtonStatus(SERVICE_STOPPED);
        return false;
    }
}
bool CServiceMonitorDlg::ServiceStop()
{
    if (m_displayName.IsEmpty())
    {
        return false;
    }
    ResetButtonStatus(0);

    if (m_sc.SetServerState(ms::tstring(m_displayName), ms::service::sc::sc_stop))
    {
        m_logListCtrl.AddString(m_displayName + _T("停止成功！"));
        ResetButtonStatus(SERVICE_STOPPED);
        return true;
    }
    else
    {
        m_logListCtrl.AddString(m_displayName + _T("停止失败！"));
        ResetButtonStatus(SERVICE_RUNNING);
        return false;
    }
}

void CServiceMonitorDlg::OnBnClickedBtStart()
{
    // TODO:  在此添加控件通知处理程序代码
    ServiceStart();
}


void CServiceMonitorDlg::OnBnClickedBtStop()
{
    // TODO:  在此添加控件通知处理程序代码
    ServiceStop();
}

void CServiceMonitorDlg::OnBnClickedBtRemove()
{
    // TODO:  在此添加控件通知处理程序代码
    if (m_displayName.IsEmpty())
    {
        return;
    }
    UpdateData(TRUE);

    m_btRemove.EnableWindow(FALSE);

    if (m_sc.SetServerState(ms::tstring(m_displayName), ms::service::sc::sc_delete))
    {
        m_logListCtrl.AddString(m_displayName+_T("删除成功！"));
        ResetButtonStatus(0);
    }
    else
    {
        m_logListCtrl.AddString(m_displayName + _T("删除失败！"));
        ResetButtonStatus(0);
    }
    OnBnClickedBtRefresh();

}

void CServiceMonitorDlg::OnLbnSelchangeListServices()
{
    // TODO:  在此添加控件通知处理程序代码
    //int b = m_servicesListCtrl.GetCaretIndex();
    int index = m_servicesListCtrl.GetCurSel();
    if (-1 == index)
    {
        return;
    }

    if (!m_sc.EnumServices())
    {
        return ;
    }
    CString displayName;
    m_servicesListCtrl.GetText(index, displayName);
    std::vector<ENUM_SERVICE_STATUS_PROCESS> esspv;
    if (!m_sc.GetInfoByDisplayName(ms::tstring(displayName), esspv))
    {
        return;
    }
    m_displayName = displayName;
    m_serviceName = esspv[0].lpServiceName;
    m_szPath = _T("");
    ResetButtonStatus(esspv[0].ServiceStatusProcess.dwCurrentState);
    UpdateData(FALSE);
}
void CServiceMonitorDlg::ResetButtonStatus(DWORD curState)
{
    UpdateData(TRUE);
    if (m_szPath.IsEmpty())
    {
        m_btInstall.EnableWindow(FALSE);
    }
    else
    {
        m_btInstall.EnableWindow(TRUE);
    }
    switch (curState)
    {
    case 0:
        m_btStop.EnableWindow(FALSE);
        m_btStart.EnableWindow(FALSE);
        m_btRestart.EnableWindow(FALSE);
        m_btRemove.EnableWindow(FALSE);
        break;

    case SERVICE_STOP_PENDING:
    case SERVICE_STOPPED :
        m_btStop.EnableWindow(FALSE);
        m_btStart.EnableWindow(TRUE);
        m_btRestart.EnableWindow(FALSE);
        m_btRemove.EnableWindow(TRUE);
        break;

        
    case SERVICE_START_PENDING:
    case SERVICE_RUNNING :
        m_btStop.EnableWindow(TRUE);
        m_btStart.EnableWindow(FALSE);
        m_btRestart.EnableWindow(TRUE);
        m_btRemove.EnableWindow(TRUE);
        break;

    case SERVICE_CONTINUE_PENDING :
        break;

    case SERVICE_PAUSE_PENDING:
    case SERVICE_PAUSED:

        break;

    default:
        break;
    }


}




void CServiceMonitorDlg::OnBnClickedBtMin()
{
    // TODO:  在此添加控件通知处理程序代码
    ShowWindow(SW_HIDE);
}


void CServiceMonitorDlg::OnBnClickedBtInstall()
{
    // TODO:  在此添加控件通知处理程序代码
    UpdateData(TRUE);
    if (m_sc.ServiceInstall(ms::tstring(m_szPath), ms::tstring(m_displayName), ms::tstring(m_serviceName)))
    {
        m_logListCtrl.AddString(m_displayName + _T("安装成功！"));
    }
    else
    {
        m_logListCtrl.AddString(m_displayName + _T("安装失败！"));
    }
    OnBnClickedBtRefresh();
}


void CServiceMonitorDlg::OnBnClickedBtRestart()
{
    // TODO:  在此添加控件通知处理程序代码
    ResetButtonStatus(0);

    if (ServiceStop())
    {
        int time = 0;
        SERVICE_STATUS status;
        while (m_sc.GetStatus(ms::tstring(m_displayName), status))
        {
            if (SERVICE_STOPPED == status.dwCurrentState)
            {
                ServiceStart();
                //ResetButtonStatus(SERVICE_RUNNING);
                return;
            }
            if (time > 5)
            {
                m_logListCtrl.AddString(m_displayName + _T("重启失败！"));
                ResetButtonStatus(SERVICE_STOPPED);
                return;
            }
            Sleep(1000);
            ++time;
        }
    }
    ResetButtonStatus(SERVICE_RUNNING);

}


void CServiceMonitorDlg::OnBnClickedBtRefresh()
{
    // TODO:  在此添加控件通知处理程序代码
    UpdateData(TRUE);

    m_servicesListCtrl.ResetContent();
    if (m_sc.Open())
    {
        if (m_sc.EnumServices())
        {
            m_esspv.clear();
            if (m_sc.GetInfoByServiceName(ms::tstring(m_serviceName), m_esspv))
            {
                for (ENUM_SERVICE_STATUS_PROCESS& var : m_esspv)
                {
                    m_servicesListCtrl.AddString(var.lpDisplayName);
                }
            }
        }
    }
    ResetButtonStatus(0);
}


void CServiceMonitorDlg::OnBnClickedBtSelect()
{
    // TODO:  在此添加控件通知处理程序代码
    BOOL isOpen = TRUE;     //是否打开(否则为保存)  
    CString defaultDir = m_szPath;   //默认打开的文件路径  
    CString fileName = _T("");         //默认打开的文件名  
    CString filter = _T("文件 (*.exe)|*.exe|文件 (*.*)|*.*||");   //文件过虑的类型  
    CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
    openFileDlg.GetOFN().lpstrInitialDir = defaultDir;
    INT_PTR result = openFileDlg.DoModal();
    if (result == IDOK)
    {
        m_serviceName = openFileDlg.GetFileName();
        int index = m_serviceName.ReverseFind(_T('.'));
        if (-1 != index)
        {
            m_serviceName = m_serviceName.Left(index);
        }
        m_displayName = m_serviceName;
        m_szPath = openFileDlg.GetPathName();

        ms::tstring serviceName = m_serviceName;
        ms::tstring displayName = m_displayName;
        UpdateData(FALSE);
        RefreshWindowsName();
        OnBnClickedBtRefresh();

//         if (m_sc.ServiceInstall(ms::tstring(m_szPath), displayName, serviceName))
//         {
//             m_logListCtrl.AddString(m_serviceName + _T("安装成功！"));
//         }
//         else
//         {
//             m_logListCtrl.AddString(m_serviceName + _T("安装失败！"));
//         }
    }


}
void CServiceMonitorDlg::RefreshWindowsName()
{
    CString appName = m_appHeadName + m_serviceName;
    SetWindowText(appName);
    _tcsncpy_s(m_notifyicon.szTip, appName, appName.GetLength());
    Shell_NotifyIcon(NIM_MODIFY, &m_notifyicon);
}
