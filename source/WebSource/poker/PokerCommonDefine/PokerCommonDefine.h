#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f 웬만하면 넘어가지마~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef __int64	BigInt;

#define PAGE_ROOMCNT				10									//한페이지당 방수.
#define CHANNEL_PER_MASHINE			10
#define MASHINE_CNT					10



#define MAX_ROOM_IN					5					//포커 한방에 7명 들어간다.	
#define MAX_NICK_LENGTH				21				
#define MAX_CHATMSG					255					//채팅메세지의 최대길이.
#define	MAX_IP_LEN					30
#define MAX_ID_LENGTH				50


#define MAX_AVAURLLEN				20					//아바타 Url길이.
#define MAX_CHANNELPER				100					//한채널의 Client총수.
#define MAX_SERVERNAME				20
#define	MAX_WAIT_ROOM				10					//대기실에서 방이 보이는 개수.
#define MAX_ROOM_NAME				20
#define MAX_ROOMCNT					100
#define MAX_PAGE_CNT				MAX_ROOMCNT / PAGE_ROOMCNT			//100 / 10 = 10
#define MAX_ROOM_PASSWORD			10	
#define MAX_USER_CARD				7					//유저가 가질수 있는 총카드.
#define MAX_CHANNEL					10									//채널수
#define MAX_INIT_CARD				4					//초기 카드 나눠주는
#define MAX_CARD					52
#define MAX_GAMESTART_MEM			2					//게임시작 인원

#define MAX_GAMEVIEWWAIT			30									//게임방에서 초대자 목록 보기.

#define MAX_GAMESTRMONEYLEN			25					
#define NOTICE_BYTE					100


#define MAX_PIRZECNT				11	// 

//#define MAX_MONEY_STRING			25					//유저머니 스트링.


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
	} sUserInfo;


typedef struct tagUserList	//Server가 가지고 있는 유저정보
{
	char szUser_Id[MAX_ID_LENGTH];		//유저아이디
	sUserInfo l_sUserInfo;
} sUserList;

typedef struct tagOption {
	bool bInvite;	//true : 초대가능
	bool bEarTalk;  //true : 귓말가능
	bool bMemoRecv; //true : 쪽지가능.
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

	char szUser_Id[MAX_ID_LENGTH];		//유저아이디
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

	typedef struct tagRoomInfo	//클라이언트에게 보내주는 방정보
	{
		short int nRoomNo;					//방번호
		char szRoomName[MAX_ROOM_NAME];		//방제목
		ush_int nCurCnt;					//현재인원
		ush_int nTotCnt;					//방총인원
		bool bSecret;						//비밀방 여부 ( true : 비밀방, false : 공개방 )
		ush_int nGameDivi;					//0 : 대기중, 1 : 스타트버튼대기중, 2 : 게임중, 3 : 엔딩중.
		BigInt biRoomMoney;					//판돈.
		BOOL bChoiceRoom;					//0 : 그냥방, 1 : 초이스방.
	} sRoomInfo;


typedef struct tagRoomIn
{
	ush_int nRoomNo;
	char szRoomPass[MAX_ROOM_PASSWORD];
} sRoomIn;

//내가 게임방에 들어갔을때.
typedef struct tagRoomInAdd
{
	ush_int nRoomInUserNo;
	ush_int nGameDivi;
	BigInt biRoomCurMoney;
} sRoomInAdd;


	typedef struct tagCreateRoom
	{
		bool bSecret;						//공개방 false 비밀방 true일경우 
		char szRoomName[MAX_ROOM_NAME];		//방제
		char szRoomPass[MAX_ROOM_PASSWORD];	
		ush_int nTotCnt;					//방 총인원 
		BigInt biRoomMoney;
		BOOL bChoiceRoom;					//TRUE 면 초이스방,.
	} sCreateRoom;	


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

typedef struct tagRoomInUserSend
{
	bool		bRoomMaster;
	bool		bBbangJjang;
	sUserInfo	l_sUserInfo;
	int nGameJoin;	//0 : 대기자, 1 : 참가자
	int	nCardCnt;
	BOOL bChoice;
	BYTE nUserCard[MAX_USER_CARD];
	//BYTE szJokboCard[8];
} sRoomInUserSend;



//조인했을시에 뿌려줌.
typedef struct tagRoomOutOtherSend
{
	int			nUserNo;
	int		nNewRoomMaster; //마스터가바뀌지 않으면 0이다. 0가 아니면 마스터의 번호가 들어간다.
	int		nNewBbangJjnag; //방장가바뀌지 않으면 0이다. 0가 아니면 방장의 번호가 들어간다.
	ush_int		nRoomCurCnt;
} sRoomOutOtherSend;

typedef struct tagRoomList	//서버가 가지고 있는 방정보
{
	sRoomInfo l_sRoomInfo;
	char szRoom_Pass[MAX_ROOM_PASSWORD];
} sRoomList;

//방에서 게임시작이나 게임이 끝났을떄.
typedef struct tagRoomGameWait
{
	ush_int		nRoomNo;
	char		cGameWait;			// 'G' Game 시작   'W' 대기 시작.
} sRoomGameWait;



//////////////////////////////////////////////////////////////////////////
///Poker게임에서 필요한 정보
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
	//BigInt biUserDieMoney;								//유저가 다이했을때 그 사림 금액을 화면에 뿌려주기위해.
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
		int nBeforeGameUserUniNo;	//6구째 마스터 넘버.
		int nWinnerUserUniNo;	//승리자.
		BigInt biWinnerUserMoney;
		BigInt biPureWinnerMoney;	//승라자의 순수 딴머니.
		int nCallUserUniNo;	//마지막 콜한사람 돈과 콜한사람유저번호.
		BigInt biCallGameUserMoney;
		char cUserSelect;	//마지막엔 콜.
		//BigInt biUserDieMoney;
		sUserTotalCardInfo l_sUserTotalCardInfo[MAX_ROOM_IN];	
		BigInt l_biUserMoney[MAX_ROOM_IN];	//방의 유저들 머니. 결과를 보여줘야 하기때문..승패 보여주는건 클라이언트서 하장
		BigInt l_biPureUserWinLoseMoney[MAX_ROOM_IN];	//유저가 순수 따고 잃은 머니.
		ush_int nJackPot;			//0: No JackPot, 1 : StiFul, 2 : RotiFul
		BigInt biUserJackPotMoney;		//유저의 잭팟 당첨 머니
		BigInt biRoomCurMoney;
		BYTE szJokboCard[MAX_ROOM_IN][8];
		BYTE szFixJokboCard[MAX_ROOM_IN][8];
	} sEndGame;

	typedef struct tagMyWinGame
	{
		int nWinnerUserUniNo;	//승리자.
		BigInt biWinnerUserMoney;	//승리자, 즉 나의 머니.
		BigInt biPureWinnerMoney;		//승라자의 순수 딴머니.
		int nCallUserUniNo;	//마지막 콜한사람 돈과 콜한사람유저번호.
		BigInt biCallGameUserMoney;
		char cUserSelect;	//마지막엔 콜.
		BigInt biRoomCurMoney;
		//BigInt biUserDieMoney;
		int l_nUserUniqNo[MAX_ROOM_IN];
		BigInt l_biUserMoney[MAX_ROOM_IN];	//방의 유저들 머니. 결과를 보여줘야 하기때문..승패 보여주는건 클라이언트서 하장
		BigInt l_biPureUserWinLoseMoney[MAX_ROOM_IN];	//유저가 순수 따고 잃은 머니.
	} sMyWinGame;


	typedef struct tagUserDBUpdateInfo
	{
		BigInt m_biRaceMoney;										//유저가 딜한 머니
		BigInt biRealRaceMoney;										//사이드 머니 뺀돈.
		int nWinLose;												//0 : 패배, 1 : 승리 
		int nCutTime;												//0 : 정상, 1 : 끊김
		BigInt biRoomMoney;											//방 기본머니
		char szOtherGameUser[MAX_ROOM_IN - 1][MAX_NICK_LENGTH];		//나와 게임한 유저들
		ush_int nJackPot;											//0: No JackPot, 1 : StiFul, 2 : RotiFul
		BigInt biUserJackPotMoney;									//유저의 잭팟 당첨 머니
		BigInt biSystemMoney;										//유저가 딴돈중 시스템 머니.
		int nSucceedCode;
		int nPrizeNo;
	} sUserDBUpdateInfo;

	typedef struct tagGameWaitInfo	//게임판에서 대기실 유저를 보는것..
	{
		char szNick_Name[MAX_NICK_LENGTH];	//유저닉네임.
		BigInt	biUserMoney;				//유저의 돈.
		int nUserNo;						//유저의 고유넘버
		char cSex;							//'0' 여자 '1'남자
	} sGameWaitInfo;



typedef struct tagUserInvite	//유저초대를 날린다.
{
	ush_int nRoomNo;
	BigInt	biRoomMoney;				
	char	szRoomPass[MAX_ROOM_PASSWORD];
	ush_int	nInviteMsgLen;					//이긴횟수.
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
#define NGSC_WAITINPUT				0x00		//대기실로 들어갈때 대기실 정보를 받아온다.
#define NGSC_CREATEROOM				0x01		//방을 만든다.
#define NGSC_ROOMIN					0x02		//방에 들어온다.
#define NGSC_USERINFOCHANGE			0x03		//유저리스트에 변화가 생길때.
#define NGSC_WAITINFOCHANGE			0x04		//대기실에 변화가 생길때( 룸리스트 + 유저리스트 )
#define NGSC_WAITCHAT				0x05		//대기실 채팅.
#define NGSC_VIEWPAGE				0x06		//룸리스트를 볼때.
#define NGSC_ROOMOUT_OTHERSEND		0x07		//방에서 유저가 나갔을때..다른사람들에게 정보를 날림.
#define NGSC_ROOMIN_OTHERSEND		0x08		//방에서 유저가 들어올때..다른사람들에게 정보를 날림.
#define NGSC_QUICKJOIN				0x09		
#define NGCSC_BROADSERVERINFO		0x0A		//서버들의 카운터를 실시간 전송해준다.
#define NGSC_GETSERVERLIST			0x0B		//Client가 카운터 서버로 서버들 정보 요청.
#define NGSC_ALLCMT					0x0C
#define NGSC_IPCHECK				0x0D		//같은방에서 한아이피를 막아야한다.
#define NGSC_ROOMINFO				0x0E		//대기실에서 방을 클릭했을때 방정보를 뿌려준다.
#define NGSC_GAMECHAT				0x0F
#define NGSC_STARTGAME				0x10		//게임 시작시에 초기 카드 세장을 준다.
#define NGSC_GETUSERINFO			0x11		//
#define NGSC_CARDCHOICE				0x12		//처음 세장중 하나를 초이스한다.
#define NGSC_GETONECARD				0x13		//한장을 나누어 준다
#define NGSC_SETGAMECURUSER			0x14		//자신의 할차례와 게임판돈, 버튼세팅등을 보내준다.
#define NGSC_ENDGAME				0x15		//게임의 승자와 각 유저의 카드 및 방 총 돈을 보낸다.
#define NGSC_MYWINGAME				0x16		//기권승.
#define NGSC_NAGARIGAME				0x17
#define NGSC_STARTBTN_ONOFF			0x18		//스타트 버튼 온 오프
#define NGSC_VIEWWAIT				0x19		//게임방에서 대기실 유저보기.
#define NGSC_USERINVITE				0x20		//유저를 초대.
#define NGSC_CARDDUMP				0x21		//한장 버림.
#define NGSC_CHOICECARDVIEW			0x22		//초이스카드를 모두에게 보내준다. 한번에 보기위해서.
#define NGSC_MYUSERJOKBO			0x23		//내 족보.
#define NGSC_GETOUT					0x24		//강퇴.
#define NGSC_MEMO					0x25		//쪽지 보내기.
#define NGSC_GETOUTQUES				0x26		//강퇴동의를 구한다.
#define NGSC_ALLUSERJOKBO			0x27		//모든 사람의 족보를 보낸다.
#define NGSC_INITUSERCARD			0x28		//초기 선택후 유저 카드를 한번씩 다시 보내준다.
#define NGSC_USERCHANGE				0x29		//유저가 원하는 카드로 바꿔준다.

//Client -> Server
//WaitMode
#define NGCS_WAITINPUT				0		//대기실로 들어갈때 대기실 정보를 받아온다.
#define NGCS_CREATEROOM				1		//방을 만든다.
#define NGCS_ROOMIN					2		//방에 들어간다.
#define NGCS_ROOMOUT				3		//방을 나간다.
#define NGCS_QUICKJOIN				4		//퀵조인.
#define NGCS_WAITCHAT				5		//대기실 채팅.
#define NGCS_VIEWPAGE				6		//룸리스트를 볼때.
#define NGCS_ROOMINFO				7		//대기실에서 방을 클릭했을때 방정보를 뿌려준다.
#define NGCS_GETUSERINFO			9		
#define NGCS_MEMOSEND				10		//쪽지 보내기.

// -- LOGINSERVER 부분에 정의 #define NGCS_KEEPALIVE				0x08		//KeepAlive


//GameMode
#define NGCS_GAMECHAT				0		
#define NGCS_GAMESTART				1
#define NGCS_CARDCHIOCE				2
#define NGCS_USERSELECT				3		//call, quater 등 선택.
#define NGCS_RESTART				4		//게임이 끝나고 점수를 보고나서 다시 시작하려 할때.
#define NGCS_VIEWWAIT				5		//게임방에서 대기실 유저보기.
#define	NGCS_USERINVITE				6		//유저를 초대한다.
#define NGCS_CARDDUMP				7		//카드 한장 버리기.
#define NGCS_GETOUT					8		//유저 강퇴.
#define NGCS_GETOUTQUESREPL			9		//유저 강퇴에 대한 동의 답변.
#define NGCS_USERCHANGE				10		//유저 카드를 바꾼다.

//AllMODE
#define NGCS_SETOPTION				0		//옵션저장.
#define NGCS_ALLCMT					2		//모두에게 공지를 날린다. 바꾸면 안됨.


//CS server Mode
#define NGCS_GETSERVERLIST			0x00		//Client가 카운터 서버로 서버들 정보 요청.


//////////////////////////////////////////////////////////////////////////
//LOGIN SERVER와 관련
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
//LOGIN SERVER와 관련
//////////////////////////////////////////////////////////////////////////





//ErrorCode
// Server Error Code
#define ERROR_LIMIT_USER			0x83		//채널당 유저를 초과 했다.
#define ERROR_NOCREATEROOM			0x84		//방을 생성할수 없다.( 더이상방을 만들수 없다)
#define ERROR_NOROOM				0x85		//RoomIn했을시에 없는방이다.
#define ERROR_ROOMFULL				0x86		//RoomIn했을시에 방에 인원이 꽉차있다.
#define	ERROR_NOTPWD				0x87		//비밀방일때 Password가 맞지않다.
#define ERROR_NOQUICK				0x88		//퀵조인할 방이 없다. Client가 다시 CreateRoom을 날린다.
#define ERROR_NOTOPTION				0x91		//귓말, 초대, 메세지 거부.
#define ERROR_NOCARD				0x92		//유저가 카드를 바꾸려 했는데 유저에게 없는 카드이다.
#define ERROR_NOCARDPACK			0x93		//유저가 바꾸길 원하는 카드였는데 카드팩에 없는 카드이다.


#define SC_PKTLEN_POS               2



#define SYSTEM_MONEY_RATE			5			//시스템 머니 5%
#define JACKPOT_MONEY_RATE			10			//시스템 머니에서 10% 전체 금액에서 0.5%

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
	//잭팟 종류
	#define JACKPOT_CNT				8
	
	//방종류 
	#define ROOM_DIVI_CNT			8

	//잭팟 적립 비율.
	#define	JACKPOT_FOUR_CARD  	    0.2
	#define	JACKPOT_STIFUL			2
	#define JACKPOT_ROTIFUL			4





#pragma pack( pop, 1)



#endif
