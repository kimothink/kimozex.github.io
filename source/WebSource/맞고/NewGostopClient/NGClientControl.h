#ifndef _NGCLIENTCONTROL_H
#define _NGCLIENTCONTROL_H 

#include "ClientDefine.h"



typedef struct tagRoomInfoClient	//Ŭ���̾�Ʈ���� �����ִ� ������
{
	sRoomInfo l_sRoomInfo;
	sUserInfo l_sUserInfo[MAX_ROOM_IN];

} sRoomInfoClient;



//������ ���̶� ġ�� ������ ������ ������ ���⿡ �־�д�.
typedef struct tagTempUserSave
{
	bool        bEmpty;
	int         nUserListNo;
	char        szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];
} sTempUserSave;



typedef struct tagImageData
{
	int         nFile;			         //�̹��� ���ϸ�
	char        szCurImageFileName[255];	//�̹��� ���� ���ϸ�
	int         nLastDec;	                //�̹��� �����ܰ�
	int         nCurDec;		            //�̹��� ����ܰ�
	int         nUserImageNo;               //���� �̹��� ��ȣ( ���� �ڽ��� �ƹ�Ÿ ���° )  �����϶��� 0
	char        szUserImage[MAX_IMAGE_LEN]; // ������ ������ �ִ� �׸�( 7 ����Ʈ )
	BOOL        bFreeGuest;                 // ���� ȸ���̸� TRUE ����ȸ���̸� FALSE
	CDXSprite  *l_SPR_Avatar;
	BOOL l_bPrint;
} sIMAGEDATA;



//������ ȯ�� ����.
typedef struct tagClientEnviro
{
	//nOption[0] bInvite;           // �ʴ�     TRUE ��� 
	//nOption[1] bSecretMessage;    // �ӼӸ�   TRUE ���
	//nOption[2] bWaveSnd;          // ȿ����	TRUE ���
	//nOption[3] bMessageAgree;     // �޼���   TRUE ���		

	BOOL bOption[4];
	ush_int nVoice;	// 0 : �ϼҿ� ��Ҹ�, 1 : ���� ����, 2 : ���� ����
} sClientEnviro;



typedef struct tagInviteCheck
{
	int  Y;                         // üũ�� Y ��ǥ
	BOOL bCheck;                    // üũ�� ǥ�õǸ� TRUE , �׷��� ������ FALSE
	
}sINVITECHECK;



typedef struct tagAvatarPlayStr
{
	int  nSlot;
	char szAvaUrl[MAX_AVAURLLEN];
	char szGameGubun[5];
	char cSex;

} sAvatarPlayStr;

	

#define USER_COUNT_PER_CHANNEL	100
#define CCPAKETLEN			2
#define MAX_RECVCARD_CNT	60


#define NGCC_GAMECHAT		0x60
#define NGCC_STARTGAME		0x61
#define NGCC_SENDUSERCARD	0x62
#define NGCC_RESULTOK		0x63
#define NGCC_CARDSHAKE		0x64
#define NGCC_GOSTOP			0x65
#define NGCC_SENDNINECARD	0x66
#define NGCC_USEREXITRESERV 0x67
#define NGCC_MISSIONSET		0x6a
#define NGCC_CARDCHANGE		0x6b

extern char g_szHeader[2];
extern HWND g_hWnd;


DWORD WINAPI ThreadProc(LPVOID lpParameter);
DWORD WINAPI UserCardRecvThreadProc(LPVOID lpParameter);

void GetErrorMessage(BYTE nErrorCode);

#endif



