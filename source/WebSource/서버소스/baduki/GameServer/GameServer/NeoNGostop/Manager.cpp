#include "Common.h"
//#include "Comfunc.h"
#include "Manager.h"
//#include "TxTQ.h"
#include "World.h"
#include "CmdProcess.h"
#include "NGFrame.h"

#include "dtrace.h"

char *g_ServerPath;
class CDescriptorData;

CTxtBlock *bufpool = NULL;	/* pool of large output buffers */
int buf_overflows = 0;		/* # of overflows of output */
int buf_switches = 0;		/* # of switches from small to large buf */
int buf_largecount = 0;		/* # of large buffers which exist */

char a_pBuffer[MAX_STRING_LENGTH];

char cFinalError = NULL;

World g_World;

bool g_bSndMsg = false;
bool g_DbEnd = false;

struct timeval a_NullTime;	/* zero-valued time structure */
 
CManager *l_pCManager = NULL;


CDbQ *g_pDbQ;

char g_szHeader[30][2] = { 	{ 0x01, 0x02 }, { 0x03, 0x04 }, { 0x05, 0x06 }, { 0x07, 0x08 }, 
							{ 0x09, 0x10 }, { 0x20, 0x21 }, { 0x22, 0x23 }, { 0x24, 0x25 }, 
							{ 0x26, 0x27 }, { 0x28, 0x29 }, { 0x30, 0x31 },	{ 0x32, 0x33 }, 
							{ 0x34, 0x35 },	{ 0x36, 0x37 }, { 0x38, 0x39 },	{ 0x40, 0x41 }, 
							{ 0x42, 0x43 },	{ 0x44, 0x45 }, { 0x46, 0x47 },	{ 0x48, 0x49 }, 
							{ 0x50, 0x51 },	{ 0x52, 0x53 }, { 0x54, 0x55 },	{ 0x56, 0x57 },
							{ 0x58, 0x59 }, { 0x60, 0x61 },	{ 0x62, 0x63 }, { 0x64, 0x65 },
							{ 0x66, 0x67 }, { 0x68, 0x69 }
						};

CManager::CManager()
{

	m_MaxPlayers = MAX_PLAYING;
	m_ServerShutdown = false;
	m_ShutdownRemain = 0;
	m_NameserverIsSlow = true;
	m_UserDescriptorList = NULL;		/* master desc list */
	m_pDbQ = new CDbQ;
	m_PlayerCount = 0;
	m_pulse = 0;
	m_bServerNowOff = false;
	m_nShutdown_time = 0;
	m_Reboot = false;
	m_bKeepAliveSend = FALSE;
	m_nKeepAliveSendTime = 0;
	m_nKeepAliveCheckTime = 0;
	m_nSelectCnt = 0;

	l_pCManager = this;

	g_pDbQ = m_pDbQ; 
}

CManager::~CManager()
{
	delete m_pDbQ;
}


void CManager::ServerSleep(struct timeval *timeout)
{
	Sleep(timeout->tv_sec * 1000 + timeout->tv_usec / 1000);
}

bool CManager::GetPath()
{
	char szBuffer[512];

	GetModuleFileName(::GetModuleHandle(NULL), szBuffer, sizeof(szBuffer));

	int path_len = strlen(szBuffer);
	int i;

	for (i = path_len - 1; i >= 0; i-- ) {
		if (szBuffer[i] == '\\')  {
			szBuffer[i+1] = '\0';
			break;
		}
	}
	if (i < 0)
		return false;

	g_ServerPath = strdup(szBuffer);
	
	return true;
}

bool CManager::InitGame()
{

	m_ListenSocket = InitSocket( g_ServerInfo.nListenPort );

	Log("ServeSocket Init");
	return true;
}

SOCKET CManager::InitSocket(ush_int port)
{
	SOCKET s;
	struct sockaddr_in sa;
	int opt;
	
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		Log("SYSERR: WinSock not available!");
		exit(1);
	}

	strcpy (a_Log, "Max players : ");
	IntCat (a_Log, m_MaxPlayers);
	Log (a_Log);

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		Log ("SYSERR: Error opening network connection : Winsock error");
		exit(1);
	}

#if defined(SO_REUSEADDR)
	opt = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0){
		Log ("SYSERR: setsockopt REUSEADDR");
		exit(1);
	}
#endif

	SetSendbuf(s);

#if defined(SO_LINGER)
	{
	struct linger ld;

	ld.l_onoff = 0;
	ld.l_linger = 0;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0)
		Log ("SYSERR: setsockopt SO_LINGER");	/* Not fatal I suppose. */
	}
#endif

	memset((char *)&sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = *(GetBindAddr());

	if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		Log ("SYSERR: bind");
		CLOSE_SOCKET(s);
		FILE *fp = fopen (".shutdown", "w");
		fclose (fp);
		exit(1);
	}
	Nonblock(s);
	listen(s, 57);

	return (s);
}

int CManager::SetSendbuf(SOCKET s)
{
#if defined(SO_SNDBUF)
	int opt = MAX_SOCK_BUF;

	if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
		Log ("SYSERR: setsockopt SNDBUF");
		return (-1);
	}
#endif
	return (0);
}

struct in_addr *CManager::GetBindAddr()
{
	static struct in_addr bind_addr;

	memset((char *) &bind_addr, 0, sizeof(bind_addr));

	bind_addr.s_addr = htonl(INADDR_ANY);

	if (bind_addr.s_addr == htonl(INADDR_ANY))
		Log("Binding to all IP.");
	else {
		strcpy (a_Log, "Binding only to IP address");
		strcat (a_Log, inet_ntoa(bind_addr));
		Log (a_Log);
	}
	
	return (&bind_addr);
}

void CManager::Nonblock(SOCKET s)
{
	unsigned long val = 1;
	ioctlsocket(s, FIONBIO, &val);
}



void CManager::Run()
{

	struct timeval tics_time, check_time;

	char comm[INPUT_Q_BUFSIZE];
	int nInput_Q_BufSize = 0;
	CUserDescriptor *d, *next_d;

	int nSendPacketLen = 0;
	SOCKET maxdesc;

	
	GetTimeofDay(&tics_time, (struct timezone *) 0);
	int ntics = 0;
	int nClientCnt = 0;
	int nSelectCnt = 0;
	
	int nTemp = 0;
	while (!m_ServerShutdown || m_ShutdownRemain) {
		
		/* Set up the input, output, and exception sets for select(). */
		FD_ZERO(&input_set);
		FD_ZERO(&exc_set);
		FD_SET(m_ListenSocket, &input_set);

		maxdesc = m_ListenSocket;

		m_nSelectCnt = 0;
		for (d = m_UserDescriptorList; d; d = d->m_pNext) {

			if (d->m_ClientSocket > maxdesc)
				maxdesc = d->m_ClientSocket;

			FD_SET(d->m_ClientSocket, &input_set);
			FD_SET(d->m_ClientSocket, &exc_set);

			m_nSelectCnt++;
		}


		a_NullTime.tv_sec = 0;
		a_NullTime.tv_usec = 100000;

		nSelectCnt = select(maxdesc + 1, &input_set, NULL, &exc_set, &a_NullTime);

		if ( nSelectCnt < 0) {
			sprintf( a_Log, "SYSERR: Select poll errorno = %d", WSAGetLastError() );
			Log(a_Log);
			LogFile ( a_Log );
			return;
		}

		if (FD_ISSET(m_ListenSocket, &input_set)) {
 			NewDescriptor(m_ListenSocket);
		}

		/*----------------------------------------------------------------------------------------------- 1차 처리 시작 %%%*/
		for (d = m_UserDescriptorList; d; d = next_d) {
			next_d = d->m_pNext;

			if ( STATE(d) == CLIENT_WILL_DISCONNECT ) continue;

			if (FD_ISSET(d->m_ClientSocket, &exc_set)) 
			{
				FD_CLR(d->m_ClientSocket, &input_set);
	

				sprintf( a_Log, "[CLOSE SOCKET] >> exc_set < UserID : %s User State : %d", d->m_MyInfo.szUser_Id, STATE(d) );
				LogFile (a_Log);

				d->m_bClosed = true;
				CloseSocket(d);
				continue;
			}

			if (WAITTING_DB(d))
				continue;

			if (FD_ISSET(d->m_ClientSocket, &input_set))
			{ 
				if (STATE(d) != CLIENT_WILL_DISCONNECT && d->ProcessInput(false) < 0) {
					sprintf( a_Log, "(d->ProcessInput(false) < 0)\t UserId : %s", d->m_MyInfo.szUser_Id );
					LogFile (a_Log);
					
					d->m_MyInfo.l_sUserInfo.nCut_Time++;
					d->m_bClosed = true;
					CloseSocket(d);
				} 
				continue;
			}

			if ( !ACCESSED_DB(d) ) {

				sprintf( a_Log, "(ACCESSED_DB False)\t UserId : %s", d->m_MyInfo.szUser_Id );
				LogFile (a_Log);
					
				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_ACCESS_DBFAIL );

				SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
				SendOutput (d);

				d->m_bClosed = true;
				CloseSocket(d);
				continue;
			}

			switch(STATE(d)) {
				case GETUSER:
			
					memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
					nSendPacketLen = NGFrameGetUserInfo(g_ClientSendBuf, d );
					SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
					SendOutput (d);	
	
					STATE(d) = CMD_WAIT;

					sprintf( a_Log, "(GETUSERINFO REP)\t UserNo = %d, UserId : %s, UserNick = %s UserIp = %s \t SendLen = %d\tGETUSERINFO", 
						d->m_MyInfo.l_sUserInfo.nUserNo, d->m_MyInfo.szUser_Id, 
						d->m_MyInfo.l_sUserInfo.szNick_Name, d->m_Host, nSendPacketLen );
					LogFile (a_Log);

					DTRACE( a_Log );

					break;

			}


			if ( !d->input.GetFromQ(comm, &nInput_Q_BufSize) ) continue;
		
			d->m_bDataInput = TRUE;
			
			switch(STATE(d)) {
				case CMD_WAIT:
					CmdProcess( d, comm );
					continue;
			}
		}


		if (m_ServerShutdown && m_ShutdownRemain == 0)
			break;



		GetTimeofDay(&check_time, (struct timezone*)0);
		if( time_gap_return_second(&tics_time, &check_time) >= TICS_GAP_SEC )
		{
			tics_time = check_time;

			m_nKeepAliveSendTime++;

			if ( m_bKeepAliveSend )
				m_nKeepAliveCheckTime++;

			if( ntics++ > REFRESH_TICS ) {
				ntics = 1; 

				m_nKeepAliveSendTime = 1;
				m_nKeepAliveCheckTime = 1;
			}

			do_game_server_tics(ntics);

		}
	}
}

void CManager::KeepAliveSend()
{
	CUserDescriptor *d;
	int nSendPacketLen;
	
	for (d = m_UserDescriptorList; d; d = d->m_pNext) {

		if ( d->m_bDataInput == FALSE ) {

			d->m_bKeepAliveSend = TRUE;

			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameKeepAlive( g_ClientSendBuf );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			SendOutput (d);

			sprintf( a_Log, "(KeepAliveServerSend)\t UserId : %s", d->m_MyInfo.szUser_Id );
			LogFile (a_Log);
		}

		d->m_bDataInput = FALSE;
	}
}

void CManager::KeepAliveCheck()
{
	CUserDescriptor *d;
	
	for (d = m_UserDescriptorList; d; d = d->m_pNext) {

		if ( d->m_bKeepAliveSend == TRUE ) {
			if ( d->m_bDataInput == FALSE ) {

				sprintf( a_Log, "[USERWAIT CLOSE SOCKET] >> Connection timeout < UserID : %s User State : %d, d->m_IdleTics : %d", d->m_MyInfo.szUser_Id, STATE(d), d->m_IdleTics );
				LogFile (a_Log);

				d->m_MyInfo.l_sUserInfo.nCut_Time++;
				d->m_bClosed = true;
				CloseSocket(d);
				continue;
			}
			else 
				d->m_bKeepAliveSend = FALSE;
		}
	}
}


void CManager::do_game_server_tics(int tics)
{
	if( m_nKeepAliveSendTime >= PULSE_CONNECT_TIMEOUT)   { //2분에 한번씩 검사를 한다.

		KeepAliveSend();
		m_nKeepAliveSendTime = 0;
		m_bKeepAliveSend = TRUE;

		if (m_ShutdownRemain > 0) {
			SendToAllPlaying ();
		}

	}

	if ( m_bKeepAliveSend == TRUE && m_nKeepAliveCheckTime >= LOSE_CONNECT_TIMEOUT )  {
		KeepAliveCheck();

		m_nKeepAliveSendTime = 0;
		m_nKeepAliveCheckTime = 0;
		m_bKeepAliveSend = FALSE;
	}

	if ( !( tics % 20 ) ) {
		sprintf( a_Log, "ClientSocketCnt : %d", m_nSelectCnt);
		Log (a_Log);
	}


}

void CManager::SendToAllPlaying()
{
	CUserDescriptor *d;
	int nSendLen = sizeof(g_ClientSendBuf);
	for (d = m_UserDescriptorList; d; d = d->m_pNext) 
	{
		SEND_TO_Q( g_ClientAllSendBuf, d, g_ClientAllSendLen );
		SendOutput (d);
	}
	return;
}

void CManager::ServerOff()
{

	if ( m_bServerNowOff == true ) {
		m_ServerShutdown = true;
		m_ShutdownRemain = 0;
	}
	else {
		m_Reboot = true;
		m_ServerShutdown = true;
		m_ShutdownRemain = m_nShutdown_time * 10;
	}

}

void CManager::Close()
{
	CUserDescriptor *d;

	int nSendPacketLen = 0;
	int nDbAccessCnt = 0;

	Log("Saving All User Data....");
	LogFile("Saving All User Data....");

	for (d = m_UserDescriptorList; d; d = d->m_pNext) {
		CloseSocket(d);

		nDbAccessCnt++;
		if ( nDbAccessCnt > 30 ) {
			nDbAccessCnt = 0;
			LogFile("Close()에서 유저가 많아서 DB처리를 5초간 기다린다.");
			Sleep(5000);
		}
	}

	LogFile("Closing all sockets.");

	CLOSE_SOCKET(m_ListenSocket);

	if (g_ServerPath)
		delete[] g_ServerPath;

	g_DbEnd = true;

	while (!m_pDbQ->m_dbEnd) {
		Log ("Waiting for Quit DB ...");
		Sleep (1000);
	}
}

int CManager::NewDescriptor(int s)
{
	SOCKET desc;
	int i;
	static int last_desc = INIT_USER_NO;	/* last descriptor number */
	CUserDescriptor *newd;
	struct sockaddr_in peer;
	struct hostent *from;

	int nTemp = 0;
	int nSendPacketLen = 0;


	i = sizeof(peer);
	if ((desc = accept(s, (struct sockaddr *) &peer, &i)) == INVALID_SOCKET) {
		return (-1);		// 흑흑 접속할 수가 없어 
	}
	Nonblock(desc);

	if (SetSendbuf(desc) < 0) {
		CLOSE_SOCKET(desc);	// 애겅 버퍼를 초기화 할 수가 없어
		return (0);
	}

	CREATE(newd, CUserDescriptor, 1);

	/* find the sitename */
	if (m_NameserverIsSlow || 
		!(from = gethostbyaddr((char *) &peer.sin_addr,	sizeof(peer.sin_addr), AF_INET))) {
		if (!m_NameserverIsSlow)
			Log ("SYSERR: gethostbyaddr");

		/* find the numeric site address */
		strncpy(newd->m_Host, (char *)inet_ntoa(peer.sin_addr), HOST_LENGTH);
		*(newd->m_Host + HOST_LENGTH) = '\0';
		sprintf( newd->m_HostCClass, "%d.%d.%d", peer.sin_addr.S_un.S_un_b.s_b1, peer.sin_addr.S_un.S_un_b.s_b2, peer.sin_addr.S_un.S_un_b.s_b3 );
		
	} else {
		strncpy(newd->m_Host, from->h_name, HOST_LENGTH);
		*(newd->m_Host + HOST_LENGTH) = '\0';
	}

	newd->m_ClientSocket = desc;
	newd->m_IdleTics = 0;
	newd->m_Output = newd->m_SmallOutbuf;
	newd->m_Bufspace = SMALL_BUFSIZE - 1;
	*newd->m_Output = '\0';
	newd->m_Bufptr = 0;

	STATE(newd) = CMD_WAIT;

	newd->m_MyInfo.l_sUserInfo.nUserNo = last_desc;



	if (++last_desc == 100000000)
		last_desc = INIT_USER_NO;

	newd->m_pNext = m_UserDescriptorList;
	m_UserDescriptorList = newd;


	nTemp = GetRandom( 0, 29 );

	newd->m_szPureHeader[0] = g_szHeader[nTemp][0];
	newd->m_szPureHeader[1] = g_szHeader[nTemp][1];
	
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameFirst(g_ClientSendBuf, newd );
	SEND_TO_Q(g_ClientSendBuf, newd, nSendPacketLen);
	SendOutput (newd);	

	return (0);
}

bool CManager::MakeThread()
{
	if ((m_hDbQThread = _beginthread(DbQThread, sizeof(CDbQ *), m_pDbQ)) == -1)
		return false;
	m_pDbQ->SetThread(m_hDbQThread);
	Log ("User-Thread Start");
	return true;
}

void CManager::SendOutput(CUserDescriptor *d)
{
	if (d == NULL) return;

	if (d->m_bClosed) return;
	
	if ((int)d->m_Output == -1) {
		d->m_bClosed = true;
		return;
	}
	if (d->m_Bufptr > 0) {
			if (d->ProcessOutput() < 0) {
				d->m_bClosed = true;
			}
	}
}

void CManager::CloseSocket(CUserDescriptor *d)
{
	CUserDescriptor *temp;
	int nRoomNo = 0;
	int nRoomInUserNo = 0;
	int nSendPacketLen = 0;

	sprintf( a_Log, "(CloseSocket)시작 UserIp : %s", d->m_Host );
	LogFile (a_Log);	

	switch(USERPOSITION(d)) {
		case USERWAIT:
			g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].DeleteWaitChannelUser(d);	//해당유저를 삭제한다.
			LogFile ("(CloseSocket)대기실리스트에서 유저삭제");

			USERPOSITION(d) = USERNONE;

#ifdef REALTIME_REFRESH
			UserWaitRefresh( d, d->m_MyInfo.l_sUserInfo.nChannel, 'D', d->m_MyInfo.l_sUserInfo.nUserNo );  //유저추가 
#endif
			sprintf( a_Log, "(CloseSocket)UserId : %s 대기실유저들에게 날려준다.", d->m_MyInfo.szUser_Id );
			LogFile (a_Log);
			
			break;
		case USERGAME:
			
			USERPOSITION(d) = USERNONE;

			LogFile ("(CloseSocket)게임리스트에서 유저삭제");
			break;
	}

	if (d->m_DBQRunning || STATE(d) == CLIENT_WILL_DISCONNECT ) {

		LogFile ("(CloseSocket)그냥나간다.");

		return;
	}

	switch (STATE(d)) {
		case CMD_WAIT:
		case GETUSER:
		case USER_UPDATE:

			#ifdef PACKETANALY
				if ( d != NULL ) {
					sprintf( a_Log, "(CloseSocket)UserId : %s, m_bPlay = true이다.", d->m_MyInfo.szUser_Id);
					LogFile (a_Log);

				}
			#endif

				if ( ( 0 <= d->m_nRoomNo && d->m_nRoomNo < MAX_ROOMCNT ) &&
				 ( 0 <= d->m_nRoomInUserNo && d->m_nRoomInUserNo < MAX_ROOM_IN )) {
					
				nRoomNo = d->m_nRoomNo;
				nRoomInUserNo = d->m_nRoomInUserNo;

				#ifdef PACKETANALY
					if ( d != NULL ) {
						sprintf( a_Log, "(CloseSocket)방에있다. UserId : %s, nRoomNo = %d, nRoomInUserNo = %d", d->m_MyInfo.szUser_Id, nRoomNo, nRoomInUserNo  );
						LogFile (a_Log);
					}
				#endif

				g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].RoomOutUser( nRoomNo, nRoomInUserNo, d );

		#ifdef REALTIME_REFRESH	//방에서 CloseSocket을 했음을 대기실 유저게에 뿌린다.
				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, nRoomNo, nRoomInUserNo, d->m_MyInfo.l_sUserInfo.nUserNo, 'X' );
				BroadCastUserPage( g_ClientSendBuf, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nUserPage, nSendPacketLen );			//대기실 해당페이지만.
		#endif	

			}

			d->CloseSocket();
			REMOVE_FROM_LIST(d, m_UserDescriptorList, m_pNext);

			STATEDBMODE(d) = CLIENT_WILL_DISCONNECT;
			STATE(d) = CLIENT_WILL_DISCONNECT;
			
			WAITTING_DB(d) = true;
			m_pDbQ->AddToQ(d);	

			sprintf( a_Log, "(CloseSocket)UserId : %s DB Q에 넣는다.", d->m_MyInfo.szUser_Id);
			LogFile (a_Log);
			return;
	}

	sprintf( a_Log, "(CloseSocket)UserId : %s 완전삭제", d->m_MyInfo.szUser_Id);
	LogFile (a_Log);

	m_pDbQ->EraseQ (d);
	
	d->CloseSocket();
	REMOVE_FROM_LIST(d, m_UserDescriptorList, m_pNext);

	SAFE_DELETE_ARRAY(d);

	return;
}

int WriteToDescriptor(SOCKET desc, const char *txt, int length)
{
	int iStart;
	int nWrite;
	int nBlock;

	if ( length <= 0 )
		length = strlen(txt);

	for ( iStart = 0; iStart < length; iStart += nWrite ) {
		nBlock = ((length - iStart) < (4096)) ? (length - iStart) : (4096);

		if ((nWrite = send (desc, txt + iStart, nBlock, 0)) <= 0 ) {

			int nError = WSAGetLastError();

			sprintf( a_Log, "WriteToDescriptor - Socket Error : %d", nError );
			LogFile (a_Log);

			if (nWrite == 0) return -1;	// send가 0을 리턴하면 상대방이 연결을 닫았거나 연결이 끊어졌다는 것.

			if ( WSAGetLastError() == WSAEWOULDBLOCK ) {
				LogFile ("WSAEWOULDBLOCK - 이게 나면 음..");
				return (0);
			}
				
			if ( WSAGetLastError() == WSAEINTR) {
				LogFile ("WSAEINTR - 이게 나면 음..");
				return (0);
			}

			return -1;
		}
	}

	return 0;
}

int PerformSocketRead(SOCKET desc, char *read_point, size_t space_left)
{
	int ret;

	ret = recv(desc, read_point, space_left, 0);

/* Read was successful. */
	if (ret > 0)
		return (ret);

/* read() returned 0, meaning we got an EOF. */
	if (ret == 0) {
		LogFile("WARNING: EOF on socket read (connection broken by peer)");
		
		return (-1);
	}

/*
* read returned a value < 0: there was an error
*/

	int nError = WSAGetLastError();

	sprintf( a_Log, "PerformSocketRead - Socket Error : %d", nError );
	LogFile (a_Log);


	if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
		return (0);
	return (-1);

}

