#include "DbQ.h"
#include "common.h"

#include "dtrace.h"

char t_buf [2048];		// for temp query
inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};


#include "Manager.h"
extern CManager *l_pCManager;

CDbQ::CDbQ()
{
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

	SetServerIsLogin();

	return TRUE;

}

bool CDbQ::SetServerIsLogin()
{
#ifndef DBCONNECT
	return true;
#endif

	if (!DBConnCheck())
	{
		return false;
	}
	
	_CommandPtr   pCmd = NULL;

	VariantClear( &m_v_DbServerName );
	
	m_csDbServerName = g_ServerInfo.szDbServerName;

	m_v_DbServerName.vt = VT_BSTR;
	m_v_DbServerName.bstrVal  = m_csDbServerName.AllocSysString();

	m_v_GameChannel.vt = VT_I2;
	m_v_GameChannel.iVal = g_ServerInfo.szServerChannel;

	try
	{
		pCmd.CreateInstance(__uuidof(Command));
		pCmd->ActiveConnection = m_AdoDB.m_pConnection;
		pCmd->CommandText = "uspG_SetServerLogin";
		pCmd->CommandType  = adCmdStoredProc;


		pCmd->Parameters->Append(pCmd->CreateParameter("@V_DbServerName"  ,adVarChar,adParamInput,sizeof(char) * 20,m_v_DbServerName)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_GameChannel"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_GameChannel)); 

		m_hr = pCmd->Execute(NULL,NULL,adCmdStoredProc);
		
		if(SUCCEEDED(m_hr))
		{
			pCmd.Release();

			DBClose();
			return true;    
		}    
	}
	catch(_com_error &e)
	{
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(SetServerIsLogin) Error = %s", (char*)e.Description() );
		LogFile (a_Log);

		pCmd.Release();

		DBClose();
		return FALSE;
	}

	pCmd.Release();

	DBClose();
	return TRUE;



}

bool CDbQ::GetUserInfo( CUserDescriptor *d )
{
	if (!DBConnCheck())
	{
		return false;
	}

	CADORecordset l_AdoRS;
	BigInt biUserMoney;

	VariantClear( &m_v_id );
	VariantClear( &m_v_DbServerName );

	m_csUserId = d->m_MyInfo.szUser_Id;	  
	m_csDbServerName = g_ServerInfo.szDbServerName;
	
	m_v_id.vt = VT_BSTR;
	m_v_id.bstrVal  = m_csUserId.AllocSysString();

	m_v_DbServerName.vt = VT_BSTR;
	m_v_DbServerName.bstrVal  = m_csDbServerName.AllocSysString();

	m_v_GameChannel.vt = VT_I2;
	m_v_GameChannel.iVal = d->m_MyInfo.l_sUserInfo.nChannel;


	try
	{
		m_AdoDB.m_pConnection->CursorLocation = adUseClient;
		l_AdoRS.m_pCmd->ActiveConnection = m_AdoDB.m_pConnection;

		l_AdoRS.m_pCmd->CommandText = "uspG_NewMatgoLogin";
		l_AdoRS.m_pCmd->CommandType = adCmdStoredProc;
		l_AdoRS.m_pCmd->Parameters->Append(l_AdoRS.m_pCmd->CreateParameter("@V_id"  ,adVarChar,adParamInput, sizeof(char) * MAX_ID_LENGTH , m_v_id)); 
		l_AdoRS.m_pCmd->Parameters->Append(l_AdoRS.m_pCmd->CreateParameter("@V_DbServerName"  ,adVarChar,adParamInput, sizeof(char) * 20, m_v_DbServerName)); 
		l_AdoRS.m_pCmd->Parameters->Append(l_AdoRS.m_pCmd->CreateParameter("@V_GameChannel"  ,adInteger,adParamInput, sizeof(VARIANT), m_v_GameChannel)); 


		l_AdoRS.m_pRecordset = l_AdoRS.m_pCmd->Execute( NULL, NULL, adCmdStoredProc );

		if ( l_AdoRS.IsEOF() ) {

			m_csUserId.ReleaseBuffer();
			DBClose();
			sprintf( a_Log, "(MatgoLoginDbUser - l_AdoRS.IsEOF() 없는유저)실패 User_Id = %s", d->m_MyInfo.szUser_Id );
			LogFile (a_Log);	
			return FALSE;
			
		}

		while(!l_AdoRS.IsEOF())
		{
			//유저 ID
			if (l_AdoRS.GetFieldValue("nick", m_vData))
			{
				m_tmpString = m_vData.bstrVal;
				strcpy( d->m_MyInfo.l_sUserInfo.szNick_Name , m_tmpString );
			}

			//유저머니.
			if (l_AdoRS.GetFieldValue("goMoney", biUserMoney))
				d->m_MyInfo.l_sUserInfo.biUserMoney = biUserMoney;


			//계급
			if (l_AdoRS.GetFieldValue("class", m_vData))
				d->m_MyInfo.l_sUserInfo.nClass = m_vData.intVal;	
			
			//최고머니.
			if (l_AdoRS.GetFieldValue("maxMoney", biUserMoney))
				d->m_MyInfo.l_sUserInfo.biMaxMoney = biUserMoney;
			
			//승률
			if (l_AdoRS.GetFieldValue("winRate", m_vData))
				d->m_MyInfo.l_sUserInfo.fWinRate = m_vData.dblVal;

			
			//성별
			if (l_AdoRS.GetFieldValue("sex", m_vData))
			{
				m_tmpString.ReleaseBuffer();
				m_tmpString = m_vData.bstrVal;
				d->m_MyInfo.l_sUserInfo.cSex = m_tmpString.GetAt(0);
			}

			//승수
			if (l_AdoRS.GetFieldValue("win_num", m_vData))
				d->m_MyInfo.l_sUserInfo.nWin_Num = m_vData.intVal;

			//패수
			if (l_AdoRS.GetFieldValue("fail_num", m_vData))
				d->m_MyInfo.l_sUserInfo.nLose_Num = m_vData.intVal;

			//비김
			if (l_AdoRS.GetFieldValue("draw_num", m_vData))
				d->m_MyInfo.l_sUserInfo.nDraw_Num = m_vData.intVal;

			//끊김수.
			if (l_AdoRS.GetFieldValue("cutTime", m_vData))
				d->m_MyInfo.l_sUserInfo.nCut_Time = m_vData.intVal;

			if (l_AdoRS.GetFieldValue("islogin", m_vData))
				d->m_nIsLogIn = m_vData.intVal;

			l_AdoRS.m_pRecordset->MoveNext();
		}

	}
	catch(_com_error &e)
	{

		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(MatgoLoginDbUser)실패 User_Id = %s, Error = %s", d->m_MyInfo.szUser_Id, (char *)e.Description() );
		LogFile (a_Log);

		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}
	m_csUserId.ReleaseBuffer();

	DBClose();
	return TRUE;

}

bool CDbQ::NewGameResultUpdate( CUserDescriptor *d )
{

#ifndef DBCONNECT
	return true;
#endif

	if (!DBConnCheck())
	{
		return false;
	}
	
	_CommandPtr   pCmd = NULL;
	DECIMAL dec;
	
	VariantClear( &m_v_id );
	VariantClear( &m_v_ObtainMoney );
	VariantClear( &m_v_LoseMoney );
	VariantClear( &m_v_WinLose );
	VariantClear( &m_v_RoomMoney );
	VariantClear( &m_v_OtherNick1 );
	VariantClear( &m_v_OtherNick2 );
	VariantClear( &m_v_PrizeNo );
	VariantClear( &m_v_SucceedCode );
	VariantClear( &m_v_WinNick );

	
	m_csUserId = d->m_MyInfo.szUser_Id;

	m_csStringObtainMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biObtainMoney );
	m_csStringLoseMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biLoseMoney );
	m_csStringRoomMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biRoomMoney );
	m_csSystemMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biSystemMoney );

	m_csOtherNick1 = d->m_sUserDBUpdateInfo.szOtherGameUser[0];

	try
	{
		pCmd.CreateInstance(__uuidof(Command));
		pCmd->ActiveConnection = m_AdoDB.m_pConnection;
		pCmd->CommandText = "uspG_NewMatGo_Result";
		pCmd->CommandType  = adCmdStoredProc;

		m_v_id.vt = VT_BSTR;
		m_v_id.bstrVal  = m_csUserId.AllocSysString();

		m_v_ObtainMoney.vt = VT_BSTR;
		m_v_ObtainMoney.bstrVal = m_csStringObtainMoney.AllocSysString();

		m_v_LoseMoney.vt = VT_BSTR;
		m_v_LoseMoney.bstrVal = m_csStringLoseMoney.AllocSysString();

		m_v_WinLose.vt = VT_I2;
		m_v_WinLose.iVal = d->m_sUserDBUpdateInfo.nWinLose;

		m_v_RoomMoney.vt = VT_BSTR;
		m_v_RoomMoney.bstrVal = m_csStringRoomMoney.AllocSysString();

		m_v_OtherNick1.vt = VT_BSTR;
		m_v_OtherNick1.bstrVal  = m_csOtherNick1.AllocSysString();

		m_v_SystemMoney.vt = VT_BSTR;
		m_v_SystemMoney.bstrVal  = m_csSystemMoney.AllocSysString();

		m_v_PrizeNo.vt = VT_I2;
		m_v_PrizeNo.iVal = d->m_nPrizeNo;
		m_v_SucceedCode.vt = VT_I2;
		m_v_SucceedCode.iVal = d->m_nSucceedCode;

		
		DTRACE("UserUpdate : %s, ObtainMoney : %I64u, LoseMoney : %I64u, nWinLose : %d, RoomMoney : %I64u, SystemMoney = %I64u",
			d->m_MyInfo.l_sUserInfo.szNick_Name, d->m_sUserDBUpdateInfo.biObtainMoney,
			d->m_sUserDBUpdateInfo.biLoseMoney,
			d->m_sUserDBUpdateInfo.nWinLose,
			d->m_sUserDBUpdateInfo.biRoomMoney, d->m_sUserDBUpdateInfo.biSystemMoney );

		sprintf( a_Log, "UserUpdate : %s, ObtainMoney : %I64u, LoseMoney : %I64u, nWinLose : %d, RoomMoney : %I64u, OtherNick1 = %s, SystemMoney = %I64u, PrizeNo = %d, SucceedCode = %d ",
			d->m_MyInfo.l_sUserInfo.szNick_Name, 
			d->m_sUserDBUpdateInfo.biObtainMoney,
			d->m_sUserDBUpdateInfo.biLoseMoney,
			d->m_sUserDBUpdateInfo.nWinLose,
			d->m_sUserDBUpdateInfo.biRoomMoney, 
			d->m_sUserDBUpdateInfo.szOtherGameUser[0],
			d->m_sUserDBUpdateInfo.biSystemMoney,
			d->m_nPrizeNo,
			d->m_nSucceedCode );
		LogFile (a_Log);


			
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_id"  ,adVarChar,adParamInput,sizeof(char) * MAX_ID_LENGTH,m_v_id)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_ObtainMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_ObtainMoney));
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_LoseMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_LoseMoney));
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_WinLose"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_WinLose)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_RoomMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_RoomMoney)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_OtherNick1"  ,adVarChar,adParamInput,sizeof(char) * MAX_NICK_LENGTH,m_v_OtherNick1)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_SystemMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_SystemMoney)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_PrizeNo"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_PrizeNo)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_SucceedCode"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_SucceedCode)); 

		m_hr = pCmd->Execute(NULL,NULL,adCmdStoredProc);

		memset( &d->m_sUserDBUpdateInfo, 0x00, sizeof(d->m_sUserDBUpdateInfo) );

		if(SUCCEEDED(m_hr))
		{
			pCmd.Release();
			m_csUserId.ReleaseBuffer();

			DBClose();

			DTRACE( "(NewGameResultUpdate)성공 User_Id = %s", d->m_MyInfo.szUser_Id );
		
			return true;    
		}    
	}
	catch(_com_error &e)
	{
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(NewGameResultUpdate)실패 User_Id = %s, Error = %s", d->m_MyInfo.szUser_Id, (char*)e.Description() );
		LogFile (a_Log);

		pCmd.Release();
		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}

	pCmd.Release();
	m_csUserId.ReleaseBuffer();
	
	DBClose();
	return TRUE;	
}

bool CDbQ::GameResultUpdate( CUserDescriptor *d )
{
	
#ifndef DBCONNECT
	return true;
#endif

	if (!DBConnCheck())
	{
		return false;
	}
	
	_CommandPtr   pCmd = NULL;
	DECIMAL dec;

	
	VariantClear( &m_v_id );
	VariantClear( &m_v_ObtainMoney );
	VariantClear( &m_v_LoseMoney );
	VariantClear( &m_v_WinLose );
	VariantClear( &m_v_RoomMoney );
	VariantClear( &m_v_OtherNick1 );
	VariantClear( &m_v_OtherNick2 );
	VariantClear( &m_v_PrizeNo );
	VariantClear( &m_v_SucceedCode );
	
	m_csUserId = d->m_MyInfo.szUser_Id;

	m_csStringObtainMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biObtainMoney );
	m_csStringLoseMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biLoseMoney );
	m_csStringRoomMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biRoomMoney );
	m_csSystemMoney.Format( "%I64u", d->m_sUserDBUpdateInfo.biSystemMoney );

	m_csOtherNick1 = d->m_sUserDBUpdateInfo.szOtherGameUser[0];

	try
	{
		pCmd.CreateInstance(__uuidof(Command));
		pCmd->ActiveConnection = m_AdoDB.m_pConnection;

		pCmd->CommandText = "uspG_MatgoResult";

		pCmd->CommandType  = adCmdStoredProc;

		m_v_id.vt = VT_BSTR;
		m_v_id.bstrVal  = m_csUserId.AllocSysString();

		m_v_ObtainMoney.vt = VT_BSTR;
		m_v_ObtainMoney.bstrVal = m_csStringObtainMoney.AllocSysString();

		m_v_LoseMoney.vt = VT_BSTR;
		m_v_LoseMoney.bstrVal = m_csStringLoseMoney.AllocSysString();

		m_v_WinLose.vt = VT_I2;
		m_v_WinLose.iVal = d->m_sUserDBUpdateInfo.nWinLose;

		m_v_RoomMoney.vt = VT_BSTR;
		m_v_RoomMoney.bstrVal = m_csStringRoomMoney.AllocSysString();

		m_v_OtherNick1.vt = VT_BSTR;
		m_v_OtherNick1.bstrVal  = m_csOtherNick1.AllocSysString();


		m_v_SystemMoney.vt = VT_BSTR;
		m_v_SystemMoney.bstrVal  = m_csSystemMoney.AllocSysString();

		m_v_PrizeNo.vt = VT_I2;
		m_v_PrizeNo.iVal = d->m_nPrizeNo;
		m_v_SucceedCode.vt = VT_I2;
		m_v_SucceedCode.iVal = d->m_nSucceedCode;

		
		DTRACE("UserUpdate : %s, ObtainMoney : %I64u, LoseMoney : %I64u, nWinLose : %d, RoomMoney : %I64u, SystemMoney = %I64u",
			d->m_MyInfo.l_sUserInfo.szNick_Name, d->m_sUserDBUpdateInfo.biObtainMoney,
			d->m_sUserDBUpdateInfo.biLoseMoney,
			d->m_sUserDBUpdateInfo.nWinLose,
			d->m_sUserDBUpdateInfo.biRoomMoney, d->m_sUserDBUpdateInfo.biSystemMoney );
		
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_id"  ,adVarChar,adParamInput,sizeof(char) * MAX_ID_LENGTH,m_v_id)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_ObtainMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_ObtainMoney));
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_LoseMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_LoseMoney));
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_WinLose"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_WinLose)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_RoomMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_RoomMoney)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_OtherNick1"  ,adVarChar,adParamInput,sizeof(char) * MAX_NICK_LENGTH,m_v_OtherNick1)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_SystemMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_SystemMoney)); 

		m_hr = pCmd->Execute(NULL,NULL,adCmdStoredProc);
		

		memset( &d->m_sUserDBUpdateInfo, 0x00, sizeof(d->m_sUserDBUpdateInfo) );

		if(SUCCEEDED(m_hr))
		{
			pCmd.Release();
			m_csUserId.ReleaseBuffer();

			DBClose();

			DTRACE( "(GameResultUpdate)성공 User_Id = %s", d->m_MyInfo.szUser_Id );
		
			return true;    
		}    
	}
	catch(_com_error &e)
	{
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(GameResultUpdate)실패 User_Id = %s, Error = %s", d->m_MyInfo.szUser_Id, (char*)e.Description() );
		LogFile (a_Log);

		pCmd.Release();
		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}

	pCmd.Release();
	m_csUserId.ReleaseBuffer();
	
	DBClose();
	return TRUE;	
}

bool CDbQ::BugsyGameResultUpdate( CUserDescriptor *d )
{

#ifndef DBCONNECT
	return true;
#endif

	if (!DBConnCheck())
	{
		return false;
	}
	
	_CommandPtr   pCmd = NULL;

	VariantClear( &m_v_id );
	VariantClear( &m_v_GoMoney );
	
	VariantClear( &m_v_MaxMoney );
	VariantClear( &m_v_WinNum );
	VariantClear( &m_v_FailNum );
	VariantClear( &m_v_DrawNum );
	VariantClear( &m_v_CutNum );
	VariantClear( &m_v_WinRate );
	VariantClear( &m_v_IsLogin );
	VariantClear( &m_v_SiteCode );
	VariantClear( &m_v_PrizeNo );
	VariantClear( &m_v_SucceedCode );

	VariantClear( &m_v_OtherNickName1 );
	VariantClear( &m_v_OtherNickName2 );
	VariantClear( &m_v_ToriZisu );
	VariantClear( &m_v_GameRoomMoney );
	VariantClear( &m_v_SystemMoney );


	m_csUserId = d->m_MyInfo.szUser_Id;
	m_csSiteCode = d->szSiteCode;


	m_csStringUserMoney.Format( "%I64u", d->m_MyInfo.l_sUserInfo.biUserMoney );
	m_csMaxMoney.Format( "%I64u", d->m_MyInfo.l_sUserInfo.biMaxMoney );
	m_csStringRoomMoney.Format( "%I64u", d->m_biGameRoomMoney );
	m_csSystemMoney.Format( "%I64u", d->m_biSystemMoney );



	try
	{
		pCmd.CreateInstance(__uuidof(Command));
		pCmd->ActiveConnection = m_AdoDB.m_pConnection;
		pCmd->CommandText = "uspG_MatgoResult";
		pCmd->CommandType  = adCmdStoredProc;

		m_v_id.vt = VT_BSTR;
		m_v_id.bstrVal  = m_csUserId.AllocSysString();

		m_v_SiteCode.vt = VT_BSTR;
		m_v_SiteCode.bstrVal  = m_csSiteCode.AllocSysString();

		m_csOtherNick1 = d->m_szOtherNick_Name[0];
		m_v_OtherNickName1.vt = VT_BSTR;
		m_v_OtherNickName1.bstrVal = m_csOtherNick1.AllocSysString();
		
		m_csOtherNick2 = d->m_szOtherNick_Name[1];
		m_v_OtherNickName2.vt = VT_BSTR;
		m_v_OtherNickName2.bstrVal = m_csOtherNick2.AllocSysString();

		m_v_GoMoney.vt = VT_BSTR;
		m_v_GoMoney.bstrVal = m_csStringUserMoney.AllocSysString();

		m_v_MaxMoney.vt = VT_BSTR;
		m_v_MaxMoney.bstrVal = m_csMaxMoney.AllocSysString();

		m_v_IsLogin.vt = VT_I2;
		m_v_IsLogin.iVal = d->m_nIsLogIn;
		

		m_v_WinNum.vt = VT_I2;
		m_v_WinNum.iVal = d->m_MyInfo.l_sUserInfo.nWin_Num;
		m_v_FailNum.vt = VT_I2;
		m_v_FailNum.iVal = d->m_MyInfo.l_sUserInfo.nLose_Num;
		m_v_DrawNum.vt = VT_I2;
		m_v_DrawNum.iVal = d->m_MyInfo.l_sUserInfo.nDraw_Num;
		m_v_CutNum.vt = VT_I2;
		m_v_CutNum.iVal = d->m_MyInfo.l_sUserInfo.nCut_Time;

		m_v_WinRate.vt = VT_R4;
		m_v_WinRate.fltVal = d->m_MyInfo.l_sUserInfo.fWinRate;


		m_v_PrizeNo.vt = VT_I2;
		m_v_PrizeNo.iVal = d->m_nPrizeNo;
		m_v_SucceedCode.vt = VT_I2;
		m_v_SucceedCode.iVal = d->m_nSucceedCode;


		m_v_ToriZisu.vt = VT_I2;
		m_v_ToriZisu.iVal = d->m_nToriZisu;

		m_v_GameRoomMoney.vt = VT_BSTR;
		m_v_GameRoomMoney.bstrVal = m_csStringRoomMoney.AllocSysString();


		m_v_SystemMoney.vt = VT_BSTR;
		m_v_SystemMoney.bstrVal = m_csSystemMoney.AllocSysString();


		d->m_nSucceedCode = 0;
		d->m_nToriZisu = 0;

		d->m_biGameRoomMoney = 0;

		pCmd->Parameters->Append(pCmd->CreateParameter("@V_id"  ,adVarChar,adParamInput,sizeof(char) * MAX_ID_LENGTH,m_v_id)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_GoMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_GoMoney));
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_MaxMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_MaxMoney));
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_WinNum"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_WinNum)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_FailNum"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_FailNum)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_DrawNum"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_DrawNum)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_CutNum"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_CutNum)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_WinRate"  ,adCurrency,adParamInput,sizeof(VARIANT),m_v_WinRate)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_IsLogin"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_IsLogin)); 


		pCmd->Parameters->Append(pCmd->CreateParameter("@V_SiteCode"  ,adVarChar,adParamInput,sizeof(VARIANT),m_v_SiteCode)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_PrizeNo"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_PrizeNo)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_SucceedCode"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_SucceedCode)); 

		pCmd->Parameters->Append(pCmd->CreateParameter("@V_OtherNick1"  ,adVarChar,adParamInput,sizeof(char) * MAX_NICK_LENGTH,m_v_OtherNickName1)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_OtherNick2"  ,adVarChar,adParamInput,sizeof(char) * MAX_NICK_LENGTH,m_v_OtherNickName2)); 
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_ToriZisu"  ,adInteger,adParamInput,sizeof(VARIANT),m_v_ToriZisu)); 

		pCmd->Parameters->Append(pCmd->CreateParameter("@V_GameRoomMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_GameRoomMoney));		
		pCmd->Parameters->Append(pCmd->CreateParameter("@V_SystemMoney"  ,adVarChar,adParamInput,sizeof(char) * MAX_GAMESTRMONEYLEN,m_v_SystemMoney));		

		m_hr = pCmd->Execute(NULL,NULL,adCmdStoredProc);
		



		if(SUCCEEDED(m_hr))
		{
			pCmd.Release();
			m_csUserId.ReleaseBuffer();

			DBClose();
			return true;    
		}    
	}
	catch(_com_error &e)
	{
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(BugsyGameResultUpdate)실패 User_Id = %s, Error = %s", d->m_MyInfo.szUser_Id, (char*)e.Description() );
		LogFile (a_Log);

		pCmd.Release();
		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}

	pCmd.Release();
	m_csUserId.ReleaseBuffer();
	
	DBClose();
	return TRUE;
}


bool CDbQ::CSDBOut( CUserDescriptor *d )
{
	if (!DBConnCheck())
	{
		return false;
	}
	
	_CommandPtr   pCmd = NULL;

	VariantClear( &m_v_id );

	m_csUserId = d->m_MyInfo.szUser_Id;

	try
	{
		pCmd.CreateInstance(__uuidof(Command));
		pCmd->ActiveConnection = m_AdoDB.m_pConnection;
		pCmd->CommandText = "uspG_DbOut";
		pCmd->CommandType  = adCmdStoredProc;

		m_v_id.vt = VT_BSTR;
		m_v_id.bstrVal  = m_csUserId.AllocSysString();

		pCmd->Parameters->Append(pCmd->CreateParameter("@V_id"  ,adVarChar,adParamInput,sizeof(char) * MAX_ID_LENGTH,m_v_id)); 

		m_hr = pCmd->Execute(NULL,NULL,adCmdStoredProc);
		
		if(SUCCEEDED(m_hr))
		{
			pCmd.Release();
			m_csUserId.ReleaseBuffer();


			DBClose();
			return true;    
		}    
	}
	catch(_com_error &e)
	{
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		sprintf( a_Log, "(CSDBOut)실패 User_Id = %s, Error = %s", d->m_MyInfo.szUser_Id, (char*)e.Description() );
		LogFile (a_Log);


		pCmd.Release();
		m_csUserId.ReleaseBuffer();

		DBClose();
		return FALSE;
	}

	pCmd.Release();
	m_csUserId.ReleaseBuffer();

	DBClose();
	return TRUE;

}

//****************************************************************************/


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
	CUserDescriptor *temp;
	CUserDescriptor *d;
	pQ->m_dbEnd = false;

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
		
		if (pQ->IsEmpty ()) {
			Sleep (300);		
			if ( g_DbEnd == true ) break;
		}

		d = pQ->GetFromQ();

		if (d) {
			if (d->m_QuitSave) continue;
			
			switch (STATEDBMODE(d)) {

				case GETUSER:
					sprintf( a_Log, "(DB_PROCESS GETUSERINFO)DB Update - ID : %s", d->m_MyInfo.szUser_Id);
					LogFile (a_Log);

					if ( !pQ->GetUserInfo(d) ) {
						WAITTING_DB(d) = false;
						ACCESSED_DB(d) = false;				

						sprintf( a_Log, "(DB_PROCESS GETUSERINFO) Game Update Fail - ID : %s", d->m_MyInfo.szUser_Id);
						LogFile (a_Log);

					}
					else {
						STATEDBMODE(d) = NONE;
						WAITTING_DB(d) = false;
						ACCESSED_DB(d) = true;					

						sprintf( a_Log, "(DB_PROCESS GETUSERINFO) Game Update Success - ID : %s", d->m_MyInfo.szUser_Id);
						LogFile (a_Log);
					}

					break;	

				case GAMING_EXITUPDATE:

					sprintf( a_Log, "(DB_PROCESS LogOut - GAMING_EXITUPDATE)DB Update시작 - ID : %s", d->m_MyInfo.szUser_Id);
					LogFile (a_Log);

#ifdef DBCONNECT
					d->m_nIsLogIn = 0;
					pQ->NewGameResultUpdate(d);
#endif
					
					pQ->CSDBOut(d);
					pQ->EraseQ (d);	// erase Descriptor from queue.	

					REMOVE_FROM_LIST(d, l_pCManager->m_UserDescriptorList, m_pNext);
					SAFE_DELETE_ARRAY(d);	
					
					sprintf( a_Log, "(DB_PROCESS LogOut - GAMING_EXITUPDATE)DB Update끝");
					LogFile (a_Log);

					break;


				case GAME_RESULT_UPDATE:
					sprintf( a_Log, "(DB_PROCESS GAME_RESULT_UPDATE)DB Update - ID : %s", d->m_MyInfo.szUser_Id);
					LogFile (a_Log);

					if ( !pQ->NewGameResultUpdate(d) ) {

						ACCESSED_DB(d) = false;				

						sprintf( a_Log, "(DB_PROCESS GAME_RESULT_UPDATE) Game Update Fail - ID : %s", d->m_MyInfo.szUser_Id);
						LogFile (a_Log);

					}
					else {

						STATEDBMODE(d) = NONE;
						ACCESSED_DB(d) = true;					

						sprintf( a_Log, "(DB_PROCESS GAME_RESULT_UPDATE) Game Update Success - ID : %s", d->m_MyInfo.szUser_Id);
						LogFile (a_Log);
					}

					break;			

				case CLIENT_WILL_DISCONNECT:

					sprintf( a_Log, "(DB_PROCESS LogOut - CLIENT_WILL_DISCONNECT)DB Update시작 - ID : %s", d->m_MyInfo.szUser_Id);
					LogFile (a_Log);

#ifdef DBCONNECT
					d->m_nIsLogIn = 0;


					pQ->CSDBOut(d);
#endif

					pQ->EraseQ (d);	// erase Descriptor from queue.

					d->m_QuitSave = true;
					d->m_bPlay = false;
					
					WAITTING_DB(d) = false;
					ACCESSED_DB(d) = true;	

					STATEDBMODE(d) = NONE;

					
					STATE(d) = CLIENT_DISCONNECTED;					


					sprintf( a_Log, "(DB_PROCESS LogOut - CLIENT_WILL_DISCONNECT)DB Update끝 - ID : %s", d->m_MyInfo.szUser_Id);
					LogFile (a_Log);

					REMOVE_FROM_LIST(d, l_pCManager->m_UserDescriptorList, m_pNext);
					SAFE_DELETE_ARRAY(d);
					


					break;

			}

			if (d)
				d->m_DBQRunning = false;
		}
	}
	pQ->m_dbEnd = true;
}

