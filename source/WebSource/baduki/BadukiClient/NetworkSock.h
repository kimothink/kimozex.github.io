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
	
	CRITICAL_SECTION m_stCriticalSection;
	CObList	m_listSend;
	
public:
	bool RoomIn( sRoomIn *psRoomIn );
	bool SetClientOption( BOOL bInvite, BOOL bEarTalk, BOOL bMemoRecv );
	bool WaitInput( sPKWaitInputCS *psPKWaitInputCS );
	bool CSGetServerList( char *pUserId );							//��������Ʈ�� �����´�.
	bool MemoSend( char *pUserNickName, char *pRecvNickName, char *pMsg );
	bool WaitChat( char *pChatNick, char *pChatMsg, int nEarUserNo );
	bool GameChat( char *pChatNick, char *pChatMsg, int nEarUserNo );
	bool LogInServer( char *pUserId );			//ó�� �α��μ����� �α���.
	bool CreateRoom( sCreateRoom *psCreateRoom ); 
	bool ViewPage( int nViewPage );
	bool KeepAlive(); 
	bool RoomOut();
	bool GetRoomInfo( int nRoomNo );

	bool SendQ();	//Ŭ���̾�Ʈ�鿡�� ������ �Ǵ��� üũ.
	bool SendServer( int nCurrentMode, bool bSendCheck = false );	//������ ������ �Ǵ��� üũ. bSendCheck = true�̸� ��Ŷ�� �������ϴ��� üũ�Ѵ�.	
	BOOL IsError();
	bool QuickJoin();
	bool GameStart();
	bool GetUserInfo( char *pUserId, int nGameChannel );
	bool CardChoice( int nCardSlot );
	bool CardDump( int nCardSlot );
	bool UserSelect( char cUserSelect );	// '0':����, '1':üũ, '2':����, '3':��, '4':����, '5':��, '6':Ǯ, '7':����
	bool GameReStart();
	bool GameViewWait( int nViewPageNo );
	bool UserInvite( int nUserInviteNo, char *pInviteMsg );//0
	bool GetOut( int nUserUniqNo );
    bool GetOutQues();
    bool ChangeUserCard( sUserCardChange *psUserCardChange );
    bool ChangeUserCardEnd();

    //yuno 2006.2.27
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


