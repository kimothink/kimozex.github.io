#include "CSServerCon.h"




CSServerCon::CSServerCon()
{
	m_CSClient = NULL;

}


CSServerCon::~CSServerCon()
{

}





void CSServerConThread(LPVOID lParam)
{
	CSServerCon *pCSServerCon = (CSServerCon *) lParam;

	CSServerConDbFunc(pCSServerCon);
}

bool ConnCSServer (CSServerCon *pCSServerCon)
{
	/*
	struct sockaddr_in msg_socket;

	char szTemp[MAX_ID_LENGTH];
	int nInput_Q_BufSize;

	
	if (pCSServerCon->m_CSClient) {
		if(pCSServerCon->m_CSClient->m_bClosed){ 
			pCSServerCon->m_CSClient->CloseSocket();
			delete [] pCSServerCon->m_CSClient;
			pCSServerCon->m_CSClient = NULL;
		}
		else
			return true;
	}

	pCSServerCon->m_CSClient = new CUserDescriptor [1];
	if ((pCSServerCon->m_CSClient->m_ClientSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		puts ("CSThread SYSERR: Error opening network connection : Winsock error");
		return false;
	}

	if (pCSServerCon->m_CSClient->m_ClientSocket <= 0) return false;
	memset (&msg_socket, 0, sizeof(msg_socket));
	msg_socket.sin_family = AF_INET;
	msg_socket.sin_addr.s_addr = inet_addr ( g_ServerInfo.szCSServerIp );
	msg_socket.sin_port = htons( g_ServerInfo.nCSPort );

	if (connect (pCSServerCon->m_CSClient->m_ClientSocket, (struct sockaddr *) &msg_socket, sizeof(msg_socket)) < 0) {
		Log ("CSServer Connecting Failed!!!");
		return false;
	}

	pCSServerCon->m_CSClient->m_Output	= pCSServerCon->m_CSClient->m_SmallOutbuf;
	pCSServerCon->m_CSClient->m_Bufspace = SMALL_BUFSIZE - 1;
	*pCSServerCon->m_CSClient->m_Output = '\0';
	pCSServerCon->m_CSClient->m_Bufptr	= 0;
	pCSServerCon->m_CSClient->m_MyInfo.l_sUserInfo.nUserNo = -1;
	
	int opt = MAX_SOCK_BUF;

	if (setsockopt(pCSServerCon->m_CSClient->m_ClientSocket, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
		puts ("SYSERR: CSServer setsockopt SNDBUF");
		return false;
	}

	//unsigned long val = 1;
	//ioctlsocket(pCSServerCon->m_CSClient->m_ClientSocket, FIONBIO, &val);

	Log ( "CSServer Connect Ok!!!");



	//////////////////////////////////////////////////////////////////////////
	//접속이 되면 아이피와 포트를 세팅한다.
	int nIndex = CS_HEADER_SIZE;
	char szBuffer[1024];

	szBuffer[nIndex++] = HIBYTE(LOWORD(g_ServerInfo.nServerIdx));
	szBuffer[nIndex++] = LOBYTE(LOWORD(g_ServerInfo.nServerIdx));

	szBuffer[nIndex++] = HIBYTE(LOWORD(g_ServerInfo.nListenPort));
	szBuffer[nIndex++] = LOBYTE(LOWORD(g_ServerInfo.nListenPort));

	memcpy( &szBuffer[nIndex], g_ServerInfo.szSelfIp, MAX_IP_LEN );
	nIndex += MAX_IP_LEN;

	memcpy( &szBuffer[nIndex], g_ServerInfo.szServerName, MAX_SERVERNAME );
	nIndex += MAX_SERVERNAME;

	memcpy( &szBuffer[nIndex], g_ServerInfo.szChannelName, MAX_SERVERNAME );
	nIndex += MAX_SERVERNAME;


	// Herader를 넣는다.
	//////////////////////////////////////////////////////////////////////////
	sCSHeaderPkt l_sCSHeaderPkt;

	l_sCSHeaderPkt.szHeader[0] = '0';
	l_sCSHeaderPkt.szHeader[1] = '0';
	l_sCSHeaderPkt.nTPacketSize = nIndex;
	l_sCSHeaderPkt.cMode = '0';
	l_sCSHeaderPkt.cCmd = NGSS_GSCSSERVERSET;
	l_sCSHeaderPkt.cErrorCode = ERROR_NOT;

	memcpy( szBuffer, &l_sCSHeaderPkt, sizeof(l_sCSHeaderPkt) );
	//////////////////////////////////////////////////////////////////////////
	




	SEND_TO_Q( szBuffer, pCSServerCon->m_CSClient, nIndex );
	pCSServerCon->m_CSClient->ProcessOutput(false);

	pCSServerCon->m_CSClient->ProcessInput(false);
	pCSServerCon->m_CSClient->input.GetFromQ( szTemp, &nInput_Q_BufSize );
  */
	return true;
}

void CSServerConDbFunc(CSServerCon *pCSServerCon)
{
	while (true) {	
		Sleep (3000);

		if (ConnCSServer (pCSServerCon))
			continue;
		if (pCSServerCon->m_CSClient)
			pCSServerCon->m_CSClient->m_bClosed = true;

	}		
}

