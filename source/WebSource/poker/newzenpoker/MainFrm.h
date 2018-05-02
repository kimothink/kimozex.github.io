// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__6FA7B5FA_1496_4A83_9E4D_8E012DB18D1D__INCLUDED_)
#define AFX_MAINFRM_H__6FA7B5FA_1496_4A83_9E4D_8E012DB18D1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "AlarmMsg.h"
#include "FrameSkip.h"
#include "Animation.h"
#include "ChipAni.h"
#include "CTextList.h"
#include "CScrollText.h"
#include <process.h>



#include "..\\..\\..\\cdxlib\\cdx.h"
#include "..\\..\\..\\cdxlib\\cdxbutton.h"
#include "..\\..\\..\\cdxlib\\cdxime.h"
#include "..\\..\\..\\cdxlib\\cdxinput.h"
#include "..\\..\\..\\cdxlib\\cdxscrollbar.h"
#include "..\\..\\..\\cdxlib\\cdxsound.h"
#include "..\\..\\..\\cdxlib\\cdxsprite.h"
#include "..\\..\\..\\cdxlib\\cdxtile.h"



#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Imm32.lib")



#define GTC GetTickCount

struct CDXBtnTime
{
	CDXButton *CDXBtn;
	int nTime;
};




class CONFIGBOX
{
public:

	//BOOL bInvite;         // �ʴ�     TRUE ��� 
	//BOOL bSecretMessage;  // �ӼӸ�   TRUE ���
	//BOOL bWaveSnd;        // ȿ����	TRUE ���
	//BOOL bMessageAgree;   // �޼���   TRUE ���
	BOOL bCheck[4];

	int nVoice;             //1 ���� . 2 ���� . 3 . ���� ( �ʱ� ���� )
	
public:
	
	CONFIGBOX( BOOL btInvite = TRUE , BOOL btSecretMessage = TRUE , BOOL btWaveSnd = TRUE , BOOL btMessageAgree = TRUE , int ntVoice = 0 ); // ������
	void RenderConfigBox( void );

	BOOL GetCheckState( int nNum );
	void ChangetCheck( int nNum , BOOL TempCheck );

	void ChangeVoice( int nNum );
	int  GetVoiceState( void );
};




class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
	int m_nCurrentMode;
	int m_mX, m_mY;                 // ���콺 ��ġ ����� ������
	int m_nChatTime;                // ä�� ���� �ð�
	int m_nDegree;                  // ������� ������ ���ΰ�.
	int m_nScrollY;                 // ��ũ�� �� ��ư�� Y ��ǥ
	int m_nScrollNo;                // ��ũ�� ��ȣ ������ 0 ��
	int m_nCurMashine;              // ���� ä��â���� ���° ������ ����Ű�°�.
	int m_nCurChannelNo;
	int m_nWaitUserCurCnt;          // ���� ���� �����

	int m_nSelectedRoomNum;         // ���õ� �� ��ȣ (���õ��� �ʾ����� -1 )
    int m_nViewUserIdx;             // ���õ� ������ �ε��� ��ȣ(�����, ������ ��ο� ���)
    int m_nViewUserNo;              // ���õ� ������ ���� ��ȣ(�����, ������ ��ο� ���)

	int m_nRButtonWaitUserNo;
	int m_TempMX;
	int m_TempMY;
	int m_nCurSorCnt;
	int m_StartBTNState;
	int m_StartAni;
	int m_StartT1, m_StartT2;
	int m_nStartBtnCnt;
	int m_nCardChoiceCnt;
	int m_nCardDumpCnt;
	int m_nWinnerMarkPos;           //�̰��� ���� �̹����� ������ ��ȣ
    int m_nWinnerMarkPosCnt;        //�̰��� ���� ���ϸ��̼� �ð����� ����� ���� ī����
	int m_nEndingUserTime;	        //�Ѹ�� ������� �ð�
	int m_nEndingShowTime;	        //���� ���â ���� �ð�.
	int m_nEndingUserCurTime;	    //���� ���� �������� ���� �ð�.
	int m_nEndingUserCurSlot;	    //���� ���� �������� ���� ����.
	int m_nUserCallTime;	        //���� ��,���� �� ���ýð�.
	int m_nUserCallSlotNo;	        //���� ��,���̸� �ؾ��� ����.
	int m_nTimePos;			        //���� ��,���� Ÿ�� �׸� ��ġ.
	int m_nJackPotPos;		        //���� �������.
	int m_nJackPotLen;		        //���� -> ���ڷ� ��ȭ�� ����.
	int m_nJackPotStopCnt;	        //���� ���� ��ŭ ���� ���� �ִϸ� �����.
	int m_nWaitViewPageNo;	        //�ʴ� ������ No
	int m_nInviteRoomNo;	        //�ʴ�� ���ȣ.
	int m_nInviteXPos;
	int m_nInviteYPos;
	int m_nInviteTempX;
	int m_nInviteTempY;
	int m_cX;
	int m_cY;
	int m_nUserCallDefineTime;	    //���� �� ������ Ÿ��.
	int m_nTimePosCnt;
	int m_nShowUserSlot;		    //���ӹ濡�� ������������.
	int m_nSelectionCnt;		    //�����̴� �ӵ�.
    int m_nUserListTitleNum;        // ���� ����Ʈ Ÿ��Ʋ �̹��� ��ȣ(0 = ����� ���, 1 = �� ���� ���)
    int m_nHighlightBarNum;         // ������ ���� ����Ʈ�� ĭ ��ȣ
    int m_nSelectedRoomTitleList;   // �游��� â - �� ����Ʈ �� ���õ� ���̸��� ��ȣ(������ -1)
    int m_nCardOption;              // �пɼ� - CO_NORMAL_MODE = �Ϲ� ���, CO_CHOICE_MODE = ���̽� ���
    int m_nJackpotBackFrm;          // ���� ��� �̹��� ������ ��ȣ
    int m_nJackpotBackFrmCnt;       // ���� ��� �̹��� ������ ��ȣ�� �ٲٱ� ���� �ð���� ī����
	int m_nTemp;

    unsigned m_dwAniUpdateThread;

	//����ڰ� ���Ҷ� ��ư���� ��ǥ�����ֱ����ؼ�.
	BigInt m_biCall;
	BigInt m_biHalf;
	BigInt m_biBing;
	BigInt m_biDdaDang;
	BigInt m_biCheck;
	BigInt m_biDie;	
	BigInt m_biFull;	
	BigInt m_biQuater;	
	BigInt m_biInviteRoomMoney;		//�ʴ�� ��Ӵ�.
    BigInt m_biMakeRoomMoneyNo;	    //�游���â���� �ǵ��� ���� ���°�� ����Ű�� �ִ°�.

	BOOL m_bShowMakeRoomDlg;
	BOOL m_bShowMessageBox;
	BOOL m_bMBoxMoveDlg;
	BOOL m_bMouseDown;
	BOOL m_bActive;
	BOOL m_bAniActive;
	BOOL m_bMoveDlg;
	BOOL m_bFullScreen;
	BOOL m_bRButtonDown;
	BOOL m_bShowResult;
	BOOL m_bShowDialog;
	BOOL m_bShowStartBTN;           //���� ��ư ���ִ� ��..
	BOOL m_bCardChoice;             //������ ī�� ���� ��..
	BOOL m_bCardDump;		        //4���� ���� ������ ��..
	BOOL m_bShowConfigBox;
	BOOL m_bUserState;	            //������ ������(�����ʸ��콺��ư�������������ٰ�����)
	BOOL m_bMessageSend;            // ������ �����°��� �޴� ���� üũ // TRUE = ������. FALSE = �޴´�.
	BOOL m_bMouseEvent;	            //MFC ��Ʈ�� ���� ���콺�� ���� ���콺�� ���� �ʰ� �Ϸ���.
	BOOL m_bMouseRender;            //MFC ��Ʈ�� ���� ���콺�� ���� ���콺�� �׸��� �ʰ� �Ϸ���.
	BOOL m_bStartInit;
	BOOL m_bEndding;	            //���� ��� ����
	BOOL m_bEnddingShow;            //���� ����� ����
    BOOL m_bGameInit;               //������ AniUpdateThread���� �ʱ�ȭ ��Ų��.
	BOOL m_bUserCallTime;	        //���� ��,���� �� ���� �ð�.
	BOOL m_bWinnerCpoy;	            //�¸����� ������ �ѹ��� ī���ϱ� ���ؼ�.
	BOOL m_bRenderPanMoney;	
	BOOL m_bJackPot;	            //������ ������ ��Ʈ���� ������ �׸�.
	BOOL m_bRoomJackPotRender;	    //������ �����ϸ� ������ ���ư���.
	BOOL m_bShowInviteBox;	        //������ �ʴ�â�� ����.
	BOOL m_bShowChodaeBox;	        //�ʴ�ڽ��� ����.
	BOOL m_bChodaeDlg;		        //�ڽ� ��� �������.
	BOOL m_bShowUserInfo;	        //���ӹ濡�� ���� ��������.
	BOOL m_bSelection;		        //���� �����̰� �ϴ°�.
	BOOL m_bSelectionStart;	        //���� �����̱� ����.
	BOOL m_bBeforeFull;		        //�� ������ Ǯ�̸� ���̽� ��ư ��� Ǯ������ ����
    BOOL m_bPassRoomChk;            // ��й� �н����� üũ     // [HAM+] 2005.08.19 [HAM-]
    BOOL m_bPassDlg;                // �н����� �Է�â�� ����.
    BOOL m_bShowRoomTitleList;      // �游��� â - �� Ÿ��Ʋ ����Ʈ�� ���������� ����
    BOOL m_bMoveHiddenCard;         // ���� ī�尡 ���콺 �巡�׷� �̵��������� ��Ÿ����.

	char m_szMessageSendUserNick[MAX_NICK_LENGTH];      // ���� ���� NickName
	char m_szMessageData[MAX_CHATMSG];                  // ���� �Ҹ� ����
	char m_szMessageRecieveUserNick[MAX_NICK_LENGTH];   // �޴� ���� NickName
	char m_szGetMessageData[MAX_CHATMSG];               // ���� �Ѹ� ����( ������ �� ��)
	char m_szInviteRoomPass[MAX_ROOM_PASSWORD];         // �ʴ�� ������ ��й�ȣ
	char m_szInviteMsg[MAX_CHATMSG];                    // �ʴ�� ������ �޼���
    char m_szPass[10];
	char m_szJackPotMoney[25];
	char m_szTempText[255];
	char m_szTempText2[255];
    char m_szUserAddMoney[MAX_ROOM_IN][256];            // �������� �ɾ��� �ݾ��� ���ڿ�
	
	HBRUSH m_Brush[4];
	COLORREF m_ColControl[4];                           // ��Ʈ�ѵ��� ����..

	BUTTON_STATUS m_nUserStatus[10];

    DWORD m_rgbMakeRoomTitleListBorder;                     // �游��� ��ȭ������ ������ ����Ʈ �׵θ� ����
    DWORD m_rgbMakeRoomTitleListBar;                        // �游��� ��ȭ������ ������ ����Ʈ ���� �� ����
    DWORD m_rgbGray;                                        // ȸ��

	POINT m_sMakeRoomMoneyPoint[ROOM_DIVI_CNT];         //�游��� �Ҷ� �ݾ��� ��� ��ġ.
  	POINT m_JakcPotMathPoint[15];
	POINT m_JackPotCommaPoint[4];

	RECT m_rCheckRect;
	RECT m_SelRect;                                     // ������ ��ư ��Ʈ 1��
	RECT m_StartBTNRect;

	//���� �ƹ�Ÿ.
	ImageEx			*m_WaitAvaImg;
	CDXSprite		*m_SPR_WaitAvatar;

	//��ƹ�Ÿ
	ImageEx			*m_GameAvaImg[MAX_ROOM_IN];
	CDXSprite		*m_SPR_GameAvatar[MAX_ROOM_IN];

	HANDLE m_hSocketThread;
	HANDLE m_hRenderThread;
	HANDLE m_hAniUpdateThread;
    HANDLE m_hGameInitEvent;
	
	CDXSprite *m_SPR_Money;                 // ��� ǥ���� ��
	CDXSprite *m_SPR_MessageBox;            // ���� â
	CDXSprite *m_SPR_Message;               // �޼��� �ڽ� �׸�
	CDXSprite *m_SPR_LoGo;
	CDXSprite *m_SPR_Cursor;
	CDXSprite *m_SPR_Connting;
	CDXSprite *m_SPR_ConBox;
	CDXSprite *m_SPR_WaitBase;
	CDXSprite *m_SPR_MakeRoom;
	CDXSprite *m_SPR_Class;                 // ��� ������
	CDXSprite *m_SPR_FullWait;
	CDXSprite *m_SRP_SexIcon;
	CDXSprite *m_SPR_GameBase;
	CDXSprite *m_SPR_Sun;
	CDXSprite *m_SPR_BangJang;
	CDXSprite *m_SPR_BetType_Big;
	CDXSprite *m_SPR_BetType_Small;
	CDXSprite *m_SPR_USERTIME;
    CDXSprite *m_SPR_UserListTitle[2];      // [0] = ����� ����Ʈ Ÿ��Ʋ�̹���, [1] = �� ���� ����Ʈ Ÿ��Ʋ �̹���
    CDXSprite *m_SPR_HighlightBar;
    CDXSprite *m_SPR_SelRoomBar;            // ���õ� �� ǥ�� �����
    CDXSprite *m_SPR_PassDlg;               // ��й� �� �� �н����� �Է� â
    CDXSprite *m_SPR_ChodaeOX;              // ���� ���� ����Ʈ ���� �ʴ밡�� ���� ǥ��

	CDXSprite *m_SPR_CardSpade;
	CDXSprite *m_SPR_CardClover;
	CDXSprite *m_SPR_CardDia;
	CDXSprite *m_SPR_CardHeart;
	CDXSprite *m_SPR_CardBack;
	CDXSprite *m_SPR_CardChioce;
	CDXSprite *m_SPR_WindowWin;
	CDXSprite *m_SPR_WinnerMark;
	CDXSprite *m_SPR_Chip;
	CDXSprite *m_SPR_JackPot;
    CDXSprite *m_SPR_JackPotBack;
    CDXSprite *m_SPR_JackPotMath;


	CDXSprite *m_SPR_AllIn;
	CDXSprite *m_SPR_InviteBox;
	CDXSprite *m_SPR_Invitation;
    CDXSprite *m_SPR_Start;
	CDXSprite *m_SPR_Enter;
	CDXSprite *m_SPR_WasteCard;
	CDXSprite *m_SPR_ShowUserInfo;
	CDXSprite *m_SPR_UserState;
    CDXSprite *m_SPR_CheckBall;                 // �游��⿡�� �ݾ� üũ�ϴ� ���׶��
    CDXSprite *m_SPR_SelectUser;                // ���õ� ���� ǥ���ϴ� �̹���
    CDXSprite *m_SPR_BetMoneyBack;              // ��ư ���� �ݾ� ǥ�ÿ� ��� �̹���
    CDXSprite *m_SPR_Arrow;                     // ����ī�� ���� ǥ�õǴ� ȭ��ǥ

	CDXButton *m_MessageOKBTN;                  // �޼��� �ڽ� OK ��ư
	CDXButton *m_FullBTN;                       // Ǯ��ũ�� ��ư ( ä�� )
	CDXButton *m_MaxBTN;
	CDXButton *m_MaxBTN2;
	CDXButton *m_ExitBTN;
	CDXButton *m_ConfigBTN;
	CDXButton *m_Exit2BTN;
	CDXButton *m_MinBTN;
	CDXButton *m_OkBTN;
    CDXButton *m_CaptureBTN;                    // ĸó ��ư

	CDXButton *m_MakeRoomBTN;
	CDXButton *m_JoinBTN;
	CDXButton *m_QuickJoinBTN;
	CDXButton *m_CancelBTN;
    CDXButton *m_RoomTitleBTN;                  // �� ��� �����ִ� ��ư
    CDXButton *m_MakeRoomOkBTN;                 // �游��� â - '�����' ��ư
    CDXButton *m_MakeRoomCancelBTN;             // �游��� â - '���' ��ư

	//Game Room
	CDXButton *m_InviteBTN;
	CDXButton *m_DDaDDang;    // [*BTN]
	CDXButton *m_Quater;
	CDXButton *m_Half;
	CDXButton *m_Full; // [*BTN]
	CDXButton *m_Check;
	CDXButton *m_BBing;
	CDXButton *m_Call;
	CDXButton *m_Die;
	CDXButton *m_StartBTN;
	CDXButton *m_InviteOKBTN;       // ���� �ʴ� �ڽ����� �ʴ� ��ư
    CDXButton *m_InviteCancelBTN;   // ���� �ʴ� �ڽ����� Cancel ��ư
	CDXButton *m_ChodaeYesBTN;
	CDXButton *m_ChodaeNoBTN;
	CDXButton *m_UserCloseBTN;
	CDXButton *m_GetOutBTN[MAX_ROOM_IN];
	CDXButton *m_UserStateBTN;
	CDXButton *m_MemoSendBTN;
	CDXButton *m_MemoCancelBTN;

    CDXScrollBar *m_SCB_WaitChat;               // ���� ä��â ��ũ�� ��
    CDXScrollBar *m_SCB_WaitUser;               // ����� ����Ʈ ��ũ�� ��
    CDXScrollBar *m_SCB_WaitRoom;               // �� ����Ʈ ��ũ�� ��
	CDXScrollBar *m_SCB_GameChat;               // �� ä��â ��ũ�� ��
    CDXScrollBar *m_SCB_Invite;                 // �ʴ�â�� ��ũ�� ��

	CDXBtnTime m_szBtnTime[MAX_BTNTIMECOUNT];
	
    CDXIME             *m_IME_WaitChat;         // ���� ä��â �Է¿� IME
    CDXIME             *m_IME_GameChat;         // ����ȭ�� ä��â �Է¿� IME
    CDXIME             *m_IME_InputPass;        // ��й� ��й�ȣ �Է¿� IME
    CDXIME             *m_IME_InputRoomTitle;   // �游��⿡�� �� ���� �Է¿� IME
    CDXIME             *m_IME_Memo;             // ���� �Է¿� IME
    CDXIME             *m_IME_RoomInPassBox;    // ��й� �� ���� ��й�ȣ �Է�â
	
	GAMEMESSAGE        *m_Message;              // �˶� �޽���
	CONFIGBOX          *m_pConfigBox;           // ���� â ����

    CTextList          *m_pWaitChat;            // ���� ä��â(�ؽ�Ʈ ����Ʈ)
    CTextList          *m_pGameChat;            // ����ȭ�� ä��â(�ؽ�Ʈ ����Ʈ)
    CScrollText        *m_pScrollNotice;

	CAnimation          m_CAniMation;
    //CHttpView          *m_pWaitRoomBanner;      // ���� ���
    CChipAni            m_ChipAni;              // �� ���ϸ��̼� ����
	
	sRoomInUserPosition m_sRoomInUserPosition[MAX_ROOM_IN];
	sINVITECHECK        m_sInviteCheck[MAX_GAMEVIEWWAIT];     // �ʴ� �ڽ��� üũ ������

	sPanMoneyInfo       m_sPanMoneyInfo[16];
    sMovingHiddenCardInfo m_sHiddenCardInfo;

	POINT SetPoint( LONG x, LONG y );
	void Render();
	void RenderGameBase();
	void RenderUserInfo();
	void RenderPanMoney();
	void RenderWinner();
	void RenderWinnerMark();
	void RenderTimePos();
	void RenderUserCard();
    void RenderCardSelectionDlg();
	void RenderRoomJackPot();
	void RenderInviteBox();
	void LoadEnviroMent();
	void LoadData();
	void OnLButtonDown();
	void OnLButtonUp();
	void ReleaseData();
	void RButtonDownCheck( int MouseX , int MouseY );
	void RButtonDownCheckInGame( int MouseX , int MouseY );
	void RButtonEvent();
	void RButtonUpCheck();	
	void ChannelModeSet();
	void GameModeSet();
	void WaitRoomModeSet();
	void SoundLoading();
    void DestroyAllSound();
	void LoadControl();
	void SetHumanVoice( int nVoiceNo );

	void ProcessButtonEvent();
	void CheckButtons( int mX, int mY );
	void ButtonUpCheck();
	void ButtonDownCheck( int mX, int mY );
	void OnExitBtnDown();
	void RenderWaitRoom();
	void CreateRoom();
	void SetGameCursor( BOOL bUserCursor , BOOL bClick, BOOL bMouseRender = TRUE );
	void RenderMakeRoom();                      // �游��� â ������
	void RecieveInvite( int nRoomNo , BigInt biRoomMoney, char *pRoomPass , char *pInviteMsg );
	void MakeRoom( BOOL bSecret );              //true ���
    void OpenMakeRoomDlg();                     // �游��� ��ȭ���ڸ� ����.
	void RecieveMemo( char *szSendUserNick , char *szMessage );
	void I64toA_Money( BigInt biMoney, char *szBuff, int nUnitCnt = 0 );
	void DeleteUserForScroll( void );
	void GameInit();
    void NetWorkThreaOnGameInit();
	void GetOut();
    //void ShowBanner();
    //void CloseBanner();
    void ThrowChip( int nSlot, BigInt nMoney );

	BOOL SendChatTime( int nTime );
	BOOL SendBtnTime( CDXButton *Btn , int nTime , BOOL bWavePlay = TRUE );
	BOOL MouseIn( int X , int Y , int X2 , int Y2 );
	void AvatarDelete();
	void AvatarPlay( int nSlot, char *pAvaUrl );


    BOOL isInteger(CString origin);
	int GetRandom(int from, int to);
	unsigned long ServerRandom(void);
	void ServerSrandom( unsigned long initial_seed );
    void NickSave( char *pShortNick, char *pNickName ); // ���� �Գ����� 10����Ʈ�� ������ �ٿ��� �����Ѵ�.
	void NickSavePrint( char *pNickName, int x, int y , COLORREF rgbCol = RGB(255, 255, 255) ); //���� �г��� 10����Ʈ�� ������ �ٿ���´�.
	void AnimationUpdate();
	void OnBtnStart();                          // ���� ��ư Ŭ����
	int  FindSunArrayNo();
	void UserCardAdd( BYTE nCardNo , int nUserArrayNo, BOOL bAniMation = TRUE ); // �ѿ��� �������� �� ������
	void PutCardImage( int x, int y, BYTE nCardNo, BOOL bUp = TRUE );
	BOOL CheckPosition( sUserCardInfo *pos );   // pos->NewPos�� pos->Pos�� �̵�
    BOOL CheckChipPosition( sChipInfo *pos );
	void SetUserCall( sGameCurUser asGameCurUser, int nUserArrayNo );
	void AllCallBtnUp();
	void JokboShow( BYTE *pJokbo, char *pResult );
	void UserSelectView( int nSlotNo, char cUserSelect, BOOL bNetWorkCall = FALSE );	//�ڽ��� ȣ���߳�.��Ʈ��ũ���޳�.
	void UserSelectPutSprite( int nSlotNo, char cUserSelect, BOOL bBig = TRUE );
	void SetNumLock( BOOL bState );
	void SetPanMoney( int nUserSlot, BigInt biPanMoney, BOOL bThrow = TRUE );
	void PutSpriteAvatar( int x, int y, CDXSprite *aAvatarSPR, ImageEx *aAvaImageEx, int nType = SMALL_SIZE );
	void SetJackPotMoney( BigInt biJackPotMoney );
	void InitInviteChecks( void );
	int  GetInviteCheck( void );
	void DrawInviteUsers( int InviteBox_XPos , int InviteBox_YPos );
	void InputInviteChecks( int X , int Y , int SizeX , int SizeY );
	void DrawInvite( int X , int Y );
	void EndingSound( BYTE nEnding );
	void RenderShowUserInfo();
	void DrawUserState( int X , int Y );
	void DrawMessageBox( int X , int Y );
    void RenderPassDlg();                           //�н����� �Է�â�� ����.
    void RenderWaitChat();                          // ���� ä��â�� �׸���.
    void RenderGameChat();                          // ����ȭ�� ä��â�� �׸���.
    void RenderWaitUser( int nUserNum );            // �ڼ��� ����� ������ ����Ѵ�.
    void RenderRoomUser( int nUserNum );            // �ڼ��� �� ���� ������ ����Ѵ�.
    void RenderThrowChip();                         // ������ ���� ���� �׸���.
    void PanMoneyReFresh();                         // �� �����̵��� ������ ���� ���� �ٽ� �׷��ش�.
    void PutSprite( CDXSprite *pSprite, int nX, int nY, int nFrame = 0 );
    void PutSprite( CDXSprite *pSprite, int nX, int nY, RECT *srcRect );
    void PutSpriteBlk( CDXSprite *pSprite, int nX, int nY, int nFrame = 0 );
    void DrawScale(CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame = 0);
    void GetFillRectColor( UINT bpp );
    
    BOOL ScreenCapture();
    void WaitRoomIndexRefresh();                    // ���� ��½� ���Ǵ� �ε����� �����ϴ� �Լ�
    void ViewRoomInfoRefresh( ush_int nRoomNo );    // ���� �����ִ� ���� ������ �����Ѵ�.

    CDXSoundBuffer *LoadSndFile( char *szPath, char *szFilename, int nBuffNum = 1 );
    void PlaySound( CDXSoundBuffer *pSound );
    void ChangeVideoMode( BOOL bFullScreen );
    void AdjustWinStyle();
    void DrawAvatarScale( CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight );
    void SetColorKey();

    CDXButton    *LoadButton( LPSTR szFilename, int nX, int nY, int nW, int nH, BOOL bMulti = FALSE, int nCount = 0, int nDirection = NONE, int nDistance = 0 );
    CDXSprite    *LoadSprite( LPSTR szFilename, int nW, int nH, int nNum );
    CDXScrollBar *LoadScrollBar( LPSTR szUpBTN, LPSTR szDownBTN, LPSTR szDragBTN, RECT *pScrollBarRect,
                                int nUpDownBTNWidth, int nUpDownBTNHeight, int nDragBTNWidth, int nDragBTNHeight,
                                int nViewNum, int nMaxNum );
    void DrawText( char *szText, int nX, int nY, COLORREF rgbColor, int nStyle, int nAlign, int nFontSize = 12 );
    void SetGameChatText( char *szText, COLORREF rgbColor );
    void ExitGame();    // ���� ����(WM_CLOSE �޽����� ������.)
    void RecoveryAvatar();  // ȭ���� ��ȯ������ ���� ǥ���ı��� �ƹ�Ÿ ����

#ifdef CHANGEUSERCARD
    BOOL CheckChatCommand(char *szText);                            // ä�� ��ɾ� ó�� �Լ�
    void ChangeUserCard( int nSlotNo, sUserChange *pChgCardInfo );  // ī�� �ٲٴ� �Լ�
    void SendChangeUserCardInfo(char *szParam1, char *szParam2);    // ������ �ٲ� ī�� ������ ������.
    int ConvCardNum( char *szCardInfo );                            // ī�� ���� ���ڿ��� ī�� ��ȣ�� ��ȯ�Ѵ�.
#endif
    
public:

    //Frame���õ� ����.
	FrameSkip m_FrameSkip;
	float m_fSkipTime;
};


DWORD WINAPI RenderThreadProc( LPVOID lParameter ); 
unsigned __stdcall AniUpdateThreadProc( LPVOID lParameter );    // ham 2005.11.29

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__6FA7B5FA_1496_4A83_9E4D_8E012DB18D1D__INCLUDED_)
