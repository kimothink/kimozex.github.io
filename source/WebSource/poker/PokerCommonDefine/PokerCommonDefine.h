#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f �����ϸ� �Ѿ����~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef __int64	BigInt;

#define PAGE_ROOMCNT				10									//���������� ���.
#define CHANNEL_PER_MASHINE			10
#define MASHINE_CNT					10



#define MAX_ROOM_IN					5					//��Ŀ �ѹ濡 7�� ����.	
#define MAX_NICK_LENGTH				21				
#define MAX_CHATMSG					255					//ä�ø޼����� �ִ����.
#define	MAX_IP_LEN					30
#define MAX_ID_LENGTH				50


#define MAX_AVAURLLEN				20					//�ƹ�Ÿ Url����.
#define MAX_CHANNELPER				100					//��ä���� Client�Ѽ�.
#define MAX_SERVERNAME				20
#define	MAX_WAIT_ROOM				10					//���ǿ��� ���� ���̴� ����.
#define MAX_ROOM_NAME				20
#define MAX_ROOMCNT					100
#define MAX_PAGE_CNT				MAX_ROOMCNT / PAGE_ROOMCNT			//100 / 10 = 10
#define MAX_ROOM_PASSWORD			10	
#define MAX_USER_CARD				7					//������ ������ �ִ� ��ī��.
#define MAX_CHANNEL					10									//ä�μ�
#define MAX_INIT_CARD				4					//�ʱ� ī�� �����ִ�
#define MAX_CARD					52
#define MAX_GAMESTART_MEM			2					//���ӽ��� �ο�

#define MAX_GAMEVIEWWAIT			30									//���ӹ濡�� �ʴ��� ��� ����.

#define MAX_GAMESTRMONEYLEN			25					
#define NOTICE_BYTE					100


#define MAX_PIRZECNT				11	// 

//#define MAX_MONEY_STRING			25					//�����Ӵ� ��Ʈ��.


//****************************************************************************************
//Protocol ����.
//****************************************************************************************
//S->C
typedef struct tagCSHeaderPkt
{
	char 	szHeader[2];			//'N''G'
	ush_int	nTPacketSize;
	char	cCmd;
	char	cErrorCode;
} sSCHeaderPkt;


//yun 2004.10.19
//C -> S HEADER  
typedef struct tagSCHeaderPkt
{ 
	char	szHeader[2];
	ush_int nTPacketSize;
	char	cMode;				//0 : GameMode,   1 : PlayMode
	char	cCmd;
	char	cErrorCode;
} sCSHeaderPkt;



//Structure
typedef struct tagPKFirst
{
	char szHeader[2];
	int nUserNo;
	char szTail[2];
} sPKFirst;

	typedef struct tagUserInfo	//Ŭ���̾�Ʈ���� �����ִ� ���� ����
	{
		bool bInvite;
		char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
		BigInt	biUserMoney;				//������ ��.
		ush_int nClass;						//���
		BigInt	biMaxMoney;					//���ݲ� �ְ�� ���� ����.
		float	fWinRate;					//�·�.
		ush_int	nWin_Num;					//�̱�Ƚ��.
		ush_int	nLose_Num;					//����Ƚ��.
		ush_int	nDraw_Num;					//���º�Ƚ��.
		ush_int	nCut_Time;					//����Ƚ��.
		ush_int nChannel;					//������ ä��
		int nUserNo;						//������ �����ѹ�
		char cSex;							//'0' ���� '1'����
		char szPreAvatar[MAX_AVAURLLEN];	//��ǥ�ƹ�Ÿ.
	} sUserInfo;


typedef struct tagUserList	//Server�� ������ �ִ� ��������
{
	char szUser_Id[MAX_ID_LENGTH];		//�������̵�
	sUserInfo l_sUserInfo;
} sUserList;

typedef struct tagOption {
	bool bInvite;	//true : �ʴ밡��
	bool bEarTalk;  //true : �Ӹ�����
	bool bMemoRecv; //true : ��������.
} sOption;

typedef struct tagPKWaitInputCS
{
	sUserList l_sUserInfo;
	sOption l_sOption;
} sPKWaitInputCS;


typedef struct tagLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;
	sUserList l_sUserList;

} sLogIn;


typedef struct tagIsLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;

} sIsLogIn;

typedef struct tagGetUserInfo
{

	char szUser_Id[MAX_ID_LENGTH];		//�������̵�
	ush_int nUserChannel;

} sGetUserInfo;


typedef struct tagServrMashineList
{
	ush_int nMashineNo;
	char szServerMashineName[MAX_SERVERNAME];
	char szServerMashineIp[MAX_IP_LEN];
	UINT nPort;
	char szChannelName[MAX_SERVERNAME];
	ush_int nTotCnt;
	ush_int nCurCnt[CHANNEL_PER_MASHINE];
} sServrMashineList;

typedef struct tagBroadServerInfo
{
	ush_int nGSNo;
	ush_int nChannel;
	ush_int nCurCnt;
} sBroadServerInfo;

	typedef struct tagRoomInfo	//Ŭ���̾�Ʈ���� �����ִ� ������
	{
		short int nRoomNo;					//���ȣ
		char szRoomName[MAX_ROOM_NAME];		//������
		ush_int nCurCnt;					//�����ο�
		ush_int nTotCnt;					//�����ο�
		bool bSecret;						//��й� ���� ( true : ��й�, false : ������ )
		ush_int nGameDivi;					//0 : �����, 1 : ��ŸƮ��ư�����, 2 : ������, 3 : ������.
		BigInt biRoomMoney;					//�ǵ�.
		BOOL bChoiceRoom;					//0 : �׳ɹ�, 1 : ���̽���.
	} sRoomInfo;


typedef struct tagRoomIn
{
	ush_int nRoomNo;
	char szRoomPass[MAX_ROOM_PASSWORD];
} sRoomIn;

//���� ���ӹ濡 ������.
typedef struct tagRoomInAdd
{
	ush_int nRoomInUserNo;
	ush_int nGameDivi;
	BigInt biRoomCurMoney;
} sRoomInAdd;


	typedef struct tagCreateRoom
	{
		bool bSecret;						//������ false ��й� true�ϰ�� 
		char szRoomName[MAX_ROOM_NAME];		//����
		char szRoomPass[MAX_ROOM_PASSWORD];	
		ush_int nTotCnt;					//�� ���ο� 
		BigInt biRoomMoney;
		BOOL bChoiceRoom;					//TRUE �� ���̽���,.
	} sCreateRoom;	


typedef struct tagOnCreateGame
{
	sRoomInfo l_sRoomInfo;
	int nUserNo;
} sOnCreateRoom;

//�濡�� ������ �ٷ� �������� �Ǵ� ������ ����
typedef struct tagRoomInfoToWaitUserAX
{
	ush_int		nRoomNo;
	int			nUserNo;
	ush_int		nRoomCnt;
} sRoomInfoToWaitUserAX;


//�濡�� ��������.
typedef struct tagRoomInfoToWaitUserR
{
	ush_int		nRoomNo;
	ush_int		nRoomCnt;
	sUserInfo	l_sUserInfo;

} sRoomInfoToWaitUserR;

typedef struct tagRoomInUserSend
{
	bool		bRoomMaster;
	bool		bBbangJjang;
	sUserInfo	l_sUserInfo;
	int nGameJoin;	//0 : �����, 1 : ������
	int	nCardCnt;
	BOOL bChoice;
	BYTE nUserCard[MAX_USER_CARD];
	//BYTE szJokboCard[8];
} sRoomInUserSend;



//���������ÿ� �ѷ���.
typedef struct tagRoomOutOtherSend
{
	int			nUserNo;
	int		nNewRoomMaster; //�����Ͱ��ٲ��� ������ 0�̴�. 0�� �ƴϸ� �������� ��ȣ�� ����.
	int		nNewBbangJjnag; //���尡�ٲ��� ������ 0�̴�. 0�� �ƴϸ� ������ ��ȣ�� ����.
	ush_int		nRoomCurCnt;
} sRoomOutOtherSend;

typedef struct tagRoomList	//������ ������ �ִ� ������
{
	sRoomInfo l_sRoomInfo;
	char szRoom_Pass[MAX_ROOM_PASSWORD];
} sRoomList;

//�濡�� ���ӽ����̳� ������ ��������.
typedef struct tagRoomGameWait
{
	ush_int		nRoomNo;
	char		cGameWait;			// 'G' Game ����   'W' ��� ����.
} sRoomGameWait;



//////////////////////////////////////////////////////////////////////////
///Poker���ӿ��� �ʿ��� ����
//////////////////////////////////////////////////////////////////////////
typedef struct tagStartUserCard
{
	BYTE byCardNo[MAX_INIT_CARD];
	int nRoomMasterUniqNo;
	int nBbanJjangUniqNo;
	int nCardCnt;
	int nPrizeDecide;
	int nPrizeNo;
	BigInt biRoomCurMoney;
	BigInt biJackPotMoney;
	BOOL bChoiceRoom;
} sStartUserCard;


typedef struct tagStartPerUserCard
{
	int nCardCnt;
	int nUserUniqNo;
	BYTE byCardNo[MAX_INIT_CARD];
} sStartPerUserCard;


typedef struct tagStartTotUserCard
{
	int nRoomMasterUniqNo;
	int nBbanJjangUniqNo;
	BigInt biRoomCurMoney;
	BigInt biJackPotMoney;
	BOOL bChoiceRoom;
	sStartPerUserCard l_sStartPerUserCard[MAX_ROOM_IN];
} sStartTotUserCard;


typedef struct tagGameCurUser
{
	BOOL bCall;
	BigInt biCall;
	BOOL bHalf;
	BigInt biHalf;
	BOOL bBing;
	BigInt biBing;
	BOOL bDdaDang;
	BigInt biDdaDang;
	BOOL bCheck;
	BigInt biCheck;
	BOOL bDie;
	BigInt biDie;	
	BOOL bFull;
	BigInt biFull;	
	BOOL bQuater;
	BigInt biQuater;	
} sGameCurUser;

typedef struct tagUserOneCardInfo
{
	int nUserUniqueNo;
	BYTE byCardNo;
} sUserOneCardInfo;

typedef struct tagGetOneCard
{
	int nRound;
	int nRoomMasterUniqNo;
	BigInt biRoomCurMoney;
	int nCallUserUniNo;
	char cUserSelect;
	BigInt biCallUserMoney;
	sGameCurUser l_sGameCurUser;
	sUserOneCardInfo l_sUserOneCardInfo[MAX_ROOM_IN];
	BYTE szJokboCard[MAX_ROOM_IN][8];
	//BigInt biUserDieMoney;								//������ ���������� �� �縲 �ݾ��� ȭ�鿡 �ѷ��ֱ�����.
	//BYTE szFixJokboCard[MAX_ROOM_IN][8];
} sGetOneCard;

typedef struct tagUserChange
{
	int nUserUnqiNo;
	BYTE bUerCardNo;
	BYTE bChangeCardNo;
} sUserChange;

typedef struct tagMyUserJokBo
{
	BYTE szJokboCard[8];
} sMyUserJokBo;

typedef struct tagInitUserCard
{
	int nUserUniqNo;
	BYTE nUserCard[MAX_USER_CARD];
	
} sInitUserCard;


typedef struct tagAllUserJokBo
{
	int nUserUniqNo[MAX_ROOM_IN];
	sMyUserJokBo l_sMyUserJokBo[MAX_ROOM_IN];
} sAllUserJokBo;


typedef struct tagChoiceViewCard
{
	int nCallUserUniNo;
	BYTE byCardNo;
} sChoiceViewCard;

typedef struct tagSetGameCurUser
{
	int nBeforeGameUserUniNo;
	char cBeforeUserSelect;
	BigInt biBeforeGameUserMoney;
	int nCurGameUserUniNo;
	BigInt biRoomCurMoney;
	//BigInt biUserDieMoney;
	sGameCurUser l_sGameCurUser;

} sSetGameCurUser;

typedef struct tagUserTotalCardInfo
{
	int nUserUniqueNo;
	BYTE byCardNo[MAX_CARD];
} sUserTotalCardInfo;

	typedef struct tagEndGame
	{
		int nBeforeGameUserUniNo;	//6��° ������ �ѹ�.
		int nWinnerUserUniNo;	//�¸���.
		BigInt biWinnerUserMoney;
		BigInt biPureWinnerMoney;	//�¶����� ���� ���Ӵ�.
		int nCallUserUniNo;	//������ ���ѻ�� ���� ���ѻ��������ȣ.
		BigInt biCallGameUserMoney;
		char cUserSelect;	//�������� ��.
		//BigInt biUserDieMoney;
		sUserTotalCardInfo l_sUserTotalCardInfo[MAX_ROOM_IN];	
		BigInt l_biUserMoney[MAX_ROOM_IN];	//���� ������ �Ӵ�. ����� ������� �ϱ⶧��..���� �����ִ°� Ŭ���̾�Ʈ�� ����
		BigInt l_biPureUserWinLoseMoney[MAX_ROOM_IN];	//������ ���� ���� ���� �Ӵ�.
		ush_int nJackPot;			//0: No JackPot, 1 : StiFul, 2 : RotiFul
		BigInt biUserJackPotMoney;		//������ ���� ��÷ �Ӵ�
		BigInt biRoomCurMoney;
		BYTE szJokboCard[MAX_ROOM_IN][8];
		BYTE szFixJokboCard[MAX_ROOM_IN][8];
	} sEndGame;

	typedef struct tagMyWinGame
	{
		int nWinnerUserUniNo;	//�¸���.
		BigInt biWinnerUserMoney;	//�¸���, �� ���� �Ӵ�.
		BigInt biPureWinnerMoney;		//�¶����� ���� ���Ӵ�.
		int nCallUserUniNo;	//������ ���ѻ�� ���� ���ѻ��������ȣ.
		BigInt biCallGameUserMoney;
		char cUserSelect;	//�������� ��.
		BigInt biRoomCurMoney;
		//BigInt biUserDieMoney;
		int l_nUserUniqNo[MAX_ROOM_IN];
		BigInt l_biUserMoney[MAX_ROOM_IN];	//���� ������ �Ӵ�. ����� ������� �ϱ⶧��..���� �����ִ°� Ŭ���̾�Ʈ�� ����
		BigInt l_biPureUserWinLoseMoney[MAX_ROOM_IN];	//������ ���� ���� ���� �Ӵ�.
	} sMyWinGame;


	typedef struct tagUserDBUpdateInfo
	{
		BigInt m_biRaceMoney;										//������ ���� �Ӵ�
		BigInt biRealRaceMoney;										//���̵� �Ӵ� ����.
		int nWinLose;												//0 : �й�, 1 : �¸� 
		int nCutTime;												//0 : ����, 1 : ����
		BigInt biRoomMoney;											//�� �⺻�Ӵ�
		char szOtherGameUser[MAX_ROOM_IN - 1][MAX_NICK_LENGTH];		//���� ������ ������
		ush_int nJackPot;											//0: No JackPot, 1 : StiFul, 2 : RotiFul
		BigInt biUserJackPotMoney;									//������ ���� ��÷ �Ӵ�
		BigInt biSystemMoney;										//������ ������ �ý��� �Ӵ�.
		int nSucceedCode;
		int nPrizeNo;
	} sUserDBUpdateInfo;

	typedef struct tagGameWaitInfo	//�����ǿ��� ���� ������ ���°�..
	{
		char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
		BigInt	biUserMoney;				//������ ��.
		int nUserNo;						//������ �����ѹ�
		char cSex;							//'0' ���� '1'����
	} sGameWaitInfo;



typedef struct tagUserInvite	//�����ʴ븦 ������.
{
	ush_int nRoomNo;
	BigInt	biRoomMoney;				
	char	szRoomPass[MAX_ROOM_PASSWORD];
	ush_int	nInviteMsgLen;					//�̱�Ƚ��.
} sUserInvite;


typedef struct tagRoomInOtherSend
{
	ush_int nNextNo;
	ush_int	nRoomCurCnt;				
	int	nBbangJjangUniqNo;					
} sRoomInOtherSend;


typedef struct tagNotice
{
	char	m_Notice1[NOTICE_BYTE];
	char	m_Notice2[NOTICE_BYTE];
} sNotice;


#define SC_HEADER_SIZE			sizeof(sSCHeaderPkt)   // 6
#define CS_HEADER_SIZE			sizeof(sCSHeaderPkt)   // 7





#define WAITSENDMODE		0
#define GAMESENDMODE		1
#define ALLSENDMODE			2


//Server -> Client
#define NGSC_WAITINPUT				0x00		//���Ƿ� ���� ���� ������ �޾ƿ´�.
#define NGSC_CREATEROOM				0x01		//���� �����.
#define NGSC_ROOMIN					0x02		//�濡 ���´�.
#define NGSC_USERINFOCHANGE			0x03		//��������Ʈ�� ��ȭ�� ���涧.
#define NGSC_WAITINFOCHANGE			0x04		//���ǿ� ��ȭ�� ���涧( �븮��Ʈ + ��������Ʈ )
#define NGSC_WAITCHAT				0x05		//���� ä��.
#define NGSC_VIEWPAGE				0x06		//�븮��Ʈ�� ����.
#define NGSC_ROOMOUT_OTHERSEND		0x07		//�濡�� ������ ��������..�ٸ�����鿡�� ������ ����.
#define NGSC_ROOMIN_OTHERSEND		0x08		//�濡�� ������ ���ö�..�ٸ�����鿡�� ������ ����.
#define NGSC_QUICKJOIN				0x09		
#define NGCSC_BROADSERVERINFO		0x0A		//�������� ī���͸� �ǽð� �������ش�.
#define NGSC_GETSERVERLIST			0x0B		//Client�� ī���� ������ ������ ���� ��û.
#define NGSC_ALLCMT					0x0C
#define NGSC_IPCHECK				0x0D		//�����濡�� �Ѿ����Ǹ� ���ƾ��Ѵ�.
#define NGSC_ROOMINFO				0x0E		//���ǿ��� ���� Ŭ�������� �������� �ѷ��ش�.
#define NGSC_GAMECHAT				0x0F
#define NGSC_STARTGAME				0x10		//���� ���۽ÿ� �ʱ� ī�� ������ �ش�.
#define NGSC_GETUSERINFO			0x11		//
#define NGSC_CARDCHOICE				0x12		//ó�� ������ �ϳ��� ���̽��Ѵ�.
#define NGSC_GETONECARD				0x13		//������ ������ �ش�
#define NGSC_SETGAMECURUSER			0x14		//�ڽ��� �����ʿ� �����ǵ�, ��ư���õ��� �����ش�.
#define NGSC_ENDGAME				0x15		//������ ���ڿ� �� ������ ī�� �� �� �� ���� ������.
#define NGSC_MYWINGAME				0x16		//��ǽ�.
#define NGSC_NAGARIGAME				0x17
#define NGSC_STARTBTN_ONOFF			0x18		//��ŸƮ ��ư �� ����
#define NGSC_VIEWWAIT				0x19		//���ӹ濡�� ���� ��������.
#define NGSC_USERINVITE				0x20		//������ �ʴ�.
#define NGSC_CARDDUMP				0x21		//���� ����.
#define NGSC_CHOICECARDVIEW			0x22		//���̽�ī�带 ��ο��� �����ش�. �ѹ��� �������ؼ�.
#define NGSC_MYUSERJOKBO			0x23		//�� ����.
#define NGSC_GETOUT					0x24		//����.
#define NGSC_MEMO					0x25		//���� ������.
#define NGSC_GETOUTQUES				0x26		//�����Ǹ� ���Ѵ�.
#define NGSC_ALLUSERJOKBO			0x27		//��� ����� ������ ������.
#define NGSC_INITUSERCARD			0x28		//�ʱ� ������ ���� ī�带 �ѹ��� �ٽ� �����ش�.
#define NGSC_USERCHANGE				0x29		//������ ���ϴ� ī��� �ٲ��ش�.

//Client -> Server
//WaitMode
#define NGCS_WAITINPUT				0		//���Ƿ� ���� ���� ������ �޾ƿ´�.
#define NGCS_CREATEROOM				1		//���� �����.
#define NGCS_ROOMIN					2		//�濡 ����.
#define NGCS_ROOMOUT				3		//���� ������.
#define NGCS_QUICKJOIN				4		//������.
#define NGCS_WAITCHAT				5		//���� ä��.
#define NGCS_VIEWPAGE				6		//�븮��Ʈ�� ����.
#define NGCS_ROOMINFO				7		//���ǿ��� ���� Ŭ�������� �������� �ѷ��ش�.
#define NGCS_GETUSERINFO			9		
#define NGCS_MEMOSEND				10		//���� ������.

// -- LOGINSERVER �κп� ���� #define NGCS_KEEPALIVE				0x08		//KeepAlive


//GameMode
#define NGCS_GAMECHAT				0		
#define NGCS_GAMESTART				1
#define NGCS_CARDCHIOCE				2
#define NGCS_USERSELECT				3		//call, quater �� ����.
#define NGCS_RESTART				4		//������ ������ ������ ������ �ٽ� �����Ϸ� �Ҷ�.
#define NGCS_VIEWWAIT				5		//���ӹ濡�� ���� ��������.
#define	NGCS_USERINVITE				6		//������ �ʴ��Ѵ�.
#define NGCS_CARDDUMP				7		//ī�� ���� ������.
#define NGCS_GETOUT					8		//���� ����.
#define NGCS_GETOUTQUESREPL			9		//���� ���� ���� ���� �亯.
#define NGCS_USERCHANGE				10		//���� ī�带 �ٲ۴�.

//AllMODE
#define NGCS_SETOPTION				0		//�ɼ�����.
#define NGCS_ALLCMT					2		//��ο��� ������ ������. �ٲٸ� �ȵ�.


//CS server Mode
#define NGCS_GETSERVERLIST			0x00		//Client�� ī���� ������ ������ ���� ��û.


//////////////////////////////////////////////////////////////////////////
//LOGIN SERVER�� ����
//////////////////////////////////////////////////////////////////////////
//Client -> Server
//��������� ������ ��¿�� ����.
#define NGCS_ISLOGIN				0		//Client�� LogInServer�� �����Ҷ� ���� Command
#define NGCS_KEEPALIVE				8		//KeepAlive

//LogInServer���� �������� Ư���� �����´�.
#define NGSC_HEADERSEND				0x30		//ó�� Client���ӽ� ������ Header�� �����ش�.
#define NGSC_ERROR					0x31		//Error
#define NGSC_LOGIN					0x32		//LogInServer���� ������.
#define ERROR_NOT					0x33			
#define ERROR_PK					0x34		//packeterror
#define ERROR_NOTVER				0x35		//������ �ٸ���.
#define ERROR_ACCESS_DBFAIL			0x36		//��� �������� ����.
#define ERROR_MULTILOGIN			0x37		//�ߺ��α���.
#define NGSC_KEEPALIVE				0x38		//KeepAlive�� �������� ���� ������.
//////////////////////////////////////////////////////////////////////////
//LOGIN SERVER�� ����
//////////////////////////////////////////////////////////////////////////





//ErrorCode
// Server Error Code
#define ERROR_LIMIT_USER			0x83		//ä�δ� ������ �ʰ� �ߴ�.
#define ERROR_NOCREATEROOM			0x84		//���� �����Ҽ� ����.( ���̻���� ����� ����)
#define ERROR_NOROOM				0x85		//RoomIn�����ÿ� ���¹��̴�.
#define ERROR_ROOMFULL				0x86		//RoomIn�����ÿ� �濡 �ο��� �����ִ�.
#define	ERROR_NOTPWD				0x87		//��й��϶� Password�� �����ʴ�.
#define ERROR_NOQUICK				0x88		//�������� ���� ����. Client�� �ٽ� CreateRoom�� ������.
#define ERROR_NOTOPTION				0x91		//�Ӹ�, �ʴ�, �޼��� �ź�.
#define ERROR_NOCARD				0x92		//������ ī�带 �ٲٷ� �ߴµ� �������� ���� ī���̴�.
#define ERROR_NOCARDPACK			0x93		//������ �ٲٱ� ���ϴ� ī�忴�µ� ī���ѿ� ���� ī���̴�.


#define SC_PKTLEN_POS               2



#define SYSTEM_MONEY_RATE			5			//�ý��� �Ӵ� 5%
#define JACKPOT_MONEY_RATE			10			//�ý��� �ӴϿ��� 10% ��ü �ݾ׿��� 0.5%

	#define GAMEROOM_MONEY1			50
	#define GAMEROOM_MIN1			500
	#define GAMEROOM_MAX1			0

	#define GAMEROOM_MONEY2			1000
	#define GAMEROOM_MIN2			300000
	#define GAMEROOM_MAX2			0

	#define GAMEROOM_MONEY3			100
	#define GAMEROOM_MIN3			500
	#define GAMEROOM_MAX3			0

	#define GAMEROOM_MONEY4			2000
	#define GAMEROOM_MIN4			500000
	#define GAMEROOM_MAX4			0

	#define GAMEROOM_MONEY5			200
	#define GAMEROOM_MIN5			500
	#define GAMEROOM_MAX5			0			
/*
	#define GAMEROOM_MONEY6			5000
	#define GAMEROOM_MIN6			500000
	#define GAMEROOM_MAX6			0
*/
	#define GAMEROOM_MONEY7			500
	#define GAMEROOM_MIN7			200000
	#define GAMEROOM_MAX7			0
/*
	#define GAMEROOM_MONEY8			5000
	#define GAMEROOM_MIN8			500000
	#define GAMEROOM_MAX8			0

*/
	//���� ����
	#define JACKPOT_CNT				8
	
	//������ 
	#define ROOM_DIVI_CNT			8

	//���� ���� ����.
	#define	JACKPOT_FOUR_CARD  	    0.2
	#define	JACKPOT_STIFUL			2
	#define JACKPOT_ROTIFUL			4





#pragma pack( pop, 1)



#endif
