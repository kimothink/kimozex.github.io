#ifndef _CLIENTDEFINE_H_
#define _CLIENTDEFINE_H_

#include "..\\..\\..\\cdxlib\\cdx.h"
#include "..\\..\\..\\cdxlib\\cdxbutton.h"
#include "..\\..\\..\\cdxlib\\cdxime.h"
#include "..\\..\\..\\cdxlib\\cdxinput.h"
#include "..\\..\\..\\cdxlib\\cdxscrollbar.h"
#include "..\\..\\..\\cdxlib\\cdxsound.h"
#include "..\\..\\..\\cdxlib\\cdxsprite.h"
#include "..\\..\\..\\cdxlib\\cdxtile.h"

//
// #define SHOWUSERCARD                // 상대방 유저의 패를 보게할지의 여부 지정
// #define CHANGEUSERCARD              // 자신의 카드를 바꿀 수 있는지의 여부 설정
// #define ADMINJUSTVIEW               // 관전 Only
//// 


#define SCREEN_WIDTH    1024        //화면 가로 크기
#define SCREEN_HEIGHT   768         //화면 세로 크기


#define		H_20		0x08		//광
#define		H_10		0x04		//십긋
#define		H_5			0x02		//5긋
#define		H_1			0x01		//피
#define		H_2			0x10		//쌍피
#define		H_3			0x20		//쓰리피


#define GS_BTNCLICK			0		//Button_click.wav		(X)
#define GS_ROOMJOIN			1		//Room_Join.wav
#define GS_ROOMEXIT			2		//Room_Exit.wav
#define GS_START			3		//GameStart.wav
#define GS_GAMECLEAR		4		//Game_clear.wav
#define GS_HWATOOCLICK		5		//Hwatoo_click.wav		(X)
#define GS_HWATOOFRPUT		6		//Hwatoo_put.wav
#define GS_HWATOOMATCH		7		//Hwatoo_match.wav
#define GS_HWATOOMOVE		8		//Hwatoo_move.wav
#define GS_HWATOOSHARE		9		//Hwatoo_share.wav
#define GS_MISS				10		//Gsmiss.wav
#define GS_MYORDER			11		//My_order.wav
#define GS_WIN				12		//Game_win.wav
#define GS_LOST				13		//Game_lost.wav
#define GS_MISSION_START	14		//Mission.wav
#define GS_MISSION_COMPLETE	15		//Mission_complete.wav
#define GS_POPUP			16		//Popup.wav				(X)
#define GS_COUNT			17		//Count.wav
#define GS_CARDUP			18		
#define GS_HWATOOMATCH_1GO	19		//hwatoo_match_1goaf.wav
#define GS_HWATOOMATCH_2GO	20		//hwatoo_match_2goaf.wav
#define GS_HWATOOMATCH_3GO	21		//hwatoo_match_3goaf.wav
#define GS_BOOM				22		//bom.wav
#define GS_RESULT           23		//result_dojang.wav
#define GS_CARDPACKSTART	24



#define GS_CHODAN			50		//chodan.wav
#define GS_CHUNGDAN			51		//chungdan.wav
#define GS_HONGDAN			52		//hongdan.wav
#define GS_GODORI			53		//godori.wav
#define GS_GETONEPEE		54		//gsgetonepee.wav
#define GS_GETTWOPEE		55		//gsgettwopee.wav
#define GS_GETTHREEPEE		56
#define GS_GETFOURPEE		57
#define GS_GETFIVEPEE		58
#define GS_STOP				59		//gsstop.wav
#define	GS_MADE				60		//gsmade.wav
#define GS_GO				61		//
#define GS_SHAKE			70		//gsshake.wav
#define GS_SSAM				71		//sasam.wav
#define GS_JJOK				72		//jjok.wav
#define GS_NAGARI			73		//nagari.wav
#define GS_CHONGTONG		74		//chongtong.wav
#define GS_DOKBAK			75		//dokbak.wav
#define GS_3KWNAG			76		//3kwang.wav
#define GS_4KWNAG			77		//4kwnag.wav
#define GS_5KWNAG			78		//5kwnag.wav
#define GS_BONUS			79		//gsbonus.wav
#define GS_PANSL			80		//pansl.wav
#define GS_AFTERJJOK		81		//afterjjok.wav
#define GS_DADDAK			82		//ddadak.wav

#define GS_WAITBGM			180		//waitroom.wav


extern BYTE g_pGoStopValue[];


#define NOTICE_NONE		0
#define NOTICE_THREE	1
#define NOTICE_BOOM		2
#define NOTICE_SHAKE	3


//바닥에 카드 정보.
struct SFLOORCARD
{
	BYTE nCardNo;					//카드의 번호
	BYTE nPosNo;					//카드의 위치
};

#define CHANNEL_MODE	0
#define WAITROOM_MODE	1
#define GAME_MODE		2
#define LOGO_MODE		3
#define GAMEVIEW_MODE	4

#define MAX_FLOOR_CARD					60				//바닥에 카드가 깔리는 최대수
#define	MAX_CARD_POS					17				//바닥에 카드의 위치수. 16군데에 깔린다.	17번은 못먹고 뒤짚은패가 서비스일때 놓일위치.
#define MAX_CARDPACK_CNT				60				//하나의 카드팩이 최대 몇개가 될수 있나.
#define MAX_USERCARD_CNT				12				//유저가 최대가질수 있는 패수.
#define MASTER_POS						0				//방장의 슬롯 위치...2
#define REAL_CARD_MAXNO					48				//48번까지가 진짜 카드 넘버..그이후는 서비스카드
#define MAX_SOUND_CNT					250
#define BOMB_CARDNO						51
#define BBUK_X							3				//첫뻑, 연뻑 했을때의 판돈에 곱하는수, 첫따닥,
#define TEMP_STR_LENGTH					500

#define WAIT_PAINT_TIME					60000			//60초
#define WAIT_THREAD_KILLTIME			5000

#define USER_WAIT_TIME					3				//3초후부터 유저카운터를 들어간다.
#define WAIT_STARTTIME					60000			//60초 
#define WAIT_1TIME						1000			//1초.
#define WAIT_5TIME						5000			//5초.
#define WAIT_60TIME						60000			//60초 ( 대통령, shake  )
#define WAIT_90TIME						90000			

//자신의 이벤트 60초 , 다른사람의 이벤트 90초.

#define PREGIDENT_POINT					14				// 대통령점수.
#define THREEBBUCK_POINT				14

#define ORIGINAL_AVATAR_SIZE            0
#define SMALL_AVATAR_SIZE               1

#define HWATU_WIDTH                     45
#define HWATU_HEIGHT                    69
#define HWATU_PUT_DISTANCE              20              // 화두를 놓는 간격

// [zoom]
#define HWATU_IMG_WIDTH                 180             // 화투 전체 이미지의 가로 크기( HWATU_WIDTH * 4 )


#define MAX_TEXT_LINE                   100             // 채팅창에 저장되는 최대 텍스트 라인 수
#define MAX_VIEW_WAIT_CHAT              5               // 화면에 보여지는 대기실 채팅창 텍스트 라인 수
#define MAX_VIEW_WAIT_USER              12              // 한번에 보이는 최대 대기자 수
#define MAX_VIEW_WAIT_ROOM              3               // 한 화면에 보이는 대기실 수
#define MAX_VIEW_GAME_CHAT              9               // 게임화면 채팅창에 보여지는 텍스트 라인의 갯수
#define MAX_VIEW_CHODAE_USER            10              // 초대창에서 보여지는 최대 유저 수

// 창 기준 위치

#define DEF_DLG_POS_X                   267             // 대화상자의 디폴트 좌표
#define DEF_DLG_POS_Y                   259             // 대화상자의 디폴트 좌표
#define SHAKE_DLG_POS_X                 267             // "흔드시겠습니까?", "흔들었습니다." 대화상자 출력 좌표
#define SHAKE_DLG_POS_Y                 259             // "흔드시겠습니까?", "흔들었습니다." 대화상자 출력 좌표
#define SUN_DLG_POS_X                   267             // 선 결정 후 나오는 창 좌표
#define SUN_DLG_POS_Y                   287             // 선 결정 후 나오는 창 좌표
#define RESULT_POS_X                    362             // 결과창 X 좌표
#define RESULT_POS_Y                    205             // 결과창 Y 좌표
#define DEF_WAIT_DLG_POS_X              267             // 대기용 대화상자의 디폴트 좌표
#define DEF_WAIT_DLG_POS_Y              304             // 대기용 대화상자의 디폴트 좌표


#define CONFIG_POS_X                    349             // 옵션창의 위치
#define CONFIG_POS_Y                    250             // 옵션창의 위치
#define MAKEROOM_POS_X                  349             // 방만들기 창 좌표
#define MAKEROOM_POS_Y                  226             // 방만들기 창 좌표
#define PASS_DLG_POS_X                  349             // 패스워드 입력용 대화상자 좌표
#define PASS_DLG_POS_Y                  297             // 패스워드 입력용 대화상자 좌표
#define CHODAE_POS_X                    349             // 초대하기 대화상자 좌표
#define CHODAE_POS_Y                    174             // 초대하기 대화상자 좌표
#define CHODAE_RECV_POS_X               407             // 초대 받았을 때의 대화상자 좌표
#define CHODAE_RECV_POS_Y               299             // 초대 받았을 때의 대화상자 좌표
#define ALARM_MSG_POS_X                 349             // 알림 메시지 상자 출력 좌표
#define ALARM_MSG_POS_Y                 269             // 알림 메시지 상자 출력 좌표
#define MEMO_DLG_POS_X                  349             // 쪽지(메모) 대화상자 출력 좌표
#define MEMO_DLG_POS_Y                  250             // 쪽지(메모) 대화상자 출력 좌표

#define INIT_MISSION_DLG_POS_X          SCREEN_WIDTH - 10   // 미션창의 이동 전 초기 좌표
#define INIT_MISSION_DLG_POS_Y          221                 // 미션창의 이동 전 초기 좌표

#define MISSION_DLG_POS_X               785                 // 미션창의 이동 후 좌표
#define MISSION_DLG_POS_Y               221                 // 미션창의 이동 후 좌표
#define MISSION_DLG_MOVE_DISTANCE       ( INIT_MISSION_DLG_POS_X - MISSION_DLG_POS_X )  // 미션창의 이동 거리

#define ROOM_TITLE_LIST_X               MAKEROOM_POS_X + 94     // 방만들기 창 - 방 타이틀 리스트 출력 좌표( X )
#define ROOM_TITLE_LIST_Y               MAKEROOM_POS_Y + 79     // 방만들기 창 - 방 타이틀 리스트 출력 좌표( Y )
#define ROOM_TITLE_LIST_WIDTH           200             // 방만들기 창 - 방 타이틀 리스트 가로 크기
#define ROOM_TITLE_LIST_HEIGHT          81              // 방만들기 창 - 방 타이틀 리스트 세로 크기
#define ROOM_TITLE_BAR_HEIGHT           14              // 방만들기 창 - 방 제목 리스트 바의 세로 크기

// 출력할 숫자의 기호 타입
#define MARK_NONE                       0               // 기호 출력 안함
#define MARK_PLUS                       1               // + 기호 출력
#define MARK_MINUS                      2               // - 기호 출력
#define MARK_MULTIPLY                   3               // * 기호 출력

// 반투명 대화상자의 투명도 값(0~255)
#define TRANS_DLG_ALPHA                 110


#define WM_OUICKFAILCREATEROOM		(WM_USER + 0x16)	//퀵조인실패시 방을 바로 만들어 준다.


#define WM_AVATARPLAY			(WM_USER + 0x17)
#define WM_AVATARDEL			(WM_USER + 0x18)
#define WM_GAMEMODESET			(WM_USER + 0x1A)	//게임모드전환
#define WM_WAITMODESET			(WM_USER + 0x1B)	//대기모드전환.


//다이얼로그 타입.
#define DLGTYPE_GOSTOP		0x00        // GO?, STOP?
#define DLGTYPE_PREGIDENT	0x01        // 대통령
#define DLGTYPE_SHAKEQ		0x02        // 흔드시겠습니까?
#define DLGTYPE_SHAKE		0x03        // 흔들었습니다.
#define DLGTYPE_GOSTOPINFO	0x04
#define DLGTYPE_SSANGPICHO	0x06        // 쌍피로 사용?
#define DLGTYPE_TWOCARDCHO	0x07        // 두개중 어떤거 선택?
#define DLGTYPE_WAITTWOCARDCHO 0x08		// 타유저 카드선택 또는 쌍피 선택.


enum MISSION { KKWANG, JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC,
				HONGDAN, CHONGDAN, CHODAN, GODORI, OKWANG, DAEBAK };


#define MAX_DESTROY_TIME		10000
#define VIEWMINUSCNANNEL		5		//채널을 6개만 보여준다....이수를 빼준다..


extern CDXSoundBuffer *g_Sound[MAX_SOUND_CNT];


#define IDE_WAITEDIT	1011
#define IDE_GAMEEDIT	1012
#define IDE_MSGEDIT		1013
#define IDE_ROOMCOMBO   1014


#define GOSTOP_ENDPOINT		7

#define SYSTEMMONEY_RATE	5	//5%

#define PIRZECNT		12	// 총그림수 -1


#define MONEY_OUTPUT_TYPE_FULL		1
#define MONEY_OUTPUT_TYPE_SHORT		2


// 채팅창의 텍스트 한 줄을 저장하는 구조체
typedef struct tagSTextInfo
{
    COLORREF    rgbColor;                       // 텍스트의 색깔( RGB(red, green, blue) )
    int         nStyle;                         // 텍스트 스타일(0=보통, 1=굵게)
    char        szText[256];                    // 텍스트
} STextInfo;



// [alpha]
// 알파 블렌딩을 표현하기 위해 사용하는 구조체
//
// 1. 반투명 대화상자와 관련된 이전의 불린값들을 가지고 있다.
// 2. 이 정보는 Render()에서 Flip()하기 바로 전에 새로운 값들로 갱신된다.

typedef struct tagSPrevDlgInfo
{
    BOOL bPrevShowDialog;
    BOOL bPrevRenderSun;
    int nPrevDlgType;
} sPrevDlgInfo;

enum TRANS_DLG_TYPE
{
    TDT_NONE = 0,       // 아무것도 아니다.
    TDT_SEL_SUN,        // 선을 선택하는 반투명 대화상자
    TDT_GOSTOP,         // 고? 스톱?
    TDT_PREGIDENT,      // 총통
    TDT_SHAKEQ,         // 흔드시겠습니까?
    TDT_SHAKE,          // 흔들었습니다.
    TDT_SSANGPICHO,     // 국진을 쌍피로 쓰시겠습니까?
    TDT_TWOCARDCHO      // 두 카드중 하나 선택
};



// 카드 이미지 확대해서 출력시 필요한 참조 테이블

typedef struct tagSCardZoomTable
{
    int nZoomWidth;                 // 확대된 가로 크기
    int nZoomHeight;                // 확대된 세로 크기
    int *nRepHTable;                // 확대시 화투 이미지의 각 점당 같은 점을 몇개씩 그려주어야 하는지를 저장하는 테이블
    int *nRepVTable;                // 확대시 화투 이미지의 각 줄당 같은 줄을 몇줄씩 그려주어야 하는지를 저장하는 테이블
} sCardZoomTable;



// 미션 카드 정보( 미션 카드 출력에 사용 )

typedef struct tagSMissionCardInfo
{
    int  nCardCnt;                  // 이번 판 미션의 카드 숫자(카드가 없는 미션이면 값이 0이 된다.)
    int  nUserNo[5];                // 카드를 먹은 유저 번호
    BYTE nCardNo[5];                // 미션 카드 번호(각 카드가 배열에 저장된다.)
    BOOL bEat[5];                   // 유저가 미션 카드를 먹었는지를 표시
} sMissionCardInfo;



// 미션 창 정보

typedef struct tagSMissionDlgInfo
{
    int  nDlgX;                     // 미션창 좌표
    int  nDlgY;                     // 미션창 좌표
    int  nAniCnt;                   // 현재 얼만큼 이동했는지 결정
    int  nMaxAniCnt;                // 몇 번만에 이동할 것인지 결정
    int  nTimeCnt;                  // 한번 이동시 몇 프레임만에 이동할지 시간을 계산하는 카운터
    bool bAni;                      // 미션창의 이동여부 결정
} sMissionDlgInfo;


#endif