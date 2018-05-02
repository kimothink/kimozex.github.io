#ifndef _EXTERNVAR_H_
#define _EXTERNVAR_H_

#include "MainFrm.h"
#include "NetworkSock.h"
#include "UserList.h"

extern BigInt g_biBeforePanMoney;
extern BigInt g_biPanMoney;
extern BigInt g_biPureWinMoney;
extern BigInt g_biCallMoney;
extern BigInt g_biTotalBettingMoney;
extern BigInt g_biUserJackPotMoney;
extern BigInt g_biJackPotMoney;
extern BigInt g_biTotTempMoney;

extern int g_nWinnerSlotNo;
extern int g_nCurMasterSlotNo;
extern int g_LSPort;
extern int g_nUserNo;
extern int g_nCSPort;
extern int g_nViewPageNo;
extern int g_nGameSpeed;

extern bool g_ServerSend;  //서버로 패킷을 보내도 되는지...true면 보내도 된다.
extern bool g_bPaketThredOn;
extern bool g_bConnectingSpr;
extern bool g_bChoiceRoom;
extern bool g_bMyWin;
extern bool g_bSoundCard;
extern bool g_bRealExit;

extern char g_szResultCmt[20];
extern char g_szHeader[2];
extern char g_LSIp[MAX_IP_LEN];
extern char g_cServerDivi;
extern char g_szUser_ID[MAX_ID_LENGTH];
extern char g_szCSServerIp[MAX_IP_LEN];
extern char g_szAvaUrl[MAX_AVAURLLEN];
extern char g_szTempStr[255];
extern char g_szRoomName[MAX_ROOM_NAME];
extern char g_szTempParam[256];
extern ush_int g_nServiceDiv;

extern int g_nWaitRoomIdx[MAX_ROOMCNT];
extern int g_nWaitRoomSelIdx;
extern int g_nWaitRoomCnt;

extern char g_RoomTitle[5][20];
extern char g_szProgPath[512];

extern HWND g_hWnd;
extern CDXInput *g_pCDXInput;
extern CDXScreen *g_pCDXScreen;
extern CDXSound *g_pCDXSound;
extern CMainFrame *g_pCMainFrame;
extern sClientEnviro g_sClientEnviro;
extern CNetworkSock *g_pClient;
extern CUserList g_CUserList;
extern CGameUserList g_CGameUserList;
extern sServrMashineList g_sServrMashineList[MASHINE_CNT];
extern CDXSoundBuffer *g_Sound[MAX_SOUND_CNT];
extern CWaitRoomList g_CWaitRoomList;
extern ush_int g_szGameJong;
extern ush_int g_szGameServerChannel;
extern sUserInfo g_ClickRoomUserInfo[MAX_ROOM_IN];
extern sRoomInfo g_sRoomInfo;
extern sWinnerCardInfo g_sWinnerCardInfo;
extern sGameWaitInfo g_sGameWaitInfo[MAX_GAMEVIEWWAIT];

#endif
