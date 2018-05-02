#ifndef _CLIENTDEFINE_H_
#define _CLIENTDEFINE_H_


#include "..\BadukiCommonDefine\BadukiCommonDefine.h"

//
//#define SHOWUSERCARD                    // ���� ������ ī�带 ���������� ����
//#define CHANGEUSERCARD                  // ������ ī�带 �ٲ� �� �ִ����� ���θ� ����
//

#define SCREEN_WIDTH        1024        // ȭ���� ���� ����
#define SCREEN_HEIGHT       768         // ȭ���� ���� ����

#define CHANNEL_MODE        0
#define WAITROOM_MODE       1
#define GAME_MODE           2
#define LOGO_MODE           3

#define MAX_SOUND_CNT       200
#define MAX_CHAT_COUNT      25
#define MAX_DESTROY_TIME    10000
#define MAX_WAITUSERCNT     12          //���ǿ� ������ �����ִ� ����

#define SC_PKTLEN_POS       2
#define CCPAKETLEN          2
#define DEF_TIME            600			//ä�� �޼��� ������ Ÿ��.
#define VIEWMINUSCNANNEL    5		    //ä���� 6���� �����ش�....�̼��� ���ش�..
#define NGCC_GAMECHAT       0x60

#define DEF_TIME            600
#define MAX_BTNTIMECOUNT    100

#define MAX_TEXT_LINE       100         // ä��â�� ����Ǵ� �ִ� �ؽ�Ʈ ���� ��
#define MAX_VIEW_WAIT_CHAT  5           // ȭ�鿡 �������� ���� ä��â �ؽ�Ʈ ���� ��
#define MAX_VIEW_GAME_CHAT  10          // ����ȭ�� ä��â�� �������� �ؽ�Ʈ ������ ����

#define MAX_WAIT_USER       200         // ���� �ִ� ���� ��
#define MAX_VIEW_WAIT_USER  12          // �ѹ��� ���̴� �ִ� ����� ��

#define MAX_VIEW_WAIT_ROOM  9           // �� ȭ�鿡 ���̴� ���� ��

#define MAX_CHIP_ANI        200         // �ѹ��� ���ϸ��̼��� ������ ���� ��
#define REMOVE_CHIP_TIME    20          // ���� ���������鼭 ������� �ð�(FRAME)

#define MAX_VIEW_CHODAE_USER 10         // �ʴ�â���� �������� �ִ� ���� ��



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
#define GS_QUARTER			12
#define GS_ROOMEXIT			25
#define GS_ROOMIN			26
#define GS_POPUP			27		//Popup.wav		
#define GS_WINNER			28
#define GS_LOSE				29
#define GS_CARDSHRINK		30
#define GS_COUNT			31
#define GS_MYTURN           36      // myturn.wav - �� ���ʰ� �Ǿ����� �˸��� �Ҹ�
#define GS_JACKPOT			11

#define GS_WAITBGM			180		//waitroom.wav
#define GS_LOGO				181


///TIME

#define ONSTARTBTN_TIME		500		//��ŸƮ ��ư Ÿ��.
#define CHOICEOPEN_TIME		25		//ī�带 �� �������ְ� ����Ŀ� ���̽� ȭ���� ����.
#define CHOICECARD_TIME		500		//ī�� ���̽� Ÿ��.
#define ENDDING_TIME		250		//���â ���ִ� �ð�.
#define CALL_TIME			500		//������ ���� �θ��� �ִ� Ÿ��. Ÿ���� ������ �ڵ� ���̸� ���ع�����.
#define CHGCARD_TIME        500     //ī�带 �ٲ� �� �ִ� �ð�. �ð��� ������ �ڵ� "�н�" ó��

#define WM_OUICKFAILCREATEROOM	(WM_USER + 0x16)	//�����ν��н� ���� �ٷ� ����� �ش�.
#define WM_GAMEMODESET			(WM_USER + 0x17)	//���Ӹ����ȯ
#define WM_CHANNELMODESET		(WM_USER + 0x18)	//ä�θ����ȯ
#define WM_WAITMODESET			(WM_USER + 0x19)	//�������ȯ.
#define WM_AVATARPLAY			(WM_USER + 0x20)	

#define IDE_WAITEDIT	        1011
#define IDE_GAMEEDIT	        1012
#define IDE_MSGEDIT		        1013
#define IDE_ROOMCOMBO           1014

#define INIT_CARD_POS_X			534     // ī�� �����ִ� ���� ��ġ(X) - ���� : 476
#define INIT_CARD_POS_Y			42      // ī�� �����ִ� ���� ��ġ(Y) - ���� : 62

#define CARD_WIDTH              82      // ī�� ���� ũ��
#define CARD_HEIGHT             109     // ī�� ���� ũ��
#define CARD_DISTANCE_A         38      // ī�� ���� �� ī���� ����
#define CARD_DISTANCE_B         19      // ī�� ���� �� ī���� ����

#define ROOM_TITLE_LIST_X       443     // �游��� â - �� Ÿ��Ʋ ����Ʈ ��� ��ǥ( X )
#define ROOM_TITLE_LIST_Y       287     // �游��� â - �� Ÿ��Ʋ ����Ʈ ��� ��ǥ( Y )
#define ROOM_TITLE_LIST_WIDTH   200     // �游��� â - �� Ÿ��Ʋ ����Ʈ ���� ũ��
#define ROOM_TITLE_LIST_HEIGHT  81      // �游��� â - �� Ÿ��Ʋ ����Ʈ ���� ũ��
#define ROOM_TITLE_BAR_HEIGHT   14      // �游��� â - �� ���� ����Ʈ ���� ���� ũ��


#define CONFIG_DLG_POS_X        349     // �ɼ�â�� ��ġ
#define CONFIG_DLG_POS_Y        250     // �ɼ�â�� ��ġ
#define ALARM_MSG_POS_X         349     // �˸� �޽��� ���� ��� ��ǥ
#define ALARM_MSG_POS_Y         269     // �˸� �޽��� ���� ��� ��ǥ
#define PASS_DLG_POS_X          349     // �н����� �Է¿� ��ȭ���� ��ǥ
#define PASS_DLG_POS_Y          297     // �н����� �Է¿� ��ȭ���� ��ǥ
#define MEMO_DLG_POS_X          349     // ����(�޸�) ��ȭ���� ��� ��ǥ
#define MEMO_DLG_POS_Y          250     // ����(�޸�) ��ȭ���� ��� ��ǥ
#define CHODAE_POS_X            349     // �ʴ��ϱ� ��ȭ���� ��ǥ
#define CHODAE_POS_Y            174     // �ʴ��ϱ� ��ȭ���� ��ǥ
#define CHODAE_RECV_POS_X       407     // �ʴ� �޾��� ���� ��ȭ���� ��ǥ
#define CHODAE_RECV_POS_Y       299     // �ʴ� �޾��� ���� ��ȭ���� ��ǥ

//�ƹ�Ÿ ũ��.
#define SMALL_SIZE		        0
#define ORIGINAL_SIZE	        1

// �游��� - �� �ɼ�
#define CO_NORMAL_MODE          0
#define CO_CHOICE_MODE          1

// ī�� ��ȯ Ƚ��
#define CHGCNT_MORNING          4       // ��ħ�� ��ȯ�� �� �ִ� ī�� ��
#define CHGCNT_AFTERNOON        4       // ���ɿ� ��ȯ�� �� �ִ� ī�� ��
#define CHGCNT_EVENING          4       // ���ῡ ��ȯ�� �� �ִ� ī�� ��

//������ ȯ�� ����.
typedef struct tagClientEnviro
{
	BOOL bOption[4];
	ush_int nVoice;	// 0 : �ϼҿ� ��Ҹ�, 1 : ���� ����, 2 : ���� ����
} sClientEnviro;



//�游��� �Ҷ� �ݾ��� ��� ��ġ.
typedef struct tagRoomInUserPosition
{
	POINT pAvatarPos;			//�ƹ�Ÿ ��ġ.
	POINT pCardStartPos;		//����ī�� ��ġ
	POINT pCardInfo;			//����ī�� ����â ��ġ
	POINT pNickPos;				//���� �г� ��ġ
	POINT pBossPos;				//���� ��ġ
	POINT pBangJangPos;			//������ġ
	POINT pUserMoneyPos;		//���� �Ӵ� ��ġ
	POINT pJokboPos;			//���� ��� ��ġ
	POINT pUserSelectBigPos;	//���� ��, ���� �� ū������ ��ġ
	POINT pUserSelectSmallPos;	//���� ��, ���� �� ���������� ��ġ
	POINT pWinnerPos;			//�¸��� Winner ��� ��ġ
	POINT pTimePos;				//��, ���̽� �ð���� ��ġ
	POINT pAllInPos;			//���� ��ũ ǥ��.
    POINT pChipStartPos;        //�� ������ ���� ��ġ ham 2005.09.26
    POINT pGoodsDojangPos;      //��ǰ ��÷ ���� �̹��� ��ġ
} sRoomInUserPosition;

// ��, �̵�, ����, ��ȯī��
enum CARDTYPE { PACK, FLOOR, USERCARD, CHGCARD };

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



// ���콺�� �巡�� ���� ���� ī�� ���� - ham 2005.09.29
typedef struct tagMovingHiddenCardInfo : public tagUserCardInfo
{
    int nOffsetY;               // ī�� ���� ���콺�� ������ ��, ī�� �� ���� ���� ���콺 ��ġ������ ����� �Ÿ�
} sMovingHiddenCardInfo;



// �� ���ϸ��̼ǿ� �ʿ��� ����ü

enum CHIPTYPE
{
    CT_NONE = 0,    // ���� ����
    CT_MOVE,        // �̵����� ��
    CT_REMOVE       // ������� �ִ� ��
};

typedef struct tagChipInfo
{
	POINT NewPos;
	POINT StartPos;
	POINT Pos;
	int nAniCount;
	int nAniMaxCount;
    int nRemoveCnt;         // ���� ����� �� ����ϴ� ī����
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
	int  Y;  // üũ�� Y ��ǥ
	BOOL bCheck; // üũ�� ǥ�õǸ� TRUE , �׷��� ������ FALSE
	
}sINVITECHECK;



// ä��â�� �ؽ�Ʈ �� ���� �����ϴ� ����ü
typedef struct tagSTextInfo
{
    COLORREF    rgbColor;                       // �ؽ�Ʈ�� ����( RGB(red, green, blue) )
    int         nStyle;                         // �ؽ�Ʈ ��Ÿ��(0=����, 1=����)
    char        szText[256];                    // �ؽ�Ʈ
} STextInfo;



// ��ħ, ����, ���� �� ����������, ī�� �ٲٴ� ������ ����

// nTime
#define TTIME_MORNING   1           // ��ħ
#define TTIME_AFTERNOON 2           // ����
#define TTIME_EVENING   3           // ����

// nMode
#define TMODE_BET       1           // ���� ��
#define TMODE_CHGCARD   2           // ī�� �ٲٴ� ��

typedef struct tagTimeMode {
    int nTime;                      // 0=�ƹ��͵� �ƴ�, 1=��ħ, 2=����, 3=����
    int nMode;                      // 0=�ƹ��͵� �ƴ�, 1=������, 2=ī�� �ٲٴ� ��
} sTimeMode;


#define CHGCARD_UP      10          // ���õ� ī�尡 �ö���� �Ÿ�(ī�带 �����ϸ� ���� �ణ �ö�)

// ���� �ٲ� ī���� ���� ����
typedef struct tagChgCardInfo {
    sUserCardChange l_sUserCardChange;  //
    int             nStatus;          // ī�� �ٲٱ� ����(0=�ٲٱ� �Ϸ� �ƴ�, 1=�ٲٱ� �Ϸ�)
} sChgCardInfo;



// ���� ������ ���� ������ �ٲٱ� ���ؼ� ����ϴ� ����ü
// ī�� �̵��߿� ���� ��Ŷ�� ������ �ٷ� ó���� �� �����Ƿ�
// ����ȭ�� ���� �̸� ���⿡ �����ߴٰ� ���߿� �����ϱ� ����
// ����Ѵ�.

typedef struct tagNextUserInfo {
    sSetGameCurUser l_sNextUser;    // ���� ������ ����
    int nSlotNo;                    // ���� ������ ���� ��ȣ
    BOOL bChange;                   // TRUE�� �ٲپ�� �Ѵٴ� ��.
} sNextUserInfo;


// � ������ ī�带 �ٲپ�� ���� ���� ���������� ���޵Ǹ�
// �ӽ÷� ���⿡ �����ߴٰ� ����ȭ�ϱ� ���ؼ� ���.

typedef struct tagChgCardUserInfo {
    int nSlotNo;                    // ī�带 �ٲ� ������ ���� ��ȣ
    BOOL bChange;                   // ī�� �ٲ� ���� ���� ���������� ���޵Ǹ� TRUE
} sChgCardUserInfo;


// ������ ó�� ������ �� �ʱ� ī�尡 �� ���������� ����
// ���� ������ ���۵ǵ��� ����ȭ ��Ű�� ���ؼ� �� ����ü�� ����Ѵ�.

typedef struct tagStartUserSync {
    sGameCurUser l_sCurUser;        // ������ ó�� ������ ������ ��ư ����
    int nSlotNo;                    // ������ ó�� ������ ������ ���� ��ȣ
    BOOL bInsertInitCard;           // ���ϸ��̼� ���ÿ� �ʱ� ī�带 �� �־������� ǥ���Ѵ�.
} sStartUserSync;


#endif


