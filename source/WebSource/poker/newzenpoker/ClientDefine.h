#ifndef _CLIENTDEFINE_H_
#define _CLIENTDEFINE_H_


#include "..\PokerCommonDefine\PokerCommonDefine.h"

//
//#define SHOWUSERCARD                    // 상대방 카드를 보여줄지를 결정하는 상수
//#define CHANGEUSERCARD                  // 명령어를 이용해서 자신의 카드를 바꿀 수 있는지 여부를 결정하는 상수
//

#define SERVICE_TORIZONE    0           // 토리존 게임
#define SERVICE_GAMEPOOL    1           // 게임풀 게임

#define SCREEN_WIDTH        1024        // 화면의 가로 길이
#define SCREEN_HEIGHT       768         // 화면의 세로 길이

#define CHANNEL_MODE        0
#define WAITROOM_MODE       1
#define GAME_MODE           2
#define LOGO_MODE           3


#define MAX_SOUND_CNT       200
#define MAX_CHAT_COUNT      25
#define MAX_DESTROY_TIME    10000
#define MAX_WAITUSERCNT     12          //대기실에 유저를 보여주는 숫자

#define SC_PKTLEN_POS       2
#define CCPAKETLEN          2
#define DEF_TIME            600			//채팅 메세지 보내는 타임.
#define VIEWMINUSCNANNEL    5		    //채널을 6개만 보여준다....이수를 빼준다..
#define NGCC_GAMECHAT       0x60



#define DEF_TIME            600
#define MAX_BTNTIMECOUNT    100


#define MAX_TEXT_LINE       100         // 채팅창에 저장되는 최대 텍스트 라인 수
#define MAX_VIEW_WAIT_CHAT  5           // 화면에 보여지는 대기실 채팅창 텍스트 라인 수
#define MAX_VIEW_GAME_CHAT  10          // 게임화면 채팅창에 보여지는 텍스트 라인의 갯수

#define MAX_WAIT_USER       200         // 대기실 최대 유저 수
#define MAX_VIEW_WAIT_USER  12          // 한번에 보이는 최대 대기자 수

#define MAX_VIEW_WAIT_ROOM  9           // 한 화면에 보이는 대기실 수

#define MAX_CHIP_ANI        200         // 한번이 에니메이션이 가능한 톨의 수
#define REMOVE_CHIP_TIME    20          // 톨이 투명해지면서 사라지는 시간(FRAME)

#define MAX_VIEW_CHODAE_USER 10         // 초대창에서 보여지는 최대 유저 수



//////////////////////////////////////////////////////////////////////////
///SOUND
//////////////////////////////////////////////////////////////////////////
#define GS_BTNCLICK			0		//Button_click.wav		(X)
#define GS_ALLIN			1
#define GS_BBING			2
#define GS_CALL				3
#define GS_CHECK			4
#define GS_CHIP				5
#define GS_DEAL				6
#define GS_DIE				7
#define GS_DOUBLE			81
#define GS_FULL				9
#define GS_HALF				10
#define GS_JACKPOT			11
#define GS_QUARTER			12
#define GS_BACKSTRAIGHT		13
#define GS_FLUSH			14
#define GS_FOURCARD			15
#define GS_FULLHOUSE		16
#define GS_MOUNTAIN			17
#define GS_ONEPAIR			18
#define GS_RSFLUSH			19
#define GS_STRAIGHT			20
#define GS_STRAIGHTFLUSH	21
#define GS_TOP				22
#define GS_TRIPLE			23
#define GS_TWOPAIR			24
#define GS_ROOMEXIT			25
#define GS_ROOMIN			26
#define GS_POPUP			27		//Popup.wav		
#define GS_WINNER			28
#define GS_LOSE				29
#define GS_CARDSHRINK		30
#define GS_COUNT			31
#define GS_DEALSTART		32

#define GS_WAITBGM			180		//waitroom.wav
#define GS_LOGO				181


///TIME

#define ONSTARTBTN_TIME		500		//스타트 버튼 타임.
#define CHOICEOPEN_TIME		25		//카드를 다 나누어주고서 잠시후에 초이스 화면을 띠운다.
#define CHOICECARD_TIME		500		//카드 초이스 타임.
#define ENDDING_TIME		250		//결과창 떠있는 시간.
#define CALL_TIME			500		//유저가 콜을 부를수 있는 타임. 타임이 지나면 자동 다이를 콜해버린다.

#define WM_OUICKFAILCREATEROOM	(WM_USER + 0x16)	//퀵조인실패시 방을 바로 만들어 준다.
#define WM_GAMEMODESET			(WM_USER + 0x17)	//게임모드전환
#define WM_CHANNELMODESET		(WM_USER + 0x18)	//채널모드전환
#define WM_WAITMODESET			(WM_USER + 0x19)	//대기모드전환.
#define WM_AVATARPLAY			(WM_USER + 0x20)	

#define IDE_WAITEDIT	        1011
#define IDE_GAMEEDIT	        1012
#define IDE_MSGEDIT		        1013
#define IDE_ROOMCOMBO           1014

#define INIT_CARD_POS_X			534     // 카드 나눠주는 시작 위치(X) - 기존 : 476
#define INIT_CARD_POS_Y			42      // 카드 나눠주는 시작 위치(Y) - 기존 : 62

#define CARD_WIDTH              82      // 카드 가로 크기
#define CARD_HEIGHT             109     // 카드 세로 크기
#define CARD_DISTANCE_A         38      // 카드 선택 전 카드의 간격
#define CARD_DISTANCE_B         19      // 카드 선택 후 카드의 간격

#define ROOM_TITLE_LIST_X       443     // 방만들기 창 - 방 타이틀 리스트 출력 좌표( X )
#define ROOM_TITLE_LIST_Y       287     // 방만들기 창 - 방 타이틀 리스트 출력 좌표( Y )
#define ROOM_TITLE_LIST_WIDTH   200     // 방만들기 창 - 방 타이틀 리스트 가로 크기
#define ROOM_TITLE_LIST_HEIGHT  81      // 방만들기 창 - 방 타이틀 리스트 세로 크기
#define ROOM_TITLE_BAR_HEIGHT   14      // 방만들기 창 - 방 제목 리스트 바의 세로 크기

#define CONFIG_DLG_POS_X        349     // 옵션창의 위치
#define CONFIG_DLG_POS_Y        250     // 옵션창의 위치
#define ALARM_MSG_POS_X         349     // 알림 메시지 상자 출력 좌표
#define ALARM_MSG_POS_Y         269     // 알림 메시지 상자 출력 좌표
#define PASS_DLG_POS_X          349     // 패스워드 입력용 대화상자 좌표
#define PASS_DLG_POS_Y          297     // 패스워드 입력용 대화상자 좌표
#define MEMO_DLG_POS_X          349     // 쪽지(메모) 대화상자 출력 좌표
#define MEMO_DLG_POS_Y          250     // 쪽지(메모) 대화상자 출력 좌표
#define CHODAE_POS_X            349     // 초대하기 대화상자 좌표
#define CHODAE_POS_Y            174     // 초대하기 대화상자 좌표
#define CHODAE_RECV_POS_X       407     // 초대 받았을 때의 대화상자 좌표
#define CHODAE_RECV_POS_Y       299     // 초대 받았을 때의 대화상자 좌표


//아바타 크기.
#define SMALL_SIZE		        0
#define ORIGINAL_SIZE	        1

// 방만들기 - 패 옵션
#define CO_NORMAL_MODE          0
#define CO_CHOICE_MODE          1

//유저의 환경 변수.
typedef struct tagClientEnviro
{
	BOOL bOption[4];
	ush_int nVoice;	// 0 : 하소연 목소리, 1 : 남자 성우, 2 : 여자 성우
} sClientEnviro;

//방만들기 할때 금액을 찍는 위치.
typedef struct tagRoomInUserPosition
{
	POINT pAvatarPos;			//아바타 위치.
	POINT pCardStartPos;		//유저카드 위치
	POINT pCardInfo;			//유저카드 정보창 위치
	POINT pNickPos;				//유저 닉넴 위치
	POINT pBossPos;				//보스 위치
	POINT pBangJangPos;			//방장위치
	POINT pUserMoneyPos;		//유저 머니 위치
	POINT pJokboPos;			//족보 찍는 위치
	POINT pUserSelectBigPos;	//유저 콜, 다이 등 큰사이즈 위치
	POINT pUserSelectSmallPos;	//유저 콜, 다이 등 작은사이즈 위치
	POINT pWinnerPos;			//승리시 Winner 찍는 위치
	POINT pTimePos;				//콜, 다이시 시계찍는 위치
	POINT pAllInPos;			//올인 마크 표시.
    POINT pChipStartPos;        //톨 던지는 시작 위치 ham 2005.09.26
} sRoomInUserPosition;


enum CARDTYPE { PACK, FLOOR, USERCARD };

typedef struct tagUserCardInfo
{
	POINT NewPos;
	POINT StartPos;
	POINT Pos;
	int nAniCount;
	int nAniMaxCount;
	BYTE nCardNo;
	CARDTYPE ctCardType;
} sUserCardInfo;



// 마우스로 드래그 중인 히든 카드 정보 - ham 2005.09.29
typedef struct tagMovingHiddenCardInfo : public tagUserCardInfo
{
    int nOffsetY;               // 카드 위를 마우스로 눌렀을 때, 카드 맨 위로 부터 마우스 위치까지의 상대적 거리
} sMovingHiddenCardInfo;



// 톨 에니메이션에 필요한 구조체 - ham 2005.09.25

enum CHIPTYPE
{
    CT_NONE = 0,    // 톨이 없다
    CT_MOVE,        // 이동중인 톨
    CT_REMOVE       // 사라지고 있는 톨
};

typedef struct tagChipInfo
{
	POINT NewPos;
	POINT StartPos;
	POINT Pos;
	int nAniCount;
	int nAniMaxCount;
    int nRemoveCnt;         // 톨이 사라질 때 사용하는 카운터
	BYTE nChipNo;
	CHIPTYPE ctChipType;
} sChipInfo;



typedef struct tagWinnerCardInfo
{
	char szNick_Name[MAX_NICK_LENGTH];
	int nCardCnt;
	BYTE nCardNo[MAX_CARD];
	BYTE szJokboCard[8];
} sWinnerCardInfo;


typedef struct tagPanMoneyInfo
{
	POINT PanMoneyPos;
	int nPanMoneyCnt;
} sPanMoneyInfo;


typedef struct tagInviteCheck
{
	int  Y;  // 체크의 Y 좌표
	BOOL bCheck; // 체크가 표시되면 TRUE , 그렇지 않으면 FALSE
	
}sINVITECHECK;



// 채팅창의 텍스트 한 줄을 저장하는 구조체
typedef struct tagSTextInfo
{
    COLORREF    rgbColor;                       // 텍스트의 색깔( RGB(red, green, blue) )
    int         nStyle;                         // 텍스트 스타일(0=보통, 1=굵게)
    char        szText[256];                    // 텍스트
} STextInfo;


#endif
