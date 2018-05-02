//===========================================================
//======== CNetworkSock.h 게임관련 소켓클래스 정의 ==========
//===========================================================
//#define WIN32_LEAD_AND_MEAN

#ifndef _CNETWORKSOCK_H_
#define _CNETWORKSOCK_H_



#include "NGNCSProtocol.h"
#include "NGClientControl.h"
#include "TxtQ.h"


#pragma comment(lib, "ws2_32.lib")



#define WM_ASYNC     				(WM_USER + 0x51)
#define QUE_TAIL	'\n'


#define MAX_BUFFER_SIZE 5000

//===============================================================
// 게임 관련 네트웍 클래스					=====================
//===============================================================
class CNetworkSock
{
private:
	SOCKADDR_IN m_SocketAddr;

//Send variable
	WSABUF	m_WSASendBuf; 
	int		m_nSendBufIndex;


public:
	CNetworkSock();
	~CNetworkSock();
	void	Destroy();
	bool	InitSocket(HWND hWnd, char *ip, int port);
	bool	CheckSendBuf();
	bool	Recv();
	bool	Send();
	void	OnConnect( int nErrorCode );
	bool	WriteSendQue( char cMode, char cCommand, char cErrorCode, char* SendPacket, int PacketLen );
	
	SOCKET 	m_sClientSocket;
	bool	m_bCanWrite;
	bool	m_bConnected;

	char	m_szSBuffer[MAX_BUFFER_SIZE];

	int		m_nCurPtr;
	char	m_szBufferQ[MAX_BUFFER_SIZE];
	CTxtQ	m_input;		/* q of unprocessed input		*/ 
	CRecvCardQ m_RecvCardInputQ;
	UINT	m_nReveillsize; //받아야할 데이타 크기.

	char	m_szNetBuff[MAX_BUFFER_SIZE];

public:
	//*************************************
	bool CreateRoom( sCreateRoom *psCreateRoom, ush_int nReqRoomNo ); //O				
	bool WaitInput( sPKWaitInputCS *psPKWaitInputCS );//O				
	bool ExitGame();	//현재 안쓴다.
	bool RoomIn( sRoomIn *psRoomIn ); //0
	bool RoomView( sRoomIn *psRoomIn ); //0

	bool RoomOut();	//0
	bool QuickJoin(); //0
	bool WaitChat( char *pChatNick, char *pChatMsg, int nEarUserNo );
	bool ViewPage( int nViewPage ); //0
	bool KeepAlive(); 

	bool TurnMsg( char *pBuffer, int nIndex );

	bool GameEnd( sGameEnd *psGameEnd );
	//bool GetAvatar( char *pBuffer );
	bool GameViewWait( int nViewPageNo );
	bool UserInvite( int nUserInviteNo, char *pInviteMsg );//0
	bool LogInServer( char *pUserId );			//처음 로그인서버로 로그인.
	bool GameOtherEnd( sEndGameOther *l_EndGameOther );				//게임중에 나간놈을 업데이트 해준다.
	bool GameReady();

	
	
	bool SendServer( int nCurrentMode, bool bSendCheck = false );	//서버에 보내도 되는지 체크. bSendCheck = true이면 패킷을 보내야하는지 체크한다.

	bool GetAvatarUrl( int nGetUserNo );

	//bool CSGetServerList( char *pUserId );							//서버리스트를 가져온다.

	//*****CLIENT TO CLIENT
	bool SendQ();	//클라이언트들에게 보내도 되는지 체크.
	bool StartGame( int nSeed, int nCardCnt, BYTE *pCardPack);
	bool SendUserCard( int nChoiceUserCardPos );
	bool ResultOk( int nUniqUserNo );
	bool SendShake( int nChoiceUserCardPos, int *pShakeCardNo );	//흔듬카드.
	bool SendGoStop( int nGoStop);									//상대방이 고스톱 결정.		0 : Stop, 1 : Go
	bool SendNineCard( int nChoiceUserCardPos );					//국진 옮긴다.
	bool UserExitReserv( int nUniQNo, bool bExit );					//true 면 나간다 한다.
	bool MissionSet( int nMissionType, int nMissionMultiply, int nPrizeNo );		//미션을 세팅한다.
	bool GameChat( char *pChatNick, char *pChatMsg, int nEarUserNo );
	bool MemoSend( char *pUserNickName, char *pRecvNickName, char *pMsg );

	//yuno 2005.10.19
	bool SetClientOption( BOOL bInvite, BOOL bEarTalk, BOOL bMemoRecv, int nVoice );


	bool GameSunSet();
	bool SunChoice( int nSunChoiceNo );
	bool AutoChoice();
	bool GetUserInfo( char *pUserId, int nGameChannel );
    bool GetRoomInfo( int nRoomNo );
    
	CObList	m_listSend;
	CRITICAL_SECTION m_stCriticalSection;
	BOOL IsError();


	bool UserChange( sUserChange *psUserChange );

};


/////////////////////////////////////////////////////////////////////////////
// CSockBlock
// 주고받는 데이터 블럭을 담아두기 위한 클래스
// 내부적으로만 사용된다.

class CSockBlock : public CObject
{
// Construction
public:
	CSockBlock(char* pBlock, int nLength);
	virtual ~CSockBlock();

// Attributes
public:
	operator int() { return m_nLength; }
	operator LPCSTR() { return (LPCSTR)m_pBlock; }
	int RemoveHead(int nLength);

protected:
	char* 	m_pBlock;
	int	    m_nLength;
};

#endif


