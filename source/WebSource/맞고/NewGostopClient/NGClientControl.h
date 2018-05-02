#ifndef _NGCLIENTCONTROL_H
#define _NGCLIENTCONTROL_H 

#include "ClientDefine.h"



typedef struct tagRoomInfoClient	//클라이언트에게 보내주는 방정보
{
	sRoomInfo l_sRoomInfo;
	sUserInfo l_sUserInfo[MAX_ROOM_IN];

} sRoomInfoClient;



//방장이 컴이랑 치고 있을때 유저가 들어오면 여기에 넣어둔다.
typedef struct tagTempUserSave
{
	bool        bEmpty;
	int         nUserListNo;
	char        szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];
} sTempUserSave;



typedef struct tagImageData
{
	int         nFile;			         //이미지 파일명
	char        szCurImageFileName[255];	//이미지 실제 파일명
	int         nLastDec;	                //이미지 최종단계
	int         nCurDec;		            //이미지 현재단계
	int         nUserImageNo;               //유저 이미지 번호( 현재 자신의 아바타 몇번째 )  무료일때는 0
	char        szUserImage[MAX_IMAGE_LEN]; // 유저가 가지고 있는 그림( 7 바이트 )
	BOOL        bFreeGuest;                 // 무료 회원이면 TRUE 유료회원이면 FALSE
	CDXSprite  *l_SPR_Avatar;
	BOOL l_bPrint;
} sIMAGEDATA;



//유저의 환경 변수.
typedef struct tagClientEnviro
{
	//nOption[0] bInvite;           // 초대     TRUE 허용 
	//nOption[1] bSecretMessage;    // 귓속말   TRUE 허용
	//nOption[2] bWaveSnd;          // 효과음	TRUE 허용
	//nOption[3] bMessageAgree;     // 메세지   TRUE 허용		

	BOOL bOption[4];
	ush_int nVoice;	// 0 : 하소연 목소리, 1 : 남자 성우, 2 : 여자 성우
} sClientEnviro;



typedef struct tagInviteCheck
{
	int  Y;                         // 체크의 Y 좌표
	BOOL bCheck;                    // 체크가 표시되면 TRUE , 그렇지 않으면 FALSE
	
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



