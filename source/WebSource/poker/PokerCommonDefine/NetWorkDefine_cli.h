#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f �����ϸ� �Ѿ����~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef UINT	BigInt;		//21�����....


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
	char	cMode;				//0 : GameMode,   1 : PlayMode
	char	cCmd;
	char	cErrorCode;
	//char	*szPayload;
	//char	szTail[2];			//'\n'\n'
} sCSHeaderPkt;


#define SC_HEADER_SIZE			sizeof(sSCHeaderPkt)   // NGHeader(2) + packetsize(2) + cmd(1) + errorcode(1)
#define CS_HEADER_SIZE			sizeof(sCSHeaderPkt)   // NGHeader(2) + cMode(1) + cmd(1) + errorcode(1)

#define SC_PKTLEN_POS			2


#define WAITMODE		0
#define GAMEMODE		1



//Server -> Client
#define NGSC_HEADERSEND				0x00		//ó�� Client���ӽ� ������ Header�� �����ش�.
#define NGSC_WAITINPUT				0x01		//���Ƿ� ���� ���� ������ �޾ƿ´�.
#define NGSC_CREATEROOM				0x02		//���� �����.
#define NGSC_ROOMIN					0x03		//�濡 ����.
#define NGSC_ROOMOUT				0x04		//���� ������.
#define NGSC_USERINFOCHANGE			0x05		//��������Ʈ�� ��ȭ�� ���涧.
#define NGSC_WAITINFOCHANGE			0x06		//���ǿ� ��ȭ�� ���涧( �븮��Ʈ + ��������Ʈ )
#define NGSC_WAITCHAT				0x07		//���� ä��.
#define NGSC_VIEWPAGE				0x08		//�븮��Ʈ�� ����.
#define NGSC_ERROR					0x09		//Error
#define NGSC_ROOMOUT_OTHERSEND		0x0A		//�濡�� ������ ��������..�ٸ�����鿡�� ������ ����.
#define NGSC_ROOMIN_OTHERSEND		0x0B		//�濡�� ������ ���ö�..�ٸ�����鿡�� ������ ����.
#define NGSC_QUICKJOIN				0x0C		
#define NGSC_KEEPALIVE				0x0D		//KeepAlive�� �������� ���� ������.
#define NGSC_TURNMSG				0x0E		//Client���� �� packet�� �״�� �ٸ� Client�� �����ش�.
#define NGSC_GAMEEND				0x0F		//Client�� ������ Update�ϰ� ����.
#define NGSC_VIEWWAIT				0x10		//Client�� ���ӹ�ȿ��� ������ ����� ����.
#define NGSC_USERINVITE				0x11		//Client�� ���ӹ濡�� ���� ������ �ʴ��Ѵ�.
#define NGSC_LOGIN					0x12		//LogInServer���� ������.
#define NGSC_GAMINGEXITUSER			0x13		//�����߿� ������ ��������..
#define NGCSC_BROADSERVERINFO		0x14		//�������� ī���͸� �ǽð� �������ش�.
#define NGSC_GETSERVERLIST			0x15		//Client�� ī���� ������ ������ ���� ��û.






//Client -> Server
//WaitMode
#define NGCS_WAITINPUT				0x00		//���Ƿ� ���� ���� ������ �޾ƿ´�.
#define NGCS_CREATEROOM				0x01		//���� �����.
#define NGCS_ROOMIN					0x02		//�濡 ����.
#define NGCS_ROOMOUT				0x03		//���� ������.
#define NGCS_QUICKJOIN				0x04		//������.
#define NGCS_WAITCHAT				0x05		//���� ä��.
#define NGCS_VIEWPAGE				0x06		//�븮��Ʈ�� ����.
#define NGCS_EXITGAME				0x07		//���ӿ��� ������ ������.
#define NGCS_KEEPALIVE				0x08		//KeepAlive
//#define NGCS_CSNUMPLUS				0x09		//�̰ɹް� ������ ���̰� ������ �����Ѵ�.


//GameMode
#define NGCS_TURNMSG				0x00		//Client���� �� packet�� �״�� �ٸ� Client�� �����ش�.
#define NGCS_GAMEEND				0x01		//Client���� ������ ��������..
#define NGCS_GETAVATAR				0x02		//Client�� ����� �ƹ�Ÿ�� ��������...
#define NGCS_VIEWWAIT				0x03		//Client�� ���ӹ�ȿ��� ������ ����� ����.
#define NGCS_USERINVITE				0x04		//Client�� ���ӹ濡�� ���� ������ �ʴ��Ѵ�.
#define NGCS_GAMINGEXITUSER			0x05		//�����߿� ������ ��������..


//LogInServerMode
#define NGCS_LOGIN					0x00		//Client�� LogInServer�� �����Ҷ� ���� Command


//CSServerMode
#define NGCS_GETSERVERLIST			0x00		//Client�� ī���� ������ ������ ���� ��û.
#define NGSS_GSCSNUMMINUS			0x01		//���Ӽ������� Ŭ���̾�Ʈ�� ������ ������ ī���� �������� �����ο��� ���ҽ�Ų��.
#define NGSS_GSCSSERVERSET			0x02		//���Ӽ����� ī���� ������ �����ϸ鼭 ������.
#define NGSS_GSCSNUMPLUS			0x03		//������ ���Ӽ����� �����Ҷ� ī���ͼ����� �̰ɳ����� ����ī��Ʈ�� �����Ѵ�.
#define NGSS_GSCSTOTCLIENTNUM		0x04		//������ ��Ż Ŭ���̾�Ʈ���� ������.

//ErrorCode
// Server Error Code
#define ERROR_NOT					0x50			
#define ERROR_ACCESS_DBFAIL			0x51		//��� �������� ����.
#define ERROR_PK					0x52		//packeterror
#define ERROR_LIMIT_USER			0x53		//ä�δ� ������ �ʰ� �ߴ�.
#define ERROR_NOCREATEROOM			0x54		//���� �����Ҽ� ����.( ���̻���� ����� ����)
#define ERROR_NOROOM				0x55		//RoomIn�����ÿ� ���¹��̴�.
#define ERROR_ROOMFULL				0x56		//RoomIn�����ÿ� �濡 �ο��� �����ִ�.
#define	ERROR_NOTPWD				0x57		//��й��϶� Password�� �����ʴ�.
#define ERROR_NOQUICK				0x58		//�������� ���� ����. Client�� �ٽ� CreateRoom�� ������.
#define ERROR_NOTVER				0x59		//������ �ٸ���.
#define ERROR_MULTILOGIN			0x60		//�ߺ��α���.


//****************************************************************************************
//������ �� ����.
//****************************************************************************************
#define MAX_ROOM_NAME				20
#define MAX_ID_LENGTH				24
#define MAX_NAME_LENGTH				20
#define MAX_NICK_LENGTH				20
#define MAX_ROOM_PASSWORD			10	
#define MAX_ROOM_IN					3
//#define MAX_CHANNELPER				120									//��ä���� Client�Ѽ�.
#define MAX_CHANNELPER				120									//��ä���� Client�Ѽ�.

#define MAX_CHANNEL					10									//ä�μ�
#define MAX_CHANNEL_NAME			20									//ä�θ�
#define PAGE_ROOMCNT				5									//���������� ���.
#define MAX_ROOMCNT					MAX_CHANNELPER / FIX_GAME_USERCNT	//�� ä�δ� 120 / 3 = 40
#define MAX_PAGE_CNT				MAX_ROOMCNT / PAGE_ROOMCNT			// 40 / 5 = 8  
#define FIX_GAME_USERCNT			3									//gostop�� 2,3���� ģ��.
#define MAX_CHATMSG					255									//ä�ø޼����� �ִ����.
#define MAX_IMAGE_LEN				7									//�̹��� Į���� �ѱ���. 7byte + NULL
#define MAX_IMAGE_CNT				10									//�ѻ���� ������ �ִ� �̹��� �Ѽ�.
#define MAX_GET_AVATARCNT			21									//�ѻ���� ������ �ִ� �ƹ�Ÿ �Ѽ�( 10�� * 2byte ). + NULL
#define	MAX_IP_LEN					30
#define MAX_SERVERNAME				20


#define MASHINE_CNT				10
#define CHANNEL_PER_MASHINE		10
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
	ush_int nRoomMoney;					//�ǵ�.
} sRoomInfo;

typedef struct tagRoomList	//������ ������ �ִ� ������
{
	sRoomInfo l_sRoomInfo;
	char szRoom_Pass[MAX_ROOM_PASSWORD];
} sRoomList;

typedef struct tagUserInfo	//Ŭ���̾�Ʈ���� �����ִ� ���� ����
{
	char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
	BigInt	nbUserMoney;				//������ ��.
	ush_int nClass;						//���
	BigInt	nbMaxMoney;					//���ݲ� �ְ�� ���� ����.
	float	fWinRate;					//�·�.
	//ush_int nTot_Num;					//����.
	ush_int	nWin_Num;					//�̱�Ƚ��.
	ush_int	nLose_Num;					//����Ƚ��.
	ush_int	nDraw_Num;					//���º�Ƚ��.
	ush_int	nCut_Time;					//����Ƚ��.
	ush_int nChannel;					//������ ä��
	int nUserNo;						//������ �����ѹ�
	char cSex;							//'0' ���� '1'����
	ush_int nStripNumber;				//����Ƚ��.
	char szPreAvatar[MAX_IMAGE_LEN];	//��ǥ�ƹ�Ÿ.
} sUserInfo;

typedef struct tagUserList	//Server�� ������ �ִ� ��������
{
	char szUser_Id[MAX_ID_LENGTH];		//�������̵�
	char szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];		//������ �̹���.   2byte(�̹�����ȣ) + 2byte(��������) + 1byte(total�ܰ�) + 2byte(�ܰ躰����)
	sUserInfo l_sUserInfo;
} sUserList;




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

//���������ÿ� �ѷ���.
typedef struct tagRoomOutOtherSend
{
	int			nUserNo;
	ush_int		nNewHostNo; //�����Ͱ��ٲ��� ������ 0�̴�. 0�� �ƴϸ� �������� ��ȣ�� ����.
	ush_int		nNewBbangJjnag; //���尡�ٲ��� ������ 0�̴�. 0�� �ƴϸ� ������ ��ȣ�� ����.
} sRoomOutOtherSend;

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


//�濡 ������ ���������� ���� �ٸ���� ������ ������.
//�濡 ���� ���� ������ ������ �ٸ� �������� �������� ����Ѵ�.
typedef struct tagRoomInUserSend
{
	//int			nUserNo;
	bool		bRoomMaster;
	bool		bBbangJjang;
	sUserInfo	l_sUserInfo;
	char		szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];		//������ �̹���.   2byte(�̹�����ȣ) + 2byte(��������) + 1byte(total�ܰ�) + 2byte(�ܰ躰����)	
	char		szPrevImage[MAX_IMAGE_LEN];
} sRoomInUserSend;


typedef struct tagLogIn
{

	char szCSServerIp[MAX_IP_LEN];
	ush_int nPort;
	sUserList l_sUserList;

} sLogIn;


typedef struct tagGamingExitUser {
	
	ush_int	nNewBbangJjnag; //���尡�ٲ��� ������ 0�̴�. 0�� �ƴϸ� ������ ��ȣ�� ����.	
	int nUserNo;			//���� ������ �����ѹ�
	ush_int nGameUserNo;		//�濡���� �������� ��ȣ.
}sGamingExitUser;




typedef struct tagServerChannelList
{
	int nChannelNo;
	char szChannelName[MAX_SERVERNAME];
	int nCurCnt;
	int nTotCnt;
} sServerChannelList;


typedef struct tagServrMashineList
{
	int nMashineNo;
	char szServerMashineName[MAX_SERVERNAME];
	char szServerMashineIp[MAX_IP_LEN];
	UINT nPort;
	sServerChannelList	l_sServerChannelList[CHANNEL_PER_MASHINE];
} sServrMashineList;





//////////////////////////C->S/////////////////////////////




typedef struct tagPKWaitInputCS
{
	sUserList l_sUserInfo;
} sPKWaitInputCS;

typedef struct tagCreateRoom
{
	bool bSecret;						//������ false ��й� true�ϰ�� 
	char szRoomName[MAX_ROOM_NAME];		//����
	char szRoomPass[MAX_ROOM_PASSWORD];	
	ush_int nTotCnt;					//�� ���ο� 
} sCreateRoom;	

typedef struct tagRoomIn
{
	ush_int nRoomNo;
	char szRoomPass[MAX_ROOM_PASSWORD];
} sRoomIn;

typedef struct tagGameEnd
{
	BigInt	nbUserMoney;				//������ ��.
	BigInt	nbMaxMoney;					//���ݲ� �ְ�� ���� ����.
	float	fWinRate;					//�·�.
	//ush_int nTot_Num;					//����.
	ush_int	nWin_Num;					//�̱�Ƚ��.
	ush_int	nLose_Num;					//����Ƚ��.
	ush_int	nDraw_Num;					//���º�Ƚ��.
	bool	bWinner;					//���ǿ��� �̰����.
	ush_int nStripNumber;				//����Ƚ��.
	//ush_int	nCut_Time;					//����Ƚ��.
	char szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];		//������ �̹���.   2byte(�̹�����ȣ) + 2byte(��������) + 1byte(total�ܰ�) + 2byte(�ܰ躰����)
	char szPrevImage[MAX_IMAGE_LEN];
} sGameEnd;


//hy_v004_04.01.29
typedef struct tagRoomStateInfo
{
	sRoomInfo l_sRoomInfo;
	bool bUser[MAX_ROOM_IN];
	sUserInfo l_sUserInfo[MAX_ROOM_IN];
} sRoomStateInfo;

//hy_v006_04.02.3
typedef struct tagPuzzle
{
	int x, y;
	int nAlpha;
	bool bShow;
	
	//LPDIRECTDRAWSURFACE7 lpDDS;

}PUZZLE;
//***************

typedef struct tagEndGameOther
{
	char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
	sGameEnd l_sGameEnd;
} sEndGameOther;

#pragma pack( pop, 1)


#endif
