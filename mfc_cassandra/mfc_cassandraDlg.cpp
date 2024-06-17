
#include <stdio.h>

#include "pch.h"
#include "framework.h"
#include "mfc_cassandra.h"
#include "mfc_cassandraDlg.h"
#include "afxdialogex.h"

#include <string>
#include <vector>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CmfccassandraDlg::CmfccassandraDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_CASSANDRA_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_session = NULL;

	m_strIpAddress = GetHost();
	m_nPortNumber = 9042;
	m_strKeySpace = _T("system");
	m_strUserName = _T("cassandra");
	m_strPassword = _T("cassandra");
	m_strInputQuery = _T("select * from store.shopping_cart;");

}

void CmfccassandraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_DriverList);

	DDX_Text(pDX, IDC_EDIT_IP, m_strIpAddress);
	DDV_MaxChars(pDX, m_strIpAddress, 30);

	DDX_Text(pDX, IDC_EDIT_PORT, m_nPortNumber);
	DDV_MinMaxInt(pDX, m_nPortNumber, 0, 70000);

	DDX_Text(pDX, IDC_EDIT_USER, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, 100);

	DDX_Text(pDX, IDC_EDIT_KEYSPACE, m_strKeySpace);
	DDV_MaxChars(pDX, m_strKeySpace, 100);

	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 30);

	DDX_Text(pDX, IDC_EDIT_SQL_TEXT, m_strInputQuery);

	DDX_Control(pDX, IDC_SCHEMA_LIST, m_ListSchema);
	DDX_Control(pDX, IDC_TABLE_LIST, m_ListTable);
}

BEGIN_MESSAGE_MAP(CmfccassandraDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXAMPLE, &CmfccassandraDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CmfccassandraDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CmfccassandraDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_EXECUTE, &CmfccassandraDlg::OnBnClickedButtonQueryExecute)
END_MESSAGE_MAP()

BOOL CmfccassandraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	// ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			// pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_QUERY_EXECUTE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_SQL_TEXT)->EnableWindow(FALSE);

	return TRUE;
}

void CmfccassandraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	//if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	//{
	//}
	//else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CmfccassandraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CmfccassandraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CmfccassandraDlg::OnBnClickedButton2() // 기본예제
{
	UpdateData(TRUE);

	m_cluster = cass_cluster_new();

	CStringA szIPAddress = (CStringA)m_strIpAddress;
	CStringA szUserName = (CStringA)m_strUserName;
	CStringA szPassword = (CStringA)m_strPassword;
	CStringA szKeySpace = (CStringA)m_strKeySpace;

	cass_cluster_set_contact_points(m_cluster, szIPAddress);						// 1. 호스트를 연결합니다.
	cass_cluster_set_port(m_cluster, m_nPortNumber);								// 2. 포트를 지정합니다.
	cass_cluster_set_credentials(m_cluster, szUserName, szPassword);			// 3. 계정, 비밀번호를 지정합니다.

	m_session = cass_session_new();
	m_Connect_future = cass_session_connect_keyspace(m_session, m_cluster, szKeySpace); // 4. 키스페이스 지정 하여 접속 진행

	// 연결 도중 발생한 에러가 있는지 확인합니다.
	CassError rc = cass_future_error_code(m_Connect_future);

	if (rc != CASS_OK) 
	{
		// 연결 오류 처리
		const char* message;
		size_t message_length;
		cass_future_error_message(m_Connect_future, &message, &message_length);

 		TRACE(_T("접속 실패 관련 메세지  [%s] \n"), message);

		// 접속 실패 관련 메세지  [Underlying connection error: Received error response 'Provided username cassandra and/or password are incorrect' (0x02000100)] 
		// 접속 실패 관련 메세지  [Underlying connection error: Received error response 'Provided username cassandr and/or password are incorrect' (0x02000100)] 

		cass_future_free(m_Connect_future);
		cass_cluster_free(m_cluster);
		cass_session_free(m_session);
		return;
	}




	cass_future_free(m_Connect_future);


	// 데이터 쿼리


	CStringA szQuery = (CStringA)m_strInputQuery;

	CassStatement* statement = cass_statement_new(szQuery, 0);
	CassFuture* result_future = cass_session_execute(m_session, statement);

	// 결과 대기
	rc = cass_future_error_code(result_future);

	if (rc != CASS_OK)
	{
		// 쿼리 오류 처리
		const char* message;
		CStringA sMsg;
		size_t message_length;

		cass_future_error_message(result_future, &message, &message_length);
		rc = cass_future_error_code(result_future);
		sMsg.Format("[ Error Code : %d ]  , Error Message [ %s ] \n", rc, message);
		CString str = (CString)sMsg;


		GetDlgItem(IDC_EDIT_EXECUTE_OUTPUT)->SetWindowText(str);

		TRACE(str);

	}
	else 
	{
		// 결과 가져오기
		const CassResult* result = cass_future_get_result(result_future);
		const CassRow* row;
		CassIterator* iterator = cass_iterator_from_result(result);

		// 결과 반복 및 출력
		int i = 0;
		while (cass_iterator_next(iterator)) 
		{
			row = cass_iterator_get_row(iterator);

			// 각 열의 값 가져오기
			
			const CassValue* column1 = cass_row_get_column(row, 0);
			const CassValue* column2 = cass_row_get_column(row, 1);

			// const CassValue* column1 = cass_row_get_column_by_name(row, "userid");
			// const CassValue* column2 = cass_row_get_column_by_name(row, "item_count");

			const char* value1;
			size_t value1_length;
			cass_value_get_string(column1, &value1, &value1_length);

			int32_t value2;
			cass_value_get_int32(column2, &value2);

			TRACE(_T(" row [%d]  column1:  [%s] , column2: [%d] \n"), i++, value1, value2);
		}

		// 메모리 해제
		cass_result_free(result);
		cass_iterator_free(iterator);
	}

	// 메모리 해제
	cass_statement_free(statement);
	cass_future_free(result_future);









	// 세션 종료 및 클러스터 해제
	CassFuture* close_future = cass_session_close(m_session);
	cass_future_wait(close_future);
	cass_future_free(close_future);

	cass_cluster_free(m_cluster);
	cass_session_free(m_session);

}



CString CmfccassandraDlg::GetHost()
{
	// 레지스트리 키 경로

	// (노트북) 10.10.78.11 , 
	// (집   )  192.168.200.101

	CString sReturn;
	LPCTSTR lpszSubKey = _T("Software\\Warevalley\\Orange");

	// 레지스트리 키를 연다.
	CRegKey regKey;
	LONG lResult = regKey.Open(HKEY_CURRENT_USER, lpszSubKey, KEY_READ);

	TCHAR szValue[256];
	if (lResult == ERROR_SUCCESS)
	{
		// 값을 읽을 버퍼와 길이를 지정한다.
		ULONG nChars = 256;

		// "TestAddress" 값을 읽어온다.
		lResult = regKey.QueryStringValue(_T("TestAddress"), szValue, &nChars);

		if (lResult == ERROR_SUCCESS)
		{
			// 값을 성공적으로 읽어왔다.
		}
		else
			AfxMessageBox(_T("값을 읽는 데 실패했다."));

		// 레지스트리 키를 닫는다.
		regKey.Close();
	}
	else
		AfxMessageBox(_T("레지스트리 키를 여는 데 실패했다."));

	sReturn = szValue;
	return sReturn;
}

void CmfccassandraDlg::OnBnClickedButtonConnect()
{

	UpdateData(TRUE);

	m_cluster = cass_cluster_new();

	CStringA szIPAddress = (CStringA)m_strIpAddress;
	CStringA szUserName = (CStringA)m_strUserName;
	CStringA szPassword = (CStringA)m_strPassword;
	CStringA szKeySpace = (CStringA)m_strKeySpace;

	cass_cluster_set_contact_points(m_cluster, szIPAddress);						// 1. 호스트를 연결합니다.
	cass_cluster_set_port(m_cluster, m_nPortNumber);								// 2. 포트를 지정합니다.
	cass_cluster_set_credentials(m_cluster, szUserName, szPassword);			// 3. 계정, 비밀번호를 지정합니다.

	m_session = cass_session_new();
	m_Connect_future = cass_session_connect_keyspace(m_session, m_cluster, szKeySpace); // 4. 키스페이스 지정 하여 접속 진행

	// 연결 도중 발생한 에러가 있는지 확인합니다.
	CassError rc = cass_future_error_code(m_Connect_future);

	if (rc != CASS_OK)
	{
		// 연결 오류 처리
		const char* message;
		size_t message_length;
		cass_future_error_message(m_Connect_future, &message, &message_length);
		CString sMsg = (CString)message;
		
		AfxMessageBox(sMsg);

		TRACE(_T("접속 실패 관련 메세지  [%s] \n"), sMsg);

		// 접속 실패 관련 메세지  [Underlying connection error: Received error response 'Provided username cassandra and/or password are incorrect' (0x02000100)] 
		// 접속 실패 관련 메세지  [Underlying connection error: Received error response 'Provided username cassandr and/or password are incorrect' (0x02000100)] 

		cass_future_free(m_Connect_future);
		cass_cluster_free(m_cluster);
		cass_session_free(m_session);
		return;
	}
	cass_future_free(m_Connect_future); // 메모리 누수 있으니 반드시 확인바람!


	// 컨트롤 상태 변경 

	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_QUERY_EXECUTE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_IP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_KEYSPACE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_SQL_TEXT)->EnableWindow(TRUE);
}


void CmfccassandraDlg::OnBnClickedButtonDisconnect()
{
	UpdateData(TRUE);
	// 세션 종료 및 클러스터 해제
	CassFuture* close_future = cass_session_close(m_session);
	cass_future_wait(close_future);
	cass_future_free(close_future);

	cass_cluster_free(m_cluster);
	cass_session_free(m_session);


	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_QUERY_EXECUTE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_IP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_KEYSPACE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_SQL_TEXT)->EnableWindow(FALSE);
}


void CmfccassandraDlg::OnBnClickedButtonQueryExecute()
{
	UpdateData(TRUE);

	// 데이터 쿼리

	CStringA szQuery = (CStringA)m_strInputQuery;
	CassStatement* statement = cass_statement_new(szQuery, 0);
	CassFuture* result_future = cass_session_execute(m_session, statement);

	// 결과 대기
	CassError rc = cass_future_error_code(result_future);

	if (rc != CASS_OK)
	{
		// 쿼리 오류 처리
		const char* message;
		CString sMsg;
		size_t message_length;

		cass_future_error_message(result_future, &message, &message_length);
		rc = cass_future_error_code(result_future);
		sMsg.Format(_T("[ Error Code : %d ]  , Error Message [ %s ] \n"), rc, message);

		GetDlgItem(IDC_EDIT_EXECUTE_OUTPUT)->SetWindowText(sMsg);
		
		TRACE(sMsg);

	}
	else
	{
		// 결과 가져오기
		const CassResult* result = cass_future_get_result(result_future);
		const CassRow* row;
		CassIterator* iterator = cass_iterator_from_result(result);

		// 결과 반복 및 출력
		int i = 0;
		while (cass_iterator_next(iterator))
		{
			row = cass_iterator_get_row(iterator);

			// 각 열의 값 가져오기

			const CassValue* column1 = cass_row_get_column(row, 0);
			const CassValue* column2 = cass_row_get_column(row, 1);

			// const CassValue* column1 = cass_row_get_column_by_name(row, "userid");
			// const CassValue* column2 = cass_row_get_column_by_name(row, "item_count");

			const char* value1;
			size_t value1_length;
			cass_value_get_string(column1, &value1, &value1_length);

			int32_t value2;
			cass_value_get_int32(column2, &value2);

			TRACE(_T(" row [%d]  column1:  [%s] , column2: [%d] \n"), i++, value1, value2);
		}

		// 메모리 해제
		cass_result_free(result);
		cass_iterator_free(iterator);
	}

	// 메모리 해제
	cass_statement_free(statement);
	cass_future_free(result_future);

}
