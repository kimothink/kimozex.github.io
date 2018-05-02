#include "Common.h"
#include "Manager.h"
#include "CmdProcess.h"
#include "NGFrame.h"

#include "dtrace.h"

//게임의 현재경로를 가져온다.
char *g_ServerPath;
CManager *g_CManager = NULL;
class CDescriptorData;

CTxtBlock *bufpool = NULL;	/* pool of large output buffers */
int buf_overflows = 0;		/* # of overflows of output */
int buf_switches = 0;		/* # of switches from small to large buf */
int buf_largecount = 0;		/* # of large buffers which exist */

char a_pBuffer[MAX_STRING_LENGTH];

char cFinalError = NULL;

//World g_World;

bool g_bSndMsg = false;
bool g_DbEnd = false;

struct timeval a_NullTime;	/* zero-valued time structure */
 
//cmdprocess에서 사용하기 위해
CManager *l_pCManager;


CDbQ *g_pDbQ;

char g_szHeader[30][2] = { 	{ 0x50, 0x51 }, { 0x52, 0x53 }, { 0x60, 0x61 }, { 0x62, 0x63 }, 
							{ 0x70, 0x71 }, { 0x72, 0x73 }, { 0x80, 0x81 }, { 0x82, 0x83 }, 
							{ 0x90, 0x91 }, { 0x92, 0x93 }, { 0xa0, 0xa1 },	{ 0xa2, 0xa3 }, 
							{ 0xb0, 0xb1 },	{ 0xb2, 0xb3 }, { 0xc0, 0xc1 },	{ 0xc2, 0xc3 }, 
							{ 0xd0, 0xd1 },	{ 0xd2, 0xd3 }, { 0xe0, 0xe1 },	{ 0xe2, 0xe3 }, 
							{ 0xf0, 0xf1 },	{ 0xf2, 0xf3 }, { 0x10, 0x11 },	{ 0x12, 0x13 },
							{ 0x20, 0x21 }, { 0x22, 0x23 },	{ 0x30, 0x31 }, { 0x32, 0x33 },
							{ 0x40, 0x41 }, { 0x42, 0x43 }
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
	g_CManager = this;

	//Log("Finding player limit.");
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

	/* Clear the structure */
	memset((char *)&sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr = *(GetBindAddr());

	if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		Log ("SYSERR: bind");	// 서버 소켓이 사용중이면 겜 종료후 다시 띄우지 말자 
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

	/* Clear the structure */
	memset((char *) &bind_addr, 0, sizeof(bind_addr));

	bind_addr.s_addr = htonl(INADDR_ANY);

	/* Put the address that we've finally decided on into the logs */
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
	struct timeval last_time, opt_time, process_time, temp_time;
	struct timeval before_sleep, now, timeout;
	char comm[INPUT_Q_BUFSIZE];
	int nInput_Q_BufSize = 0;
	CUserDescriptor *d, *next_d;
	int missed_pulses;//, aliased;

	int nSendPacketLen = 0;
	SOCKET maxdesc;

	int nSelectCnt = 0;
	int nInputRe = 0;

	CUserDescriptor *temp;
	/* initialize various time values */
	opt_time.tv_usec = OPT_USEC;
	opt_time.tv_sec = 0;
	
	//yun 처음 시작시의 시간을 가져온다.
	GetTimeofDay(&last_time, (struct timezone *) 0);

	/* The Main Loop.  The Big Cheese.  The Top Dog.  The Head Honcho.  The.. */

	int count=0;
	int count_pulse=0;


	while (!m_ServerShutdown || m_ShutdownRemain) {
		
		/* Set up the input, output, and exception sets for select(). */
		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);
		FD_SET(m_ListenSocket, &input_set);

		maxdesc = m_ListenSocket;

		for (d = m_UserDescriptorList; d; d = d->m_pNext) {

			if (d->m_ClientSocket > maxdesc)
				maxdesc = d->m_ClientSocket;

			FD_SET(d->m_ClientSocket, &input_set);
			FD_SET(d->m_ClientSocket, &output_set);
			FD_SET(d->m_ClientSocket, &exc_set);

			count++;
		}

		/*
		* At this point, we have completed all input, output and heartbeat
		* activity from the previous iteration, so we have to put ourselves
		* to sleep until the next 0.1 second tick.  The first step is to
		* calculate how long we took processing the previous iteration.
		*/
    
		//yun 지금시간을 가져온다.
		GetTimeofDay(&before_sleep, (struct timezone *) 0); /* current time */

		//yun 두시간의 차를 구한다.
		TimeDiff(&process_time, &before_sleep, &last_time);

		/*
		* If we were asleep for more than one pass, count missed pulses and sleep
		* until we're resynchronized with the next upcoming pulse.
		*/

		if (process_time.tv_sec == 0 && process_time.tv_usec < OPT_USEC) {
			missed_pulses = 0;
		} else {
			missed_pulses = process_time.tv_sec * PASSES_PER_SEC;
			missed_pulses += process_time.tv_usec / OPT_USEC;
			process_time.tv_sec = 0;
			process_time.tv_usec = process_time.tv_usec % OPT_USEC;
		}

		/* Calculate the time we should wake up */
		TimeDiff(&temp_time, &opt_time, &process_time);
		TimeAdd(&last_time, &before_sleep, &temp_time);

		/* Now keep sleeping until that time has come */
		GetTimeofDay(&now, (struct timezone *) 0);
		TimeDiff(&timeout, &last_time, &now);

		/* Go to sleep */
		do {
			ServerSleep(&timeout);
			GetTimeofDay(&now, (struct timezone *) 0);
			TimeDiff(&timeout, &last_time, &now);
		} while (timeout.tv_usec || timeout.tv_sec);

		/* Poll (without blocking) for new input, output, and exceptions */
		a_NullTime.tv_sec = 0;
		a_NullTime.tv_usec = 0;
		
		nSelectCnt = select(maxdesc + 1, &input_set, &output_set, &exc_set, &a_NullTime);
		if ( nSelectCnt < 0) {
			sprintf( a_Log, "SYSERR: Select poll errorno = %d", WSAGetLastError() );
			Log(a_Log);
			LogFile ( a_Log );
			return;
		}

		/* If there are new connections waiting, accept them. */
		if (FD_ISSET(m_ListenSocket, &input_set)) {
 			NewDescriptor(m_ListenSocket);
		}

		count_pulse ++;
		if (count_pulse > PULSE_SAVE_ALL) {
			sprintf( a_Log, "PLAYERS : %d", count );
			Log(a_Log);
			LogFile (a_Log);
			count_pulse = 0;
		}

		count = 0;
		/*
		* Now, we execute as many pulses as necessary--just one if we haven't
		* missed any pulses, or make up for lost time if we missed a few
		* pulses by sleeping for too long.
		*/
		missed_pulses++;
		if (missed_pulses <= 0) missed_pulses = 1;
		/*----------------------------------------------------------------------------------------------- 1차 처리 시작 %%%*/
		for (d = m_UserDescriptorList; d; d = next_d) {
			next_d = d->m_pNext;
			
			if (d->m_TimeStamp >=0) d->m_TimeStamp += missed_pulses;
			d->m_IdleTics++;

			/* Process descriptors with input pending */
		
			if ( d->m_IdleTics > PULSE_CONNECT_TIMEOUT ) {

				if ( d->m_bKeepAliveSend ) {

					if ( d->m_bClosed == true )
						continue;

					if ( d->m_IdleTics > (PULSE_CONNECT_TIMEOUT + LOSE_CONNECT_TIMEOUT) ) {
						sprintf( a_Log, "[USERWAIT CLOSE SOCKET] >> Connection timeout < UserID : %s User State : %d, d->m_IdleTics : %d", d->m_MyInfo.szUser_Id, STATE(d), d->m_IdleTics );
						LogFile (a_Log);
						d->m_bClosed = true;
						CloseSocket(d);
						continue;
					}
				}
				else {
					d->m_bKeepAliveSend = true;
					sprintf( a_Log, "(KeepAliveServerSend)\t UserId : %s", d->m_MyInfo.szUser_Id );
					LogFile (a_Log);

					memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
					nSendPacketLen = NGFrameKeepAlive( g_ClientSendBuf );
					SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
					SendOutput (d);
				}
			}

			if (FD_ISSET(d->m_ClientSocket, &exc_set)) 
			{
				FD_CLR(d->m_ClientSocket, &input_set);
				FD_CLR(d->m_ClientSocket, &output_set);
				d->m_IdleTics = 0;

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
				if ( d->ProcessInput(false) < 0) {
					//입력 쪽에서 접속이 끊어졌을 때..
					sprintf( a_Log, "(d->ProcessInput(false) < 0)\t UserId : %s", d->m_MyInfo.szUser_Id );
					LogFile (a_Log);
					
					//끈긶 횟수를 증가.
					d->m_MyInfo.l_sUserInfo.nCut_Time++;
					d->m_bClosed = true;
					CloseSocket(d);
				} 
				continue;
			}

			//DB에 엑세스를 했는데 에러가 난상황이면 짤라버려려야 한다. 어떤상황이던지....
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
				case DBSEARCH_ISLOGIN:
					//여기에서 클라이언트로 메세지를 보내준다.
					memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);

					if ( d->m_nIsLogin == 1 ) {
						sprintf( a_Log, "(DBSEARCH_ISLOGIN False)\t 이미로그인. UserId : %s", d->m_MyInfo.szUser_Id );
						LogFile (a_Log);
							
						memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
						nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_MULTILOGIN );

						SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
						SendOutput (d);

						d->m_bClosed = true;
						CloseSocket(d);
						continue;
					}
					else {

						nSendPacketLen = NGFrameIsLogIn( g_ClientSendBuf, d );

						SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
						SendOutput (d);
						STATE(d) = CMD_WAIT;

						sprintf( a_Log, "(DBSEARCH_ISLOGIN REP)\t UserId : %s \t SendLen = %d\tDBSEARCH_USERLOGIN", d->m_MyInfo.szUser_Id, nSendPacketLen );
						LogFile (a_Log);
					}

					break;

					break;
			}


			if ( !d->input.GetFromQ(comm, &nInput_Q_BufSize) ) continue;

			d->m_IdleTics = 0;
			d->m_bKeepAliveSend = false;
			
			switch(STATE(d)) {
				case CMD_WAIT:
					CmdProcess( d, comm );
					continue;
			}
		}

		//----------------------------------------------------------------------------------------------- 1차 처리 끝 %%%
		if (m_ServerShutdown && m_ShutdownRemain == 0)
			break;
		//----------------------------------------------------------------------------------------------- 2차 처리 시작 %%%
		// Send queued output out to the operating system (ultimately to user). 
		for (d = m_UserDescriptorList; d; d = next_d) {
			next_d = d->m_pNext;
			// Kick out folks in the CON_CLOSE or CON_DISCONNECT state 
			if ( d->m_bClosed ) {
				CloseSocket(d);
				continue;
			}
			if (*(d->m_Output) && FD_ISSET(d->m_ClientSocket, &output_set)) {
				if (!(d->m_bClosed) && d->ProcessOutput() < 0) {
					d->m_bClosed = true;
					CloseSocket(d);
					continue;
				}
			}
		}

		//----------------------------------------------------------------------------------------------- 2차 처리 끝 %%%
		// Now execute the heartbeat functions 
		if (m_ServerShutdown && m_ShutdownRemain > 0) {
			m_ShutdownRemain --;
			if ((m_ShutdownRemain <= 300) && (m_ShutdownRemain % 50 == 0)) {
			}
		}

		m_pulse += missed_pulses;

		//24시간 가동 표시
		if ( m_pulse >= DAY_LOOPING ) {
			strcpy (a_Log, "[24HOUR OPERATION]");
			LogFile (a_Log);
			m_pulse = 0;
		}

		missed_pulses = 0;
		m_PlayerCount = count;
	}
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


	// 소켓 닫고 다 저장한다.
	Log("Saving All User Data....");
	LogFile("Saving All User Data....");

	for (d = m_UserDescriptorList; d; d = d->m_pNext) {
		CloseSocket(d);

		nDbAccessCnt++;
		//CloseSocket을 부르면 디비를 엑세스 하기때문에 30명이상은 기다린다.
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

	//디비Thread를 여기에서 멈춘다.
	g_DbEnd = true;

	while (!m_pDbQ->m_dbEnd) {
		Log ("Waiting for Quit DB ...");
		//DB Thread가 완전히 끝날때까지 기다린다
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
		return (-1);		
	}
	Nonblock(desc);

	if (SetSendbuf(desc) < 0) {
		CLOSE_SOCKET(desc);	
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

	
	if (++last_desc == 100000000)
		last_desc = INIT_USER_NO;

	newd->m_MyInfo.l_sUserInfo.nUserNo = last_desc;


	newd->m_pNext = m_UserDescriptorList;
	m_UserDescriptorList = newd;

	nTemp = GetRandom( 0, 29 );

	newd->m_szPureHeader[0] = g_szHeader[nTemp][0];
	newd->m_szPureHeader[1] = g_szHeader[nTemp][1];

	
	//Client에게 생성된 Header를 보낸다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameFirst(g_ClientSendBuf, newd );
	SEND_TO_Q(g_ClientSendBuf, newd, nSendPacketLen);
	SendOutput (newd);


	sprintf( a_Log, "(NewDescriptor)\t UserIp : %s", newd->m_Host );
	LogFile (a_Log);

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
		if (FD_ISSET(d->m_ClientSocket, &output_set)) {
			if (d->ProcessOutput() < 0) {
				d->m_bClosed = true;
			}
		}
	}
}

void CManager::CloseSocket(CUserDescriptor *d)
{
	CUserDescriptor *temp;
	int nRoomNo = 0;
	int nRoomInUserNo = 0;
	int nSendPacketLen = 0;

	sprintf( a_Log, "(CloseSocket)\t UserId : %s 완전삭제", d->m_MyInfo.szUser_Id);
	LogFile (a_Log);

	m_pDbQ->EraseQ (d);	// erase Descriptor from queue.
	REMOVE_FROM_LIST(d, m_UserDescriptorList, m_pNext);
	d->CloseSocket();
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
			if (nWrite == 0) return -1;
			if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
				return (0);
			if (errno != 104) {
				strcpy (a_Log, "SYSERR : WriteToDescriptor : no =");
				IntCat (a_Log, errno);
				LogFile (a_Log);

				strcpy (a_Log, "(WriteToDescriptor)\t");
				StrCat (a_Log, "SocketSend"); 
				StrCat (a_Log, "\t0\tSend시에 Error발생"); 
				LogFile (a_Log);
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
		//Log("WARNING: EOF on socket read (connection broken by peer)");
		return (-1);
	}

/*
* read returned a value < 0: there was an error
*/

	if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
		return (0);
	return (-1);
}

