#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f 웬만하면 넘어가지마~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef __int64	BigInt;

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
//#define NGSC_HEADERSEND				0x00		//처음 Client접속시 정해진 Header을 보내준다.
#define NGSC_WAITINPUT				0x01		//대기실로 들어갈때 대기실 정보를 받아온다.
#define NGSC_CREATEROOM				0x02		//방을 만든다.
#define NGSC_ROOMIN					0x03		//방에 들어간다.
#define NGSC_ROOMOUT				0x04		//방을 나간다.
#define NGSC_USERINFOCHANGE			0x05		//유저리스트에 변화가 생길때.
#define NGSC_WAITINFOCHANGE			0x06		//대기실에 변화가 생길때( 룸리스트 + 유저리스트 )
#define NGSC_WAITCHAT				0x07		//대기실 채팅.
#define NGSC_VIEWPAGE				0x08		//룸리스트를 볼때.
#define NGSC_ROOMOUT_OTHERSEND		0x0A		//방에서 유저가 나갔을때..다른사람들에게 정보를 날림.
#define NGSC_ROOMIN_OTHERSEND		0x0B		//방에서 유저가 들어올때..다른사람들에게 정보를 날림.
#define NGSC_QUICKJOIN				0x0C		
#define NGSC_TURNMSG				0x0E		//Client에서 온 packet을 그대로 다른 Client로 보내준다.
#define NGSC_GAMEEND				0x0F		//Client의 정보를 Update하고 난후.
#define NGSC_VIEWWAIT				0x10		//Client가 게임방안에서 대기실의 목록을 볼때.
#define NGSC_USERINVITE				0x11		//Client가 게임방에서 대기실 유저를 초대한다.
#define NGSC_GAMINGEXITUSER			0x13		//게임중에 유저가 나갔을때..
#define NGCSC_BROADSERVERINFO		0x14		//서버들의 카운터를 실시간 전송해준다.
#define NGSC_GETSERVERLIST			0x15		//Client가 카운터 서버로 서버들 정보 요청.
#define NGSC_MEMO					0x16		
#define NGSC_STARTGAME				0x17
#define NGSC_GETAVAURL				0x18
#define NGSC_ALLCMT					0x19
#define NGSC_IPCHECK				0x1A		//같은방에서 한아이피를 막아야한다.
#define NGSC_GAMESUNSET				0x1B		//선 결정.
#define	NGSC_CHOICECARDNO			0x1C		//선 결정시 카드 초이스 완료.
#define NGSC_SUNDECIDE				0x1D		//선 결정.
#define NGSC_RECHOICE				0x1E		//선다시 결정.
#define NGSC_GETUSERINFO			0x1F		//
#define NGSC_ROOMINFO				0x20		//방안에 사람들정보.

#define NGSC_GAMEREALSTARTBTN		0x21
#define NGSC_SETVOICE				0x22		//방목소리를 바꾼다.


//#define NGSC_GAMECOMSTARTBTN		0x22

//////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////






//Client -> Server
//WaitMode
#define NGCS_WAITINPUT				0		//대기실로 들어갈때 대기실 정보를 받아온다.
#define NGCS_CREATEROOM				1		//방을 만든다.
#define NGCS_ROOMIN					2		//방에 들어간다.
#define NGCS_ROOMOUT				3		//방을 나간다.
#define NGCS_QUICKJOIN				4		//퀵조인.
#define NGCS_WAITCHAT				5		//대기실 채팅.
#define NGCS_VIEWPAGE				6		//룸리스트를 볼때.
#define NGCS_EXITGAME				7		//게임에서 완전히 나간다.
//#define NGSC_KEEPALIVE			8		//위에 있다.
#define NGCS_GETAVAURL				9		//
#define NGCS_GETUSERINFO			10		
#define NGCS_ROOMINFO				11		//방안에 사람들.

#define NGCS_ADMINJUSTVIEW  		12		//관전

//GameMode
#define NGCS_TURNMSG				0		//Client에서 온 packet을 그대로 다른 Client로 보내준다.
#define NGCS_GAMEEND				1		//Client에서 게임이 끝났을떄..
#define NGCS_VIEWWAIT				3		//Client가 게임방안에서 대기실의 목록을 볼때.
#define NGCS_USERINVITE				4		//Client가 게임방에서 대기실 유저를 초대한다.
#define NGCS_GAMINGEXITUSER			5		//게임중에 유저가 나갔을때..
#define NGCS_STARTREADY				6		//게임준비.
#define NGCS_GAMESUNSET				7
#define NGCS_SUNCHOICE				8		
#define NGCS_AUTOCHOICE				9


//AllMODE
#define NGCS_MEMOSEND				0		//쪽지 보내기.
#define NGCS_SETOPTION				1		//옵션저장.
#define NGCS_ALLCMT					2		//모두에게 공지를 날린다.



/*
//CSServerMode
#define NGCS_GETSERVERLIST			0x00		//Client가 카운터 서버로 서버들 정보 요청.
#define NGSS_GSCSNUMMINUS			0x01		//게임서버에서 클라이언트가 접속을 끊으면 카운터 서버에서 현재인원을 감소시킨다.
#define NGSS_GSCSSERVERSET			0x02		//게임서버가 카운터 서버로 접속하면서 날린다.
#define NGSS_GSCSNUMPLUS			0x03		//유저가 게임서버로 접속할때 카운터서버로 이걸날려서 접속카운트를 증가한다.
#define NGSS_GSCSTOTCLIENTNUM		0x04		//서버의 토탈 클라이언트수를 보낸다.
*/
//ErrorCode
// Server Error Code
#define ERROR_LIMIT_USER			0x83		//채널당 유저를 초과 했다.
#define ERROR_NOCREATEROOM			0x84		//방을 생성할수 없다.( 더이상방을 만들수 없다)
#define ERROR_NOROOM				0x85		//RoomIn했을시에 없는방이다.
#define ERROR_ROOMFULL				0x86		//RoomIn했을시에 방에 인원이 꽉차있다.
#define	ERROR_NOTPWD				0x87		//비밀방일때 Password가 맞지않다.
#define ERROR_NOQUICK				0x88		//퀵조인할 방이 없다. Client가 다시 CreateRoom을 날린다.
#define ERROR_NOTOPTION				0x91		//귓말, 초대, 메세지 거부.

//****************************************************************************************
//정해진 수 관련.
//****************************************************************************************
#define MAX_ROOM_NAME				20

#define MAX_ID_LENGTH				50


#define MAX_NAME_LENGTH				20
#define MAX_NICK_LENGTH				21
#define MAX_ROOM_PASSWORD			10	
#define MAX_GAMESTRMONEYLEN			25					
#define MAX_ROOM_IN					2

#define MAX_CHANNELPER				100									//한채널의 Client총수.
#define MAX_CHANNEL					10									//채널수
#define MAX_CHANNEL_NAME			20									//채널명

#define PAGE_ROOMCNT				10									//한페이지당 방수.


//#define MAX_ROOMCNT				MAX_CHANNELPER / MAX_ROOM_IN		//한 채널당 120 / 3 = 40  맞고 120 / 2 = 60
#define MAX_ROOMCNT					100
#define MAX_PAGE_CNT				MAX_ROOMCNT / PAGE_ROOMCNT			// 100 / 5 = 20  맞고 100 / 10 = 10
#define MAX_CHATMSG					255									//채팅메세지의 최대길이.
#define MAX_IMAGE_LEN				7									//이미지 칼럼의 총길이. 7byte + NULL
#define MAX_IMAGE_CNT				10									//한사람이 갖을수 있는 이미지 총수.
#define MAX_GET_AVATARCNT			21									//한사람이 뺏을수 있는 아바타 총수( 10개 * 2byte ). + NULL
#define	MAX_IP_LEN					30
#define MAX_SERVERNAME				20
#define MAX_GAMEVIEWWAIT		30									//게임방에서 초대자 목록 보기.


#define MASHINE_CNT					10
#define CHANNEL_PER_MASHINE			10
#define MAX_AVAURLLEN				20									//아바타 Url길이.

//#define MAX_MINUS_CHANNELPER		20									//100명까지만 받는다.
#define MAX_SUNCARD_CNT				10									//처음 선뽑기 카드수
#define NOTICE_BYTE					100
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
	BigInt biRoomMoney;					//판돈.
} sRoomInfo;

typedef struct tagRoomList	//서버가 가지고 있는 방정보
{
	sRoomInfo l_sRoomInfo;
	char szRoom_Pass[MAX_ROOM_PASSWORD];
} sRoomList;

typedef struct tagUserInfo	//클라이언트에게 보내주는 유저 정보
{
	bool bInvite;
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	BigInt	biUserMoney;				//유저의 돈.
	ush_int nClass;						//계급
	BigInt	biMaxMoney;					//지금껏 최고로 마니 번돈.
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

typedef struct tagNotice
{
	char	m_Notice1[NOTICE_BYTE];
	char	m_Notice2[NOTICE_BYTE];
} sNotice;


typedef struct tagWaitUserInfo	//클라이언트에게 보내주는 유저 정보
{
	bool bInvite;
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	BigInt	biUserMoney;				//유저의 돈.
	ush_int nClass;						//계습
	BigInt	biMaxMoney;					//지금껏 최고로 마니 번돈.
	float	fWinRate;					//승률.
	ush_int	nWin_Num;					//이긴횟수.
	ush_int	nLose_Num;					//패한횟수.
	ush_int	nDraw_Num;					//무승부횟수.
	ush_int	nCut_Time;					//끊긴횟수.
	ush_int nChannel;					//유저의 채널
	int nUserNo;						//유저의 고유넘버
	char cSex;							//'0' 여자 '1'남자
	char szGameGubun[5];				// '0' : Neo, '1' : 천리안.
} sWaitUserInfo;  //대기실의 유저는 아바타를 제외한다. 필요할때 직접 받는다.



typedef struct tagUserList	//Server가 가지고 있는 유저정보
{
	char szUser_Id[MAX_ID_LENGTH];		//유저아이디
	sUserInfo l_sUserInfo;
} sUserList;

typedef struct tagUserInvite	//Server가 가지고 있는 유저정보
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


typedef struct tagRoomInUserSend
{
	bool		bRoomMaster;
	bool		bBbangJjang;
	sUserInfo	l_sUserInfo;
	char		szGameGubun[5];				// '0' : Neo, '1' : 천리안.
	int			nVoice;
} sRoomInUserSend;

//내가 게임방에 들어갔을때.
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
	
	int	nNewBbangJjnag; //방장가바뀌지 않으면 0이다. 0가 아니면 방장의 번호가 들어간다.	
	int nUserNo;			//나간 유저의 고유넘버
	int nGameUserNo;	//방에서의 게임유저 번호.
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
typedef struct tagGameWaitInfo	//게임판에서 대기실 유저를 보는것..
{
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	BigInt	biUserMoney;				//유저의 돈.
	int nUserNo;						//유저의 고유넘버
	char cSex;							//'0' 여자 '1'남자
} sGameWaitInfo;

	

//방에 유저가 그유저에게 방의 다른사람 정보를 보낸다.
//방에 들어온 유저 정보를 기존의 다른 유저에게 보낼떄도 사용한다.
typedef struct tagRoomInOtherSend
{
	ush_int nNextNo;
	ush_int nRoomCurCnt;
	int	nBbangJjangUniqNo;
	sUserInfo l_sUserInfo;
} sRoomInOtherSend;

typedef struct tagOption {
	bool bInvite;	//true : 초대가능
	bool bEarTalk;  //true : 귓말가능
	bool bMemoRecv; //true : 쪽지가능.
	int nVoice;
} sOption;



typedef struct tagPKWaitInputCS
{
	sUserList l_sUserInfo;
	sOption l_sOption;
} sPKWaitInputCS;

typedef struct tagCreateRoom
{
	bool bSecret;						//공개방 false 비밀방 true일경우 
	char szRoomName[MAX_ROOM_NAME];		//방제
	char szRoomPass[MAX_ROOM_PASSWORD];	
	ush_int nTotCnt;					//방 총인원 
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
	int  m_nEvalution;								//유저가 먹은 점수.
	int m_nShakeCnt;
	int m_nBbukCnt;									//뻑 카운트
	int m_nGoCnt;
	bool m_bGodori;
	bool m_bHongdan;
	bool m_bChongdan;
	bool m_bGusa;	
	bool m_bMmong;
	bool m_bShake;
	bool m_bMission;
	bool m_bPbak; // 피박
	bool m_bKwangbak; // 광박
	bool m_bDokbak; // 독박	
	int m_nMulti;									//유저가 나중에 *2 배 * 3배 표시.
	int m_nMissionMultiply;
} sGameEndHistory;


typedef struct tagGameEnd
{
	BigInt	biObtainMoney;									//유저가 이번판에 딴돈. 무조건(+)
	BigInt  biLoseMoney;									//유저가 이번판에 잃은돈. 무조건(+)
	int 	nWinner;										//0 : 패, 1 : 승, 2 : 비김
	char	szOtherNick_Name[MAX_ROOM_IN - 1][MAX_NICK_LENGTH];	//게임을 친 상대 유저 닉네임.
	BigInt	biSystemMoney;				//시스템 머니.	
	ush_int nPrizeNo;
	ush_int nSucceedCode;				// 0 이면 실패, 1이면 성공
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
	char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
	sGameEnd l_sGameEnd;
} sEndGameOther;


typedef struct tagPaintAvatar
{
	BOOL bPrint[3];
} sPaintAvatar;


typedef struct tagGetUserInfo
{

	char szUser_Id[MAX_ID_LENGTH];		//유저아이디
	ush_int nUserChannel;

} sGetUserInfo;

typedef struct tagIsLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;

} sIsLogIn;


//디비 업데이트.
typedef struct tagUserDBUpdateInfo
{
	BigInt biObtainMoney;										//유저가 얻거나 잃은 머니
	BigInt biLoseMoney;											//유저가 얻거나 잃은 머니
	int	   nWinLose;											//0 : 패, 1 : 승, 2 : 비김
	BigInt biRoomMoney;											//방 기본머니
	BigInt biSystemMoney;										//유저가 딴돈중 시스템 머니. 
	char szOtherGameUser[MAX_ROOM_IN - 1][MAX_NICK_LENGTH];		//나와 게임한 유저들
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
	int			nNewHostNo; //마스터가바뀌지 않으면 0이다. 0가 아니면 마스터의 번호가 들어간다.
	int			nNewBbangJjnag; //방장가바뀌지 않으면 0이다. 0가 아니면 방장의 번호가 들어간다.
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

//방종류 
#define ROOM_DIVI_CNT				8



#pragma pack( pop, 1)


#endif
