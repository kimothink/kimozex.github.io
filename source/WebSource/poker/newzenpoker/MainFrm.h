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

	//BOOL bInvite;         // 초대     TRUE 허용 
	//BOOL bSecretMessage;  // 귓속말   TRUE 허용
	//BOOL bWaveSnd;        // 효과음	TRUE 허용
	//BOOL bMessageAgree;   // 메세지   TRUE 허용
	BOOL bCheck[4];

	int nVoice;             //1 남자 . 2 여자 . 3 . 무음 ( 초기 남자 )
	
public:
	
	CONFIGBOX( BOOL btInvite = TRUE , BOOL btSecretMessage = TRUE , BOOL btWaveSnd = TRUE , BOOL btMessageAgree = TRUE , int ntVoice = 0 ); // 생성자
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
	int m_mX, m_mY;                 // 마우스 위치 저장용 변수들
	int m_nChatTime;                // 채팅 막는 시간
	int m_nDegree;                  // 몇번만에 움직일 것인가.
	int m_nScrollY;                 // 스크롤 바 버튼의 Y 좌표
	int m_nScrollNo;                // 스크롤 번호 맨위가 0 번
	int m_nCurMashine;              // 현재 채널창에서 몇번째 마당을 가르키는가.
	int m_nCurChannelNo;
	int m_nWaitUserCurCnt;          // 대기실 현재 사람수

	int m_nSelectedRoomNum;         // 선택된 방 번호 (선택되지 않았으면 -1 )
    int m_nViewUserIdx;             // 선택된 유저의 인덱스 번호(대기자, 방유저 모두에 사용)
    int m_nViewUserNo;              // 선택된 유저의 고유 번호(대기자, 방유저 모두에 사용)

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
	int m_nWinnerMarkPos;           //이겼을 때의 이미지의 프레임 번호
    int m_nWinnerMarkPosCnt;        //이겼을 때의 에니메이션 시간간격 계산을 위한 카운터
	int m_nEndingUserTime;	        //한명당 결과보기 시간
	int m_nEndingShowTime;	        //완전 결과창 보기 시간.
	int m_nEndingUserCurTime;	    //현재 엔딩 보여지는 유저 시간.
	int m_nEndingUserCurSlot;	    //현재 엔딩 보여지는 유저 슬롯.
	int m_nUserCallTime;	        //유저 콜,다이 등 선택시간.
	int m_nUserCallSlotNo;	        //유저 콜,다이를 해야할 슬롯.
	int m_nTimePos;			        //유저 콜,다이 타임 그림 위치.
	int m_nJackPotPos;		        //잭팟 숫자찍기.
	int m_nJackPotLen;		        //잭팟 -> 문자로 변화후 길이.
	int m_nJackPotStopCnt;	        //잭팟 길이 만큼 돌면 잭팟 애니를 멈춘다.
	int m_nWaitViewPageNo;	        //초대 페이지 No
	int m_nInviteRoomNo;	        //초대된 방번호.
	int m_nInviteXPos;
	int m_nInviteYPos;
	int m_nInviteTempX;
	int m_nInviteTempY;
	int m_cX;
	int m_cY;
	int m_nUserCallDefineTime;	    //유저 콜 디파인 타임.
	int m_nTimePosCnt;
	int m_nShowUserSlot;		    //게임방에서 유저인포보기.
	int m_nSelectionCnt;		    //깜빡이는 속도.
    int m_nUserListTitleNum;        // 유저 리스트 타이틀 이미지 번호(0 = 대기자 목록, 1 = 방 유저 목록)
    int m_nHighlightBarNum;         // 오른쪽 유저 리스트의 칸 번호
    int m_nSelectedRoomTitleList;   // 방만들기 창 - 방 리스트 중 선택된 방이름의 번호(없으면 -1)
    int m_nCardOption;              // 패옵션 - CO_NORMAL_MODE = 일반 방식, CO_CHOICE_MODE = 초이스 방식
    int m_nJackpotBackFrm;          // 잭팟 배경 이미지 프레임 번호
    int m_nJackpotBackFrmCnt;       // 잭팟 배경 이미지 프레임 번호를 바꾸기 위한 시간계산 카운터
	int m_nTemp;

    unsigned m_dwAniUpdateThread;

	//사용자가 콜할때 버튼위에 돈표시해주기위해서.
	BigInt m_biCall;
	BigInt m_biHalf;
	BigInt m_biBing;
	BigInt m_biDdaDang;
	BigInt m_biCheck;
	BigInt m_biDie;	
	BigInt m_biFull;	
	BigInt m_biQuater;	
	BigInt m_biInviteRoomMoney;		//초대된 방머니.
    BigInt m_biMakeRoomMoneyNo;	    //방만들기창에서 판돈과 관련 몇번째를 가르키고 있는가.

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
	BOOL m_bShowStartBTN;           //시작 버튼 떠있는 중..
	BOOL m_bCardChoice;             //보여질 카드 고르는 중..
	BOOL m_bCardDump;		        //4장중 한장 버리는 중..
	BOOL m_bShowConfigBox;
	BOOL m_bUserState;	            //유저의 정보를(오른쪽마우스버튼눌었을때보여줄것인지)
	BOOL m_bMessageSend;            // 쪽지를 보내는건지 받는 건지 체크 // TRUE = 보낸다. FALSE = 받는다.
	BOOL m_bMouseEvent;	            //MFC 콘트롤 위에 마우스가 가면 마우스를 먹지 않게 하려고.
	BOOL m_bMouseRender;            //MFC 콘트롤 위에 마우스가 가면 마우스를 그리지 않게 하려고.
	BOOL m_bStartInit;
	BOOL m_bEndding;	            //게임 결과 시작
	BOOL m_bEnddingShow;            //게임 결과판 보기
    BOOL m_bGameInit;               //게임을 AniUpdateThread에서 초기화 시킨다.
	BOOL m_bUserCallTime;	        //유저 콜,다이 등 선택 시간.
	BOOL m_bWinnerCpoy;	            //승리자의 정보를 한번만 카피하기 위해서.
	BOOL m_bRenderPanMoney;	
	BOOL m_bJackPot;	            //유저가 잭팟을 터트리면 나오는 그림.
	BOOL m_bRoomJackPotRender;	    //게임을 시작하면 잭팟이 돌아가는.
	BOOL m_bShowInviteBox;	        //게임중 초대창을 띠운다.
	BOOL m_bShowChodaeBox;	        //초대박스를 띠운다.
	BOOL m_bChodaeDlg;		        //박스 상단 잡았을때.
	BOOL m_bShowUserInfo;	        //게임방에서 유저 인포보기.
	BOOL m_bSelection;		        //유저 깜빡이게 하는거.
	BOOL m_bSelectionStart;	        //유저 깜빡이기 시작.
	BOOL m_bBeforeFull;		        //앞 유저가 풀이면 레이스 버튼 모두 풀꺼질때 꺼짐
    BOOL m_bPassRoomChk;            // 비밀방 패스워드 체크     // [HAM+] 2005.08.19 [HAM-]
    BOOL m_bPassDlg;                // 패스워드 입력창을 띠운다.
    BOOL m_bShowRoomTitleList;      // 방만들기 창 - 방 타이틀 리스트를 보여줄지를 결정
    BOOL m_bMoveHiddenCard;         // 히든 카드가 마우스 드래그로 이동중인지를 나타낸다.

	char m_szMessageSendUserNick[MAX_NICK_LENGTH];      // 보낸 유저 NickName
	char m_szMessageData[MAX_CHATMSG];                  // 쪽지 할말 정보
	char m_szMessageRecieveUserNick[MAX_NICK_LENGTH];   // 받는 유저 NickName
	char m_szGetMessageData[MAX_CHATMSG];               // 쪽지 한말 정보( 상대방이 쓴 글)
	char m_szInviteRoomPass[MAX_ROOM_PASSWORD];         // 초대시 저장할 비밀번호
	char m_szInviteMsg[MAX_CHATMSG];                    // 초대시 저장할 메세지
    char m_szPass[10];
	char m_szJackPotMoney[25];
	char m_szTempText[255];
	char m_szTempText2[255];
    char m_szUserAddMoney[MAX_ROOM_IN][256];            // 유저들이 걸었던 금액의 문자열
	
	HBRUSH m_Brush[4];
	COLORREF m_ColControl[4];                           // 컨트롤들의 색상..

	BUTTON_STATUS m_nUserStatus[10];

    DWORD m_rgbMakeRoomTitleListBorder;                     // 방만들기 대화상자의 방제목 리스트 테두리 색깔
    DWORD m_rgbMakeRoomTitleListBar;                        // 방만들기 대화상자의 방제목 리스트 선택 바 색깔
    DWORD m_rgbGray;                                        // 회색

	POINT m_sMakeRoomMoneyPoint[ROOM_DIVI_CNT];         //방만들기 할때 금액을 찍는 위치.
  	POINT m_JakcPotMathPoint[15];
	POINT m_JackPotCommaPoint[4];

	RECT m_rCheckRect;
	RECT m_SelRect;                                     // 셀렉션 버튼 렉트 1개
	RECT m_StartBTNRect;

	//대기실 아바타.
	ImageEx			*m_WaitAvaImg;
	CDXSprite		*m_SPR_WaitAvatar;

	//방아바타
	ImageEx			*m_GameAvaImg[MAX_ROOM_IN];
	CDXSprite		*m_SPR_GameAvatar[MAX_ROOM_IN];

	HANDLE m_hSocketThread;
	HANDLE m_hRenderThread;
	HANDLE m_hAniUpdateThread;
    HANDLE m_hGameInitEvent;
	
	CDXSprite *m_SPR_Money;                 // 결과 표시의 돈
	CDXSprite *m_SPR_MessageBox;            // 쪽지 창
	CDXSprite *m_SPR_Message;               // 메세지 박스 그림
	CDXSprite *m_SPR_LoGo;
	CDXSprite *m_SPR_Cursor;
	CDXSprite *m_SPR_Connting;
	CDXSprite *m_SPR_ConBox;
	CDXSprite *m_SPR_WaitBase;
	CDXSprite *m_SPR_MakeRoom;
	CDXSprite *m_SPR_Class;                 // 계급 디자인
	CDXSprite *m_SPR_FullWait;
	CDXSprite *m_SRP_SexIcon;
	CDXSprite *m_SPR_GameBase;
	CDXSprite *m_SPR_Sun;
	CDXSprite *m_SPR_BangJang;
	CDXSprite *m_SPR_BetType_Big;
	CDXSprite *m_SPR_BetType_Small;
	CDXSprite *m_SPR_USERTIME;
    CDXSprite *m_SPR_UserListTitle[2];      // [0] = 대기자 리스트 타이틀이미지, [1] = 방 유저 리스트 타이틀 이미지
    CDXSprite *m_SPR_HighlightBar;
    CDXSprite *m_SPR_SelRoomBar;            // 선택된 방 표시 막대기
    CDXSprite *m_SPR_PassDlg;               // 비밀방 들어갈 때 패스워드 입력 창
    CDXSprite *m_SPR_ChodaeOX;              // 대기실 유저 리스트 유저 초대가능 여부 표시

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
    CDXSprite *m_SPR_CheckBall;                 // 방만들기에서 금액 체크하는 동그라미
    CDXSprite *m_SPR_SelectUser;                // 선택된 유저 표시하는 이미지
    CDXSprite *m_SPR_BetMoneyBack;              // 버튼 베팅 금액 표시용 배경 이미지
    CDXSprite *m_SPR_Arrow;                     // 히든카드 위에 표시되는 화살표

	CDXButton *m_MessageOKBTN;                  // 메세지 박스 OK 버튼
	CDXButton *m_FullBTN;                       // 풀스크린 버튼 ( 채널 )
	CDXButton *m_MaxBTN;
	CDXButton *m_MaxBTN2;
	CDXButton *m_ExitBTN;
	CDXButton *m_ConfigBTN;
	CDXButton *m_Exit2BTN;
	CDXButton *m_MinBTN;
	CDXButton *m_OkBTN;
    CDXButton *m_CaptureBTN;                    // 캡처 버튼

	CDXButton *m_MakeRoomBTN;
	CDXButton *m_JoinBTN;
	CDXButton *m_QuickJoinBTN;
	CDXButton *m_CancelBTN;
    CDXButton *m_RoomTitleBTN;                  // 방 목록 보여주는 버튼
    CDXButton *m_MakeRoomOkBTN;                 // 방만들기 창 - '만들기' 버튼
    CDXButton *m_MakeRoomCancelBTN;             // 방만들기 창 - '취소' 버튼

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
	CDXButton *m_InviteOKBTN;       // 게임 초대 박스에서 초대 버튼
    CDXButton *m_InviteCancelBTN;   // 게임 초대 박스에서 Cancel 버튼
	CDXButton *m_ChodaeYesBTN;
	CDXButton *m_ChodaeNoBTN;
	CDXButton *m_UserCloseBTN;
	CDXButton *m_GetOutBTN[MAX_ROOM_IN];
	CDXButton *m_UserStateBTN;
	CDXButton *m_MemoSendBTN;
	CDXButton *m_MemoCancelBTN;

    CDXScrollBar *m_SCB_WaitChat;               // 대기실 채팅창 스크롤 바
    CDXScrollBar *m_SCB_WaitUser;               // 대기자 리스트 스크롤 바
    CDXScrollBar *m_SCB_WaitRoom;               // 방 리스트 스크롤 바
	CDXScrollBar *m_SCB_GameChat;               // 방 채팅창 스크롤 바
    CDXScrollBar *m_SCB_Invite;                 // 초대창의 스크롤 바

	CDXBtnTime m_szBtnTime[MAX_BTNTIMECOUNT];
	
    CDXIME             *m_IME_WaitChat;         // 대기실 채팅창 입력용 IME
    CDXIME             *m_IME_GameChat;         // 게임화면 채팅창 입력용 IME
    CDXIME             *m_IME_InputPass;        // 비밀방 비밀번호 입력용 IME
    CDXIME             *m_IME_InputRoomTitle;   // 방만들기에서 방 제목 입력용 IME
    CDXIME             *m_IME_Memo;             // 쪽지 입력용 IME
    CDXIME             *m_IME_RoomInPassBox;    // 비밀방 들어갈 때의 비밀번호 입력창
	
	GAMEMESSAGE        *m_Message;              // 알람 메시지
	CONFIGBOX          *m_pConfigBox;           // 설정 창 생성

    CTextList          *m_pWaitChat;            // 대기실 채팅창(텍스트 리스트)
    CTextList          *m_pGameChat;            // 게임화면 채팅창(텍스트 리스트)
    CScrollText        *m_pScrollNotice;

	CAnimation          m_CAniMation;
    //CHttpView          *m_pWaitRoomBanner;      // 대기실 배너
    CChipAni            m_ChipAni;              // 톨 에니메이션 스택
	
	sRoomInUserPosition m_sRoomInUserPosition[MAX_ROOM_IN];
	sINVITECHECK        m_sInviteCheck[MAX_GAMEVIEWWAIT];     // 초대 박스에 체크 정보들

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
	void RenderMakeRoom();                      // 방만들기 창 렌더링
	void RecieveInvite( int nRoomNo , BigInt biRoomMoney, char *pRoomPass , char *pInviteMsg );
	void MakeRoom( BOOL bSecret );              //true 비밀
    void OpenMakeRoomDlg();                     // 방만들기 대화상자를 띄운다.
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
    void NickSave( char *pShortNick, char *pNickName ); // 유저 님네임이 10바이트를 넘으면 줄여서 저장한다.
	void NickSavePrint( char *pNickName, int x, int y , COLORREF rgbCol = RGB(255, 255, 255) ); //유저 닉네임 10바이트가 넘으면 줄여찍는다.
	void AnimationUpdate();
	void OnBtnStart();                          // 시작 버튼 클릭시
	int  FindSunArrayNo();
	void UserCardAdd( BYTE nCardNo , int nUserArrayNo, BOOL bAniMation = TRUE ); // 팩에서 유저한테 패 날리기
	void PutCardImage( int x, int y, BYTE nCardNo, BOOL bUp = TRUE );
	BOOL CheckPosition( sUserCardInfo *pos );   // pos->NewPos로 pos->Pos를 이동
    BOOL CheckChipPosition( sChipInfo *pos );
	void SetUserCall( sGameCurUser asGameCurUser, int nUserArrayNo );
	void AllCallBtnUp();
	void JokboShow( BYTE *pJokbo, char *pResult );
	void UserSelectView( int nSlotNo, char cUserSelect, BOOL bNetWorkCall = FALSE );	//자신이 호출했냐.네트워크가햇냐.
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
    void RenderPassDlg();                           //패스워드 입력창을 띠운다.
    void RenderWaitChat();                          // 대기실 채팅창을 그린다.
    void RenderGameChat();                          // 게임화면 채팅창을 그린다.
    void RenderWaitUser( int nUserNum );            // 자세한 대기자 정보를 출력한다.
    void RenderRoomUser( int nUserNum );            // 자세한 방 유저 정보를 출력한다.
    void RenderThrowChip();                         // 유저가 던진 톨을 그린다.
    void PanMoneyReFresh();                         // 톨 슬라이딩이 끝난후 고정 톨을 다시 그려준다.
    void PutSprite( CDXSprite *pSprite, int nX, int nY, int nFrame = 0 );
    void PutSprite( CDXSprite *pSprite, int nX, int nY, RECT *srcRect );
    void PutSpriteBlk( CDXSprite *pSprite, int nX, int nY, int nFrame = 0 );
    void DrawScale(CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame = 0);
    void GetFillRectColor( UINT bpp );
    
    BOOL ScreenCapture();
    void WaitRoomIndexRefresh();                    // 대기실 출력시 사용되는 인덱스를 갱신하는 함수
    void ViewRoomInfoRefresh( ush_int nRoomNo );    // 현재 보고있는 방의 정보를 갱신한다.

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
    void ExitGame();    // 게임 종료(WM_CLOSE 메시지를 보낸다.)
    void RecoveryAvatar();  // 화면모드 전환등으로 인한 표면파괴시 아바타 복구

#ifdef CHANGEUSERCARD
    BOOL CheckChatCommand(char *szText);                            // 채팅 명령어 처리 함수
    void ChangeUserCard( int nSlotNo, sUserChange *pChgCardInfo );  // 카드 바꾸는 함수
    void SendChangeUserCardInfo(char *szParam1, char *szParam2);    // 서버에 바꿀 카드 정보를 보낸다.
    int ConvCardNum( char *szCardInfo );                            // 카드 정보 문자열을 카드 번호로 변환한다.
#endif
    
public:

    //Frame관련된 변수.
	FrameSkip m_FrameSkip;
	float m_fSkipTime;
};


DWORD WINAPI RenderThreadProc( LPVOID lParameter ); 
unsigned __stdcall AniUpdateThreadProc( LPVOID lParameter );    // ham 2005.11.29

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__6FA7B5FA_1496_4A83_9E4D_8E012DB18D1D__INCLUDED_)
