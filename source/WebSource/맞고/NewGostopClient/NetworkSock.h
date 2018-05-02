//===========================================================
//======== CNetworkSock.h ���Ӱ��� ����Ŭ���� ���� ==========
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
// ���� ���� ��Ʈ�� Ŭ����					=====================
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
	UINT	m_nReveillsize; //�޾ƾ��� ����Ÿ ũ��.

	char	m_szNetBuff[MAX_BUFFER_SIZE];

public:
	//*************************************
	bool CreateRoom( sCreateRoom *psCreateRoom, ush_int nReqRoomNo ); //O				
	bool WaitInput( sPKWaitInputCS *psPKWaitInputCS );//O				
	bool ExitGame();	//���� �Ⱦ���.
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
	bool LogInServer( char *pUserId );			//ó�� �α��μ����� �α���.
	bool GameOtherEnd( sEndGameOther *l_EndGameOther );				//�����߿� �������� ������Ʈ ���ش�.
	bool GameReady();

	
	
	bool SendServer( int nCurrentMode, bool bSendCheck = false );	//������ ������ �Ǵ��� üũ. bSendCheck = true�̸� ��Ŷ�� �������ϴ��� üũ�Ѵ�.

	bool GetAvatarUrl( int nGetUserNo );

	//bool CSGetServerList( char *pUserId );							//��������Ʈ�� �����´�.

	//*****CLIENT TO CLIENT
	bool SendQ();	//Ŭ���̾�Ʈ�鿡�� ������ �Ǵ��� üũ.
	bool StartGame( int nSeed, int nCardCnt, BYTE *pCardPack);
	bool SendUserCard( int nChoiceUserCardPos );
	bool ResultOk( int nUniqUserNo );
	bool SendShake( int nChoiceUserCardPos, int *pShakeCardNo );	//���ī��.
	bool SendGoStop( int nGoStop);									//������ ���� ����.		0 : Stop, 1 : Go
	bool SendNineCard( int nChoiceUserCardPos );					//���� �ű��.
	bool UserExitReserv( int nUniQNo, bool bExit );					//true �� ������ �Ѵ�.
	bool MissionSet( int nMissionType, int nMissionMultiply, int nPrizeNo );		//�̼��� �����Ѵ�.
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
// �ְ�޴� ������ ���� ��Ƶα� ���� Ŭ����
// ���������θ� ���ȴ�.

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


