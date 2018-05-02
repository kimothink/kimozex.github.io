#ifndef __GLOVALVAR_H__
#define __GLOVALVAR_H__

#include "NGClientControl.h"
#include "ClientDefine.h"
#include "MainFrm.h"
#include "NetworkSock.h"
#include "UserList.h"


//���� �ǵ� ( ���ִϸ��̼� �ϱ����ؼ� )
BigInt g_biBeforePanMoney;
//���� �ǵ�.
BigInt g_biPanMoney = 0;
//���� ����.
BigInt g_biPureWinMoney = 0;
//���� �ǵ��� �ӽ÷� ������ �ִ´�. �� �����̵��� ������ ���� �������ֱ�����.
BigInt g_biTotTempMoney = 0;
//���� �ݸӴ�
BigInt g_biCallMoney = 0;
//���� ��Ż���� �Ӵ�
BigInt g_biTotalBettingMoney = 0;
//���� ���� �Ӵ�
BigInt g_biUserJackPotMoney = 0;
//�ش� ��Ӵ� ���� �Ӵ�
BigInt g_biJackPotMoney = 0;


int g_nWinnerSlotNo = 0;
int g_nCurMasterSlotNo = 0;
int g_LSPort = 0;
int g_nUserNo = 0;
int g_nCSPort = 0;
int g_nViewPageNo = 0;
int g_nGameSpeed = 10;              // ī�� �̵� �ӵ� (Frame ����)

bool g_ServerSend = true;           //������ ��Ŷ�� ������ �Ǵ���...true�� ������ �ȴ�.
bool g_bPaketThredOn = true;
bool g_bConnectingSpr = false;
bool g_bChoiceRoom = false;
bool g_bMyWin = false;	            //��ǽ�
bool g_bSoundCard = true;           // ���� ī�� ����̹��� ��ġ�Ǿ� �ִ��� �����Ѵ�.
bool g_bRealExit = false;           // �������� ���� �÷���

char g_szResultCmt[20];             // ���� ��� �ڸ�Ʈ
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
    "�����սô�.", 
    "������޲ٸ�", 
    "���丮��ƿ�",
    "�ų������Ǵ޷�",
    "�ųʰ����սô�."
};

char g_szProgPath[512];             // ���α׷� ���
int g_nWaitRoomIdx[MAX_ROOMCNT];    // ���ĵ� �� �ε���
int g_nWaitRoomSelIdx = -1;         // �� �ε������� ���õ� ��ȣ(-1�̸� ���õ� �� ����)
int g_nWaitRoomCnt = 0;             // ���� �� ����


HWND            g_hWnd;
CDXInput       *g_pCDXInput   = NULL;
CDXScreen      *g_pCDXScreen  = NULL;
CDXSound       *g_pCDXSound   = NULL;
CDXSoundBuffer *g_Sound[MAX_SOUND_CNT];
CMainFrame     *g_pCMainFrame = NULL;
CNetworkSock   *g_pClient     = NULL;

//���ǿ����� ������.
CUserList g_CUserList;

//���ӹ濡���� ������.
CGameUserList g_CGameUserList;

//�渮��Ʈ
CWaitRoomList g_CWaitRoomList;


//���ӹ濡�� ����� ����.
sGameWaitInfo g_sGameWaitInfo[MAX_GAMEVIEWWAIT];

sServrMashineList g_sServrMashineList[MASHINE_CNT];


//���ǿ��� ������ Ư������ Ŭ���������.���⿡ ��Ƴ��´�.
sUserInfo g_ClickRoomUserInfo[MAX_ROOM_IN];


//�ڽ��� ������
sRoomInfo g_sRoomInfo;



sClientEnviro g_sClientEnviro;


//�¸����� ī��
sWinnerCardInfo g_sWinnerCardInfo;


#endif