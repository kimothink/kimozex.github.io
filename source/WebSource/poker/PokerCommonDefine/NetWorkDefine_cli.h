#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f 웬만하면 넘어가지마~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef UINT	BigInt;		//21억까지....


//****************************************************************************************
//Protocol 관련.
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
#define NGSC_HEADERSEND				0x00		//처음 Client접속시 정해진 Header을 보내준다.
#define NGSC_WAITINPUT				0x01		//대기실로 들어갈때 대기실 정보를 받아온다.
#define NGSC_CREATEROOM				0x02		//방을 만든다.
#define NGSC_ROOMIN					0x03		//방에 들어간다.
#define NGSC_ROOMOUT				0x04		//방을 나간다.
#define NGSC_USERINFOCHANGE			0x05		//유저리스트에 변화가 생길때.
#define NGSC_WAITINFOCHANGE			0x06		//대기실에 변화가 생길때( 룸리스트 + 유저리스트 )
#define NGSC_WAITCHAT				0x07		//대기실 채팅.
#define NGSC_VIEWPAGE				0x08		//룸리스트를 볼때.
#define NGSC_ERROR					0x09		//Error
#define NGSC_ROOMOUT_OTHERSEND		0x0A		//방에서 유저가 나갔을때..다른사람들에게 정보를 날림.
#define NGSC_ROOMIN_OTHERSEND		0x0B		//방에서 유저가 들어올때..다른사람들에게 정보를 날림.
#define NGSC_QUICKJOIN				0x0C		
#define NGSC_KEEPALIVE				0x0D		//KeepAlive를 서버에서 먼저 보낸다.
#define NGSC_TURNMSG				0x0E		//Client에서 온 packet을 그대로 다른 Client로 보내준다.
#define NGSC_GAMEEND				0x0F		//Client의 정보를 Update하고 난후.
#define NGSC_VIEWWAIT				0x10		//Client가 게임방안에서 대기실의 목록을 볼때.
#define NGSC_USERINVITE				0x11		//Client가 게임방에서 대기실 유저를 초대한다.
#define NGSC_LOGIN					0x12		//LogInServer에서 날린다.
#define NGSC_GAMINGEXITUSER			0x13		//게임중에 유저가 나갔을때..
#define NGCSC_BROADSERVERINFO		0x14		//서버들의 카운터를 실시간 전송해준다.
#define NGSC_GETSERVERLIST			0x15		//Client가 카운터 서버로 서버들 정보 요청.






//Client -> Server
//WaitMode
#define NGCS_WAITINPUT				0x00		//대기실로 들어갈때 대기실 정보를 받아온다.
#define NGCS_CREATEROOM				0x01		//방을 만든다.
#define NGCS_ROOMIN					0x02		//방에 들어간다.
#define NGCS_ROOMOUT				0x03		//방을 나간다.
#define NGCS_QUICKJOIN				0x04		//퀵조인.
#define NGCS_WAITCHAT				0x05		//대기실 채팅.
#define NGCS_VIEWPAGE				0x06		//룸리스트를 볼때.
#define NGCS_EXITGAME				0x07		//게임에서 완전히 나간다.
#define NGCS_KEEPALIVE				0x08		//KeepAlive
//#define NGCS_CSNUMPLUS				0x09		//이걸받고 소켓을 죽이고 서버를 접속한다.


//GameMode
#define NGCS_TURNMSG				0x00		//Client에서 온 packet을 그대로 다른 Client로 보내준다.
#define NGCS_GAMEEND				0x01		//Client에서 게임이 끝났을떄..
#define NGCS_GETAVATAR				0x02		//Client가 상대의 아바타를 뺏었을때...
#define NGCS_VIEWWAIT				0x03		//Client가 게임방안에서 대기실의 목록을 볼때.
#define NGCS_USERINVITE				0x04		//Client가 게임방에서 대기실 유저를 초대한다.
#define NGCS_GAMINGEXITUSER			0x05		//게임중에 유저가 나갔을때..


//LogInServerMode
#define NGCS_LOGIN					0x00		//Client가 LogInServer로 접속할때 쓰는 Command


//CSServerMode
#define NGCS_GETSERVERLIST			0x00		//Client가 카운터 서버로 서버들 정보 요청.
#define NGSS_GSCSNUMMINUS			0x01		//게임서버에서 클라이언트가 접속을 끊으면 카운터 서버에서 현재인원을 감소시킨다.
#define NGSS_GSCSSERVERSET			0x02		//게임서버가 카운터 서버로 접속하면서 날린다.
#define NGSS_GSCSNUMPLUS			0x03		//유저가 게임서버로 접속할때 카운터서버로 이걸날려서 접속카운트를 증가한다.
#define NGSS_GSCSTOTCLIENTNUM		0x04		//서버의 토탈 클라이언트수를 보낸다.

//ErrorCode
// Server Error Code
#define ERROR_NOT					0x50			
#define ERROR_ACCESS_DBFAIL			0x51		//디비에 엑세스를 실패.
#define ERROR_PK					0x52		//packeterror
#define ERROR_LIMIT_USER			0x53		//채널당 유저를 초과 했다.
#define ERROR_NOCREATEROOM			0x54		//방을 생성할수 없다.( 더이상방을 만들수 없다)
#define ERROR_NOROOM				0x55		//RoomIn했을시에 없는방이다.
#define ERROR_ROOMFULL				0x56		//RoomIn했을시에 방에 인원이 꽉차있다.
#define	ERROR_NOTPWD				0x57		//비밀방일때 Password가 맞지않다.
#define ERROR_NOQUICK				0x58		//퀵조인할 방이 없다. Client가 다시 CreateRoom을 날린다.
#define ERROR_NOTVER				0x59		//버전이 다를떄.
#define ERROR_MULTILOGIN			0x60		//중복로그인.


//****************************************************************************************
//정해진 수 관련.
//****************************************************************************************
#define MAX_ROOM_NAME				20
#define MAX_ID_LENGTH				24
#define MAX_NAME_LENGTH				20
#define MAX_NICK_LENGTH				20
#define MAX_ROOM_PASSWORD			10	
#define MAX_ROOM_IN					3
//#define MAX_CHANNELPER				120									//한채널의 Client총수.
#define MAX_CHANNELPER				120									//한채널의 Client총수.

#define MAX_CHANNEL					10									//채널수
#define MAX_CHANNEL_NAME			20									//채널명
#define PAGE_ROOMCNT				5									//한페이지당 방수.
#define MAX_ROOMCNT					MAX_CHANNELPER / FIX_GAME_USERCNT	//한 채널당 120 / 3 = 40
#define MAX_PAGE_CNT				MAX_ROOMCNT / PAGE_ROOMCNT			// 40 / 5 = 8  
#define FIX_GAME_USERCNT			3									//gostop은 2,3명이 친다.
#define MAX_CHATMSG					255									//채팅메세지의 최대길이.
#define MAX_IMAGE_LEN				7									//이미지 칼럼의 총길이. 7byte + NULL
#define MAX_IMAGE_CNT				10									//한사람이 갖을수 있는 이미지 총수.
#define MAX_GET_AVATARCNT			21									//한사람이 뺏을수 있는 아바타 총수( 10개 * 2byte ). + NULL
#define	MAX_IP_LEN					30
#define MAX_SERVERNAME				20


#define MASHINE_CNT				10
#define CHANNEL_PER_MASHINE		10
//****************************************************************************************
//일반구조체.
//****************************************************************************************
typedef struct tagRoomInfo	//클라이언트에게 보내주는 방정보
{
	short int nRoomNo;					//방번호
	char szRoomName[MAX_ROOM_NAME];		//방제목
	ush_int nCurCnt;					//현재인원
	ush_int nTotCnt;					//방총인원
	bool bSecret;						//비밀방 여부 ( true : 비밀방, false : 공개방 )
	char cGameDivi;						//게임중이냐. '0'대기중, '1'게임중
	ush_int nRoomMoney;					//판돈.
} sRoomInfo;

typedef struct tagRoomList	//서버가 가지고 있는 방정보
{
	sRoomInfo l_sRoomInfo;
	char szRoom_Pass[MAX_ROOM_PASSWORD];
} sRoomList;

typedef struct tagUserInfo	//클라이언트에게 보내주는 유저 정보
{
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	BigInt	nbUserMoney;				//유저의 돈.
	ush_int nClass;						//계습
	BigInt	nbMaxMoney;					//지금껏 최고로 마니 번돈.
	float	fWinRate;					//승률.
	//ush_int nTot_Num;					//전적.
	ush_int	nWin_Num;					//이긴횟수.
	ush_int	nLose_Num;					//패한횟수.
	ush_int	nDraw_Num;					//무승부횟수.
	ush_int	nCut_Time;					//끊긴횟수.
	ush_int nChannel;					//유저의 채널
	int nUserNo;						//유저의 고유넘버
	char cSex;							//'0' 여자 '1'남자
	ush_int nStripNumber;				//벗긴횟수.
	char szPreAvatar[MAX_IMAGE_LEN];	//대표아바타.
} sUserInfo;

typedef struct tagUserList	//Server가 가지고 있는 유저정보
{
	char szUser_Id[MAX_ID_LENGTH];		//유저아이디
	char szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];		//유저의 이미지.   2byte(이미지번호) + 2byte(현재점수) + 1byte(total단계) + 2byte(단계별점수)
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

//조인했을시에 뿌려줌.
typedef struct tagRoomOutOtherSend
{
	int			nUserNo;
	ush_int		nNewHostNo; //마스터가바뀌지 않으면 0이다. 0가 아니면 마스터의 번호가 들어간다.
	ush_int		nNewBbangJjnag; //방장가바뀌지 않으면 0이다. 0가 아니면 방장의 번호가 들어간다.
} sRoomOutOtherSend;

//방에서 접속이 바로 끊겼을때 또는 방으로 조인
typedef struct tagRoomInfoToWaitUserAX
{
	ush_int		nRoomNo;
	int			nUserNo;
	ush_int		nRoomCnt;
} sRoomInfoToWaitUserAX;

//방에서 나갔을때.
typedef struct tagRoomInfoToWaitUserR
{
	ush_int		nRoomNo;
	ush_int		nRoomCnt;
	sUserInfo	l_sUserInfo;

} sRoomInfoToWaitUserR;


//방에서 게임시작이나 게임이 끝났을떄.
typedef struct tagRoomGameWait
{
	ush_int		nRoomNo;
	char		cGameWait;			// 'G' Game 시작   'W' 대기 시작.
} sRoomGameWait;


//방에 유저가 그유저에게 방의 다른사람 정보를 보낸다.
//방에 들어온 유저 정보를 기존의 다른 유저에게 보낼떄도 사용한다.
typedef struct tagRoomInUserSend
{
	//int			nUserNo;
	bool		bRoomMaster;
	bool		bBbangJjang;
	sUserInfo	l_sUserInfo;
	char		szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];		//유저의 이미지.   2byte(이미지번호) + 2byte(현재점수) + 1byte(total단계) + 2byte(단계별점수)	
	char		szPrevImage[MAX_IMAGE_LEN];
} sRoomInUserSend;


typedef struct tagLogIn
{

	char szCSServerIp[MAX_IP_LEN];
	ush_int nPort;
	sUserList l_sUserList;

} sLogIn;


typedef struct tagGamingExitUser {
	
	ush_int	nNewBbangJjnag; //방장가바뀌지 않으면 0이다. 0가 아니면 방장의 번호가 들어간다.	
	int nUserNo;			//나간 유저의 고유넘버
	ush_int nGameUserNo;		//방에서의 게임유저 번호.
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
	bool bSecret;						//공개방 false 비밀방 true일경우 
	char szRoomName[MAX_ROOM_NAME];		//방제
	char szRoomPass[MAX_ROOM_PASSWORD];	
	ush_int nTotCnt;					//방 총인원 
} sCreateRoom;	

typedef struct tagRoomIn
{
	ush_int nRoomNo;
	char szRoomPass[MAX_ROOM_PASSWORD];
} sRoomIn;

typedef struct tagGameEnd
{
	BigInt	nbUserMoney;				//유저의 돈.
	BigInt	nbMaxMoney;					//지금껏 최고로 마니 번돈.
	float	fWinRate;					//승률.
	//ush_int nTot_Num;					//전적.
	ush_int	nWin_Num;					//이긴횟수.
	ush_int	nLose_Num;					//패한횟수.
	ush_int	nDraw_Num;					//무승부횟수.
	bool	bWinner;					//그판에서 이겼는지.
	ush_int nStripNumber;				//벗긴횟수.
	//ush_int	nCut_Time;					//끊긴횟수.
	char szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];		//유저의 이미지.   2byte(이미지번호) + 2byte(현재점수) + 1byte(total단계) + 2byte(단계별점수)
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
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	sGameEnd l_sGameEnd;
} sEndGameOther;

#pragma pack( pop, 1)


#endif
