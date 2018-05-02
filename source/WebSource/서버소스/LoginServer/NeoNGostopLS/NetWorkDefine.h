#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f 웬만하면 넘어가지마~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef UINT	BigInt;		//21억까지....

//****************************************************************************************
//Protocol 관련.
//****************************************************************************************

#define MAX_NICK_LENGTH				21
#define MAX_AVAURLLEN				20									//아바타 Url길이.
#define MAX_ID_LENGTH				50
#define	MAX_IP_LEN					30


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
#define SC_PKTLEN_POS			2



//Client -> Server
//펑션포인터 때문에 어쩔수 없다.
#define NGCS_ISLOGIN				0		//Client가 LogInServer로 접속할때 쓰는 Command
#define NGCS_KEEPALIVE				8		//KeepAlive

//LogInServer와의 공통으로 특별히 빼놓는다.
#define NGSC_HEADERSEND				0x30		//처음 Client접속시 정해진 Header을 보내준다.
#define NGSC_ERROR					0x31		//Error
#define NGSC_LOGIN					0x32		//LogInServer에서 날린다.
#define ERROR_NOT					0x33			
#define ERROR_PK					0x34		//packeterror
#define ERROR_NOTVER				0x35		//버전이 다를떄.
#define ERROR_ACCESS_DBFAIL			0x36		//디비에 엑세스를 실패.
#define ERROR_MULTILOGIN			0x37		//중복로그인.
#define NGSC_KEEPALIVE				0x38		//KeepAlive를 서버에서 먼저 보낸다.





typedef struct tagUserInfo	//클라이언트에게 보내주는 유저 정보
{
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	BigInt	nbUserMoney;				//유저의 돈.
	ush_int nClass;						//계급
	BigInt	nbMaxMoney;					//지금껏 최고로 마니 번돈.
	float	fWinRate;					//승률.
	ush_int	nWin_Num;					//이긴횟수.
	ush_int	nLose_Num;					//패한횟수.
	ush_int	nDraw_Num;					//무승부횟수.
	ush_int	nCut_Time;					//끊긴횟수.
	ush_int nChannel;					//유저의 채널
	int nUserNo;						//유저의 고유넘버
	char cSex;							//'0' 여자 '1'남자
	char szPreAvatar[MAX_AVAURLLEN];	//대표아바타.
	char szGameGubun[5];				// '0' : Neo, '1' : 천리안.
} sUserInfo;


typedef struct tagUserList	//Server가 가지고 있는 유저정보
{
	char szUser_Id[MAX_ID_LENGTH];		//유저아이디
	sUserInfo l_sUserInfo;
} sUserList;


typedef struct tagPKFirst
{
	char szHeader[2];
	int nUserNo;
	char szTail[2];
} sPKFirst;


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


#pragma pack( pop, 1)


#endif
