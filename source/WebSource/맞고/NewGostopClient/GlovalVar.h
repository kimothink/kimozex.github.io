#ifndef __GLOVALVAR_H__
#define __GLOVALVAR_H__


#include "UserList.h"
#include "NGClientControl.h"


#define IDI_ICON 1004


BYTE g_pGoStopValue[]	= 
{	H_20, 	H_5, 	H_1, 	H_1,			/*  1 */
	H_5, 	H_10, 	H_1, 	H_1,			/*  2 */
	H_20, 	H_5, 	H_1, 	H_1,			/*  3 */
	H_5, 	H_10, 	H_1, 	H_1,			/*  4 */
	H_5, 	H_10, 	H_1, 	H_1,			/*  5 */
	H_5, 	H_10, 	H_1, 	H_1,			/*  6 */
	H_5, 	H_10, 	H_1, 	H_1,			/*  7 */
	H_20, 	H_10, 	H_1, 	H_1,			/*  8 */
	H_5, 	H_10|H_2, 	H_1, 	H_1,		/*  9 */
	H_5, 	H_10, 	H_1, 	H_1,			/* 10 */
	H_20, 	H_2, 	H_1, 	H_1,			/* 11 */
	H_20, 	H_5, 	H_10, 	H_2,			/* 12 */
	H_2, 	H_2, 	H_3, 	0
};

HWND g_hWnd;		//View Handle
HWND g_hEdithWnd;


CDXInput       *g_pCDXInput = NULL;
CDXScreen      *g_pCDXScreen = NULL;
CDXSound       *g_pCDXSound = NULL;
CDXSoundBuffer *g_Sound[MAX_SOUND_CNT];

CGoStopEngine  *g_pGoStop;
CNetworkSock   *g_pClient = NULL;
CMainFrame     *g_pCMainFrame;


int g_i = 0;
int g_CurUser = -1;
int g_MyUser = -1;
int g_nCardNoArr[5];
int g_TwoPeeCnt[3];
BigInt g_biRoomMoney = 0;
BigInt g_biSystemMoney = 0;
int g_nGameCnt = 0;			        // ���� �濡�� ������ ���������� ������ �ƴ°�..
int g_nDoubleCardCnt = 1; // Edited by shkim 
int g_nThreeCardCnt = 1;
int g_nRoomNo = 0;
int g_CSPort;
int g_LSPort;
int g_SelectCardNo = -1;
int g_nGameSpeed = 10;
int g_nBombCardNo[3] = { -1, -1, -1 };
int g_nGoCnt = 0;
int g_nViewPageNo = 0;              // ���� ���� �ִ� ������ �ѹ�
int g_nCardPackNo = -1;
int g_nWaitRoomCnt = 0;             // ���� �� ����
int g_nRoomCurCnt = 0;
int g_nMakeRoomNo = -1;             // �� ����� ��ư�� ������ �� ���� ���� ���ȣ ����



char g_szTempStr[255];
char g_szChatTempStr[255];
char g_cServerDivi = '0';
char g_ClientPath[255] = { 0, };
char g_szMessageText[255];
char g_LSIp[MAX_IP_LEN];
char g_szUser_ID[MAX_ID_LENGTH];
char g_szCSServerIp[MAX_IP_LEN];
char g_szRoomName[MAX_ROOM_NAME] = { 0, };  //����ȭ��Ÿ��Ʋ�ٿ� �����ֱ����� ��Ƴ��´�.
char g_szClassName[20];



char g_szAvaUrl[MAX_AVAURLLEN];
char g_szGameGubun[5];
ush_int g_szGameJong;
ush_int g_szGameServerChannel;
ush_int g_nServiceDiv;
char g_szTempParam[256];

char g_szItem[7];
char g_szProgPath[512];             // ���α׷� ���

bool g_bUserCardRecv = false;	    //������ ī�带 �Ƴ�..
bool g_bPaketThredOn = true;
bool g_bUserCardRecvThreadOn = true;
bool g_bDeal = true;	            //true�� ī���и� �����ְ� �ִ°Ŵ�.
bool g_bConnectingSpr = false;
bool g_ServerSend = true;           //������ ��Ŷ�� ������ �Ǵ���...true�� ������ �ȴ�.
bool g_bBomb = false;
bool g_bSoundCard = true;           // ���� ī�� ����̹��� ��ġ�Ǿ� �ִ��� �����Ѵ�.
bool g_bRefreshTransDlg = true;     // ���� ��ȭ���ڸ� ���������� �����ϴ� ����
bool g_bRealExit = false;           // true�� ���α׷����� ���� ��û�� ������ �Ǵ�, �������� ������ ���

CGameWaitRoomList   g_CGameWaitRoomList;
sGameWaitInfo       g_sGameWaitInfo[MAX_GAMEVIEWWAIT];
CGameUserList       g_CGameUserList;

sServrMashineList   g_sServrMashineList[MASHINE_CNT];
sLogIn              g_sLogIn;
sClientEnviro       g_sClientEnviro;
sGamingExitUser     g_sGamingExitUser[MAX_ROOM_IN];
sUserInfo           g_ClickRoomUserInfo[MAX_ROOM_IN];         // ������ ���� ���� ����

sPaintAvatar        g_sPaintAvatar;
sAvatarPlayStr      g_sAvatarPlayStr;
sMissionCardInfo    g_sMissionCardInfo;


extern sIMAGEDATA   g_sImageData[MAX_ROOM_IN];//�� ������ �����ִ� �̹���
extern CUserList    g_CUserList;

#endif
