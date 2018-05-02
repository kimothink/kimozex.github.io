// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__18F46A52_7465_47AC_A995_C47A43F681B3__INCLUDED_)
#define AFX_MAINFRM_H__18F46A52_7465_47AC_A995_C47A43F681B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Animation.h"
#include "GoStopEngine.h"
#include "AlarmMsg.h"
#include "HttpView.h"
#include "FrameSkip.h"
#include "CTextList.h"
#include "CScrollText.h"
#include "AdminLogin.h"


#define EVENTCOUNT	1
#define DEF_TIME 600
#define MAX_BTNTIMECOUNT 100

#define GTC GetTickCount


class CONFIGBOX
{
public:

	//BOOL bInvite; // �ʴ� //	   TRUE ��� 
	//BOOL bSecretMessage; // �ӼӸ� TRUE ���
	//BOOL bWaveSnd; // ȿ����	   TRUE ���
	//BOOL bMessageAgree; // �޼���  TRUE ���

	BOOL bCheck[4];

	int nVoice; // 0 �ϼҿ�. 1 ���� . 2 ���� . 3 . ���� ( �ʱ� ���� )
	
public:
	
	CONFIGBOX( BOOL btInvite = TRUE , BOOL btSecretMessage = TRUE , BOOL btWaveSnd = TRUE , BOOL btMessageAgree = TRUE , int ntVoice = 0 ); // ������
	void RenderConfigBox( void );

	BOOL GetCheckState( int nNum );
	void ChangetCheck( int nNum , BOOL TempCheck );

	void ChangeVoice( int nNum );
	int  GetVoiceState( void );
};



class CDXTime
{ 
public:
	double targetfps;
	double fps;
	LARGE_INTEGER tickspersecond;
	LARGE_INTEGER currentticks;
	LARGE_INTEGER framedelay;
	
	double speedfactor;			

public:
	
	void Init(double tfps);
	void SetSpeedFactor();
};



struct CDXBtnTime
{
	CDXButton *CDXBtn;
	int nTime;
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
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CDXSprite *l_SPR_WaitBase;
	CDXSprite *l_SPR_GameBase;
	CDXSprite *l_SPR_MakeRoom;
	CDXSprite *l_SPR_Result;
	CDXSprite *l_SPR_HwatuBackSmall;
	CDXSprite *l_SPR_Hwatu;
	CDXSprite *l_SPR_MissionTimes;
	CDXSprite *l_SPR_Test;
	CDXSprite *l_SPR_Cover;
	CDXSprite *l_SPR_BombLine;      // bsw 01.08
	CDXSprite *l_SPR_Dialog;        // bsw 01.08
	CDXSprite *l_SPR_Cursor;        // bsw 01.08
	CDXSprite *l_SPR_Cursor2;       // bsw 01.08
	CDXSprite *l_SPR_EVENT;         // bsw 01.08
	CDXSprite *l_SPR_Connting;
	CDXSprite *l_SPR_Wait;          // bsw 01.08
	CDXSprite *l_SPR_NotifyText;    // 01.12
	CDXSprite *l_SPR_TurnLine[MAX_ROOM_IN]; // �����̴� ���(� ������ ������ ������ ���������� ǥ����)
	CDXSprite *l_SPR_HwatuSelect;
	CDXSprite *l_SPR_Time;
	CDXSprite *l_SPR_LoGo;
	CDXSprite *l_SPR_WaitAvatar;
	CDXSprite *l_SPR_Start;
	CDXSprite *l_SPR_Baks;          // ���� , �ǹ� , ���ڵ�.
	CDXSprite *l_SPR_DlgBar;        // ���̾� �α׿� ����ϴ� �� �� �� ǥ��
	CDXSprite *l_SPR_P_Back;        // �� ���� ���
	CDXSprite *l_SPR_FullWait;      // 
	CDXSprite *l_SPR_ConBox;        // ���� â �׸� ����
	CDXSprite *l_SPR_KingFirst;     // ����� �� �׸�
	CDXSprite *l_SPR_COM;           // Com �ƹ�Ÿ.
	CDXSprite *l_SPR_InviteBox;     // ���� �ʴ� �ڽ� �׸�
	CDXSprite *l_SPR_InviteIcon;    // ���� �ʴ� �ڽ� �׸��� ���� , ���� ������
	CDXSprite *l_SPR_MessageBox;    // ���� â
	CDXSprite *l_SPR_Invitation;    // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? â )
	CDXSprite *l_SPR_UserState;     // ���콺 ������ ��ư���� ���� �κ��� ������ �ߴ� â �׸�
	CDXSprite *l_SPR_Message;       // �޼��� �ڽ� �׸�
	CDXSprite *l_SPR_AffairNum;     // ��, ��,  ��� ���� Ƚ���� ��Ÿ���� �̹���
	CDXSprite *l_SPR_Jumsu;         // ���̸� ����( �� )
	CDXSprite *l_SPR_RoomFullorNot; // ���� �� ���ְų�.. ��� ���ϴ� ���϶�..
	CDXSprite *l_SPR_ResultMessage; // ���â���� �޼��� �׸� ( ������ , ������ �� �Դ�. )
	CDXSprite *l_SPR_MissionOK;     // �̼� OK ( �� ���Կ� ���� �ִ� �̼� ���� �׸� )
	CDXSprite *l_SPR_Class;         // ��� ������
	CDXSprite *l_SPR_MissionNext;   // ���� ��ȸ��.. ����ش�.
    CDXSprite *m_SPR_ResultNum[2];  // ��� ��¿� ����
    CDXSprite *m_SPR_ExitReserv;    // ������ ���� �̹���
    CDXSprite *m_SPR_UserListTitle[2];
    CDXSprite *m_SPR_WaitDlg;       // ��� �޽��� â
    CDXSprite *m_SPR_CheckBall;     // �游��⿡�� ���� ���׶��
    CDXSprite *m_SPR_HighlightBar;  // �����/������ ����Ʈ�� ���϶���Ʈ �� �̹���
    CDXSprite *m_SPR_PassDlg;       // ��� �Է�â.
    CDXSprite *m_SPR_CardBlink;     // �� �����ϱ� ���ؼ� ī�� ������ �� ī�尡 �����̴� ���� ǥ���ϱ� ���� ���
    CDXSprite *m_SPR_ChodaeOX;      // ���� �ʴ� OX �̹���
    CDXSprite *m_SPR_RoomBack;      // ���� �� ��� �̹��� // ham 2006.01.18
    CDXSprite *m_SPR_UserIcon;      // ���� �� ��� ��¿� ���� �̹���(0=����, 1=����)

    CDXSprite *m_SPR_Arrow;         // �̼� ī�带 ���� �Ծ����� ǥ���ϱ� ���� ȭ��ǥ �̹���
    CDXSprite *m_SPR_MissionDlg;    // �̼� ��ȭ����
    CDXSprite *l_SPR_MComplete;     // �̼� ���ø� �׸�
    CDXSprite *m_SPR_SelSunText;    // �� �����ϴ� ȭ�鿡�� ����ϴ� �ؽ�Ʈ �̹���
    CDXSprite *m_SPR_SelSunTextBack;// �� �����ϴ� ȭ�鿡�� ����ϴ� �ؽ�Ʈ�� ��� �̹���(������)

    CDXSprite *l_SPR_ResultDojang;  // ���â ���� ���..
	RECT       l_rDojangRect[2];    // ���� ��Ʈ
	int        m_nDojangCount;
	int        m_nDojangShow;
	BOOL       m_bDojangSound;
	void       DrawResultDojang( int nType );   // 1 �� �̼� , 2�� ��� , 3�� ����

	CDXButton *l_MinBTN;
	CDXButton *l_MaxBTN;
	CDXButton *l_MaxBTN2;
	CDXButton *l_ExitBTN;
	CDXButton *l_StartBTN;
	CDXButton *l_MakeRoomOkBTN;
	CDXButton *l_MakeRoomCancelBTN;
	CDXButton *l_GoBTN;             // 01.08
	CDXButton *l_StopBTN;           // 01.08
	CDXButton *l_GYESBTN;           // 01.12
	CDXButton *l_GNOBTN;            // 01.12
	CDXButton *l_ConfigBTN;         // 01.13 ���� ��ư( ���ӷ� )
	CDXButton *l_Exit2BTN;          // 01.14 ������ ��ư ( ���ӷ� )
	CDXButton *l_ConfigOkBTN;       // 01.14 ��� Ȯ�� ��ư( ���� �� )
	CDXButton *l_WaitGamerBTN;      // hy 04.01.16 ���� ��� ��ư( ���� , ��Ƽ )
	CDXButton *l_ResultOKBTN;       // ���â OK ��ư
	CDXButton *l_InviteBTN;         // ���� �ʴ� ��ư
	CDXButton *l_InviteOKBTN;       // ���� �ʴ� �ڽ����� OK ��ư
    CDXButton *l_InviteCancelBTN;   // ���� �ʴ� �ڽ����� Cancel ��ư
    CDXButton *l_RecvChodaeOkBTN;   // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? â & �ʴ� ���� â ) Ȯ�� ��ư
    CDXButton *l_RecvChodaeCancelBTN;  // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? ); ��� ��ư
	CDXButton *l_MemoSendBTN;       // ���� â ������ ��ư
    CDXButton *l_MemoCancelBTN;     // ���� ������ ���
	CDXButton *l_UserStateBTN;      // �� ���� ��ư
	CDXButton *l_MessageOKBTN;      // �޼��� �ڽ� OK ��ư
    CDXButton *l_CaptureBTN;        // ȭ�� ĸó ��ư
    CDXButton *m_RoomTitleBTN;      // �� ��� �����ִ� ��ư
    CDXButton *m_PassDlgOkBTN;      // �н����� �Է� â OK ��ư
    CDXButton *m_PassDlgCancelBTN;  // �н����� �Է� â Cancel ��ư

    CDXButton *m_MakeRoomBTN[9];    // �游��� ��ư 9�� // ham 2006.01.18
    CDXButton *m_JoinBTN[9];        // �� ���� ��ư 9�� // ham 2006.01.18

    CDXScrollBar *m_SCB_WaitChat;               // ���� ä��â ��ũ�� ��
    CDXScrollBar *m_SCB_WaitUser;               // ����� ����Ʈ ��ũ�� ��
    CDXScrollBar *m_SCB_WaitRoom;               // �� ����Ʈ ��ũ�� ��
   	CDXScrollBar *m_SCB_GameChat;               // �� ä��â ��ũ�� ��
    CDXScrollBar *m_SCB_Invite;                 // �ʴ�â�� ��ũ�� ��

    CDXSurface *m_SFC_DialogBack;    // ������ ȿ���� ���� �ӽ� ���۷� ����� ��������Ʈ[alpha]

    CDXIME *m_IME_WaitChat;         // ���� ä��â �Է¿� IME
    CDXIME *m_IME_GameChat;         // ����ȭ�� ä��â �Է¿� IME
    CDXIME *m_IME_InputPass;        // ��й� ��й�ȣ �Է¿� IME
    CDXIME *m_IME_InputRoomTitle;   // �游��⿡�� �� ���� �Է¿� IME
    CDXIME *m_IME_RoomInPassBox;    // ��й� �� ���� ��й�ȣ �Է�â
    CDXIME *m_IME_Memo;             // ���� �Է¿� IME

public:
	int l_nSecPos;
	
	int l_nDrawEventType, l_nDrawEventSlotNo, l_nDrawEventFrameCnt, l_nDrawEventWaitTime;
	int l_nDialogType, l_nPercentage, l_nWaitTime, l_nMouseOverCardNo;
	int l_nAniEventType;
	int l_EventTime;
	int l_nNameNo; // �� ���� ��ȣ
	int l_nCurName; // 
	int l_nAniMaxCount;
	int l_nOldSlotNo;
	int	l_nFloorCnt;
	int	l_nUserNo;
	int	l_nTimeValue;
	int l_nRoomNo;
	int	l_nOldSelNo;
	int l_nCurRoom;
	int l_nTempCardNo;
	int l_nBombLineFrameCnt;
	int l_nCardFlipFrameCnt;
	int l_nDialogWaitFrameCnt;
	int m_nSunRenderDecided;	//�� ������ �� �������� �ð�
	int m_nSunSetRender;		//�� �����ϴ� �ð�.
	int l_nDialogResultFrameCnt, l_nDialogResultWaitTime;
	int l_nDialgType;
	int l_nDlgMode;
	int l_nPVal[MAX_ROOM_IN];  // ���� ���� ���� ���� ( ���� ���� ����� ���� ) 
	int l_nPX[MAX_ROOM_IN];    // ���� ������ X ��ǥ. ( ���� ���� ����� ���� )
	int l_nPY[MAX_ROOM_IN];    // ���� ������ Y ��ǥ. ( ���� ���� ����� ���� )
	int l_mX, l_mY, l_sX, l_sY, l_cX, l_cY; // ���콺 ��ġ ����� ������
	int nTempTic1, nTempTic2;
	int nRoomX, nRoomY;         // ���� X , Y ��ǥ
	int CurT1, CurT2, CurTime;
	int nChatTime;              // ä�� ���� �ð�
	int l_nViewRoomMessage;     // ��� ���� �������� ����.
	int l_nViewRoomUser;        // ��� ���� ������ �������� ����.
	int l_nViewWaitUser;        // ���õ� ����� �ε���(��ȣ)
	int l_nOldViewRoomMessage;
	int l_nOldViewRoomUser;     // ��� ���� ������ �������� ����.
	int l_nOldViewWaitUser;     // ������ ������ ���ȴ��� ����.
	int l_nRoomUserNo;          // ���� ���� ���� �˾Ƴ���.
	int l_nCurRoomUser;         // ���� ���� ���� �˾Ƴ���
	int l_nWaitUserNo;          // ������ ���� ���� �˾Ƴ���
	int l_nPageStart;           // ���� ���� �ִ� �������� ���� ��ȣ
	int l_nWaitUserCurCnt;      // ���� ���� �����
	int nDegree;                // ������� ������ ���ΰ�.
	int g_nPuzzleNo;
	int l_nScrollSelNo;         // ��ũ�ѿ��� ������ ���� �ڷ� ��ȣ
	int l_nMBoxTempX , l_nMBoxTempY;
	int l_nMBoxXPos, l_nMBoxYPos;
	int l_nRButtonWaitUserNo;
	int l_TempMX , l_TempMY ;
	int l_nInviteTempX, l_nInviteTempY;
	int l_nInviteXPos, l_nInviteYPos; 
	int l_nWaitViewPageNo;
	int l_nInviteRoomNo;        // �ʴ�� ������ �� ��ȣ
	BigInt l_biInviteRoomMoney;
	int l_nCurMashine;
	int l_nCurChannelNo;
	int l_nStartBtnCnt;
	int l_nOrder[12];
	int l_nCurrentMode;
	int l_TempAlpha;
	int l_StartBTNState;
	int l_StartT1 , l_StartT2 , l_StartAni;
	int m_nCurSorCnt;
	int m_nMySunChioce;
	int m_nOtherSunChioce;
    int m_nHighlightBarNum;         // ����� ����Ʈ ���� ��ȣ
    int m_nUserListTitleNum;        // ���� ����Ʈ Ÿ��Ʋ �̹��� ��ȣ(0 = ����� ���, 1 = �� ���� ���)
    int m_nSelectedRoomTitleList;   // �游��� â - �� ����Ʈ �� ���õ� ���̸��� ��ȣ(������ -1)
    int m_nHwatuSelectCnt;          // ���� ī�� ���� ���콺�� �ִ°�� ����ī�� ǥ�� �̹��� ���Ͽ��̼��� ���� ���
    int m_nHwatuSelectFrm;          // ���� ī������ ǥ�õǴ� �̹����� ������ ��ȣ
    int m_nCardBlinkCnt;            // �� ���� ���� ī�� ������ �����̰� �ϴµ� ���Ǵ� ī����
    int m_nCardBlinkFrm;            // �� ���� ���� ī�� ���Ǹ� �����̰� �ϴµ� ���Ǵ� ������ ����

	HANDLE  m_hSocketThread;
	HANDLE  m_hQThread;
	HANDLE m_hRenderThread;
	HANDLE m_hAniUpdateThread;

	BOOL l_bDrawEvent, l_bShowDialog, l_bShowResult;        //l_bWaitDialog;
	BOOL l_bTimeEvent;
	BOOL l_bShowRoomOpen;                                   // �� ��ư ���ȳ� �˻�
	BOOL l_bFullScreen;
	BOOL l_bActive;
	
	BOOL l_bMoveDlg;
    BOOL l_bMouseDown;
	BOOL l_bShowStartBTN;
	BOOL l_bShowBombLine;
	BOOL l_bShowTurnLine;
	
	BOOL SetCur;
	BOOL l_bShowConfigBox;                                  // ���� â�� ����°�.
	BOOL l_bShowUserState;                                  // ������ ���� ������ ������ , ���� ���� ������ ������ �����Ѵ�
	BOOL l_bShowInviteBox;                                  // ���� �ʴ� �ڽ��� ���� �ݴ� BOOL ����
	BOOL l_bShowMessageBox , l_bMBoxMoveDlg ;
	BOOL l_bMessageSend;                                    // ������ �����°��� �޴� ���� üũ // TRUE = ������. FALSE = �޴´�.
	BOOL l_bUserState;
	BOOL l_bRButtonDown;
	BOOL l_bShowChodaeBox;                                  // �̰� TRUE �̸� �ʴ�ڽ��� ����.
	BOOL l_bChodaeDlg;                                      // ���콺�� �ʴ�â�� �����δ�. = TRUE; ( Drag ȿ�� )
	BOOL m_bActive;
	BOOL l_bStartFadeOut[MAX_ROOM_IN];
	BOOL l_bShowMission;                                    // �̼� ��� ����
	BOOL l_bShowMakeRoomDlg;
	BOOL m_bMouseEvent;
	BOOL m_bMouseRender;
	BOOL m_UserTimerThread;
	BOOL m_bSunSet;
	BOOL m_bSunSetRender;		                            //10�� �߿� �����ϱ����� 10�� �Ѹ��°�
	BOOL m_bSunSetChoice;		                            //�ѹ��� �����ϱ� ���� üũ
	BOOL m_bRenderSun;			                            //�� �������� ���� ������ �� �ѷ��ִ°�.
    BOOL m_bShowExitReservIcon[MAX_ROOM_IN];                // ������ ���� �������� ������ ���� ����
    BOOL m_bShowRoomTitleList;                              // �游��� â - �� Ÿ��Ʋ ����Ʈ�� ���������� ����
    BOOL m_bPassRoomChk;                                    // ��й� �н����� üũ
    BOOL m_bPassDlg;                                        // �н����� �Է�â�� ����.

	RECT ComRect;                                           // ��ǻ�� ��Ʈ
	RECT l_BakRect[3][1];
	RECT ResultDataRect[5];
	RECT FullWaitRect;
	RECT CheckRect;
	RECT KingFirstRect;                                     // ����� �� �׸� ��Ʈ
	RECT SelRect;                                           // ������ ��ư ��Ʈ 1��
	RECT AvatarRect;
	RECT ResultPlusRect;                                    // ��� â���� ��, ��, ��, ��, �� �� ����Ҷ� ���� ��Ʈ
	RECT l_StartBTNRect;

	DWORD dwTurnFrameCnt;

    DWORD m_rgbMakeRoomTitleListBorder;                     // �游��� ��ȭ������ ������ ����Ʈ �׵θ� ����
    DWORD m_rgbMakeRoomTitleListBar;                        // �游��� ��ȭ������ ������ ����Ʈ ���� �� ����

	POINT l_TempMousePos;                                   // �ӽ� ���콺  ��ġ
	POINT l_CurMousePos;                                    // ���� ���콺 ��ġ
	POINT l_CardPackPos;                                    // ī�� �� ��ġ
	POINT l_UserCardPos[MAX_ROOM_IN][10];              // ����� �� ��ġ
	POINT l_EatCardPos[MAX_ROOM_IN][9];                // ���� �� ���� ��ġ
	POINT l_StartPos;
	POINT l_point;

	char l_szRoomName[255] , l_szTempRoom[80];              // Ÿ��Ʋ �̸�
	char g_szUserImage[MAX_ROOM_IN][MAX_IMAGE_LEN];    // ������ ������ �ִ� �׸� ( ���� )( 8 ����Ʈ )
	char l_szMessageSendUserNick[MAX_NICK_LENGTH];          // ���� ���� NickName
	char l_szMessageRecieveUserNick[MAX_NICK_LENGTH];       // �޴� ���� NickName
	char l_szGetMessageData[MAX_CHATMSG];                   // ���� �Ѹ� ����( ������ �� ��)
	char l_szMessageData[MAX_CHATMSG];                      // ���� �Ҹ� ����
	char l_szMyNick[MAX_NICK_LENGTH];                       // �� �г���
	char l_szInviteRoomPass[MAX_ROOM_PASSWORD];             // �ʴ�� ������ ��й�ȣ
	char l_szInviteMsg[MAX_CHATMSG];                        // �ʴ�� ������ �޼���
	char l_szTemp[MAX_ROOM_IN][10];
	char l_szTempText[255];
	char m_szBannerIp[255];
    char m_szTempText[256];
    char m_szPass[10];                                      // �� �н����� ����

    CARDPOS l_CardPos[53];
	CARDPOS m_szSunPackPos[MAX_SUNCARD_CNT];
	POINT m_szSunPackPoint[MAX_SUNCARD_CNT];
	BYTE m_szSunPack[MAX_SUNCARD_CNT];

    POINT m_sExitResrv[MAX_ROOM_IN];                        // "������ ����" �̹��� ��� ��ǥ
    POINT m_sSunMarkPos[MAX_ROOM_IN];                       // "��" ǥ�� ��ġ 
    POINT m_sBangJangMarkPos[MAX_ROOM_IN];                  // "����" ǥ�� ��ġ
    POINT m_sUserScorePos[MAX_ROOM_IN];                     // ���� ���� ȭ�� - ���� ���� ��� ��ġ
    POINT m_sGoCntPos[MAX_ROOM_IN];                         // ���� ���� ȭ�� - �� Ƚ�� ��� ��ġ
    POINT m_sShakeCntPos[MAX_ROOM_IN];                      // ���� ���� ȭ�� - ��� Ƚ�� ��� ��ġ
    POINT m_sPPuckCntPos[MAX_ROOM_IN];                      // ���� ���� ȭ�� - �� Ƚ�� ��� ��ġ
    POINT m_sTurnLinePos[MAX_ROOM_IN];                      // �����̴� ���� ��� �̹��� ��� ��ġ
    POINT m_sTimerPos[MAX_ROOM_IN];                         // Ÿ�̸� ��ġ
    POINT m_sEventPos[MAX_ROOM_IN];                         // �Ǿ�~, ���徿, �ٴ� ���� �̺�Ʈ ��� ��ġ
    POINT m_sNickNamePos[MAX_ROOM_IN];                      // �г��� ��� ��ġ
    POINT m_sClassPos[MAX_ROOM_IN];                         // ��� ��� ��ġ
    POINT m_sUserMoneyPos[MAX_ROOM_IN];                     // ���� �Ӵ� ��� ��ġ
    POINT m_sWinLoseMoneyPos[MAX_ROOM_IN];                  // ���� ���� �ݾ� ��� ��ġ
    POINT m_sMissionOkPos[MAX_ROOM_IN];                     // �̼� ���� ������ ��� ��ġ

	CRect m_rcWnd;

    sPrevDlgInfo    m_sPrevDlgInfo;                         //[alpha] ������ ������ ��ȭ������ ����(��¿���) ����
	sCreateRoom     l_sCreateRoom;
	sRoomIn         l_sRoomInData;                          // ������ ���� ����ü
	sINVITECHECK    sInviteCheck[MAX_GAMEVIEWWAIT];         // �ʴ� �ڽ��� üũ ������
    sMissionDlgInfo m_sMissionDlgInfo;                      // �̼�â ����

	int m_nMakeRoomMoneyNo;
	POINT l_sMakeRoomMoney[ROOM_DIVI_CNT];	                //���� ���鶧...���ΰ��� ���� ��ǥ.

	CDXTime l_Time;
	CAnimation l_AniStack;
	CDXBtnTime BtnTime[MAX_BTNTIMECOUNT];
	CONFIGBOX *l_ConfigBox; // ���� â ����
	GAMEMESSAGE *l_Message; // �˶� �޽���
    BUTTON_STATUS l_nUserStatus[12];
	BUTTON_STATUS l_nUserStatus2[MAX_GAMEVIEWWAIT];
	
	//CHttpView* m_pHtmlView;

    CTextList *m_pWaitChat;
    CTextList *m_pGameChat;

    CScrollText *m_pScrollNotice;

    // [zoom]
    // ī�� Ȯ�� ���̺�
    sCardZoomTable *m_sSmallZoomTable;
    sCardZoomTable *m_sBigZoomTable;

public:

	void AnimationUpdate();
	void Render();
	void LoadFile();  //�������ǰ� �ִ� ��ü ���ϸ��� �����´�.
	void LoadData();
    void SoundLoading();    // ���� �ε�
	
	void InitCardPostion();
	void RecieveInvite( int nRoomNo , BigInt biRoomMoney, char *pRoomPass , char *pInviteMsg );
	void RecieveMemo( char *szSendUserNick , char *szMessage ); // �޸� �ޱ�
	
	void ReleaseData();
	void RenderChannelBase();
	void RenderWaitRoom();
	void PutCursorText( int X , int Y , COLORREF rgb , char *szString );	
	void RenderMakeRoom();
    void RenderPassDlg();
	void RenderGameBase();
	void RenderUserInfo();
	void RenderSunPack();
	void RenderMission();
	void RenderScore();
	void RenderCardPack();
    void RenderDummyCardPack();
	void RenderUserEatCard();
	void RenderFloor();
	void RenderUserCard();
	void ShowTurnLine(int nSlotNo);
	void DrawEvent(int nSlotNo, int nType);
	void TimeEvent( int nSlotNo );
	void ShowDialog(BOOL bShowDlg, int nType, int nPercentage, int nMode = 1);
	void ShowResult();
	void DrawInviteUsers( int InviteBox_XPos , int InviteBox_YPos );
	void PutHwatuBackImage(long x, long y, BOOL bSmall = FALSE, int nAlpha = 34);
	void CreateRoom();
	void ProcessButtonEvent();
	void OnExitBtnDown();
	void InputInviteChecks( int X , int Y , int SizeX , int SizeY );
	void InitInviteChecks( void );
	void OnBtnStart(); // ���� ��ư Ŭ����
	void MakeRoom( BOOL bSecret ); //true ���
	void OnReGames( bool bMasterMode, int nSlotNo );
	void EndOtherGameUser();  //�����߿� �ٸ����� ������ �̳��� ������ �����ߴٰ� ������ ������ ���⿡�� ó�����ش�.
	void ButtonUpCheck();
	void ButtonDownCheck(int mX, int mY);
	void CheckButtons(int mX, int mY);
	void SortUserEatCard(int nSlotNo);
	void RepaintUserCard(int nUserNo);
	void AniEvent(int nSlotNo, int nType);
	void GetMouseOverCardNo();
	void ShakeFloor( void );
	void DrawMessageBox( int X , int Y ); // �޼���(����)â ������
	void DrawUserState( int X , int Y );
	void RButtonDownCheck( int MouseX , int MouseY );
	void RButtonUpCheck( void );
	void RButtonEvent( void );
	void RButtonDownCheckInGame( int MouseX , int MouseY );
	void RButtonUpCheckInGame( void );
	void RButtonEventInGame( void );
	void DrawInvite( int X , int Y ); // �ʴ� â ������
	void FuctionKeyDown( WPARAM wParam );
	void OnLButtonDown();
	void OnLButtonUp();
	//void ShowBanner();
	//void CloseBanner();
	void GameBreak();
	void Effect( int nSlotNo, int nEventDivi, int nDelayTime = 50 );
	void PregiDent( int nPregidentCardNo );
	void ShowMission();
	void ShakeQ();
	void Shake();
	void WaitCardChoice();
	void SelectNine();
	void SelectGoStop();
	void SelectTwoCard( int nCurUserNo, int nCardNo );
	void WaitGoInfo();
	void MouseRender();
	BOOL ChannelCheck( int nCurMashine );
    BOOL ScreenCapture();
    void GetFillRectColor( UINT bpp );                      // FillRect()���� ����ϴ� ���� ���� ȭ�� ��忡 �°� �����Ѵ�.

    int GetSelUserIdx();       // ���� ������ ���� ����Ʈ���� ���콺 Ŭ���� ��ġ�� ���ؼ� �����Ѵ�.
    int GetSelRoomIdx();
	int	CheckFloorTwoPee(const int nCardNo);
	int GetInviteCheck( void );		

	bool GetPath();
	BOOL SendChatTime( int nTime );
	BOOL CheckPosition(CARDPOS *pos);	
	BOOL MouseIn( int X , int Y , int X2 , int Y2 );
	BOOL SendBtnTime( CDXButton *Btn , int nTime , BOOL bWavePlay = TRUE );

	void SetMoneyStringToSprite( char szMoneyString[50] , int nXPos , int nYPos , int nGubun, BOOL bResult = FALSE );
	void ShowUserScore( void );
	void SetGameCursor( BOOL bUserCursor , BOOL bClick, BOOL bMouseRender = TRUE );
	void MakeNickName( int nMaxLine , char szNick[512] );
	void PutResultText( void );
	void PutTextInGame( char *szNick , char *szText , BigInt biMoneyVal = 0 , COLORREF rgb = RGB( 255 , 236 , 214 ) , BOOL bMoneyUse = TRUE , BOOL bPlus = TRUE );
	int  GetPixelByString( LPCTSTR szText );
	void KillThread(HANDLE Thread_Handle);
	void AvarTarProc( int nSlot, char *pAvaUrl,  char *pGameGubun ); 
	BOOL GetAvaFileName( char *pAvaUrl, char *pFileName );
	void AvatarPlay( int nSlot, char *pAvaUrl, char *pGameGubun, char cSex );
	void DefaultAvatarPlay( int nSlot, char *pAvaUrl, char *pGameGubun );
	void GetAvaTotUrl( char *pAvaUrl, char *pGameGubun, char *pTotUrl );
	void AvatarDelete( int nSlot );
	void ChannelAvatarPaly();
	void RenderSunDecided();
    void I64toA_Money(BigInt biMoney, char *szBuff, int nUnitCnt = 2 );
    void PutSprite( CDXSprite *pSprite, int nX, int nY, int nFrame = 0 );
    void PutSpriteBlk( CDXSprite *pSprite, int nX, int nY, int nFrame = 0 );
    void PutSprite( CDXSprite *pSprite, int nX, int nY, RECT *srcRect );
    void PutSprite( CDXSprite *pSprite, int nFrame = 0 );
    void PutSpriteBlk( CDXSprite *pSprite, int nFrame = 0 );
    void PutSpriteAlpha( CDXSprite *pSprite, int nX, int nY, int nAlpha, int nFrame = 0 );
    void DrawScale( CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame = 0 );
    void DrawScaleBlk( CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame = 0 );
    void PutSpriteAvatar( CDXSprite *aAvatarSPR, int x, int y, int nType );
    void DrawText(char *szText, int nX, int nY, COLORREF rgbColor, int nStyle, int nAlign, int nFontSize = 12 );
    void DrawMoney( CDXSprite *pSprite, int nX, int nY, BigInt nMoney, int nMark );
    void DrawNumber( CDXSprite *pSprite, int nX, int nY, int nNum, int nMarkType, int nAlign );
    void DrawMissionCompleteUser( char *szNickname );
    void DrawTransDialog( int nDlgType, int nX, int nY, int nAlpha, int nFrame = 0 ); //������ ��ȭ���ڸ� �׸���.[alpha]

    // [zoom]
    void (CMainFrame::* m_fpPutHwatuZoom)( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo );
    void PutHwatuZoom16( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo );    // 15/16 bit ����
    void PutHwatuZoom32( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo );    // 24/32 bit ����

	POINT SetPoint(LONG x, LONG y);
	
	HRESULT PutSmallHwatuImage(long nX, long nY, int nHwatuNo); // bsw 01.05
    HRESULT PutHwatuImage(long nX, long nY, int nHwatuNo, bool bScale = false, int nScaleMode = 0); // bsw 01.05
    int GDI_GetStringSize(LPCTSTR szString, LPSIZE size);
    void RenderWaitChat();
    void RenderGameChat();
    void AddWaitChatText( char *szText, COLORREF rgbTextColor, int nTextType = 0 );
    void AddGameChatText( char *szText, COLORREF rgbTextColor, int nTextType = 0 );

    CDXSoundBuffer *LoadSndFile(char *szPath, char *szFilename, int nBuffNum = 1);
    void PlaySound( CDXSoundBuffer *pSound , bool bPlayFlags = false );
    void StopSound( CDXSoundBuffer *pSound );
    void StopAllSound( void );
    void DestroySound( void );

    void WaitRoomIndexRefresh();                    // �� ����Ʈ �ε��� ����
    void WaitUserScrollBarRefresh( int nUserCnt );  // ����� ����Ʈ ��ũ�� �� ����
    void ViewRoomInfoRefresh( ush_int nRoomNo );    // ���� ���� �ִ� �� ���� ����

    // [zoom]
    void MakeCardZoomTable( sCardZoomTable **pZoomTable, int nZoomWidth, int nZoomHeight );  // ī�� Ȯ���ؼ� ��½� �ʿ��� ���̺��� �����.

    void          ReGetSprite( CDXSprite **pSprite, char *szFilename, int nW, int nH, int nFrmNum );
    CDXButton    *LoadButton(LPSTR szFilename, int nX, int nY, int nW, int nH, BOOL bMulti = FALSE, int nCount = 0, int nDirection = NONE, int nDistance = 0);
    CDXSprite    *LoadSprite(LPSTR szFilename, int nW, int nH, int nNum);
    CDXScrollBar *LoadScrollBar(LPSTR szUpBTN, LPSTR szDownBTN, LPSTR szDragBTN, RECT *pScrollBarRect,
                                int nUpDownBTNWidth, int nUpDownBTNHeight, int nDragBTNWidth, int nDragBTNHeight,
                                int nViewNum, int nMaxNum);
    void SetColorKey();
    void AdjustWinStyle();
    void ChangeVideoMode(BOOL bFullScreen);
	
	LRESULT OnUserCardPaint(WPARAM wParam, LPARAM lParam); // �ѿ��� �������� �� ������
	LRESULT OnFloorEat(WPARAM wParam, LPARAM lParam); // �ٴڿ��� ���� �� ���� �������� ������
	LRESULT OnCardUp(WPARAM wParam = NULL, LPARAM lParam = NULL);
	LRESULT OnCardThrow(WPARAM wParam, LPARAM lParam); // �� ������ ������ �� �ٴ����� ������
	LRESULT OnCardThrowFloor(WPARAM wParam, LPARAM lParam); // �� ������ ������ �� �ٴ����� ������
	LRESULT OnGetOneCard(WPARAM wParam, LPARAM lParam);	//������ ī�带 ���徿 �����´�
	LRESULT OnNineCardMove(WPARAM wParam, LPARAM lParam);	
	LRESULT OnResultOk(WPARAM wParam, LPARAM lParam);
	LRESULT OnGoStop(WPARAM wParam, LPARAM lParam);
	LRESULT OnCBCommand(WPARAM wParam, LPARAM lParam); 

	HANDLE hUserCardThread;
	void UserCardSetTime( int nSlotNo );
    void ExitGame();    // ���� ����(WM_CLOSE �޽����� ������.)
    void NickSave( char *pShortNick, char *pNickName );
    void RecoveryAvatar();  // ȭ���� ��ȯ������ ���� ǥ���ı��� �ƹ�Ÿ ����

    BOOL isInteger(CString origin);
//	BOOL isFloat(CString origin);



#ifdef CHANGEUSERCARD
    BOOL CheckChatCommand(char *szText);                            // ä�� ��ɾ� ó�� �Լ�
    void SendChangeUserCardInfo(int nCardNo1, int nCardNo2);    // ������ �ٲ� ī�� ������ ������.
#endif
	

	
public:
	//////////////////////////////////////////////////////////////////////////
	///�ͽ��÷ξ� ����
	//////////////////////////////////////////////////////////////////////////
	DECLARE_DISPATCH_MAP()
	void WindowRegistered(long lCookie);
	void WindowRevoked(long lCookie);

	void ConnectToShell();
	//void ExploClose();


public:
	//////////////////////////////////////////////////////////////////////////
	///Avatar
	//////////////////////////////////////////////////////////////////////////
	ImageEx        *m_PrevAvaImg;
	long            m_PrevAvaWidth;

	ImageEx        *m_AvaImg0;
	long            m_AvaWidth0;

	ImageEx        *m_AvaImg1;
	long            m_AvaWidth1;

	CDXSprite      *l_SPR_GifAvatar[MAX_ROOM_IN];
	CDXSprite      *l_SPR_GifWaitAvatar;

    POINT           m_sAvatarPos[MAX_ROOM_IN];      // �ƹ�Ÿ ��� ��ġ

public:
	FrameSkip m_FrameSkip;
	float m_fSkipTime;



public:
	void SetHumanVoice( int nVoiceNo );
};


DWORD WINAPI UserCardThreadProc( LPVOID lParameter ); 
DWORD WINAPI RenderThreadProc( LPVOID lParameter ); 
DWORD WINAPI AniUpdateThreadProc( LPVOID lParameter ); 


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__18F46A52_7465_47AC_A995_C47A43F681B3__INCLUDED_)
