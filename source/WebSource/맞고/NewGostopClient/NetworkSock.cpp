#include "stdafx.h"

#include "MainFrm.h"
#include "NetworkSock.h"
#include "dtrace.h"
#include "AlarmMsg.h"
#include "ClientDefine.h"

extern CGoStopEngine *g_pGoStop;
extern HWND g_hWnd;

char g_szHeader[2];
extern bool g_ServerSend;
extern CMainFrame *g_pCMainFrame;
extern ush_int g_szGameJong;
extern ush_int g_szGameServerChannel;

/////////////////////////////////////////////////////////////////////////////
// CSockBlock

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
//패킷처리 클래스 구현
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
	// 연결이 되지 않아도 OnConnect가 발생한다.
	if ( nErrorCode != 0 )
	{
		g_pCMainFrame->l_Message->PutMessage( "Server Connect Fail!!!", "Code - 141", TRUE );
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
	//DTRACE("1111");
	if ( g_ServerSend == false ) return false;
	//DTRACE("2222");		

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
	m_nSendBufIndex = NGFramePacket( m_szSBuffer, g_szHeader[0], g_szHeader[1], PacketLen, cMode, cCommand, cErrorCode, SendPacket );

	if ( m_sClientSocket == INVALID_SOCKET )	return	FALSE;
	

	EnterCriticalSection(&m_stCriticalSection); 
	m_listSend.AddTail(new CSockBlock((LPSTR)m_szSBuffer, m_nSendBufIndex));
	LeaveCriticalSection(&m_stCriticalSection);

	return TRUE;
}

BOOL CNetworkSock::IsError()
{
	// 정상적인 상태라고 여길 수 있는 Error의 경우 무시하고
	// 그 외의 경우에는 소켓을 닫는다
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
	// 리스트에 쌓인 내용을 읽어서 한 블럭씩 보낸다

	BOOL nRe;
	EnterCriticalSection(&m_stCriticalSection);
	while (!m_listSend.IsEmpty())
	{
		CSockBlock* pBlock = (CSockBlock*)m_listSend.GetHead();
		int nSend = send( m_sClientSocket, (LPCSTR)*pBlock, (int)*pBlock, 0 );
		//DTRACE("SeneLen = %d", nSend);
		if (SOCKET_ERROR != nSend) 
		{
			if (!pBlock->RemoveHead(nSend))
				delete	m_listSend.RemoveHead();
		} else {
			nRe = FALSE;
			break;
		}
	}

	nRe = TRUE;


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
		
		//받아야할 데이터만큼 왔거나더 왔을때..
		if ( m_nReveillsize <= tRecvBytes ) {
			//딱맞게 왔을때.
			if ( m_nReveillsize == tRecvBytes ) {
				m_nCurPtr = 0;
				m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
			}
			else  {	
				while(1) {
						//받아야할데이타보다 더많이 왔을때.
						m_nCurPtr = 0;
						m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
							
						//남은바이트
						tRecvBytes = tRecvBytes - m_nReveillsize;
						
						if ( tRecvBytes == 0 ) break;
						//남은데이타를 앞으로 땡긴다.
						memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );

						//다음받아야할패킷.
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
		//받아야할 데이타보다 더 적게 왔을때.
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
				//완성된놈을 큐로복사한다.
				memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, m_nReveillsize );	
				m_nCurPtr += m_nReveillsize;
				m_input.WriteToQ( m_szBufferQ, m_nCurPtr);
				memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ));

				//처음과 같아진다.
				tRecvBytes = tRecvBytes - m_nReveillsize;
				memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );
				m_nCurPtr = 0;

				m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );
				
				//받아야할 데이타만큼 왔거나더 왔을때..
				if ( m_nReveillsize <= tRecvBytes ) {
					//딱맞게 왔을때.
					if ( m_nReveillsize == tRecvBytes ) {
						m_nCurPtr = 0;
						m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
					}
					else  {	
						while(1) {
							//받아야할데이타보다 더많이 왔을때.
							m_nCurPtr = 0;
							m_input.WriteToQ( RecvBuf.buf, m_nReveillsize);
										
							//남은바이트
							tRecvBytes = tRecvBytes - m_nReveillsize;
									
							//남은데이타를 앞으로 땡긴다.
							memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );

							//다음받아야할패킷.
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

//================================================
bool CNetworkSock::CreateRoom( sCreateRoom *psCreateRoom, ush_int nReqRoomNo )
{
	//DTRACE("CNetworkSock::CreateRoom");

	
	if ( !SendServer( WAITROOM_MODE, true )  )
		return false;

	g_ServerSend = false;


	int nIndex = 0;

	memcpy( &m_szNetBuff[nIndex], psCreateRoom, sizeof(sCreateRoom) );
	nIndex += sizeof(sCreateRoom);

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nReqRoomNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nReqRoomNo));


	m_szNetBuff[nIndex] = '\0';

	

	WriteSendQue( WAITSENDMODE, (char)NGCS_CREATEROOM, (char)ERROR_NOT, (char *)m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::WaitInput( sPKWaitInputCS *psPKWaitInputCS )
{
	//DTRACE("CNetworkSock::WaitInput");

	if ( !SendServer(LOGO_MODE, true) )
		return false;

	g_ServerSend = false;

	//DTRACE("6-g_ServerSend = false");

	WriteSendQue( WAITSENDMODE, (char)NGCS_WAITINPUT, (char)ERROR_NOT, (char *)psPKWaitInputCS, sizeof(sPKWaitInputCS));					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::ExitGame()
{
	//DTRACE("CNetworkSock::ExitGame");

	if ( !SendServer(CHANNEL_MODE) )
		return false;

	WriteSendQue( WAITSENDMODE, (char)NGCS_EXITGAME, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::RoomIn( sRoomIn *psRoomIn )
{
	//DTRACE("CNetworkSock::RoomIn");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;

	//DTRACE("7-g_ServerSend = false");

	WriteSendQue( WAITSENDMODE, (char)NGCS_ROOMIN, (char)ERROR_NOT, (char *)psRoomIn, sizeof(sRoomIn));					
	if ( !Send() ) return false;



	return true;
}
bool CNetworkSock::RoomView( sRoomIn *psRoomIn )
{
	//DTRACE("CNetworkSock::RoomIn");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;
	//DTRACE("7-g_ServerSend = false");
	WriteSendQue( WAITSENDMODE, (char)NGCS_ADMINJUSTVIEW, (char)ERROR_NOT, (char *)psRoomIn, sizeof(sRoomIn));					
	if ( !Send() ) return false;


	return true;
}
bool CNetworkSock::RoomOut()
{
	//DTRACE("CNetworkSock::RoomOut");

	if ( !SendServer(GAME_MODE, true) )
		return false;

	//혹시 게임이 시작될지 모르니...
	g_pGoStop->m_bGameStop = true;

	g_ServerSend = false;

	WriteSendQue( WAITSENDMODE, (char)NGCS_ROOMOUT, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::QuickJoin()
{
	//DTRACE("CNetworkSock::QuickJoin");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;

	//DTRACE("8-g_ServerSend = false");


	WriteSendQue( WAITSENDMODE, (char)NGCS_QUICKJOIN, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::WaitChat( char *pChatNick, char *pChatMsg, int nEarUserNo )
{
	//DTRACE("CNetworkSock::WaitChat");


	if ( !SendServer(WAITROOM_MODE) )
		return false;

	int nIndex = 0;

	memcpy( &m_szNetBuff[nIndex], pChatNick, MAX_NICK_LENGTH);
	nIndex += MAX_NICK_LENGTH;

	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nEarUserNo));  	//사용자의 고유 번호를 넘겨준다.
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

bool CNetworkSock::ViewPage( int nViewPage )
{
	//DTRACE("CNetworkSock::ViewPage");

	if ( !SendServer(WAITROOM_MODE, true) )
		return false;

	g_ServerSend = false;

	//DTRACE("9-g_ServerSend = false");

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
	//DTRACE("CNetworkSock::KeepAlive");
	WriteSendQue( WAITSENDMODE, (char)NGCS_KEEPALIVE, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}


bool CNetworkSock::TurnMsg( char *pBuffer, int nIndex )
{
	//DTRACE("CNetworkSock::TurnMsg");

	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, pBuffer, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GameEnd( sGameEnd *psGameEnd )
{
	//DTRACE("CNetworkSock::GameEnd");

	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCS_GAMEEND, (char)ERROR_NOT, (char*)psGameEnd, sizeof(sGameEnd));					
	if ( !Send() ) return false;

	return true;
}

/*
bool CNetworkSock::GetAvatar( char *pBuffer )
{
	DTRACE("CNetworkSock::GetAvatar");
	WriteSendQue( GAMESENDMODE, (char)NGCS_GETAVATAR, (char)ERROR_NOT, pBuffer, MAX_GET_AVATARCNT );					
	if ( !Send() ) return false;

	return true;
}
*/

bool CNetworkSock::GetUserInfo( char *pUserId, int nGameChannel )
{
	//DTRACE("CNetworkSock::GetUserInfo");

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

bool CNetworkSock::GetAvatarUrl( int nGetUserNo ) 
{
	if ( !SendServer( WAITROOM_MODE, true )  )
		return false;

	//DTRACE("CNetworkSock::GetAvatarUrl");
	int nIndex = 0;
	
	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nGetUserNo));  	//사용자의 고유 번호를 넘겨준다.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nGetUserNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nGetUserNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nGetUserNo));

	m_szNetBuff[nIndex] = '\0';

	WriteSendQue( WAITSENDMODE, (char)NGCS_GETAVAURL, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::GameViewWait( int nViewPageNo )
{
	//DTRACE("CNetworkSock::GameViewWait");
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
	//DTRACE("CNetworkSock::UserInvite");
	int nIndex = 0;
	
	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nUserInviteNo));  	//사용자의 고유 번호를 넘겨준다.
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

bool CNetworkSock::LogInServer( char *pUserId )
{
	//DTRACE("CNetworkSock::LogInServer");

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


/*
bool CNetworkSock::CSGetServerList( char *pUserId )
{
	DTRACE("CNetworkSock::LogInServer");
	WriteSendQue( GAMESENDMODE, (char)NGCS_GETSERVERLIST, (char)ERROR_NOT, pUserId, MAX_ID_LENGTH);					
	if ( !Send() ) return false;

	return true;
}
*/
bool CNetworkSock::GameOtherEnd( sEndGameOther *l_EndGameOther )
{
	//DTRACE("CNetworkSock::GameOtherEnd");

	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCS_GAMINGEXITUSER, (char)ERROR_NOT, (char *)l_EndGameOther, sizeof(sEndGameOther) * MAX_ROOM_IN);
	if ( !Send() ) return false;

	return true;

}



bool CNetworkSock::GameReady()
{
	//DTRACE("CNetworkSock::GameReady");

	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCS_STARTREADY, (char)ERROR_NOT, NULL, 0 );
	if ( !Send() ) return false;

	return true;

}


bool CNetworkSock::StartGame( int nSeed, int nCardCnt, BYTE *pCardPack)
{
	//DTRACE("CNetworkSock::StartGame");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nSeed));  	//사용자의 고유 번호를 넘겨준다.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nSeed));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nSeed));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nSeed));

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nCardCnt));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nCardCnt));

	memcpy( &m_szNetBuff[nIndex], pCardPack, nCardCnt);

	nIndex += nCardCnt;

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_STARTGAME, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;
}


/* ham 2006.03.01
bool CNetworkSock::UserChange( sUserChange *psUserChange )
{
	//DTRACE("CNetworkSock::GameOtherEnd");

	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCC_CARDCHANGE, (char)ERROR_NOT, (char *)psUserChange, sizeof(sUserChange) );
	if ( !Send() ) return false;

	return true;

}
*/



bool CNetworkSock::UserChange( sUserChange *psUserChange )
{
 //DTRACE("CNetworkSock::GameOtherEnd");
 if (!SendQ()) return false;

 int nIndex = 0;
 int nRealPayloadlen;

 nIndex += SC_HEADER_SIZE + CCPAKETLEN;
 
 memcpy( &m_szNetBuff[nIndex], psUserChange, sizeof(sUserChange) );
 nIndex += sizeof(sUserChange);

 m_szNetBuff[nIndex] = '\0';

 nRealPayloadlen = nIndex - CCPAKETLEN;
 
 m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
 m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

 NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_CARDCHANGE, ERROR_NOT);

 WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
 if (!Send()) return false;

 return true;
}



bool CNetworkSock::SendUserCard( int nChoiceUserCardPos ) 
{
	//DTRACE("CNetworkSock::SendUserCard");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nChoiceUserCardPos));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nChoiceUserCardPos));

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_SENDUSERCARD, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;

}

bool CNetworkSock::GameSunSet() 
{
	//DTRACE("CNetworkSock::GameReady");

	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCS_GAMESUNSET, (char)ERROR_NOT, NULL, 0 );
	if ( !Send() ) return false;

	return true;

}

bool CNetworkSock::SunChoice( int nSunChoiceNo )
{
	//DTRACE("CNetworkSock::SunChoice");


	if ( !SendServer(GAME_MODE) )
		return false;

	int nIndex = 0;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nSunChoiceNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nSunChoiceNo));

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( GAMESENDMODE, (char)NGCS_SUNCHOICE, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::AutoChoice( )
{
	//DTRACE("CNetworkSock::AutoChoice");


	if ( !SendServer(GAME_MODE) )
		return false;

	WriteSendQue( GAMESENDMODE, (char)NGCS_AUTOCHOICE, (char)ERROR_NOT, NULL, 0);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::SendNineCard( int nChoiceUserCardPos ) 
{
	//DTRACE("CNetworkSock::SendNineCard");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nChoiceUserCardPos));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nChoiceUserCardPos));

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_SENDNINECARD, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;

}


bool CNetworkSock::ResultOk( int nUniqUserNo )
{
	//DTRACE("CNetworkSock::ResultOk");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nUniqUserNo));  	//사용자의 고유 번호를 넘겨준다.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nUniqUserNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nUniqUserNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nUniqUserNo));
	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_RESULTOK, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;

}

bool CNetworkSock::GetRoomInfo( int nRoomNo )
{
	//DTRACE("CNetworkSock::GetRoomInfo");

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


bool CNetworkSock::SendShake( int nChoiceUserCardPos, int *pShakeCardNo )
{
	//DTRACE("CNetworkSock::SendShake");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nChoiceUserCardPos));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nChoiceUserCardPos));

	//DTRACE("%d, %d, %d, %d", pShakeCardNo[0], pShakeCardNo[1], pShakeCardNo[2], pShakeCardNo[3]);
	memcpy( &m_szNetBuff[nIndex], pShakeCardNo, sizeof(int) * 5 );
	//DTRACE("%d", m_szNetBuff[nIndex]);

	nIndex += sizeof(int) * 5;

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_CARDSHAKE, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;
}


bool CNetworkSock::SendGoStop( int nGoStop )
{
	//DTRACE("CNetworkSock::SendGoStop");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nGoStop));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nGoStop));

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_GOSTOP, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;
}

bool CNetworkSock::UserExitReserv( int nUniQNo, bool bExit )
{
	//DTRACE("CNetworkSock::UserExitReserv");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;

	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nUniQNo));  	//사용자의 고유 번호를 넘겨준다.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nUniQNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nUniQNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nUniQNo));

	memcpy( &m_szNetBuff[nIndex], &bExit, sizeof(bool) );
	nIndex += sizeof(bool);

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_USEREXITRESERV, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;
}

bool CNetworkSock::MissionSet( int nMissionType, int nMissionMultiply, int nPrizeNo )							//미션을 세팅한다.
{
	//DTRACE("CNetworkSock::MissionSet");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nMissionType));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nMissionType));

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nMissionMultiply));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nMissionMultiply));

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nPrizeNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nPrizeNo));


	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_MISSIONSET, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;


}

bool CNetworkSock::GameChat( char *pChatNick, char *pChatMsg, int nEarUserNo )	//미션을 세팅한다.
{
	//DTRACE("CNetworkSock::GameChat");

	if (!SendQ()) return false;

	int nIndex = 0;
	int nRealPayloadlen;

	nIndex += SC_HEADER_SIZE + CCPAKETLEN;
	
	memcpy( &m_szNetBuff[nIndex], pChatNick, MAX_NICK_LENGTH);
	nIndex += MAX_NICK_LENGTH;

	m_szNetBuff[nIndex++] = HIBYTE(HIWORD(nEarUserNo));  	//사용자의 고유 번호를 넘겨준다.
	m_szNetBuff[nIndex++] = LOBYTE(HIWORD(nEarUserNo));
	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nEarUserNo));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nEarUserNo));
	
	int nStrLen = strlen(pChatMsg) + 1;

	m_szNetBuff[nIndex++] = HIBYTE(LOWORD(nStrLen));
	m_szNetBuff[nIndex++] = LOBYTE(LOWORD(nStrLen));

	memcpy( &m_szNetBuff[nIndex], pChatMsg, nStrLen );
	nIndex += nStrLen;

	m_szNetBuff[nIndex] = '\0';

	nRealPayloadlen = nIndex - CCPAKETLEN;
	
	m_szNetBuff[0] = HIBYTE(LOWORD(nRealPayloadlen));
	m_szNetBuff[1] = LOBYTE(LOWORD(nRealPayloadlen));

	NGTURNFrameHeader(m_szNetBuff, nRealPayloadlen, NGCC_GAMECHAT, ERROR_NOT);

	WriteSendQue(GAMESENDMODE, (char)NGCS_TURNMSG, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if (!Send()) return false;

	return true;


}

bool CNetworkSock::SendQ()
{
	if ( !g_pGoStop->m_pFloor.RealGame() ) {

		//DTRACE("CNetworkSock::SendQ() - fail");
		return false;
	}

	return true;
}

bool CNetworkSock::SendServer( int nCurrentMode, bool bSendCheck )
{
	if ( g_pCMainFrame->l_nCurrentMode != nCurrentMode ) {
		//DTRACE("SendServer Mode fail(1)!!!");
		return false;
	}

	if ( bSendCheck ) {//Send를 체크해야하는데 글로벌 변수가 false이면 false이다.
		if ( g_ServerSend == false ) {
			//DTRACE("SendServer Mode fail(2)!!!");
			return false;
		}
	}

	//DTRACE("SendServer Mode Succeed!!!");
	return true;
}

bool CNetworkSock::MemoSend( char *pUserNickName, char *pRecvNickName, char *pMsg )
{
	//DTRACE("CNetworkSock::MemoSend");

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


	WriteSendQue( ALLSENDMODE, (char)NGCS_MEMOSEND, (char)ERROR_NOT, m_szNetBuff, nIndex);					
	if ( !Send() ) return false;

	return true;
}

bool CNetworkSock::SetClientOption( BOOL bInvite, BOOL bEarTalk, BOOL bMemoRecv, int nVoice )
{
	//DTRACE("CNetworkSock::SetClientOption");

	sOption l_sOption;
	int nIndex = 0;

	l_sOption.bInvite = bInvite;
	l_sOption.bEarTalk = bEarTalk;
	l_sOption.bMemoRecv = bMemoRecv;
	l_sOption.nVoice = nVoice;


	memcpy( &m_szNetBuff[nIndex], &l_sOption, sizeof(l_sOption) );
	nIndex += sizeof(l_sOption);

	m_szNetBuff[nIndex] = '\0';


	WriteSendQue( ALLSENDMODE, (char)NGCS_SETOPTION, (char)ERROR_NOT, m_szNetBuff, nIndex);
	if ( !Send() ) return false;

	return true;
}
