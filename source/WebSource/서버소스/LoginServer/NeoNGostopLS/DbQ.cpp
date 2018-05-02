#include "DbQ.h"
#include "common.h"

char t_buf [2048];		// for temp query
inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};

CDbQ::CDbQ()
{
//	CREATE_Q(m_Desc, CDescriptorData *, MAX_Q_NUM);
	m_MgrClient = NULL;
	m_Desc = new CUserDescriptor * [MAX_Q_NUM];
	if (!m_Desc) {
		perror ("SYSERR: Cannot create thread queue");
		exit (0);
	}
	m_Head = 0;
	m_Tail = 0;
	m_hThread = 0;
	m_Semaphore = false;
	m_dbEnd = false;
	m_dwLastError = 0;
	
	m_DescNum	= 0;
}


CDbQ::~CDbQ()
{
	SAFE_DELETE_ARRAY(m_Desc);
}

bool CDbQ::IsFull()
{
	int tail = m_Tail+1;

	if (tail >= MAX_Q_NUM)
		tail = 0;
	if (tail == m_Head)
		return true;

	//SAFE_DELETE(m_AdoDB);


	return false;
}

bool CDbQ::IsEmpty()
{
	return (m_Head == m_Tail);
}


bool CDbQ::AddToQ(CUserDescriptor *d)
{
	m_Semaphore = true;
	if (IsFull()) {

		m_Semaphore = false;
		return false; // SYSERR

	}

	d->m_DBQRunning = true;
	m_Desc[m_Tail] = d;

	int tail = m_Tail+1;
	if (tail >= MAX_Q_NUM) {
		tail = 0;
	}

	m_Tail = tail;

	m_Semaphore = false;

	return true;
}

CUserDescriptor* CDbQ::GetFromQ()
{
	if (m_Semaphore) {

		sprintf( a_Log, "(CDbQ::GetFromQ) m_Semaphore : %d 이거때문에 큐에서 가져오는거 실패했다. ", m_Semaphore );
		LogFile (a_Log);
	
		return NULL;
	}

	if (m_Head == m_Tail) {
		return NULL;
	}

	CUserDescriptor *d;
	d = m_Desc[m_Head];

	int head = m_Head+1;
	if (head >= MAX_Q_NUM)
		head = 0;
	m_Head = head;

	return d;
}

void CDbQ::EraseQ (CUserDescriptor *d)
{
	int point = m_Head;
	int count = 0;
	int delete_count = 0;

	//yun ClientSocket을 끊을순간에는 DB Q에 할일이 있으면 다 없애버린다.
	while (point != m_Tail) {
		if (m_Desc[point] == d) {
			m_Desc[point] = NULL;
			delete_count ++;
		}
		point ++;
		if (point >= MAX_Q_NUM) 
			point = 0;
		if (count > MAX_Q_NUM) {
			break;
		}
	}
	if (delete_count > 0) {
		strcpy (t_buf, "QUEUE : Delete");
		IntCat2(t_buf, delete_count);
		LogFile(t_buf);
	}
}

BOOL CDbQ::DBConn()
{	
	m_AdoDB.m_pConnection->PutConnectionTimeout(0);

	if (!m_AdoDB.Open(g_AdoConn.m_AdoConStr)) {
		return FALSE;
	}

	return TRUE;
}

BOOL CDbQ::DBClose()
{
	m_AdoDB.Close();
	return TRUE;
}

bool CDbQ::DBConnCheck()
{
	
	if (!m_AdoDB.IsOpen()) {
		
		strcpy (a_Log, "DBConnect Try.....");
		LogFile (a_Log);	
		
		if ( !DBConn() )
		{
			strcpy (a_Log, "DBConnect Failed!!! Oh....No...");
			LogFile (a_Log);	
			return false;
		}

		strcpy (a_Log, "DBConnect OK.....");
		LogFile (a_Log);	
	}

	return true;
}


void CDbQ::dump_com_error(_com_error &e)
{
	CString ErrorStr;
	CString strLastError;
	CString ConStr;

	ConStr = g_AdoConn.m_AdoConStr;
	
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	ErrorStr.Format( "CADODataBase Error\n\tCode = %08lx\n\tCode meaning = %s\n\tSource = %s\n\tDescription = %s\n",
		e.Error(), e.ErrorMessage(), (LPCSTR)bstrSource, (LPCSTR)bstrDescription );
	strLastError = ("Connection String = " + ConStr + '\n' + ErrorStr);
	m_dwLastError = e.Error(); 
	#ifdef _DEBUG
		AfxMessageBox( ErrorStr, MB_OK | MB_ICONERROR );
	#endif	
}

bool CDbQ::SetDbInit()
{
	if(FAILED(::CoInitialize(NULL)))
		return FALSE;

	if (!DBConnCheck())
	{
		return false;
	}


	Log("*********DB Connect Succeed*********");

	return TRUE;

}

bool CDbQ::IsLoginDbUser( CUserDescriptor *d )
{
	if (!DBConnCheck())
	{
		return false;
	}

 	//DBConn();
	CADORecordset l_AdoRS;

	VariantClear( &m_v_id );
	m_csUserId = d->m_MyInfo.szUser_Id;	  

	m_v_id.vt = VT_BSTR;
	m_v_id.bstrVal  = m_csUserId.AllocSysString();

	try
	{
		m_AdoDB.m_pConnection->CursorLocation = adUseClient;
		l_AdoRS.m_pCmd->ActiveConnection = m_AdoDB.m_pConnection;

		l_AdoRS.m_pCmd->CommandText = "uspG_IsLogin";
		l_AdoRS.m_pCmd->CommandType = adCmdStoredProc;
		l_AdoRS.m_pCmd->Parameters->Append(l_AdoRS.m_pCmd->CreateParameter("@V_id"  ,adVarChar,adParamInput, sizeof(char) * MAX_ID_LENGTH , m_v_id )); 
		l_AdoRS.m_pRecordset = l_AdoRS.m_pCmd->Execute( NULL, NULL, adCmdStoredProc );

		if ( l_AdoRS.IsEOF() ) {

			m_csUserId.ReleaseBuffer();
			DBClose();
			sprintf( a_Log, "(IsLoginDbUser - l_AdoRS.IsEOF() 없는유저)실패 User_Id = %s", d->m_MyInfo.szUser_Id );
			LogFile (a_Log);
			return FALSE;
			
		}

		while(!l_AdoRS.IsEOF())
		{
			if (l_AdoRS.GetFieldValue("islogin", m_vData))
				d->m_nIsLogin = m_vData.intVal;

			l_AdoRS.m_pRecordset->MoveNext();
		}

	}
	catch(_com_error &e)
	{

		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(IsLoginDbUser)실패 User_Id = %s, Error = %s", d->m_MyInfo.szUser_Id, (char *)e.Description() );
		LogFile (a_Log);

		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}
	m_csUserId.ReleaseBuffer();

	DBClose();
	return TRUE;
}



bool CDbQ::NoticeRead()
{
	if (!DBConnCheck())
	{
		return false;
	}

 	//DBConn();
	CADORecordset l_AdoRS;

	try
	{
		m_AdoDB.m_pConnection->CursorLocation = adUseClient;
		l_AdoRS.m_pCmd->ActiveConnection = m_AdoDB.m_pConnection;

		l_AdoRS.m_pCmd->CommandText = "uspG_NoticeRead";
		l_AdoRS.m_pCmd->CommandType = adCmdStoredProc;
		l_AdoRS.m_pRecordset = l_AdoRS.m_pCmd->Execute( NULL, NULL, adCmdStoredProc );

		if ( l_AdoRS.IsEOF() ) {

			m_csUserId.ReleaseBuffer();
			DBClose();
			sprintf( a_Log, "(NoticeRead - l_AdoRS.IsEOF() 없는유저)실패" );
			LogFile (a_Log);
			return FALSE;
			
		}

		while(!l_AdoRS.IsEOF())
		{
			//유저 ID
			if (l_AdoRS.GetFieldValue("notice1", m_vData))
			{
				m_tmpString = m_vData.bstrVal;
				strcpy( g_sNotice.m_Notice1 , m_tmpString );
			}
			
			//유저 ID
			if (l_AdoRS.GetFieldValue("notice2", m_vData))
			{
				m_tmpString = m_vData.bstrVal;
				strcpy( g_sNotice.m_Notice2 , m_tmpString );
			}

			l_AdoRS.m_pRecordset->MoveNext();
		}

	}
	catch(_com_error &e)
	{

		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(NoticeRead)실패 Error = %s", (char *)e.Description() );
		LogFile (a_Log);

		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}
	m_csUserId.ReleaseBuffer();

	DBClose();
	return TRUE;
}


void DbQThread(LPVOID lParam)
{
	CDbQ *pQ = (CDbQ *) lParam;

	ThreadDbFunc(pQ);
}

bool ConnMgrServer (CDbQ *pQ)
{
	struct sockaddr_in msg_socket;


	
	if (pQ->m_MgrClient) {
		if(pQ->m_MgrClient->m_bClosed){ 
			pQ->m_MgrClient->CloseSocket();
			delete [] pQ->m_MgrClient;
			pQ->m_MgrClient = NULL;
		}
		else
			return true;
	}

	pQ->m_MgrClient = new CUserDescriptor [1];
	if ((pQ->m_MgrClient->m_ClientSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		puts ("SYSERR: Error opening network connection : Winsock error");
		return false;
	}

	if (pQ->m_MgrClient->m_ClientSocket <= 0) return false;
	memset (&msg_socket, 0, sizeof(msg_socket));
	msg_socket.sin_family = AF_INET;
	msg_socket.sin_addr.s_addr = inet_addr ( g_ServerInfo.szMgrServerIp );
	msg_socket.sin_port = htons( g_ServerInfo.nMgrPort );

	if (connect (pQ->m_MgrClient->m_ClientSocket, (struct sockaddr *) &msg_socket, sizeof(msg_socket)) < 0) {
		Log ("MgrServer Connecting Failed!!!");
		return false;
	}

	pQ->m_MgrClient->m_Output	= pQ->m_MgrClient->m_SmallOutbuf;
	pQ->m_MgrClient->m_Bufspace = SMALL_BUFSIZE - 1;
	*pQ->m_MgrClient->m_Output = '\0';
	pQ->m_MgrClient->m_Bufptr	= 0;
	pQ->m_MgrClient->m_MyInfo.l_sUserInfo.nUserNo = -1;
	
	int opt = MAX_SOCK_BUF;

	if (setsockopt(pQ->m_MgrClient->m_ClientSocket, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
		puts ("SYSERR: setsockopt SNDBUF");
		return false;
	}

	unsigned long val = 1;
	ioctlsocket(pQ->m_MgrClient->m_ClientSocket, FIONBIO, &val);

	Log ( "ManagerServer Connect Ok!!!");

	return true;
}

void ThreadDbFunc(CDbQ *pQ)
{
	CUserDescriptor *d;
	pQ->m_dbEnd = false;


	int nNoticeReadCnt = NOTICE_READ_TIME;		// 0.1초 가 한바퀴니(최대 0.4초)깐 
	
	
	//서버가 로그인되었다고 디비에 업데이트 한다.
#ifdef DBCONNECT

	if ( !pQ->SetDbInit() )
		Log ("DB Start Failed!!! Oh! My god. Crazy SetDbInit");
#endif


	while (true) {
#ifndef _DEBUG
		while (true) {	
			if (ConnMgrServer (pQ))
				break;
			if (pQ->m_MgrClient)
				pQ->m_MgrClient->m_bClosed = true;
			Sleep (1000);
		}	
#endif
		Sleep(100);
		

		nNoticeReadCnt++;

		if ( nNoticeReadCnt > NOTICE_READ_TIME )
		{
			pQ->NoticeRead();
			nNoticeReadCnt = 0;
		}

		if (pQ->IsEmpty ()) {
			Sleep (300);			//0.1초 대기
			if ( g_DbEnd == true ) break;
		}

		d = pQ->GetFromQ();

		if (d) {
			if (d->m_QuitSave) continue;
			
			switch (STATE(d)) {

				case DBSEARCH_ISLOGIN:
					LogFile("(DB_PROCESS DBSEARCH_ISLOGIN)로그인 처리시작");			
	
					if ( !pQ->IsLoginDbUser(d) ) {
						WAITTING_DB(d) = false;
						ACCESSED_DB(d) = false;				

						LogFile("(DB_PROCESS DBSEARCH_ISLOGIN)로그인 처리실패");

					}
					else {
						WAITTING_DB(d) = false;
						ACCESSED_DB(d) = true;					

						LogFile("(DB_PROCESS DBSEARCH_ISLOGIN)로그인 처리성공");
					}
		
					break;

			}
			d->m_DBQRunning = false;
		}
	}
	pQ->m_dbEnd = true;
}
