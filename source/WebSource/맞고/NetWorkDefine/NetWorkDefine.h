#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f �����ϸ� �Ѿ����~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef __int64	BigInt;

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


//C -> S HEADER  
typedef struct tagSCHeaderPkt
{ 
	char	szHeader[2];
	ush_int nTPacketSize;
	char	cMode;				//0 : GameMode,   1 : PlayMode
	char	cCmd;
	char	cErrorCode;
} sCSHeaderPkt;






#define SC_HEADER_SIZE			sizeof(sSCHeaderPkt)   // NGHeader(2) + packetsize(2) + cmd(1) + errorcode(1)
#define CS_HEADER_SIZE			sizeof(sCSHeaderPkt)   // 7

#define SC_PKTLEN_POS			2


#define WAITSENDMODE		0
#define GAMESENDMODE		1
#define ALLSENDMODE			2



//Server -> Client
//#define NGSC_HEADERSEND				0x00		//ó�� Client���ӽ� ������ Header�� �����ش�.
#define NGSC_WAITINPUT				0x01		//���Ƿ� ���� ���� ������ �޾ƿ´�.
#define NGSC_CREATEROOM				0x02		//���� �����.
#define NGSC_ROOMIN					0x03		//�濡 ����.
#define NGSC_ROOMOUT				0x04		//���� ������.
#define NGSC_USERINFOCHANGE			0x05		//��������Ʈ�� ��ȭ�� ���涧.
#define NGSC_WAITINFOCHANGE			0x06		//���ǿ� ��ȭ�� ���涧( �븮��Ʈ + ��������Ʈ )
#define NGSC_WAITCHAT				0x07		//���� ä��.
#define NGSC_VIEWPAGE				0x08		//�븮��Ʈ�� ����.
#define NGSC_ROOMOUT_OTHERSEND		0x0A		//�濡�� ������ ��������..�ٸ�����鿡�� ������ ����.
#define NGSC_ROOMIN_OTHERSEND		0x0B		//�濡�� ������ ���ö�..�ٸ�����鿡�� ������ ����.
#define NGSC_QUICKJOIN				0x0C		
#define NGSC_TURNMSG				0x0E		//Client���� �� packet�� �״�� �ٸ� Client�� �����ش�.
#define NGSC_GAMEEND				0x0F		//Client�� ������ Update�ϰ� ����.
#define NGSC_VIEWWAIT				0x10		//Client�� ���ӹ�ȿ��� ������ ����� ����.
#define NGSC_USERINVITE				0x11		//Client�� ���ӹ濡�� ���� ������ �ʴ��Ѵ�.
#define NGSC_GAMINGEXITUSER			0x13		//�����߿� ������ ��������..
#define NGCSC_BROADSERVERINFO		0x14		//�������� ī���͸� �ǽð� �������ش�.
#define NGSC_GETSERVERLIST			0x15		//Client�� ī���� ������ ������ ���� ��û.
#define NGSC_MEMO					0x16		
#define NGSC_STARTGAME				0x17
#define NGSC_GETAVAURL				0x18
#define NGSC_ALLCMT					0x19
#define NGSC_IPCHECK				0x1A		//�����濡�� �Ѿ����Ǹ� ���ƾ��Ѵ�.
#define NGSC_GAMESUNSET				0x1B		//�� ����.
#define	NGSC_CHOICECARDNO			0x1C		//�� ������ ī�� ���̽� �Ϸ�.
#define NGSC_SUNDECIDE				0x1D		//�� ����.
#define NGSC_RECHOICE				0x1E		//���ٽ� ����.
#define NGSC_GETUSERINFO			0x1F		//
#define NGSC_ROOMINFO				0x20		//��ȿ� ���������.

#define NGSC_GAMEREALSTARTBTN		0x21
#define NGSC_SETVOICE				0x22		//���Ҹ��� �ٲ۴�.


//#define NGSC_GAMECOMSTARTBTN		0x22

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






//Client -> Server
//WaitMode
#define NGCS_WAITINPUT				0		//���Ƿ� ���� ���� ������ �޾ƿ´�.
#define NGCS_CREATEROOM				1		//���� �����.
#define NGCS_ROOMIN					2		//�濡 ����.
#define NGCS_ROOMOUT				3		//���� ������.
#define NGCS_QUICKJOIN				4		//������.
#define NGCS_WAITCHAT				5		//���� ä��.
#define NGCS_VIEWPAGE				6		//�븮��Ʈ�� ����.
#define NGCS_EXITGAME				7		//���ӿ��� ������ ������.
//#define NGSC_KEEPALIVE			8		//���� �ִ�.
#define NGCS_GETAVAURL				9		//
#define NGCS_GETUSERINFO			10		
#define NGCS_ROOMINFO				11		//��ȿ� �����.

#define NGCS_ADMINJUSTVIEW  		12		//����

//GameMode
#define NGCS_TURNMSG				0		//Client���� �� packet�� �״�� �ٸ� Client�� �����ش�.
#define NGCS_GAMEEND				1		//Client���� ������ ��������..
#define NGCS_VIEWWAIT				3		//Client�� ���ӹ�ȿ��� ������ ����� ����.
#define NGCS_USERINVITE				4		//Client�� ���ӹ濡�� ���� ������ �ʴ��Ѵ�.
#define NGCS_GAMINGEXITUSER			5		//�����߿� ������ ��������..
#define NGCS_STARTREADY				6		//�����غ�.
#define NGCS_GAMESUNSET				7
#define NGCS_SUNCHOICE				8		
#define NGCS_AUTOCHOICE				9


//AllMODE
#define NGCS_MEMOSEND				0		//���� ������.
#define NGCS_SETOPTION				1		//�ɼ�����.
#define NGCS_ALLCMT					2		//��ο��� ������ ������.



/*
//CSServerMode
#define NGCS_GETSERVERLIST			0x00		//Client�� ī���� ������ ������ ���� ��û.
#define NGSS_GSCSNUMMINUS			0x01		//���Ӽ������� Ŭ���̾�Ʈ�� ������ ������ ī���� �������� �����ο��� ���ҽ�Ų��.
#define NGSS_GSCSSERVERSET			0x02		//���Ӽ����� ī���� ������ �����ϸ鼭 ������.
#define NGSS_GSCSNUMPLUS			0x03		//������ ���Ӽ����� �����Ҷ� ī���ͼ����� �̰ɳ����� ����ī��Ʈ�� �����Ѵ�.
#define NGSS_GSCSTOTCLIENTNUM		0x04		//������ ��Ż Ŭ���̾�Ʈ���� ������.
*/
//ErrorCode
// Server Error Code
#define ERROR_LIMIT_USER			0x83		//ä�δ� ������ �ʰ� �ߴ�.
#define ERROR_NOCREATEROOM			0x84		//���� �����Ҽ� ����.( ���̻���� ����� ����)
#define ERROR_NOROOM				0x85		//RoomIn�����ÿ� ���¹��̴�.
#define ERROR_ROOMFULL				0x86		//RoomIn�����ÿ� �濡 �ο��� �����ִ�.
#define	ERROR_NOTPWD				0x87		//��й��϶� Password�� �����ʴ�.
#define ERROR_NOQUICK				0x88		//�������� ���� ����. Client�� �ٽ� CreateRoom�� ������.
#define ERROR_NOTOPTION				0x91		//�Ӹ�, �ʴ�, �޼��� �ź�.

//****************************************************************************************
//������ �� ����.
//****************************************************************************************
#define MAX_ROOM_NAME				20

#define MAX_ID_LENGTH				50


#define MAX_NAME_LENGTH				20
#define MAX_NICK_LENGTH				21
#define MAX_ROOM_PASSWORD			10	
#define MAX_GAMESTRMONEYLEN			25					
#define MAX_ROOM_IN					2

#define MAX_CHANNELPER				100									//��ä���� Client�Ѽ�.
#define MAX_CHANNEL					10									//ä�μ�
#define MAX_CHANNEL_NAME			20									//ä�θ�

#define PAGE_ROOMCNT				10									//���������� ���.


//#define MAX_ROOMCNT				MAX_CHANNELPER / MAX_ROOM_IN		//�� ä�δ� 120 / 3 = 40  �°� 120 / 2 = 60
#define MAX_ROOMCNT					100
#define MAX_PAGE_CNT				MAX_ROOMCNT / PAGE_ROOMCNT			// 100 / 5 = 20  �°� 100 / 10 = 10
#define MAX_CHATMSG					255									//ä�ø޼����� �ִ����.
#define MAX_IMAGE_LEN				7									//�̹��� Į���� �ѱ���. 7byte + NULL
#define MAX_IMAGE_CNT				10									//�ѻ���� ������ �ִ� �̹��� �Ѽ�.
#define MAX_GET_AVATARCNT			21									//�ѻ���� ������ �ִ� �ƹ�Ÿ �Ѽ�( 10�� * 2byte ). + NULL
#define	MAX_IP_LEN					30
#define MAX_SERVERNAME				20
#define MAX_GAMEVIEWWAIT		30									//���ӹ濡�� �ʴ��� ��� ����.


#define MASHINE_CNT					10
#define CHANNEL_PER_MASHINE			10
#define MAX_AVAURLLEN				20									//�ƹ�Ÿ Url����.

//#define MAX_MINUS_CHANNELPER		20									//100������� �޴´�.
#define MAX_SUNCARD_CNT				10									//ó�� ���̱� ī���
#define NOTICE_BYTE					100
//****************************************************************************************
//�Ϲݱ���ü.
//****************************************************************************************
typedef struct tagRoomInfo	//Ŭ���̾�Ʈ���� �����ִ� ������
{
	short int nRoomNo;					//���ȣ
	char szRoomName[MAX_ROOM_NAME];		//������
	ush_int nCurCnt;					//�����ο�
	ush_int nTotCnt;					//�����ο�
	bool bSecret;						//��й� ���� ( true : ��й�, false : ������ )
	char cGameDivi;						//�������̳�. '0'�����, '1'������
	BigInt biRoomMoney;					//�ǵ�.
} sRoomInfo;

typedef struct tagRoomList	//������ ������ �ִ� ������
{
	sRoomInfo l_sRoomInfo;
	char szRoom_Pass[MAX_ROOM_PASSWORD];
} sRoomList;

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
	char szGameGubun[5];				// '0' : Neo, '1' : õ����.
} sUserInfo;

typedef struct tagNotice
{
	char	m_Notice1[NOTICE_BYTE];
	char	m_Notice2[NOTICE_BYTE];
} sNotice;


typedef struct tagWaitUserInfo	//Ŭ���̾�Ʈ���� �����ִ� ���� ����
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
	char szGameGubun[5];				// '0' : Neo, '1' : õ����.
} sWaitUserInfo;  //������ ������ �ƹ�Ÿ�� �����Ѵ�. �ʿ��Ҷ� ���� �޴´�.



typedef struct tagUserList	//Server�� ������ �ִ� ��������
{
	char szUser_Id[MAX_ID_LENGTH];		//�������̵�
	sUserInfo l_sUserInfo;
} sUserList;

typedef struct tagUserInvite	//Server�� ������ �ִ� ��������
{
	ush_int nRoomNo;
	BigInt biRoomMoney;
	char szRoomPass[MAX_ROOM_PASSWORD];	
	ush_int nInviteLen;
} sUserInvite;


/*
typedef struct tagLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;
	sUserList l_sUserList;

} sLogIn;
*/


//****************************************************************************************
//Pakcet Structure
//****************************************************************************************

//////////////////////////S->CS////////////////////////////////////////////////
typedef struct tagNumMinus
{
	ush_int nGSNo;
	ush_int nChannel;
} sNumPlusMinus;


/////////////////////////CS->C/////////////////////////////////////////////////
typedef struct tagBroadServerInfo
{
	ush_int nGSNo;
	ush_int nChannel;
	ush_int nCurCnt;
} sBroadServerInfo;





//////////////////////////S->C/////////////////////////////
typedef struct tagCurMashineList
{
	ush_int nMashineNo;
	char szServerIp[MAX_IP_LEN];
	ush_int nServerPort;
	ush_int nChnnelCutCnt[CHANNEL_PER_MASHINE];
} sCurMashineList;


typedef struct tagPKFirst
{
	char szHeader[2];
	int nUserNo;
	char szTail[2];
} sPKFirst;

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


//�濡�� ���ӽ����̳� ������ ��������.
typedef struct tagRoomGameWait
{
	ush_int		nRoomNo;
	char		cGameWait;			// 'G' Game ����   'W' ��� ����.
} sRoomGameWait;


typedef struct tagRoomInUserSend
{
	bool		bRoomMaster;
	bool		bBbangJjang;
	sUserInfo	l_sUserInfo;
	char		szGameGubun[5];				// '0' : Neo, '1' : õ����.
	int			nVoice;
} sRoomInUserSend;

//���� ���ӹ濡 ������.
typedef struct tagRoomInAdd
{
	ush_int nRoomInUserNo;
} sRoomInAdd;

typedef struct tagLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;
	sUserList l_sUserList;

} sLogIn;


typedef struct tagGamingExitUser {
	
	int	nNewBbangJjnag; //���尡�ٲ��� ������ 0�̴�. 0�� �ƴϸ� ������ ��ȣ�� ����.	
	int nUserNo;			//���� ������ �����ѹ�
	int nGameUserNo;	//�濡���� �������� ��ȣ.
}sGamingExitUser;


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








//////////////////////////C->S/////////////////////////////
typedef struct tagGameWaitInfo	//�����ǿ��� ���� ������ ���°�..
{
	char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
	BigInt	biUserMoney;				//������ ��.
	int nUserNo;						//������ �����ѹ�
	char cSex;							//'0' ���� '1'����
} sGameWaitInfo;

	

//�濡 ������ ���������� ���� �ٸ���� ������ ������.
//�濡 ���� ���� ������ ������ �ٸ� �������� �������� ����Ѵ�.
typedef struct tagRoomInOtherSend
{
	ush_int nNextNo;
	ush_int nRoomCurCnt;
	int	nBbangJjangUniqNo;
	sUserInfo l_sUserInfo;
} sRoomInOtherSend;

typedef struct tagOption {
	bool bInvite;	//true : �ʴ밡��
	bool bEarTalk;  //true : �Ӹ�����
	bool bMemoRecv; //true : ��������.
	int nVoice;
} sOption;



typedef struct tagPKWaitInputCS
{
	sUserList l_sUserInfo;
	sOption l_sOption;
} sPKWaitInputCS;

typedef struct tagCreateRoom
{
	bool bSecret;						//������ false ��й� true�ϰ�� 
	char szRoomName[MAX_ROOM_NAME];		//����
	char szRoomPass[MAX_ROOM_PASSWORD];	
	ush_int nTotCnt;					//�� ���ο� 
	BigInt biRoomMoney;
} sCreateRoom;	


typedef struct tagRoomIn
{
	ush_int nRoomNo;
	char szRoomPass[MAX_ROOM_PASSWORD];
} sRoomIn;


typedef struct tagGameEndHistory
{
	BYTE m_nEatCard[4][30];
	int  m_nEvalution;								//������ ���� ����.
	int m_nShakeCnt;
	int m_nBbukCnt;									//�� ī��Ʈ
	int m_nGoCnt;
	bool m_bGodori;
	bool m_bHongdan;
	bool m_bChongdan;
	bool m_bGusa;	
	bool m_bMmong;
	bool m_bShake;
	bool m_bMission;
	bool m_bPbak; // �ǹ�
	bool m_bKwangbak; // ����
	bool m_bDokbak; // ����	
	int m_nMulti;									//������ ���߿� *2 �� * 3�� ǥ��.
	int m_nMissionMultiply;
} sGameEndHistory;


typedef struct tagGameEnd
{
	BigInt	biObtainMoney;									//������ �̹��ǿ� ����. ������(+)
	BigInt  biLoseMoney;									//������ �̹��ǿ� ������. ������(+)
	int 	nWinner;										//0 : ��, 1 : ��, 2 : ���
	char	szOtherNick_Name[MAX_ROOM_IN - 1][MAX_NICK_LENGTH];	//������ ģ ��� ���� �г���.
	BigInt	biSystemMoney;				//�ý��� �Ӵ�.	
	ush_int nPrizeNo;
	ush_int nSucceedCode;				// 0 �̸� ����, 1�̸� ����
	sGameEndHistory l_sGameEndHistory;
} sGameEnd;



typedef struct tagRoomStateInfo
{
	sRoomInfo l_sRoomInfo;
	bool bUser[MAX_ROOM_IN];
	sUserInfo l_sUserInfo[MAX_ROOM_IN+1];
} sRoomStateInfo;


typedef struct tagUserChange
{
	BYTE bUerCardNo;
	BYTE bChangeCardNo;
} sUserChange;


typedef struct tagPuzzle
{
	int x, y;
	int nAlpha;
	bool bShow;

}PUZZLE;


typedef struct tagEndGameOther
{
	char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
	sGameEnd l_sGameEnd;
} sEndGameOther;


typedef struct tagPaintAvatar
{
	BOOL bPrint[3];
} sPaintAvatar;


typedef struct tagGetUserInfo
{

	char szUser_Id[MAX_ID_LENGTH];		//�������̵�
	ush_int nUserChannel;

} sGetUserInfo;

typedef struct tagIsLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;

} sIsLogIn;


//��� ������Ʈ.
typedef struct tagUserDBUpdateInfo
{
	BigInt biObtainMoney;										//������ ��ų� ���� �Ӵ�
	BigInt biLoseMoney;											//������ ��ų� ���� �Ӵ�
	int	   nWinLose;											//0 : ��, 1 : ��, 2 : ���
	BigInt biRoomMoney;											//�� �⺻�Ӵ�
	BigInt biSystemMoney;										//������ ������ �ý��� �Ӵ�. 
	char szOtherGameUser[MAX_ROOM_IN - 1][MAX_NICK_LENGTH];		//���� ������ ������
} sUserDBUpdateInfo;


typedef struct tagRealGameStart
{
	int nBbangJjangUniqNo;
	int nRoomMasterUniqNo;
} sRealGameStart;

typedef struct tagGameUserInfo {
	sUserInfo l_sUserInfo;
	BigInt biObtainMoney;
	BigInt biLoseMoney;
	sGameEndHistory l_sGameEndHistory;
} sGameUserInfo;

typedef struct tagRoomOutOtherSend
{
	int			nUserNo;
	ush_int		nRoomCurCnt;
	int			nNewHostNo; //�����Ͱ��ٲ��� ������ 0�̴�. 0�� �ƴϸ� �������� ��ȣ�� ����.
	int			nNewBbangJjnag; //���尡�ٲ��� ������ 0�̴�. 0�� �ƴϸ� ������ ��ȣ�� ����.
} sRoomOutOtherSend;




#define GAMEROOM_MONEY1				50
#define GAMEROOM_MIN1				50000
#define GAMEROOM_MAX1				0			

#define GAMEROOM_MONEY2				500
#define GAMEROOM_MIN2				300000
#define GAMEROOM_MAX2				0

#define GAMEROOM_MONEY3				100
#define GAMEROOM_MIN3				100000
#define GAMEROOM_MAX3				0

#define GAMEROOM_MONEY4				1000
#define GAMEROOM_MIN4				500000
#define GAMEROOM_MAX4				0

#define GAMEROOM_MONEY5				200
#define GAMEROOM_MIN5				200000
#define GAMEROOM_MAX5				0

#define GAMEROOM_MONEY6				2000
#define GAMEROOM_MIN6				500000
#define GAMEROOM_MAX6				0

#define GAMEROOM_MONEY7				300
#define GAMEROOM_MIN7				300000
#define GAMEROOM_MAX7				0

#define GAMEROOM_MONEY8				5000
#define GAMEROOM_MIN8				500000
#define GAMEROOM_MAX8				0

//������ 
#define ROOM_DIVI_CNT				8



#pragma pack( pop, 1)


#endif
