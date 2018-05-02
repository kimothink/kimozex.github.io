// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "zenpoker.h"
#include "MainFrm.h"
#include "AdminLogin.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "dtrace.h"
#include "Externvar.h"

#include "..\PokerCommonDefine\serverclientcommon.h"




static unsigned long seed;

#define	m (unsigned long)2147483647
#define	q (unsigned long)127773
#define	a (unsigned int)16807
#define	r (unsigned int)2836


#define RGB16BIT555(r, g, b)    ((((r) % 32) << 10) + (((g) % 32) << 5) + ((b) % 32))
#define RGB16BIT565(r, g, b)    ((((r) % 32) << 11) + (((g) % 64) << 5) + ((b) % 32))
#define RGB24BIT(r, g, b)       RGB((r), (g), (b))
#define RGB32BIT(r, g, b)       RGB((r), (g), (b))




/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_CLOSE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//20040504_2
//-----------------------------------------------------------------------------
// Name: DXUtil_Timer()
// Desc: Performs timer opertations. Use the following commands:
//          TIMER_RESET           - to reset the timer
//          TIMER_START           - to start the timer
//          TIMER_STOP            - to stop (or pause) the timer
//          TIMER_ADVANCE         - to advance the timer by 0.1 seconds
//          TIMER_GETABSOLUTETIME - to get the absolute system time
//          TIMER_GETAPPTIME      - to get the current time
//          TIMER_GETELAPSEDTIME  - to get the time that elapsed between 
//                                  TIMER_GETELAPSEDTIME calls
//-----------------------------------------------------------------------------
FLOAT __stdcall DXUtil_Timer( TIMER_COMMAND command )
{
    static BOOL     m_bTimerInitialized = FALSE;
    static BOOL     m_bUsingQPF         = FALSE;
    static BOOL     m_bTimerStopped     = TRUE;
    static LONGLONG m_llQPFTicksPerSec  = 0;

    // Initialize the timer
    if( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;

        // Use QueryPerformanceFrequency() to get frequency of timer.  If QPF is
        // not supported, we will timeGetTime() which returns milliseconds.
        LARGE_INTEGER qwTicksPerSec;
        m_bUsingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
        if( m_bUsingQPF )
            m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
    }

    if( m_bUsingQPF )
    {
        static LONGLONG m_llStopTime        = 0;
        static LONGLONG m_llLastElapsedTime = 0;
        static LONGLONG m_llBaseTime        = 0;
        double fTime;
        double fElapsedTime;
        LARGE_INTEGER qwTime;
        
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if( m_llStopTime != 0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter( &qwTime );

        // Return the elapsed time
        if( command == TIMER_GETELAPSEDTIME )
        {
            fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
            m_llLastElapsedTime = qwTime.QuadPart;
            return (FLOAT) fElapsedTime;
        }
    
        // Return the current time
        if( command == TIMER_GETAPPTIME )
        {
            double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;
            return (FLOAT) fAppTime;
        }
    
        // Reset the timer
        if( command == TIMER_RESET )
        {
            m_llBaseTime        = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            m_llStopTime        = 0;
            m_bTimerStopped     = FALSE;
            return 0.0f;
        }
    
        // Start the timer
        if( command == TIMER_START )
        {
            if( m_bTimerStopped )
                m_llBaseTime += qwTime.QuadPart - m_llStopTime;
            m_llStopTime = 0;
            m_llLastElapsedTime = qwTime.QuadPart;
            m_bTimerStopped = FALSE;
            return 0.0f;
        }
    
        // Stop the timer
        if( command == TIMER_STOP )
        {
            if( !m_bTimerStopped )
            {
                m_llStopTime = qwTime.QuadPart;
                m_llLastElapsedTime = qwTime.QuadPart;
                m_bTimerStopped = TRUE;
            }
            return 0.0f;
        }
    
        // Advance the timer by 1/10th second
        if( command == TIMER_ADVANCE )
        {
            m_llStopTime += m_llQPFTicksPerSec/10;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
            return (FLOAT) fTime;
        }

        return -1.0f; // Invalid command specified
    }
    else
    {
        // Get the time using timeGetTime()
        static double m_fLastElapsedTime  = 0.0;
        static double m_fBaseTime         = 0.0;
        static double m_fStopTime         = 0.0;
        double fTime;
        double fElapsedTime;
        
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if( m_fStopTime != 0.0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            fTime = m_fStopTime;
        else
            fTime = GetTickCount() * 0.001;
    
        // Return the elapsed time
        if( command == TIMER_GETELAPSEDTIME )
        {   
            fElapsedTime = (double) (fTime - m_fLastElapsedTime);
            m_fLastElapsedTime = fTime;
            return (FLOAT) fElapsedTime;
        }
    
        // Return the current time
        if( command == TIMER_GETAPPTIME )
        {
            return (FLOAT) (fTime - m_fBaseTime);
        }
    
        // Reset the timer
        if( command == TIMER_RESET )
        {
            m_fBaseTime         = fTime;
            m_fLastElapsedTime  = fTime;
            m_fStopTime         = 0;
            m_bTimerStopped     = FALSE;
            return 0.0f;
        }
    
        // Start the timer
        if( command == TIMER_START )
        {
            if( m_bTimerStopped )
                m_fBaseTime += fTime - m_fStopTime;
            m_fStopTime = 0.0f;
            m_fLastElapsedTime  = fTime;
            m_bTimerStopped = FALSE;
            return 0.0f;
        }
    
        // Stop the timer
        if( command == TIMER_STOP )
        {
            if( !m_bTimerStopped )
            {
                m_fStopTime = fTime;
                m_fLastElapsedTime  = fTime;
                m_bTimerStopped = TRUE;
            }
            return 0.0f;
        }
    
        // Advance the timer by 1/10th second
        if( command == TIMER_ADVANCE )
        {
            m_fStopTime += 0.1f;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            return (FLOAT) fTime;
        }

        return -1.0f; // Invalid command specified
    }
}




static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



CONFIGBOX::CONFIGBOX( BOOL btInvite , BOOL btSecretMessage , BOOL btWaveSnd , BOOL btMessageAgree , int ntVoice )
{
	bCheck[0] = btInvite;
	bCheck[1] = btSecretMessage;
	bCheck[2] = btWaveSnd;
	bCheck[3] = btMessageAgree;
	nVoice    = ntVoice;
}



//========================================================================
// 환경 설정창을 그린다.
//========================================================================

void CONFIGBOX::RenderConfigBox( void )
{
    g_pCMainFrame->m_SPR_ConBox->SetPos( CONFIG_DLG_POS_X , CONFIG_DLG_POS_Y );
	g_pCMainFrame->m_SPR_ConBox->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
    g_pCMainFrame->m_OkBTN->SetPos( CONFIG_DLG_POS_X + 111, CONFIG_DLG_POS_Y + 214 );
	g_pCMainFrame->m_OkBTN->DrawButton();

	for( int i = 0 ; i < 4; i ++ ) {
		if( g_pCMainFrame->m_pConfigBox->bCheck[i] == TRUE ) {
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_DLG_POS_X + 100, CONFIG_DLG_POS_Y + 60 + ( i * 27 ), 1 );
		}
		else {
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_DLG_POS_X + 205, CONFIG_DLG_POS_Y + 60 + ( i * 27 ), 1 );
		}
	}
}



void CONFIGBOX::ChangetCheck( int nNum , BOOL TempCheck )
{
	bCheck[nNum] = TempCheck;
}



BOOL CONFIGBOX::GetCheckState( int nNum )
{
	BOOL ReturnCheck;
	ReturnCheck = bCheck[nNum];

	return ReturnCheck;
}



void CONFIGBOX::ChangeVoice( int nNum )
{
	nVoice = nNum;
}



int CONFIGBOX::GetVoiceState( void )
{
	return nVoice;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	g_pCMainFrame = this;


	ServerSrandom(time(0));
	
	m_nCurrentMode = LOGO_MODE;
	m_mX = 0;
	m_mY = 0;
	m_nChatTime = 0;
	m_nDegree = 0;
	m_nScrollY = 0;
	m_nCurMashine = 0;
	m_nCurChannelNo = 0;
	m_nWaitUserCurCnt = 0;
	m_nSelectedRoomNum = -1;    // 몇번 방이 눌렀는지 본다.
	m_biMakeRoomMoneyNo = 0;
	m_nRButtonWaitUserNo = 0;
	m_TempMX = 0;
	m_TempMY = 0;
	m_nCurSorCnt = 0;

	m_nDegree = 0;
	m_nScrollY = 123;
	m_nScrollNo = 0;
	m_StartBTNState = 0;
	m_StartAni = 0;
	m_StartT1 = 0 , m_StartT2 = 0;
	m_nStartBtnCnt = 0;
	m_nCardChoiceCnt = 0;
	m_nCardDumpCnt = 0;
	m_nWinnerMarkPos = 0;
    m_nWinnerMarkPosCnt = 0;
	m_nEndingUserTime = 0;
	m_nEndingShowTime = 0;
	m_nEndingUserCurTime = 0;
	m_nEndingUserCurSlot = 0;
	m_nShowUserSlot = 0;

	m_biCall = 0;
	m_biHalf = 0;
	m_biBing = 0;
	m_biDdaDang = 0;
	m_biCheck = 0;
	m_biDie = 0;	
	m_biFull = 0;	
	m_biQuater = 0;	
	m_nUserCallTime = 0;
	m_nUserCallSlotNo = 0;
	m_nTimePos = 0;
	m_nJackPotPos = 0;
	m_nJackPotLen = 0;
	m_nJackPotStopCnt = 0;
	m_nWaitViewPageNo = 0;
	m_nInviteRoomNo = 0;
	m_biInviteRoomMoney = 0;
	m_nInviteXPos = 407;
	m_nInviteYPos = 299;
	m_nInviteTempX = 0;
	m_nInviteTempY = 0;
	m_cX = 0;
	m_cY = 0;
	m_nUserCallDefineTime = 0;
	m_nTimePosCnt = 0;
	m_nSelectionCnt = 0;
    m_nUserListTitleNum = 0;
    m_nHighlightBarNum = -1;
    m_nSelectedRoomTitleList = -1;
	m_nTemp = 0;
    m_nViewUserIdx = -1;
    m_nViewUserNo = -1;
    m_nCardOption = CO_CHOICE_MODE;
    m_nJackpotBackFrm = 0;
    m_nJackpotBackFrmCnt = 0;
    m_rgbMakeRoomTitleListBorder = 0;
    m_rgbMakeRoomTitleListBar = 0;
    m_rgbGray = 0;


    m_bGameInit = FALSE;
	m_bShowMakeRoomDlg = FALSE;
	m_bShowMessageBox = FALSE;
	m_bMBoxMoveDlg = FALSE;
	m_bMouseDown = FALSE;
	m_bActive = FALSE;
	m_bAniActive = FALSE;
	m_bMoveDlg = FALSE;
	m_bFullScreen = FALSE;
	m_bRButtonDown = FALSE;
	m_bShowResult = FALSE;
	m_bShowDialog = FALSE;
	m_bShowStartBTN = FALSE;
	m_bShowConfigBox = FALSE;
	m_bUserState = FALSE;
	m_bMessageSend = TRUE; // 쪽지를 보내는건지 받는 건지 체크 // TRUE = 보낸다. FALSE = 받는다.
	m_bMouseEvent = TRUE;
	m_bMouseRender = TRUE;
	m_bCardChoice = FALSE;	//처음 카드를 초이스 하는부분.
	m_bCardDump = FALSE;	//4장중 한장 버리기.
	m_bStartInit = FALSE;
	m_bEndding = FALSE;
	m_bEnddingShow = FALSE;
	m_bUserCallTime = FALSE;
	m_bWinnerCpoy = FALSE;
	m_bRenderPanMoney = FALSE;
	m_bJackPot = FALSE;
	m_bRoomJackPotRender = FALSE;
	m_bShowChodaeBox = FALSE;
	m_bChodaeDlg = FALSE;
	m_bShowUserInfo = FALSE;
	m_bSelection = FALSE;
	m_bSelectionStart = FALSE;
	m_bBeforeFull = FALSE;
    m_bPassRoomChk = FALSE;
    m_bPassDlg = FALSE;
    m_bShowRoomTitleList = FALSE;
    m_bMoveHiddenCard = FALSE;

	memset( m_szMessageSendUserNick, 0x00, sizeof( m_szMessageSendUserNick ));
	memset( m_szMessageData,         0x00, sizeof( m_szMessageData ));
	memset( m_szBtnTime,             0x00, sizeof( m_szBtnTime ));	
	memset( m_szGetMessageData,      0x00, sizeof( m_szGetMessageData ));
	memset( m_szJackPotMoney,        0x00, sizeof( m_szJackPotMoney ));
	memset( m_szInviteRoomPass,      0x00, sizeof( m_szInviteRoomPass ));
	memset( m_szInviteMsg,           0x00, sizeof( m_szInviteMsg ));
    memset( m_szPass,                0x00, sizeof( m_szPass ));
    memset( m_szUserAddMoney,        0x00, sizeof( m_szUserAddMoney ));
    memset(&m_sHiddenCardInfo,       0x00, sizeof( m_sHiddenCardInfo ));

    //m_pWaitRoomBanner = NULL;
	m_hSocketThread = NULL;
	m_hRenderThread = NULL;
    m_hGameInitEvent = NULL;
	m_SPR_Money = NULL; // 결과 표시의 돈
	m_SPR_MessageBox = NULL; // 쪽지 창
	m_SPR_Message = NULL; // 메세지 박스 그림
	m_SPR_LoGo = NULL;
	m_SPR_Cursor = NULL;
	m_SPR_Connting = NULL;
	m_SPR_ConBox = NULL;
	m_SPR_WaitBase = NULL;
	m_SPR_MakeRoom = NULL;
	m_SPR_Class = NULL;
	m_SPR_WaitAvatar = NULL;
	m_SPR_FullWait = NULL;
	m_SRP_SexIcon = NULL;
	m_SPR_GameBase = NULL;
	m_SPR_Start = NULL;
	m_SPR_Sun = NULL;
	m_SPR_CardSpade = NULL;
	m_SPR_CardClover = NULL;
	m_SPR_CardDia = NULL;
	m_SPR_CardHeart = NULL;
	m_SPR_CardBack = NULL;
	m_SPR_CardChioce = NULL;
	m_SPR_BetType_Big = NULL;
	m_SPR_BetType_Small = NULL;
	m_SPR_WindowWin = NULL;
	m_SPR_WinnerMark = NULL;
	m_SPR_USERTIME = NULL;
	m_SPR_Chip = NULL;

	m_SPR_JackPot = NULL;
    m_SPR_JackPotBack = NULL;
	m_SPR_AllIn = NULL;
	m_SPR_JackPotMath = NULL;



    m_SPR_SelectUser = NULL;
    m_SPR_BetMoneyBack = NULL;
	m_SPR_InviteBox = NULL;
	m_SPR_Invitation = NULL;
	m_SPR_Enter = NULL;
	m_SPR_WasteCard = NULL;
	m_SPR_ShowUserInfo = NULL;
	m_SPR_UserState = NULL;
    m_SPR_UserListTitle[0] = NULL;
    m_SPR_UserListTitle[1] = NULL;
    m_SPR_HighlightBar = NULL;
    m_SPR_SelRoomBar = NULL;
    m_SPR_CheckBall = NULL;
    m_SPR_Arrow = NULL;
    m_SPR_PassDlg = NULL;
    m_SPR_ChodaeOX = NULL;
    m_SPR_BangJang = NULL;
	
	m_MessageOKBTN = NULL; // 메세지 박스 OK 버튼
	m_MaxBTN = NULL;
	m_MaxBTN2 = NULL;
	m_ExitBTN = NULL;
	m_ConfigBTN = NULL;
	m_Exit2BTN = NULL;
	m_MinBTN = NULL;
	m_FullBTN = NULL; // 풀스크린 버튼 ( 채널 )
    m_CaptureBTN = NULL;
	
	m_MakeRoomBTN = NULL;
	m_JoinBTN = NULL;
	m_QuickJoinBTN = NULL;
    m_OkBTN = NULL;
	m_CancelBTN = NULL;
    m_RoomTitleBTN = NULL;

	m_InviteBTN = NULL;
	m_DDaDDang = NULL;    // [*BTN]
	m_Quater = NULL;
	m_Half = NULL;
	m_Full = NULL; // [*BTN]
	m_Check = NULL;
	m_BBing = NULL;
	m_Call = NULL;
	m_Die = NULL;
	m_StartBTN = NULL;
	m_InviteOKBTN = NULL;
    m_InviteCancelBTN = NULL;
	m_ChodaeYesBTN = NULL;
	m_ChodaeNoBTN = NULL;
	m_UserCloseBTN = NULL;
	m_UserStateBTN = NULL;
	m_MemoSendBTN = NULL;
    m_SCB_WaitChat = NULL;
    m_SCB_WaitUser = NULL;
    m_SCB_WaitRoom = NULL;
    m_SCB_GameChat = NULL;
    m_SCB_Invite = NULL;
    m_pWaitChat = NULL;
    m_pGameChat = NULL;
    m_pScrollNotice = NULL;
	m_WaitAvaImg = NULL;
    m_MakeRoomOkBTN = NULL;
    m_MakeRoomCancelBTN = NULL;
    m_MemoCancelBTN = NULL;

    m_IME_WaitChat = NULL;
    m_IME_GameChat = NULL;
    m_IME_InputPass = NULL;
    m_IME_InputRoomTitle = NULL;
    m_IME_RoomInPassBox = NULL;
    m_IME_Memo = NULL;


	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_GameAvaImg[i] = NULL;
		m_SPR_GameAvatar[i] = NULL;
	}

    m_hAniUpdateThread = NULL;

	m_ColControl[0] = RGB( 221, 242, 255 );             // 방 제목 
	m_ColControl[1] = RGB( 255, 255, 255 );             // 대기실 채팅
	m_ColControl[2] = RGB(   3,  38,  56 );             // 게임 방 채팅
	m_ColControl[3] = RGB( 255, 255, 255 );             // 메시지 ..

	m_Brush[0] = CreateSolidBrush( m_ColControl[0] );   // 방 제목 
	m_Brush[1] = CreateSolidBrush( m_ColControl[1] );   // 대기실 채팅
	m_Brush[2] = CreateSolidBrush( m_ColControl[2] );   // 게임 방 채팅
	m_Brush[3] = CreateSolidBrush( m_ColControl[3] );   // 메시지 ..

    // 방만들기 창에서 금액 항목 체크 이미지 위치
	m_sMakeRoomMoneyPoint[0] = SetPoint( 378, 361 );
	m_sMakeRoomMoneyPoint[1] = SetPoint( 518, 361 );
	m_sMakeRoomMoneyPoint[2] = SetPoint( 378, 382 );
	m_sMakeRoomMoneyPoint[3] = SetPoint( 518, 382 );
    m_sMakeRoomMoneyPoint[4] = SetPoint( 378, 403 );
	m_sMakeRoomMoneyPoint[5] = SetPoint( 518, 403 );
	m_sMakeRoomMoneyPoint[6] = SetPoint( 378, 424 );
	m_sMakeRoomMoneyPoint[7] = SetPoint( 518, 424 );

	//판 돈 위치. 0 ~부터 낮은 순서. 100단위, 1000단위......
    m_sPanMoneyInfo[0].PanMoneyPos  = SetPoint( 408, 191 );
	m_sPanMoneyInfo[1].PanMoneyPos  = SetPoint( 450, 191 );
	m_sPanMoneyInfo[2].PanMoneyPos  = SetPoint( 492, 191 );
	m_sPanMoneyInfo[3].PanMoneyPos  = SetPoint( 534, 191 );
    m_sPanMoneyInfo[4].PanMoneyPos  = SetPoint( 576, 191 );
	m_sPanMoneyInfo[5].PanMoneyPos  = SetPoint( 429, 220 );
	m_sPanMoneyInfo[6].PanMoneyPos  = SetPoint( 471, 220 );
	m_sPanMoneyInfo[7].PanMoneyPos  = SetPoint( 513, 220 );
	m_sPanMoneyInfo[8].PanMoneyPos  = SetPoint( 555, 220 );
	m_sPanMoneyInfo[9].PanMoneyPos  = SetPoint( 450, 249 );
	m_sPanMoneyInfo[10].PanMoneyPos = SetPoint( 492, 249 );
	m_sPanMoneyInfo[11].PanMoneyPos = SetPoint( 534, 249 );
	m_sPanMoneyInfo[12].PanMoneyPos = SetPoint( 471, 278 );
	m_sPanMoneyInfo[13].PanMoneyPos = SetPoint( 513, 278 );
	m_sPanMoneyInfo[14].PanMoneyPos = SetPoint( 492, 307 );
    m_sPanMoneyInfo[15].PanMoneyPos = SetPoint( 534, 307 );

	m_sRoomInUserPosition[0].pTimePos = SetPoint( 445, 538 );
	m_sRoomInUserPosition[1].pTimePos = SetPoint( 106, 489 );
	m_sRoomInUserPosition[2].pTimePos = SetPoint( 106, 284 );
	m_sRoomInUserPosition[3].pTimePos = SetPoint( 700, 284 );
	m_sRoomInUserPosition[4].pTimePos = SetPoint( 700, 489 );

    // 각 유저의 톨 던지는 시작 위치 - ham 2005.09.26
    m_sRoomInUserPosition[0].pChipStartPos = SetPoint( 446, 535 );
    m_sRoomInUserPosition[1].pChipStartPos = SetPoint( 301, 407 );
    m_sRoomInUserPosition[2].pChipStartPos = SetPoint( 301, 213 );
    m_sRoomInUserPosition[3].pChipStartPos = SetPoint( 684, 213 );
    m_sRoomInUserPosition[4].pChipStartPos = SetPoint( 684, 407 );

	m_JakcPotMathPoint[0]  = SetPoint( 295, 84 );
	m_JakcPotMathPoint[1]  = SetPoint( 284, 84 );
	m_JakcPotMathPoint[2]  = SetPoint( 273, 84 );
	m_JakcPotMathPoint[3]  = SetPoint( 255, 84 );
	m_JakcPotMathPoint[4]  = SetPoint( 244, 84 );
	m_JakcPotMathPoint[5]  = SetPoint( 233, 84 );
	m_JakcPotMathPoint[6]  = SetPoint( 215, 84 );
	m_JakcPotMathPoint[7]  = SetPoint( 204, 84 );
	m_JakcPotMathPoint[8]  = SetPoint( 193, 84 );
	m_JakcPotMathPoint[9]  = SetPoint( 175, 84 );
	m_JakcPotMathPoint[10] = SetPoint( 164, 84 );
	m_JakcPotMathPoint[11] = SetPoint( 153, 84 );
	m_JakcPotMathPoint[12] = SetPoint( 136, 84 );
	m_JakcPotMathPoint[13] = SetPoint( 125, 84 );
	m_JakcPotMathPoint[14] = SetPoint( 114, 84 );

	m_JackPotCommaPoint[0] = SetPoint( 263, 84 );
	m_JackPotCommaPoint[1] = SetPoint( 223, 84 );
	m_JackPotCommaPoint[2] = SetPoint( 183, 84 );
	m_JackPotCommaPoint[3] = SetPoint( 144, 84 );
    
	//GameRender에서 위치.
	m_sRoomInUserPosition[0].pAvatarPos = SetPoint( 346, 567 );
	m_sRoomInUserPosition[1].pAvatarPos = SetPoint(  15, 363 );
	m_sRoomInUserPosition[2].pAvatarPos = SetPoint(  15, 158 );
	m_sRoomInUserPosition[3].pAvatarPos = SetPoint( 924, 158 );
	m_sRoomInUserPosition[4].pAvatarPos = SetPoint( 924, 363 );
    
  	m_sRoomInUserPosition[0].pNickPos = SetPoint( 370, 718 );
	m_sRoomInUserPosition[1].pNickPos = SetPoint(  31, 335 );
	m_sRoomInUserPosition[2].pNickPos = SetPoint(  31, 130 );
	m_sRoomInUserPosition[3].pNickPos = SetPoint( 704, 130 );
	m_sRoomInUserPosition[4].pNickPos = SetPoint( 704, 335 );

	m_sRoomInUserPosition[0].pCardInfo = SetPoint( 445, 557 );
	m_sRoomInUserPosition[1].pCardInfo = SetPoint( 106, 360 );
	m_sRoomInUserPosition[2].pCardInfo = SetPoint( 106, 155 );
	m_sRoomInUserPosition[3].pCardInfo = SetPoint( 694, 155 );
	m_sRoomInUserPosition[4].pCardInfo = SetPoint( 694, 360 );

	m_sRoomInUserPosition[0].pAllInPos = SetPoint( m_sRoomInUserPosition[0].pCardInfo.x, m_sRoomInUserPosition[0].pCardInfo.y + 36 );
	m_sRoomInUserPosition[1].pAllInPos = SetPoint( m_sRoomInUserPosition[1].pCardInfo.x, m_sRoomInUserPosition[1].pCardInfo.y + 36 );
	m_sRoomInUserPosition[2].pAllInPos = SetPoint( m_sRoomInUserPosition[2].pCardInfo.x, m_sRoomInUserPosition[2].pCardInfo.y + 36 );
	m_sRoomInUserPosition[3].pAllInPos = SetPoint( m_sRoomInUserPosition[3].pCardInfo.x, m_sRoomInUserPosition[3].pCardInfo.y + 36 );
	m_sRoomInUserPosition[4].pAllInPos = SetPoint( m_sRoomInUserPosition[4].pCardInfo.x, m_sRoomInUserPosition[4].pCardInfo.y + 36 );

	m_sRoomInUserPosition[0].pCardStartPos = SetPoint( m_sRoomInUserPosition[0].pCardInfo.x + 8, m_sRoomInUserPosition[0].pCardInfo.y + 10 );
	m_sRoomInUserPosition[1].pCardStartPos = SetPoint( m_sRoomInUserPosition[1].pCardInfo.x + 8, m_sRoomInUserPosition[1].pCardInfo.y + 10 );
	m_sRoomInUserPosition[2].pCardStartPos = SetPoint( m_sRoomInUserPosition[2].pCardInfo.x + 8, m_sRoomInUserPosition[2].pCardInfo.y + 10 );
	m_sRoomInUserPosition[3].pCardStartPos = SetPoint( m_sRoomInUserPosition[3].pCardInfo.x + 8, m_sRoomInUserPosition[3].pCardInfo.y + 10 );
	m_sRoomInUserPosition[4].pCardStartPos = SetPoint( m_sRoomInUserPosition[4].pCardInfo.x + 8, m_sRoomInUserPosition[4].pCardInfo.y + 10 );

	m_sRoomInUserPosition[0].pBossPos = SetPoint( m_sRoomInUserPosition[0].pCardStartPos.x, m_sRoomInUserPosition[0].pCardStartPos.y + 80 );
	m_sRoomInUserPosition[1].pBossPos = SetPoint( m_sRoomInUserPosition[1].pCardStartPos.x, m_sRoomInUserPosition[1].pCardStartPos.y + 80 );
	m_sRoomInUserPosition[2].pBossPos = SetPoint( m_sRoomInUserPosition[2].pCardStartPos.x, m_sRoomInUserPosition[2].pCardStartPos.y + 80 );
	m_sRoomInUserPosition[3].pBossPos = SetPoint( m_sRoomInUserPosition[3].pCardStartPos.x, m_sRoomInUserPosition[3].pCardStartPos.y + 80 );
	m_sRoomInUserPosition[4].pBossPos = SetPoint( m_sRoomInUserPosition[4].pCardStartPos.x, m_sRoomInUserPosition[4].pCardStartPos.y + 80 );

	m_sRoomInUserPosition[0].pBangJangPos = SetPoint( m_sRoomInUserPosition[0].pAvatarPos.x, m_sRoomInUserPosition[0].pAvatarPos.y + 120 );
	m_sRoomInUserPosition[1].pBangJangPos = SetPoint( m_sRoomInUserPosition[1].pAvatarPos.x, m_sRoomInUserPosition[1].pAvatarPos.y + 120 );
	m_sRoomInUserPosition[2].pBangJangPos = SetPoint( m_sRoomInUserPosition[2].pAvatarPos.x, m_sRoomInUserPosition[2].pAvatarPos.y + 120 );
	m_sRoomInUserPosition[3].pBangJangPos = SetPoint( m_sRoomInUserPosition[3].pAvatarPos.x + 85 - 16, m_sRoomInUserPosition[3].pAvatarPos.y + 120 );
	m_sRoomInUserPosition[4].pBangJangPos = SetPoint( m_sRoomInUserPosition[4].pAvatarPos.x + 85 - 16, m_sRoomInUserPosition[4].pAvatarPos.y + 120 );

	m_sRoomInUserPosition[0].pUserMoneyPos = SetPoint(  660, m_sRoomInUserPosition[0].pNickPos.y );
	m_sRoomInUserPosition[1].pUserMoneyPos = SetPoint(  321, m_sRoomInUserPosition[1].pNickPos.y );
	m_sRoomInUserPosition[2].pUserMoneyPos = SetPoint(  321, m_sRoomInUserPosition[2].pNickPos.y );
	m_sRoomInUserPosition[3].pUserMoneyPos = SetPoint( 1000, m_sRoomInUserPosition[3].pNickPos.y );
	m_sRoomInUserPosition[4].pUserMoneyPos = SetPoint( 1000, m_sRoomInUserPosition[4].pNickPos.y );

	m_sRoomInUserPosition[0].pJokboPos = SetPoint( m_sRoomInUserPosition[0].pCardInfo.x + 111, m_sRoomInUserPosition[0].pCardInfo.y + 136 );
	m_sRoomInUserPosition[1].pJokboPos = SetPoint( m_sRoomInUserPosition[1].pCardInfo.x + 219, m_sRoomInUserPosition[1].pCardInfo.y + 149 );
	m_sRoomInUserPosition[2].pJokboPos = SetPoint( m_sRoomInUserPosition[2].pCardInfo.x + 219, m_sRoomInUserPosition[2].pCardInfo.y + 149 );
	m_sRoomInUserPosition[3].pJokboPos = SetPoint( m_sRoomInUserPosition[3].pCardInfo.x + 219, m_sRoomInUserPosition[3].pCardInfo.y + 149 );
	m_sRoomInUserPosition[4].pJokboPos = SetPoint( m_sRoomInUserPosition[4].pCardInfo.x + 219, m_sRoomInUserPosition[4].pCardInfo.y + 149 );

	m_sRoomInUserPosition[0].pUserSelectBigPos = SetPoint( 661, 487 );
	m_sRoomInUserPosition[1].pUserSelectBigPos = SetPoint( 326, 290 );
	m_sRoomInUserPosition[2].pUserSelectBigPos = SetPoint( 326,  85 );
	m_sRoomInUserPosition[3].pUserSelectBigPos = SetPoint( 633,  85 );
	m_sRoomInUserPosition[4].pUserSelectBigPos = SetPoint( 633, 290 );

	m_sRoomInUserPosition[0].pUserSelectSmallPos = SetPoint( m_sRoomInUserPosition[0].pTimePos.x + 192, m_sRoomInUserPosition[0].pTimePos.y + 1 );
	m_sRoomInUserPosition[1].pUserSelectSmallPos = SetPoint( m_sRoomInUserPosition[1].pTimePos.x + 192, m_sRoomInUserPosition[1].pTimePos.y + 1 );
	m_sRoomInUserPosition[2].pUserSelectSmallPos = SetPoint( m_sRoomInUserPosition[2].pTimePos.x + 192, m_sRoomInUserPosition[2].pTimePos.y + 1 );
	m_sRoomInUserPosition[3].pUserSelectSmallPos = SetPoint( m_sRoomInUserPosition[3].pTimePos.x + 192, m_sRoomInUserPosition[3].pTimePos.y + 1 );
	m_sRoomInUserPosition[4].pUserSelectSmallPos = SetPoint( m_sRoomInUserPosition[4].pTimePos.x + 192, m_sRoomInUserPosition[4].pTimePos.y + 1 );

	m_sRoomInUserPosition[0].pWinnerPos = SetPoint( m_sRoomInUserPosition[0].pCardInfo.x + 31, m_sRoomInUserPosition[0].pCardInfo.y + 45 );
	m_sRoomInUserPosition[1].pWinnerPos = SetPoint( m_sRoomInUserPosition[1].pCardInfo.x + 31, m_sRoomInUserPosition[1].pCardInfo.y + 45 );
	m_sRoomInUserPosition[2].pWinnerPos = SetPoint( m_sRoomInUserPosition[2].pCardInfo.x + 31, m_sRoomInUserPosition[2].pCardInfo.y + 45 );
	m_sRoomInUserPosition[3].pWinnerPos = SetPoint( m_sRoomInUserPosition[3].pCardInfo.x + 31, m_sRoomInUserPosition[3].pCardInfo.y + 45 );
	m_sRoomInUserPosition[4].pWinnerPos = SetPoint( m_sRoomInUserPosition[4].pCardInfo.x + 31, m_sRoomInUserPosition[4].pCardInfo.y + 45 );

 	m_FrameSkip.SetFramePerSec( 50 );
}



CMainFrame::~CMainFrame()
{
    SAFE_DELETE( m_IME_WaitChat );
    SAFE_DELETE( m_IME_GameChat );
    SAFE_DELETE( m_IME_InputPass );
    SAFE_DELETE( m_IME_InputRoomTitle );
    SAFE_DELETE( m_IME_RoomInPassBox );
    SAFE_DELETE( m_IME_Memo );
	SAFE_DELETE( m_Message );
	SAFE_DELETE( m_pConfigBox );
	SAFE_DELETE( g_pClient );
    SAFE_DELETE( m_pWaitChat );
    SAFE_DELETE( m_pGameChat );
    
	AvatarDelete();

    SAFE_DELETE( g_pCDXSound );
    SAFE_DELETE( g_pCDXInput );
    SAFE_DELETE( g_pCDXScreen );
}



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }
		HWND hWnd = this->m_hWnd;
		g_hWnd = hWnd;


#ifdef SHOWUSERCARD
	CAdminLogin log;
	if(log.DoModal()!=IDOK)
		ExitGame();
    else {

		memset( g_LSIp,      0x00, sizeof( g_LSIp ));
		memset( g_szUser_ID, 0x00, sizeof( g_szUser_ID ));
		memset( g_szAvaUrl,  0x00, sizeof( g_szAvaUrl ));

		strcpy( g_LSIp,                 "222.231.24.127" );
				g_LSPort              = 9000;
		strcpy( g_szUser_ID,            log.m_AdminID );
	//	strcpy( g_szUser_ID,            "test1");

		strcpy( g_szAvaUrl,             "101002.gif" );
				g_szGameJong          = 1;
				g_szGameServerChannel = 0;
				g_nServiceDiv         = 0;
		strcpy( g_szTempParam,         "0" );
		g_nSystemMoneyRate =  5;


    }

#else

		DTRACE2("[뉴 포커] 컴파일 시기 %s (%s)\n",__DATE__,__TIME__);


		char szTemp[255];
		//넘어온 인자확인
		if( __argc != 10 ) {       //실행파일 서버넘버, 아이디
			::MessageBox( hWnd, "웹에서 실행해주세요.", "NewZenPoker", MB_OK );
			ExitGame();
			return 0;
		}

	#ifdef _DEBUG
		DTRACE("================================================");
		DTRACE("[0] FileName ----- %s", __argv[0]);
		DTRACE("[1] Server IP ---- %s", __argv[1]);
		DTRACE("[2] Server Port -- %s", __argv[2]);
		DTRACE("[3] User ID ------ %s", __argv[3]);
		DTRACE("[4] Avatar URL --- %s", __argv[4]);
		DTRACE("[5] 게임 종류 ---- %s", __argv[5]);
		DTRACE("[6] Channel ------ %s", __argv[6]);
		DTRACE("[7] 서비스 구분 -- %s", __argv[7]);
		DTRACE("[8] 여분의 정보 -- %s", __argv[8]);

		TRACE("IP  %s Port %s \n",__argv[1],__argv[2]);

		DTRACE("================================================");
	#endif

		memset( g_LSIp,      0x00, sizeof( g_LSIp ));
		memset( g_szUser_ID, 0x00, sizeof( g_szUser_ID ));
		memset( g_szAvaUrl,  0x00, sizeof( g_szAvaUrl ));

//		strcpy( g_LSIp,                       __argv[1] );
//				g_LSPort              = atoi( __argv[2] );
//	    strcpy( g_LSIp,               "61.100.185.202" );

//	    strcpy( g_LSIp,               "211.239.117.147" );
		strcpy( g_LSIp,               "192.168.10.123" );
//		strcpy( g_LSIp,               "61.250.93.105" );


        g_LSPort              = 9000;



		strcpy( g_szUser_ID,                  __argv[3] );
		strcpy( g_szAvaUrl,                   __argv[4] );
				g_szGameJong          = atoi( __argv[5] );
				g_szGameServerChannel = atoi( __argv[6] );
				g_nServiceDiv         = atoi( __argv[7] );
		strcpy( g_szTempParam,                __argv[8] );

		memset( szTemp, 0x00, sizeof(szTemp) );
		strcpy( szTemp,        __argv[9] );

		if(isInteger(szTemp))
			g_nSystemMoneyRate = atoi(szTemp);

#endif


    DTRACE2( "[%s] 프로그램 시작", g_szUser_ID );



    memset(g_szProgPath, 0x00, sizeof(g_szProgPath));

	//yun 현재 실행되는 전체경로와 실행 파일을 가져온다. c:\aaa\bbb.exe
	GetModuleFileName(::GetModuleHandle(NULL), g_szProgPath, sizeof(g_szProgPath));

	int path_len = strlen(g_szProgPath);

	//yun 경로를 찾아서 뒤에서 부터 읽어드린후 마지막 경로 앞까지의 경로를 찾는다.
	// 다음을 거치고 나면 c:\aaa\만 남는다.
	for ( int i = path_len - 1; i >= 0; i-- ) {
		if (g_szProgPath[i] == '\\') {
			g_szProgPath[i+1] = '\0';
			break;
		}
	}
    if (i < 0) {
        return false;
    }

	strdup(g_szProgPath);
	


#ifndef _DEBUG
	SetCurrentDirectory( g_szProgPath );
	DTRACE("SetCurrentDirectory - %s", g_szProgPath);
#endif

    /*
	//DirectX버전 체크
	DWORD VerCheck = 0;
	VerCheck = g_dx2d.GetDXVerCheck();

	// DX Version이 7.0 보다 작거나 같다면 에러를 표시한다.
	if( VerCheck < 0x700 )
	{
		//ShowCursor(TRUE);
		int nRe = ::MessageBox( hWnd, "DirectX 버젼 7.0을 설치해야 합니다!", "DirectX Version Error", MB_ICONERROR | MB_YESNO );

		if( nRe == IDYES ) { // YES
			
			::ShellExecute( hWnd, NULL, "http://www.microsoft.com/downloads/details.aspx?FamilyID=141d5f9e-07c1-462a-baef-5eab5c851cf5&displaylang=ko", NULL, NULL, SW_SHOW );
		}
		else // NO
		{
			::MessageBox( hWnd, "DirectX 7.0을 설치하지 않았습니다.", "DX Version Error", MB_ICONWARNING | MB_OK );
		}
		
		::PostMessage( hWnd, WM_CLOSE, 0, 0 );
		
		return 0;
	}
    */

    m_pWaitChat = new CTextList( 98, MAX_TEXT_LINE );
    m_pGameChat = new CTextList( 48, MAX_TEXT_LINE );

	//config.ini 읽어와서 세팅.
	LoadEnviroMent();

	g_pClient = new CNetworkSock;

	if( g_pClient->InitSocket( g_hWnd, g_LSIp, g_LSPort ) ) {
		g_ServerSend = false;
        g_pCDXScreen = new CDXScreen();

        if( FAILED(g_pCDXScreen->CreateWindowed(g_hWnd, SCREEN_WIDTH, SCREEN_HEIGHT))) {
            DTRACE("1024x768 창모드를 설정할 수 없습니다.");
            return 0;
        }

        g_pCDXInput = new CDXInput();

        if (g_pCDXInput == NULL ) {
            DTRACE("입력장치를 설정할 수 없습니다.");
            return 0;
        }

        if (g_pCDXInput->Create( (HINSTANCE)GetWindowLong( g_hWnd, GWL_HINSTANCE ), g_hWnd) < 0) {
            DTRACE("입력장치를 설정할 수 없습니다.");
            return 0;
        }

        g_pCDXSound = new CDXSound();

        if (g_pCDXSound->Create(g_hWnd) == DS_OK) {
            g_bSoundCard = true;
        }
        else {
            g_bSoundCard = false;
        }

		ShowCursor( FALSE );
		LoadData();

		m_fSkipTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );
		m_bActive = TRUE;
		m_bAniActive = TRUE;

		unsigned m_dwAniUpdateThread;
        //m_hAniUpdateThread = (HANDLE)_beginthreadex( NULL, 0, &AniUpdateThreadProc, NULL, 0, &m_dwAniUpdateThread );

		LoadControl();
        
        /*
        // 대기실 광고창을 만든다.

        m_pWaitRoomBanner = new CHttpView;

	    if (!m_pWaitRoomBanner->CreateWnd( CRect(13, 13, 682, 144), this )) 
		    return -1;

        if ( g_nServiceDiv == SERVICE_TORIZONE ) {          // 토리존 게임
            m_pWaitRoomBanner->Navigate( "http://www.torizone.net/advertise/advertise.asp" );
        }
        else if ( g_nServiceDiv == SERVICE_GAMEPOOL ) {     // 게임풀 게임
            m_pWaitRoomBanner->Navigate( "http://www.gamepool.co.kr/advertise/advertise.asp" );
        }
        else {                                              // 기타
            m_pWaitRoomBanner->Navigate( "http://www.torizone.net/advertise/advertise.asp" );
        }
        */
	}

	SetNumLock( TRUE );


	return 0;
}



//========================================================================
// 처음 게임을 시작할때 초기화.
//========================================================================

void CMainFrame::GameInit()
{
	g_biPanMoney = 0;
    g_biTotTempMoney = 0;
	g_biCallMoney = 0;
	g_biTotalBettingMoney = 0;
	g_nWinnerSlotNo = 0;
	g_nCurMasterSlotNo = 0;
	g_bMyWin = FALSE;
	
	m_bSelectionStart = FALSE;
	m_bSelection = FALSE;
	m_bEndding = FALSE;
	m_bEnddingShow = FALSE;
	m_bJackPot = FALSE;
	m_bWinnerCpoy = FALSE;
	m_bRenderPanMoney = FALSE;
	m_bStartInit = FALSE;
	m_bCardChoice = FALSE;
	m_bCardDump = FALSE;
	m_bUserCallTime = FALSE;
	m_bRoomJackPotRender = FALSE;
	m_bShowInviteBox = FALSE;
	m_bShowChodaeBox = FALSE;
	m_bBeforeFull = FALSE;
    m_bMoveHiddenCard = FALSE;

	m_nJackPotStopCnt = 0;
	m_nJackPotPos = 0;
	m_nUserCallTime = 0;
	m_nEndingUserTime = 0;
	m_nEndingShowTime = 0;
	m_nEndingUserCurTime = 0;
	m_nEndingUserCurSlot = 0;
	m_nUserCallSlotNo = 0;
	m_bUserCallTime = 0;
	m_nTimePos = 0;

	//버튼 리셋.
	m_Call->status = DISABLED;
	m_Half->status = DISABLED;
	m_BBing->status = DISABLED;
	m_DDaDDang->status = DISABLED; // [*BTN]
	m_Check->status = DISABLED;
	m_Die->status = DISABLED;
	m_Full->status = DISABLED; // [*BTN]
	m_Quater->status = DISABLED;

    memset( m_szUserAddMoney,     0x00, sizeof( m_szUserAddMoney ));
    memset(&m_sHiddenCardInfo,    0x00, sizeof( m_sHiddenCardInfo ));

	for ( int i = 0; i < MAX_ROOM_IN; i++  ) {
		g_CGameUserList.m_GameUserInfo[i].Init();
		g_CGameUserList.m_GameUserInfo[i].InitCard();
	}
				
	for ( i = 0; i < 16; i++ ) {
		m_sPanMoneyInfo[i].nPanMoneyCnt = 0;
	}


    //톨 및 애니 스택 초기화
    m_CAniMation.EmptyStack();
    m_ChipAni.EmptyStack();
}



void CMainFrame::NetWorkThreaOnGameInit()
{
	g_biPanMoney = 0;
    g_biTotTempMoney = 0;
	g_biCallMoney = 0;
	g_biTotalBettingMoney = 0;
	g_nWinnerSlotNo = 0;
	g_nCurMasterSlotNo = 0;
	g_bMyWin = FALSE;
	
	m_bSelectionStart = FALSE;
	m_bSelection = FALSE;
	m_bEndding = FALSE;
	m_bEnddingShow = FALSE;
	m_bJackPot = FALSE;
	m_bWinnerCpoy = FALSE;
	m_bRenderPanMoney = FALSE;
	m_bStartInit = FALSE;
	m_bCardChoice = FALSE;
	m_bCardDump = FALSE;
	m_bUserCallTime = FALSE;
	m_bRoomJackPotRender = FALSE;
	m_bShowInviteBox = FALSE;
	m_bShowChodaeBox = FALSE;
	m_bBeforeFull = FALSE;
    m_bMoveHiddenCard = FALSE;

	m_nJackPotStopCnt = 0;
	m_nJackPotPos = 0;
	m_nUserCallTime = 0;
	m_nEndingUserTime = 0;
	m_nEndingShowTime = 0;
	m_nEndingUserCurTime = 0;
	m_nEndingUserCurSlot = 0;
	m_nUserCallSlotNo = 0;
	m_bUserCallTime = 0;
	m_nTimePos = 0;

	//버튼 리셋.
	m_Call->status = DISABLED;
	m_Half->status = DISABLED;
	m_BBing->status = DISABLED;
	m_DDaDDang->status = DISABLED;  //[*BTN]
	m_Check->status = DISABLED;
	m_Die->status = DISABLED;
	m_Full->status = DISABLED; // [*BTN]
	m_Quater->status = DISABLED;

    memset( m_szUserAddMoney,     0x00, sizeof( m_szUserAddMoney ));
    memset(&m_sHiddenCardInfo,    0x00, sizeof( m_sHiddenCardInfo ));

	for ( int i = 0; i < MAX_ROOM_IN; i++  ) {
		g_CGameUserList.m_GameUserInfo[i].Init();
		g_CGameUserList.m_GameUserInfo[i].InitCard();
	}
				
	for ( i = 0; i < 16; i++ ) {
		m_sPanMoneyInfo[i].nPanMoneyCnt = 0;
	}


    //톨 및 애니 스택 초기화
    m_CAniMation.EmptyStack();
    m_ChipAni.EmptyStack();

	DTRACE2("[%s]이벤트푼다", g_szUser_ID);

    if ( SetEvent(m_hGameInitEvent) == FALSE ) {
        DTRACE2("[%s] CMainFrame::NetWorkThreaOnGameInit() - SetEvent() 오류", g_szUser_ID );
    }
}



void CMainFrame::EndingSound( BYTE nEnding )
{
	switch(nEnding) {
		case 11:
			PlaySound(g_Sound[GS_RSFLUSH]);
			break;

		case 10:
			PlaySound(g_Sound[GS_STRAIGHTFLUSH]);
			break;

		case 9:
			PlaySound(g_Sound[GS_FOURCARD]);
			break;

		case 8:
			PlaySound(g_Sound[GS_FULLHOUSE]);
			break;

		case 7:
			PlaySound(g_Sound[GS_FLUSH]);
			break;

		case 6:
			PlaySound(g_Sound[GS_MOUNTAIN]);
			break;

		case 5:
			PlaySound(g_Sound[GS_BACKSTRAIGHT]);
			break;

		case 4:
			PlaySound(g_Sound[GS_STRAIGHT]);
			break;

		case 3:
			PlaySound(g_Sound[GS_TRIPLE]);
			break;

		case 2:
			PlaySound(g_Sound[GS_TWOPAIR]);
			break;

		case 1:
			PlaySound(g_Sound[GS_ONEPAIR]);
			break;

		case 0:
			PlaySound(g_Sound[GS_TOP]);
			break;
	}
}



void CMainFrame::AnimationUpdate()
{
	//========================================================================
	//50 이 1초.
	//========================================================================

    if ( m_nCurrentMode == GAME_MODE ) {

        //----------------------------------------
        // 잭팟 배경 이미지의 프레임번호를 바꾼다.
        //----------------------------------------

        m_nJackpotBackFrmCnt++;

        if ( m_nJackpotBackFrmCnt >= 10 ) {
            m_nJackpotBackFrmCnt = 0;
            m_nJackpotBackFrm = ( m_nJackpotBackFrm + 1 ) % 2;  // 다음 프레임
        }
    }

    //-------------------
    // 스크롤 텍스트 이동
    //-------------------

    if ( m_nCurrentMode == WAITROOM_MODE && m_pScrollNotice != NULL) {
        m_pScrollNotice->Update();
    }

    //-------------------------------
	// 유저가 콜, 다이등 부르는 시간.
    //-------------------------------

	if ( m_bUserCallTime ) {
		m_nUserCallTime++;

		//5초 남겨놓고 타이머 시작한다.
		if ( ( m_nUserCallDefineTime - m_nUserCallTime ) <= 500 ) {     // ham 2005.09.26 - 250 Frame --> 500 Frame

			if ( !(m_nTemp % 50) ) {
				m_nTimePos = ( m_nTemp / 50 );

                if ( m_nTimePos >= 5 ) {                                // 5초 이하로 남았을 때 소리를 출력한다.
                    PlaySound( g_Sound[GS_COUNT] );
                }
			}
			m_nTemp++;
		}
       
		//디파인 타임이 지나면 자동 다이 한다.
		if ( m_nUserCallTime > m_nUserCallDefineTime ) {
			m_nUserCallTime = 0;
			m_bUserCallTime = FALSE;
			m_nTemp = 0;
			m_nTimePos = 0;

            // [*IMSI]
			//자신이면 다이를 보낸다.
			if ( m_nUserCallSlotNo == 0 ) {
				AllCallBtnUp();
				g_pClient->UserSelect( '7' );
			}
            ////
		}

	}

    //-------------------------
	// 스타트 버튼 떠있는 시간.
    //-------------------------

	if ( m_bShowStartBTN ) {

		m_nStartBtnCnt++;

		if( m_nStartBtnCnt > ONSTARTBTN_TIME ) {    // 숫자가 클수록 더 오래 떠있음
			OnBtnStart();
		}
	}

	//초이스 방일때.
	if ( g_bChoiceRoom ) {
		//------------------
		// 시작하기.
		//------------------
		
		if ( m_bStartInit && !m_bCardDump ) {

            DTRACE("m_bStartInit = %d, m_bCardDump = %d, m_CAniMation.m_nStackTop = %d", 
                m_bStartInit, m_bCardDump, m_CAniMation.m_nStackTop);
			if ( m_CAniMation.m_nStackTop <= 0 && m_CAniMation.m_card[0] == NULL ) {
                 
				m_nCardDumpCnt++;

				if ( m_nCardDumpCnt > CHOICEOPEN_TIME ) {
					m_bCardDump = TRUE;
					m_bStartInit = FALSE;
					m_nCardDumpCnt = 0;
				}
			}
		}

		//--------------------
		// 4장 중 한 장 버리기
		//--------------------

        // [*IMSI]
		if ( m_bCardDump ) {
			m_nCardDumpCnt++;

			if ( m_nCardDumpCnt > CHOICECARD_TIME ) {
                m_nCardDumpCnt = 0;
				m_bCardDump = FALSE;
				m_bCardChoice = TRUE;
				g_CGameUserList.UserCardDump( 0, 3 );
				g_pClient->CardDump( 3 );
			}
		}
        ////

		//-------------------------
		// 초이스 화면 떠있는 시간.
		//-------------------------

        // [*IMSI]
		if ( m_bCardChoice ) {
			m_nCardChoiceCnt++;

			if ( m_nCardChoiceCnt > CHOICECARD_TIME ) {
				m_bCardChoice = FALSE;
				m_nCardChoiceCnt = 0;
				g_CGameUserList.UserCardChoice( 0, 2 );
				g_pClient->CardChoice( 2 );

				//자동 초이스가 되고 나서 카드를 제자리로 옮겨야 한다.
				//카드위치를 다시 제자리로 놓는다.
				g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
				for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
				}
			}
		}
        ////
	}
	else {
        
		//------------------
		// 시작하기.
		//------------------
		if ( m_bStartInit && !m_bCardChoice ) {
			if ( m_CAniMation.m_nStackTop <= 0 && m_CAniMation.m_card[0] == NULL ) {

				m_nCardChoiceCnt++;

				if ( m_nCardChoiceCnt > CHOICEOPEN_TIME ) {
					m_bCardChoice = TRUE;
					m_bStartInit = FALSE;
					m_nCardChoiceCnt = 0;
				}
			}
		}

		//-------------------------
		// 초이스 화면 떠있는 시간.
		//-------------------------

		if ( m_bCardChoice ) {

			m_nCardChoiceCnt++;

			if ( m_nCardChoiceCnt > CHOICECARD_TIME ) {
				m_bCardChoice = FALSE;
				m_nCardChoiceCnt = 0;
				g_CGameUserList.UserCardChoice( 0, 2 );
				g_pClient->CardChoice( 2 );

				//자동 초이스가 되고 나서 카드를 제자리로 옮겨야 한다.
				//카드위치를 다시 제자리로 놓는다.
				g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
				for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
				}
			}
		}
	}

    //--------------------
	// 결과창 떠있는 시간.
    //--------------------

	if ( m_bEnddingShow ) {
        // 이겼을 때의 에니메이션을 위해 다음 프레임을 선택한다.
        m_nWinnerMarkPosCnt++;

        if ( m_nWinnerMarkPosCnt >= 3 ) {                       // 3 Frame 마다 이미지의 프레임 바꾼다.
            m_nWinnerMarkPosCnt = 0;
            m_nWinnerMarkPos = ( m_nWinnerMarkPos + 1 ) % 6;    // 총 6 프레임으로 구성되어 있다.
        }

		//카드 초이스가 떠있을때 다른 유저들을 죽이면 결과가 나오고 나서도 이부분이 계속 돌아갈수 있다.
		if ( m_bStartInit || m_bCardDump || m_bCardChoice ) {
			m_bCardChoice = FALSE;
			m_bStartInit = FALSE;
			m_bCardDump = FALSE;
			m_nCardChoiceCnt = 0;	
			m_nCardDumpCnt = 0;

			//m_bStartInit, m_bCardChoice 이게 트루 인상태는 카드 초이스 상태에서 게임이 종료된것이다.
			//그렇기 때문에 카드위치를 다시 제자리로 놓는다.
			g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
			for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
				g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
				g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
			}			
		}

		//유저 콜타임 시계작동.
		if ( m_bUserCallTime ) {
			AllCallBtnUp();
		}

		//결과창이 뜨자마자 승리자 돈세팅
		if ( m_nEndingShowTime == 0 ) {

			g_pCMainFrame->m_bSelectionStart = FALSE;

			char pTemp[256];
            char szUserJokbo[256];
			memset( pTemp, 0x00, sizeof(pTemp) );
            memset( szUserJokbo, 0x00, sizeof(szUserJokbo) );

			//유저들의 돈을 다시 세팅해준다.
			for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

				if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 && 
					 g_CGameUserList.m_GameUserInfo[i].m_biPureUserWinLoseMoney > 0 ) 
				{

					if ( g_nWinnerSlotNo != i ) {
						sprintf( pTemp, "◎ %s님패 : (-)%I64u톨", 
						g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, g_CGameUserList.m_GameUserInfo[i].m_biPureUserWinLoseMoney );
                        SetGameChatText( pTemp , RGB( 185, 255, 102 ) );
					}
					else {
						sprintf( pTemp, "◎ %s님승리 : (+)%I64u톨", 
						g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, g_CGameUserList.m_GameUserInfo[i].m_biPureUserWinLoseMoney );	
                        SetGameChatText( pTemp , RGB( 185, 255, 102 ) );
					}

					//족보 보여주기.
					if ( g_CGameUserList.m_GameUserInfo[i].m_nCardCnt >= 7 ) {
						JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, szUserJokbo );
						sprintf( pTemp, "◎ %s님 카드 : %s", g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, szUserJokbo );
                        SetGameChatText( pTemp , RGB( 100, 255, 50 ) );
					}
					else if ( g_CGameUserList.m_GameUserInfo[i].m_bDie ) {
						sprintf( pTemp, "◎ %s님 다이", g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name );
                        SetGameChatText( pTemp , RGB( 100, 255, 50 ) );
					}
				}

				if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 && 
					 g_CGameUserList.m_GameUserInfo[i].m_biRealUserMoney > 0 )
                {
					g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney = g_CGameUserList.m_GameUserInfo[i].m_biRealUserMoney;
				}
			}

            SetGameChatText( "◎---------------◎" , RGB( 255, 255, 51 ));

			//보스 바꾸기.
			g_CGameUserList.NewSunSet( g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_sUserInfo.nUserNo );

			if ( g_nWinnerSlotNo == 0 ) 
				PlaySound(g_Sound[GS_WINNER]);
			else
				PlaySound(g_Sound[GS_LOSE]);
		}	
		
		m_nEndingShowTime++;
	
		if ( m_nEndingShowTime > ENDDING_TIME ) {

			//유저 자동 강퇴.
			if ( AvailableRoomIn( g_sRoomInfo.biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney, FALSE ) == FALSE ) {							

				//게임 초기화.
				GameInit();	

				g_pClient->RoomOut();
				g_pCMainFrame->m_Message->PutMessage( "최소머니 부족으로 퇴장되었습니다.", "Code - 500" );
			}

            else if ( g_CGameUserList.m_GameUserInfo[0].m_bExitReserve ) {

				g_pClient->RoomOut(); 

				//게임 초기화.
				GameInit();			
			}
			else {
				//게임 초기화.
				GameInit();
				g_pClient->GameReStart();
			}
		}
	}

    //----------------------------
	// 결과시에 순서대로 카드 오픈
    //----------------------------

	if ( m_bEndding ) {
		//0.5초.
		//m_nEndingShowTime / 25;	// 0~24 : 0번 슬롯.
		m_nEndingUserCurSlot =  ( g_nCurMasterSlotNo + ( m_nEndingUserCurTime/25) ) % MAX_ROOM_IN;	//현재 마스터부터 오픈한다.
		m_nEndingUserCurTime++;

		//확인 끝.
		if ( m_nEndingUserCurTime >= 126 ) {	        //5번을 지나야 하므로.
			m_bEndding = FALSE;
			m_bEnddingShow = TRUE;
            m_bMoveHiddenCard = FALSE;
		}

		if ( g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_nCardCnt > 0 && 
			 g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_bDie == FALSE ) 
		{
			if ( m_nEndingUserCurTime % 25 == 0 ) {     //카드를 확 핀다.

				for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_nCardCnt; i++ ) {
					g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[i].nCardNo = g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_byUserRealCardNo[i];
					g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[m_nEndingUserCurSlot].pCardStartPos.x + ( i * CARD_DISTANCE_B );
				}

				memcpy( g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_szJokboCard, 
					    g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_szTempJokboCard, 
					    sizeof(g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_szTempJokboCard) );

				g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_bEnd = TRUE;
				EndingSound( g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_szJokboCard[0] );
			}

			else {
				for ( int i = g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_nCardCnt; i > 0 ; i-- ) {
                    if ( ( m_nEndingUserCurTime - 1 ) % 25 == 0 ) {
                        PlaySound(g_Sound[GS_CARDSHRINK]);
                    }

					int nTempX = g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[i].Pos.x;
					nTempX -= i;

					//x 좌표를 점점 줄여주고 0번카드와 같게되면 0번카드 위치로 계속 유지한다.
                    if ( nTempX > g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[0].Pos.x ) {
                        g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[i].Pos.x = nTempX;
                    }
                    else {
                        g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[i].Pos.x = g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_sUserCardInfo[0].Pos.x;
                    }
				}
			}
		}
		else {
			//없는놈이면..바로 다음놈을 하기 위해서.
			m_nEndingUserCurTime = ( ( m_nEndingUserCurTime/25) + 1 ) * 25;
		}
	}

    //-----------------------------------------------
    // 게임 초기화를 시킨다.
    //-----------------------------------------------
    if ( m_bGameInit ) {
		DTRACE2("[%s]이벤트걸고 초기화 시작한다.", g_szUser_ID);

        m_bGameInit = FALSE;
        //게임 초기화.
		NetWorkThreaOnGameInit();		
    }

    //-----------------------------------------------
	// 유저가 선택한거 큰거 뿌려주고 작은거 뿌려주기.
    //-----------------------------------------------

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 ) {
			if ( g_CGameUserList.m_GameUserInfo[i].m_bUserSelectShow ) {
				if ( g_CGameUserList.m_GameUserInfo[i].m_nUserSelectCnt > 0 ) {
					g_CGameUserList.m_GameUserInfo[i].m_nUserSelectCnt--;					
				}
			}
		}
	}

    //------------------------
	// 카드의 애니메이션 처리.
    //------------------------

	for( i = 0; i <= m_CAniMation.GetAniCount(); i++ ) // 카드 이동 애니메이션 처리
	{
		sUserCardInfo *card = m_CAniMation.GetCard(i);

        if ( card->nAniCount == 1 ) {
            PlaySound(g_Sound[GS_DEAL]);
        }

        if(card == NULL) {
            continue;
        }

		//DTRACE( "X : %d, Y : %d", card->Pos.x, card->Pos.y  );
		//줄이면 느려지고.
		if( CheckPosition(card) ) {

			card->Pos = card->NewPos; 
			card->ctCardType = USERCARD;

			card->nAniCount = 0;
			card->nAniMaxCount = -1;

			m_CAniMation.DeleteStack(i);
		}
		else {	//이걸 넣어주면..차례대로 카드가 날아간다.
			break;
		}
	}

    //---------------------------------------------------
    // 톨 에니메이션 처리
    //
    // 모두 이동한 톨은 제거되는 톨으로 타입이 바뀌며,
    // 제거되는 톨은 렌더링 횟수에 다라 점점 투명해지면서
    // 사라진다.
    //---------------------------------------------------

    for ( i = 0; i <= m_ChipAni.GetAniCount(); i++ ) {

        sChipInfo *chip = m_ChipAni.GetChip( i );

        if ( chip == NULL || chip->ctChipType == CT_NONE ) {
            continue;
        }

        if ( chip->ctChipType == CT_MOVE ) {        // 이동중인 톨 처리(이동이 완료되면 사라지도록 설정한다)

            if (CheckChipPosition( chip )) {
                chip->Pos = chip->NewPos;
                chip->ctChipType = CT_REMOVE;
                chip->nAniCount = 0;
                chip->nAniMaxCount = -1;
            }
        }
        else if ( chip->ctChipType == CT_REMOVE ) { // 사라지는 톨들을 카운터를 증가시키다가 시간이 되면 제거한다.
            chip->nRemoveCnt++;

            if ( chip->nRemoveCnt >= REMOVE_CHIP_TIME ) {

                m_ChipAni.DeleteStack( i );

                if ( m_ChipAni.GetAniCount() <= 0 ) {   // 톨이 다 이동했으면 판 톨 갱신
                    PanMoneyReFresh();
                }
            }
        }
    }
}



//========================================================================
// 카드 위치 갱신
//========================================================================

BOOL CMainFrame::CheckPosition( sUserCardInfo *pos )    // pos->NewPos로 pos->Pos를 이동
{
	double w = pos->NewPos.x - pos->StartPos.x;
	double h = pos->NewPos.y - pos->StartPos.y;

	//속도	//커지면 천천히. 느
	pos->nAniMaxCount = g_nGameSpeed;
	
	pos->Pos.x = pos->StartPos.x + (int)((double)(w / pos->nAniMaxCount * pos->nAniCount));
	pos->Pos.y = pos->StartPos.y + (int)((double)(h / pos->nAniMaxCount * pos->nAniCount));
	
	pos->nAniCount++;

	if(pos->nAniCount >= pos->nAniMaxCount) return TRUE;
	return FALSE;
}



//========================================================================
// 톨 위치 갱신
//========================================================================

BOOL CMainFrame::CheckChipPosition( sChipInfo *pos )    // pos->NewPos로 pos->Pos를 이동
{
	double w = pos->NewPos.x - pos->StartPos.x;
	double h = pos->NewPos.y - pos->StartPos.y;

	//속도	//커지면 천천히. 느
	pos->nAniMaxCount = g_nGameSpeed;
	
	pos->Pos.x = pos->StartPos.x + (int)((double)(w / pos->nAniMaxCount * pos->nAniCount));
	pos->Pos.y = pos->StartPos.y + (int)((double)(h / pos->nAniMaxCount * pos->nAniCount));
	
	pos->nAniCount++;

	if(pos->nAniCount >= pos->nAniMaxCount) return TRUE;
	return FALSE;
}



//========================================================================
// 에니메이션 업데이트 스레드
//========================================================================

unsigned __stdcall AniUpdateThreadProc( LPVOID lParameter ) // ham 2005.11.29
{
	//이바타 때문에 m_bActive 이게 FALSE가 되는 순간이 있다.
	while( g_pCMainFrame->m_bAniActive ) {
		g_pCMainFrame->m_fSkipTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

		if( g_pCMainFrame->m_bAniActive ) {
			if( g_pCMainFrame->m_FrameSkip.Update( g_pCMainFrame->m_fSkipTime ) ) {
                g_pCMainFrame->AnimationUpdate();
			}
        }

        Sleep(5);
	}

    DTRACE2( "[%s] 에니메이션 스레드가 종료되었다.", g_szUser_ID );

    _endthreadex( 0 );
	return 0;
}



//========================================================================
// 문자 입력용 IME 생성하고 초기화
//========================================================================

void CMainFrame::LoadControl( void )
{
	//////////////////////////////////////////////////////////////////////////			
	// 대기실의 채팅 컨트롤을 만든다.
	//////////////////////////////////////////////////////////////////////////
	
    // 대기실 채팅창 메시지 입력용 IME 초기화
    m_IME_WaitChat  = new CDXIME( g_pCDXScreen, g_hWnd, 98, TRUE, FALSE );
    m_IME_WaitChat->InitCaret();

    // 게임화면 채팅창 메시지 입력용 IME 초기화
    m_IME_GameChat  = new CDXIME( g_pCDXScreen, g_hWnd, 50, TRUE, FALSE );
    m_IME_GameChat->InitCaret();

    // 방만들기 - 비밀방 만들때 비밀번호 입력용 IME 초기화
    m_IME_InputPass = new CDXIME( g_pCDXScreen, g_hWnd, 4, TRUE, FALSE );
    m_IME_InputPass->SetTextLen( 4 );           // 입력가능한 글자 수
    m_IME_InputPass->InitCaret();               // 커서 초기화
    m_IME_InputPass->SetPassword( true );       // 비밀번호 입력모드로 설정
    m_IME_InputPass->SetPasswordChar( '*' );    // 비밀번호 출력용 문자 설정 : 기본값은 '*'

    // 방만들기 - 방제목 입력용 IME 초기화
    m_IME_InputRoomTitle = new CDXIME( g_pCDXScreen, g_hWnd, MAX_ROOM_NAME - 1, TRUE, FALSE );
    m_IME_InputRoomTitle->SetTextLen( MAX_ROOM_NAME - 1 );
    m_IME_InputRoomTitle->InitCaret();

    // 방 입장시 비밀번호 입력용 IME 초기화
    m_IME_RoomInPassBox = new CDXIME( g_pCDXScreen, g_hWnd, 4, TRUE, FALSE );
    m_IME_RoomInPassBox->SetTextLen( 4 );
    m_IME_RoomInPassBox->InitCaret();
    m_IME_RoomInPassBox->SetPassword ( true );
    m_IME_RoomInPassBox->SetPasswordChar( '*' );

    // 쪽지 입력용 IME 초기화
    m_IME_Memo = new CDXIME( g_pCDXScreen, g_hWnd, 40, TRUE, FALSE );
    m_IME_Memo->SetTextLen( 40 );
    m_IME_Memo->InitCaret();
    m_IME_InputRoomTitle->SetIMEText( g_RoomTitle[0] );

}



void CMainFrame::LoadEnviroMent()
{
	char szBuf[255];
	memset( szBuf, 0x00, sizeof(szBuf) );

	int nTemp = 0;

	if ( nTemp >= 0 ) {
		GetPrivateProfileString( "Environment", "Invite", "1", szBuf, sizeof(szBuf), ".\\config.ini" );
		g_sClientEnviro.bOption[0] = atoi(szBuf);
		
		GetPrivateProfileString( "Environment", "Eartalk", "1", szBuf, sizeof(szBuf), ".\\config.ini" );
		g_sClientEnviro.bOption[1] = atoi(szBuf);

		GetPrivateProfileString( "Environment", "Sound", "1", szBuf, sizeof(szBuf), ".\\config.ini" );
		g_sClientEnviro.bOption[2] = atoi(szBuf);

		GetPrivateProfileString( "Environment", "MemoRecv", "1", szBuf, sizeof(szBuf), ".\\config.ini" );
		g_sClientEnviro.bOption[3] = atoi(szBuf);

		GetPrivateProfileString( "Environment", "Voice", "0", szBuf, sizeof(szBuf), ".\\config.ini" );
		g_sClientEnviro.nVoice = atoi(szBuf);	

	}
	else {
		WritePrivateProfileString("Environment", "Invite",   "1", ".\\config.ini");
		WritePrivateProfileString("Environment", "Eartalk",  "1", ".\\config.ini");
		WritePrivateProfileString("Environment", "Sound",    "1", ".\\config.ini");
		WritePrivateProfileString("Environment", "MemoRecv", "1", ".\\config.ini");
		WritePrivateProfileString("Environment", "Voice",    "0", ".\\config.ini");

		g_sClientEnviro.bOption[0] = TRUE;
		g_sClientEnviro.bOption[1] = TRUE;
		g_sClientEnviro.bOption[2] = TRUE;
		g_sClientEnviro.bOption[3] = TRUE;
		g_sClientEnviro.nVoice = 0;
	}
}



void CMainFrame::LoadData()
{
    char szTemp[255];
	memset( szTemp, 0x00, sizeof(szTemp) );

	unsigned dwThreadId; 
	m_hSocketThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadProc, (LPVOID)g_pClient, 0, &dwThreadId );

    // Common Image ------------------------------------------------------

    m_SPR_LoGo          = LoadSprite( ".\\image\\common\\logo.spr",             1024, 768,  1 );
    m_SPR_Cursor        = LoadSprite( ".\\image\\common\\cursor.spr",             32,  32,  2 );
	m_SPR_Connting      = LoadSprite( ".\\image\\common\\connecting.spr",        300,  60,  1 );
    m_SPR_Class         = LoadSprite( ".\\image\\common\\icon_class.spr",         16,  16,  6 );
    m_SPR_CheckBall     = LoadSprite( ".\\image\\common\\icon_check_ball.spr",    14,  14,  2 );
    m_SPR_Message       = LoadSprite( ".\\image\\common\\dlg_alram.spr",         325, 230,  1 );
    m_SPR_ConBox        = LoadSprite( ".\\image\\common\\dlg_config.spr",        325, 267,  1 );
    m_SPR_Invitation    = LoadSprite( ".\\image\\common\\dlg_invite.spr",        325, 230,  1 );
    m_SPR_InviteBox     = LoadSprite( ".\\image\\common\\dlg_invite_box.spr",    325, 420,  1 );
    m_SPR_MessageBox    = LoadSprite( ".\\image\\common\\dlg_message.spr",       325, 267,  1 );
    m_SPR_WaitAvatar    = LoadSprite( ".\\image\\common\\defaultuser.spr",       100, 152,  1 );

    for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_SPR_GameAvatar[i] = LoadSprite(".\\image\\common\\defaultuser.spr", 100, 152, 1 );
	}

    m_InviteOKBTN       = LoadButton(".\\image\\common\\btn_invite_ok.spr",  CHODAE_POS_X +  41, CHODAE_POS_Y + 364, 107, 30 );	
    m_InviteCancelBTN   = LoadButton(".\\image\\common\\btn_cancel.spr",     CHODAE_POS_X + 178, CHODAE_POS_Y + 364, 107, 30 );	
    m_ChodaeYesBTN      = LoadButton(".\\image\\common\\btn_yes.spr",               0,   0, 107, 30 );
    m_ChodaeNoBTN       = LoadButton(".\\image\\common\\btn_no.spr",                0,   0, 107, 30 );
    m_MemoSendBTN       = LoadButton(".\\image\\common\\btn_send.spr",              0,   0, 107, 30 );
    m_MemoCancelBTN     = LoadButton(".\\image\\common\\btn_cancel.spr",            0,   0, 107, 30 );
    m_MessageOKBTN      = LoadButton(".\\image\\common\\btn_ok.spr",                0,   0, 107, 30 );
    m_OkBTN             = LoadButton(".\\image\\common\\btn_ok.spr",                0,   0, 107, 30 );
	m_CancelBTN         = LoadButton(".\\image\\common\\btn_cancel.spr",            0,   0, 107, 30 );

    // Lobby Image -------------------------------------------------------

    m_SPR_WaitBase      = LoadSprite(".\\image\\lobby\\waitbase.spr",           1024, 768,   1 );
    m_SPR_FullWait      = LoadSprite(".\\image\\lobby\\icon_fullwait.spr",        22,  22,   4 );
    m_SRP_SexIcon       = LoadSprite(".\\image\\lobby\\icon_sex.spr",             13,  20,   2 );
    m_SPR_SelRoomBar    = LoadSprite(".\\image\\lobby\\sel_room_bar.spr",        610,  29,   1 );
    m_SPR_HighlightBar  = LoadSprite(".\\image\\lobby\\select.spr",              237,  24,   1 );
    m_SPR_UserState		= LoadSprite(".\\image\\lobby\\UserState.spr",	          92,  21,   1 );
    m_SPR_MakeRoom      = LoadSprite(".\\image\\lobby\\dlg_makeroom.spr",        325, 351,   1 );
    m_SPR_PassDlg       = LoadSprite(".\\image\\lobby\\dlg_pass.spr",            325, 173,   1 );
    m_SPR_ChodaeOX      = LoadSprite(".\\image\\lobby\\chodae_ox.spr",            13,  13,   2 );
    m_SPR_UserListTitle[0] = LoadSprite(".\\image\\lobby\\title_wait_user.spr",  297,  40,   1 );
    m_SPR_UserListTitle[1] = LoadSprite(".\\image\\lobby\\title_room_user.spr",  297,  40,   1 );
    
    m_ExitBTN           = LoadButton(".\\image\\lobby\\btn_exit.spr",           1000,   5,  16, 16 );
    m_Exit2BTN          = LoadButton(".\\image\\lobby\\btn_exit2.spr",            29, 555, 107, 30 );
//    m_Exit2BTN          = LoadButton(".\\image\\lobby\\btn_exit2.spr",            29, 705, 107, 30 );

	
	m_MaxBTN            = LoadButton(".\\image\\lobby\\btn_max.spr",             981,   5,  16, 16 );
    m_MaxBTN2           = LoadButton(".\\image\\lobby\\btn_max2.spr",            981,   5,  16, 16 );
    m_MinBTN            = LoadButton(".\\image\\lobby\\btn_min.spr",             962,   5,  16, 16 );
    m_MakeRoomBTN       = LoadButton(".\\image\\lobby\\btn_makeroom.spr",        392, 547, 107, 30 );
//    m_MakeRoomBTN       = LoadButton(".\\image\\lobby\\btn_makeroom.spr",        392, 697, 107, 30 );

   m_JoinBTN           = LoadButton(".\\image\\lobby\\btn_join.spr",            142, 555, 107, 30 );
 //   m_JoinBTN           = LoadButton(".\\image\\lobby\\btn_join.spr",            142, 705, 107, 30 );

   m_QuickJoinBTN      = LoadButton(".\\image\\lobby\\btn_quick_join.spr",      511, 535, 125, 43 );
//
//    m_QuickJoinBTN      = LoadButton(".\\image\\lobby\\btn_quick_join.spr",      511, 685, 125, 43 );
    
	m_RoomTitleBTN      = LoadButton(".\\image\\lobby\\btn_room_title.spr",      626, 269,  16, 16 );
 //   m_MakeRoomOkBTN     = LoadButton(".\\image\\lobby\\btn_makeroom_ok.spr",     390, 502, 107, 30 );
    m_MakeRoomOkBTN     = LoadButton(".\\image\\lobby\\btn_makeroom_ok.spr",     390, 466, 107, 30 );

  //  m_MakeRoomCancelBTN = LoadButton(".\\image\\lobby\\btn_makeroom_cancel.spr", 527, 502, 107, 30 );
   m_MakeRoomCancelBTN = LoadButton(".\\image\\lobby\\btn_makeroom_cancel.spr", 527, 466, 107, 30 );

    m_CaptureBTN        = LoadButton(".\\image\\lobby\\btn_capture.spr",         804,   4,  71, 19 );
    m_ConfigBTN         = LoadButton(".\\image\\lobby\\btn_config.spr",          881,   4,  71, 19 );
    m_UserStateBTN      = LoadButton(".\\image\\lobby\\btn_user_state.spr",      490, 352,  87, 18 );

    // Game Play Image ---------------------------------------------------

    m_SPR_GameBase      = LoadSprite(".\\image\\game\\gamebase.spr",          1024, 768,  1 );
    m_SPR_AllIn         = LoadSprite(".\\image\\game\\allin.spr",              121,  96,  1 );
    m_SPR_Sun           = LoadSprite(".\\image\\game\\icon_boss.spr",           42,  29,  1 );
    m_SPR_BangJang      = LoadSprite(".\\image\\game\\icon_chief.spr",          17,  17,  1 );	
    m_SPR_Enter         = LoadSprite(".\\image\\game\\btn_enter.spr",           41,  23,  1 );
    m_SPR_WinnerMark    = LoadSprite(".\\image\\game\\spr_win.spr",            143,  53,  6 );
    m_SPR_Start         = LoadSprite(".\\image\\game\\spr_start.spr",          150,  45,  2 );
    m_SPR_USERTIME      = LoadSprite(".\\image\\game\\spr_timer.spr",          224,  17, 11 );
    m_SPR_WindowWin     = LoadSprite(".\\image\\game\\dlg_win.spr",            304, 200,  1 );
    m_SPR_WasteCard     = LoadSprite(".\\image\\game\\dlg_card_del.spr",       396, 200,  1 );
    m_SPR_CardChioce    = LoadSprite(".\\image\\game\\dlg_card_open.spr",      304, 200,  1 );
    m_SPR_ShowUserInfo  = LoadSprite(".\\image\\game\\dlg_user_info.spr",      266, 224,  1 );
    m_SPR_BetType_Small = LoadSprite(".\\image\\game\\bet_type_small.spr",      27,  15,  8 );
    m_SPR_BetType_Big   = LoadSprite(".\\image\\game\\bet_type_big.spr",        65,  70, 16 );
    m_SPR_Chip          = LoadSprite(".\\image\\game\\chip.spr",                40,  30, 16 );
    m_SPR_JackPot       = LoadSprite(".\\image\\game\\jackpot.spr",            199,  63,  1 );
	m_SPR_JackPotMath   = LoadSprite(".\\image\\game\\jackpotmath.spr",         10,  22, 48 );
    m_SPR_JackPotBack   = LoadSprite(".\\image\\game\\jackpot_back.spr",       303,  85,  2 );
  
	
	
	m_SPR_SelectUser    = LoadSprite(".\\image\\game\\select_user.spr",        224, 127,  1 );
    m_SPR_BetMoneyBack  = LoadSprite(".\\image\\game\\bet_money_bg.spr",        70,  19,  1 );
    m_SPR_Arrow         = LoadSprite(".\\image\\game\\arrow.spr",               21,  11,  1 );

    m_SPR_CardSpade     = LoadSprite(".\\image\\game\\card_spade.spr",  82, 109, 26 );
	m_SPR_CardClover    = LoadSprite(".\\image\\game\\card_clover.spr", 82, 109, 26 );
	m_SPR_CardDia       = LoadSprite(".\\image\\game\\card_dia.spr",    82, 109, 26 );
	m_SPR_CardHeart     = LoadSprite(".\\image\\game\\card_heart.spr",  82, 109, 26 );
	m_SPR_CardBack      = LoadSprite(".\\image\\game\\card_back.spr",   82, 109,  1 );

    m_StartBTN          = LoadButton(".\\image\\game\\btn_start.spr",           437, 361, 150, 45 );
    m_UserCloseBTN      = LoadButton(".\\image\\game\\btn_user_info_close.spr", 576, 468,  53, 16 );
    m_FullBTN           = LoadButton(".\\image\\game\\btn_board_fullmode.spr",  908, 721,  45, 26 );
	m_InviteBTN         = LoadButton(".\\image\\game\\btn_board_invite.spr",    861, 721,  45, 26 );
/*
    m_DDaDDang  = LoadButton(".\\image\\game\\btn_bet_double.spr",   704, 587, 70, 43 ); // [*BTN]
    m_Check     = LoadButton(".\\image\\game\\btn_bet_check.spr",    741, 587, 70, 43 );
	m_Quater    = LoadButton(".\\image\\game\\btn_bet_quarter.spr",  816, 587, 70, 43 );
    m_BBing     = LoadButton(".\\image\\game\\btn_bet_bbing.spr",    891, 587, 70, 43 );
	m_Half      = LoadButton(".\\image\\game\\btn_bet_half.spr",     741, 637, 70, 43 );
    m_Call      = LoadButton(".\\image\\game\\btn_bet_call.spr",     816, 637, 70, 43 );
	m_Full      = LoadButton(".\\image\\game\\btn_bet_full.spr",     854, 637, 70, 43 ); // [*BTN]
	m_Die       = LoadButton(".\\image\\game\\btn_bet_die.spr",      891, 637, 70, 43 );
 */   
    m_DDaDDang  = LoadButton(".\\image\\game\\btn_bet_double.spr",   704, 587, 70, 43 );
    m_Check     = LoadButton(".\\image\\game\\btn_bet_check.spr",    779, 587, 70, 43 );
	m_Quater    = LoadButton(".\\image\\game\\btn_bet_quarter.spr",  854, 587, 70, 43 );
    m_BBing     = LoadButton(".\\image\\game\\btn_bet_bbing.spr",    929, 587, 70, 43 );
	m_Half      = LoadButton(".\\image\\game\\btn_bet_half.spr",     704, 637, 70, 43 );
    m_Call      = LoadButton(".\\image\\game\\btn_bet_call.spr",     779, 637, 70, 43 );
	m_Full      = LoadButton(".\\image\\game\\btn_bet_full.spr",     854, 637, 70, 43 );
	m_Die       = LoadButton(".\\image\\game\\btn_bet_die.spr",      929, 637, 70, 43 );
  
	m_pConfigBox = new CONFIGBOX();

    for ( i = 0 ; i < 4; i++ ) {
        m_pConfigBox->bCheck[i] = g_sClientEnviro.bOption[i];
    }

	m_pConfigBox->nVoice = g_sClientEnviro.nVoice;

	m_Message = new GAMEMESSAGE();


    RECT sScrollBarRect;
    memset(&sScrollBarRect, 0x00, sizeof(sScrollBarRect));


    // 대기실 채팅창 스크롤 바
    
    SetRect( &sScrollBarRect, 649, 609, 673, 729 );

    m_SCB_WaitChat = LoadScrollBar(
        ".\\image\\lobby\\btn_scroll_up.spr",
        ".\\image\\lobby\\btn_scroll_down.spr",
        ".\\image\\lobby\\btn_scroll_boll.spr",
        &sScrollBarRect,
        25, 22, 23, 19,
        MAX_VIEW_WAIT_CHAT,
        MAX_TEXT_LINE
    );


    // 대기자 스크롤 바

    SetRect( &sScrollBarRect, 963, 227, 987, 534 );

    m_SCB_WaitUser = LoadScrollBar(
        ".\\image\\lobby\\btn_scroll_up.spr",
        ".\\image\\lobby\\btn_scroll_down.spr",
        ".\\image\\lobby\\btn_scroll_boll.spr",
        &sScrollBarRect,
        25, 22, 23, 19,
        MAX_VIEW_WAIT_USER,
        MAX_WAIT_USER
    );


    // 방 리스트 스크롤 바

    SetRect( &sScrollBarRect, 651, 208, 675, 531 );

    m_SCB_WaitRoom = LoadScrollBar(
        ".\\image\\lobby\\btn_scroll_up.spr",
        ".\\image\\lobby\\btn_scroll_down.spr",
        ".\\image\\lobby\\btn_scroll_boll.spr",
        &sScrollBarRect,
        25, 22, 23, 19,
        MAX_VIEW_WAIT_ROOM,
        MAX_ROOMCNT
    );


    // 게임화면 채팅창 스크롤 바

    SetRect( &sScrollBarRect, 314, 557, 329, 728 );

    m_SCB_GameChat = LoadScrollBar(
        ".\\image\\game\\btn_gamechat_up.spr",
        ".\\image\\game\\btn_gamechat_down.spr",
        ".\\image\\game\\btn_gamechat_bar.spr",
        &sScrollBarRect,
        16, 16, 14, 13,
        MAX_VIEW_GAME_CHAT,
        MAX_TEXT_LINE
    );

    // 초대 창 스크롤 바

    SetRect(
        &sScrollBarRect,
        CHODAE_POS_X + 284,
        CHODAE_POS_Y + 85,
        CHODAE_POS_X + 299,
        CHODAE_POS_Y + 351
    );

    m_SCB_Invite = LoadScrollBar(
        ".\\image\\common\\scb_invite_up.spr",
        ".\\image\\common\\scb_invite_down.spr",
        ".\\image\\common\\scb_invite_boll.spr",
        &sScrollBarRect,
        16, 16, 16, 16,
        MAX_VIEW_CHODAE_USER,
        MAX_CHANNELPER
    );

    SetColorKey();
	SoundLoading();
}



//========================================================================
// 모든 이미지의 컬러키를 설정한다.
//
// 화면 모드가 변경되면 이 함수를 호출해 주어야 한다.
//========================================================================

void CMainFrame::SetColorKey()
{
    int i;
    RGBFORMAT pixelFormat;
    DWORD dwColorKey;

    g_pCDXScreen->GetFront()->GetPixelFormat(&pixelFormat);

    switch( pixelFormat.bpp ) {
        case 15:    dwColorKey = RGB16BIT555( 255, 0, 255 );    break;
        case 16:    dwColorKey = RGB16BIT565( 255, 0, 255 );    break;
        case 24:    dwColorKey = RGB24BIT( 255, 0, 255 );       break;
        case 32:    dwColorKey = RGB32BIT( 255, 0, 255 );       break;
    }

	if ( m_SPR_Cursor       != NULL ) m_SPR_Cursor->SetColorKey( dwColorKey );
	if ( m_SPR_Connting     != NULL ) m_SPR_Connting->SetColorKey( dwColorKey );
	if ( m_SPR_MessageBox   != NULL ) m_SPR_MessageBox->SetColorKey( dwColorKey );
	if ( m_SPR_Message      != NULL ) m_SPR_Message->SetColorKey( dwColorKey );
	if ( m_SPR_ConBox       != NULL ) m_SPR_ConBox->SetColorKey( dwColorKey );

    if ( m_SPR_WaitBase     != NULL ) m_SPR_WaitBase->SetColorKey( dwColorKey );
    if ( m_SPR_MakeRoom     != NULL ) m_SPR_MakeRoom->SetColorKey( dwColorKey );
	if ( m_SPR_BetType_Big  != NULL ) m_SPR_BetType_Big->SetColorKey( dwColorKey );
	if ( m_SPR_GameBase     != NULL ) m_SPR_GameBase->SetColorKey( dwColorKey );
	if ( m_SPR_LoGo         != NULL ) m_SPR_LoGo->SetColorKey( dwColorKey );
    
	if ( m_SPR_Class        != NULL ) m_SPR_Class->SetColorKey( dwColorKey );
	if ( m_SPR_WaitAvatar   != NULL ) m_SPR_WaitAvatar->SetColorKey( dwColorKey );
	if ( m_SPR_FullWait     != NULL ) m_SPR_FullWait->SetColorKey( dwColorKey );
	if ( m_SRP_SexIcon      != NULL ) m_SRP_SexIcon->SetColorKey( dwColorKey );
	if ( m_SPR_Start        != NULL ) m_SPR_Start->SetColorKey( dwColorKey );
	if ( m_SPR_Sun          != NULL ) m_SPR_Sun->SetColorKey( dwColorKey );
	if ( m_SPR_BangJang     != NULL ) m_SPR_BangJang->SetColorKey( dwColorKey );
	if ( m_SPR_CardSpade    != NULL ) m_SPR_CardSpade->SetColorKey( dwColorKey );
	if ( m_SPR_CardClover   != NULL ) m_SPR_CardClover->SetColorKey( dwColorKey );
	if ( m_SPR_CardDia      != NULL ) m_SPR_CardDia->SetColorKey( dwColorKey );
	if ( m_SPR_CardHeart    != NULL ) m_SPR_CardHeart->SetColorKey( dwColorKey );
	if ( m_SPR_CardBack     != NULL ) m_SPR_CardBack->SetColorKey( dwColorKey );
	if ( m_SPR_CardChioce       != NULL ) m_SPR_CardChioce->SetColorKey( dwColorKey );
	if ( m_SPR_BetType_Small    != NULL ) m_SPR_BetType_Small->SetColorKey( dwColorKey );
	if ( m_SPR_WindowWin        != NULL ) m_SPR_WindowWin->SetColorKey( dwColorKey );
	if ( m_SPR_WinnerMark       != NULL ) m_SPR_WinnerMark->SetColorKey( dwColorKey );
	if ( m_SPR_USERTIME         != NULL ) m_SPR_USERTIME->SetColorKey( dwColorKey );
	if ( m_SPR_Chip             != NULL ) m_SPR_Chip->SetColorKey( dwColorKey );

	if ( m_SPR_JackPot          != NULL ) m_SPR_JackPot->SetColorKey( dwColorKey );
    if ( m_SPR_JackPotBack      != NULL ) m_SPR_JackPotBack->SetColorKey( dwColorKey );
	if ( m_SPR_AllIn            != NULL ) m_SPR_AllIn->SetColorKey( dwColorKey );
	if ( m_SPR_JackPotMath      != NULL ) m_SPR_JackPotMath->SetColorKey( dwColorKey );
    if ( m_SPR_UserListTitle[0] != NULL ) m_SPR_UserListTitle[0]->SetColorKey( dwColorKey );
    if ( m_SPR_UserListTitle[1] != NULL ) m_SPR_UserListTitle[1]->SetColorKey( dwColorKey );
    if ( m_SPR_HighlightBar     != NULL ) m_SPR_HighlightBar->SetColorKey( dwColorKey );
    if ( m_SPR_SelRoomBar       != NULL ) m_SPR_SelRoomBar->SetColorKey( dwColorKey );
    if ( m_SPR_SelectUser       != NULL ) m_SPR_SelectUser->SetColorKey( dwColorKey );
    if ( m_SPR_BetMoneyBack     != NULL ) m_SPR_BetMoneyBack->SetColorKey( dwColorKey );
    if ( m_SPR_Arrow            != NULL ) m_SPR_Arrow->SetColorKey( dwColorKey );
	
	if ( m_SPR_InviteBox    != NULL ) m_SPR_InviteBox->SetColorKey( dwColorKey );
	if ( m_SPR_Invitation   != NULL ) m_SPR_Invitation->SetColorKey( dwColorKey );
	if ( m_SPR_Enter        != NULL ) m_SPR_Enter->SetColorKey( dwColorKey );
	if ( m_SPR_WasteCard    != NULL ) m_SPR_WasteCard->SetColorKey( dwColorKey );
	if ( m_SPR_ShowUserInfo != NULL ) m_SPR_ShowUserInfo->SetColorKey( dwColorKey );
	if ( m_SPR_UserState    != NULL ) m_SPR_UserState->SetColorKey( dwColorKey );
    if ( m_SPR_CheckBall    != NULL ) m_SPR_CheckBall->SetColorKey( dwColorKey );
    if ( m_SPR_PassDlg      != NULL ) m_SPR_PassDlg->SetColorKey( dwColorKey );
    if ( m_SPR_ChodaeOX     != NULL ) m_SPR_ChodaeOX->SetColorKey( dwColorKey );

	if ( m_DDaDDang         != NULL ) m_DDaDDang->SetColorKey( dwColorKey ); // [*BTN]
	if ( m_Quater           != NULL ) m_Quater->SetColorKey( dwColorKey );
	if ( m_Half             != NULL ) m_Half->SetColorKey( dwColorKey );
	if ( m_Full             != NULL ) m_Full->SetColorKey( dwColorKey ); // [*BTN]
	if ( m_Check            != NULL ) m_Check->SetColorKey( dwColorKey );
	if ( m_BBing            != NULL ) m_BBing->SetColorKey( dwColorKey );
	if ( m_Call             != NULL ) m_Call->SetColorKey( dwColorKey );
	if ( m_Die              != NULL ) m_Die->SetColorKey( dwColorKey );

	if ( m_MessageOKBTN     != NULL ) m_MessageOKBTN->SetColorKey( dwColorKey );
	if ( m_MaxBTN           != NULL ) m_MaxBTN->SetColorKey( dwColorKey );
	if ( m_MaxBTN2          != NULL ) m_MaxBTN2->SetColorKey( dwColorKey );
	if ( m_ExitBTN          != NULL ) m_ExitBTN->SetColorKey( dwColorKey );
	if ( m_ConfigBTN        != NULL ) m_ConfigBTN->SetColorKey( dwColorKey );
	if ( m_Exit2BTN         != NULL ) m_Exit2BTN->SetColorKey( dwColorKey );
	if ( m_MinBTN           != NULL ) m_MinBTN->SetColorKey( dwColorKey );

	if ( m_FullBTN          != NULL ) m_FullBTN->SetColorKey( dwColorKey );
	if ( m_OkBTN            != NULL ) m_OkBTN->SetColorKey( dwColorKey );

	if ( m_MakeRoomBTN      != NULL ) m_MakeRoomBTN->SetColorKey( dwColorKey );
	if ( m_JoinBTN          != NULL ) m_JoinBTN->SetColorKey( dwColorKey );
	if ( m_QuickJoinBTN     != NULL ) m_QuickJoinBTN->SetColorKey( dwColorKey );
	if ( m_CancelBTN        != NULL ) m_CancelBTN->SetColorKey( dwColorKey );
    if ( m_RoomTitleBTN     != NULL ) m_RoomTitleBTN->SetColorKey( dwColorKey );
    if ( m_MakeRoomOkBTN    != NULL ) m_MakeRoomOkBTN->SetColorKey( dwColorKey );
    if ( m_MakeRoomCancelBTN != NULL ) m_MakeRoomCancelBTN->SetColorKey( dwColorKey );

	//Game Room

	if ( m_InviteBTN        != NULL ) m_InviteBTN->SetColorKey( dwColorKey );
	if ( m_StartBTN         != NULL ) m_StartBTN->SetColorKey( dwColorKey );
	if ( m_InviteOKBTN      != NULL ) m_InviteOKBTN->SetColorKey( dwColorKey );
    if ( m_InviteCancelBTN  != NULL ) m_InviteCancelBTN->SetColorKey( dwColorKey );
	if ( m_ChodaeYesBTN     != NULL ) m_ChodaeYesBTN->SetColorKey( dwColorKey );
	if ( m_ChodaeNoBTN      != NULL ) m_ChodaeNoBTN->SetColorKey( dwColorKey );
	if ( m_UserCloseBTN     != NULL ) m_UserCloseBTN->SetColorKey( dwColorKey );
	if ( m_UserStateBTN     != NULL ) m_UserStateBTN->SetColorKey( dwColorKey );
	if ( m_MemoSendBTN      != NULL ) m_MemoSendBTN->SetColorKey( dwColorKey );
	if ( m_MemoCancelBTN    != NULL ) m_MemoCancelBTN->SetColorKey( dwColorKey );
    if ( m_CaptureBTN       != NULL ) m_CaptureBTN->SetColorKey( dwColorKey );

    if ( m_SCB_WaitChat     != NULL ) m_SCB_WaitChat->SetColorKey( dwColorKey );
    if ( m_SCB_WaitUser     != NULL ) m_SCB_WaitUser->SetColorKey( dwColorKey );
    if ( m_SCB_WaitRoom     != NULL ) m_SCB_WaitRoom->SetColorKey( dwColorKey );
    if ( m_SCB_GameChat     != NULL ) m_SCB_GameChat->SetColorKey( dwColorKey );
    if ( m_SCB_Invite       != NULL ) m_SCB_Invite->SetColorKey( dwColorKey );

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_SPR_GameAvatar[i]    != NULL ) m_SPR_GameAvatar[i]->SetColorKey( dwColorKey );
	}

    // FillRect() 함수에서 사용하기 위한 색깔을, 현재 화면 모드에 맞게 구한다.
    GetFillRectColor( pixelFormat.bpp );
}



//========================================================================
// FillRect() 함수에서 사용하기 위한 색깔을, 현재 화면 모드에 맞게 구한다.
//
// NOTE! - FillRect() 함수에서 사용하는 색깔은 R, G, B 순서가 아니라
//         B, G, R 순서이다. RGB( Blue, Green, Red ).. 이런식으로 해주어야
//         한다.
//
// input:
//      bpp:    현재 화면 모드의 픽셀 깊이( 8, 15, 16, 24, 32 )
//========================================================================

void CMainFrame::GetFillRectColor( UINT bpp )
{
    switch( bpp ) {
        case 15:
            m_rgbMakeRoomTitleListBorder = RGB16BIT555( 51, 26, 5 );
            m_rgbMakeRoomTitleListBar = RGB16BIT555( 183, 112, 15 );
            m_rgbGray = RGB16BIT555( 208, 208, 208 );
            break;

        case 16:
            //--------------------------------------------------------------------------------------
            //
            // NOTE!!!
            //
            // FillRect() 함수에서 사용하는 색깔에서... 다른 모드는 Blue, Green, Red 순서인데반해서,
            // 16bit 565 모드에서는 Red, Green, Blue 순서이다.
            //
            //--------------------------------------------------------------------------------------

            m_rgbMakeRoomTitleListBorder = RGB16BIT565( (int)((float)31 * ((float)5 / (float)255)),
                                                        (int)((float)63 * ((float)26 / (float)255)),
                                                        (int)((float)31 * ((float)51  / (float)255)));

            m_rgbMakeRoomTitleListBar = RGB16BIT565( (int)((float)31 * ((float)15 / (float)255)),
                                                     (int)((float)63 * ((float)112 / (float)255)),
                                                     (int)((float)31 * ((float)183  / (float)255)));

            m_rgbGray = RGB16BIT565( (int)((float)31 * ((float)208 / (float)255)),
                                     (int)((float)63 * ((float)208 / (float)255)),
                                     (int)((float)31 * ((float)208 / (float)255)));
            break;

        case 24:
            m_rgbMakeRoomTitleListBorder = RGB24BIT( 51, 26, 5 );
            m_rgbMakeRoomTitleListBar = RGB24BIT( 183, 112, 15 );
            m_rgbGray = RGB24BIT( 208, 208, 208 );
            break;

        case 32:
            m_rgbMakeRoomTitleListBorder = RGB32BIT( 51, 26, 5 );
            m_rgbMakeRoomTitleListBar = RGB32BIT( 183, 112, 15 );
            m_rgbGray = RGB32BIT( 208, 208, 208 );
            break;
    }
}



void CMainFrame::ReleaseData()
{
	SAFE_DELETE( m_SPR_LoGo );
	SAFE_DELETE( m_SPR_Cursor );
	SAFE_DELETE( m_SPR_Connting );
	SAFE_DELETE( m_SPR_MessageBox );
	SAFE_DELETE( m_SPR_Message );
	SAFE_DELETE( m_SPR_ConBox );
	SAFE_DELETE( m_SPR_WaitBase );
	SAFE_DELETE( m_SPR_MakeRoom );
	SAFE_DELETE( m_SPR_Class );
	SAFE_DELETE( m_SPR_WaitAvatar );
	SAFE_DELETE( m_SPR_FullWait );
	SAFE_DELETE( m_SRP_SexIcon );
	SAFE_DELETE( m_SPR_GameBase );
	SAFE_DELETE( m_SPR_Start );
	SAFE_DELETE( m_SPR_Sun );
	SAFE_DELETE( m_SPR_BangJang );
	SAFE_DELETE( m_SPR_CardSpade );
	SAFE_DELETE( m_SPR_CardClover );
	SAFE_DELETE( m_SPR_CardDia );
	SAFE_DELETE( m_SPR_CardHeart );
	SAFE_DELETE( m_SPR_CardBack );
	SAFE_DELETE( m_SPR_CardChioce );
	SAFE_DELETE( m_SPR_BetType_Big );
	SAFE_DELETE( m_SPR_BetType_Small );
	SAFE_DELETE( m_SPR_WindowWin );
	SAFE_DELETE( m_SPR_WinnerMark );
	SAFE_DELETE( m_SPR_USERTIME );
	SAFE_DELETE( m_SPR_Chip );


	SAFE_DELETE( m_SPR_JackPot );
    SAFE_DELETE( m_SPR_JackPotBack );
	SAFE_DELETE( m_SPR_AllIn );
	SAFE_DELETE( m_SPR_JackPotMath );



	SAFE_DELETE( m_SPR_InviteBox );
	SAFE_DELETE( m_SPR_Invitation );
	SAFE_DELETE( m_SPR_Enter );
	SAFE_DELETE( m_SPR_WasteCard );
	SAFE_DELETE( m_SPR_ShowUserInfo );
	SAFE_DELETE( m_SPR_UserState );
    SAFE_DELETE( m_SPR_UserListTitle[0] );
    SAFE_DELETE( m_SPR_UserListTitle[1] );
    SAFE_DELETE( m_SPR_HighlightBar );
    SAFE_DELETE( m_SPR_SelRoomBar );
    SAFE_DELETE( m_SPR_CheckBall );
    SAFE_DELETE( m_SPR_SelectUser );
    SAFE_DELETE( m_SPR_BetMoneyBack );
    SAFE_DELETE( m_SPR_Arrow );
    SAFE_DELETE( m_SPR_PassDlg );
    SAFE_DELETE( m_SPR_ChodaeOX );
	
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		SAFE_DELETE( m_SPR_GameAvatar[i] );
	}
	
	SAFE_DELETE( m_MessageOKBTN );
	SAFE_DELETE( m_MaxBTN );
	SAFE_DELETE( m_MaxBTN2 );
	SAFE_DELETE( m_ExitBTN );
	SAFE_DELETE( m_ConfigBTN );
	SAFE_DELETE( m_Exit2BTN );
	SAFE_DELETE( m_MinBTN );
	SAFE_DELETE( m_FullBTN );
	SAFE_DELETE( m_OkBTN );
	SAFE_DELETE( m_MakeRoomBTN );
	SAFE_DELETE( m_JoinBTN );
	SAFE_DELETE( m_QuickJoinBTN );
	SAFE_DELETE( m_CancelBTN );
	SAFE_DELETE( m_InviteBTN );
	SAFE_DELETE( m_DDaDDang ); // [*BTN]
	SAFE_DELETE( m_Quater );
	SAFE_DELETE( m_Half );
	SAFE_DELETE( m_Full ); // [*BTN]
	SAFE_DELETE( m_Check );
	SAFE_DELETE( m_BBing );
	SAFE_DELETE( m_Call );
	SAFE_DELETE( m_Die );
	SAFE_DELETE( m_StartBTN );
	SAFE_DELETE( m_InviteOKBTN );
    SAFE_DELETE( m_InviteCancelBTN );
	SAFE_DELETE( m_ChodaeYesBTN );
	SAFE_DELETE( m_ChodaeNoBTN );
	SAFE_DELETE( m_UserCloseBTN );
	SAFE_DELETE( m_UserStateBTN );
	SAFE_DELETE( m_MemoSendBTN );
	SAFE_DELETE( m_MemoCancelBTN );
    SAFE_DELETE( m_CaptureBTN );
    SAFE_DELETE( m_RoomTitleBTN );
    SAFE_DELETE( m_MakeRoomOkBTN );
    SAFE_DELETE( m_MakeRoomCancelBTN );

    SAFE_DELETE( m_SCB_WaitChat );
    SAFE_DELETE( m_SCB_WaitUser );
    SAFE_DELETE( m_SCB_WaitRoom );
    SAFE_DELETE( m_SCB_GameChat );
    SAFE_DELETE( m_SCB_Invite );

    //SAFE_DELETE( m_pWaitRoomBanner );
}



void CMainFrame::SetPanMoney( int nUserSlot, BigInt biPanMoney, BOOL bThrow )
{
    g_biTotTempMoney = biPanMoney;
    BigInt nTempMoney = biPanMoney - g_biPanMoney;

    if ( nTempMoney > 0 ) {
        
        // 유저가 걸은 돈을 문자열로 저장
        memset( m_szUserAddMoney[nUserSlot], 0x00, sizeof( m_szUserAddMoney[nUserSlot] ));
        I64toA_Money( nTempMoney, m_szUserAddMoney[nUserSlot] );

        if ( bThrow ) {
            // 톨을 화면 가운데로 던진다.
            ThrowChip( nUserSlot, nTempMoney );
			PlaySound( g_Sound[GS_CHIP] );
        } else {
            PanMoneyReFresh();
        }
    }
    else
        PanMoneyReFresh();
}

void CMainFrame::PanMoneyReFresh()
{
    m_bRenderPanMoney = TRUE;

    g_biPanMoney = g_biTotTempMoney;
	
    BigInt biTempMoney = g_biPanMoney;
	char szTemp[30];                    // 문자열로 벼환된 판 머니

	memset( szTemp, 0x00, sizeof(szTemp) );
	sprintf( szTemp, "%I64u", biTempMoney );

	char szTemp2[2];
	memset( szTemp2, 0x00, sizeof(szTemp2) );

	int nCnt = 0;

	for ( int i = (strlen(szTemp) - 1); i >= 0 ; i-- ) {
		sprintf(szTemp2,"%c",szTemp[i]);
		m_sPanMoneyInfo[nCnt++].nPanMoneyCnt = atoi(szTemp2);
	}
}



void CMainFrame::Render()
{
    g_pCDXInput->Update();

	if( g_pCDXInput->MOUSE_LBDOWN() ) {
		if ( m_bMouseEvent ) {
			m_bMouseDown = TRUE;
			//현재 버튼들의 위치에 마우스가 있는지 체크하고 마우스가 있으면 다운을 시켜준다.
			//DTRACE( "OnLButtonDown" );
			OnLButtonDown();
			
		}
	}

    g_pCDXInput->Get_MouseState(); 

	if( g_pCDXInput->MOUSE_LBUP() ) {
		if ( m_bMouseEvent ) {
			m_bMouseDown = FALSE;
            ::SetFocus( g_hWnd );       // CHttpView로 인해 키 입력 안되는 문제 방지
			//DTRACE( "OnLButtonUp" );
			OnLButtonUp();
			
		}
	}

    g_pCDXScreen->GetBack()->Fill(0);

	switch( m_nCurrentMode )
		{
		case LOGO_MODE:         // 처음 실행되었을 때의 화면
			m_SPR_LoGo->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );

            if( m_Message->GetbShowCheck() ) {
                m_Message->Draw();
            }
	
            if( m_bMouseDown ) m_SPR_Cursor->SetFrame( 1 );
            else               m_SPR_Cursor->SetFrame( 0 );

            m_SPR_Cursor->SetPos( m_mX, m_mY );
            m_SPR_Cursor->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

			g_pCDXScreen->Flip( FALSE, FALSE, FALSE, TRUE );
			return;

		case WAITROOM_MODE:     // 대기실이면
			RenderWaitRoom();
            
            if ( g_bConnectingSpr ) {
                m_SPR_Connting->SetPos( 362, 354 );
                m_SPR_Connting->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }

			break;

		case GAME_MODE:         // 게임화면이면
			RenderGameBase();

            if ( g_bConnectingSpr ) {
                m_SPR_Connting->SetPos( 362, 354 );
                m_SPR_Connting->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }
			
			break;
	}

	if ( m_bShowMessageBox ) {      // 쪽지
		DrawMessageBox( MEMO_DLG_POS_X, MEMO_DLG_POS_Y );
	}

    if ( m_bFullScreen ) {
        m_MaxBTN2->DrawButton();    // 최대화 버튼 출력
    }
    else {
        m_MaxBTN->DrawButton();
    }

	m_ExitBTN->DrawButton();        // 종료 버튼 출력
	m_ConfigBTN->DrawButton();      // 설정 버튼을 그린다.
	m_Exit2BTN->DrawButton();       // 나가기 버튼을 그린다.
    m_CaptureBTN->DrawButton();     // 화면 캡쳐 버튼을 그린다.
    m_MinBTN->DrawButton();         // 창 최소화 버튼을 그린다.

	if( m_bShowConfigBox ) {        // 옵션
		m_pConfigBox->RenderConfigBox();
	}

    if( m_Message->GetbShowCheck() ) {
        m_Message->Draw();
    }

	if ( m_bMouseRender ) {         // 마우스 커서 출력
        if( m_bMouseDown ) m_SPR_Cursor->SetFrame( 1 );
        else               m_SPR_Cursor->SetFrame( 0 );
        m_SPR_Cursor->SetPos( m_mX, m_mY );
        m_SPR_Cursor->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}
	
    g_pCDXScreen->Flip(FALSE, FALSE, FALSE, TRUE);      // Blt로 화면에 그린다.
}



void CMainFrame::CreateRoom()
{
    OpenMakeRoomDlg();      // 방만들기 창을 띄운다.
	ProcessButtonEvent();
}



int CMainFrame::FindSunArrayNo()
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( g_CGameUserList.m_GameUserInfo[i].m_bSun == TRUE ) {
			return i;			
		}
	}

	return 0;
}



void CMainFrame::RenderTimePos()
{
	//유저가 클릭했을때..없애기
	//유저가 해야할때 세팅하기.

    m_SPR_USERTIME->SetFrame( m_nTimePos );
    m_SPR_USERTIME->SetPos( m_sRoomInUserPosition[m_nUserCallSlotNo].pTimePos.x, m_sRoomInUserPosition[m_nUserCallSlotNo].pTimePos.y );
    m_SPR_USERTIME->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
}



void CMainFrame::SetJackPotMoney( BigInt biJackPotMoney )
{
	memset( m_szJackPotMoney, 0x00, sizeof(m_szJackPotMoney) );
	g_biJackPotMoney = biJackPotMoney;

	sprintf( m_szJackPotMoney, "%I64u", g_biJackPotMoney );

	m_nJackPotLen = strlen(m_szJackPotMoney);
	m_nJackPotStopCnt = 0;
	m_nJackPotPos = 0;
	m_bRoomJackPotRender = TRUE;
}



void CMainFrame::RenderRoomJackPot()
{
	int nWidth = 10, nHeight = 22;
	
	//잭팟 하나 숫자
	char szTemp[2] = { 0, };
	int nOneMeth = 0;	

	//애니를 한다.
	if ( m_nJackPotStopCnt < m_nJackPotLen ) {
        m_SPR_JackPotMath->SetFrame((((m_nJackPotPos % 3) + 1) * 12)   +   (m_nJackPotPos / 3));
        m_SPR_JackPotMath->SetPos( m_JakcPotMathPoint[m_nJackPotStopCnt].x, m_JakcPotMathPoint[m_nJackPotStopCnt].y );
        m_SPR_JackPotMath->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		//m_nJackPotPos : 한바퀴 애니메이션, m_nJackPotStopCnt : 한바퀴에한번증가
		m_nJackPotPos++;

		if ( m_nJackPotPos > 29 ) {
			m_nJackPotPos = 0;
			m_nJackPotStopCnt++;
		}
	}

	//애니가 끝난 숫자를 찍는다.
	int nJackPotLen = m_nJackPotLen;
		
	for ( int i = 0; i < m_nJackPotStopCnt; i++ ) {
		nJackPotLen--;

		//잭팟 하나 숫자
		szTemp[0] = m_szJackPotMoney[nJackPotLen];
		nOneMeth = atoi(szTemp);

        m_SPR_JackPotMath->SetFrame( nOneMeth );
        m_SPR_JackPotMath->SetPos( m_JakcPotMathPoint[i].x, m_JakcPotMathPoint[i].y );
        m_SPR_JackPotMath->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}

	//애니가 끝난후 숫자 사이 콤마를 찍는다.
	for ( i = 0; i < (m_nJackPotStopCnt - 1) / 3; i++ ) {
        m_SPR_JackPotMath->SetFrame( 11 );
        m_SPR_JackPotMath->SetPos( m_JackPotCommaPoint[i].x, m_JackPotCommaPoint[i].y );
        m_SPR_JackPotMath->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}
}



void CMainFrame::RenderWinner()
{
	if ( m_bWinnerCpoy == FALSE ) {
		memcpy( g_sWinnerCardInfo.szNick_Name, g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_sUserInfo.szNick_Name, MAX_NICK_LENGTH );
		g_sWinnerCardInfo.nCardCnt = g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_nCardCnt;

		for ( int i = 0; i < g_sWinnerCardInfo.nCardCnt; i++ ) {
			g_sWinnerCardInfo.nCardNo[i] = g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_sUserCardInfo[i].nCardNo;
		}

		memcpy( g_sWinnerCardInfo.szJokboCard, g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_szJokboCard,
			    sizeof(g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_szJokboCard) );

		m_bWinnerCpoy = TRUE;
	}

	memset( m_szTempText, 0x00, sizeof(m_szTempText) );
	memset( m_szTempText2, 0x00, sizeof(m_szTempText2) );

	//중간에 나갈수도 있기 때문에.
	if ( g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_sUserInfo.nUserNo > 0 &&
		 g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_nCardCnt > 0 )
    {
        // 승리한 유저의 위치에 "Win" 이미지 출력
        RenderWinnerMark();
    }

    // 결과창 출력
    PutSprite( m_SPR_WindowWin, 360, 284 );

    // 승리자의 닉네임
	NickSavePrint( g_sWinnerCardInfo.szNick_Name, 482, 316 );	

	// 승리자의 이긴 돈.
	I64toA_Money( g_biPureWinMoney, m_szTempText );
	sprintf( m_szTempText2, "+%s", m_szTempText );
    DrawText( m_szTempText2, 512, 331, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	

	//////////////////////////////////////////////////////////////////////////
	BYTE nCardNo = 0;
	
	for ( int i = 0; i < g_sWinnerCardInfo.nCardCnt; i++ ) {
		nCardNo = g_sWinnerCardInfo.nCardNo[i];

		if ( g_bMyWin == FALSE ) {
			bool bRe = FALSE;

			for ( int nQ = 0; nQ < 7; nQ++ ) {
				if ( nCardNo == g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_szFixJokboCard[nQ] ) {
					PutCardImage( 386 + i * 27, 345, nCardNo );
					bRe = TRUE;
					break;
				}
			}

            if ( bRe == FALSE ) {
                PutCardImage( 386 + i * 27, 345, nCardNo, FALSE );
            }
		}
        else {
            PutCardImage( 386 + i * 27, 345, nCardNo );
        }
	}
    
	//////////////////////////////////////////////////////////////////////////

	//승리자 족보.
	JokboShow( g_sWinnerCardInfo.szJokboCard, m_szTempText );
	
    if ( g_szResultCmt[0] == NULL ) {
        DrawText( m_szTempText, 512, 458, RGB( 0, 252, 255 ), FW_NORMAL, TA_CENTER );
    }
	else {
		sprintf( m_szTempText2, "(%s)", g_szResultCmt );
        DrawText( m_szTempText2, 512, 458, RGB( 0, 252, 255 ), FW_NORMAL, TA_CENTER );
	}

	//유저의 잭팟이면.
	if ( m_bJackPot ) {
		m_SPR_JackPot->SetPos( 412, 220 );
        m_SPR_JackPot->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		I64toA_Money( g_biUserJackPotMoney, m_szTempText );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 509, 258, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}
}



//========================================================================
// "WIN" 이미지 출력
//========================================================================

void CMainFrame::RenderWinnerMark()
{
    PutSprite( m_SPR_WinnerMark,
               m_sRoomInUserPosition[g_nWinnerSlotNo].pWinnerPos.x,
               m_sRoomInUserPosition[g_nWinnerSlotNo].pWinnerPos.y,
               m_nWinnerMarkPos );
}



void CMainFrame::PutCardImage( int x, int y, BYTE nCardNo, BOOL bUp )
{
	if( nCardNo >= MAX_CARD ) {	        //뒷면이면 255이기 때문에.
        m_SPR_CardBack->SetPos( x, y );
        m_SPR_CardBack->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		return;
	}

    int nFrame = 0;

	if ( bUp ) nFrame = (nCardNo % 13);        // 일반 카드
	else       nFrame = 13 + (nCardNo % 13);   // 어두운 카드

	switch( nCardNo / 13 ) {
		case 0:
            m_SPR_CardClover->SetFrame( nFrame );
            m_SPR_CardClover->SetPos( x, y );
            m_SPR_CardClover->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			break;

		case 1:
            m_SPR_CardHeart->SetFrame( nFrame );
            m_SPR_CardHeart->SetPos( x, y );
            m_SPR_CardHeart->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			break;
		
		case 2:
            m_SPR_CardDia->SetFrame( nFrame );
            m_SPR_CardDia->SetPos( x, y );
            m_SPR_CardDia->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			break;
		
		case 3:
            m_SPR_CardSpade->SetFrame( nFrame );
            m_SPR_CardSpade->SetPos( x, y );
            m_SPR_CardSpade->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			break;
	}
}
BOOL CMainFrame::isInteger(CString origin)
{
	origin.TrimLeft();
	origin.TrimRight();

	int iTmp = atoi(origin);
	CString strNum;
    strNum.Format("%d",iTmp);

	if(strcmp(origin,strNum)==0 || iTmp==0)
		return TRUE;
	else
		return FALSE;
}


void CMainFrame::AllCallBtnUp()
{
	//시계 작동 멈춤.
	m_bUserCallTime = FALSE;
	m_nUserCallTime = 0;	

	m_Call->status      = DISABLED;
	m_Half->status      = DISABLED;
	m_BBing->status     = DISABLED;
	m_DDaDDang->status  = DISABLED; // [*BTN]
	m_Check->status     = DISABLED;
	m_Die->status       = DISABLED;
	m_Full->status      = DISABLED; // [*BTN]
	m_Quater->status    = DISABLED;

}



void CMainFrame::SetUserCall( sGameCurUser asGameCurUser, int nUserArrayNo )
{
	//시계 타임 세팅.
	m_nUserCallSlotNo = nUserArrayNo;
	m_bUserCallTime = TRUE;
	m_nUserCallDefineTime = CALL_TIME;

	if ( nUserArrayNo == 0  ) {
		m_biCall    = asGameCurUser.biCall;
		m_biHalf    = asGameCurUser.biHalf;
		m_biBing    = asGameCurUser.biBing;
		m_biDdaDang = asGameCurUser.biDdaDang;
		m_biCheck   = asGameCurUser.biCheck;
		m_biDie     = asGameCurUser.biDie;	
		m_biFull    = asGameCurUser.biFull;	
		m_biQuater  = asGameCurUser.biQuater;	

		if ( asGameCurUser.bCall )
            m_Call->EnableButton();
		else
            m_Call->status = DISABLED;

		if ( asGameCurUser.bHalf )
			m_Half->EnableButton();
		else
			m_Half->status = DISABLED;

		if ( asGameCurUser.bBing )
			m_BBing->EnableButton();
		else
			m_BBing->status = DISABLED;


		if ( asGameCurUser.bDdaDang )
			m_DDaDDang->EnableButton();
		else
			m_DDaDDang->status = DISABLED;

        /* [*BTN]
		if ( asGameCurUser.bDdaDang )
			m_DDaDDang->EnableButton();
		else
			m_DDaDDang->status = DISABLED;
        */

		if ( asGameCurUser.bCheck )
			m_Check->EnableButton();
		else
			m_Check->status = DISABLED;
	
		if ( asGameCurUser.bDie )
			m_Die->EnableButton();
		else
			m_Die->status = DISABLED;

        /* [*BTN]
		if ( asGameCurUser.bFull )
			m_Full->EnableButton();
		else
			m_Full->status = DISABLED;
        */

		if ( asGameCurUser.bFull )
			m_Full->EnableButton();
		else
			m_Full->status = DISABLED;




		if ( asGameCurUser.bQuater )
			m_Quater->EnableButton();
		else
			m_Quater->status = DISABLED;
			
        g_biCallMoney = m_biCall;
	}
    else {
        g_biCallMoney = 0;
    }
}



void CMainFrame::UserCardAdd( BYTE nCardNo , int nUserArrayNo, BOOL bAniMation ) // 팩에서 유저한테 패 날리기
{
	UINT nUserCardCnt = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt;

	g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].nCardNo = nCardNo;
	g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt++;
	
	int nXDistance = 0;

	if ( bAniMation ) {
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bChoioce == TRUE ) 
			nXDistance = CARD_DISTANCE_B;
		else
			nXDistance = CARD_DISTANCE_A;

		g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].NewPos.x = m_sRoomInUserPosition[nUserArrayNo].pCardStartPos.x + ( nUserCardCnt * nXDistance );
		g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].NewPos.y = m_sRoomInUserPosition[nUserArrayNo].pCardStartPos.y;
		g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].ctCardType = FLOOR;
		m_CAniMation.AddStack(&g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt]);
	}
	else {
		nXDistance = CARD_DISTANCE_B;

		g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].ctCardType = USERCARD;
		g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].Pos.x = m_sRoomInUserPosition[nUserArrayNo].pCardStartPos.x + ( nUserCardCnt * nXDistance );
		g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nUserCardCnt].Pos.y = m_sRoomInUserPosition[nUserArrayNo].pCardStartPos.y;
	}
}



void CMainFrame::RenderGameBase()
{
    // 게임 화면 배경 이미지
	m_SPR_GameBase->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );

    // 잭팟 배경 이미지
    PutSpriteBlk( m_SPR_JackPotBack, 16, 27, m_nJackpotBackFrm );

    // 채팅창을 그린다.
    RenderGameChat();

	// 게임방에서 버튼들..
	m_FullBTN->DrawButton();
	m_InviteBTN->DrawButton();
	m_DDaDDang->DrawButton(); // [*BTN]
	m_Quater->DrawButton();
	m_Half->DrawButton();
	m_Full->DrawButton(); // [*BTN]
	m_Check->DrawButton();
	m_BBing->DrawButton(); 
	m_Call->DrawButton();
	m_Die->DrawButton();

    if( g_sRoomInfo.nCurCnt >= MAX_ROOM_IN ) {
        m_InviteBTN->DisableButton();
    }

    // 쌓여있는 톨을 그린다.
    if ( m_bRenderPanMoney ) {
        RenderPanMoney();
    }

	// 방안의 유저를 뿌린다.
	RenderUserCard();

    // 타이머를 그린다.
    if ( m_bUserCallTime && ( m_nUserCallDefineTime - m_nUserCallTime ) <= 500 ) {
        RenderTimePos();
    }

    //유저 정보.    
    RenderUserInfo();

#ifndef SHOWUSERCARD
    // 엔터키 이미지
    if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt >= MAX_USER_CARD ) {
        m_SPR_Enter->SetPos( 627, 687 );
        m_SPR_Enter->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
    }

    // 이동중인 히든 카드를 그린다.
    if ( m_bMoveHiddenCard ) {
        PutCardImage( m_sHiddenCardInfo.Pos.x, m_sHiddenCardInfo.Pos.y, 255 );
    }
#endif

    // 톨 슬라이딩 그리기
    RenderThrowChip();      // chonkw  유저가 버튼(쿼터, 하프 등)을 눌러서 던진 톨을 그린다.










    // 이동중인 카드 그리기. 한장씩 뿌리기위해 스택에 있는것만 뿌린다.
	sUserCardInfo *card = m_CAniMation.GetCard(0);
    if ( card != NULL ) {
        PutCardImage( card->Pos.x, card->Pos.y, card->nCardNo );
    }

    // 버릴 카드나 보여줄 카드를 고르는 대화상자를 보여준다.
    RenderCardSelectionDlg();

    // 결과 화면 그리기
    if ( m_bEnddingShow ) {
        RenderWinner();
    }

    // 잭팟
    if ( m_bRoomJackPotRender ) {
        RenderRoomJackPot();
    }

    // 유저 정보 창
	if ( m_bShowUserInfo )
		RenderShowUserInfo();



	// 시작 버튼을 뿌린다.
	if( m_bShowStartBTN ) {		
		m_StartBTN->DrawButton(); // 시작 버튼 출력	
		
		if( MouseIn( 325, 253, 474, 301 ) ) {
			m_StartBTNState = 1;
		}
        else {
            m_StartBTNState = 0;
        }
		 
		if( m_StartBTNState == 0 ) {
            m_SPR_Start->SetFrame( m_StartAni );

			m_StartT1 = GTC();

			if( m_StartT1 - m_StartT2 > 300 ) {
				m_StartT2 = GTC();

                if( ++ m_StartAni > 1 ) {
                    m_StartAni = 0;
                }
			}		
		
            if( m_bShowStartBTN ) {
                m_SPR_Start->SetPos( 437, 361 );
                m_SPR_Start->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }
		}
	}	

	if( m_bShowInviteBox ) {    // 초대 박스가 그려지면
		RenderInviteBox();
	}
/*
	memset( m_szTempText, 0x00, sizeof(m_szTempText) );
	sprintf( m_szTempText , "포커[%d채널]" , g_szGameServerChannel + 1 );
    DrawText( m_szTempText, 13, 6, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );
*/	
	sprintf( m_szTempText , "%d번방  방제 - %s   [기본 : %d톨]" , g_sRoomInfo.nRoomNo + 1, g_sRoomInfo.szRoomName, g_sRoomInfo.biRoomMoney );	
    DrawText( m_szTempText, 300, 6, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );
}



//========================================================================
// 유저 닉네임 10바이트가 넘으면 줄여서 저장한다.
//========================================================================

void CMainFrame::NickSave( char *pShortNick, char *pNickName )
{
	// 별명
	if ( strlen( pNickName ) > 10 ) {
        char szTemp[255];
	    memset( szTemp, 0x00, sizeof(szTemp) );
		memcpy( szTemp, pNickName, 10 );
		strcat( szTemp, "..." );
        strcpy( pShortNick, szTemp );
	}
	else {
        strcpy( pShortNick, pNickName );
	}
}



//========================================================================
// 유저 닉네임 10바이트가 넘으면 줄여찍는다
//========================================================================

void CMainFrame::NickSavePrint( char *pNickName, int x, int y, COLORREF rgbCol )
{
	char szTemp[255];
	memset( szTemp, 0x00, sizeof(szTemp) );

	// 별명
	if ( strlen( pNickName ) > 10 ) {
		memcpy( szTemp, pNickName, 10 );
		strcat( szTemp, "..." );
        DrawText( szTemp, x, y, rgbCol, FW_NORMAL, TA_LEFT );
	}
	else {
        DrawText( pNickName, x, y, rgbCol, FW_NORMAL, TA_LEFT );
	}
}



void CMainFrame::UserSelectView( int nSlotNo, char cUserSelect, BOOL bNetWorkCall )
{
	//나가리이거나 카드초이스에서 혼자 남았을경우.
    if ( cUserSelect == '8' ) {
        return;
    }

	//시계 애니메이션 초기화.
	m_nUserCallTime = 0;
	m_bUserCallTime = FALSE;
	m_nTemp         = 0;
	m_nTimePos      = 0;

	//유저가 다이 했다는것을 세팅해노아야 중간에 죽은 사람들을 피해간다. 결과등에서.
    if ( cUserSelect == '7' ) {
        g_CGameUserList.m_GameUserInfo[nSlotNo].m_bDie = TRUE;
    }

	// '0':따당, '1':체크, '2':쿼터, '3':삥, '4':하프, '5':콜, '6':풀, '7':다이
	int nSoundDivi = 0;

	switch( cUserSelect ) {
		case '0':
			nSoundDivi = GS_DOUBLE;
			break;

		case '1':
			nSoundDivi = GS_CHECK;
			break;

		case '2':
			nSoundDivi = GS_QUARTER;
			break;

		case '3':
			nSoundDivi = GS_BBING;
			break;

		case '4':
			nSoundDivi = GS_HALF;
			break;

		case '5':
			nSoundDivi = GS_CALL;
			break;

		case '6':
			nSoundDivi = GS_FULL;
			break;

		case '7':
			nSoundDivi = GS_DIE;
			break;
	}

	g_CGameUserList.m_GameUserInfo[nSlotNo].m_cUserSelct = cUserSelect;

	if ( bNetWorkCall && nSlotNo == 0 ) {
	}
	else {
		//앞사람이 풀했냐
        if ( cUserSelect == '6' ) {
            m_bBeforeFull = TRUE;
        }
        else {
            m_bBeforeFull = FALSE;
        }

		g_CGameUserList.m_GameUserInfo[nSlotNo].m_nUserSelectCnt = 50;
		g_CGameUserList.m_GameUserInfo[nSlotNo].m_bUserSelectShow = TRUE;
		PlaySound(g_Sound[nSoundDivi]);
	}
}



void CMainFrame::RenderShowUserInfo()
{
	m_SPR_ShowUserInfo->SetPos( 379, 272 );
    m_SPR_ShowUserInfo->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	m_UserCloseBTN->DrawButton( 576, 468 );

	// 별명
	if ( strlen( g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.szNick_Name ) > 10 ) {
		memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
		memcpy( g_szTempStr, g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.szNick_Name, 10 );
		strcat( g_szTempStr, "..." );
        DrawText( g_szTempStr, 559, 344, RGB( 0 , 0, 0 ), FW_NORMAL, TA_LEFT );
	}
	else {
        DrawText( g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.szNick_Name, 559, 344, RGB( 0 , 0 , 0 ), FW_NORMAL, TA_LEFT );
	}

	// 금액
	I64toA_Money(  g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.biUserMoney, m_szTempText, 1 );
    DrawText( m_szTempText, 559, 368, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
	
	// 계급
	int nTempClass = g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.nClass;
	int nWinCnt;
	int nLoseCnt;
	int nDrawCnt;
	int nAllCnt;

    m_SPR_Class->SetFrame( nTempClass );
    m_SPR_Class->SetPos( 559 , 389 );
    m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

	nWinCnt   =   g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.nWin_Num;
	nLoseCnt  =   g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.nLose_Num;
	nDrawCnt  =   g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.nDraw_Num;

	nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );

	// 전적
	sprintf( m_szTempText , "%d전 %d승" , nAllCnt, nWinCnt );
    DrawText( m_szTempText, 559, 416, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

	sprintf( m_szTempText , "%d패 %d무" , nLoseCnt, nDrawCnt );
    DrawText( m_szTempText, 559, 442, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

	PutSpriteAvatar( 392, 333, m_SPR_GameAvatar[m_nShowUserSlot], m_GameAvaImg[m_nShowUserSlot], ORIGINAL_SIZE );
}



//========================================================================
// 쌓여 있는 톨을 그린다.
//========================================================================

void CMainFrame::RenderPanMoney()
{
    int nX, nY;

	for ( int i = 0; i < 16; i++ ) {
        m_SPR_Chip->SetFrame( i );

        nX = m_sPanMoneyInfo[i].PanMoneyPos.x;
        nY = m_sPanMoneyInfo[i].PanMoneyPos.y;
		
		for ( int j = 0; j < m_sPanMoneyInfo[i].nPanMoneyCnt; j++ ) {
            m_SPR_Chip->SetPos( nX, nY );
            m_SPR_Chip->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            nY -= 5;
		}
	}

}



void CMainFrame::UserSelectPutSprite( int nSlotNo, char cUserSelect, BOOL bBig )
{
	if ( bBig ) {
		int nUserSelect = atoi( &cUserSelect );

        if ( nSlotNo >= 3 ) nUserSelect += 8;   // 0, 1, 2번 유저는 ` 모양.... 3, 4번 유저는 ' 모양
        m_SPR_BetType_Big->SetFrame( nUserSelect );
        m_SPR_BetType_Big->SetPos( m_sRoomInUserPosition[nSlotNo].pUserSelectBigPos.x, m_sRoomInUserPosition[nSlotNo].pUserSelectBigPos.y );
        m_SPR_BetType_Big->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}
	else {
		int nUserSelect = atoi( &cUserSelect );

        m_SPR_BetType_Small->SetFrame( nUserSelect );
        m_SPR_BetType_Small->SetPos( m_sRoomInUserPosition[nSlotNo].pUserSelectSmallPos.x, m_sRoomInUserPosition[nSlotNo].pUserSelectSmallPos.y );
        m_SPR_BetType_Small->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}
}



void CMainFrame::RenderUserCard()
{
	//판돈 찍기. ( 이것만 풀이다. )
	I64toA_Money( g_biPanMoney, m_szTempText );
    DrawText( m_szTempText, 670, 458, RGB( 241, 178, 9 ), FW_BOLD, TA_RIGHT, 16 );

    // 유저 콜 머니 출력 - ham 2005.09.26
    I64toA_Money( g_biCallMoney, m_szTempText );
    DrawText( m_szTempText, 670, 492, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT, 16 );

	//처음 카드를 세장주고 선택 화면 
	int i = 0;
	int nSunArrayNo = FindSunArrayNo();
	int nUserArrayNo = 0;
	BYTE nCardNo = 0;

	//방안의 모든 유저에게 한장씩 차례대로 돌려야 하기 때문에.
	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		nUserArrayNo = ( nSunArrayNo + i ) % MAX_ROOM_IN;

		//카드가 가려지니 지금 찍는다.
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.nUserNo > 0 ) {
            // 선택된 유저 밝은 테두리 출력
            if ( m_nUserCallSlotNo == nUserArrayNo ) {
                m_SPR_SelectUser->SetPos( m_sRoomInUserPosition[nUserArrayNo].pCardInfo.x, m_sRoomInUserPosition[nUserArrayNo].pCardInfo.y );
                m_SPR_SelectUser->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }

            I64toA_Money( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.biUserMoney, m_szTempText );
            DrawText( m_szTempText, m_sRoomInUserPosition[nUserArrayNo].pUserMoneyPos.x, m_sRoomInUserPosition[nUserArrayNo].pUserMoneyPos.y, RGB( 255, 255, 0 ), FW_NORMAL, TA_RIGHT );
		}
        
		//선의 카드부터 찍어준다.
		for ( int j = 0; j < g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt; j++ )
        {
			if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].ctCardType == USERCARD )
            {
#ifdef SHOWUSERCARD
                nCardNo = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].nCardNo;
#else
				//자기 자신일때는 카드를 보여주고
				if ( nUserArrayNo == 0 ) {
					nCardNo = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].nCardNo;
				}
				else {
					//남들 카드는 자신이 초이스를 한후 부터 보이도록 한다.
                    if ( g_CGameUserList.m_GameUserInfo[0].m_bChoioce == TRUE ) {
                        nCardNo = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].nCardNo;		
                    }
                    else {
                        nCardNo = 255;
                    }
				}
#endif

				//카드를 다 받았고 마지막 장이 오픈이면. 결과창이 뜸과 동시에.
				if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt == 7 && 
					 g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[6].nCardNo != 255 && 
 					 m_bEnddingShow && g_bMyWin == FALSE )
				{
					bool bRe = FALSE;

					for ( int nQ = 0; nQ < 7; nQ++ ) {
						if ( nCardNo == g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_szFixJokboCard[nQ] && 
							 g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_szFixJokboCard[nQ] != 255 ) 
						{
							PutCardImage( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.x, 
								          g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.y, nCardNo );

							bRe = TRUE;
							break;
						}
					}

					if ( bRe == FALSE )
						PutCardImage( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.x, 
							g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.y, nCardNo, FALSE );

				}
				else {
					//내카드 일때 처음 두장은 어두운 카드
					if ( nUserArrayNo == 0 && j < 2 ) {
						PutCardImage( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.x, 
							g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.y, nCardNo, FALSE );
					}
					else {
						PutCardImage( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.x, 
							g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].Pos.y, nCardNo );
					}
                }
			}
		}
        


		//선
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bSun == TRUE &&
			 g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt > 0 ) 
		{
			m_SPR_Sun->SetPos( m_sRoomInUserPosition[nUserArrayNo].pBossPos.x, m_sRoomInUserPosition[nUserArrayNo].pBossPos.y );
            m_SPR_Sun->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		}


		//방장, 유저(콜,다이)선택 찍기.
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.nUserNo > 0 ) {
			//방장
			if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bBBangJjang == TRUE ) {
				m_SPR_BangJang->SetPos( m_sRoomInUserPosition[nUserArrayNo].pBangJangPos.x, m_sRoomInUserPosition[nUserArrayNo].pBangJangPos.y );
                m_SPR_BangJang->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			}

            //타이머에 가려지기 때문에. 작은 모양을 여기에서 찍고...큰 모양은 타이머 찍고 나서 찍는다.
   			if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bUserSelectShow ) {
                if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nUserSelectCnt <= 0 ) {
                    UserSelectPutSprite( nUserArrayNo, g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_cUserSelct, FALSE );
                    DrawText( m_szUserAddMoney[nUserArrayNo], m_sRoomInUserPosition[nUserArrayNo].pUserSelectSmallPos.x - 3, m_sRoomInUserPosition[nUserArrayNo].pUserSelectSmallPos.y + 2, RGB( 153, 255, 255 ), FW_BOLD, TA_RIGHT );
                }
			}
		}

#ifndef SHOWUSERCARD
        // 히든 카드를 이동할 수 있다는 뜻으로 화살표를 표시해준다.

        if (( g_CGameUserList.m_GameUserInfo[0].m_bDie == FALSE
           && g_CGameUserList.m_GameUserInfo[0].m_nCardCnt >= 7 
           && g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.x == g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].NewPos.x
           && g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y == g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].NewPos.y
           && g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].nCardNo >= MAX_CARD )
           ||
           m_bMoveHiddenCard )
        {
            if ( m_bEnddingShow == FALSE ) {
                m_SPR_Arrow->SetPos( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.x + 30,
                                     g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y - 12);
                m_SPR_Arrow->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }
        }
#endif
	}
}



void CMainFrame::RenderUserInfo()
{
	memset( m_szTempText, 0x00, sizeof(m_szTempText) );
	
	//유저 정보를 뿌려야 하는지를 판다.
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 ) {
			//개인아바타
			PutSpriteAvatar( m_sRoomInUserPosition[i].pAvatarPos.x, m_sRoomInUserPosition[i].pAvatarPos.y, m_SPR_GameAvatar[i], m_GameAvaImg[i] );

            // 클래스
            PutSprite( m_SPR_Class, m_sRoomInUserPosition[i].pNickPos.x - 18, m_sRoomInUserPosition[i].pNickPos.y - 2, g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nClass );

			//개인닉네임.
			NickSavePrint( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, m_sRoomInUserPosition[i].pNickPos.x , m_sRoomInUserPosition[i].pNickPos.y, RGB( 255, 255, 0 ) );
	
#ifdef SHOWUSERCARD
            JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
            if ( i == 0 ) {
                // 게임중 및 게임 끝났을 때의 자신의 족보 표시
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_CENTER );
            }
            else {
                // 게임이 끝났을 때의 다른 유저의 족보 표시
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
            }
#else
			// 자신에게 카드가 있으면 다른 유저의 족보를 보게한다.
			if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt > 0 ) {
				if ( g_CGameUserList.m_GameUserInfo[i].m_bEnd && g_CGameUserList.m_GameUserInfo[i].m_nCardCnt > 0  ) { 
					JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
                    if ( i == 0 ) {
                        // 게임중 및 게임 끝났을 때의 자신의 족보 표시
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_CENTER );
                    }
                    else {
                        // 게임이 끝났을 때의 다른 유저의 족보 표시
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
                    }
				}
			}

            // 중간에 들어와서 카드가 없는 사람은 다른사람의 족보도 못보게 한다. 단, 마지막에는 보게한다.
			else {
				if ( g_CGameUserList.m_GameUserInfo[i].m_nCardCnt == 7 && g_CGameUserList.m_GameUserInfo[i].m_bEnd && g_CGameUserList.m_GameUserInfo[i].m_nCardCnt > 0  ) { 
					JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
                    DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
					DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
				}
			}
#endif

			//올인 마크 표시. 카드 카운트를 조건으로 내세운 이유는 타유저가 올인이 되어서 방에서 쫓겨날때 m_ballin이 false가 되어서 소리가 한번더난다.
			if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney <= 0 && g_CGameUserList.m_GameUserInfo[i].m_nCardCnt > 0 ) {
				m_SPR_AllIn->SetPos( m_sRoomInUserPosition[i].pAllInPos.x, m_sRoomInUserPosition[i].pAllInPos.y );
                m_SPR_AllIn->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

				if ( g_CGameUserList.m_GameUserInfo[i].m_bAllin == FALSE ) {
					PlaySound(g_Sound[GS_ALLIN]);
					g_CGameUserList.m_GameUserInfo[i].m_bAllin = TRUE;
				}
			}



            //유저 call big mark표시.
			if ( g_CGameUserList.m_GameUserInfo[i].m_bUserSelectShow ) {
                if ( g_CGameUserList.m_GameUserInfo[i].m_nUserSelectCnt > 0 ) {
                    UserSelectPutSprite( i, g_CGameUserList.m_GameUserInfo[i].m_cUserSelct );
                }
			}

			//게임 중일때의 타 유저 족보표시.
			if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt > 0 &&
				 !m_bStartInit && !m_bCardDump && !m_bCardChoice && i != 0 && 
				 g_CGameUserList.m_GameUserInfo[i].m_sUserCardInfo[3].nCardNo != 255 &&
				 g_CGameUserList.m_GameUserInfo[i].m_sUserCardInfo[6].nCardNo == 255 ) 
			{
				JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}

	//버튼위에 마우스 올렸을시 해당금액 찍어주기.

    // 켜진 모든 버튼 옆에 걸었을 때의 금액이 출력되도록 수정
    /* [*BTN]
    // 따당
    if ( m_DDaDDang->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_DDaDDang->GetPosX(), m_DDaDDang->GetPosY() - 23 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biDdaDang, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 738, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}
    */
/*
    if ( m_DDaDDang->status == UP ) {
    //    m_SPR_BetMoneyBack->SetPos( m_DDaDDang->GetPosX(), m_DDaDDang->GetPosY() - 23 );
     //   m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	//	I64toA_Money( m_biDdaDang, m_szTempText, 1 );
	//	sprintf( m_szTempText2, "%s", m_szTempText );
     //   DrawText( m_szTempText2, 738, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}


    if ( m_DDaDDang->status == UP ) {
   //     m_SPR_BetMoneyBack->SetPos( m_DDaDDang->GetPosX(), m_DDaDDang->GetPosY() - 23 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	//	I64toA_Money( m_biDdaDang, m_szTempText, 1 );
	//	sprintf( m_szTempText2, "%s", m_szTempText );
     //   DrawText( m_szTempText2, 738, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}


    // 쿼터
	if ( m_Quater->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_Quater->GetPosX(), m_Quater->GetPosY() - 23 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biQuater, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 850, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}

    // 삥
    if ( m_BBing->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_BBing->GetPosX(), m_BBing->GetPosY() - 23 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biBing, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 925, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}
			
    // 하프
	if ( m_Half->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_Half->GetPosX(), m_Half->GetPosY() + 47 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biHalf, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 775, 688, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}

    // 콜
    if ( m_Call->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_Call->GetPosX(), m_Call->GetPosY() + 47 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biCall, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 850, 688, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}


	if ( m_Full->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_Full->GetPosX(), m_Full->GetPosY() + 47 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biFull, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 888, 688, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}
	*/
}



void CMainFrame::JokboShow( BYTE *pJokbo, char *pResult )
{
	int nTemp = 0;
	char szTemp[255];

	memset( szTemp, 0x00, sizeof(szTemp) );
	memset( pResult, 0x00, 255 );

	pResult[0] = '[';

	BOOL bRe = FALSE;

	for ( int i = 1; i < 8; i++ ) {
		if ( pJokbo[i] != 0xff /*&& pJokbo[i] != NULL*/ ) {

			bRe = TRUE;
			nTemp = pJokbo[i] % 13;
            
			switch( nTemp ) {
				case 12:
					strcat( pResult, "A," );
					break;

				case 11:
					strcat( pResult, "K," );
					break;

				case 10:
					strcat( pResult, "Q," );
					break;

				case 9:
					strcat( pResult, "J," );
					break;
                    
				default:
					sprintf( szTemp, "%d,", nTemp + 2 );
					strcat( pResult, szTemp );
					break;
				}
		}
	}

	if ( bRe == FALSE )
	{
		pResult[0] = NULL;
		return;
	}

	nTemp = strlen( pResult );
	pResult[nTemp - 1] = ']';

	switch(pJokbo[0]) {
		case 11:
			strcpy( szTemp, "로열스트레이트 플러쉬" );
			break;

		case 10:
			strcpy( szTemp, "스트레이트 플러쉬" );
			break;

		case 9:
			strcpy( szTemp, "포카드" );
			break;

		case 8:
			strcpy( szTemp, "풀하우스" );
			break;

		case 7:
			strcpy( szTemp, "플러쉬" );
			break;

		case 6:
			strcpy( szTemp, "마운틴" );
			break;

		case 5:
			strcpy( szTemp, "빽스트레이트" );
			break;

		case 4:
			strcpy( szTemp, "스트레이트" );
			break;

		case 3:
			strcpy( szTemp, "트리플" );
			break;

		case 2:
			strcpy( szTemp, "투페어" );
			break;

		case 1:
			strcpy( szTemp, "원페어" );
			break;

		case 0:
			strcpy( szTemp, "탑" );
			break;
	}

	strcat( pResult, szTemp );
}

/*
void CMainFrame::JokboShow( BYTE *pJokbo, char *pResult )
{
	int nTemp = 0;
	char szTemp[255];

	memset( szTemp, 0x00, sizeof(szTemp) );
	memset( pResult, 0x00, 255 );

	pResult[0] = '[';

	for ( int i = 1; i < 8; i++ ) {
		if ( pJokbo[i] != 0xff ) {

			nTemp = pJokbo[i] % 13;
            
			switch( nTemp ) {
				case 12:
					strcat( pResult, "A," );
					break;

				case 11:
					strcat( pResult, "K," );
					break;

				case 10:
					strcat( pResult, "Q," );
					break;

				case 9:
					strcat( pResult, "J," );
					break;
                    
				default:
					sprintf( szTemp, "%d,", nTemp + 2 );
					strcat( pResult, szTemp );
					break;
				}
		}
	}
	nTemp = strlen( pResult );
	pResult[nTemp - 1] = ']';

	switch(pJokbo[0]) {
		case 11:
			strcpy( szTemp, "로열스트레이트 플러쉬" );
			break;

		case 10:
			strcpy( szTemp, "스트레이트 플러쉬" );
			break;

		case 9:
			strcpy( szTemp, "포카드" );
			break;

		case 8:
			strcpy( szTemp, "풀하우스" );
			break;

		case 7:
			strcpy( szTemp, "플러쉬" );
			break;

		case 6:
			strcpy( szTemp, "마운틴" );
			break;

		case 5:
			strcpy( szTemp, "빽스트레이트" );
			break;

		case 4:
			strcpy( szTemp, "스트레이트" );
			break;

		case 3:
			strcpy( szTemp, "트리플" );
			break;

		case 2:
			strcpy( szTemp, "투페어" );
			break;

		case 1:
			strcpy( szTemp, "원페어" );
			break;

		case 0:
			strcpy( szTemp, "탑" );
			break;
	}

	strcat( pResult, szTemp );
}
*/


POINT CMainFrame::SetPoint(LONG x, LONG y)
{
	POINT pt;

	pt.x = x;
	pt.y = y;

	return pt;
}



void CMainFrame::RenderWaitRoom()
{
    m_SPR_WaitBase->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );
    m_MakeRoomBTN->DrawButton();
	m_JoinBTN->DrawButton();
	m_QuickJoinBTN->DrawButton();
    m_SCB_WaitRoom->Draw();                     // 방 리스트 스크롤 바

    if (m_nUserListTitleNum == 0 || m_nUserListTitleNum == 1) {
        m_SPR_UserListTitle[m_nUserListTitleNum]->SetPos( 698, 160 );
        m_SPR_UserListTitle[m_nUserListTitleNum]->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
    }

    RenderWaitChat();                           // 대기실 채팅창

    if ( m_nUserListTitleNum == 0 ) {           // 대기자 목록에서만 스크롤바 출력. 방유저 목록은 유저 수가 적어서 필요없다.
        m_SCB_WaitUser->Draw();
    }

    if ( m_nHighlightBarNum != -1 ) {
        m_SPR_HighlightBar->SetPos(713, 231 + (m_nHighlightBarNum * 25));
        m_SPR_HighlightBar->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
    }

    if ( g_nWaitRoomSelIdx != -1 ) {            // 선택된 방 배경 표시
        int nStartElem = m_SCB_WaitRoom->GetStartElem();

        if ( g_nWaitRoomSelIdx >= nStartElem &&
             g_nWaitRoomSelIdx <  nStartElem + MAX_VIEW_WAIT_ROOM)
        {
            m_SPR_SelRoomBar->SetPos(35, 208 + (((g_nWaitRoomSelIdx - nStartElem) % MAX_VIEW_WAIT_ROOM) * 33));
            m_SPR_SelRoomBar->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
        }
    }

    if ( m_pScrollNotice != NULL ) {
        m_pScrollNotice->Draw();                    // 스크롤되는 공지사항
    }
    
    //========================================================================
	//대기실에서 방리스트 뿌려주기.
	//========================================================================

	BOOL bBoldType = FALSE;
	int nRoomInCurCnt = 0;
	int nUserNo = 0;
    int nRoomNo = 0;
    int nRoomViewCnt = 0;
    int i = 0;
    int j = 0;
    int nYPos = 217;                            // 텍스트나 이미지 출력의 기준 좌표

    nRoomViewCnt = 0;
    
	for ( i = m_SCB_WaitRoom->GetStartElem(); i < g_nWaitRoomCnt; i++ ) {
        nRoomNo = g_nWaitRoomIdx[i];

        if ( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].nCurCnt <= 0 ) {
            continue;
        }

        // 입장 가능 여부 ------------------------------------------------

		nRoomInCurCnt = g_CWaitRoomList.m_sWaitRoomList[nRoomNo].nCurCnt;

		if( nRoomInCurCnt >= MAX_ROOM_IN ) {    //방이 FULL 이면 FULL 마크 찍어준다.
            m_SPR_FullWait->SetFrame( 0 );
			m_SPR_FullWait->SetPos( 69 , nYPos - 5 );
            m_SPR_FullWait->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			bBoldType = FALSE;
		}
		else {                                  //방이 FULL 이 아니면 빈 마크 찍어준다.
            m_SPR_FullWait->SetFrame( 1 );
			m_SPR_FullWait->SetPos( 69 , nYPos - 5 );
            m_SPR_FullWait->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			bBoldType = TRUE;
		}


        // 방 번호 -------------------------------------------------------
        
        sprintf( m_szTempText, "%03d", nRoomNo + 1);

        if ( bBoldType ) {
            DrawText( m_szTempText, 132 , nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_LEFT );
        }
        else {
            DrawText( m_szTempText, 132 , nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_LEFT );
        }


		// 방 이름 -------------------------------------------------------

        if ( bBoldType ) {
            DrawText( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].szRoomName, 250 , nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_LEFT );
        }
        else {
            DrawText( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].szRoomName, 250 , nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_LEFT );
        }


		// 방 머니 -------------------------------------------------------

		sprintf( m_szTempText, "%d", g_CWaitRoomList.m_sWaitRoomList[nRoomNo].biRoomMoney );

        if ( bBoldType ) {
            DrawText( m_szTempText, 501 , nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_CENTER );
        }
        else {
            DrawText( m_szTempText, 501 , nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_CENTER );
        }


		// 방 인원 -------------------------------------------------------
        
		sprintf( m_szTempText , "%d/%d" , g_CWaitRoomList.m_sWaitRoomList[nRoomNo].nCurCnt, MAX_ROOM_IN );

        if ( bBoldType ) {
            DrawText( m_szTempText, 593, nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_LEFT );
        }
        else {
            DrawText( m_szTempText, 593, nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_LEFT );
        }


        // 비밀방 여부 ---------------------------------------------------

        if ( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].bSecret ) {
            m_SPR_FullWait->SetFrame( 3 );
        }
        else {
            m_SPR_FullWait->SetFrame( 2 );
        }

        m_SPR_FullWait->SetPos( 622 , nYPos - 3 );
        m_SPR_FullWait->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

        //----------------------------------------------------------------
        

        nYPos += 33;                                // 다음줄 출력을 위해 Y 좌표 이동
        nRoomViewCnt++;

        if ( nRoomViewCnt >= MAX_VIEW_WAIT_ROOM ) { // 한 화면에 보이는 만큼만 출력한다.
            break;
        }
	}


    //========================================================================
    // 대기실 유저 리스트를 뿌려준다.
    //========================================================================

    int nTempClass = 0;
    nYPos = 0;

    if ( m_nUserListTitleNum == 0 ) {
	    nRoomViewCnt = 0;
        nYPos = 233;                                // 대기실 유저 리스트의 첫번째 줄의 Y 좌표

	    for( i = m_SCB_WaitUser->GetStartElem(); i < MAX_CHANNELPER; i++ ) {
            if ( g_CUserList.m_WaitUserInfo[i].m_bUser == false ) {
                continue;
            }

            // 계급 ------------------------------------------------------
            
            nTempClass = g_CUserList.m_WaitUserInfo[i].m_sUserInfo.nClass;

            m_SPR_Class->SetFrame( nTempClass );
		    m_SPR_Class->SetPos( 729 , nYPos + 2 );
            m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );


            // 성별 ------------------------------------------------------
            
		    if( g_CUserList.m_WaitUserInfo[i].m_sUserInfo.cSex == '0' ) {   // 여자
                m_SRP_SexIcon->SetFrame( 1 );
			    m_SRP_SexIcon->SetPos( 776, nYPos );
                m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		    }
		    else {                                                          // 남자
                m_SRP_SexIcon->SetFrame( 0 );
			    m_SRP_SexIcon->SetPos( 776 , nYPos );
                m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		    }


            // 닉네임 ----------------------------------------------------

            NickSave( m_szTempText, g_CUserList.m_WaitUserInfo[i].m_sUserInfo.szNick_Name );

            if ( (i - m_SCB_WaitUser->GetStartElem()) == m_nHighlightBarNum ) {
                DrawText( m_szTempText, 850, nYPos + 5, RGB(5, 26, 51), FW_NORMAL, TA_CENTER );
            }
            else {
                DrawText( m_szTempText, 850, nYPos + 5, RGB( 172 , 246 , 243 ), FW_NORMAL, TA_CENTER );
            }

            // 초대 가능 여부 출력 ---------------------------------------

            if ( g_CUserList.m_WaitUserInfo[i].m_sUserInfo.bInvite ) {
                PutSprite( m_SPR_ChodaeOX, 915, nYPos + 4, 1 );
            }
            else {
                PutSprite( m_SPR_ChodaeOX, 915, nYPos + 4, 0 );
            }

            nYPos += 25;
            nRoomViewCnt++;

            if( nRoomViewCnt >= MAX_VIEW_WAIT_USER ) {
			    break;
            }
	    }
    }

    //========================================================================
    // 대기실에서 선택된 방의 유저 리스트
    //========================================================================

    else if (m_nUserListTitleNum == 1) {
	    if( m_nSelectedRoomNum >= 0 ) {

            nYPos = 233;
				    
		    for( int j = 0 ; j < MAX_ROOM_IN; j++ ) {
                if ( g_ClickRoomUserInfo[j].szNick_Name[0] == NULL ) {
                    continue;
                }


                // 계급 --------------------------------------------------

                nUserNo = g_ClickRoomUserInfo[j].nClass;

                m_SPR_Class->SetFrame( nUserNo );
			    m_SPR_Class->SetPos( 729 , nYPos + 2 );
                m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );


                // 성별 --------------------------------------------------

			    if( g_ClickRoomUserInfo[j].cSex == '0' ) {  // 여자
                    m_SRP_SexIcon->SetFrame( 1 );
				    m_SRP_SexIcon->SetPos( 776, nYPos );
                    m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			    }
			    else {                                      // 남자
                    m_SRP_SexIcon->SetFrame( 0 );
				    m_SRP_SexIcon->SetPos( 776 , nYPos );
                    m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			    }


                // 닉네임 ------------------------------------------------

                NickSave( m_szTempText, g_ClickRoomUserInfo[j].szNick_Name );

                if (j == m_nHighlightBarNum ) {
                    DrawText( m_szTempText, 850, nYPos + 5, RGB( 5 , 26 , 51 ), FW_NORMAL, TA_CENTER );
                }
                else {
                    DrawText( m_szTempText, 850, nYPos + 5, RGB( 172 , 246 , 243 ), FW_NORMAL, TA_CENTER );
                }

                // 초대 가능 여부 출력 ---------------------------------------

                if ( g_ClickRoomUserInfo[j].bInvite ) {
                    PutSprite( m_SPR_ChodaeOX, 915, nYPos + 4, 1 );
                }
                else {
                    PutSprite( m_SPR_ChodaeOX, 915, nYPos + 4, 0 );
                }

                nYPos += 25;
		    }
	    }
    }
    

    if ( m_nUserListTitleNum == 0 ) {
        if ( m_nViewUserIdx != -1 ) {
            RenderWaitUser( m_nViewUserIdx );  // 선택한 대기실 유저의 자세한 정보 출력
        }
        else {
            RenderWaitUser( 0 );                // 선택한 대기실 유저가 없으면 유저 자신의 정보 출력
        }
    }
    else if ( m_nUserListTitleNum == 1 ) {      // 선택한 방 유저의 자세한 정보 출력
        if ( m_nViewUserIdx != -1 ) {
            RenderRoomUser( m_nViewUserIdx );
        }
        else {
            RenderWaitUser( 0 );                // 선택한 방 유저가 없으면 유저 자신의 정보 출력
        }
    }

	if ( m_bShowMakeRoomDlg ) {
		RenderMakeRoom();                       // 방만들기 다이알로그 렌더링
	}

	if ( m_bShowChodaeBox ) {
        DrawInvite( CHODAE_RECV_POS_X, CHODAE_RECV_POS_Y );
	}

    if ( m_bPassDlg ) {
        RenderPassDlg();                                // 비밀방 들어갈 때의 비밀번호 입력 창
    }

    //유저가 대기실 리스트에 마우스를 올려놓으면 버튼형식으로 그리기 위해서.
	DrawUserState( m_TempMX , m_TempMY );
}



//========================================================================
// 자세한 대기자 정보를 출력한다.
//========================================================================

void CMainFrame::RenderWaitUser( int nUserNum )
{
    //========================================================================
	// 대기실유저를 클릭하면 상세 정보를 뿌려준다.
    // 대기실 창에 Character Information정보 부분.
    //========================================================================

    int nWaitUserSelNo = nUserNum;   // 전체 리스트에서의 대기자 번호
    int nTempClass = 0;
	int nWinCnt = 0;
	int nLoseCnt = 0;
	int nDrawCnt = 0;
	int nAllCnt = 0;

	if ( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_bUser == TRUE ) {
		if ( nWaitUserSelNo >= 0 ) {
			// 별명
			if ( strlen( g_CUserList.m_WaitUserInfo[ nWaitUserSelNo ].m_sUserInfo.szNick_Name ) > 10 ) {
				memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
				memcpy( g_szTempStr, g_CUserList.m_WaitUserInfo[ nWaitUserSelNo].m_sUserInfo.szNick_Name, 10 );
				strcat( g_szTempStr, "..." );
                DrawText( g_szTempStr, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
			}
			else {
                DrawText( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.szNick_Name, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
			}

			// 금액
            if ( MouseIn(832, 592, 978, 606) ) {
                I64toA_Money( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.biUserMoney, m_szTempText );
            }
            else {
                I64toA_Money( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.biUserMoney, m_szTempText, 2 );
            }
            DrawText( m_szTempText, 885, 594, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
			
			// 계급
			nTempClass = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nClass;

            m_SPR_Class->SetFrame( nTempClass );
			m_SPR_Class->SetPos( 885 , 615 );
            m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

			nWinCnt  = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nWin_Num;
			nLoseCnt = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nLose_Num;
			nDrawCnt = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nDraw_Num;

			nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );

			// 전적
			sprintf( m_szTempText , "%d전 %d승" , nAllCnt, nWinCnt );
            DrawText( m_szTempText, 885, 642, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

			sprintf( m_szTempText , "%d패 %d무" , nLoseCnt, nDrawCnt );
            DrawText( m_szTempText, 885, 666, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

			PutSpriteAvatar( 721, 548, m_SPR_WaitAvatar, m_WaitAvaImg, ORIGINAL_SIZE );
		}
	}
}



//========================================================================
// 자세한 방 유저 정보를 출력한다.
//========================================================================

void CMainFrame::RenderRoomUser( int nUserNum )
{
	int nWinCnt = 0;
	int nLoseCnt = 0;
	int nDrawCnt = 0;
	int nAllCnt = 0;
    int nUserNo = 0;

	if ( g_ClickRoomUserInfo[nUserNum].szNick_Name[0] != NULL ) {
		// 별명
		if ( strlen( g_ClickRoomUserInfo[nUserNum].szNick_Name ) > 10 ) {
			memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
			memcpy( g_szTempStr, g_ClickRoomUserInfo[nUserNum].szNick_Name, 10 );
			strcat( g_szTempStr, "..." );

            DrawText( g_szTempStr, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
		}
		else {
            DrawText( g_ClickRoomUserInfo[nUserNum].szNick_Name, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
		}

		// 금액
        if ( MouseIn(832, 592, 978, 606) ) {
		    I64toA_Money( g_ClickRoomUserInfo[nUserNum].biUserMoney, m_szTempText );
        }
        else {
            I64toA_Money( g_ClickRoomUserInfo[nUserNum].biUserMoney, m_szTempText, 2 );
        }
        DrawText( m_szTempText, 885, 594, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

		// 계급
		nUserNo = g_ClickRoomUserInfo[nUserNum].nClass;
        m_SPR_Class->SetFrame( nUserNo );
		m_SPR_Class->SetPos( 885 , 615 );
        m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		nWinCnt  = g_ClickRoomUserInfo[nUserNum].nWin_Num;
		nLoseCnt = g_ClickRoomUserInfo[nUserNum].nLose_Num;
		nDrawCnt = g_ClickRoomUserInfo[nUserNum].nDraw_Num;

		nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );
		
		// 전적
		sprintf( m_szTempText , "%d전 %d승" , nAllCnt, nWinCnt );
        DrawText( m_szTempText, 885, 642, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

		sprintf( m_szTempText , "%d패 %d무" , nLoseCnt , nDrawCnt );
        DrawText( m_szTempText, 885, 666, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

		PutSpriteAvatar( 721, 548, m_SPR_WaitAvatar, m_WaitAvaImg, ORIGINAL_SIZE );
	}
}



//========================================================================
// 쪽지를 출력한다.
//========================================================================

void CMainFrame::DrawMessageBox( int X, int Y )
{
	char szTempNick[126], szTempMsg[255];
	
	m_SPR_MessageBox->SetPos( X, Y );
    m_SPR_MessageBox->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

	m_MemoSendBTN->SetPos( X + 41, Y + 211 );
	m_MemoSendBTN->DrawButton();

	m_MemoCancelBTN->SetPos( X + 178, Y + 211 );
	m_MemoCancelBTN->DrawButton();
	
	if( m_bMessageSend ) {
		sprintf( szTempNick, "%s님에게 보내는 쪽지입니다.", m_szMessageRecieveUserNick );
	}
	else {
		sprintf( szTempNick, "%s님이 보낸 쪽지입니다.", m_szMessageRecieveUserNick );
		
		// 보낸 사람이 쓴 메세지를 보여준다
		strcpy( szTempMsg, m_szGetMessageData );
        DrawText( szTempMsg, X + 28, Y + 94, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
	}

    DrawText( szTempNick, X + 29, Y + 67, RGB( 208, 208, 208 ), FW_NORMAL, TA_LEFT );
    DrawText( szTempNick, X + 28, Y + 66, RGB(   0,   0,   0 ), FW_NORMAL, TA_LEFT );

    if ( m_IME_Memo->IsInputState() ) {
       g_pCDXScreen->GetBack()->FillRect( X + 24, Y + 183, X + 301, Y + 199, RGB(255, 255, 255) );
       m_IME_Memo->PutIMEText( X + 28, Y + 186, RGB( 0, 0, 0 ), RGB( 0, 0, 0 ) );
    }
}



void CMainFrame::DrawUserState( int X , int Y )
{
	if( m_bUserState ) {
		int TempX = X - 930;
		
		if( X > 930 ) {
			X = 930 - TempX;
		}

        m_SPR_UserState->SetPos( X , Y );
        m_SPR_UserState->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		
		m_UserStateBTN->SetPos( X + 2 , Y + 2 );
		m_UserStateBTN->DrawButton( X + 2 , Y + 2 );
		
        DrawText( "메세지 보내기", X + 4, Y + 4, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
	}
}



//========================================================================
// 초대 받았을 때의 대화상자를 그린다.
//========================================================================

void CMainFrame::DrawInvite( int X , int Y )
{
	char szTempText[MAX_CHATMSG]; // 임시변수 ( 메세지 내용 )
	memset( szTempText, 0x00, sizeof( szTempText ));

    PutSprite( m_SPR_Invitation, X, Y );

	m_ChodaeYesBTN->SetPos( X + 41, Y + 176 );
	m_ChodaeYesBTN->DrawButton();

	m_ChodaeNoBTN->SetPos( X + 178, Y + 176 );
	m_ChodaeNoBTN->DrawButton();

	sprintf( szTempText , "%d번방에서 초대하셨습니다." , m_nInviteRoomNo + 1 );
    DrawText( szTempText, X + 163, Y + 92, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
    DrawText( "초대에 응하시겠습니까?", X + 163, Y + 120, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
}



void CMainFrame::PutSpriteAvatar( int x, int y, CDXSprite *aAvatarSPR, ImageEx *aAvaImageEx, int nType )
{
	if ( nType == SMALL_SIZE ) {
        DrawAvatarScale( aAvatarSPR, x, y, 85, 120 );
	}
	else {
		aAvatarSPR->SetPos( x, y );
        aAvatarSPR->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}
}



//========================================================================
// 방만들기 창을 띄운다.
//========================================================================

void CMainFrame::OpenMakeRoomDlg()
{
    m_bPassRoomChk = FALSE;             // 비밀방 체크 해제
    m_bShowRoomTitleList = FALSE;       // 방 이름 목록을 안보여주도록 초기화
    m_nSelectedRoomTitleList = -1;      // 선택된 방이름 목록 초기화

    // 기본 방이름 5개 중에서 임의로 하나 선택해서 띄운다.

	int nTemp = GetRandom( 0, 4 );

    m_IME_InputRoomTitle->SetIMEText( g_RoomTitle[nTemp] );
    m_IME_InputRoomTitle->InputBegin( false );

	if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
		m_bUserState = FALSE;
		m_biMakeRoomMoneyNo = 0;
        m_nCardOption = CO_CHOICE_MODE; // 방만들때 기본 모드는 초이스 모드
		m_bShowMakeRoomDlg = TRUE;

        // 채팅창이 입력상태면 입력 불가능 상태로 만든다.
        m_IME_WaitChat->InputEnd();
	}
	else {
		m_Message->PutMessage( "게임을 종료하고 충전후 이용하세요!!!", "Code - 132" );
	}
}



//========================================================================
// 방을 만든다.
//========================================================================

void CMainFrame::MakeRoom( BOOL bSecret ) //true 비밀
{
	char szJunk[512];
	sCreateRoom l_sCreateRoom;

	memset( &szJunk , 0x00 , sizeof( szJunk ));	
	memset( &l_sCreateRoom, 0x00, sizeof(l_sCreateRoom) );

    strcpy( szJunk, m_IME_InputRoomTitle->GetIMEText() );

	if( strlen( szJunk ) > 0 ) {
		m_bShowMakeRoomDlg = FALSE;

		memset( &l_sCreateRoom, 0x00, sizeof(l_sCreateRoom) );

		if ( !bSecret ) {
			l_sCreateRoom.bSecret = false;			
		}
		else {
			l_sCreateRoom.bSecret = true;
			strcpy( l_sCreateRoom.szRoomPass , m_szPass );
		}

		strcpy( l_sCreateRoom.szRoomName, szJunk );		
		
        m_IME_InputPass->InputEnd();
        m_IME_InputPass->ClearBuffer();

        m_IME_InputRoomTitle->InputEnd();
        m_IME_InputRoomTitle->ClearBuffer();

		l_sCreateRoom.nTotCnt = MAX_ROOM_IN;

		switch( m_biMakeRoomMoneyNo ) {
			case 0:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY1;
				break;
/*
			case 1:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY2;
				break;
*/
			case 2:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY3;
				break;
/*
			case 3:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY4;
				break;
*/
            case 4:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY5;
				break;
/*
            case 5:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY6;
				break;
*/
            case 6:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY7;
				break;

        /*    case 7:
				l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY8;
				break;*/
		}

        // 초이스 모드인지, 일반모드인지 결정한다.
        if ( m_nCardOption == CO_CHOICE_MODE ) {
            l_sCreateRoom.bChoiceRoom = TRUE;
        }
        else {
            l_sCreateRoom.bChoiceRoom = FALSE;
        }

		//유저 머니에 따라서 방을 만들수 있는 권한이 정해진다.
		if ( AvailableRoomIn( l_sCreateRoom.biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) == FALSE ) {
			sprintf( m_szTempText, "유저의 보유 톨이 해당방기준과 맞지 않습니다." );
			m_Message->PutMessage( m_szTempText, "다시 확인해 주세요!!!", false );
			return;
		}

		if ( !g_pClient->CreateRoom(&l_sCreateRoom) ) {
			m_Message->PutMessage( "방생성 실행 실패", "Code - 124" );		
		}
	}
	else {
		m_Message->PutMessage( "방제목을 입력하세요.!!!", "Code - 125" );
	}
}



BOOL CMainFrame::SendBtnTime( CDXButton *Btn , int nTime , BOOL bWavePlay )
{
	int nTempTime;	

    if( bWavePlay ) {
        PlaySound( g_Sound[GS_BTNCLICK] );
    }
	
	for( int i = 0 ; i < 50 ; i ++ ) {
		if( m_szBtnTime[i].CDXBtn == Btn ) {
			nTempTime = nTime - m_szBtnTime[i].nTime;
			
			if( DEF_TIME < nTempTime ) {
				m_szBtnTime[i].nTime = nTime;
				return TRUE;
			}

			return FALSE;
		}		
	}

	for( i = 0 ; i < 50 ; i ++ ) {
		if( m_szBtnTime[i].CDXBtn == NULL ) {
			m_szBtnTime[i].CDXBtn = Btn;
			m_szBtnTime[i].nTime = nTime;
			return TRUE;
		}		
	}

    return FALSE;
}



void CMainFrame::SetHumanVoice( int nVoiceNo )
{
    for( int i = 50 ; i < 81 ; i ++ ) {
        if( g_Sound[i] != NULL ) {
            g_Sound[i]->Stop();		
        }
	}

	if ( g_pCDXSound != NULL ) {
		for( int i = 50; i < 81 ; i ++) {
            SAFE_DELETE( g_Sound[i] );
		}
	}
}



void CMainFrame::ProcessButtonEvent()
{
    //---------------------
    // 경고창이 떠있는 경우
    //---------------------

	if( m_Message->GetbShowCheck() ) {
		if( m_MessageOKBTN->status == DOWN ) {
			m_MessageOKBTN->SetMouseLBDownStatus(FALSE);
			
			PlaySound(g_Sound[GS_BTNCLICK]);
			m_Message->SetbShowCheck( FALSE );
			
			if ( m_Message->m_bCloseGame ) {
				g_bPaketThredOn = false;
				Sleep(1000);
                DestroyAllSound();
                ExitGame();
			}
			
			return;
		}
	}

	if ( g_bConnectingSpr ) return;

	if ( m_bShowUserInfo ) {
		if ( m_UserCloseBTN->status == DOWN ) {
			m_UserCloseBTN->SetMouseLBDownStatus( FALSE );
			m_nShowUserSlot = 0;
			m_bShowUserInfo = FALSE;
		}
	}

    
	if( m_bShowChodaeBox )
	{
		// 초대 "예" 버튼
		if( SendBtnTime( m_ChodaeYesBTN , GTC()) )
		{
			if( m_ChodaeYesBTN->status == DOWN )
			{
				m_ChodaeYesBTN->SetMouseLBDownStatus( FALSE );

				m_bShowChodaeBox = FALSE;
				if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
				
					//자신이 들어갈수 있는 방인지를 조사한다.
					if ( AvailableRoomIn( m_biInviteRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
					
						sRoomIn TempRoom;
						memset( &TempRoom , 0x00 , sizeof( TempRoom ));
						
						TempRoom.nRoomNo = m_nInviteRoomNo;			
						strcpy( TempRoom.szRoomPass , m_szInviteRoomPass );
						
						g_pClient->RoomIn( &TempRoom );
					}
					else {
						m_Message->PutMessage( "유저의 보유머니로 해당방을", "입장하실수 없습니다.!!!", false );
					}

				}
				else {
					m_Message->PutMessage( "게임을 종료하고 충전후 이용하세요!!!", "Code - 161" );
				}

				return;
			}			
		}		

		// 초대 "아니오" 버튼
		if( m_ChodaeNoBTN->status == DOWN )
		{
			m_ChodaeNoBTN->SetMouseLBDownStatus( FALSE );

			m_bShowChodaeBox = FALSE;
			
			return;
		}

        return;
	}
   
	if ( m_nCurrentMode == GAME_MODE) {
        //게임방에서 아바타를 클릭하면 정보가 나온다.
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
            if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo <= 0 ) {
                continue;
            }

			if ( MouseIn( m_sRoomInUserPosition[i].pAvatarPos.x,
                          m_sRoomInUserPosition[i].pAvatarPos.y,
                          m_sRoomInUserPosition[i].pAvatarPos.x + 85,
                          m_sRoomInUserPosition[i].pAvatarPos.y + 120 ) )
            {
				m_nShowUserSlot = i;
				m_bShowUserInfo	= TRUE;
			}
		}

        // 게임화면 채팅창 스크롤바 마우스 클릭 처리
        m_SCB_GameChat->MouseClick( m_mX, m_mY );
	}

	//유저가 대기실 리스트에 마우스를 올려놓으면 버튼형식으로 그리기 위해서.
	//버튼 먹게 하기위해서.
	if( m_bUserState == TRUE ) {
		if( m_UserStateBTN->status == DOWN ) {
			m_UserStateBTN->SetMouseLBDownStatus(FALSE);

			if( strcmp( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, m_szMessageRecieveUserNick ) ) {
				m_bShowMessageBox = TRUE;
                m_IME_Memo->ClearBuffer();
                m_IME_Memo->InputBegin( true );
			}
		}

		m_bUserState = FALSE;
		return;
	}

	//쪽지 메세지 박스에서 입력창을 띠운다.
	if( m_bShowMessageBox ) {
        if ( MouseIn( 382, 436, 643, 456 ) ) {  // 쪽지 입력 부분 누르면 활성화
            m_IME_Memo->InputBegin( true );
        }
        else {                                  // 다른 부분 누르면 비활성화
            m_IME_Memo->InputBegin( false );
        }

		if( SendBtnTime( m_MemoSendBTN , GTC()) ) {
			if( m_MemoSendBTN->status == DOWN ) {
				m_MemoSendBTN->SetMouseLBDownStatus( FALSE );

                char *pIMEText = m_IME_Memo->GetIMEText();

                if( strlen( pIMEText ) > 0 ) {
					memset( &m_szMessageSendUserNick , 0x00 , sizeof( m_szMessageSendUserNick ));
					memset( &m_szMessageData , 0x00 , sizeof( m_szMessageData ));
					
					strcpy( m_szMessageSendUserNick , g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );
					strcpy( m_szMessageData , pIMEText );		
					
                    m_IME_Memo->InputEnd();
                    m_IME_Memo->ClearBuffer();

					m_bShowMessageBox = FALSE;
					
					g_pClient->MemoSend( m_szMessageSendUserNick , m_szMessageRecieveUserNick , m_szMessageData );
				}
				else {
                    m_IME_Memo->InputEnd();
					m_bShowMessageBox = FALSE;
				}
		
				return;
			}
		}

		if( m_MemoCancelBTN->status == DOWN ) {
			m_MemoCancelBTN->SetMouseLBDownStatus( FALSE );
            m_IME_Memo->InputEnd();
            m_IME_Memo->ClearBuffer();
			m_bShowMessageBox = FALSE;
		}
		
		return;
	}

    //-----------------------------------
	// 처음 유저 카드 덤프 부분.
    // 4 장의 카드 중 버릴 카드 한장 선택
    //-----------------------------------

	if ( m_bCardDump ) {
		if( MouseIn( 333, 328, 414, 436 ) ) {
			m_bCardDump = FALSE;
			m_nCardDumpCnt = 0;
			m_bCardChoice = TRUE;
			g_CGameUserList.UserCardDump( 0, 0 );
			g_pClient->CardDump( 0 );
		}
		else if( MouseIn( 425, 328, 506, 436 ) ) {
			m_bCardDump = FALSE;
			m_nCardDumpCnt = 0;
			m_bCardChoice = TRUE;
			g_CGameUserList.UserCardDump( 0, 1 );
			g_pClient->CardDump( 1 );
		}
		else if( MouseIn( 517, 328, 598, 436 ) ) {
			m_bCardDump = FALSE;
			m_nCardDumpCnt = 0;
			m_bCardChoice = TRUE;
			g_CGameUserList.UserCardDump( 0, 2 );
			g_pClient->CardDump( 2 );
		}
		else if( MouseIn( 609, 328, 690, 436 ) ) {
			m_bCardDump = FALSE;
			m_nCardDumpCnt = 0;
			m_bCardChoice = TRUE;
			g_CGameUserList.UserCardDump( 0, 3 );
			g_pClient->CardDump( 3 );
		}

		return;
	}

    //-----------------------------------
	// 처음 유저 카드 초이스 부분.
    // 3장의 카드 중 오픈할 카드 1장 선택
    //-----------------------------------

	if ( m_bCardChoice ) {
		if( MouseIn( 379, 328, 460, 436 ) ) {
			m_bCardChoice = FALSE;
			m_nCardChoiceCnt = 0;
			g_CGameUserList.UserCardChoice( 0, 0 );
			g_pClient->CardChoice( 0 );
		}
		else if( MouseIn( 471, 328, 552, 436 ) ) {
			m_bCardChoice = FALSE;
			m_nCardChoiceCnt = 0;
			g_CGameUserList.UserCardChoice( 0, 1 );
			g_pClient->CardChoice( 1 );
		}
		else if( MouseIn( 563, 328, 644, 436 ) ) {
			m_bCardChoice = FALSE;
			m_nCardChoiceCnt = 0;
			g_CGameUserList.UserCardChoice( 0, 2 );
			g_pClient->CardChoice( 2 );
		}

		if ( !m_bCardChoice ) {
			//카드위치를 다시 제자리로 놓는다.
			g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
            
			for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
				g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
				g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
			}

			PlaySound(g_Sound[GS_DEAL]);
		}
		return;
	}

    //--------------------------------------
    // 초대하기 대화상자의 클릭 및 버튼 처리
    //--------------------------------------

	if( m_bShowInviteBox ) {
		InputInviteChecks( CHODAE_POS_X + 23, CHODAE_POS_Y + 84, 261, 27 );

		// 초대 박스 안의 초대(확인)을/를 눌렀다.
		if( m_InviteOKBTN->status == DOWN ) {
			m_InviteOKBTN->SetMouseLBDownStatus( FALSE );

			int nTempInviteUserNo = GetInviteCheck();

			if( nTempInviteUserNo >= 0 ) {
				g_pClient->UserInvite( g_sGameWaitInfo[nTempInviteUserNo].nUserNo , "" );
			}

            if( m_bUserState ) {
                m_bUserState = FALSE;
            }

			m_bShowInviteBox = FALSE;
			return;
		}

        // 취소 버튼 눌렀을 때

        if ( m_InviteCancelBTN->status == DOWN ) {
            m_InviteCancelBTN->SetMouseLBDownStatus( FALSE );
            m_bUserState = FALSE;
            m_bShowInviteBox = FALSE;
        }

        // 스크롤 바 클릭 및 버튼 처리

        m_SCB_Invite->MouseClick( m_mX, m_mY );
        return;
	}	

    //---------------------------------
    // 초대하기 버튼을 눌렀을 때의 처리
    //---------------------------------

	if( m_InviteBTN->status == DOWN ) {
		m_InviteBTN->SetMouseLBDownStatus( FALSE );
        m_SCB_Invite->StartPage();

		m_nWaitViewPageNo = 0;
		g_pClient->GameViewWait( m_nWaitViewPageNo );

		InitInviteChecks();
		return;
	}

	//결과창 떠있을떄 버튼들 막기.
    if ( m_bEnddingShow ) {
        return;
    }

    // 히든 카드 이동중이면 이동중지
    if ( m_bMoveHiddenCard ) {
        m_bMoveHiddenCard = FALSE;
        memset(&m_sHiddenCardInfo, 0x00, sizeof( m_sHiddenCardInfo ));
    }

	if( m_ExitBTN->status == DOWN ) {
		m_ExitBTN->SetMouseLBDownStatus(FALSE);

        if( SendBtnTime( m_ExitBTN , GTC()) ) {
            OnExitBtnDown();
        }

		m_bUserState = FALSE;
		return;
	}

	//게임 룸에서의 버튼 설정들
	if( m_bShowStartBTN ) {
		if( m_StartBTN->status == DOWN) {   // 시작 버튼 클릭시
			m_StartBTN->SetMouseLBDownStatus(FALSE);
			OnBtnStart();
			return;
		}
	}

	if( m_Exit2BTN->status == DOWN ) {
		m_Exit2BTN->SetMouseLBDownStatus(FALSE);

        if( SendBtnTime( m_Exit2BTN , GTC()) ) {
            OnExitBtnDown();
        }

		m_bUserState = FALSE;
		return;
	}

	if( m_MaxBTN->status == DOWN || m_MaxBTN2->status == DOWN || m_FullBTN->status == DOWN ) {
		m_MaxBTN->SetMouseLBDownStatus(FALSE);
		m_MaxBTN2->SetMouseLBDownStatus(FALSE);
		m_FullBTN->SetMouseLBDownStatus(FALSE);
	//	m_FullBTN->SetMouseLBDownStatus(TRUE);

		PlaySound(g_Sound[GS_BTNCLICK]);
		
		m_bFullScreen = !m_bFullScreen;
		ChangeVideoMode( m_bFullScreen );

		return;
	}

	if( m_MinBTN->status == DOWN ) {
		m_MinBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK]);

		if ( m_bFullScreen == TRUE ) { //풀스크린일때만.
			m_bFullScreen = !m_bFullScreen;
			ChangeVideoMode( m_bFullScreen );
		}

		::ShowWindow( g_hWnd, SW_MINIMIZE );
		return;
	}

    //------------------------------------
    // 환경설정 대화상자 클릭 및 버튼 처리
    //------------------------------------

	if( m_bShowConfigBox )
	{
		for( int i = 0; i < 4; i ++ ) {
			if( MouseIn( CONFIG_DLG_POS_X +  93, CONFIG_DLG_POS_Y + 54 + (i * 27),
                         CONFIG_DLG_POS_X + 202, CONFIG_DLG_POS_Y + 80 + (i * 27) ))
            {
				m_pConfigBox->ChangetCheck( i , TRUE );
			}
			else if( MouseIn( CONFIG_DLG_POS_X + 203, CONFIG_DLG_POS_Y + 54 + (i * 27),
                              CONFIG_DLG_POS_X + 301, CONFIG_DLG_POS_Y + 80 + (i * 27) ))
            {
				m_pConfigBox->ChangetCheck( i , FALSE );
			}
		}

		if      ( MouseIn( CONFIG_DLG_POS_X +  93, CONFIG_DLG_POS_Y + 162, CONFIG_DLG_POS_X + 202, CONFIG_DLG_POS_Y + 183 )) { m_pConfigBox->ChangeVoice( 1 ); } // 남자1
        else if ( MouseIn( CONFIG_DLG_POS_X + 203, CONFIG_DLG_POS_Y + 162, CONFIG_DLG_POS_X + 301, CONFIG_DLG_POS_Y + 183 )) { m_pConfigBox->ChangeVoice( 0 ); } // 여자1
        else if ( MouseIn( CONFIG_DLG_POS_X +  93, CONFIG_DLG_POS_Y + 163, CONFIG_DLG_POS_X + 202, CONFIG_DLG_POS_Y + 207 )) { m_pConfigBox->ChangeVoice( 2 ); } // 남자2
        else if ( MouseIn( CONFIG_DLG_POS_X + 203, CONFIG_DLG_POS_Y + 163, CONFIG_DLG_POS_X + 301, CONFIG_DLG_POS_Y + 207 )) { m_pConfigBox->ChangeVoice( 3 ); } // 여자2

		if( m_OkBTN->status == DOWN ) {
			m_OkBTN->SetMouseLBDownStatus(FALSE);

			PlaySound(g_Sound[GS_BTNCLICK]);
			
			//설정사항을 파일에 기록한다.	

			//BOOL bInvite;         // 초대     TRUE 허용 
			//BOOL bSecretMessage;  // 귓속말   TRUE 허용
			//BOOL bWaveSnd;        // 효과음   TRUE 허용
			//BOOL bMessageAgree;   // 메세지   TRUE 허용

			for( int i = 0 ; i < 4; i ++ ) {
				g_sClientEnviro.bOption[i] = m_pConfigBox->bCheck[i];
			}

			g_sClientEnviro.nVoice = m_pConfigBox->nVoice;

			_itoa( g_sClientEnviro.bOption[0], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","Invite",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.bOption[1], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","Eartalk",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.bOption[2], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","Sound",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.bOption[3], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","MemoRecv",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.nVoice, g_szTempStr, 255 );
			WritePrivateProfileString("Environment","Voice",g_szTempStr,".\\config.ini");
			
			g_pClient->SetClientOption( g_sClientEnviro.bOption[0], g_sClientEnviro.bOption[1], g_sClientEnviro.bOption[3] );

			SetHumanVoice( g_sClientEnviro.nVoice );

			m_bShowConfigBox = FALSE;
			
			return; // 설정 창 떠있으면 여기서 종료..
		}
        
        return;
	}

	if( m_ConfigBTN->status == DOWN ) {
		m_ConfigBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK]);

		m_bShowConfigBox = TRUE;
		return;
	}

    if( m_CaptureBTN->status == DOWN ) {            // 화면 캡쳐 하기
		m_CaptureBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK]);
		DTRACE("캡처 버튼 눌렸다");
        ScreenCapture();
		return;
	}

    // 방만들기 창이 떠있으면
	if( m_bShowMakeRoomDlg )
	{
        //-----------------------------------
        // 비밀번호 입력 체크박스 클릭한 경우
        //-----------------------------------

        if ( MouseIn( 626, 293, 642, 309)  ) {
            m_bPassRoomChk = !m_bPassRoomChk;

            if (m_bPassRoomChk) {
                m_IME_InputPass->ClearBuffer();
                m_IME_InputPass->InputBegin(true);      // 활성화(포커스 얻음)
            }
            else {
                m_IME_InputPass->InputEnd();
            }
        } 
        else if ( MouseIn( 443, 292, 621, 310 ) ) {     // 비밀번호 입력 부분
            if ( m_bPassRoomChk ) {
                m_IME_InputPass->InputBegin(true);
            }
        }
        else {
            m_IME_InputPass->InputBegin(false);         // 비활성화(포커스 잃음)
        }
        

        //---------------------------
        // 방 제목 입력란 클릭한 경우
        //---------------------------

        if ( MouseIn(443, 267, 623, 285 ) ) {
            m_IME_InputRoomTitle->InputBegin(true);     // 활성화
        }
        else {
            m_IME_InputRoomTitle->InputBegin(false);    // 비활성화
        }

        //------------------------------
        // 방 금액 선택 부분 클릭한 경우
        //------------------------------

		if( MouseIn( 375, 358, 645, 485 ) ) {
            if ( !m_bShowRoomTitleList ) {
                PlaySound(g_Sound[GS_BTNCLICK]);
            }
		}

        if ( m_RoomTitleBTN->status == DOWN ) {
            m_RoomTitleBTN->SetMouseLBDownStatus(FALSE);
            PlaySound(g_Sound[GS_BTNCLICK]);

            m_bShowRoomTitleList = !m_bShowRoomTitleList;

            // 방제목 리스트를 보여주는 경우 0번 제목을 기본적으로 선택한다.
            if (m_bShowRoomTitleList) {
                m_nSelectedRoomTitleList = 0;
            }
            else {
                m_nSelectedRoomTitleList = -1;
            }
        } else {
            if ( MouseIn( ROOM_TITLE_LIST_X,
                          ROOM_TITLE_LIST_Y,
                          ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH - 1,
                          ROOM_TITLE_LIST_Y + ROOM_TITLE_LIST_HEIGHT - 1) )
            {
                if (m_nSelectedRoomTitleList != -1) {
                    m_IME_InputRoomTitle->SetIMEText( g_RoomTitle[m_nSelectedRoomTitleList] );
                }
            }

            m_bShowRoomTitleList = FALSE;
            m_nSelectedRoomTitleList = -1;
        }

        // 확인 버튼 클릭시
		if( m_MakeRoomOkBTN->status == DOWN ) {
			m_MakeRoomOkBTN->SetMouseLBDownStatus(FALSE);
			PlaySound(g_Sound[GS_BTNCLICK]);

            //비밀방 생성.
            if ( m_bPassRoomChk ) {
                memset( m_szPass, 0x00, sizeof(m_szPass) );
                strcpy(m_szPass, m_IME_InputPass->GetIMEText());

                if ( m_szPass[0] != NULL ) {
			        MakeRoom(true);
                }
                else {
                    //메세지 박스.
                    m_IME_InputPass->InputEnd();
                    m_Message->PutMessage( "비밀번호를 입력하세요.", "Code - 600" );
                    return;
                }
            }
            else {
                MakeRoom(false);
            }

			return;
		}

        // 취소 버튼 클릭시
		if( m_MakeRoomCancelBTN->status == DOWN ) {
			m_MakeRoomCancelBTN->SetMouseLBDownStatus(FALSE);
			PlaySound(g_Sound[GS_BTNCLICK]);

			WaitRoomModeSet();

            m_IME_InputPass->InputEnd();
            m_IME_InputPass->ClearBuffer();

            m_IME_InputRoomTitle->InputEnd();
            m_IME_InputRoomTitle->ClearBuffer();

			return; // 방만들기 창이 열렸으면 여기서 종료
		}

        return;
	}

    //-----------------
    // 비밀번호 입력 창
    //-----------------

    if ( m_bPassDlg ) {
        if (MouseIn( PASS_DLG_POS_X + 94,
                     PASS_DLG_POS_Y + 71,
                     PASS_DLG_POS_X + 94 + 191,
                     PASS_DLG_POS_Y + 71 + 18 ))
        {
            m_IME_RoomInPassBox->InputBegin(true);
        }
        else {
            m_IME_RoomInPassBox->InputBegin(false);
        }

        if ( m_OkBTN->status == DOWN ) {
            m_OkBTN->SetMouseLBDownStatus(FALSE);
            PlaySound(g_Sound[GS_BTNCLICK]);

            sRoomIn l_sRoomInData; // 접속할 방의 구조체
			memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

            l_sRoomInData.nRoomNo = g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].nRoomNo;
            strcpy( l_sRoomInData.szRoomPass, m_IME_RoomInPassBox->GetIMEText() );

            m_bPassDlg = FALSE;
            m_IME_RoomInPassBox->InputEnd();
            m_IME_RoomInPassBox->ClearBuffer();

            if ( l_sRoomInData.szRoomPass[0] != NULL) {
                m_bUserState = FALSE;
			    g_pClient->RoomIn( &l_sRoomInData ); // 방으로 들어간다.
            }
            else {
                m_Message->PutMessage("비밀번호를 입력하세요.", "Code - 600");
            }

            m_bShowRoomTitleList = FALSE;
            m_nSelectedRoomTitleList = -1;
            return;
        }

        if ( m_CancelBTN->status == DOWN ) {
            m_CancelBTN->SetMouseLBDownStatus(FALSE);
            PlaySound(g_Sound[GS_BTNCLICK]);

            m_bPassDlg = FALSE;
            m_IME_RoomInPassBox->InputEnd();
            m_IME_RoomInPassBox->ClearBuffer();

            m_bShowRoomTitleList = FALSE;
            m_nSelectedRoomTitleList = -1;
            return;
        }
        return;
    }


	///////// 대기실 화면에서 사용하는 버튼들 /////////////
	if( m_JoinBTN->status == DOWN ) {
		m_JoinBTN->SetMouseLBDownStatus(FALSE);

		if( SendBtnTime( m_JoinBTN , GTC()) ) {
			if( m_nSelectedRoomNum >= 0 ) {
				if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
					if ( g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].nCurCnt >= MAX_ROOM_IN ) {
						m_Message->PutMessage( "방에 인원이 모두 찼습니다.", "Code - 126" );
						return;
					}

					//자신이 들어갈수 있는 방인지를 조사한다.
					if ( AvailableRoomIn( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
                        if ( g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].bSecret ) {
                            m_IME_RoomInPassBox->ClearBuffer();
                            m_IME_RoomInPassBox->InputBegin(true);
                            m_bPassDlg = TRUE;
                        }
                        else {
                            sRoomIn l_sRoomInData;                  // 접속할 방의 구조체
						    memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

						    l_sRoomInData.nRoomNo = g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].nRoomNo;
						    memset( l_sRoomInData.szRoomPass , 0 , sizeof(l_sRoomInData.szRoomPass));

						    m_bUserState = FALSE;
						    g_pClient->RoomIn( &l_sRoomInData );    // 방으로 들어간다.
                        }
					}
					else {
						m_Message->PutMessage( "유저의 보유머니로 해당방을", "입장하실수 없습니다.!!!", false );
					}
				}
				else {
					m_Message->PutMessage( "게임을 종료하고 충전후 이용하세요!!!", "Code - 133" );
				}
			}
			else {
				m_Message->PutMessage( "입장할 방을 선택해주세요" , "Code - 127" );
			}
		}

		return;
	}
	
	if( m_QuickJoinBTN->status == DOWN ) {
		m_QuickJoinBTN->SetMouseLBDownStatus(FALSE);

		if( SendBtnTime( m_QuickJoinBTN , GTC()) ) {
			if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
				g_pClient->QuickJoin();
			}
			else {
				m_Message->PutMessage( "게임을 종료하고 충전후 이용하세요!!!", "Code - 134" );
			}
		}
		
		return;
	}
	
	if ( m_nCurrentMode == WAITROOM_MODE ) {
        m_SCB_WaitChat->MouseClick( m_mX, m_mY );
        m_SCB_WaitRoom->MouseClick( m_mX, m_mY );

        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->MouseClick( m_mX, m_mY );
        }

        if ( MouseIn(700, 165, 820, 195) ) {    // 방유저 리스트 보다가 대기자 리스트 타이틀 클릭시
            if ( m_nUserListTitleNum == 1 ) {
                m_nViewUserIdx = -1;
                m_nViewUserNo = -1;
                m_nUserListTitleNum = 0;
                AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
            }
            return;
        }

        if ( MouseIn(821, 165, 921, 195) ) {    // 대기자 리스트 보다가 방 유저 리스트 타이틀 클릭시
            if ( m_nUserListTitleNum == 0 ) {
                m_nViewUserIdx = -1;
                m_nViewUserNo = -1;
                m_nUserListTitleNum = 1;
                AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
            }
            return;
        }

        // IME FOCUS 처리

        // 채팅창 부분 클릭하면 채팅창 입력상태로 만든다.
        if ( MouseIn( 30, 710, 643, 729 ) && !m_IME_WaitChat->IsInputState() ) {
            m_SCB_WaitChat->EndPage();          // 맨 아래 페이지로 이동
            m_IME_WaitChat->InputBegin(true);   // 입력 시작
        }

        // 채팅창 이외의 부분을 클릭하면 입력 불가능 상태로 만든다.
        else if ( !MouseIn( 23, 601, 680, 737 ) && m_IME_WaitChat->IsInputState() ) {
            m_IME_WaitChat->InputEnd();
            m_IME_WaitChat->ClearBuffer();
        }

        // 대기실에서 방 클릭 시
        else if ( MouseIn( 35, 206, 644, 502 - 2 ) ) {
            int nSelectedRoom = (m_mY + 1 - 206)  /  ((502 + 1 - 206) / MAX_VIEW_WAIT_ROOM);

            if ( nSelectedRoom != -1 &&
                 !(m_bShowMakeRoomDlg || m_bShowChodaeBox || m_bShowInviteBox || m_bShowConfigBox || m_bShowMessageBox || m_Message->GetbShowCheck()))
            {
                g_nWaitRoomSelIdx = nSelectedRoom + m_SCB_WaitRoom->GetStartElem();

                // 방이 없는 곳을 클릭한 경우
                if ( g_nWaitRoomSelIdx >= g_nWaitRoomCnt ) {
                    m_nSelectedRoomNum = -1;
                    g_nWaitRoomSelIdx = -1;
                }

                // 방이 있는 곳을 클릭한 경우
                else {
                    // 이전에 선택된 방 번호와 이번에 클릭한 방의 번호가 다르면 선택된 유저 없다고 표시
                    if ( m_nUserListTitleNum == 1 ) {
                        if ( m_nSelectedRoomNum != -1 && m_nSelectedRoomNum != g_nWaitRoomIdx[ g_nWaitRoomSelIdx ] ) {
                            memset( g_ClickRoomUserInfo, 0x00, sizeof( g_ClickRoomUserInfo ));
                            m_nViewUserIdx = -1;
                            m_nViewUserNo = -1;
                            AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
                        }
                    }

                    m_nSelectedRoomNum = g_nWaitRoomIdx[ g_nWaitRoomSelIdx ];
                }

                if ( m_nSelectedRoomNum >= 0 && m_nSelectedRoomNum < MAX_ROOMCNT ) {
				    PlaySound(g_Sound[GS_BTNCLICK]);

				    if( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt > 0 ) {

					    //여기에서 서버로 방에대한 정보를 요청한다.
					    g_pClient->GetRoomInfo( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nRoomNo );
				    }
			    }
			    
			    return;
            }
        }

		//대기실에서 방만들기를 클릭했을시
        if( SendBtnTime( m_MakeRoomBTN, GTC()) ) {
		    if( m_MakeRoomBTN->status == DOWN )
		    {
			    m_MakeRoomBTN->SetMouseLBDownStatus(FALSE);

                OpenMakeRoomDlg();                  // 방만들기 창을 띄운다.
			    return;
		    }
        }

        if( m_nHighlightBarNum != -1) {
            // 대기자 클릭
		    if( m_nUserListTitleNum == 0 ) {
			    int nSelWaitUserIdx = m_SCB_WaitUser->GetStartElem() + m_nHighlightBarNum;

			    if ( nSelWaitUserIdx >= 0 && nSelWaitUserIdx < m_nWaitUserCurCnt ) {
				    if( g_CUserList.m_WaitUserInfo[ nSelWaitUserIdx ].m_bUser == TRUE ) {
                        PlaySound(g_Sound[GS_BTNCLICK]);
					    m_nViewUserIdx = nSelWaitUserIdx;
                        m_nViewUserNo = g_CUserList.m_WaitUserInfo[ m_nViewUserIdx ].m_sUserInfo.nUserNo;
					    AvatarPlay( -1, g_CUserList.m_WaitUserInfo[ m_nViewUserIdx ].m_sUserInfo.szPreAvatar );
				    }
			    }
			    return;
		    }

            // 방 유저 클릭
            else if ( m_nUserListTitleNum == 1 ) {
                if ( m_nSelectedRoomNum != -1 ) {
                    // 방에 사람이 있고, 사람이 있는 위치를 선택했을 때만 정보를 보여준다.
			        if ( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt > 0 ) {
				        if ( m_nHighlightBarNum >= 0 && m_nHighlightBarNum < g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt ) {
                            PlaySound(g_Sound[GS_BTNCLICK]);

                            m_nViewUserIdx = m_nHighlightBarNum;
                            m_nViewUserNo = g_ClickRoomUserInfo[ m_nHighlightBarNum ].nUserNo;
					        AvatarPlay( -1, g_ClickRoomUserInfo[ m_nHighlightBarNum ].szPreAvatar );
				        }
                        else {
                            m_nViewUserIdx = -1;
                            m_nViewUserNo = -1;
                            AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
                        }
			        }
                    else {
                        m_nViewUserIdx = -1;
                        m_nViewUserNo = -1;
                        AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
                    }

			        return;
                }
            }
        }
	}


    if ( m_nCurrentMode == GAME_MODE ) {
        // 채팅창 부분 클릭하면 채팅창 입력상태로 만든다.
        if ( MouseIn( 15, 733, 329, 752 ) && !m_IME_GameChat->IsInputState() ) {
            m_SCB_GameChat->EndPage();          // 맨 아래 페이지로 이동
            m_IME_GameChat->InputBegin(true);   // 입력 시작
        }

        // 채팅창 이외의 부분을 클릭하면 입력 불가능 상태로 만든다.
        else if ( !MouseIn( 15, 557, 329, 752 ) && m_IME_GameChat->IsInputState() ) {
            m_IME_GameChat->InputEnd();
            m_IME_GameChat->ClearBuffer();
        }
    }

	
	// '0':따당, '1':체크, '2':쿼터, '3':삥, '4':하프, '5':콜, '6':풀, '7':다이

    /* [*BTN]
	if( m_DDaDDang->status == DOWN ) {
		DTRACE("따당 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '0' );
		g_pClient->UserSelect( '0' );
        g_biTotalBettingMoney += m_biDdaDang;
		return;
	}
    */
	if( m_DDaDDang->status == DOWN ) {
		DTRACE("따당 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '0' );
		g_pClient->UserSelect( '0' );
        g_biTotalBettingMoney += m_biDdaDang;
		return;
	}


	if( m_Check->status == DOWN ) {
		DTRACE("체크 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '1' );
		g_pClient->UserSelect( '1' );
        g_biTotalBettingMoney += m_biCheck;
		return;
	}

	if( m_Quater->status == DOWN ) {
		DTRACE("쿼터 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '2' );
		g_pClient->UserSelect( '2' );
        g_biTotalBettingMoney += m_biQuater;
		return;
	}

	if( m_BBing->status == DOWN ) {
		DTRACE("삥 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '3' );
		g_pClient->UserSelect( '3' );
        g_biTotalBettingMoney +=  m_biBing;
		return;
	}
	
	if( m_Half->status == DOWN ) {
		DTRACE("하프 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '4' );
		g_pClient->UserSelect( '4' );
        g_biTotalBettingMoney += m_biHalf;
		return;
	}

	if( m_Call->status == DOWN ) {
		DTRACE("콜 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '5' );
		g_pClient->UserSelect( '5' );
        g_biTotalBettingMoney += m_biCall;
		return;
	}

    /* [*BTN]
	if( m_Full->status == DOWN ) {
		DTRACE("풀 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '6' );
		g_pClient->UserSelect( '6' );
        g_biTotalBettingMoney += m_biFull;
		return;
	}
    */
	if( m_Full->status == DOWN ) {
		DTRACE("풀 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '6' );
		g_pClient->UserSelect( '6' );
        g_biTotalBettingMoney += m_biFull;
		return;
	}
	if( m_Die->status == DOWN ) {
		DTRACE("다이 눌렸다.");
		AllCallBtnUp();

		UserSelectView( 0, '7' );
		g_pClient->UserSelect( '7' );
		return;
	}
}



// 초대할때 체크된 번호를 리턴한다. 리턴할 것이 없으면 마이너스( -1 ) 값이 나온다.

int CMainFrame::GetInviteCheck( void )
{
    int nUserCnt = m_SCB_Invite->GetTotalElem();

	for( int i = 0; i < nUserCnt; i++ ) {
		if( m_sInviteCheck[i].bCheck == TRUE ) {
			return i;
		}
	}

	return -1;
}



void CMainFrame::DrawInviteUsers( int InviteBox_XPos , int InviteBox_YPos )
{
	int nCheckNo;
    int nViewCnt;       // 한 화면에 출력할 항목수 계산에 사용하는 카운터
    int nBaseY;         // 한줄한줄 출력시 사용하는 기준 Y 좌표
    int nIdx;           // 유저 인덱스
	char szChatCopy[255];

    nBaseY = InviteBox_YPos + 84;           // 첫줄의 기준좌표 설정
    nIdx = m_SCB_Invite->GetStartElem();    // 현재 보여지는 시작 유저 인덱스 설정
    nViewCnt = 0;

	for( int i = 0; i < MAX_VIEW_CHODAE_USER; i++ ) {
		if( g_sGameWaitInfo[nIdx].nUserNo > 0 ) {      // 데이타를 그려줄지 결정한다.

            //------------------------------------
            // 선택한 유저의 표식 이미지를 그린다.
            //------------------------------------
            
            if( m_sInviteCheck[nIdx].bCheck == TRUE ) {
                PutSprite( m_SPR_CheckBall, InviteBox_XPos + 29, nBaseY + 5, 1 );
		    }

            //------------
            // 성별 아이콘
            //------------

			if( g_sGameWaitInfo[nIdx].cSex == '1' ) {  // 1 이면 남자이므로 남자 아이콘을 그린다.
                PutSprite( m_SRP_SexIcon, InviteBox_XPos + 189 , nBaseY + 3, 0 );
			}
			else {                                  // 0 이니까 여자 아이콘을 그린다.
                PutSprite( m_SRP_SexIcon, InviteBox_XPos + 189 , nBaseY + 3, 1 );
			}

			nCheckNo = GetInviteCheck();            // 체크된 번호를 받는다.

            //-------------------
            // 닉네임 & 유저 머니
            //-------------------
			
			if( strlen( g_sGameWaitInfo[nIdx].szNick_Name ) > 14 ) {
				memset( &szChatCopy, 0x00, sizeof( szChatCopy ));
				memcpy(  szChatCopy, g_sGameWaitInfo[nIdx].szNick_Name, 14 );
				strcat(  szChatCopy, "..." );
			}
			else {
				strcpy( szChatCopy, g_sGameWaitInfo[nIdx].szNick_Name );
			}

			if( nIdx == nCheckNo ) {                   // 이놈이 체크된 번호면 다른 색으로 그린다.
                // 닉네임
                DrawText( szChatCopy, InviteBox_XPos + 48 , nBaseY + 7, RGB( 140, 215, 69 ), FW_BOLD, TA_LEFT, 13 );

                // 유저 머니
                I64toA_Money( g_sGameWaitInfo[nIdx].biUserMoney, szChatCopy, 1 );
                DrawText( szChatCopy, InviteBox_XPos + 277, nBaseY + 7, RGB( 140, 215, 69 ), FW_BOLD, TA_RIGHT );
			}
            else {                                  // 그렇지 않으면 그냥 보통 색으로 찍는다.
                // 닉네임
                DrawText( szChatCopy, InviteBox_XPos + 48 , nBaseY + 7, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT, 13 );

                // 유저 머니
                I64toA_Money( g_sGameWaitInfo[nIdx].biUserMoney, szChatCopy, 1 );
                DrawText( szChatCopy, InviteBox_XPos + 277, nBaseY + 7, RGB( 255, 255, 255 ), FW_NORMAL, TA_RIGHT );
			}

            nBaseY += 27;   // 출력용 기준 좌표를 다음 줄로 이동
            nIdx++;         // 다음 유저 선택
            
            nViewCnt++;
            if ( nViewCnt >= MAX_VIEW_CHODAE_USER ) {
                break;
            }
		}
	}
}



//========================================================================
// 초대하기 창을 그린다.
//========================================================================

void CMainFrame::RenderInviteBox()
{
    PutSprite( m_SPR_InviteBox, CHODAE_POS_X, CHODAE_POS_Y );
	
	DrawInviteUsers( CHODAE_POS_X, CHODAE_POS_Y );

    m_SCB_Invite->Draw();
	m_InviteOKBTN->DrawButton();
    m_InviteCancelBTN->DrawButton();
}



void CMainFrame::InputInviteChecks( int X , int Y , int SizeX , int SizeY )
{
	int nTempY;
    int nLine = 0;  // 초대창의 줄 번호(첫번째줄은 0)
    int nUserCnt = m_SCB_Invite->GetTotalElem();

	for( int i = m_SCB_Invite->GetStartElem(); i < nUserCnt; i++ )  {
		nTempY = Y + ( nLine * SizeY );

        // 마우스 커서가 nLine번째 줄을 클릭한 경우
		if( MouseIn( X, nTempY, X + SizeX - 1, nTempY + SizeY - 1 )) {
			if( g_sGameWaitInfo[i].nUserNo > 0 ) {
                // 현재 클릭한 위치를 제외하고 모두 FALSE로 만든다.
				for( int j = 0; j < MAX_GAMEVIEWWAIT; j++ ) {
                    if( i != j ) {
                        m_sInviteCheck[j].bCheck = FALSE;
                    }
				}

                if( m_bUserState ) {
                    m_bUserState = FALSE;
                }

                // 클릭한 위치의 상태를 반전시킨다.
				m_sInviteCheck[i].bCheck = !m_sInviteCheck[i].bCheck;
			}		
		}
		else {
            if( m_bUserState ) {
                m_bUserState = FALSE;
            }
		}

        nLine++;
        if ( nLine >= MAX_VIEW_CHODAE_USER ) {
            break;
        }
	}
}



//========================================================================
// 초대창 유저 선택상태를 초기화한다.
//========================================================================

void CMainFrame::InitInviteChecks( void )
{
    // 초대창 좌상단을 기준으로한 상대적 위치

    for( int i = 0 ; i < MAX_GAMEVIEWWAIT; i++ ) {
		m_sInviteCheck[i].bCheck = FALSE;
	}
}



//========================================================================
// 시작 버튼 클릭시
//========================================================================

void CMainFrame::OnBtnStart()
{	
	m_nStartBtnCnt = 0;
	m_bShowStartBTN = FALSE;
	g_pClient->GameStart();
}



void CMainFrame::ServerSrandom(unsigned long initial_seed)
{
	seed = initial_seed; 
}



BOOL CMainFrame::MouseIn( int X , int Y , int X2 , int Y2 )
{
    if( m_mX >= X && m_mY >= Y && m_mX <= X2 && m_mY <= Y2 ) {
        return TRUE;
    }

	return FALSE;		
}



unsigned long CMainFrame::ServerRandom(void)
{
	int lo, hi, test;

	hi = seed/q;
	lo = seed%q;

	test = a*lo - r*hi;

    if (test > 0) {
        seed = test;
    }
    else {
        seed = test+ m;
    }

	return (seed);
}



int CMainFrame::GetRandom(int from, int to)
{
	if (from > to) {
		int tmp = from;
		from = to;
		to = tmp;
	}

	return ((ServerRandom() % (to - from + 1)) + from);
}



void CMainFrame::GetOut()
{
	if( m_nCurrentMode == GAME_MODE ) {     //나간다.
        if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt <= 0 ) {
            g_pClient->RoomOut();
        }
	}
}



void CMainFrame::OnExitBtnDown()
{
	char szTempStr2[255];

	if( m_nCurrentMode == GAME_MODE) {      //나간다.
        if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt <= 0 ) {
            g_pClient->RoomOut();
        }
		else {
			g_CGameUserList.m_GameUserInfo[0].m_bExitReserve = !g_CGameUserList.m_GameUserInfo[0].m_bExitReserve;

			if ( g_CGameUserList.m_GameUserInfo[0].m_bExitReserve ) {
				sprintf( szTempStr2, "◎ %s님 나가기 예약", g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );					
				SetGameChatText( szTempStr2, RGB( 255, 255, 255 ));
				g_pClient->GameChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, szTempStr2 , 0 );
			}
			else {
				sprintf( szTempStr2, "◎ %s님 나가기 예약취소", g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );					
                SetGameChatText( szTempStr2, RGB( 255, 255, 255 ));
				g_pClient->GameChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, szTempStr2 , 0 );
			}
		}
	}
	else {
        DestroyAllSound();
        ExitGame();
	}
}



void CMainFrame::OnLButtonDown()
{
	ButtonDownCheck(m_mX, m_mY);

	m_cX = m_mX;
	m_cY = m_mY;

	//유저가 선택한 돈을 저장.
	if ( m_bShowMakeRoomDlg && !m_bShowRoomTitleList) {
        // 룸머니 선택
		if      ( MouseIn( 374, 357, 507, 377 ) ) m_biMakeRoomMoneyNo = 0;
	//	else if ( MouseIn( 514, 357, 647, 377 ) ) m_biMakeRoomMoneyNo = 1;
		else if ( MouseIn( 374, 378, 507, 398 ) ) m_biMakeRoomMoneyNo = 2;
	//	else if ( MouseIn( 514, 378, 647, 398 ) ) m_biMakeRoomMoneyNo = 3;
        else if ( MouseIn( 374, 399, 507, 419 ) ) m_biMakeRoomMoneyNo = 4;
//		else if ( MouseIn( 514, 399, 647, 419 ) ) m_biMakeRoomMoneyNo = 5;
        else if ( MouseIn( 374, 420, 507, 440 ) ) m_biMakeRoomMoneyNo = 6;
//		else if ( MouseIn( 514, 420, 647, 440 ) ) m_biMakeRoomMoneyNo = 7;
/*
        // 패옵션 선택
        if ( MouseIn( 449, 461, 539, 485 ) ) {
            m_nCardOption = CO_NORMAL_MODE;     // 일반 모드
        }
        else if ( MouseIn( 543, 461, 633, 485 ) ) {
            m_nCardOption = CO_CHOICE_MODE;     // 초이스 모드
        }
		*/
		m_nCardOption = CO_CHOICE_MODE; 
	}

	if( m_bShowChodaeBox && !m_bChodaeDlg && !m_bShowMessageBox ) {
		if( MouseIn( m_nInviteXPos , m_nInviteYPos , m_nInviteXPos + 290 , m_nInviteYPos + 21 ) ) {
			if( m_nCurrentMode == WAITROOM_MODE ) {
				m_bChodaeDlg = TRUE;
			}
		}
	}

	m_nInviteTempX = m_SPR_Invitation->GetPosX();
	m_nInviteTempY = m_SPR_Invitation->GetPosY();

    if ( m_nCurrentMode == GAME_MODE ) {

#ifndef SHOWUSERCARD
        //막판 히든 보기.
	    if ( g_CGameUserList.m_GameUserInfo[0].m_bDie == FALSE &&
             g_CGameUserList.m_GameUserInfo[0].m_nCardCnt >= 7 &&
             g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].nCardNo >= MAX_CARD &&
             m_bEnddingShow == FALSE )
        {
		    if( MouseIn( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.x,
                         g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y,
                         g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.x + CARD_WIDTH,
                         g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y + CARD_HEIGHT) )
            {
                m_sHiddenCardInfo.nCardNo = 255;
                m_sHiddenCardInfo.Pos.x = g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.x;
                m_sHiddenCardInfo.Pos.y = g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y;
                m_sHiddenCardInfo.nOffsetY = m_mY - (int)( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y );
                m_bMoveHiddenCard = TRUE;

			    g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].nCardNo = g_CGameUserList.m_GameUserInfo[0].m_byUserRealCardNo[6];
			    memcpy( g_CGameUserList.m_GameUserInfo[0].m_szJokboCard, g_CGameUserList.m_GameUserInfo[0].m_szTempJokboCard, sizeof(g_CGameUserList.m_GameUserInfo[0].m_szTempJokboCard) );
		    }
	    }
#endif

    }
}



//========================================================================
// 초대를 받았을 때의 처리
//========================================================================

void CMainFrame::RecieveInvite( int nRoomNo , BigInt biRoomMoney, char *pRoomPass , char *pInviteMsg )
{
	if( m_bShowMakeRoomDlg ) {
		m_bShowMakeRoomDlg = FALSE;

        m_IME_InputPass->InputEnd();
        m_IME_InputRoomTitle->InputEnd();
	}

    if ( m_bPassDlg ) {
        m_bPassDlg = FALSE;
        m_IME_RoomInPassBox->InputEnd();
    }

    // 채팅창 입력을 끝낸다.
    if ( m_nCurrentMode == WAITROOM_MODE ) {
        m_IME_WaitChat->InputEnd();
    }
    else if ( m_nCurrentMode == GAME_MODE ) {
        m_IME_GameChat->InputEnd();
    }

	PlaySound( g_Sound[GS_POPUP] );
	m_bShowChodaeBox = TRUE;
	
	m_nInviteRoomNo = nRoomNo; // 방번호를 저장한다.
	m_biInviteRoomMoney = biRoomMoney;
	
	// 방 비밀번호를 저장한다.
	memset( m_szInviteRoomPass , 0x00 ,sizeof( m_szInviteRoomPass ));
	strcpy( m_szInviteRoomPass , pRoomPass );
	
	// 보낸 메세지를 저장해둔다.
	memset( m_szInviteMsg , 0x00 ,sizeof( m_szInviteMsg ));
	strcpy( m_szInviteMsg , pInviteMsg );

}

void CMainFrame::ButtonDownCheck(int mX, int mY) // 버튼이 눌려졌는지 확인
{
    if( m_bFullScreen ) {
        m_MaxBTN2->CheckMouseOver(mX, mY, TRUE);
    }
    else {
        m_MaxBTN->CheckMouseOver(mX, mY, TRUE);
    }

	m_MinBTN->CheckMouseOver(mX, mY, TRUE);

	if( m_Message->GetbShowCheck() ) {
		m_MessageOKBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}

	if( m_bShowChodaeBox ) {
		m_ChodaeYesBTN->CheckMouseOver( mX , mY , TRUE );
		m_ChodaeNoBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}

	if( m_bShowConfigBox ) {
		m_OkBTN->CheckMouseOver(mX, mY, TRUE);
		return;
	}

	if( m_bShowMakeRoomDlg ) {
        m_RoomTitleBTN->CheckMouseOver(mX, mY, TRUE);
		m_MakeRoomOkBTN->CheckMouseOver(mX, mY, TRUE);
		m_MakeRoomCancelBTN->CheckMouseOver(mX, mY, TRUE);
		return;
	}

    if( m_bPassDlg) {
        m_OkBTN->CheckMouseOver(mX, mY, TRUE);
		m_CancelBTN->CheckMouseOver(mX, mY, TRUE);
		return;
    }

	if( m_bShowInviteBox ) {
        m_SCB_Invite->BTN_MouseDown( mX, mY );
		m_InviteOKBTN->CheckMouseOver( mX, mY, TRUE );
        m_InviteCancelBTN->CheckMouseOver( mX, mY, TRUE );
		m_UserStateBTN->CheckMouseOver( mX, mY, TRUE );
		return;
	}

	if( m_bShowMessageBox ) {
		m_MemoSendBTN->CheckMouseOver( mX , mY , TRUE );
		m_MemoCancelBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}

	m_Exit2BTN->CheckMouseOver( mX , mY , TRUE );
	m_ExitBTN->CheckMouseOver(mX, mY, TRUE);

	switch( m_nCurrentMode )
	{
		case WAITROOM_MODE: // 대기실이면

			m_ConfigBTN->CheckMouseOver( mX , mY , TRUE ); // 설정 버튼
			m_MakeRoomBTN->CheckMouseOver( mX , mY , TRUE );
			m_JoinBTN->CheckMouseOver( mX , mY , TRUE );
			m_QuickJoinBTN->CheckMouseOver( mX , mY , TRUE );
            m_CaptureBTN->CheckMouseOver( mX , mY , TRUE );
			m_UserStateBTN->CheckMouseOver( mX , mY , TRUE );

            m_SCB_WaitChat->BTN_MouseDown( mX, mY );        // 채팅창 스크롤 바
            m_SCB_WaitRoom->BTN_MouseDown( mX, mY );

            if ( m_nUserListTitleNum == 0 ) {
                m_SCB_WaitUser->BTN_MouseDown( mX, mY );        // 대기자 리스트 스크롤 바
            }

			break;

		case GAME_MODE: // 게임화면이면

			m_ConfigBTN->CheckMouseOver( mX , mY , TRUE ); // 설정 버튼
			m_FullBTN->CheckMouseOver( mX , mY , TRUE );
			m_UserCloseBTN->CheckMouseOver( mX , mY , TRUE );
            m_CaptureBTN->CheckMouseOver( mX , mY , TRUE );

			if( g_sRoomInfo.nCurCnt < MAX_ROOM_IN )
				m_InviteBTN->CheckMouseOver( mX, mY, TRUE );

            /* [*BTN]
			if ( m_DDaDDang->status != DISABLED )
                m_DDaDDang->CheckMouseOver( mX , mY , TRUE );
            */
			if ( m_DDaDDang->status != DISABLED )
                m_DDaDDang->CheckMouseOver( mX , mY , TRUE );


            if ( m_Quater->status != DISABLED )
			    m_Quater->CheckMouseOver( mX , mY , TRUE );

            if ( m_Half->status != DISABLED )
			    m_Half->CheckMouseOver( mX , mY , TRUE );

            /* [*BTN]
            if ( m_Full->status != DISABLED )
			    m_Full->CheckMouseOver( mX , mY , TRUE );
            */

            if ( m_Full->status != DISABLED )
			    m_Full->CheckMouseOver( mX , mY , TRUE );

            if ( m_Check->status != DISABLED )
			    m_Check->CheckMouseOver( mX , mY , TRUE );

            if ( m_BBing->status != DISABLED )
			    m_BBing->CheckMouseOver( mX , mY , TRUE );

            if ( m_Call->status != DISABLED )
			    m_Call->CheckMouseOver( mX , mY , TRUE );

            if ( m_Die->status != DISABLED )
			    m_Die->CheckMouseOver( mX , mY , TRUE );

            if( m_bShowStartBTN ) {
                m_StartBTN->CheckMouseOver(mX, mY, TRUE);
            }

            m_SCB_GameChat->BTN_MouseDown( mX, mY );

			break;
	}
}



void CMainFrame::OnLButtonUp()
{
	ProcessButtonEvent();
	ButtonUpCheck();

	m_bChodaeDlg = FALSE;
	
}



void CMainFrame::ButtonUpCheck()
{
	m_MaxBTN2->SetMouseLBDownStatus(FALSE);
	m_MaxBTN->SetMouseLBDownStatus(FALSE);
	m_MinBTN->SetMouseLBDownStatus(FALSE);	

	if( m_bShowMakeRoomDlg ) {
		//버튼 상태 처리
        m_RoomTitleBTN->SetMouseLBDownStatus(FALSE);
		m_MakeRoomOkBTN->SetMouseLBDownStatus(FALSE);
		m_MakeRoomCancelBTN->SetMouseLBDownStatus(FALSE);
		return;
	}

	if( m_bShowChodaeBox ) {
		m_ChodaeNoBTN->SetMouseLBDownStatus( FALSE );
		m_ChodaeYesBTN->SetMouseLBDownStatus( FALSE );
		return;
	}

	if( m_Message->GetbShowCheck() ) {
		m_MessageOKBTN->SetMouseLBDownStatus( FALSE );
		return;
	}
	
	if( m_bShowConfigBox ) {
		m_OkBTN->SetMouseLBDownStatus(FALSE);
		return;
	}

    if( m_bPassDlg ) {
        m_OkBTN->SetMouseLBDownStatus(FALSE);
        m_CancelBTN->SetMouseLBDownStatus(FALSE);
        return;
    }
	
	if( m_bShowInviteBox ) {
        m_SCB_Invite->BTN_MouseUp();
		m_InviteOKBTN->SetMouseLBDownStatus( FALSE );	
        m_InviteCancelBTN->SetMouseLBDownStatus( FALSE );
		m_UserStateBTN->SetMouseLBDownStatus( FALSE );
		return;
	}
	
	if( m_bShowMessageBox ) {
		m_MemoSendBTN->SetMouseLBDownStatus( FALSE );
		m_MemoCancelBTN->SetMouseLBDownStatus( FALSE );
		return;
	}
	
	m_Exit2BTN->SetMouseLBDownStatus(FALSE);
	m_ExitBTN->SetMouseLBDownStatus(FALSE);

	switch(m_nCurrentMode)
	{
		case WAITROOM_MODE: // 대기실이면
			m_MakeRoomBTN->SetMouseLBDownStatus(FALSE);
			m_JoinBTN->SetMouseLBDownStatus(FALSE);
			m_QuickJoinBTN->SetMouseLBDownStatus( FALSE );
			m_UserStateBTN->SetMouseLBDownStatus( FALSE );	
            m_CaptureBTN->SetMouseLBDownStatus(FALSE);

            m_SCB_WaitChat->BTN_MouseUp();          // 채팅창 스크롤 바
            m_SCB_WaitRoom->BTN_MouseUp();

            if ( m_nUserListTitleNum == 0 ) {
                m_SCB_WaitUser->BTN_MouseUp();      // 대기자 시스트 스크롤 바
            }

			break;

		case GAME_MODE:
			m_ConfigBTN->SetMouseLBDownStatus(FALSE);
			m_FullBTN->SetMouseLBDownStatus(FALSE);
			m_UserCloseBTN->SetMouseLBDownStatus(FALSE);
            m_CaptureBTN->SetMouseLBDownStatus(FALSE);

            if( g_sRoomInfo.nCurCnt < MAX_ROOM_IN )
                m_InviteBTN->SetMouseLBDownStatus(FALSE);

            /* [*BTN]
			if ( m_DDaDDang->status != DISABLED )
                m_DDaDDang->SetMouseLBDownStatus(FALSE);
            */
			if ( m_DDaDDang->status != DISABLED )
                m_DDaDDang->SetMouseLBDownStatus(FALSE);


            if ( m_Quater->status != DISABLED )
			    m_Quater->SetMouseLBDownStatus(FALSE);

            if ( m_Call->status != DISABLED )
			    m_Call->SetMouseLBDownStatus(FALSE);

            if ( m_Half->status != DISABLED )
			    m_Half->SetMouseLBDownStatus(FALSE);

            /* [*BTN]
            if ( m_Full->status != DISABLED )
			    m_Full->SetMouseLBDownStatus(FALSE);
            */
            if ( m_Full->status != DISABLED )
			    m_Full->SetMouseLBDownStatus(FALSE);

            if ( m_Check->status != DISABLED )
			    m_Check->SetMouseLBDownStatus(FALSE);

            if ( m_BBing->status != DISABLED )
			    m_BBing->SetMouseLBDownStatus(FALSE);

            if ( m_Die->status != DISABLED )
			    m_Die->SetMouseLBDownStatus(FALSE);

            if( m_bShowStartBTN ) {
                m_StartBTN->SetMouseLBDownStatus(FALSE);
            }

            m_SCB_GameChat->BTN_MouseUp();

			break;
	}
}



BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) ) {
        return FALSE;
    }

	int xx , yy;
		
	xx = ( GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH ) / 2 ;
	yy = ( GetSystemMetrics(SM_CYSCREEN) - SCREEN_HEIGHT ) / 2 ;

	cs.x = xx;
	cs.y = yy;
	
	cs.cx = SCREEN_WIDTH;
	cs.cy = SCREEN_HEIGHT;

	cs.dwExStyle =  NULL;	
	cs.hMenu = NULL;
	cs.style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
	cs.lpszName = "Games";

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG

void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}



void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	//m_wndView.SetFocus();
}



BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	AvatarDelete();

    if (m_hGameInitEvent) 
        CloseHandle(m_hGameInitEvent);
    
	if ( m_hAniUpdateThread ) {
		m_bAniActive = FALSE;
		WaitForSingleObject( m_hAniUpdateThread, MAX_DESTROY_TIME );
		CloseHandle( m_hAniUpdateThread );
	}	

	if ( m_hRenderThread ) {
		m_bActive = FALSE;
		WaitForSingleObject( m_hRenderThread, MAX_DESTROY_TIME );
		CloseHandle( m_hRenderThread );
	}	

	if ( m_hSocketThread ) {
		g_bPaketThredOn = false;
		WaitForSingleObject( m_hSocketThread, MAX_DESTROY_TIME );
		CloseHandle( m_hSocketThread );

		DTRACE("close - 2");
	}

	KillTimer(1);
	ReleaseData();
    ExitGame();
}



void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//////////////////////////////////////////////////////////////////////////
	// 마우스가 컨트롤 안에 들어오면
	//SetCursor( LoadCursor( GetModuleHandle(0) , (LPCSTR)IDC_CURSOR ));

    if( m_nCurrentMode == WAITROOM_MODE ) {

        /*
        if( MouseIn( 13 - 35, 13 - 35, 682 + 3, 144 + 3) ) {			
			SetGameCursor( FALSE , TRUE , FALSE );
		}
		else {
			SetGameCursor( FALSE , TRUE );			
		}
        */
        
        // 하일라이트바 
        if (MouseIn(713, 231, 949, 530)) {
            m_nHighlightBarNum = (m_mY - 231) / 25;    // 25는 한칸의 세로 길이
        }
        else {
            m_nHighlightBarNum = -1;
        }
	}

	m_mX = point.x;
	m_mY = point.y;
	
	CheckButtons(m_mX, m_mY);

    if ( !g_bConnectingSpr ) {          // '연결중입니다.' 메시지 나오는 동안은 입력 받지 않음
        // 스크롤 바 드래그 처리
        m_SCB_WaitChat->Drag( m_mX, m_mY );
        m_SCB_WaitRoom->Drag( m_mX, m_mY );
        
        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->Drag( m_mX, m_mY );
        }
    }

    // 방 만들기 창에서 방 제목 리스트가 떠있는 경우
    if ( m_bShowRoomTitleList ) {
        if (MouseIn( ROOM_TITLE_LIST_X + 2, 
                     ROOM_TITLE_LIST_Y + 2,
                     ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH - 3,
                     (ROOM_TITLE_LIST_Y + 2) + (ROOM_TITLE_BAR_HEIGHT * 5) - 1))
        {
            m_nSelectedRoomTitleList = (m_mY - (ROOM_TITLE_LIST_Y + 2))  /  ROOM_TITLE_BAR_HEIGHT;
        }
        else {
            m_nSelectedRoomTitleList = -1;
        }
    }

    
    if (m_nCurrentMode == GAME_MODE ) {
        // 히든 카드 이동 처리
        if ( m_bMoveHiddenCard ) {
            // 히든 카드가 기존에 뒤집혀있던 위치보다 위쪽으로 이동하지 못하게 한다.
            if (( m_mY - m_sHiddenCardInfo.nOffsetY ) < g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y ) {
                m_sHiddenCardInfo.Pos.y = g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].Pos.y;
            }
            else {
                m_sHiddenCardInfo.Pos.y = m_mY - m_sHiddenCardInfo.nOffsetY;
            }
        }
    }

	CFrameWnd::OnMouseMove(nFlags, point);
}



//========================================================================
// 비밀방에 들어갈때 패스워드를 입력하는 창을 그린다.
//========================================================================

void CMainFrame::RenderPassDlg()
{
    PutSprite( m_SPR_PassDlg, PASS_DLG_POS_X, PASS_DLG_POS_Y );

    // 입력 IME가 포커스가 있으면 배경을 흰색으로 칠한다.
    if ( m_IME_RoomInPassBox->IsFocus() ) {
        g_pCDXScreen->GetBack()->FillRect(
            PASS_DLG_POS_X + 96,
            PASS_DLG_POS_Y + 73,
            PASS_DLG_POS_X + 96 + 189,
            PASS_DLG_POS_Y + 73 + 16,
            RGB( 255, 255, 255 )
        );
    }
    m_IME_RoomInPassBox->PutIMEText( PASS_DLG_POS_X + 100, PASS_DLG_POS_Y + 76, RGB(0, 0, 0), RGB(0, 0, 0) );
    
	m_OkBTN->SetPos( PASS_DLG_POS_X +  45, PASS_DLG_POS_Y + 115 );
	m_OkBTN->DrawButton();

    m_CancelBTN->SetPos( PASS_DLG_POS_X + 173, PASS_DLG_POS_Y + 115 );
	m_CancelBTN->DrawButton();
}



void CMainFrame::RenderMakeRoom() // 방만들기 창 렌더링
{
    m_SPR_MakeRoom->SetPos( 349, 208 );
	m_SPR_MakeRoom->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
    m_RoomTitleBTN->DrawButton();

	m_MakeRoomOkBTN->DrawButton();
	m_MakeRoomCancelBTN->DrawButton();

    //방 제목을 출력한다.
    if ( m_IME_InputRoomTitle->IsInputState() ) {
        if ( !m_IME_InputRoomTitle->IsFocus() ) {
            g_pCDXScreen->GetBack()->FillRect( 445, 269, 626, 285, m_rgbGray );
        }
        m_IME_InputRoomTitle->PutIMEText( 447, 272, RGB(0, 0, 0), RGB(0, 0, 0) );
    }
	
	//방만들기에서 마우스를 클릭하면 해당 표시에 점을 찍는다.
    PutSprite( m_SPR_CheckBall,
               m_sMakeRoomMoneyPoint[m_biMakeRoomMoneyNo].x,
               m_sMakeRoomMoneyPoint[m_biMakeRoomMoneyNo].y,
               1 );
/*
    // 패옵션에 블릿을 그린다.
    if ( m_nCardOption == CO_NORMAL_MODE ) { 
        PutSprite( m_SPR_CheckBall, 454, 467, 1 );  // 일반 방식
    } else {
        PutSprite( m_SPR_CheckBall, 548, 467, 1 );  // 초이스 방식
    }
*/
//	PutSprite( m_SPR_CheckBall, 548, 467, 1 );

    // 패스워드 체크에 관계없이 포커스가 없으면 회색배경을 칠한다.
    if ( !m_IME_InputPass->IsFocus() ) {
        g_pCDXScreen->GetBack()->FillRect( 445, 294, 621, 310, m_rgbGray );
    }

    // 패스워드 체크 표시
    if ( m_bPassRoomChk ) {
        if ( m_IME_InputPass->IsInputState() ) {
            m_IME_InputPass->PutIMEText( 448, 296, RGB(0, 0, 0), RGB(0, 0, 0) );
        }
        DrawText( "V", 632 , 297, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
    }
    else {
        g_pCDXScreen->GetBack()->FillRect( 628, 295, 642, 309, m_rgbGray );
    }

    if ( m_bShowRoomTitleList ) {
        // 방 제목 리스트의 테두리를 그린다.
        g_pCDXScreen->GetBack()->FillRect(
                ROOM_TITLE_LIST_X,
                ROOM_TITLE_LIST_Y,
                ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH,
                ROOM_TITLE_LIST_Y + ROOM_TITLE_LIST_HEIGHT,
                m_rgbMakeRoomTitleListBorder
        );

        // 방 제목 리스트의 배경을 그린다.
        g_pCDXScreen->GetBack()->FillRect(
                ROOM_TITLE_LIST_X + 1,
                ROOM_TITLE_LIST_Y + 1,
                ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH - 1,
                ROOM_TITLE_LIST_Y + ROOM_TITLE_LIST_HEIGHT - 1,
                RGB( 255, 255, 255 )
        );

        // 선택된 항목 위치에 사각형 바를 그린다.
        if ( m_nSelectedRoomTitleList != -1 ) {
            g_pCDXScreen->GetBack()->FillRect(
                ROOM_TITLE_LIST_X + 2,
                ROOM_TITLE_LIST_Y + 2 + (m_nSelectedRoomTitleList * ROOM_TITLE_BAR_HEIGHT),
                ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH - 2,
                ROOM_TITLE_LIST_Y + 2 + (m_nSelectedRoomTitleList * ROOM_TITLE_BAR_HEIGHT) + ROOM_TITLE_BAR_HEIGHT,
                m_rgbMakeRoomTitleListBar
            );
        }

        for ( int i = 0; i < 5; i++ ) {
            if ( i == m_nSelectedRoomTitleList ) {
                DrawText( g_RoomTitle[i], ROOM_TITLE_LIST_X + 4, ROOM_TITLE_LIST_Y + 4 + (i * ROOM_TITLE_BAR_HEIGHT),
                        RGB(255, 255, 255), FW_NORMAL, TA_LEFT);
            }
            else {
                DrawText( g_RoomTitle[i], ROOM_TITLE_LIST_X + 4, ROOM_TITLE_LIST_Y + 4 + (i * ROOM_TITLE_BAR_HEIGHT),
                        RGB(0, 0, 0), FW_NORMAL, TA_LEFT);
            }
        }
    }
}



void CMainFrame::SetGameCursor( BOOL bUserCursor , BOOL bClick, BOOL bMouseRender )
{
	//return;

	if( bClick ) {
		m_bMouseEvent = TRUE;
	}
	else {
		m_bMouseEvent = FALSE;
	}

	if( bMouseRender ) {
		m_bMouseRender = TRUE;
	}
	else {
		m_bMouseRender = FALSE;
	}

	if ( bUserCursor ) {
		if ( m_nCurSorCnt >= 0 ) return;
	}
    else {
        if ( m_nCurSorCnt < 0 ) return;		
    }
		
	if( bUserCursor ) {
		m_nCurSorCnt = ShowCursor( TRUE );
		
		if( m_nCurSorCnt < 0 ) {
			while( m_nCurSorCnt < 0 ) {
				m_nCurSorCnt = ShowCursor( TRUE );
			};
		}
	}
	else {
		m_nCurSorCnt = ShowCursor( FALSE );

		if( m_nCurSorCnt > 0 ) {
			while( m_nCurSorCnt > 0 ) {
				m_nCurSorCnt = ShowCursor( FALSE );
			};
		}
	}
}



void CMainFrame::CheckButtons(int mX, int mY) // 마우스 커서가 버튼위에 있는지 체크
{
    if ( m_bFullScreen) {
        m_MaxBTN2->CheckMouseOver( mX, mY );
    }
    else {
        m_MaxBTN->CheckMouseOver( mX, mY );
    }

	m_MinBTN->CheckMouseOver( mX, mY );
	
	if( m_Message->GetbShowCheck() ) {
		m_MessageOKBTN->CheckMouseOver( mX, mY );
		return;
	}

	if( m_bShowChodaeBox ) {
		m_ChodaeYesBTN->CheckMouseOver( mX, mY );
		m_ChodaeNoBTN->CheckMouseOver( mX, mY );
		return;
	}

	if( m_bShowConfigBox ) {
		m_OkBTN->CheckMouseOver( mX , mY );
		return;
	}

	if( m_bShowMakeRoomDlg ) {
        m_RoomTitleBTN->CheckMouseOver( mX, mY );
		m_MakeRoomOkBTN->CheckMouseOver( mX, mY );
		m_MakeRoomCancelBTN->CheckMouseOver( mX, mY );
		return;
	}

    if( m_bPassDlg ) {
        m_OkBTN->CheckMouseOver( mX, mY );
		m_CancelBTN->CheckMouseOver( mX, mY );
		return;
    }

	if( m_bShowInviteBox ) {
        m_SCB_Invite->Drag( mX, mY );
        m_SCB_Invite->BTN_MouseOver( mX, mY );
		m_InviteOKBTN->CheckMouseOver( mX, mY );
        m_InviteCancelBTN->CheckMouseOver( mX, mY );
		m_UserStateBTN->CheckMouseOver( mX, mY );
		return;
	}

	if( m_bShowMessageBox ) {
		m_MemoSendBTN->CheckMouseOver( mX , mY );
		m_MemoCancelBTN->CheckMouseOver( mX , mY );
		return;
	}

	m_Exit2BTN->CheckMouseOver( mX , mY );
	m_ExitBTN->CheckMouseOver(mX, mY);
	
	switch( m_nCurrentMode )
	{
		case WAITROOM_MODE: // 대기실이면

			m_ConfigBTN->CheckMouseOver( mX , mY ); // 설정 버튼
			m_MakeRoomBTN->CheckMouseOver( mX , mY );
			m_JoinBTN->CheckMouseOver( mX , mY );
			m_QuickJoinBTN->CheckMouseOver( mX , mY );
			m_UserStateBTN->CheckMouseOver( mX , mY );
            m_CaptureBTN->CheckMouseOver( mX , mY );

            m_SCB_WaitChat->BTN_MouseOver( mX, mY );          // 채팅창 스크롤 바
            m_SCB_WaitRoom->BTN_MouseOver( mX, mY );

            if ( m_nUserListTitleNum == 0) {
                m_SCB_WaitUser->BTN_MouseOver( mX, mY );          // 대기자 리스트 스크롤 바
            }
			
			break;

		case GAME_MODE: // 게임화면이면

			m_ConfigBTN->CheckMouseOver( mX , mY ); // 설정 버튼
			m_FullBTN->CheckMouseOver( mX , mY );
			m_UserCloseBTN->CheckMouseOver( mX, mY );
            m_CaptureBTN->CheckMouseOver( mX , mY );
            m_SCB_GameChat->Drag( mX, mY );
            m_SCB_GameChat->BTN_MouseOver( mX, mY );

            if( g_sRoomInfo.nCurCnt >= MAX_ROOM_IN ) {
                m_InviteBTN->DisableButton();
            }
            else {
                m_InviteBTN->CheckMouseOver( mX , mY );		
            }

            if( m_bShowStartBTN ) {
                m_StartBTN->CheckMouseOver(mX, mY);
            }

			break;
	}
}



void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
	
}



void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	// TODO: Add your message handler code here
	
}



HBRUSH CMainFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CFrameWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	
    pDC->SetTextColor(RGB(0, 0, 0));            //글자 색상
	pDC->SetBkColor( m_ColControl[0] );
	return m_Brush[0];
}



UINT CMainFrame::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    UINT nHit = CFrameWnd::OnNcHitTest(point);

    ScreenToClient(&point);

	m_mX = point.x;
	m_mY = point.y;

    if( !m_bMoveDlg && !m_bFullScreen && nHit == HTCLIENT ) {   // 마우스를 움직일때 윈도우가 움직이게 할 영역
        if ( (m_nCurrentMode == WAITROOM_MODE && point.x <= 800 && point.y <= 45)  ||
             (m_nCurrentMode == GAME_MODE     && point.x <= 950 && point.y <= 26) )
        {
            nHit = HTCAPTION;
        }
    }

    return nHit;
	
	//return CFrameWnd::OnNcHitTest(point);
}



void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( m_nCurrentMode == WAITROOM_MODE ) {
        // 창이 떠있을 때 더블클릭하면 무시한다.s
        if (  m_bShowMakeRoomDlg 
           || m_bShowChodaeBox 
           || m_bShowInviteBox 
           || m_bShowConfigBox 
           || m_bShowMessageBox
           || m_Message->GetbShowCheck())
        {
            return;
        }

        int nSelRoomIdx = -1;           // 더블클릭한 방의 인덱스(리스트의 맨 처음에서 부터 몇번째인지를 나타냄)
        int nSelRoomNo = 0;             // 더블클릭한 방의 번호(실제 방의 번호)
		int nTempViewRoomMessage = -1;
		BOOL bRoomClick = false;

        // 대기실의 방 더블클릭
        if (MouseIn(35, 206, 644, 502 - 2)) {
            nSelRoomIdx = (m_mY + 1 - 206)  /  ((502 + 1 - 206) / MAX_VIEW_WAIT_ROOM);
            nSelRoomIdx += m_SCB_WaitRoom->GetStartElem();

            if ( nSelRoomIdx >= 0 && nSelRoomIdx < g_nWaitRoomCnt ) {   // 존재하는 방의 범위 내에서 클릭한 경우
                nSelRoomNo = g_nWaitRoomIdx[ nSelRoomIdx ];             // 방의 실제 번호 저장
                nTempViewRoomMessage = nSelRoomNo;
                bRoomClick = true;
            }
        }
		
		if ( bRoomClick ) {
			if( nTempViewRoomMessage >= 0 ) {
                if ( g_CWaitRoomList.m_sWaitRoomList[nTempViewRoomMessage].nCurCnt <= 0 ) {
                    return;
                }

				if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1  ) {
					if ( g_CWaitRoomList.m_sWaitRoomList[nTempViewRoomMessage].nCurCnt >= MAX_ROOM_IN ) {
						m_Message->PutMessage( "방에 인원이 모두 찼습니다.", "Code - 126" );
						return;
					}

					//자신이 들어갈수 있는 방인지를 조사한다.
					if ( AvailableRoomIn( g_CWaitRoomList.m_sWaitRoomList[nTempViewRoomMessage].biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
                        if ( g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].bSecret ) {
                            m_IME_RoomInPassBox->ClearBuffer();
                            m_IME_RoomInPassBox->InputBegin(true);
                            m_bPassDlg = TRUE;
                        }
                        else {
                            sRoomIn l_sRoomInData;                  // 접속할 방의 구조체
						    memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

						    l_sRoomInData.nRoomNo = g_CWaitRoomList.m_sWaitRoomList[nTempViewRoomMessage].nRoomNo;
						    memset( l_sRoomInData.szRoomPass , 0 , sizeof(l_sRoomInData.szRoomPass));

						    m_bUserState = FALSE;
						    g_pClient->RoomIn( &l_sRoomInData );    // 방으로 들어간다.
                        }
					}
					else {
						m_Message->PutMessage( "유저의 보유머니로 해당방을", "입장하실수 없습니다.!!!", false );
					}
				}
				else {
					m_Message->PutMessage( "게임을 종료하고 충전후 이용하세요!!!", "Code - 133" );
				}
			}
			else {
				m_Message->PutMessage( "입장 할 방을 선택해주세요" , "Code - 127" );
			}	
		}

		m_JoinBTN->SetMouseLBDownStatus(FALSE);
	}
	
	CFrameWnd::OnLButtonDblClk(nFlags, point);	
}



BOOL CMainFrame::SendChatTime( int nTime )
{
	int nTempTime;
	nTempTime = nTime - m_nChatTime;

	if( DEF_TIME < nTempTime ) {
		m_nChatTime = nTime;
		return TRUE;
	}

	return FALSE;
}



void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bRButtonDown = TRUE;

    if( m_nCurrentMode == WAITROOM_MODE ) {
        RButtonDownCheck( m_mX , m_mY );
    }

    if( m_nCurrentMode == GAME_MODE ) {
        RButtonDownCheckInGame( m_mX , m_mY );
    }
	
	CFrameWnd::OnRButtonDown(nFlags, point);	
}



void CMainFrame::RButtonDownCheck( int MouseX , int MouseY )
{
	return;

    if( m_bShowMessageBox ) {
        return;
    }

    if ( m_nHighlightBarNum != -1) {
        // 대기자 리스트의 유저 클릭한 경우
        if ( m_nUserListTitleNum == 0 ) {
            m_nUserStatus[ m_nHighlightBarNum + m_SCB_WaitUser->GetStartElem() ] = DOWN;
            m_nRButtonWaitUserNo = m_nHighlightBarNum + m_SCB_WaitUser->GetStartElem();
        }

        // 방 유저 리스트의 유저 클릭한 경우
        else if ( m_nUserListTitleNum == 1 ) {
            m_nUserStatus[ m_nHighlightBarNum ] = DOWN;
            m_nRButtonWaitUserNo = m_nHighlightBarNum;
        }
    }
    else {
        m_bUserState = FALSE;
    }
}



void CMainFrame::RButtonDownCheckInGame( int MouseX , int MouseY )
{
}



void CMainFrame::OnRButtonUp(UINT nFlags, CPoint point) 
{
    ::SetFocus( g_hWnd );       // CHttpView로 인해 키 입력 안되는 문제 방지

	if( m_nCurrentMode == WAITROOM_MODE ) {
		RButtonEvent();
		RButtonUpCheck();
	}

	CFrameWnd::OnRButtonUp(nFlags, point);
}



void CMainFrame::RButtonEvent( void )
{
	return;


    if( m_bShowMessageBox || m_nHighlightBarNum == -1 ) {
        return;
    }
    
    if ( m_nUserListTitleNum == 0 ) {
        int nTempNo = m_nHighlightBarNum + m_SCB_WaitUser->GetStartElem();

        if ( nTempNo < MAX_CHANNELPER && g_CUserList.m_WaitUserInfo[nTempNo].m_bUser ) {

            if( m_nUserStatus[m_nRButtonWaitUserNo] == DOWN ) {
				strcpy( m_szMessageRecieveUserNick, g_CUserList.m_WaitUserInfo[ nTempNo ].m_sUserInfo.szNick_Name );
				//이 메세지는 보내는 것이다.
				m_bMessageSend = TRUE;
				m_bUserState = TRUE;
				m_TempMX = m_mX;
				m_TempMY = m_mY;
			}
        }
        else {
            m_bUserState = FALSE;
        }
    }
    else if ( m_nUserListTitleNum == 1 ) {
         if ( m_nSelectedRoomNum != -1 ) {
            // 방에 사람이 있고, 사람이 있는 위치를 선택했을 때만 정보를 보여준다.
			if ( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt > 0 ) {
				if ( m_nHighlightBarNum >= 0 && m_nHighlightBarNum < g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt ) {
                    if( m_nUserStatus[m_nRButtonWaitUserNo] == DOWN ) {
				        // 받는 유저의 이름을 미리 저장해둔다.
				        strcpy( m_szMessageRecieveUserNick, g_ClickRoomUserInfo[ m_nHighlightBarNum ].szNick_Name );
				        m_bMessageSend = TRUE;  //이 메세지는 보내는 것이다.
				        m_bUserState = TRUE;
				        m_TempMX = m_mX;
				        m_TempMY = m_mY;
			        }
                }
                else {
                    m_bUserState = FALSE;
                }
            }
        }
    }
}



void CMainFrame::RButtonUpCheck( void )
{
	return;

    if( m_bShowMessageBox || m_nHighlightBarNum == -1 ) {
		return;
    }

    if ( m_nUserListTitleNum == 0 ) {
        m_nUserStatus[ m_nHighlightBarNum + m_SCB_WaitUser->GetStartElem() ] = UP;
    }
    else if ( m_nUserListTitleNum == 1 ) {
        m_nUserStatus[ m_nHighlightBarNum ] = UP;
    }
}



void CMainFrame::AvatarDelete()
{
	if ( m_WaitAvaImg != NULL ) {  
		m_WaitAvaImg->Destroy();
		SAFE_DELETE( m_WaitAvaImg );
	}

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_GameAvaImg[i] != NULL ) {
			m_GameAvaImg[i]->Destroy();
			SAFE_DELETE( m_GameAvaImg[i] );
		}
	}
}



void CMainFrame::AvatarPlay( int nSlot, char *pAvaUrl )
{
	char szAvaFileName[255];
	char szPathAvaFileName[255];
	CString csAvaName;

	memset( szAvaFileName, 0x00, 255 );
	memset( szPathAvaFileName, 0x00, 255 );

	strcpy( szPathAvaFileName, ".\\Avatar\\" );
	strcat( szPathAvaFileName, pAvaUrl );
	
	switch( nSlot ) 
	{
		case -1:
			SAFE_DELETE(m_WaitAvaImg);
			csAvaName = szPathAvaFileName;
 			m_WaitAvaImg = new ImageEx( csAvaName.AllocSysString(), m_SPR_WaitAvatar ); 
			m_WaitAvaImg->InitAnimation( g_hWnd, CPoint(0,0) );
			
			break;

		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			SAFE_DELETE(m_GameAvaImg[nSlot]);
			csAvaName = szPathAvaFileName;
 			m_GameAvaImg[nSlot] = new ImageEx( csAvaName.AllocSysString(), m_SPR_GameAvatar[nSlot] ); 
			m_GameAvaImg[nSlot]->InitAnimation( g_hWnd, CPoint(0,0) );
			
			break;
	}
}



//////////////////////////////////////////////////////////////////////////
// __int64 타입의 게임머니를 문자열로 변환한다.
//
// input:   *szBuff = 변환된 문자열이 저장될 버퍼
//          nMoney = 문자열로 변환할 게임머니
//          nUnitCnt = 출력할 단위의 갯수
//                      0 이하인 경우 : 모든 자릿수 출력 (기본값)
//                      1 이상 : 지정된 숫자만큼의 단위 출력(예: 2로
//                               지정한 경우 최상위 2단위만 출력)
//////////////////////////////////////////////////////////////////////////
void CMainFrame::I64toA_Money(BigInt biMoney, char *szBuff, int nUnitCnt )
{

    static BigInt nTemp = 0;			// 출력할 숫자를 저장하기 위한 임시 변수
    static int nUnits = 0;              // 단위 수(게임머니를 짧게 표시하는 경우 두 단위만 출력하기 위해서 사용)
    static int nIdx = 0;                // 배열 인덱스 & 숫자의 총 자릿수
    static int nNumArray[10];           // 각 단위의 숫자가 들어갈 배열(배열의 한 칸은 각 단위의 숫자(예: 5643)가 들어간다)
    static char *szUnits[5] = { "", "만", "억", "조", "경" }; // 단위
    static char szTemp[255];

    nIdx = 0;
    nUnits = 0;
    nTemp = biMoney;

    do {
        nNumArray[nIdx++] = (int)(nTemp % 10000);
        nTemp /= 10000;
    } while(nTemp > 0);

    strcpy(szBuff, "");

    // 만 자리 미만이면 무조건 숫자 표시(값이 0인 경우에도)
    if (nIdx == 1) {
        _itoa(nNumArray[0], szTemp, 10);
        strcat(szBuff, szTemp);
    }

    // 만 자리 이상이면
    else {
        for ( int i = nIdx - 1; i >= 0; i-- ) {
            // 값이 0인 단위들은 출력하지 않는다.
            if (nNumArray[i] == 0) continue;

            // 최상위 자리를 제외한 나머지 숫자들은 앞에 공백을 붙인다.
            if (i < nIdx - 1) strcat(szBuff, " ");

            // 해당 단위의 숫자를 문자열로 변환해서 저장
            _itoa(nNumArray[i], szTemp, 10);
            strcat(szBuff, szTemp);
            strcat(szBuff, szUnits[i]);

            if ( nUnitCnt > 0 ) {   // 특정 갯수만큼의 단위만 출력하기 위해서 지정한 경우( 0 이하면 모든 단위 출력)
                nUnits++;
                if ( nUnits >= nUnitCnt ) {
                    break;
                }
            }
        }
    }

    strcat(szBuff, "톨");

}



LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
		case WM_ASYNC:
			switch(WSAGETSELECTEVENT(lParam))
			{		
				//소켓 종료 메시지
				case FD_CONNECT:
					g_pClient->OnConnect( WSAGETSELECTERROR(lParam) );
					break;

				case FD_CLOSE:
					g_pClient->Destroy();
					Sleep(1000);	//이미사용중인 아이디입니다. 먼저 나오게 하기위해서.
					m_Message->PutMessage( "서버와연결종료. 다시 시작하여주세요", "Code - 136", true );
					break;

				//데이터 송신 메시지
				case FD_WRITE:
					g_pClient->Send();
					break;

				//데이터 수신 메시지
				case FD_READ:
					g_pClient->Recv();
					break;
			}
			break;

		case WM_OUICKFAILCREATEROOM:
			CreateRoom();
			break;

		case WM_AVATARPLAY:
			AvatarPlay( lParam, g_CGameUserList.m_GameUserInfo[lParam].m_sUserInfo.szPreAvatar );
			break;

		case WM_GAMEMODESET:
			GameModeSet();
			break;

		case WM_CHANNELMODESET:
			ChannelModeSet();
			break;

		case WM_WAITMODESET:
			WaitRoomModeSet();
			break;

        case WM_IME_COMPOSITION:
        case WM_IME_ENDCOMPOSITION:
            if ( m_nCurrentMode == WAITROOM_MODE ) {
                m_IME_WaitChat->OnImeComposition( g_hWnd, lParam );         // 대기실 채팅창 한글 입력 처리
                m_IME_InputRoomTitle->OnImeComposition( g_hWnd, lParam );   // 방 제목 한글 입력 처리
                m_IME_RoomInPassBox->OnImeComposition( g_hWnd, lParam );    // 비밀번호 한글 입력 처리
            }
            else if ( m_nCurrentMode == GAME_MODE ) {
                m_IME_GameChat->OnImeComposition( g_hWnd, lParam );         // 게임화면 채팅창 한글 입력 처리
            }

            m_IME_Memo->OnImeComposition( g_hWnd, lParam );                 // 쪽지 입력용 IME 한글 입력 처리
            break;

		default:
			return CFrameWnd::DefWindowProc(message, wParam, lParam);
	}

	return 0;
}



void CMainFrame::ChannelModeSet()
{
}



void CMainFrame::GameModeSet()
{
    //////////////////////////////////////////////////////////////////////////

	m_bActive = FALSE;	//이걸 넣어주지 않으면...메인쓰레드에서 랜더를 하고 GameModeSet도 메인인데 뻑난다.

	//최대화 등 그림을 바꿔준다. 
    
	//상단
	m_MinBTN->SetPos( 958, 5 );
	m_MaxBTN->SetPos( 979, 5 );
	m_MaxBTN2->SetPos( 979, 5 );
	m_ExitBTN->SetPos( 1000, 5 );
	
	//하단
	m_ConfigBTN->SetPos( 814, 721 );
	m_Exit2BTN->SetPos( 955, 721 );  //게임룸
//	m_Exit2BTN->SetPos( 555, 721 );
    m_CaptureBTN->SetPos( 767, 721 );

	//상단
	m_MinBTN->ReGetButtonImage(".\\image\\game\\btn_bar_minimize.spr",  17, 17 );
	m_MaxBTN->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr",  17, 17 );
	m_MaxBTN2->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr", 17, 17 );
	m_ExitBTN->ReGetButtonImage(".\\image\\game\\btn_bar_close.spr",    17, 17 );

	//하단
	m_ConfigBTN->ReGetButtonImage(".\\image\\game\\btn_board_option.spr",    45, 26 );
	m_Exit2BTN->ReGetButtonImage(".\\image\\game\\btn_board_close.spr",      45, 26 );
    m_CaptureBTN->ReGetButtonImage(".\\image\\game\\btn_board_capture.spr",   45, 26 );

	m_bActive = TRUE;
    
    //////////////////////////////////////////////////////////////////////////

	m_bStartInit = FALSE;
	m_bCardChoice = FALSE;
	m_bCardDump = FALSE;

	AvatarPlay( 0, g_szAvaUrl );
	
	m_FrameSkip.Clear();            //처음부터 다시 해준다.

	g_bConnectingSpr = false;
    g_nWaitRoomSelIdx = -1;         // 선택된 방 번호 초기화
	m_nStartBtnCnt = 0;
    m_nWinnerMarkPos = 0;           // 이겼을 때의 이미지의 프레임 번호 초기화
    m_nWinnerMarkPosCnt = 0;

	g_pCMainFrame->m_nCurrentMode = GAME_MODE;

    m_IME_WaitChat->InputEnd();     // 대기실 채팅창 IME도 초기화 한다.
    m_IME_WaitChat->ClearBuffer();

    m_IME_GameChat->InputEnd();     // 게임화면 채팅창 IME 초기화
    m_IME_GameChat->ClearBuffer();

    m_pGameChat->Init();
    m_SCB_GameChat->SetElem( 0 );
    m_SCB_GameChat->StartPage();

    //CloseBanner();
}



void CMainFrame::WaitRoomModeSet()
{
	//////////////////////////////////////////////////////////////////////////

	m_bActive = FALSE;	//이걸 넣어주지 않으면...메인쓰레드에서 랜더를 하고 GameModeSet도 메인인데 뻑난다.

	//최대화 등 그림을 바꿔준다. 
	//상단
	m_MinBTN->SetPos( 962, 5 );
	m_MaxBTN->SetPos( 981, 5 );
	m_MaxBTN2->SetPos( 981, 5 );
	m_ExitBTN->SetPos( 1000, 5 );
	
	//하단
	m_ConfigBTN->SetPos( 881, 4 );
	m_Exit2BTN->SetPos( 29, 555 );  //대기실 
    m_CaptureBTN->SetPos( 804, 4 );

	//상단
	m_MinBTN->ReGetButtonImage(".\\image\\lobby\\btn_min.spr",   16, 16 );
	m_MaxBTN->ReGetButtonImage(".\\image\\lobby\\btn_max.spr",   16, 16 );
	m_MaxBTN2->ReGetButtonImage(".\\image\\lobby\\btn_max2.spr", 16, 16 );
	m_ExitBTN->ReGetButtonImage(".\\image\\lobby\\btn_exit.spr", 16, 16 );

	//하단
	m_ConfigBTN->ReGetButtonImage(".\\image\\lobby\\btn_config.spr",    71, 19 );
	m_Exit2BTN->ReGetButtonImage(".\\image\\lobby\\btn_exit2.spr",     107, 30 );
    m_CaptureBTN->ReGetButtonImage(".\\image\\lobby\\btn_capture.spr",  71, 19 );

	m_bActive = TRUE;
    
	//////////////////////////////////////////////////////////////////////////

	//m_ConfigBTN->SetPos( 804 ,   4 );
	//m_Exit2BTN->SetPos(   29 , 555 );
	m_bShowMakeRoomDlg = FALSE;

	//채널 아바타를 뿌린다.
	AvatarPlay( -1, g_szAvaUrl );

	m_bShowResult = false;
	m_bShowDialog = false;
	m_bShowStartBTN = false;
    m_bShowRoomTitleList = FALSE;

	g_bConnectingSpr = false;
	m_nStartBtnCnt = 0;
    m_nHighlightBarNum = -1;
    m_nSelectedRoomTitleList = -1;
    m_nSelectedRoomNum = -1;
    m_nViewUserIdx = -1;
    m_nViewUserNo = -1;
    m_nUserListTitleNum = 0;            // 대기자 리스트
    m_nCardOption = CO_CHOICE_MODE;     // 대기실로 오면 무조건 기본 패옵션을 초이스 모드로 만든다.

    m_nJackpotBackFrm = 0;              // 잭팟 배경 이미지 프레임 번호 초기화
    m_nJackpotBackFrmCnt = 0;           // 잭팟 배경 이미지 프레임 번호를 바꾸는데 사용하는 시간계산 카운터 초기화
    
    m_pWaitChat->Init();                // 대기실 채팅창 텍스트 초기화
    m_SCB_WaitChat->SetElem( 0 );
    m_SCB_WaitChat->StartPage();
    m_SCB_WaitRoom->StartPage();

	m_nCurrentMode = WAITROOM_MODE;

    m_IME_WaitChat->ClearBuffer();
    m_IME_WaitChat->InputBegin(true);

	//게임유저 리스트 초기화.
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
        if ( i != 0 ) {
            g_CGameUserList.m_GameUserInfo[i].UserInfoInit();
        }

		g_CGameUserList.m_GameUserInfo[i].Init();
		g_CGameUserList.m_GameUserInfo[i].InitCard();
	}

    //ShowBanner();
}



void CMainFrame::SoundLoading()
{
    char szSndPath[255];
    int i = 0;
	
	for ( i = 0; i < MAX_SOUND_CNT; i++ ) {
		g_Sound[i] = NULL;
	}

    memset( szSndPath, 0, sizeof(szSndPath) );
    strcpy( szSndPath, ".\\sound\\effect\\" );

    g_Sound[GS_BTNCLICK]        = LoadSndFile(szSndPath, "button_click.wav", 2);
    g_Sound[GS_ALLIN]           = LoadSndFile(szSndPath, "allin.wav");
    g_Sound[GS_BBING]           = LoadSndFile(szSndPath, "bbing.wav",       2);
    g_Sound[GS_CALL]            = LoadSndFile(szSndPath, "call.wav",        2);
    g_Sound[GS_CHECK]           = LoadSndFile(szSndPath, "check.wav",       2);
    g_Sound[GS_CHIP]            = LoadSndFile(szSndPath, "chip.wav",        2);
    g_Sound[GS_DEAL]            = LoadSndFile(szSndPath, "deal.wav",        3);
    g_Sound[GS_DIE]             = LoadSndFile(szSndPath, "die.wav",         2);
    g_Sound[GS_DOUBLE]          = LoadSndFile(szSndPath, "double.wav",      2);
    g_Sound[GS_FULL]            = LoadSndFile(szSndPath, "full.wav",        2);
    g_Sound[GS_HALF]            = LoadSndFile(szSndPath, "half.wav",        2);
    g_Sound[GS_JACKPOT]         = LoadSndFile(szSndPath, "jackpot.wav");
    g_Sound[GS_QUARTER]         = LoadSndFile(szSndPath, "quarter.wav",     2);
    g_Sound[GS_BACKSTRAIGHT]    = LoadSndFile(szSndPath, "rank_backstraight.wav");
    g_Sound[GS_FLUSH]           = LoadSndFile(szSndPath, "rank_flush.wav");
    g_Sound[GS_FOURCARD]        = LoadSndFile(szSndPath, "rank_fourcard.wav");
    g_Sound[GS_FULLHOUSE]       = LoadSndFile(szSndPath, "rank_fullhouse.wav");
    g_Sound[GS_MOUNTAIN]        = LoadSndFile(szSndPath, "rank_mountain.wav");
    g_Sound[GS_ONEPAIR]         = LoadSndFile(szSndPath, "rank_onepair.wav");
    g_Sound[GS_RSFLUSH]         = LoadSndFile(szSndPath, "rank_rsflush.wav");
    g_Sound[GS_STRAIGHT]        = LoadSndFile(szSndPath, "rank_straight.wav");
    g_Sound[GS_STRAIGHTFLUSH]   = LoadSndFile(szSndPath, "rank_straightflush.wav");
    g_Sound[GS_TOP]             = LoadSndFile(szSndPath, "rank_top.wav");
    g_Sound[GS_TRIPLE]          = LoadSndFile(szSndPath, "rank_triple.wav");
    g_Sound[GS_TWOPAIR]         = LoadSndFile(szSndPath, "rank_twopair.wav");
    g_Sound[GS_ROOMEXIT]        = LoadSndFile(szSndPath, "room_exit.wav");
    g_Sound[GS_ROOMIN]          = LoadSndFile(szSndPath, "roomin.wav");
    g_Sound[GS_POPUP]           = LoadSndFile(szSndPath, "popup.wav");
    g_Sound[GS_WINNER]          = LoadSndFile(szSndPath, "winner.wav");
    g_Sound[GS_LOSE]            = LoadSndFile(szSndPath, "Lose.wav");
    g_Sound[GS_CARDSHRINK]      = LoadSndFile(szSndPath, "cardshrink.wav",    2);
    g_Sound[GS_COUNT]           = LoadSndFile(szSndPath, "count.wav",         2);
    g_Sound[GS_DEALSTART]       = LoadSndFile(szSndPath, "dealstart.wav");
}



//========================================================================
// 모든 사운드를 해제한다.
//========================================================================

void CMainFrame::DestroyAllSound()
{
    for (int i = 0; i < MAX_SOUND_CNT; i++) {
        if ( g_Sound[i] != NULL ) {
            g_Sound[i]->Stop();
            SAFE_DELETE( g_Sound[i] );
        }
    }
}



//========================================================================
// 쪽지를 받았을 때의 처리
//========================================================================

void CMainFrame::RecieveMemo( char *szSendUserNick , char *szMessage )
{
	if( m_Message->GetbShowCheck() )
		m_Message->SetbShowCheck( FALSE );
	
	if( m_bShowMakeRoomDlg ) {
		m_bShowMakeRoomDlg = FALSE;
        m_IME_InputPass->InputEnd();
        m_IME_InputRoomTitle->InputEnd();
	}

    if ( m_bPassDlg ) {
        m_bPassDlg = FALSE;
        m_IME_RoomInPassBox->InputEnd();
    }

    // 채팅 입력을 끝낸다.
    if ( m_nCurrentMode == WAITROOM_MODE ) {
        m_IME_WaitChat->InputEnd();
    }
    else if ( m_nCurrentMode == GAME_MODE ) {
        m_IME_GameChat->InputEnd();
    }

	PlaySound( g_Sound[GS_POPUP] );

    m_IME_Memo->ClearBuffer();
    m_IME_Memo->InputBegin( true );
	m_bShowMessageBox = TRUE;
	
	
	m_bMessageSend = FALSE;                                 // 이 메세지는 받는것이다.
	strcpy( m_szMessageRecieveUserNick , szSendUserNick );  // 보낸 사람이 누구인지 받는다.
	strcpy( m_szGetMessageData , szMessage );               // 보낸 사람이 쓴 메세지를 받는다.
}



void CMainFrame::DeleteUserForScroll( void )
{
    if( m_nScrollNo > 0 ) {
        m_nScrollNo--;
    }
}



void CMainFrame::SetNumLock( BOOL bState )
{
	BYTE keyState[256];

	GetKeyboardState((LPBYTE)&keyState);

	if( (bState && !(keyState[VK_NUMLOCK] & 1)) ||
	    (!bState && (keyState[VK_NUMLOCK] & 1)) )
	{
		// Simulate a key press
		keybd_event( VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );

		// Simulate a key release
		keybd_event( VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,	0);
	}
}



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
    if( (pMsg->message == WM_SYSKEYDOWN) && (pMsg->wParam == VK_F4) ) {
        return TRUE;
    }

	if( pMsg->message == WM_KEYDOWN ) {
		//처음 유저 카드 초이스 부분.
		if ( m_bCardChoice ) {
			switch ( pMsg->wParam ) 
			{
				case VK_NUMPAD1:
					m_bCardChoice = FALSE;
					m_nCardChoiceCnt = 0;
					g_CGameUserList.UserCardChoice( 0, 0 );
					g_pClient->CardChoice( 0 );				
					break;

				case VK_NUMPAD2:
					m_bCardChoice = FALSE;
					m_nCardChoiceCnt = 0;
					g_CGameUserList.UserCardChoice( 0, 1 );
					g_pClient->CardChoice( 1 );
					break;

				case VK_NUMPAD3:
					m_bCardChoice = FALSE;
					m_nCardChoiceCnt = 0;
					g_CGameUserList.UserCardChoice( 0, 2 );
					g_pClient->CardChoice( 2 );
					break;
			}

			if ( !m_bCardChoice ) {
				//카드위치를 다시 제자리로 놓는다.
				g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;

				for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
				}
			}
		}

		//처음 유저 카드 버리는 부분.
		if ( m_bCardDump ) {
			switch ( pMsg->wParam ) 
			{
				case VK_NUMPAD1:
					m_bCardDump = FALSE;
					m_nCardDumpCnt = 0;
					m_bCardChoice = TRUE;
					g_CGameUserList.UserCardDump( 0, 0 );
					g_pClient->CardDump( 0 );				
					break;

				case VK_NUMPAD2:
					m_bCardDump = FALSE;
					m_nCardDumpCnt = 0;
					m_bCardChoice = TRUE;
					g_CGameUserList.UserCardDump( 0, 1 );
					g_pClient->CardDump( 1 );
					break;

				case VK_NUMPAD3:
					m_bCardDump = FALSE;
					m_nCardDumpCnt = 0;
					m_bCardChoice = TRUE;
					g_CGameUserList.UserCardDump( 0, 2 );
					g_pClient->CardDump( 2 );
					break;

				case VK_NUMPAD6:
					m_bCardDump = FALSE;
					m_nCardDumpCnt = 0;
					m_bCardChoice = TRUE;
					g_CGameUserList.UserCardDump( 0, 3 );
					g_pClient->CardDump( 3 );
					break;
			}
		}

		if ( m_bShowStartBTN ) {
            if ( pMsg->wParam == VK_NUMPAD0 ) {
                OnBtnStart();
            }
		}
	
		//베팅하기 
		// '0':따당, '1':체크, '2':쿼터, '3':삥, '4':하프, '5':콜, '6':풀, '7':다이
		if ( m_nUserCallSlotNo == 0 ) {
			switch ( pMsg->wParam ) 
			{
				case VK_NUMPAD0:
					if ( m_Full->status == UP || m_Full->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '6' );
						g_pClient->UserSelect( '6' );
					}

                    /*
					if ( m_Full->status == UP || m_Full->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '6' );
						g_pClient->UserSelect( '6' );
					}
                    */

					break;

				case VK_DECIMAL:
					break;

				case VK_NUMPAD1:
					if ( m_Call->status == UP || m_Call->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '5' );
						g_pClient->UserSelect( '5' );
					}
					break;

				case VK_NUMPAD2:
					if ( m_Die->status == UP || m_Die->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '7' );
						g_pClient->UserSelect( '7' );
					}
					break;

				case VK_NUMPAD4:
					if ( m_BBing->status == UP || m_BBing->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '3' );
						g_pClient->UserSelect( '3' );
					}
					break;

				case VK_NUMPAD5:
					if ( m_Half->status == UP || m_Half->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '4' );
						g_pClient->UserSelect( '4' );
					}
					break;

				case VK_NUMPAD7:
					if ( m_DDaDDang->status == UP || m_DDaDDang->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '0' );
						g_pClient->UserSelect( '0' );
					}
                    /* [*BTN]
					if ( m_DDaDDang->status == UP || m_DDaDDang->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '0' );
						g_pClient->UserSelect( '0' );
					}
                    */
                    if ( m_Check->status == UP || m_Check->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '1' );
						g_pClient->UserSelect( '1' );
					}
					break;

				case VK_NUMPAD8:
					if ( m_Quater->status == UP || m_Quater->status == OVER ) {
						AllCallBtnUp();
						UserSelectView( 0, '2' );
						g_pClient->UserSelect( '2' );
					}
					break;
			}

		}

#ifndef SHOWUSERCARD
		//히든엔터키로 보기.
		if (  pMsg->wParam == VK_RETURN  
           && g_CGameUserList.m_GameUserInfo[0].m_bDie == FALSE
           && g_CGameUserList.m_GameUserInfo[0].m_nCardCnt >= MAX_USER_CARD 
           && !m_IME_GameChat->IsFocus() 
           && m_bEnddingShow == FALSE ) 
        {
			g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[6].nCardNo = g_CGameUserList.m_GameUserInfo[0].m_byUserRealCardNo[6];
			memcpy( g_CGameUserList.m_GameUserInfo[0].m_szJokboCard, g_CGameUserList.m_GameUserInfo[0].m_szTempJokboCard, sizeof(g_CGameUserList.m_GameUserInfo[0].m_szTempJokboCard) );
		}
#endif

	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}



//========================================================================
// 화면을 캡쳐해서 현재 실행파일이 있는 디렉토리의 capture 디렉토리에 저장한다.
//========================================================================

BOOL CMainFrame::ScreenCapture()
{
    char        szFileName[256];
    time_t      sTIME = time( NULL );
    struct tm  *tTIME = localtime( &sTIME );
    DWORD       dwCurrTime = GetTickCount();

    if(-1 == GetFileAttributes(".\\capture")) {     // Capture 디렉토리가 없으면 만든다.
        CreateDirectory(".\\capture", NULL);
    }

    sprintf(szFileName, ".\\capture\\%04d%02d%02d_%ld.bmp", tTIME->tm_year + 1900, tTIME->tm_mon + 1, tTIME->tm_mday, dwCurrTime);
    HRESULT rval = g_pCDXScreen->GetBack()->SaveAsBMP(szFileName);

    if (rval >= 0) {
        char szFullPath[512];
        sprintf( szFullPath, "%s%s로 저장되었습니다.", g_szProgPath, szFileName + 2 );

        STextInfo _sTextInfo;
        memset(&_sTextInfo, 0x00, sizeof(STextInfo) );
        strcpy( _sTextInfo.szText, szFullPath );
        _sTextInfo.nStyle = 0;
        _sTextInfo.rgbColor = RGB( 255 , 255 , 255 );

        switch ( m_nCurrentMode ) {
            case WAITROOM_MODE:
                m_pWaitChat->AddText( &_sTextInfo );
                break;

            case GAME_MODE:
                m_pGameChat->AddText( &_sTextInfo );
                break;

            default:
                return FALSE;
        }
        return TRUE;
    }
    else {
        return FALSE;
    }
}



//========================================================================
// 사운드 연주
//========================================================================

void CMainFrame::PlaySound( CDXSoundBuffer *pSound )
{
    if ( pSound == NULL || g_sClientEnviro.bOption[2] == FALSE || !g_bSoundCard ) {
        return;
    }

    pSound->Play();
}



//========================================================================
// 사운드 파일 로딩
//========================================================================

CDXSoundBuffer *CMainFrame::LoadSndFile( char *szPath, char *szFilename, int nBuffNum )
{
    if ( szPath == NULL || szFilename == NULL || !g_bSoundCard ) {
        return NULL;
    }

    char szFullPath[256];
    memset( szFullPath, 0, sizeof( szFullPath ) );
    strcpy( szFullPath, szPath );
    strcat( szFullPath, szFilename );

    CDXSoundBuffer *pSndBuf = new CDXSoundBuffer();
    pSndBuf->Load( g_pCDXSound, szFullPath, nBuffNum );
    return pSndBuf;
}



//========================================================================
// 버튼 로딩
//========================================================================

CDXButton *CMainFrame::LoadButton(LPSTR szFilename, int nX, int nY, int nW, int nH,
                                  BOOL bMulti, int nCount, int nDirection, int nDistance)
{
    CDXButton *pBtn = NULL;
    pBtn = new CDXButton( g_pCDXScreen, szFilename, nX, nY, nW, nH, bMulti, nCount, nDirection, nDistance );

    if ( pBtn == NULL ) {
        DTRACE("BUTTON LOADING ERROR! - %s", szFilename );
    }
    
    return pBtn;
}



//========================================================================
// 스프라이트 로딩
//========================================================================

CDXSprite *CMainFrame::LoadSprite(LPSTR szFilename, int nW, int nH, int nNum)
{
    CDXSprite *pSPR_Temp = new CDXSprite();
    pSPR_Temp->Create(g_pCDXScreen, szFilename, nW, nH, nNum);

    if ( pSPR_Temp == NULL ) {
        DTRACE("SPRITE LOADING ERROR! - %s", szFilename );
    }

    return pSPR_Temp;
}



//========================================================================
// 스크롤 바 로딩
//
// input:   szUpBTN = UP BUTTON 파일명
//          szDownBTN = DOWN BUTTON 파일명
//          szDragBTN = DRAG BUTTON 파일명
//          pScrollBarRect = 스크롤 바 전체 영역
//          nUpDownBTNWidth = UP/DOWN BUTTON 가로 크기
//          nUpDownBTNHeight = UP/DOWN BUTTON 세로 크기
//          nDragBTNWidth = DRAG BUTTON 가로 크기
//          nDragBTNHeight = DRAG BUTTON 세로 크기
//          nViewNum = 화면에 보이는 항목의 갯수
//          nMaxNum = 저장 가능한 최대 항목의 수
//========================================================================

CDXScrollBar *CMainFrame::LoadScrollBar(LPSTR szUpBTN, LPSTR szDownBTN, LPSTR szDragBTN,
                                        RECT *pScrollBarRect,
                                        int nUpDownBTNWidth, int nUpDownBTNHeight,
                                        int nDragBTNWidth, int nDragBTNHeight,
                                        int nViewNum, int nMaxNum)
{
    CDXScrollBar *pScrollBar = NULL;
    CDXButton    *pUpBTN     = NULL;
    CDXButton    *pDownBTN   = NULL;
    CDXButton    *pDragBTN   = NULL;

    int nUpDownBTNX = 0;                    // UP/DOWN 버튼 X 좌표
    int nDragBTNX   = 0;                    // DRAG 버튼 X 좌표
    int nUpBTNY     = 0;                    // UP 버튼 Y 좌표
    int nDownBTNY   = 0;                    // DOWN 버튼 Y 좌표
    int nDragBTNY   = 0;                    // DRAG 버튼 Y 좌표

    
    nUpDownBTNX = pScrollBarRect->left;

    if (nUpDownBTNWidth > nDragBTNWidth) {  // UP/DOWN 버튼 보다 DRAG 버튼이 작은 경우
        nDragBTNX = nUpDownBTNX + (( nUpDownBTNWidth - nDragBTNWidth ) >> 1);
    }
    else {                                  // 큰 경우
        nDragBTNX = nUpDownBTNX - (( nDragBTNWidth - nUpDownBTNWidth ) >> 1);
    }
    
    nUpBTNY   = pScrollBarRect->top;
    nDownBTNY = pScrollBarRect->bottom + 1 - nUpDownBTNHeight;
    nDragBTNY = pScrollBarRect->top + nUpDownBTNHeight;
    
    pUpBTN   = new CDXButton( g_pCDXScreen, szUpBTN,   nUpDownBTNX, nUpBTNY,   nUpDownBTNWidth, nUpDownBTNHeight );
    pDownBTN = new CDXButton( g_pCDXScreen, szDownBTN, nUpDownBTNX, nDownBTNY, nUpDownBTNWidth, nUpDownBTNHeight );
    pDragBTN = new CDXButton( g_pCDXScreen, szDragBTN, nDragBTNX,   nDragBTNY, nDragBTNWidth,   nDragBTNHeight );
    
    pScrollBar = new CDXScrollBar( pUpBTN, pDownBTN, pDragBTN, nViewNum, nMaxNum );
    pScrollBar->SetDragRect( nDragBTNX, nDragBTNY, nDragBTNX + nDragBTNWidth - 1, nDownBTNY - 1, nDragBTNHeight );
    return pScrollBar;
}



void CMainFrame::DrawText( char *szText, int nX, int nY, COLORREF rgbColor, int nStyle, int nAlign, int nFontSize )
{
    CDXSurface *pSurface = NULL;
    COLORREF rgbRetVal = 0;     // SetTextColor()의 리턴값 저장
    BOOL bRetApiFunc = TRUE;    // Api 함수의 리턴값 저장
    SIZE sTextSize;
    HDC hDC = NULL;
    int nTxtX = 0;
    int nTxtY = 0;
    int nStrLen = 0;            // 문자열 길이
    int nRetVal = 0;            // TextOut() 함수의 리턴값 저장

    if (szText == NULL) {
        DTRACE2("CMainFrame::DrawText() : 그릴 문자열이 없다.");
        return;
    }

    nStrLen = lstrlen(szText);  // 문자열의 길이를 구한다.
    pSurface = g_pCDXScreen->GetBack();

	hDC = pSurface->GetDC();

    if ( hDC == NULL ) {
        DTRACE2("CMainFrame::DrawText() : 리턴된 DC 값이 NULL이라서 텍스트를 못그리겠다!!, nStrLen = %d", nStrLen);
        return;
    }

    nRetVal = pSurface->ChangeFont("굴림체", 0, nFontSize, nStyle);   // 폰트 바꿈

    if ( nRetVal < 0 ) {
        DTRACE2("CMainFrame::DrawText() : 폰트 바꾸기 실패 - 리턴값 %d", nRetVal);
    }

    nRetVal = pSurface->SetFont();                                    // 바꾼 폰트 적용

    if ( nRetVal < 0 ) {
        DTRACE2("CMainFrame::DrawText() : 폰트 설정 실패 - 리턴값 %d", nRetVal);
    }

    bRetApiFunc = GetTextExtentPoint32(hDC, szText, nStrLen, &sTextSize); // 폰트 크기 가져옴

    if ( bRetApiFunc == FALSE ) {
        DTRACE2("CMainFrame::DrawText() : 폰트 정보 가져오기 실패");
    }

    switch (nAlign)
    {
        case TA_LEFT:
            nTxtX = nX;
            nTxtY = nY;
            break;

        case TA_CENTER:
            nTxtX = nX - (sTextSize.cx >> 1);
            nTxtY = nY;
            //nTxtY = nY - (sTextSize.cy >> 1);
            break;

        case TA_RIGHT:
            nTxtX = nX - sTextSize.cx;
            nTxtY = nY;
            //nTxtY = nY - sTextSize.cy;
            break;
    }

    nRetVal = SetBkMode(hDC, TRANSPARENT);

    if ( nRetVal == 0 ) {
        DTRACE2("CMainFrame::DrawText() : 텍스트 배경 투명하게 설정하기 실패 - 리턴값 %d", nRetVal);
    }

	rgbRetVal = SetTextColor(hDC, rgbColor);

    if ( rgbRetVal == CLR_INVALID ) {
        DTRACE2(" CMainFrame::DrawText() : 텍스트 색깔 설정 실패!!!!! - 리턴값 %ld", rgbRetVal );
    }

	bRetApiFunc = ::TextOut(hDC, nTxtX, nTxtY, szText, nStrLen);
    
    if ( bRetApiFunc == 0 ) {
        DTRACE2(" CMainFrame::DrawText() : 텍스트 출력 실패!!!!! - 리턴값 %d", bRetApiFunc );
    }

    nRetVal = pSurface->ReleaseDC();

    if ( nRetVal < 0 ) {
        DTRACE2("CMainFrame::DrawText() : DC 해제하기 실패 - 리턴값 %d", nRetVal);
    }
}



void CMainFrame::ChangeVideoMode(BOOL bFullScreen)
{
    // 전체화면 모드로 변경
	if (bFullScreen) {
		g_pCDXScreen->GetDD()->RestoreDisplayMode();
        AdjustWinStyle();
		if (FAILED(g_pCDXScreen->ChangeVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32))) {
			DTRACE("전체화면모드로 바꿀 수 없습니다.");
            return;
		}
	}

    // 창 모드로 변경
	else {
		g_pCDXScreen->GetDD()->RestoreDisplayMode();
		AdjustWinStyle();

        if (FAILED(g_pCDXScreen->ChangeVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0))) {
			DTRACE("창모드화면으로 바꿀 수 없습니다.");
		}

		CenterWindow();
	}

    //------------------------------------------------
    // !! 화면 모드 바뀌면 모든 이미지의 컬러키 재설정
    //------------------------------------------------
    SetColorKey();

    if ( m_pScrollNotice != NULL ) {
        m_pScrollNotice->Reload();
    }

    // 아바타를 복구한다.
    RecoveryAvatar();
}



void CMainFrame::AdjustWinStyle()
{
	DWORD dwStyle;

    if ( m_bFullScreen ) {    // 만약 전체화면모드이면, 전체화면에 맞게 윈도우 속성을 바꾸어 준다.
		dwStyle = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
        ::SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	else {
		RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

		// 만약 창모드화면이면, 창모드에 맞게 윈도우 속성을 변경한다.
		dwStyle = GetWindowStyle(g_hWnd);
        
		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle); // 윈도우의 속성을 바꾼다.
        AdjustWindowRectEx(&rect, GetWindowStyle(g_hWnd), ::GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));
        ::SetWindowPos(g_hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        ::SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}



void CMainFrame::DrawAvatarScale( CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight )
{
    pSprite->SetPos(nX, nY);
	pSprite->SetStretchWidth(nWidth);   // 게임화면에서 줄여찍기 위한 크기(가로)
    pSprite->SetStretchHeight(nHeight); // 게임화면에서 줄여찍기 위한 크기(세로)
    pSprite->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLKSTRETCHED);
}



//========================================================================
// 대기실 채팅창을 그린다.
//========================================================================

void CMainFrame::RenderWaitChat()
{
    int nCnt = 0;                // 한 화면 분량만 출력하기 위한 카운터
    int nTxtX = 40;              // 텍스트 출력 좌표(X 좌표는 고정)
    int nTxtY = 0;               // 텍스트 출력 좌표(Y 좌표는 매 줄 마다 증가)
    int nTxtStyle = 0;           // 텍스트 스타일
    STextInfo *pTextInfo = NULL; // 텍스트 한 줄을 가리키는 포인터

    nCnt = 0;
    nTxtY = 618;

    for ( int i = m_SCB_WaitChat->GetStartElem(); i < m_pWaitChat->GetTextCnt(); i++ ) {
        pTextInfo = m_pWaitChat->GetTextInfo(i);

        if (pTextInfo != NULL) {
            nTxtStyle = ( pTextInfo->nStyle == 0 ) ? FW_NORMAL : FW_BOLD;
            DrawText( pTextInfo->szText, nTxtX, nTxtY, pTextInfo->rgbColor, nTxtStyle, TA_LEFT );
            nTxtY += 17;

            nCnt++;
            if (nCnt == MAX_VIEW_WAIT_CHAT) break;
        }
    }
/*
    if ( m_IME_WaitChat->IsInputState() ) {
        m_IME_WaitChat->PutIMEText(40, 712, RGB(255, 255, 255), RGB(255, 255, 255));
    }
*/
    m_SCB_WaitChat->Draw();
}



//========================================================================
// 게임화면 채팅창을 그린다.
//========================================================================

void CMainFrame::RenderGameChat()
{
    int nCnt = 0;                // 한 화면 분량만 출력하기 위한 카운터
    int nTxtX = 20;              // 텍스트 출력 좌표(X 좌표는 고정)
    int nTxtY = 0;               // 텍스트 출력 좌표(Y 좌표는 매 줄 마다 증가)
    int nTxtStyle = 0;           // 텍스트 스타일
    STextInfo *pTextInfo = NULL; // 텍스트 한 줄을 가리키는 포인터

    nCnt = 0;
    nTxtY = 566;

    for ( int i = m_SCB_GameChat->GetStartElem(); i < m_pGameChat->GetTextCnt(); i++ ) {
        pTextInfo = m_pGameChat->GetTextInfo(i);

        if (pTextInfo != NULL) {
            nTxtStyle = ( pTextInfo->nStyle == 0 ) ? FW_NORMAL : FW_BOLD;
            DrawText( pTextInfo->szText, nTxtX, nTxtY, pTextInfo->rgbColor, nTxtStyle, TA_LEFT );
            nTxtY += 16;

            nCnt++;
            if (nCnt == MAX_VIEW_GAME_CHAT) break;
        }
    }
/*
    if ( m_IME_GameChat->IsInputState() ) {
        m_IME_GameChat->PutIMEText(20, 738, RGB(255, 255, 255), RGB(255, 255, 255));
    }
*/
    m_SCB_GameChat->Draw();
}



void CMainFrame::OnTimer(UINT nIDEvent) 
{
    if (m_IME_Memo->IsInputState()) {
        m_IME_Memo->RunCaretStatus();
    }

    // IME 커서 깜빡임 처리
    switch ( m_nCurrentMode ) {
        case WAITROOM_MODE:
            if (m_IME_WaitChat->IsInputState()) {
                m_IME_WaitChat->RunCaretStatus();
            }

            if (m_IME_InputPass->IsInputState()) {
                m_IME_InputPass->RunCaretStatus();
            }

            if (m_IME_InputRoomTitle->IsInputState()) {
                m_IME_InputRoomTitle->RunCaretStatus();
            }

            if (m_IME_RoomInPassBox->IsInputState()) {
                m_IME_RoomInPassBox->RunCaretStatus();
            }

            break;

        case GAME_MODE:
            if (m_IME_GameChat->IsInputState()) {
                m_IME_GameChat->RunCaretStatus();
            }
            break;
    }

	CFrameWnd::OnTimer(nIDEvent);
}



void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // 쪽지 키입력 처리
    if ( m_bShowMessageBox ) {
        if ( nChar == VK_RETURN ) {
        }
        else {
            m_IME_Memo->OnKeyDown( nChar );
        }

        return;
    }

    switch ( m_nCurrentMode ) {
        case WAITROOM_MODE:
            {
                //---------------------
                // 엔터키가 입력된 경우
                //---------------------

                if ( nChar == VK_RETURN ) {
                    if ( !m_bShowMakeRoomDlg ) {
                        if ( m_IME_WaitChat->IsFocus() ) {
                            if ( g_pCMainFrame->SendChatTime( GTC() ) ) {
                                char *pIMEText;
                                pIMEText = m_IME_WaitChat->GetIMEText();
        
                                if (strlen( pIMEText ) > 0) {     // 입력한 글이 있을 때만 추가
                                    g_pClient->WaitChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, pIMEText , 0 );
                                    m_IME_WaitChat->ClearBuffer();
                                }
                            }
                        }

                        // 입력 상태가 아닐 때 '엔터'키를 누르면 입력 상태로 만든다.
                        else {
                            m_SCB_WaitChat->EndPage();
                            m_IME_WaitChat->ClearBuffer();
                            m_IME_WaitChat->InputBegin( true );
                        }
                    }
                }

                //----------------------------------
                // 엔터키가 아닌 다른 키가 눌린 경우
                //----------------------------------

                else {
                    if ( m_bShowMakeRoomDlg ) {
                        // 포커스가 있는 IME 하나 만이 입력된 키를 처리한다.
                        m_IME_InputPass->OnKeyDown( nChar );
                        m_IME_InputRoomTitle->OnKeyDown( nChar );
                    }
                    else if ( m_bPassDlg ) {
                        m_IME_RoomInPassBox->OnKeyDown( nChar );
                    }
                    else {
                        m_IME_WaitChat->OnKeyDown( nChar );
                    }
                }
            }
            return;

        case GAME_MODE:
            {
                //---------------------
                // 엔터키가 입력된 경우
                //---------------------

                if ( nChar == VK_RETURN ) {
                    char szChatCopy[512];
                    char *pIMEText = NULL;
                    
                    memset( szChatCopy, 0x00, sizeof( szChatCopy ));
                    pIMEText = m_IME_GameChat->GetIMEText();            // 입력된 문자열을 구한다.

                    if ( m_IME_GameChat->IsFocus()) {
                        // 입력받을 수 있는 시간이고, 입력받은 문자열이 있는 경우에만 전달
                        if ( g_pCMainFrame->SendChatTime( GTC() ) && strlen( pIMEText ) > 0) {

#ifdef CHANGEUSERCARD
                            // 채팅창에서 입력받은 문자열이 명령어이면 관련 처리를 한 다음
                            // 채팅문자열 처리를 하지 않는다.
                            if ( CheckChatCommand( pIMEText ) ) {
                                m_IME_GameChat->ClearBuffer();
                                return;
                            }
#endif
                            if( strlen( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name ) > 14 ) {
							    memcpy( szChatCopy, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, 14 );
							    strcat( szChatCopy, "..." );
							    strcat( szChatCopy, "님의 말: " );
						    }
                            else {
                                sprintf( szChatCopy, "%s님의 말: ", g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );
                            }
						     
						    strcat( szChatCopy , pIMEText );
						    
                            g_pCMainFrame->SetGameChatText( szChatCopy, RGB( 204, 255, 255 ));
						    
						    g_pClient->GameChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, szChatCopy , 0 );
                            m_IME_GameChat->ClearBuffer();
                        }
                    }

                    // 입력상태가 아닐 때 엔터키를 누르면 입력상태로 만든다.
                    // 엔터키를 눌러 마지막 카드를 뒤집어 볼 수 있는 경우에는 채팅창을 입력상태로 만들지 않는다.
                    else if (g_CGameUserList.m_GameUserInfo[0].m_nCardCnt < MAX_USER_CARD) {
                        m_SCB_GameChat->EndPage();
                        m_IME_GameChat->ClearBuffer();
                        m_IME_GameChat->InputBegin( true );
                    }
                }

                //----------------------------------
                // 엔터키가 아닌 다른 키가 눌린 경우
                //----------------------------------
                
                else {
                    m_IME_GameChat->OnKeyDown( nChar );
                }
            }
            return;
    }
	
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if ( m_bShowMessageBox ) {
        m_IME_Memo->OnChar( nChar );
        return;
    }

    switch ( m_nCurrentMode ) {
        case WAITROOM_MODE:
            // CDXIME는 입력가능상태고, 포커스가 있을 때만
            // 내부적으로 입력 처리를 한다.
            m_IME_WaitChat->OnChar( nChar );

            if (m_bShowMakeRoomDlg) {
                m_IME_InputPass->OnChar( nChar );
                m_IME_InputRoomTitle->OnChar( nChar );
            }
            else if ( m_bPassDlg ) {
                m_IME_RoomInPassBox->OnChar( nChar );
            }
            return;

        case GAME_MODE:
            m_IME_GameChat->OnChar( nChar );        // 내부적으로 포커스가 있을 때만 처리
            return;
    }
    
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}



//========================================================================
// 대기실 인덱스를 정렬한다.
//
// 1. 유저가 참여할 수 있는 방을 앞쪽에 위치시킨다.
// 2. 꽉 찬 방은 뒤쪽에 위치시킨다.
//========================================================================

void CMainFrame::WaitRoomIndexRefresh()
{
    int i = 0;
    int nRoomIdx = 0;           // 방 정렬시 인덱스로 사용(HAM)
    memset(g_nWaitRoomIdx, 0x00, sizeof(g_nWaitRoomIdx));

    // 1. 꽉 차지 않은 방의 인덱스부터 저장한다.
    for (i = 0; i < MAX_ROOMCNT; i++) {
        if ( g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt <= 0 ) continue;    // 유저가 없으면 처리 안함

        if (g_CWaitRoomList.m_sWaitRoomList[i].nTotCnt > g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt) {
            g_nWaitRoomIdx[nRoomIdx++] = i;
        }
    }

    // 2. 그 다음 꽉 찬 방의 인덱스들을 차례대로 저장한다.
    for (i = 0; i < MAX_ROOMCNT; i++) {
        if ( g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt <= 0 ) continue;    // 유저가 없으면 처리 안함

        if (g_CWaitRoomList.m_sWaitRoomList[i].nTotCnt <= g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt) {
            g_nWaitRoomIdx[nRoomIdx++] = i;
        }
    }

    g_nWaitRoomCnt = nRoomIdx;              // 대기실 방 갯수 갱신
    m_SCB_WaitRoom->SetElem( nRoomIdx );    // 대기실 스크롤 바 전체 항목 수 갱신

    // 방이 사라져서 현재 페이지에 방이 없는 경우
    // 방 리스트의 첫 페이지로 이동한다.
    if ( m_SCB_WaitRoom->GetStartElem() >= nRoomIdx ) {
        m_SCB_WaitRoom->StartPage();
    }

    // 이전에 방이 선택되어 있던 경우.
    if ( g_nWaitRoomSelIdx != -1 ) {
        // 선택바가 방이 있는 위치에 있는 경우
        if ( g_nWaitRoomSelIdx < g_nWaitRoomCnt ) {
            m_nSelectedRoomNum = g_nWaitRoomIdx[ g_nWaitRoomSelIdx ];   // 선택된 방 번호 갱신
        }

        // 선택바가 방이 없는 위치에 있는 경우
        else {
            m_nSelectedRoomNum = -1;
            g_nWaitRoomSelIdx = -1;
        }
    }
}



//========================================================================
// 현재 보고 있는 방의 정보를 갱신한다.
//
// input:   nRoomNo     = 갱신할 방 번호
//========================================================================

void CMainFrame::ViewRoomInfoRefresh( ush_int nRoomNo )
{
    if ( nRoomNo < g_nWaitRoomCnt && g_nWaitRoomSelIdx >= 0 && g_nWaitRoomSelIdx < g_nWaitRoomCnt ) {
        if ( g_nWaitRoomIdx[ g_nWaitRoomSelIdx ] == nRoomNo )
            g_pClient->GetRoomInfo( nRoomNo );
    }
}



//========================================================================
// 채팅창에 텍스트를 추가한다.
//========================================================================

void CMainFrame::SetGameChatText( char *szText, COLORREF rgbColor )
{
    STextInfo sTempMsg;
    memset( &sTempMsg, 0x00, sizeof(STextInfo) );
    strcpy( sTempMsg.szText, szText );
    sTempMsg.nStyle = 0;
    sTempMsg.rgbColor = rgbColor;
    m_pGameChat->AddText( &sTempMsg );
    m_SCB_GameChat->SetElem( m_pGameChat->GetTextCnt() );
    m_SCB_GameChat->EndPage();
}



/*
void CMainFrame::ShowBanner()
{
	::ShowWindow( m_pWaitRoomBanner->m_hWnd, SW_SHOW );
    ::SetFocus( g_hWnd );
}

void CMainFrame::CloseBanner()
{
	::ShowWindow( m_pWaitRoomBanner->m_hWnd, SW_HIDE );
    ::SetFocus( g_hWnd );           // CHttpView로 인해 키 입력이 안되는 문제 방지
}
*/



//========================================================================
// 톨을 가운데로 던진다.
//========================================================================

void CMainFrame::ThrowChip( int nSlot, BigInt nMoney ) {
    int nCnt = 0;
    int nFrm = 0;
    int nBaseX = 0;
    int nBaseY = 0;
    int nWidth = 0;
    int nHeight = 0;
    BigInt nTempMoney = 0;
    sChipInfo chip;
    srand( (unsigned)time( NULL ));                     // seed 초기화

    chip.ctChipType   = CT_MOVE;
    chip.nRemoveCnt   = -1;
    chip.nAniCount    = -1;
    chip.nAniMaxCount = 3;//g_nGameSpeed;
    chip.StartPos.x   = m_sRoomInUserPosition[nSlot].pChipStartPos.x;
    chip.StartPos.y   = m_sRoomInUserPosition[nSlot].pChipStartPos.y;
    chip.Pos.x        = chip.StartPos.x;
    chip.Pos.y        = chip.StartPos.y;

    nTempMoney = nMoney;
    nCnt = 0;

    //-------------------------
    // 전체 톨의 갯수를 구한다.
    //-------------------------

    while ( nTempMoney > 0 ) {
        nCnt += (int)( nTempMoney % 10 );
        nTempMoney /= 10;
    }

    //------------------------------------
    // 톨이 전져질 위치의 범위를 설정한다.
    //------------------------------------

    if ( nCnt > 5 ) {   // 톨이 5개 보다 많을 때
        nBaseX = 412;
        nBaseY = 130;
        nWidth = 200;
        nHeight = 200;
    }
    else {              // 톨이 5개 이하일 때
        nBaseX = 462;
        nBaseY = 180;
        nWidth = 100;
        nHeight = 100;
    }

    nTempMoney = nMoney;
    nCnt = 0;

    while ( nTempMoney > 0 ) {
        nCnt = (int)( nTempMoney % 10 );                // 현재 단위의 톨 갯수를 구한다.(가장 작은 단위부터 시작)

        for ( int i = 0; i < nCnt; i++ ) {              // 톨 갯수만큼 에니메이션 스택에 추가
            chip.nChipNo = nFrm;
            chip.NewPos.x = nBaseX + ( rand() % nWidth );
            chip.NewPos.y = nBaseY + ( rand() % nHeight );

            m_ChipAni.AddStack( &chip );
        }

        nTempMoney /= 10;
        nFrm++;
    }
}



//========================================================================
// 유저가 던진 톨들을 그린다.
//========================================================================

void CMainFrame::RenderThrowChip()
{
    sChipInfo *chip = NULL;
    CDXSurface *pSurface = g_pCDXScreen->GetBack();
    int nAlpha = 0;

    for( int i = 0; i <= m_ChipAni.GetAniCount(); i++ ) {
		chip = m_ChipAni.GetChip( i );

        if( chip != NULL && chip->ctChipType != CT_NONE ) {
            m_SPR_Chip->SetFrame( chip->nChipNo );
            m_SPR_Chip->SetPos( chip->Pos.x, chip->Pos.y );
            m_SPR_Chip->Draw( pSurface, 0, 0, CDXBLT_TRANS );
        }
    }
}



//========================================================================
// 버릴 카드와 보여줄 카드를 선택하는 대화상자를 그린다.
//========================================================================

void CMainFrame::RenderCardSelectionDlg()
{
    BYTE nCardNo = 0;

    //선택 화면을 나중에 찍어야 다른 카드에 안가려진다.
	if ( m_bCardDump )
	{
		m_SPR_WasteCard->SetPos( 314, 284 );
        m_SPR_WasteCard->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
			g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = 333 + 92 * i;
			g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = 328;

			nCardNo = g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].nCardNo;		
			
			PutCardImage( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x, 
				          g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y, nCardNo );
		}
	}

	if ( m_bCardChoice ) {
		m_SPR_CardChioce->SetPos( 360, 284 );
        m_SPR_CardChioce->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
			g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = 379 + ( 92 * i );
			g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = 328;

			nCardNo = g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].nCardNo;		
			
			PutCardImage( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x, 
				          g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y, nCardNo );
		}
	}
}



//========================================================================
// 스프라이트를 그린다.
//
// input:
//      pSprite     = 출력할 스프라이트
//      nX, nY      = 출력할 좌표
//      nFrame      = 스프라이트 프레임 번호
//========================================================================

void CMainFrame::PutSprite( CDXSprite *pSprite, int nX, int nY, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
}




//========================================================================
// 컬러키가 필요없는 이미지 출력에 사용
//
// input:
//      pSprite:    출력할 스프라이트
//      nX, nY:     출력할 좌표
//      nFrame:     스프라이트 프레임 번호
//========================================================================

void CMainFrame::PutSpriteBlk( CDXSprite *pSprite, int nX, int nY, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );
}



//========================================================================
// 스프라이트를 그린다.
//
// input:
//      pSprite     = 출력할 스프라이트
//      nX, nY      = 출력할 좌표
//      srcReect    = 원본에서 출력할 범위 지정
//========================================================================

void CMainFrame::PutSprite( CDXSprite *pSprite, int nX, int nY, RECT *srcRect )
{
    pSprite->GetTile()->DrawTrans( g_pCDXScreen->GetBack(), nX, nY, srcRect );
}



//========================================================================
// 이미지를 확대 또는 축소해서 출력한다.
//========================================================================

void CMainFrame::DrawScale(CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame)
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos(nX, nY);
	pSprite->SetStretchWidth(nWidth);   // 게임화면에서 줄여찍기 위한 크기(가로)
    pSprite->SetStretchHeight(nHeight); // 게임화면에서 줄여찍기 위한 크기(세로)
    pSprite->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANSSTRETCHED);
}



//========================================================================
// 게임 종료(WM_CLOSE 메시지를 보낸다.)
//========================================================================

void CMainFrame::ExitGame()
{
    g_bRealExit = true;     // 프로그램에서 나간다는 표시( 강제종료를 방지하기 위해 사용)
    ::PostMessage( g_hWnd, WM_CLOSE, 0, 0 );
}



//========================================================================
// 프로그램 강제 종료 방지
//
// WM_CLOSE 메시지 발생시 프로그램에서 종료요청을 한 것이면
// 종료시키고, 외부에서 강제종료하려고 시도했던 것이면
// 메시지를 무시한다.
//========================================================================

void CMainFrame::OnClose() 
{

#ifndef _DEBUG
    if ( g_bRealExit == false ) {
        return;
    }
#endif
	
	CFrameWnd::OnClose();
}



//========================================================================
// 화면모드 전환등으로 인한 표면 파괴시 아바타 이미지를 복구한다.
// 에니메이션 GIF인 경우 자동으로 계속 그려주므로 에니메이션 GIF가
// 아닌 경우에만 복구해주면 된다.
//========================================================================

void CMainFrame::RecoveryAvatar()
{
    // 대기실 아바타 복구
    if ( m_SPR_WaitAvatar != NULL ) {
        if ( m_WaitAvaImg != NULL ) {
            if ( !m_WaitAvaImg->IsAnimatedGIF() ) {
                m_WaitAvaImg->InitAnimation( g_hWnd, CPoint(0, 0) );
            }
        }
    }

    // 게임방 아바타 복구
    for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
        if ( m_SPR_GameAvatar[i] != NULL ) {
            if ( m_GameAvaImg[i] != NULL ) {
                if ( !m_GameAvaImg[i]->IsAnimatedGIF() ) {
                    m_GameAvaImg[i]->InitAnimation( g_hWnd, CPoint(0, 0) );
                }
            }
        }
    }
}



void CMainFrame::OnKillFocus(CWnd* pNewWnd) 
{
	CFrameWnd::OnKillFocus(pNewWnd);
	
	RecoveryAvatar();
}



#ifdef CHANGEUSERCARD

//========================================================================
// 채팅 문자열을 분석해서 명령어이면 관련 처리해서 서버에 패킷을 보내고
//
// input:
//      szText : 채팅창에서 입력한 문자열의 포인터
//
// output:
//      TRUE = 입력한 것이 명령어이고 성공적으로 처리를 하였다.
//      FALSE = 입력한 것이 일반 채팅 문자열이거나, 잘못된 명령어이다.
//========================================================================

BOOL CMainFrame::CheckChatCommand(char *szText)
{
    char szCommBuf[256];
    char szParam1[256];
    char szParam2[256];
    int nParamNum = 0;
    memset(szCommBuf, 0x00, sizeof(szCommBuf));
    memset(szParam1,  0x00, sizeof(szParam1));
    memset(szParam2,  0x00, sizeof(szParam2));

    nParamNum = sscanf(szText, "%s %s %s", szCommBuf, szParam1, szParam2);

    // EOF(-1)가 아니고 3개 다 제대로 파싱한 경우
    if (nParamNum >= 3) {

        if ((strcmp(szCommBuf, "/C") == 0) || (strcmp(szCommBuf, "/c") == 0))
        { 
            // 여기서 바꿀자리 번호와 새카드 번호 값이
            // 적절한 값이면 서버에 카드 바꾸기 패킷을 보낸다.
            DTRACE2("명령어네.... %s, %s, %s", szCommBuf, szParam1, szParam2);

            SendChangeUserCardInfo( szParam1, szParam2 );
                     
            return TRUE;
        }
        // "/"로 시작하는 문자열은 채팅 처리하지 않는다.
        else if ( strlen(szCommBuf) >= 1 && szCommBuf[0] == '/' ) {
            SetGameChatText( "◎ 잘못된 명령어입니다." , RGB( 255, 255, 255 ));
            return TRUE;
        }
    }
    else if ( nParamNum >= 1 ) {
        // 잘못된 명령어
        if ( strlen(szCommBuf) >= 1 ) {
            if ( szCommBuf[0] == '/' ) {
                SetGameChatText( "◎ 잘못된 명령어입니다." , RGB( 255, 255, 255 ));
                return TRUE;
            }
        }
    }

    // 일반 채팅 메시지
    return FALSE;
}

#endif



#ifdef CHANGEUSERCARD

//========================================================================
// 전달된 정보를 이용하여 유저의 카드를 새 카드로 바꾼다.
//
// input:
//        typedef struct tagUserChange
//        {
//            BYTE bUerCardNo;      // 내 카드 번호
//            BYTE bChangeCardNo;   // 새로 바꿀 카드 번호
//        } sUserChange;
//
//========================================================================

void CMainFrame::ChangeUserCard( int nSlotNo, sUserChange *pChgCardInfo )
{
    if ( pChgCardInfo == NULL ) {
        return;
    }

    // 카드를 찾아서 새로운 카드로 바꿔준다.

    for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[nSlotNo].m_nCardCnt; i++ ) {
        if ( g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserCardInfo[i].nCardNo == pChgCardInfo->bUerCardNo ) {
            g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserCardInfo[i].nCardNo = pChgCardInfo->bChangeCardNo;
            break;
        }
    }
}

#endif



#ifdef CHANGEUSERCARD

//========================================================================
// 서버에 바꿀 카드 정보를 보낸다.
//
// input:
//      szParam1 : 바꿀 내 카드 번호
//      szParam2 : 새로 받을 카드 번호
//========================================================================

void CMainFrame::SendChangeUserCardInfo(char *szParam1, char *szParam2)
{
    bool bExistCard = false;
    int nCurCardNo = ConvCardNum( szParam1 );   // 바꾸고 싶은 기존의 카드 번호
    int nNewCardNo = ConvCardNum( szParam2 );   // 새로 받고 싶은 카드 번호

    DTRACE2 (" :::::카드번호 : %d, %d", nCurCardNo, nNewCardNo );

    // 어느 하나라도 잘못된 정보이면 카드를 바꾸지 않는다.
    if ( nCurCardNo == -1 || nNewCardNo == -1 ) {
        SetGameChatText( "◎ 잘못된 명령어입니다." , RGB( 255, 255, 255 ));
        return;
    }

    for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
        if ( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].nCardNo == (BYTE)nCurCardNo ) {
            if ( i < 2 || i == 6 ) {
                // 카드를 바꾼다.
                sUserChange l_sTempCardInfo;
                memset( &l_sTempCardInfo, 0x00, sizeof( l_sTempCardInfo ));
                l_sTempCardInfo.bUerCardNo = (BYTE)nCurCardNo;
                l_sTempCardInfo.bChangeCardNo = (BYTE)nNewCardNo;

                g_pClient->UserChange( &l_sTempCardInfo );
            }
            else {
                SetGameChatText( "◎ 이 카드는 바꿀 수 없습니다." , RGB( 255, 255, 255 ));
            }

            bExistCard = true;
            break;
        }
    }

    if ( !bExistCard ) {
        SetGameChatText( "◎ 없는 카드입니다." , RGB( 255, 255, 255 ));
    }
}

#endif



#ifdef CHANGEUSERCARD

//========================================================================
// 입력받은 카드정보 문자열을 카드 번호로 변환한다.
//
// input:
//      szCardInfo : 카드 정보 문자열(C2 = 클로버2, SK = 스페이드 킹, HA = 하트 에이스
//
// return:
//      -1 : 카드 정보 문자열이 아니거나 잘못된 정보
//      0 이상 : 카드 번호
//      
//========================================================================

int CMainFrame::ConvCardNum( char *szCardInfo )
{
    if ( szCardInfo == NULL ) {
        return -1;
    }

    int nLen = strlen( szCardInfo );    // 문자열의 길이를 구한다.

    // 카드 번호 문자열은 2~3 글자로 되어 있다.
    if ( nLen < 2 || nLen > 3 ) {
        return -1;
    }

    int nCardNo = 0;                // 카드 번호

    // 해당 무늬의 카드의 시작번호를 구한다.

    switch ( szCardInfo[0] ) {
        // 스페이드
        case 'S':
        case 's':
            nCardNo = 13 * 3;            // 해당 무늬의 첫 카드 번호
            break;

        // 다이아
        case 'D':
        case 'd':
            nCardNo = 13 * 2;       // 해당 무늬의 첫 카드 번호
            break;

        // 하트
        case 'H':
        case 'h':
            nCardNo = 13 * 1;       // 해당 무늬의 첫 카드 번호
            break;

        // 클로버
        case 'C':
        case 'c':
            nCardNo = 0;            // 해당 무늬의 첫 카드 번호
            break;

        default:
            return -1;
    }

    // 3글자인 경우 10번 카드인지 검사
    if ( nLen == 3 ) {
        if ( szCardInfo[1] == '1' && szCardInfo[2] == '0' ) {
            nCardNo += 8;           // 포커에서는 8이 10번 카드
        }
        // 3글자 이고 "10"이 아닌 경우는 잘못된 정보
        else {
            return -1;
        }
    }
    // 2글자인 경우 10 카드를 제외한 나머지 카드 검사
    else {
        switch ( szCardInfo[1] ) {
            case '2':
                nCardNo += 0;
                break;

            case '3':
                nCardNo += 1;
                break;

            case '4':
                nCardNo += 2;
                break;

            case '5':
                nCardNo += 3;
                break;

            case '6':
                nCardNo += 4;
                break;

            case '7':
                nCardNo += 5;
                break;

            case '8':
                nCardNo += 6;
                break;

            case '9':
                nCardNo += 7;
                break;

            case 'J':
            case 'j':
                nCardNo += 9;
                break;

            case 'Q':
            case 'q':
                nCardNo += 10;
                break;

            case 'K':
            case 'k':
                nCardNo += 11;
                break;

            case 'A':
            case 'a':
            case '1':
                nCardNo += 12;
                break;

            default:
                return -1;
        }
    }

    // 최정적으로 변환된 카드번호 리턴
    return nCardNo;
}

#endif
