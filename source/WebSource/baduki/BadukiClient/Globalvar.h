#ifndef __GLOVALVAR_H__
#define __GLOVALVAR_H__

#include "NGClientControl.h"
#include "ClientDefine.h"
#include "MainFrm.h"
#include "NetworkSock.h"
#include "UserList.h"


//이전 판돈 ( 돈애니메이션 하기위해서 )
BigInt g_biBeforePanMoney;
//현재 판돈.
BigInt g_biPanMoney = 0;
//순수 딴돈.
BigInt g_biPureWinMoney = 0;
//현재 판돈을 임시로 가지고 있는다. 톨 슬라이딩이 끝나고 나서 갱신해주기위해.
BigInt g_biTotTempMoney = 0;
//유저 콜머니
BigInt g_biCallMoney = 0;
//유저 토탈베팅 머니
BigInt g_biTotalBettingMoney = 0;
//유저 잭팟 머니
BigInt g_biUserJackPotMoney = 0;
//해당 룸머니 잭팟 머니
BigInt g_biJackPotMoney = 0;


int g_nWinnerSlotNo = 0;
int g_nCurMasterSlotNo = 0;
int g_LSPort = 0;
int g_nUserNo = 0;
int g_nCSPort = 0;
int g_nViewPageNo = 0;
int g_nGameSpeed = 10;              // 카드 이동 속도 (Frame 단위)

bool g_ServerSend = true;           //서버로 패킷을 보내도 되는지...true면 보내도 된다.
bool g_bPaketThredOn = true;
bool g_bConnectingSpr = false;
bool g_bChoiceRoom = false;
bool g_bMyWin = false;	            //기권승
bool g_bSoundCard = true;           // 사운드 카드 드라이버가 설치되어 있는지 설정한다.
bool g_bRealExit = false;           // 강제종료 방지 플래그

char g_szResultCmt[20];             // 게임 결과 코멘트
char g_szHeader[2];
char g_LSIp[MAX_IP_LEN];
char g_cServerDivi = '0';
char g_szUser_ID[MAX_ID_LENGTH];
char g_szCSServerIp[MAX_IP_LEN];
char g_szAvaUrl[MAX_AVAURLLEN];
ush_int g_szGameJong;
ush_int g_szGameServerChannel;
ush_int g_nServiceDiv;
char g_szTempParam[256];
char g_szTempStr[255];
char g_szRoomName[MAX_ROOM_NAME];

char g_RoomTitle[5][20] =
{
    "게임합시다.", 
    "대박을꿈꾸며", 
    "도토리모아요",
    "신나게한판달려",
    "매너게임합시다."
};

char g_szProgPath[512];             // 프로그램 경로
int g_nWaitRoomIdx[MAX_ROOMCNT];    // 정렬된 방 인덱스
int g_nWaitRoomSelIdx = -1;         // 방 인덱스에서 선택된 번호(-1이면 선택된 방 없음)
int g_nWaitRoomCnt = 0;             // 대기실 방 갯수


HWND            g_hWnd;
CDXInput       *g_pCDXInput   = NULL;
CDXScreen      *g_pCDXScreen  = NULL;
CDXSound       *g_pCDXSound   = NULL;
CDXSoundBuffer *g_Sound[MAX_SOUND_CNT];
CMainFrame     *g_pCMainFrame = NULL;
CNetworkSock   *g_pClient     = NULL;

//대기실에서의 유저들.
CUserList g_CUserList;

//게임방에서의 유저들.
CGameUserList g_CGameUserList;

//방리스트
CWaitRoomList g_CWaitRoomList;


//게임방에서 대기자 보기.
sGameWaitInfo g_sGameWaitInfo[MAX_GAMEVIEWWAIT];

sServrMashineList g_sServrMashineList[MASHINE_CNT];


//대기실에서 유저가 특정방을 클릭했을경우.여기에 담아놓는다.
sUserInfo g_ClickRoomUserInfo[MAX_ROOM_IN];


//자신의 방정보
sRoomInfo g_sRoomInfo;



sClientEnviro g_sClientEnviro;


//승리자의 카드
sWinnerCardInfo g_sWinnerCardInfo;


#endif