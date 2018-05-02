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

	//BOOL bInvite; // 초대 //	   TRUE 허용 
	//BOOL bSecretMessage; // 귓속말 TRUE 허용
	//BOOL bWaveSnd; // 효과음	   TRUE 허용
	//BOOL bMessageAgree; // 메세지  TRUE 허용

	BOOL bCheck[4];

	int nVoice; // 0 하소연. 1 남자 . 2 여자 . 3 . 무음 ( 초기 남자 )
	
public:
	
	CONFIGBOX( BOOL btInvite = TRUE , BOOL btSecretMessage = TRUE , BOOL btWaveSnd = TRUE , BOOL btMessageAgree = TRUE , int ntVoice = 0 ); // 생성자
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
	CDXSprite *l_SPR_TurnLine[MAX_ROOM_IN]; // 깜빡이는 배경(어떤 유저가 게임을 진행할 차례인지를 표시함)
	CDXSprite *l_SPR_HwatuSelect;
	CDXSprite *l_SPR_Time;
	CDXSprite *l_SPR_LoGo;
	CDXSprite *l_SPR_WaitAvatar;
	CDXSprite *l_SPR_Start;
	CDXSprite *l_SPR_Baks;          // 광박 , 피박 , 독박들.
	CDXSprite *l_SPR_DlgBar;        // 다이얼 로그에 사용하는 바 와 공 표시
	CDXSprite *l_SPR_P_Back;        // 피 숫자 배경
	CDXSprite *l_SPR_FullWait;      // 
	CDXSprite *l_SPR_ConBox;        // 설정 창 그림 생성
	CDXSprite *l_SPR_KingFirst;     // 방장과 선 그림
	CDXSprite *l_SPR_COM;           // Com 아바타.
	CDXSprite *l_SPR_InviteBox;     // 게임 초대 박스 그림
	CDXSprite *l_SPR_InviteIcon;    // 게임 초대 박스 그림에 남자 , 여자 아이콘
	CDXSprite *l_SPR_MessageBox;    // 쪽지 창
	CDXSprite *l_SPR_Invitation;    // 초대 창 ( 초대하시겠습니까? 창 )
	CDXSprite *l_SPR_UserState;     // 마우스 오른쪽 버튼으로 유저 부분을 누르면 뜨는 창 그림
	CDXSprite *l_SPR_Message;       // 메세지 박스 그림
	CDXSprite *l_SPR_AffairNum;     // 고, 뻑,  흔듬 등의 횟수를 나타내는 이미지
	CDXSprite *l_SPR_Jumsu;         // 게이머 점수( 나 )
	CDXSprite *l_SPR_RoomFullorNot; // 방이 꽉 차있거나.. 사용 안하는 방일때..
	CDXSprite *l_SPR_ResultMessage; // 결과창에서 메세지 그림 ( 나가리 , 유저가 다 왔다. )
	CDXSprite *l_SPR_MissionOK;     // 미션 OK ( 각 슬롯에 마다 있는 미션 성공 그림 )
	CDXSprite *l_SPR_Class;         // 계급 디자인
	CDXSprite *l_SPR_MissionNext;   // 다음 기회에.. 찍어준다.
    CDXSprite *m_SPR_ResultNum[2];  // 결과 출력용 숫자
    CDXSprite *m_SPR_ExitReserv;    // 나가기 예약 이미지
    CDXSprite *m_SPR_UserListTitle[2];
    CDXSprite *m_SPR_WaitDlg;       // 대기 메시지 창
    CDXSprite *m_SPR_CheckBall;     // 방만들기에서 선택 동그라미
    CDXSprite *m_SPR_HighlightBar;  // 대기자/방유저 리스트의 하일라이트 바 이미지
    CDXSprite *m_SPR_PassDlg;       // 비번 입력창.
    CDXSprite *m_SPR_CardBlink;     // 선 결정하기 위해서 카드 뒤집을 때 카드가 깜빡이는 것을 표현하기 위해 사용
    CDXSprite *m_SPR_ChodaeOX;      // 대기실 초대 OX 이미지
    CDXSprite *m_SPR_RoomBack;      // 대기실 방 배경 이미지 // ham 2006.01.18
    CDXSprite *m_SPR_UserIcon;      // 대기실 방 배경 출력용 유저 이미지(0=여자, 1=남자)

    CDXSprite *m_SPR_Arrow;         // 미션 카드를 누가 먹었는지 표시하기 위한 화살표 이미지
    CDXSprite *m_SPR_MissionDlg;    // 미션 대화상자
    CDXSprite *l_SPR_MComplete;     // 미션 컴플릿 그림
    CDXSprite *m_SPR_SelSunText;    // 선 결정하는 화면에서 출력하는 텍스트 이미지
    CDXSprite *m_SPR_SelSunTextBack;// 선 결정하는 화면에서 출력하는 텍스트의 배경 이미지(반투명)

    CDXSprite *l_SPR_ResultDojang;  // 결과창 도장 찍기..
	RECT       l_rDojangRect[2];    // 도장 렉트
	int        m_nDojangCount;
	int        m_nDojangShow;
	BOOL       m_bDojangSound;
	void       DrawResultDojang( int nType );   // 1 번 미션 , 2번 대박 , 3번 총통

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
	CDXButton *l_ConfigBTN;         // 01.13 설정 버튼( 게임룸 )
	CDXButton *l_Exit2BTN;          // 01.14 나가기 버튼 ( 게임룸 )
	CDXButton *l_ConfigOkBTN;       // 01.14 결과 확인 버튼( 게임 룸 )
	CDXButton *l_WaitGamerBTN;      // hy 04.01.16 대기실 사람 버튼( 대기실 , 멀티 )
	CDXButton *l_ResultOKBTN;       // 결과창 OK 버튼
	CDXButton *l_InviteBTN;         // 게임 초대 버튼
	CDXButton *l_InviteOKBTN;       // 게임 초대 박스에서 OK 버튼
    CDXButton *l_InviteCancelBTN;   // 게임 초대 박스에서 Cancel 버튼
    CDXButton *l_RecvChodaeOkBTN;   // 초대 창 ( 초대하시겠습니까? 창 & 초대 거절 창 ) 확인 버튼
    CDXButton *l_RecvChodaeCancelBTN;  // 초대 창 ( 초대하시겠습니까? ); 취소 버튼
	CDXButton *l_MemoSendBTN;       // 쪽지 창 보내기 버튼
    CDXButton *l_MemoCancelBTN;     // 쪽지 보내기 취소
	CDXButton *l_UserStateBTN;      // ↑ 내용 버튼
	CDXButton *l_MessageOKBTN;      // 메세지 박스 OK 버튼
    CDXButton *l_CaptureBTN;        // 화면 캡처 버튼
    CDXButton *m_RoomTitleBTN;      // 방 목록 보여주는 버튼
    CDXButton *m_PassDlgOkBTN;      // 패스워드 입력 창 OK 버튼
    CDXButton *m_PassDlgCancelBTN;  // 패스워드 입력 창 Cancel 버튼

    CDXButton *m_MakeRoomBTN[9];    // 방만들기 버튼 9개 // ham 2006.01.18
    CDXButton *m_JoinBTN[9];        // 방 입장 버튼 9개 // ham 2006.01.18

    CDXScrollBar *m_SCB_WaitChat;               // 대기실 채팅창 스크롤 바
    CDXScrollBar *m_SCB_WaitUser;               // 대기자 리스트 스크롤 바
    CDXScrollBar *m_SCB_WaitRoom;               // 방 리스트 스크롤 바
   	CDXScrollBar *m_SCB_GameChat;               // 방 채팅창 스크롤 바
    CDXScrollBar *m_SCB_Invite;                 // 초대창의 스크롤 바

    CDXSurface *m_SFC_DialogBack;    // 반투명 효과를 위해 임시 버퍼로 사용할 스프라이트[alpha]

    CDXIME *m_IME_WaitChat;         // 대기실 채팅창 입력용 IME
    CDXIME *m_IME_GameChat;         // 게임화면 채팅창 입력용 IME
    CDXIME *m_IME_InputPass;        // 비밀방 비밀번호 입력용 IME
    CDXIME *m_IME_InputRoomTitle;   // 방만들기에서 방 제목 입력용 IME
    CDXIME *m_IME_RoomInPassBox;    // 비밀방 들어갈 때의 비밀번호 입력창
    CDXIME *m_IME_Memo;             // 쪽지 입력용 IME

public:
	int l_nSecPos;
	
	int l_nDrawEventType, l_nDrawEventSlotNo, l_nDrawEventFrameCnt, l_nDrawEventWaitTime;
	int l_nDialogType, l_nPercentage, l_nWaitTime, l_nMouseOverCardNo;
	int l_nAniEventType;
	int l_EventTime;
	int l_nNameNo; // 방 제목 번호
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
	int m_nSunRenderDecided;	//선 결정된 것 보여지는 시간
	int m_nSunSetRender;		//선 결정하는 시간.
	int l_nDialogResultFrameCnt, l_nDialogResultWaitTime;
	int l_nDialgType;
	int l_nDlgMode;
	int l_nPVal[MAX_ROOM_IN];  // 현재 먹은 피의 갯수 ( 피의 숫자 출력을 위함 ) 
	int l_nPX[MAX_ROOM_IN];    // 피의 마지막 X 좌표. ( 피의 숫자 출력을 위함 )
	int l_nPY[MAX_ROOM_IN];    // 피의 마지막 Y 좌표. ( 피의 숫자 출력을 위함 )
	int l_mX, l_mY, l_sX, l_sY, l_cX, l_cY; // 마우스 위치 저장용 변수들
	int nTempTic1, nTempTic2;
	int nRoomX, nRoomY;         // 방의 X , Y 좌표
	int CurT1, CurT2, CurTime;
	int nChatTime;              // 채팅 막는 시간
	int l_nViewRoomMessage;     // 몇번 방이 눌렀는지 본다.
	int l_nViewRoomUser;        // 몇번 방의 유저가 눌렀는지 본다.
	int l_nViewWaitUser;        // 선택된 대기자 인덱스(번호)
	int l_nOldViewRoomMessage;
	int l_nOldViewRoomUser;     // 몇번 방의 유저가 눌렀는지 본다.
	int l_nOldViewWaitUser;     // 대기실의 유저가 눌렸는지 본다.
	int l_nRoomUserNo;          // 방의 유저 정보 알아내기.
	int l_nCurRoomUser;         // 방의 유저 정보 알아내기
	int l_nWaitUserNo;          // 대기실의 유저 정보 알아내기
	int l_nPageStart;           // 내가 보고 있는 페이지의 시작 번호
	int l_nWaitUserCurCnt;      // 대기실 현재 사람수
	int nDegree;                // 몇번만에 움직일 것인가.
	int g_nPuzzleNo;
	int l_nScrollSelNo;         // 스크롤에서 선택한 곳의 자료 번호
	int l_nMBoxTempX , l_nMBoxTempY;
	int l_nMBoxXPos, l_nMBoxYPos;
	int l_nRButtonWaitUserNo;
	int l_TempMX , l_TempMY ;
	int l_nInviteTempX, l_nInviteTempY;
	int l_nInviteXPos, l_nInviteYPos; 
	int l_nWaitViewPageNo;
	int l_nInviteRoomNo;        // 초대시 저장할 방 번호
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
    int m_nHighlightBarNum;         // 대기자 리스트 선택 번호
    int m_nUserListTitleNum;        // 유저 리스트 타이틀 이미지 번호(0 = 대기자 목록, 1 = 방 유저 목록)
    int m_nSelectedRoomTitleList;   // 방만들기 창 - 방 리스트 중 선택된 방이름의 번호(없으면 -1)
    int m_nHwatuSelectCnt;          // 먹을 카드 위에 마우스가 있는경우 먹을카드 표시 이미지 에니에미션을 위해 사용
    int m_nHwatuSelectFrm;          // 먹을 카드위에 표시되는 이미지의 프레임 번호
    int m_nCardBlinkCnt;            // 선 정할 때의 카드 주위를 깜빡이게 하는데 사용되는 카운터
    int m_nCardBlinkFrm;            // 선 정할 때의 카드 주의를 깜빡이게 하는데 사용되는 프레임 변수

	HANDLE  m_hSocketThread;
	HANDLE  m_hQThread;
	HANDLE m_hRenderThread;
	HANDLE m_hAniUpdateThread;

	BOOL l_bDrawEvent, l_bShowDialog, l_bShowResult;        //l_bWaitDialog;
	BOOL l_bTimeEvent;
	BOOL l_bShowRoomOpen;                                   // ▼ 버튼 눌렸나 검사
	BOOL l_bFullScreen;
	BOOL l_bActive;
	
	BOOL l_bMoveDlg;
    BOOL l_bMouseDown;
	BOOL l_bShowStartBTN;
	BOOL l_bShowBombLine;
	BOOL l_bShowTurnLine;
	
	BOOL SetCur;
	BOOL l_bShowConfigBox;                                  // 설정 창을 띄웠는가.
	BOOL l_bShowUserState;                                  // 대기실의 유저 정보를 보낼지 , 방의 유저 정보를 보낼지 결정한다
	BOOL l_bShowInviteBox;                                  // 게임 초대 박스를 열고 닫는 BOOL 변수
	BOOL l_bShowMessageBox , l_bMBoxMoveDlg ;
	BOOL l_bMessageSend;                                    // 쪽지를 보내는건지 받는 건지 체크 // TRUE = 보낸다. FALSE = 받는다.
	BOOL l_bUserState;
	BOOL l_bRButtonDown;
	BOOL l_bShowChodaeBox;                                  // 이게 TRUE 이면 초대박스를 띄운다.
	BOOL l_bChodaeDlg;                                      // 마우스로 초대창을 움직인다. = TRUE; ( Drag 효과 )
	BOOL m_bActive;
	BOOL l_bStartFadeOut[MAX_ROOM_IN];
	BOOL l_bShowMission;                                    // 미션 출력 여부
	BOOL l_bShowMakeRoomDlg;
	BOOL m_bMouseEvent;
	BOOL m_bMouseRender;
	BOOL m_UserTimerThread;
	BOOL m_bSunSet;
	BOOL m_bSunSetRender;		                            //10장 중에 선택하기위한 10장 뿌리는것
	BOOL m_bSunSetChoice;		                            //한번만 선택하기 위한 체크
	BOOL m_bRenderSun;			                            //선 결정나고 나서 결정된 선 뿌려주는것.
    BOOL m_bShowExitReservIcon[MAX_ROOM_IN];                // 나가기 예약 아이콘을 보여줄 지를 결정
    BOOL m_bShowRoomTitleList;                              // 방만들기 창 - 방 타이틀 리스트를 보여줄지를 결정
    BOOL m_bPassRoomChk;                                    // 비밀방 패스워드 체크
    BOOL m_bPassDlg;                                        // 패스워드 입력창을 띠운다.

	RECT ComRect;                                           // 컴퓨터 렉트
	RECT l_BakRect[3][1];
	RECT ResultDataRect[5];
	RECT FullWaitRect;
	RECT CheckRect;
	RECT KingFirstRect;                                     // 방장과 선 그림 렉트
	RECT SelRect;                                           // 셀렉션 버튼 렉트 1개
	RECT AvatarRect;
	RECT ResultPlusRect;                                    // 결과 창에서 오, 피, 광, 십, 고 를 출력할때 쓰는 렉트
	RECT l_StartBTNRect;

	DWORD dwTurnFrameCnt;

    DWORD m_rgbMakeRoomTitleListBorder;                     // 방만들기 대화상자의 방제목 리스트 테두리 색깔
    DWORD m_rgbMakeRoomTitleListBar;                        // 방만들기 대화상자의 방제목 리스트 선택 바 색깔

	POINT l_TempMousePos;                                   // 임시 마우스  위치
	POINT l_CurMousePos;                                    // 현재 마우스 위치
	POINT l_CardPackPos;                                    // 카드 팩 위치
	POINT l_UserCardPos[MAX_ROOM_IN][10];              // 사용자 패 위치
	POINT l_EatCardPos[MAX_ROOM_IN][9];                // 먹은 패 놓을 위치
	POINT l_StartPos;
	POINT l_point;

	char l_szRoomName[255] , l_szTempRoom[80];              // 타이틀 이름
	char g_szUserImage[MAX_ROOM_IN][MAX_IMAGE_LEN];    // 유저가 가지고 있는 그림 ( 세명 )( 8 바이트 )
	char l_szMessageSendUserNick[MAX_NICK_LENGTH];          // 보낸 유저 NickName
	char l_szMessageRecieveUserNick[MAX_NICK_LENGTH];       // 받는 유저 NickName
	char l_szGetMessageData[MAX_CHATMSG];                   // 쪽지 한말 정보( 상대방이 쓴 글)
	char l_szMessageData[MAX_CHATMSG];                      // 쪽지 할말 정보
	char l_szMyNick[MAX_NICK_LENGTH];                       // 내 닉네임
	char l_szInviteRoomPass[MAX_ROOM_PASSWORD];             // 초대시 저장할 비밀번호
	char l_szInviteMsg[MAX_CHATMSG];                        // 초대시 저장할 메세지
	char l_szTemp[MAX_ROOM_IN][10];
	char l_szTempText[255];
	char m_szBannerIp[255];
    char m_szTempText[256];
    char m_szPass[10];                                      // 방 패스워드 저장

    CARDPOS l_CardPos[53];
	CARDPOS m_szSunPackPos[MAX_SUNCARD_CNT];
	POINT m_szSunPackPoint[MAX_SUNCARD_CNT];
	BYTE m_szSunPack[MAX_SUNCARD_CNT];

    POINT m_sExitResrv[MAX_ROOM_IN];                        // "나가기 예약" 이미지 출력 좌표
    POINT m_sSunMarkPos[MAX_ROOM_IN];                       // "선" 표시 위치 
    POINT m_sBangJangMarkPos[MAX_ROOM_IN];                  // "방장" 표시 위치
    POINT m_sUserScorePos[MAX_ROOM_IN];                     // 게임 진행 화면 - 유저 점수 출력 위치
    POINT m_sGoCntPos[MAX_ROOM_IN];                         // 게임 진행 화면 - 고 횟수 출력 위치
    POINT m_sShakeCntPos[MAX_ROOM_IN];                      // 게임 진행 화면 - 흔듦 횟수 출력 위치
    POINT m_sPPuckCntPos[MAX_ROOM_IN];                      // 게임 진행 화면 - 뻑 횟수 출력 위치
    POINT m_sTurnLinePos[MAX_ROOM_IN];                      // 감빡이는 유저 배경 이미지 출력 위치
    POINT m_sTimerPos[MAX_ROOM_IN];                         // 타이머 위치
    POINT m_sEventPos[MAX_ROOM_IN];                         // 판쓸~, 한장씩, 다닥 등의 이벤트 출력 위치
    POINT m_sNickNamePos[MAX_ROOM_IN];                      // 닉네임 출력 위치
    POINT m_sClassPos[MAX_ROOM_IN];                         // 계급 출력 위치
    POINT m_sUserMoneyPos[MAX_ROOM_IN];                     // 유저 머니 출력 위치
    POINT m_sWinLoseMoneyPos[MAX_ROOM_IN];                  // 따고 잃은 금액 출력 위치
    POINT m_sMissionOkPos[MAX_ROOM_IN];                     // 미션 성공 아이콘 출력 위치

	CRect m_rcWnd;

    sPrevDlgInfo    m_sPrevDlgInfo;                         //[alpha] 이전의 반투명 대화상자의 정보(출력여부) 저장
	sCreateRoom     l_sCreateRoom;
	sRoomIn         l_sRoomInData;                          // 접속할 방의 구조체
	sINVITECHECK    sInviteCheck[MAX_GAMEVIEWWAIT];         // 초대 박스에 체크 정보들
    sMissionDlgInfo m_sMissionDlgInfo;                      // 미션창 정보

	int m_nMakeRoomMoneyNo;
	POINT l_sMakeRoomMoney[ROOM_DIVI_CNT];	                //방을 만들때...얼마인가를 찍을 좌표.

	CDXTime l_Time;
	CAnimation l_AniStack;
	CDXBtnTime BtnTime[MAX_BTNTIMECOUNT];
	CONFIGBOX *l_ConfigBox; // 설정 창 생성
	GAMEMESSAGE *l_Message; // 알람 메시지
    BUTTON_STATUS l_nUserStatus[12];
	BUTTON_STATUS l_nUserStatus2[MAX_GAMEVIEWWAIT];
	
	//CHttpView* m_pHtmlView;

    CTextList *m_pWaitChat;
    CTextList *m_pGameChat;

    CScrollText *m_pScrollNotice;

    // [zoom]
    // 카드 확대 테이블
    sCardZoomTable *m_sSmallZoomTable;
    sCardZoomTable *m_sBigZoomTable;

public:

	void AnimationUpdate();
	void Render();
	void LoadFile();  //현재실행되고 있는 전체 파일명을 가져온다.
	void LoadData();
    void SoundLoading();    // 사운드 로딩
	
	void InitCardPostion();
	void RecieveInvite( int nRoomNo , BigInt biRoomMoney, char *pRoomPass , char *pInviteMsg );
	void RecieveMemo( char *szSendUserNick , char *szMessage ); // 메모 받기
	
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
	void OnBtnStart(); // 시작 버튼 클릭시
	void MakeRoom( BOOL bSecret ); //true 비밀
	void OnReGames( bool bMasterMode, int nSlotNo );
	void EndOtherGameUser();  //게임중에 다른놈이 나가면 이놈의 정보를 저장했다가 게임이 끝나고 여기에서 처리해준다.
	void ButtonUpCheck();
	void ButtonDownCheck(int mX, int mY);
	void CheckButtons(int mX, int mY);
	void SortUserEatCard(int nSlotNo);
	void RepaintUserCard(int nUserNo);
	void AniEvent(int nSlotNo, int nType);
	void GetMouseOverCardNo();
	void ShakeFloor( void );
	void DrawMessageBox( int X , int Y ); // 메세지(쪽지)창 보내기
	void DrawUserState( int X , int Y );
	void RButtonDownCheck( int MouseX , int MouseY );
	void RButtonUpCheck( void );
	void RButtonEvent( void );
	void RButtonDownCheckInGame( int MouseX , int MouseY );
	void RButtonUpCheckInGame( void );
	void RButtonEventInGame( void );
	void DrawInvite( int X , int Y ); // 초대 창 보내기
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
    void GetFillRectColor( UINT bpp );                      // FillRect()에서 사용하는 색을 현재 화면 모드에 맞게 선택한다.

    int GetSelUserIdx();       // 대기실 오른쪽 유저 리스트에서 마우스 클릭한 위치를 구해서 리턴한다.
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
    void DrawTransDialog( int nDlgType, int nX, int nY, int nAlpha, int nFrame = 0 ); //반투명 대화상자를 그린다.[alpha]

    // [zoom]
    void (CMainFrame::* m_fpPutHwatuZoom)( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo );
    void PutHwatuZoom16( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo );    // 15/16 bit 모드용
    void PutHwatuZoom32( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo );    // 24/32 bit 모드용

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

    void WaitRoomIndexRefresh();                    // 방 리스트 인덱스 갱신
    void WaitUserScrollBarRefresh( int nUserCnt );  // 대기자 리스트 스크롤 바 갱신
    void ViewRoomInfoRefresh( ush_int nRoomNo );    // 현재 보고 있는 방 정보 갱신

    // [zoom]
    void MakeCardZoomTable( sCardZoomTable **pZoomTable, int nZoomWidth, int nZoomHeight );  // 카드 확대해서 출력시 필요한 테이블을 만든다.

    void          ReGetSprite( CDXSprite **pSprite, char *szFilename, int nW, int nH, int nFrmNum );
    CDXButton    *LoadButton(LPSTR szFilename, int nX, int nY, int nW, int nH, BOOL bMulti = FALSE, int nCount = 0, int nDirection = NONE, int nDistance = 0);
    CDXSprite    *LoadSprite(LPSTR szFilename, int nW, int nH, int nNum);
    CDXScrollBar *LoadScrollBar(LPSTR szUpBTN, LPSTR szDownBTN, LPSTR szDragBTN, RECT *pScrollBarRect,
                                int nUpDownBTNWidth, int nUpDownBTNHeight, int nDragBTNWidth, int nDragBTNHeight,
                                int nViewNum, int nMaxNum);
    void SetColorKey();
    void AdjustWinStyle();
    void ChangeVideoMode(BOOL bFullScreen);
	
	LRESULT OnUserCardPaint(WPARAM wParam, LPARAM lParam); // 팩에서 유저한테 패 날리기
	LRESULT OnFloorEat(WPARAM wParam, LPARAM lParam); // 바닥에서 먹은 패 먹은 유저에게 날리기
	LRESULT OnCardUp(WPARAM wParam = NULL, LPARAM lParam = NULL);
	LRESULT OnCardThrow(WPARAM wParam, LPARAM lParam); // 각 유저가 선택한 패 바닥으로 날리기
	LRESULT OnCardThrowFloor(WPARAM wParam, LPARAM lParam); // 각 유저가 선택한 패 바닥으로 날리기
	LRESULT OnGetOneCard(WPARAM wParam, LPARAM lParam);	//유저의 카드를 한장씩 가져온다
	LRESULT OnNineCardMove(WPARAM wParam, LPARAM lParam);	
	LRESULT OnResultOk(WPARAM wParam, LPARAM lParam);
	LRESULT OnGoStop(WPARAM wParam, LPARAM lParam);
	LRESULT OnCBCommand(WPARAM wParam, LPARAM lParam); 

	HANDLE hUserCardThread;
	void UserCardSetTime( int nSlotNo );
    void ExitGame();    // 게임 종료(WM_CLOSE 메시지를 보낸다.)
    void NickSave( char *pShortNick, char *pNickName );
    void RecoveryAvatar();  // 화면모드 전환등으로 인한 표면파괴시 아바타 복구

    BOOL isInteger(CString origin);
//	BOOL isFloat(CString origin);



#ifdef CHANGEUSERCARD
    BOOL CheckChatCommand(char *szText);                            // 채팅 명령어 처리 함수
    void SendChangeUserCardInfo(int nCardNo1, int nCardNo2);    // 서버에 바꿀 카드 정보를 보낸다.
#endif
	

	
public:
	//////////////////////////////////////////////////////////////////////////
	///익스플로어 제어
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

    POINT           m_sAvatarPos[MAX_ROOM_IN];      // 아바타 출력 위치

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
