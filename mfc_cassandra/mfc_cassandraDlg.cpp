
#include <stdio.h>

#include "pch.h"
#include "framework.h"
#include "mfc_cassandra.h"
#include "mfc_cassandraDlg.h"
#include "afxdialogex.h"

#include "../include/cassandra.h"
#include "../include/dse.h" // " when connecting to DataStax Enterpise */

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

	m_strIpAddress = GetHost();
	m_strKeySpace = _T("system");
	m_nPortNumber = 9042;
	m_strUsername = _T("cassandra");
	m_strPassword = _T("cassandra");
}

void CmfccassandraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_DriverList);
	DDX_Text(pDX, IDC_EDIT_IP, m_strIpAddress);
	DDV_MaxChars(pDX, m_strIpAddress, 30);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPortNumber);
	DDV_MinMaxInt(pDX, m_nPortNumber, 0, 70000);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUsername);
	DDV_MaxChars(pDX, m_strUsername, 100);
	DDX_Text(pDX, IDC_EDIT_PASS, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 30);
	DDX_Control(pDX, IDC_SCHEMA_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_TABLE_LIST, m_ctrlTableList);
}

BEGIN_MESSAGE_MAP(CmfccassandraDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CmfccassandraDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CmfccassandraDlg::OnBnClickedButton2)
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

	return TRUE;
}

void CmfccassandraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	//if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	//{
	//}
	//else
	//{
	//	CDialogEx::OnSysCommand(nID, lParam);
	//}
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

void CmfccassandraDlg::OnBnClickedButton1()
{
	CassFuture* cass_connect_future = NULL;
	CassCluster* cass_cluster = cass_cluster_new();
	CassSession* cass_session = cass_session_new();


	std::string sHost = GetHost();
	int nPort = 9042;
	std::string sKeyspace = "system";
	std::string sUsername = "cassandra";
	std::string sPassword = "cassandra";

	// 1. 호스트를 연결합니다.
	cass_cluster_set_contact_points(cass_cluster, sHost.c_str());


	// 2. 포트를 지정합니다.
	cass_cluster_set_port(cass_cluster, nPort);


	// 3. 계정, 비밀번호를 지정합니다.
	cass_cluster_set_credentials(cass_cluster, sUsername.c_str(), sPassword.c_str());

	// 4. 실제 접속을 지정합니다. 

	// 4.1 기본 접속 진행 ( 키스페이스 없음 ) 
	// connect_future = cass_session_connect(session, cluster);
	 
	// 4.2 키스페이스 지정 하여 접속 진행
	cass_connect_future = cass_session_connect_keyspace(cass_session, cass_cluster, sKeyspace.c_str());


	// 연결 도중 발생한 에러가 있는지 확인합니다.
	CassError nError = cass_future_error_code(cass_connect_future);

	if (nError == CASS_OK) // 성공
	{
		CassFuture* cass_close_future = NULL;

		/* 쿼리문 작성 및 쿼리 실행 */
		const char* query = "select * from store.shopping_cart;";
		CassStatement* cass_statement = cass_statement_new(query, 0);
		
		CassFuture* cass_result_future = cass_session_execute(cass_session, cass_statement);

		nError = cass_future_error_code(cass_result_future);
		if (nError == CASS_OK)
		{
			/* Retrieve result set and get the first row */
			const CassResult* cass_result = cass_future_get_result(cass_result_future);
			const CassRow* cass_row = cass_result_first_row(cass_result);

			if (cass_row)
			{
				const CassValue* value = cass_row_get_column_by_name(cass_row, "userid");

				size_t nDataLength;
				const char* szRowData;
				nError = cass_value_get_string(value, &szRowData, &nDataLength);



				const char* szMsg;
				size_t nMsgLength;
				cass_future_error_message(cass_result_future, &szMsg, &nMsgLength);
				std::cerr << "Unable to connect: " << std::string(szMsg, nMsgLength) << std::endl;



				printf("release_version: '%.*s'\n", (int)nDataLength, szRowData);


				std::vector<std::string> strVector;

				

				for (auto it : strVector)
					TRACE(_T("release_version: '%d %s'\n", (int)nDataLength, it));
			}

			// 결과 인스턴스를 해제합니다.
			cass_result_free(cass_result);
		}
		else 
		{
			/* Handle error */
			const char* szMsg;
			size_t nMsgLength;
			cass_future_error_message(cass_result_future, &szMsg, &nMsgLength);

			TRACE(_T("Unable to run query: '%.*s'\n", (int)nMsgLength, szMsg));
		}

		cass_statement_free(cass_statement);
		cass_future_free(cass_result_future);

		/* Close the session */
		cass_close_future = cass_session_close(cass_session);
		cass_future_wait(cass_close_future);
		cass_future_free(cass_close_future);
	}
	else 
	{

		// 접속 관련 시도 중 오류가 발생하였습니다.


		const char* szMsg;
		size_t nMsgLength;
		cass_future_error_message(cass_connect_future, &szMsg, &nMsgLength);



		fprintf(stderr, "Unable to connect: '%.*s'\n", (int)nMsgLength, szMsg);
	}

	cass_future_free(cass_connect_future);
	cass_cluster_free(cass_cluster);
	cass_session_free(cass_session);



}


void CmfccassandraDlg::OnBnClickedButton2()
{

	CassFuture* connect_future = NULL;
	CassCluster* cluster = cass_cluster_new();
	CassSession* session = cass_session_new();

	// 컴퓨터\HKEY_CURRENT_USER\Software\Warevalley\Orange
	// TestAddress

	std::string sHost = GetHost();
	int nPort = 9042;
	std::string sKeyspace = "system";
	std::string sUsername = "cassandra";
	std::string sPassword = "cassandra";

	// 1. 호스트를 연결합니다.
	cass_cluster_set_contact_points(cluster, sHost.c_str());


	// 2. 포트를 지정합니다.
	cass_cluster_set_port(cluster, nPort);


	// 3. 계정, 비밀번호를 지정합니다.
	cass_cluster_set_credentials(cluster, sUsername.c_str(), sPassword.c_str());

	// 4. 실제 접속을 지정합니다. 

	// 4.1 기본 접속 진행 ( 키스페이스 없음 ) 
	// connect_future = session_connect(session, cluster);

	// 4.2 키스페이스 지정 하여 접속 진행
	connect_future = cass_session_connect_keyspace(session, cluster, sKeyspace.c_str());


	// 연결 도중 발생한 에러가 있는지 확인합니다.
	CassError nError = cass_future_error_code(connect_future);



	// 연결 대기
	CassError rc = cass_future_error_code(connect_future);



	if (rc != CASS_OK) 
	{
		// 연결 오류 처리
		const char* message;
		size_t message_length;
		cass_future_error_message(connect_future, &message, &message_length);

		std::cerr << "Unable to connect: " << std::string(message, message_length) << std::endl;

		cass_future_free(connect_future);
		cass_cluster_free(cluster);
		cass_session_free(session);
		return;
	}




	cass_future_free(connect_future);

	// 데이터 쿼리
	const char* query = "select * from store.shopping_cart;";
	CassStatement* statement = cass_statement_new(query, 0);
	CassFuture* result_future = cass_session_execute(session, statement);

	// 결과 대기
	rc = cass_future_error_code(result_future);

	if (rc != CASS_OK) 
	{
		// 쿼리 오류 처리
		const char* message;
		size_t message_length;
		cass_future_error_message(result_future, &message, &message_length);
		std::cerr << "Query error: " << std::string(message, message_length) << std::endl;
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
	CassFuture* close_future = cass_session_close(session);
	cass_future_wait(close_future);
	cass_future_free(close_future);

	cass_cluster_free(cluster);
	cass_session_free(session);

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