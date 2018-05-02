#include "stdafx.h"
#include "NetworkSock.h"
#include "Externvar.h"

#include "dtrace.h"

CSockBlock::CSockBlock(char *pBlock, int nLength) 
{ 
	ASSERT(pBlock);
	ASSERT(0 < nLength);
	m_pBlock = new char[nLength]; 
	m_nLength = nLength; 
	memcpy(m_pBlock, pBlock, m_nLength); 
}

CSockBlock::~CSockBlock() 
{ 
	delete m_pBlock;
}

int CSockBlock::RemoveHead(int nLength)
{
	if (m_nLength <= nLength)
		m_nLength = 0;
	else
	{
		m_nLength -= nLength;
		char *p = new char[m_nLength];
		memcpy(p, m_pBlock + nLength, m_nLength);
		delete	m_pBlock;
		m_pBlock = p;
	}
	return	m_nLength;
}


//////////////////////////////////
//��Ŷó�� Ŭ���� ����
//////////////////////////////////
CNetworkSock::CNetworkSock()
{
	m_WSASendBuf.len = 0;
	m_WSASendBuf.buf = m_szSBuffer;
	m_nSendBufIndex = 0;
	m_bConnected = false;
	m_bCanWrite = true;
	m_nCurPtr = 0;
	m_nReveillsize = 0;

	memset( m_szSBuffer, 0x00, sizeof(m_szSBuffer) );
	memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ) );

	InitializeCriticalSection(&m_stCriticalSection); 
}

CNetworkSock::~CNetworkSock()
{
	while (!m_listSend.IsEmpty())
		delete	m_listSend.RemoveHead();
	
	Destroy();
}

void CNetworkSock::OnConnect( int nErrorCode )
{
	// ������ ���� �ʾƵ� OnConnect�� �߻��Ѵ�.
	if ( nErrorCode != 0 )
	{
		g_pCMainFrame->m_Message->PutMessage( "Server Connect Fail!!!", "Code - 141", TRUE );

		return;
	}
}

void CNetworkSock::Destroy()
{
	if( m_sClientSocket != INVALID_SOCKET )
	{
		closesocket(m_sClientSocket);
		m_sClientSocket = INVALID_SOCKET;
        WSACleanup();				//Destroy WS2_32.DLL
	}
}

bool CNetworkSock::InitSocket( HWND hwnd, char *IP, int Port )
{
	if ( g_ServerSend == false ) return false;

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nError;

	nError = WSAStartup( wVersionRequested, &wsaData);
	if(nError != 0)
	{
		return false;
	}

	m_sClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( m_sClientSocket == INVALID_SOCKET )
	{
		return false;
	}
	else
	{
		m_SocketAddr.sin_family = AF_INET;
		m_SocketAddr.sin_port = htons(Port);
		m_SocketAddr.sin_addr.S_un.S_addr = inet_addr(IP);

		if( WSAAsyncSelect(m_sClientSocket, hwnd, WM_ASYNC, FD_CONNECT | FD_READ | FD_CLOSE |  FD_WRITE)==SOCKET_ERROR )
		{
			return false;
		}
		else
		{
			if(WSAConnect(m_sClientSocket, (LPSOCKADDR)&m_SocketAddr, sizeof(m_SocketAddr), NULL, NULL, NULL, 
				NULL) == SOCKET_ERROR)
			{
				if(WSAGetLastError() != WSAEWOULDBLOCK)
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool CNetworkSock::WriteSendQue( char cMode, char cCommand, char cErrorCode, char* SendPacket, int PacketLen )
{
	int Len = 0;
	m_nSendBufIndex = 0;

	memset((void *)m_szSBuffer, NULL, MAX_BUFFER_SIZE);

	//yun 2004.10.19
	m_nSendBufIndex = NGFramePacket( m_szSBuffer, g_szHeader[0], g_szHeader[1], PacketLen, cMode, cCommand, cErrorCode, SendPacket );


	if ( m_sClientSocket == INVALID_SOCKET )	return	FALSE;
	

	EnterCriticalSection(&m_stCriticalSection); 
	m_listSend.AddTail(new CSockBlock((LPSTR)m_szSBuffer, m_nSendBufIndex));
	LeaveCriticalSection(&m_stCriticalSection);

	return TRUE;


}

BOOL CNetworkSock::IsError()
{
	// �������� ���¶�� ���� �� �ִ� Error�� ��� �����ϰ�
	// �� ���� ��쿡�� ������ �ݴ´�
	int nError = WSAGetLastError();
	
	switch (nError) {
		case WSAENOBUFS:
		case WSAEWOULDBLOCK:
		case WSAEFAULT:
		case 0:
		case 1400:
			return	FALSE;
	}

	Destroy();
	return	TRUE;	
}

bool CNetworkSock::Send()
{
	//if (IsError())	return	FALSE;
	// ����Ʈ�� ���� ������ �о �� ���� ������

	bool nRe;
	EnterCriticalSection(&m_stCriticalSection);
	while (!m_listSend.IsEmpty())
	{
		CSockBlock* pBlock = (CSockBlock*)m_listSend.GetHead();
		int nSend = send( m_sClientSocket, (LPCSTR)*pBlock, (int)*pBlock, 0 );
		if (SOCKET_ERROR != nSend) 
		{
			if (!pBlock->RemoveHead(nSend))
				delete	m_listSend.RemoveHead();
		} else {
			nRe = false;
			break;
		}
	}

	nRe = true;


	LeaveCriticalSection(&m_stCriticalSection);
	return nRe;

}

bool CNetworkSock::Recv()
{
	char	szRecvBuffer[MAX_BUFFER_SIZE];
	
	memset( szRecvBuffer, 0, MAX_BUFFER_SIZE );

	WSABUF  RecvBuf;
	RecvBuf.buf = szRecvBuffer;
	RecvBuf.len = MAX_BUFFER_SIZE;


	DWORD RecvBytes;
	DWORD tRecvBytes;
	int ret = 0;
	DWORD dwFlag = 0;
	

	ret = WSARecv(m_sClientSocket, &RecvBuf, 1, &RecvBytes, &dwFlag, NULL, NULL);

	DTRACE( "RecvBytes = %d", RecvBytes );
	if( ret == SOCKET_ERROR )
	{
		if( WSAGetLastError() == WSAEWOULDBLOCK )
		{
			return false;
		}
		else
		{
			return false;
		}
	}

	tRecvBytes = RecvBytes;
	
	if ( m_nCurPtr == 0 ) {
		m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );
		
		//�޾ƾ��� �����͸�ŭ �԰ų��� ������..
		if ( m_nReveillsize <= tRecvBytes ) {
			//���°� ������.
			if ( m_nReveillsize == tRecvBytes ) {
				m_nCurPtr = 0;
				m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
			}
			else  {	
				while(1) {
						//�޾ƾ��ҵ���Ÿ���� ������ ������.
						m_nCurPtr = 0;
						m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
							
						//��������Ʈ
						tRecvBytes = tRecvBytes - m_nReveillsize;
						
						if ( tRecvBytes == 0 ) break;
						//��������Ÿ�� ������ �����.
						memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );

						//�����޾ƾ�����Ŷ.
						m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );

						if ( m_nReveillsize  > tRecvBytes ) {
							m_nReveillsize = m_nReveillsize - tRecvBytes;
							memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
							m_nCurPtr += tRecvBytes;
							break;
						}
				}
			}
		}
		else {
				memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
				m_nReveillsize = m_nReveillsize - tRecvBytes;
				m_nCurPtr += tRecvBytes;
		}
	}
	else {
		//�޾ƾ��� ����Ÿ���� �� ���� ������.
		if ( m_nReveillsize > tRecvBytes ) {
			memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, tRecvBytes );
			m_nReveillsize = m_nReveillsize - tRecvBytes;
			m_nCurPtr += tRecvBytes;
		}
		else
			if ( m_nReveillsize == tRecvBytes ) {
				memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, tRecvBytes );
				m_nCurPtr += tRecvBytes;
				m_input.WriteToQ( m_szBufferQ, m_nCurPtr);
				memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ));
				m_nCurPtr = 0;
			}
			else {
				//�ϼ��ȳ��� ť�κ����Ѵ�.
				memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, m_nReveillsize );	
				m_nCurPtr += m_nReveillsize;
				m_input.WriteToQ( m_szBufferQ, m_nCurPtr);
				memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ));

				//ó���� ��������.
				tRecvBytes = tRecvBytes - m_nReveillsize;
				memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );
				m_nCurPtr = 0;

				m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );
				
				//�޾ƾ��� ����Ÿ��ŭ �԰ų��� ������..
				if ( m_nReveillsize <= tRecvBytes ) {
					//���°� ������.
					if ( m_nReveillsize == tRecvBytes ) {
						m_nCurPtr = 0;
						m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
					}
					else  {	
						while(1) {
							//�޾ƾ��ҵ���Ÿ���� ������ ������.
							m_nCurPtr = 0;
							m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
										
							//��������Ʈ
							tRecvBytes = tRecvBytes - m_nReveillsize;
									
							//��������Ÿ�� ������ �����.
							memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );

							//�����޾ƾ�����Ŷ.
							m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );

							if ( m_nReveillsize  > tRecvBytes ) {
								m_nReveillsize = m_nReveillsize - tRecvBytes;
								memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
								m_nCurPtr += tRecvBytes;
								break;
							}
									
						}

				}
			}
			else {
				memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
				m_nReveillsize = m_nReveillsize - tRecvBytes;
				m_nCurPtr += tRecvBytes;
			}
		}
	}
	return true;
}

bool CNetworkSock::CheckSendBuf()
{
	if(m_WSASendBuf.buf == &m_szSBuffer[m_nSendBufIndex])
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CNetworkSock::SetClientOption( BOOL bInvite, BOOL bEarTalk, BOOL bMemoRecv )
{
	DTRACE("CNetworkSock::SetClientOption");

	//if ( !SendServer(WAITROOM_MODE) )
	//	return false;
	sOption l_sOption;
	int nIndex = 0;

	l_sOption.bInvite = bInvite;
	l_sOption.bEarTalk = bEarTalk;
	l_sOption.bMemoRecv = bMemoRecv;


	memcpy( &m_szNetBuff[nIndex], &l_sOption, sizeof(l_sOption) );
	nIndex += sizeof(l_sOption);

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( ALLSENDMODE, (char)NGCS_SETOPTION, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::WaitInput( sPKWaitInputCS *psPKWaitInputCS )
{
	DTRACE("CNetworkSock::WaitInput");

	if ( !SendServer(LOGO_MODE, true) )
		return false;

	g_ServerSend = false;

	DTRACE("6-g_ServerSend = false");

	WriteSendQue( WAITSENDMODE, (char)NGCS_WAITINPUT, (char)ERROR_NOT, (char *)psPKWaitInputCS, sizeof(sPKWaitInputCS));					
	if ( !Send() ) return false;

	return true;
}


bool CNetworkSock::CSGetServerList( char *pUserId )
{
	DTRACE("CNetworkSock::LogInServer");
	WriteSendQue( GAMESENDMODE, (char)NGCS_GETSERVERLIST, (char)ERROR_NOT, pUserId, MAX_ID_LENGTH);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GetUserInfo( char *pUserId, int nGameChannel )
{
	DTRACE("CNetworkSock::GetUserInfo");

	int nIndex = 0;
	sGetUserInfo l_sGetUserInfo;

	memcpy( l_sGetUserInfo.szUser_Id, pUserId, MAX_ID_LENGTH );
	l_sGetUserInfo.nUserChannel = nGameChannel;

	memcpy( &m_szNetBuff[nIndex], &l_sGetUserInfo, sizeof(l_sGetUserInfo) );
	nIndex += sizeof(l_sGetUserInfo);

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( WAITSENDMODE, (char)NGCS_GETUSERINFO, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::MemoSend( char *pUserNickName, char *pRecvNickName, char *pMsg )
{
	DTRACE("CNetworkSock::MemoSend");

	int nIndex = 0;

	memcpy( &m_szNetBuff[nIndex], pUserNickName, MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;

	memcpy( &m_szNetBuff[nIndex], pRecvNickName, MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;

	int nStrLen = strlen(pMsg) + 1;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nStrLen));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nStrLen));

	memcpy( &m_szNetBuff[nIndex], pMsg, nStrLen );
	nIndex += nStrLen;

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( WAITSENDMODE, (char)NGCS_MEMOSEND, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::CardChoice( int nCardSlot )
{
	DTRACE("CNetworkSock::CardChoice");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	g_ServerSend = false;

	int nIndex = 0;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nCardSlot));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nCardSlot));

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( GAMESENDMODE, (char)NGCS_CARDCHIOCE, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::CardDump( int nCardSlot )
{
	DTRACE("CNetworkSock::CardChoice");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	//g_ServerSend = false;

	int nIndex = 0;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nCardSlot));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nCardSlot));

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( GAMESENDMODE, (char)NGCS_CARDDUMP, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}





// '0':����, '1':üũ, '2':����, '3':��, '4':����, '5':��, '6':Ǯ, '7':����
bool CNetworkSock::UserSelect( char cUserSelect )
{
	DTRACE("CNetworkSock::UserSelect");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	g_ServerSend = false;

	int nIndex = 0;

	m_szNetBuff[nIndex++] = cUserSelect;

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( GAMESENDMODE, (char)NGCS_USERSELECT, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if ( !Send() ) return false;

	return true;	
}

bool CNetworkSock::WaitChat( char *pChatNick, char *pChatMsg, int nEarUserNo )
{
	DTRACE("CNetworkSock::WaitChat");


	if ( !SendServer(WAITROOM_MODE) )
		return false;

	int nIndex = 0;

	memcpy( &m_szNetBuff[nIndex], pChatNick, MAX_NICK_LENGTH);
	nIndex += MAX_NICK_LENGTH;

	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nEarUserNo));  	//������� ���� ��ȣ�� �Ѱ��ش�.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nEarUserNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nEarUserNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nEarUserNo));
	
	int nStrLen = strlen(pChatMsg) + 1;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nStrLen));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nStrLen));

	memcpy( &m_szNetBuff[nIndex], pChatMsg, nStrLen );
	nIndex += nStrLen;

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( WAITSENDMODE, (char)NGCS_WAITCHAT, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GameChat( char *pChatNick, char *pChatMsg, int nEarUserNo )	//�̼��� �����Ѵ�.
{
	DTRACE("CNetworkSock::GameChat");


	if ( !SendServer(GAME_MODE) )
		return false;

	int nIndex = 0;

	memcpy( &m_szNetBuff[nIndex], pChatNick, MAX_NICK_LENGTH);
	nIndex += MAX_NICK_LENGTH;

	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nEarUserNo));  	//������� ���� ��ȣ�� �Ѱ��ش�.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nEarUserNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nEarUserNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nEarUserNo));
	
	int nStrLen = strlen(pChatMsg) + 1;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nStrLen));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nStrLen));

	memcpy( &m_szNetBuff[nIndex], pChatMsg, nStrLen );
	nIndex += nStrLen;

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( GAMESENDMODE, (char)NGCS_GAMECHAT, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;


}


bool CNetworkSock::SendServer( int nCurrentMode, bool bSendCheck )
{
	if ( g_pCMainFrame->m_nCurrentMode != nCurrentMode ) {
		DTRACE("SendServer Mode fail(1)!!!");
		return false;
	}

	if ( bSendCheck ) {//Send�� üũ�ؾ��ϴµ� �۷ι� ������ false�̸� false�̴�.
		if ( g_ServerSend == false ) {
			DTRACE("SendServer Mode fail(2)!!!");
			return false;
		}
	}

	DTRACE("SendServer Mode Succeed!!!");
	return true;
}

bool CNetworkSock::SendQ()
{
	/*
	if ( !g_pGoStop->m_pFloor.RealGame() ) {

		DTRACE("CNetworkSock::SendQ() - fail");
		return false;
	}
	*/

	return true;
}

bool CNetworkSock::LogInServer( char *pUserId )
{
	DTRACE("CNetworkSock::LogInServer");

	int nIndex = 0;

	memcpy( &m_szNetBuff[nIndex], "VER1.1", 10 );
	nIndex += 10;

	memcpy( &m_szNetBuff[nIndex], pUserId, MAX_ID_LENGTH );
	nIndex += MAX_ID_LENGTH;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(g_szGameJong));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(g_szGameJong));

	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( GAMESENDMODE, (char)NGCS_ISLOGIN, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::RoomIn( sRoomIn *psRoomIn )
{
	DTRACE("CNetworkSock::RoomIn");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;
	g_bConnectingSpr = true;

	DTRACE("7-g_ServerSend = false");

	WriteSendQue( WAITSENDMODE, (char)NGCS_ROOMIN, (char)ERROR_NOT, (char *)psRoomIn, sizeof(sRoomIn));					
	if ( !Send() ) return false;

	return true;
}


bool CNetworkSock::GetRoomInfo( int nRoomNo )
{
	DTRACE("CNetworkSock::GetRoomInfo");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	//g_ServerSend = false;

	int nIndex = 0;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nRoomNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nRoomNo));
	
	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( WAITSENDMODE, (char)NGCS_ROOMINFO, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}


bool CNetworkSock::QuickJoin()
{
	DTRACE("CNetworkSock::QuickJoin");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;

	
	g_bConnectingSpr = true;

	DTRACE("8-g_ServerSend = false");


	WriteSendQue( WAITSENDMODE, (char)NGCS_QUICKJOIN, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}


bool CNetworkSock::CreateRoom( sCreateRoom *psCreateRoom )
{
	DTRACE("CNetworkSock::CreateRoom");

	
	if ( !SendServer( WAITROOM_MODE, true )  )
		return false;

	g_ServerSend = false;
	DTRACE("5-g_ServerSend = false");
	

	WriteSendQue( WAITSENDMODE, (char)NGCS_CREATEROOM, (char)ERROR_NOT, (char *)psCreateRoom, sizeof(sCreateRoom));					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::ViewPage( int nViewPage )
{
	DTRACE("CNetworkSock::ViewPage");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;

	DTRACE("9-g_ServerSend = false");

	int nIndex = 0;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nViewPage));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nViewPage));
	
	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( WAITSENDMODE, (char)NGCS_VIEWPAGE, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::KeepAlive()
{
	DTRACE("CNetworkSock::KeepAlive");
	WriteSendQue( WAITSENDMODE, (char)NGCS_KEEPALIVE, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::RoomOut()
{
	DTRACE("CNetworkSock::RoomOut");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	g_ServerSend = false;

	WriteSendQue( WAITSENDMODE, (char)NGCS_ROOMOUT, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}


bool CNetworkSock::GameStart()
{
	DTRACE("CNetworkSock::GameStart");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	g_ServerSend = false;

	DTRACE("8-g_ServerSend = false");


	WriteSendQue( GAMESENDMODE, (char)NGCS_GAMESTART, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GameReStart()
{
	DTRACE("CNetworkSock::GameReStart");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	//g_ServerSend = false;

	DTRACE("8-g_ServerSend = false");


	WriteSendQue( GAMESENDMODE, (char)NGCS_RESTART, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GameViewWait( int nViewPageNo )
{
	DTRACE("CNetworkSock::GameViewWait");
	int nIndex = 0;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nViewPageNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nViewPageNo));

	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( GAMESENDMODE, (char)NGCS_VIEWWAIT, (char)ERROR_NOT, m_szNetBuff, nIndex );
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::UserInvite( int nUserInviteNo, char *pInviteMsg )
{
	DTRACE("CNetworkSock::UserInvite");
	int nIndex = 0;
	
	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nUserInviteNo));  	//������� ���� ��ȣ�� �Ѱ��ش�.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nUserInviteNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nUserInviteNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nUserInviteNo));

	int nStrLen = strlen(pInviteMsg) + 1;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nStrLen));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nStrLen));

	memcpy( &m_szNetBuff[nIndex], pInviteMsg, nStrLen );
	nIndex += nStrLen;

	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( GAMESENDMODE, (char)NGCS_USERINVITE, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GetOut( int nUserUniqNo )
{
	DTRACE("CNetworkSock::GetOut");
	int nIndex = 0;
	
	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nUserUniqNo));  	//������� ���� ��ȣ�� �Ѱ��ش�.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nUserUniqNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nUserUniqNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nUserUniqNo));

	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( GAMESENDMODE, (char)NGCS_GETOUT, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GetOutQues()
{
	DTRACE("CNetworkSock::GetOut");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	//g_ServerSend = false;

	DTRACE("8-g_ServerSend = false");


	WriteSendQue( GAMESENDMODE, (char)NGCS_GETOUTQUESREPL, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::UserChange( sUserChange *psUserChange )
{
	DTRACE("CNetworkSock::UserChange");


	if ( !SendServer(GAME_MODE) )
		return false;

	
	int nIndex = 0;


	memcpy( &m_szNetBuff[nIndex], psUserChange, sizeof(sUserChange) );
	nIndex += sizeof(sUserChange);

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( GAMESENDMODE, (char)NGCS_USERCHANGE, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;

}