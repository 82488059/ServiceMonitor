
// ServiceMonitorDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <ms/service/sc.h>
#define WM_TRAYNOTIFY WM_USER+1024

// CServiceMonitorDlg �Ի���
class CServiceMonitorDlg : public CDialogEx
{
// ����
public:
	CServiceMonitorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERVICEMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    NOTIFYICONDATA m_notifyicon;
    CListBox m_servicesListCtrl;
    ms::service::sc m_sc;
    std::vector<ENUM_SERVICE_STATUS_PROCESS> m_esspv;
    //ms::tstring m_displayName;
    //ms::tstring m_serviceName;

    void ResetButtonStatus(DWORD curState);
    bool ServiceStart();
    bool ServiceStop();
    void RefreshWindowsName();
    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBtStart();
    afx_msg void OnBnClickedBtStop();
    afx_msg void OnLbnSelchangeListServices();

    CButton m_btStart;
    CButton m_btStop;
    afx_msg void OnBnClickedBtRemove();
    afx_msg void OnBnClickedBtMin();
    CListBox m_logListCtrl;
    CButton m_btRemove;
    afx_msg void OnBnClickedBtInstall();
    CString m_szPath;
    afx_msg void OnBnClickedBtRestart();
    CButton m_btRestart;
    afx_msg void OnBnClickedBtRefresh();
    afx_msg void OnBnClickedBtSelect();
    CString m_serviceName;
    CString m_displayName;
    CString m_appHeadName;
    CButton m_btInstall;
    afx_msg void OnBnClickedBtFind();
    CString m_findName;
};
