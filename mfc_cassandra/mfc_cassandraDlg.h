
#pragma once

#include "../include/cassandra.h"
#include "../include/dse.h" // " when connecting to DataStax Enterpise */

class CmfccassandraDlg : public CDialogEx
{
public:
	CmfccassandraDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_CASSANDRA_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);


protected: 
	CassCluster* m_cluster;
	CassSession* m_session;
	CassFuture* m_Connect_future;
	
public:
	CString m_strIpAddress;
	int m_nPortNumber;
	CString m_strKeySpace;
	CString m_strUserName;
	CString m_strPassword;

	CComboBox m_DriverList;
	CListBox m_ctrlList;
	CListBox m_ctrlTableList;

public:
	CString GetHost();

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();


};
