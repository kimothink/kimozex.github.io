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
// #define SHOWUSERCARD                // ���� ������ �и� ���������� ���� ����
// #define CHANGEUSERCARD              // �ڽ��� ī�带 �ٲ� �� �ִ����� ���� ����
// #define ADMINJUSTVIEW               // ���� Only
//// 


#define SCREEN_WIDTH    1024        //ȭ�� ���� ũ��
#define SCREEN_HEIGHT   768         //ȭ�� ���� ũ��


#define		H_20		0x08		//��
#define		H_10		0x04		//�ʱ�
#define		H_5			0x02		//5��
#define		H_1			0x01		//��
#define		H_2			0x10		//����
#define		H_3			0x20		//������


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


//�ٴڿ� ī�� ����.
struct SFLOORCARD
{
	BYTE nCardNo;					//ī���� ��ȣ
	BYTE nPosNo;					//ī���� ��ġ
};

#define CHANNEL_MODE	0
#define WAITROOM_MODE	1
#define GAME_MODE		2
#define LOGO_MODE		3
#define GAMEVIEW_MODE	4

#define MAX_FLOOR_CARD					60				//�ٴڿ� ī�尡 �򸮴� �ִ��
#define	MAX_CARD_POS					17				//�ٴڿ� ī���� ��ġ��. 16������ �򸰴�.	17���� ���԰� ��¤���а� �����϶� ������ġ.
#define MAX_CARDPACK_CNT				60				//�ϳ��� ī������ �ִ� ��� �ɼ� �ֳ�.
#define MAX_USERCARD_CNT				12				//������ �ִ밡���� �ִ� �м�.
#define MASTER_POS						0				//������ ���� ��ġ...2
#define REAL_CARD_MAXNO					48				//48�������� ��¥ ī�� �ѹ�..�����Ĵ� ����ī��
#define MAX_SOUND_CNT					250
#define BOMB_CARDNO						51
#define BBUK_X							3				//ù��, ���� �������� �ǵ��� ���ϴ¼�, ù����,
#define TEMP_STR_LENGTH					500

#define WAIT_PAINT_TIME					60000			//60��
#define WAIT_THREAD_KILLTIME			5000

#define USER_WAIT_TIME					3				//3���ĺ��� ����ī���͸� ����.
#define WAIT_STARTTIME					60000			//60�� 
#define WAIT_1TIME						1000			//1��.
#define WAIT_5TIME						5000			//5��.
#define WAIT_60TIME						60000			//60�� ( �����, shake  )
#define WAIT_90TIME						90000			

//�ڽ��� �̺�Ʈ 60�� , �ٸ������ �̺�Ʈ 90��.

#define PREGIDENT_POINT					14				// ���������.
#define THREEBBUCK_POINT				14

#define ORIGINAL_AVATAR_SIZE            0
#define SMALL_AVATAR_SIZE               1

#define HWATU_WIDTH                     45
#define HWATU_HEIGHT                    69
#define HWATU_PUT_DISTANCE              20              // ȭ�θ� ���� ����

// [zoom]
#define HWATU_IMG_WIDTH                 180             // ȭ�� ��ü �̹����� ���� ũ��( HWATU_WIDTH * 4 )


#define MAX_TEXT_LINE                   100             // ä��â�� ����Ǵ� �ִ� �ؽ�Ʈ ���� ��
#define MAX_VIEW_WAIT_CHAT              5               // ȭ�鿡 �������� ���� ä��â �ؽ�Ʈ ���� ��
#define MAX_VIEW_WAIT_USER              12              // �ѹ��� ���̴� �ִ� ����� ��
#define MAX_VIEW_WAIT_ROOM              3               // �� ȭ�鿡 ���̴� ���� ��
#define MAX_VIEW_GAME_CHAT              9               // ����ȭ�� ä��â�� �������� �ؽ�Ʈ ������ ����
#define MAX_VIEW_CHODAE_USER            10              // �ʴ�â���� �������� �ִ� ���� ��

// â ���� ��ġ

#define DEF_DLG_POS_X                   267             // ��ȭ������ ����Ʈ ��ǥ
#define DEF_DLG_POS_Y                   259             // ��ȭ������ ����Ʈ ��ǥ
#define SHAKE_DLG_POS_X                 267             // "���ðڽ��ϱ�?", "�������ϴ�." ��ȭ���� ��� ��ǥ
#define SHAKE_DLG_POS_Y                 259             // "���ðڽ��ϱ�?", "�������ϴ�." ��ȭ���� ��� ��ǥ
#define SUN_DLG_POS_X                   267             // �� ���� �� ������ â ��ǥ
#define SUN_DLG_POS_Y                   287             // �� ���� �� ������ â ��ǥ
#define RESULT_POS_X                    362             // ���â X ��ǥ
#define RESULT_POS_Y                    205             // ���â Y ��ǥ
#define DEF_WAIT_DLG_POS_X              267             // ���� ��ȭ������ ����Ʈ ��ǥ
#define DEF_WAIT_DLG_POS_Y              304             // ���� ��ȭ������ ����Ʈ ��ǥ


#define CONFIG_POS_X                    349             // �ɼ�â�� ��ġ
#define CONFIG_POS_Y                    250             // �ɼ�â�� ��ġ
#define MAKEROOM_POS_X                  349             // �游��� â ��ǥ
#define MAKEROOM_POS_Y                  226             // �游��� â ��ǥ
#define PASS_DLG_POS_X                  349             // �н����� �Է¿� ��ȭ���� ��ǥ
#define PASS_DLG_POS_Y                  297             // �н����� �Է¿� ��ȭ���� ��ǥ
#define CHODAE_POS_X                    349             // �ʴ��ϱ� ��ȭ���� ��ǥ
#define CHODAE_POS_Y                    174             // �ʴ��ϱ� ��ȭ���� ��ǥ
#define CHODAE_RECV_POS_X               407             // �ʴ� �޾��� ���� ��ȭ���� ��ǥ
#define CHODAE_RECV_POS_Y               299             // �ʴ� �޾��� ���� ��ȭ���� ��ǥ
#define ALARM_MSG_POS_X                 349             // �˸� �޽��� ���� ��� ��ǥ
#define ALARM_MSG_POS_Y                 269             // �˸� �޽��� ���� ��� ��ǥ
#define MEMO_DLG_POS_X                  349             // ����(�޸�) ��ȭ���� ��� ��ǥ
#define MEMO_DLG_POS_Y                  250             // ����(�޸�) ��ȭ���� ��� ��ǥ

#define INIT_MISSION_DLG_POS_X          SCREEN_WIDTH - 10   // �̼�â�� �̵� �� �ʱ� ��ǥ
#define INIT_MISSION_DLG_POS_Y          221                 // �̼�â�� �̵� �� �ʱ� ��ǥ

#define MISSION_DLG_POS_X               785                 // �̼�â�� �̵� �� ��ǥ
#define MISSION_DLG_POS_Y               221                 // �̼�â�� �̵� �� ��ǥ
#define MISSION_DLG_MOVE_DISTANCE       ( INIT_MISSION_DLG_POS_X - MISSION_DLG_POS_X )  // �̼�â�� �̵� �Ÿ�

#define ROOM_TITLE_LIST_X               MAKEROOM_POS_X + 94     // �游��� â - �� Ÿ��Ʋ ����Ʈ ��� ��ǥ( X )
#define ROOM_TITLE_LIST_Y               MAKEROOM_POS_Y + 79     // �游��� â - �� Ÿ��Ʋ ����Ʈ ��� ��ǥ( Y )
#define ROOM_TITLE_LIST_WIDTH           200             // �游��� â - �� Ÿ��Ʋ ����Ʈ ���� ũ��
#define ROOM_TITLE_LIST_HEIGHT          81              // �游��� â - �� Ÿ��Ʋ ����Ʈ ���� ũ��
#define ROOM_TITLE_BAR_HEIGHT           14              // �游��� â - �� ���� ����Ʈ ���� ���� ũ��

// ����� ������ ��ȣ Ÿ��
#define MARK_NONE                       0               // ��ȣ ��� ����
#define MARK_PLUS                       1               // + ��ȣ ���
#define MARK_MINUS                      2               // - ��ȣ ���
#define MARK_MULTIPLY                   3               // * ��ȣ ���

// ������ ��ȭ������ ���� ��(0~255)
#define TRANS_DLG_ALPHA                 110


#define WM_OUICKFAILCREATEROOM		(WM_USER + 0x16)	//�����ν��н� ���� �ٷ� ����� �ش�.


#define WM_AVATARPLAY			(WM_USER + 0x17)
#define WM_AVATARDEL			(WM_USER + 0x18)
#define WM_GAMEMODESET			(WM_USER + 0x1A)	//���Ӹ����ȯ
#define WM_WAITMODESET			(WM_USER + 0x1B)	//�������ȯ.


//���̾�α� Ÿ��.
#define DLGTYPE_GOSTOP		0x00        // GO?, STOP?
#define DLGTYPE_PREGIDENT	0x01        // �����
#define DLGTYPE_SHAKEQ		0x02        // ���ðڽ��ϱ�?
#define DLGTYPE_SHAKE		0x03        // �������ϴ�.
#define DLGTYPE_GOSTOPINFO	0x04
#define DLGTYPE_SSANGPICHO	0x06        // ���Ƿ� ���?
#define DLGTYPE_TWOCARDCHO	0x07        // �ΰ��� ��� ����?
#define DLGTYPE_WAITTWOCARDCHO 0x08		// Ÿ���� ī�弱�� �Ǵ� ���� ����.


enum MISSION { KKWANG, JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC,
				HONGDAN, CHONGDAN, CHODAN, GODORI, OKWANG, DAEBAK };


#define MAX_DESTROY_TIME		10000
#define VIEWMINUSCNANNEL		5		//ä���� 6���� �����ش�....�̼��� ���ش�..


extern CDXSoundBuffer *g_Sound[MAX_SOUND_CNT];


#define IDE_WAITEDIT	1011
#define IDE_GAMEEDIT	1012
#define IDE_MSGEDIT		1013
#define IDE_ROOMCOMBO   1014


#define GOSTOP_ENDPOINT		7

#define SYSTEMMONEY_RATE	5	//5%

#define PIRZECNT		12	// �ѱ׸��� -1


#define MONEY_OUTPUT_TYPE_FULL		1
#define MONEY_OUTPUT_TYPE_SHORT		2


// ä��â�� �ؽ�Ʈ �� ���� �����ϴ� ����ü
typedef struct tagSTextInfo
{
    COLORREF    rgbColor;                       // �ؽ�Ʈ�� ����( RGB(red, green, blue) )
    int         nStyle;                         // �ؽ�Ʈ ��Ÿ��(0=����, 1=����)
    char        szText[256];                    // �ؽ�Ʈ
} STextInfo;



// [alpha]
// ���� ������ ǥ���ϱ� ���� ����ϴ� ����ü
//
// 1. ������ ��ȭ���ڿ� ���õ� ������ �Ҹ������� ������ �ִ�.
// 2. �� ������ Render()���� Flip()�ϱ� �ٷ� ���� ���ο� ����� ���ŵȴ�.

typedef struct tagSPrevDlgInfo
{
    BOOL bPrevShowDialog;
    BOOL bPrevRenderSun;
    int nPrevDlgType;
} sPrevDlgInfo;

enum TRANS_DLG_TYPE
{
    TDT_NONE = 0,       // �ƹ��͵� �ƴϴ�.
    TDT_SEL_SUN,        // ���� �����ϴ� ������ ��ȭ����
    TDT_GOSTOP,         // ��? ����?
    TDT_PREGIDENT,      // ����
    TDT_SHAKEQ,         // ���ðڽ��ϱ�?
    TDT_SHAKE,          // �������ϴ�.
    TDT_SSANGPICHO,     // ������ ���Ƿ� ���ðڽ��ϱ�?
    TDT_TWOCARDCHO      // �� ī���� �ϳ� ����
};



// ī�� �̹��� Ȯ���ؼ� ��½� �ʿ��� ���� ���̺�

typedef struct tagSCardZoomTable
{
    int nZoomWidth;                 // Ȯ��� ���� ũ��
    int nZoomHeight;                // Ȯ��� ���� ũ��
    int *nRepHTable;                // Ȯ��� ȭ�� �̹����� �� ���� ���� ���� ��� �׷��־�� �ϴ����� �����ϴ� ���̺�
    int *nRepVTable;                // Ȯ��� ȭ�� �̹����� �� �ٴ� ���� ���� ���پ� �׷��־�� �ϴ����� �����ϴ� ���̺�
} sCardZoomTable;



// �̼� ī�� ����( �̼� ī�� ��¿� ��� )

typedef struct tagSMissionCardInfo
{
    int  nCardCnt;                  // �̹� �� �̼��� ī�� ����(ī�尡 ���� �̼��̸� ���� 0�� �ȴ�.)
    int  nUserNo[5];                // ī�带 ���� ���� ��ȣ
    BYTE nCardNo[5];                // �̼� ī�� ��ȣ(�� ī�尡 �迭�� ����ȴ�.)
    BOOL bEat[5];                   // ������ �̼� ī�带 �Ծ������� ǥ��
} sMissionCardInfo;



// �̼� â ����

typedef struct tagSMissionDlgInfo
{
    int  nDlgX;                     // �̼�â ��ǥ
    int  nDlgY;                     // �̼�â ��ǥ
    int  nAniCnt;                   // ���� ��ŭ �̵��ߴ��� ����
    int  nMaxAniCnt;                // �� ������ �̵��� ������ ����
    int  nTimeCnt;                  // �ѹ� �̵��� �� �����Ӹ��� �̵����� �ð��� ����ϴ� ī����
    bool bAni;                      // �̼�â�� �̵����� ����
} sMissionDlgInfo;


#endif