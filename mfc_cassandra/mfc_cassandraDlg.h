
#pragma once

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
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


public:
	CComboBox m_DriverList;
	int m_nDriverListLength;
	CString m_strIpAddress;
	int m_nPortNumber;
	CString m_strKeySpace;
	CString m_strUsername;
	CString m_strPassword;
	CString m_strSelectedDriver = _T("");
	CString m_strDBInfos;
	BOOL m_bIsClicked = false;
	CString m_sSelectedSchema = _T("");
	CListBox m_ctrlList;
	CListBox m_ctrlTableList;
};
