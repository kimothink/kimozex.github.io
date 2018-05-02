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
// ȯ�� ����â�� �׸���.
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
	m_nSelectedRoomNum = -1;    // ��� ���� �������� ����.
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
	m_bMessageSend = TRUE; // ������ �����°��� �޴� ���� üũ // TRUE = ������. FALSE = �޴´�.
	m_bMouseEvent = TRUE;
	m_bMouseRender = TRUE;
	m_bCardChoice = FALSE;	//ó�� ī�带 ���̽� �ϴºκ�.
	m_bCardDump = FALSE;	//4���� ���� ������.
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
	m_SPR_Money = NULL; // ��� ǥ���� ��
	m_SPR_MessageBox = NULL; // ���� â
	m_SPR_Message = NULL; // �޼��� �ڽ� �׸�
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
	
	m_MessageOKBTN = NULL; // �޼��� �ڽ� OK ��ư
	m_MaxBTN = NULL;
	m_MaxBTN2 = NULL;
	m_ExitBTN = NULL;
	m_ConfigBTN = NULL;
	m_Exit2BTN = NULL;
	m_MinBTN = NULL;
	m_FullBTN = NULL; // Ǯ��ũ�� ��ư ( ä�� )
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

	m_ColControl[0] = RGB( 221, 242, 255 );             // �� ���� 
	m_ColControl[1] = RGB( 255, 255, 255 );             // ���� ä��
	m_ColControl[2] = RGB(   3,  38,  56 );             // ���� �� ä��
	m_ColControl[3] = RGB( 255, 255, 255 );             // �޽��� ..

	m_Brush[0] = CreateSolidBrush( m_ColControl[0] );   // �� ���� 
	m_Brush[1] = CreateSolidBrush( m_ColControl[1] );   // ���� ä��
	m_Brush[2] = CreateSolidBrush( m_ColControl[2] );   // ���� �� ä��
	m_Brush[3] = CreateSolidBrush( m_ColControl[3] );   // �޽��� ..

    // �游��� â���� �ݾ� �׸� üũ �̹��� ��ġ
	m_sMakeRoomMoneyPoint[0] = SetPoint( 378, 361 );
	m_sMakeRoomMoneyPoint[1] = SetPoint( 518, 361 );
	m_sMakeRoomMoneyPoint[2] = SetPoint( 378, 382 );
	m_sMakeRoomMoneyPoint[3] = SetPoint( 518, 382 );
    m_sMakeRoomMoneyPoint[4] = SetPoint( 378, 403 );
	m_sMakeRoomMoneyPoint[5] = SetPoint( 518, 403 );
	m_sMakeRoomMoneyPoint[6] = SetPoint( 378, 424 );
	m_sMakeRoomMoneyPoint[7] = SetPoint( 518, 424 );

	//�� �� ��ġ. 0 ~���� ���� ����. 100����, 1000����......
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

    // �� ������ �� ������ ���� ��ġ - ham 2005.09.26
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
    
	//GameRender���� ��ġ.
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

		DTRACE2("[�� ��Ŀ] ������ �ñ� %s (%s)\n",__DATE__,__TIME__);


		char szTemp[255];
		//�Ѿ�� ����Ȯ��
		if( __argc != 10 ) {       //�������� �����ѹ�, ���̵�
			::MessageBox( hWnd, "������ �������ּ���.", "NewZenPoker", MB_OK );
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
		DTRACE("[5] ���� ���� ---- %s", __argv[5]);
		DTRACE("[6] Channel ------ %s", __argv[6]);
		DTRACE("[7] ���� ���� -- %s", __argv[7]);
		DTRACE("[8] ������ ���� -- %s", __argv[8]);

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


    DTRACE2( "[%s] ���α׷� ����", g_szUser_ID );



    memset(g_szProgPath, 0x00, sizeof(g_szProgPath));

	//yun ���� ����Ǵ� ��ü��ο� ���� ������ �����´�. c:\aaa\bbb.exe
	GetModuleFileName(::GetModuleHandle(NULL), g_szProgPath, sizeof(g_szProgPath));

	int path_len = strlen(g_szProgPath);

	//yun ��θ� ã�Ƽ� �ڿ��� ���� �о�帰�� ������ ��� �ձ����� ��θ� ã�´�.
	// ������ ��ġ�� ���� c:\aaa\�� ���´�.
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
	//DirectX���� üũ
	DWORD VerCheck = 0;
	VerCheck = g_dx2d.GetDXVerCheck();

	// DX Version�� 7.0 ���� �۰ų� ���ٸ� ������ ǥ���Ѵ�.
	if( VerCheck < 0x700 )
	{
		//ShowCursor(TRUE);
		int nRe = ::MessageBox( hWnd, "DirectX ���� 7.0�� ��ġ�ؾ� �մϴ�!", "DirectX Version Error", MB_ICONERROR | MB_YESNO );

		if( nRe == IDYES ) { // YES
			
			::ShellExecute( hWnd, NULL, "http://www.microsoft.com/downloads/details.aspx?FamilyID=141d5f9e-07c1-462a-baef-5eab5c851cf5&displaylang=ko", NULL, NULL, SW_SHOW );
		}
		else // NO
		{
			::MessageBox( hWnd, "DirectX 7.0�� ��ġ���� �ʾҽ��ϴ�.", "DX Version Error", MB_ICONWARNING | MB_OK );
		}
		
		::PostMessage( hWnd, WM_CLOSE, 0, 0 );
		
		return 0;
	}
    */

    m_pWaitChat = new CTextList( 98, MAX_TEXT_LINE );
    m_pGameChat = new CTextList( 48, MAX_TEXT_LINE );

	//config.ini �о�ͼ� ����.
	LoadEnviroMent();

	g_pClient = new CNetworkSock;

	if( g_pClient->InitSocket( g_hWnd, g_LSIp, g_LSPort ) ) {
		g_ServerSend = false;
        g_pCDXScreen = new CDXScreen();

        if( FAILED(g_pCDXScreen->CreateWindowed(g_hWnd, SCREEN_WIDTH, SCREEN_HEIGHT))) {
            DTRACE("1024x768 â��带 ������ �� �����ϴ�.");
            return 0;
        }

        g_pCDXInput = new CDXInput();

        if (g_pCDXInput == NULL ) {
            DTRACE("�Է���ġ�� ������ �� �����ϴ�.");
            return 0;
        }

        if (g_pCDXInput->Create( (HINSTANCE)GetWindowLong( g_hWnd, GWL_HINSTANCE ), g_hWnd) < 0) {
            DTRACE("�Է���ġ�� ������ �� �����ϴ�.");
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
        // ���� ����â�� �����.

        m_pWaitRoomBanner = new CHttpView;

	    if (!m_pWaitRoomBanner->CreateWnd( CRect(13, 13, 682, 144), this )) 
		    return -1;

        if ( g_nServiceDiv == SERVICE_TORIZONE ) {          // �丮�� ����
            m_pWaitRoomBanner->Navigate( "http://www.torizone.net/advertise/advertise.asp" );
        }
        else if ( g_nServiceDiv == SERVICE_GAMEPOOL ) {     // ����Ǯ ����
            m_pWaitRoomBanner->Navigate( "http://www.gamepool.co.kr/advertise/advertise.asp" );
        }
        else {                                              // ��Ÿ
            m_pWaitRoomBanner->Navigate( "http://www.torizone.net/advertise/advertise.asp" );
        }
        */
	}

	SetNumLock( TRUE );


	return 0;
}



//========================================================================
// ó�� ������ �����Ҷ� �ʱ�ȭ.
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

	//��ư ����.
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


    //�� �� �ִ� ���� �ʱ�ȭ
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

	//��ư ����.
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


    //�� �� �ִ� ���� �ʱ�ȭ
    m_CAniMation.EmptyStack();
    m_ChipAni.EmptyStack();

	DTRACE2("[%s]�̺�ƮǬ��", g_szUser_ID);

    if ( SetEvent(m_hGameInitEvent) == FALSE ) {
        DTRACE2("[%s] CMainFrame::NetWorkThreaOnGameInit() - SetEvent() ����", g_szUser_ID );
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
	//50 �� 1��.
	//========================================================================

    if ( m_nCurrentMode == GAME_MODE ) {

        //----------------------------------------
        // ���� ��� �̹����� �����ӹ�ȣ�� �ٲ۴�.
        //----------------------------------------

        m_nJackpotBackFrmCnt++;

        if ( m_nJackpotBackFrmCnt >= 10 ) {
            m_nJackpotBackFrmCnt = 0;
            m_nJackpotBackFrm = ( m_nJackpotBackFrm + 1 ) % 2;  // ���� ������
        }
    }

    //-------------------
    // ��ũ�� �ؽ�Ʈ �̵�
    //-------------------

    if ( m_nCurrentMode == WAITROOM_MODE && m_pScrollNotice != NULL) {
        m_pScrollNotice->Update();
    }

    //-------------------------------
	// ������ ��, ���̵� �θ��� �ð�.
    //-------------------------------

	if ( m_bUserCallTime ) {
		m_nUserCallTime++;

		//5�� ���ܳ��� Ÿ�̸� �����Ѵ�.
		if ( ( m_nUserCallDefineTime - m_nUserCallTime ) <= 500 ) {     // ham 2005.09.26 - 250 Frame --> 500 Frame

			if ( !(m_nTemp % 50) ) {
				m_nTimePos = ( m_nTemp / 50 );

                if ( m_nTimePos >= 5 ) {                                // 5�� ���Ϸ� ������ �� �Ҹ��� ����Ѵ�.
                    PlaySound( g_Sound[GS_COUNT] );
                }
			}
			m_nTemp++;
		}
       
		//������ Ÿ���� ������ �ڵ� ���� �Ѵ�.
		if ( m_nUserCallTime > m_nUserCallDefineTime ) {
			m_nUserCallTime = 0;
			m_bUserCallTime = FALSE;
			m_nTemp = 0;
			m_nTimePos = 0;

            // [*IMSI]
			//�ڽ��̸� ���̸� ������.
			if ( m_nUserCallSlotNo == 0 ) {
				AllCallBtnUp();
				g_pClient->UserSelect( '7' );
			}
            ////
		}

	}

    //-------------------------
	// ��ŸƮ ��ư ���ִ� �ð�.
    //-------------------------

	if ( m_bShowStartBTN ) {

		m_nStartBtnCnt++;

		if( m_nStartBtnCnt > ONSTARTBTN_TIME ) {    // ���ڰ� Ŭ���� �� ���� ������
			OnBtnStart();
		}
	}

	//���̽� ���϶�.
	if ( g_bChoiceRoom ) {
		//------------------
		// �����ϱ�.
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
		// 4�� �� �� �� ������
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
		// ���̽� ȭ�� ���ִ� �ð�.
		//-------------------------

        // [*IMSI]
		if ( m_bCardChoice ) {
			m_nCardChoiceCnt++;

			if ( m_nCardChoiceCnt > CHOICECARD_TIME ) {
				m_bCardChoice = FALSE;
				m_nCardChoiceCnt = 0;
				g_CGameUserList.UserCardChoice( 0, 2 );
				g_pClient->CardChoice( 2 );

				//�ڵ� ���̽��� �ǰ� ���� ī�带 ���ڸ��� �Űܾ� �Ѵ�.
				//ī����ġ�� �ٽ� ���ڸ��� ���´�.
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
		// �����ϱ�.
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
		// ���̽� ȭ�� ���ִ� �ð�.
		//-------------------------

		if ( m_bCardChoice ) {

			m_nCardChoiceCnt++;

			if ( m_nCardChoiceCnt > CHOICECARD_TIME ) {
				m_bCardChoice = FALSE;
				m_nCardChoiceCnt = 0;
				g_CGameUserList.UserCardChoice( 0, 2 );
				g_pClient->CardChoice( 2 );

				//�ڵ� ���̽��� �ǰ� ���� ī�带 ���ڸ��� �Űܾ� �Ѵ�.
				//ī����ġ�� �ٽ� ���ڸ��� ���´�.
				g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
				for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
				}
			}
		}
	}

    //--------------------
	// ���â ���ִ� �ð�.
    //--------------------

	if ( m_bEnddingShow ) {
        // �̰��� ���� ���ϸ��̼��� ���� ���� �������� �����Ѵ�.
        m_nWinnerMarkPosCnt++;

        if ( m_nWinnerMarkPosCnt >= 3 ) {                       // 3 Frame ���� �̹����� ������ �ٲ۴�.
            m_nWinnerMarkPosCnt = 0;
            m_nWinnerMarkPos = ( m_nWinnerMarkPos + 1 ) % 6;    // �� 6 ���������� �����Ǿ� �ִ�.
        }

		//ī�� ���̽��� �������� �ٸ� �������� ���̸� ����� ������ ������ �̺κ��� ��� ���ư��� �ִ�.
		if ( m_bStartInit || m_bCardDump || m_bCardChoice ) {
			m_bCardChoice = FALSE;
			m_bStartInit = FALSE;
			m_bCardDump = FALSE;
			m_nCardChoiceCnt = 0;	
			m_nCardDumpCnt = 0;

			//m_bStartInit, m_bCardChoice �̰� Ʈ�� �λ��´� ī�� ���̽� ���¿��� ������ ����Ȱ��̴�.
			//�׷��� ������ ī����ġ�� �ٽ� ���ڸ��� ���´�.
			g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
			for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
				g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
				g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
			}			
		}

		//���� ��Ÿ�� �ð��۵�.
		if ( m_bUserCallTime ) {
			AllCallBtnUp();
		}

		//���â�� ���ڸ��� �¸��� ������
		if ( m_nEndingShowTime == 0 ) {

			g_pCMainFrame->m_bSelectionStart = FALSE;

			char pTemp[256];
            char szUserJokbo[256];
			memset( pTemp, 0x00, sizeof(pTemp) );
            memset( szUserJokbo, 0x00, sizeof(szUserJokbo) );

			//�������� ���� �ٽ� �������ش�.
			for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

				if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 && 
					 g_CGameUserList.m_GameUserInfo[i].m_biPureUserWinLoseMoney > 0 ) 
				{

					if ( g_nWinnerSlotNo != i ) {
						sprintf( pTemp, "�� %s���� : (-)%I64u��", 
						g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, g_CGameUserList.m_GameUserInfo[i].m_biPureUserWinLoseMoney );
                        SetGameChatText( pTemp , RGB( 185, 255, 102 ) );
					}
					else {
						sprintf( pTemp, "�� %s�Խ¸� : (+)%I64u��", 
						g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, g_CGameUserList.m_GameUserInfo[i].m_biPureUserWinLoseMoney );	
                        SetGameChatText( pTemp , RGB( 185, 255, 102 ) );
					}

					//���� �����ֱ�.
					if ( g_CGameUserList.m_GameUserInfo[i].m_nCardCnt >= 7 ) {
						JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, szUserJokbo );
						sprintf( pTemp, "�� %s�� ī�� : %s", g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, szUserJokbo );
                        SetGameChatText( pTemp , RGB( 100, 255, 50 ) );
					}
					else if ( g_CGameUserList.m_GameUserInfo[i].m_bDie ) {
						sprintf( pTemp, "�� %s�� ����", g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name );
                        SetGameChatText( pTemp , RGB( 100, 255, 50 ) );
					}
				}

				if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 && 
					 g_CGameUserList.m_GameUserInfo[i].m_biRealUserMoney > 0 )
                {
					g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney = g_CGameUserList.m_GameUserInfo[i].m_biRealUserMoney;
				}
			}

            SetGameChatText( "��---------------��" , RGB( 255, 255, 51 ));

			//���� �ٲٱ�.
			g_CGameUserList.NewSunSet( g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_sUserInfo.nUserNo );

			if ( g_nWinnerSlotNo == 0 ) 
				PlaySound(g_Sound[GS_WINNER]);
			else
				PlaySound(g_Sound[GS_LOSE]);
		}	
		
		m_nEndingShowTime++;
	
		if ( m_nEndingShowTime > ENDDING_TIME ) {

			//���� �ڵ� ����.
			if ( AvailableRoomIn( g_sRoomInfo.biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney, FALSE ) == FALSE ) {							

				//���� �ʱ�ȭ.
				GameInit();	

				g_pClient->RoomOut();
				g_pCMainFrame->m_Message->PutMessage( "�ּҸӴ� �������� ����Ǿ����ϴ�.", "Code - 500" );
			}

            else if ( g_CGameUserList.m_GameUserInfo[0].m_bExitReserve ) {

				g_pClient->RoomOut(); 

				//���� �ʱ�ȭ.
				GameInit();			
			}
			else {
				//���� �ʱ�ȭ.
				GameInit();
				g_pClient->GameReStart();
			}
		}
	}

    //----------------------------
	// ����ÿ� ������� ī�� ����
    //----------------------------

	if ( m_bEndding ) {
		//0.5��.
		//m_nEndingShowTime / 25;	// 0~24 : 0�� ����.
		m_nEndingUserCurSlot =  ( g_nCurMasterSlotNo + ( m_nEndingUserCurTime/25) ) % MAX_ROOM_IN;	//���� �����ͺ��� �����Ѵ�.
		m_nEndingUserCurTime++;

		//Ȯ�� ��.
		if ( m_nEndingUserCurTime >= 126 ) {	        //5���� ������ �ϹǷ�.
			m_bEndding = FALSE;
			m_bEnddingShow = TRUE;
            m_bMoveHiddenCard = FALSE;
		}

		if ( g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_nCardCnt > 0 && 
			 g_CGameUserList.m_GameUserInfo[m_nEndingUserCurSlot].m_bDie == FALSE ) 
		{
			if ( m_nEndingUserCurTime % 25 == 0 ) {     //ī�带 Ȯ �ɴ�.

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

					//x ��ǥ�� ���� �ٿ��ְ� 0��ī��� ���ԵǸ� 0��ī�� ��ġ�� ��� �����Ѵ�.
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
			//���³��̸�..�ٷ� �������� �ϱ� ���ؼ�.
			m_nEndingUserCurTime = ( ( m_nEndingUserCurTime/25) + 1 ) * 25;
		}
	}

    //-----------------------------------------------
    // ���� �ʱ�ȭ�� ��Ų��.
    //-----------------------------------------------
    if ( m_bGameInit ) {
		DTRACE2("[%s]�̺�Ʈ�ɰ� �ʱ�ȭ �����Ѵ�.", g_szUser_ID);

        m_bGameInit = FALSE;
        //���� �ʱ�ȭ.
		NetWorkThreaOnGameInit();		
    }

    //-----------------------------------------------
	// ������ �����Ѱ� ū�� �ѷ��ְ� ������ �ѷ��ֱ�.
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
	// ī���� �ִϸ��̼� ó��.
    //------------------------

	for( i = 0; i <= m_CAniMation.GetAniCount(); i++ ) // ī�� �̵� �ִϸ��̼� ó��
	{
		sUserCardInfo *card = m_CAniMation.GetCard(i);

        if ( card->nAniCount == 1 ) {
            PlaySound(g_Sound[GS_DEAL]);
        }

        if(card == NULL) {
            continue;
        }

		//DTRACE( "X : %d, Y : %d", card->Pos.x, card->Pos.y  );
		//���̸� ��������.
		if( CheckPosition(card) ) {

			card->Pos = card->NewPos; 
			card->ctCardType = USERCARD;

			card->nAniCount = 0;
			card->nAniMaxCount = -1;

			m_CAniMation.DeleteStack(i);
		}
		else {	//�̰� �־��ָ�..���ʴ�� ī�尡 ���ư���.
			break;
		}
	}

    //---------------------------------------------------
    // �� ���ϸ��̼� ó��
    //
    // ��� �̵��� ���� ���ŵǴ� ������ Ÿ���� �ٲ��,
    // ���ŵǴ� ���� ������ Ƚ���� �ٶ� ���� ���������鼭
    // �������.
    //---------------------------------------------------

    for ( i = 0; i <= m_ChipAni.GetAniCount(); i++ ) {

        sChipInfo *chip = m_ChipAni.GetChip( i );

        if ( chip == NULL || chip->ctChipType == CT_NONE ) {
            continue;
        }

        if ( chip->ctChipType == CT_MOVE ) {        // �̵����� �� ó��(�̵��� �Ϸ�Ǹ� ��������� �����Ѵ�)

            if (CheckChipPosition( chip )) {
                chip->Pos = chip->NewPos;
                chip->ctChipType = CT_REMOVE;
                chip->nAniCount = 0;
                chip->nAniMaxCount = -1;
            }
        }
        else if ( chip->ctChipType == CT_REMOVE ) { // ������� ����� ī���͸� ������Ű�ٰ� �ð��� �Ǹ� �����Ѵ�.
            chip->nRemoveCnt++;

            if ( chip->nRemoveCnt >= REMOVE_CHIP_TIME ) {

                m_ChipAni.DeleteStack( i );

                if ( m_ChipAni.GetAniCount() <= 0 ) {   // ���� �� �̵������� �� �� ����
                    PanMoneyReFresh();
                }
            }
        }
    }
}



//========================================================================
// ī�� ��ġ ����
//========================================================================

BOOL CMainFrame::CheckPosition( sUserCardInfo *pos )    // pos->NewPos�� pos->Pos�� �̵�
{
	double w = pos->NewPos.x - pos->StartPos.x;
	double h = pos->NewPos.y - pos->StartPos.y;

	//�ӵ�	//Ŀ���� õõ��. ��
	pos->nAniMaxCount = g_nGameSpeed;
	
	pos->Pos.x = pos->StartPos.x + (int)((double)(w / pos->nAniMaxCount * pos->nAniCount));
	pos->Pos.y = pos->StartPos.y + (int)((double)(h / pos->nAniMaxCount * pos->nAniCount));
	
	pos->nAniCount++;

	if(pos->nAniCount >= pos->nAniMaxCount) return TRUE;
	return FALSE;
}



//========================================================================
// �� ��ġ ����
//========================================================================

BOOL CMainFrame::CheckChipPosition( sChipInfo *pos )    // pos->NewPos�� pos->Pos�� �̵�
{
	double w = pos->NewPos.x - pos->StartPos.x;
	double h = pos->NewPos.y - pos->StartPos.y;

	//�ӵ�	//Ŀ���� õõ��. ��
	pos->nAniMaxCount = g_nGameSpeed;
	
	pos->Pos.x = pos->StartPos.x + (int)((double)(w / pos->nAniMaxCount * pos->nAniCount));
	pos->Pos.y = pos->StartPos.y + (int)((double)(h / pos->nAniMaxCount * pos->nAniCount));
	
	pos->nAniCount++;

	if(pos->nAniCount >= pos->nAniMaxCount) return TRUE;
	return FALSE;
}



//========================================================================
// ���ϸ��̼� ������Ʈ ������
//========================================================================

unsigned __stdcall AniUpdateThreadProc( LPVOID lParameter ) // ham 2005.11.29
{
	//�̹�Ÿ ������ m_bActive �̰� FALSE�� �Ǵ� ������ �ִ�.
	while( g_pCMainFrame->m_bAniActive ) {
		g_pCMainFrame->m_fSkipTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

		if( g_pCMainFrame->m_bAniActive ) {
			if( g_pCMainFrame->m_FrameSkip.Update( g_pCMainFrame->m_fSkipTime ) ) {
                g_pCMainFrame->AnimationUpdate();
			}
        }

        Sleep(5);
	}

    DTRACE2( "[%s] ���ϸ��̼� �����尡 ����Ǿ���.", g_szUser_ID );

    _endthreadex( 0 );
	return 0;
}



//========================================================================
// ���� �Է¿� IME �����ϰ� �ʱ�ȭ
//========================================================================

void CMainFrame::LoadControl( void )
{
	//////////////////////////////////////////////////////////////////////////			
	// ������ ä�� ��Ʈ���� �����.
	//////////////////////////////////////////////////////////////////////////
	
    // ���� ä��â �޽��� �Է¿� IME �ʱ�ȭ
    m_IME_WaitChat  = new CDXIME( g_pCDXScreen, g_hWnd, 98, TRUE, FALSE );
    m_IME_WaitChat->InitCaret();

    // ����ȭ�� ä��â �޽��� �Է¿� IME �ʱ�ȭ
    m_IME_GameChat  = new CDXIME( g_pCDXScreen, g_hWnd, 50, TRUE, FALSE );
    m_IME_GameChat->InitCaret();

    // �游��� - ��й� ���鶧 ��й�ȣ �Է¿� IME �ʱ�ȭ
    m_IME_InputPass = new CDXIME( g_pCDXScreen, g_hWnd, 4, TRUE, FALSE );
    m_IME_InputPass->SetTextLen( 4 );           // �Է°����� ���� ��
    m_IME_InputPass->InitCaret();               // Ŀ�� �ʱ�ȭ
    m_IME_InputPass->SetPassword( true );       // ��й�ȣ �Է¸��� ����
    m_IME_InputPass->SetPasswordChar( '*' );    // ��й�ȣ ��¿� ���� ���� : �⺻���� '*'

    // �游��� - ������ �Է¿� IME �ʱ�ȭ
    m_IME_InputRoomTitle = new CDXIME( g_pCDXScreen, g_hWnd, MAX_ROOM_NAME - 1, TRUE, FALSE );
    m_IME_InputRoomTitle->SetTextLen( MAX_ROOM_NAME - 1 );
    m_IME_InputRoomTitle->InitCaret();

    // �� ����� ��й�ȣ �Է¿� IME �ʱ�ȭ
    m_IME_RoomInPassBox = new CDXIME( g_pCDXScreen, g_hWnd, 4, TRUE, FALSE );
    m_IME_RoomInPassBox->SetTextLen( 4 );
    m_IME_RoomInPassBox->InitCaret();
    m_IME_RoomInPassBox->SetPassword ( true );
    m_IME_RoomInPassBox->SetPasswordChar( '*' );

    // ���� �Է¿� IME �ʱ�ȭ
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


    // ���� ä��â ��ũ�� ��
    
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


    // ����� ��ũ�� ��

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


    // �� ����Ʈ ��ũ�� ��

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


    // ����ȭ�� ä��â ��ũ�� ��

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

    // �ʴ� â ��ũ�� ��

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
// ��� �̹����� �÷�Ű�� �����Ѵ�.
//
// ȭ�� ��尡 ����Ǹ� �� �Լ��� ȣ���� �־�� �Ѵ�.
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

    // FillRect() �Լ����� ����ϱ� ���� ������, ���� ȭ�� ��忡 �°� ���Ѵ�.
    GetFillRectColor( pixelFormat.bpp );
}



//========================================================================
// FillRect() �Լ����� ����ϱ� ���� ������, ���� ȭ�� ��忡 �°� ���Ѵ�.
//
// NOTE! - FillRect() �Լ����� ����ϴ� ������ R, G, B ������ �ƴ϶�
//         B, G, R �����̴�. RGB( Blue, Green, Red ).. �̷������� ���־��
//         �Ѵ�.
//
// input:
//      bpp:    ���� ȭ�� ����� �ȼ� ����( 8, 15, 16, 24, 32 )
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
            // FillRect() �Լ����� ����ϴ� ���򿡼�... �ٸ� ���� Blue, Green, Red �����ε����ؼ�,
            // 16bit 565 ��忡���� Red, Green, Blue �����̴�.
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
        
        // ������ ���� ���� ���ڿ��� ����
        memset( m_szUserAddMoney[nUserSlot], 0x00, sizeof( m_szUserAddMoney[nUserSlot] ));
        I64toA_Money( nTempMoney, m_szUserAddMoney[nUserSlot] );

        if ( bThrow ) {
            // ���� ȭ�� ����� ������.
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
	char szTemp[30];                    // ���ڿ��� ��ȯ�� �� �Ӵ�

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
			//���� ��ư���� ��ġ�� ���콺�� �ִ��� üũ�ϰ� ���콺�� ������ �ٿ��� �����ش�.
			//DTRACE( "OnLButtonDown" );
			OnLButtonDown();
			
		}
	}

    g_pCDXInput->Get_MouseState(); 

	if( g_pCDXInput->MOUSE_LBUP() ) {
		if ( m_bMouseEvent ) {
			m_bMouseDown = FALSE;
            ::SetFocus( g_hWnd );       // CHttpView�� ���� Ű �Է� �ȵǴ� ���� ����
			//DTRACE( "OnLButtonUp" );
			OnLButtonUp();
			
		}
	}

    g_pCDXScreen->GetBack()->Fill(0);

	switch( m_nCurrentMode )
		{
		case LOGO_MODE:         // ó�� ����Ǿ��� ���� ȭ��
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

		case WAITROOM_MODE:     // �����̸�
			RenderWaitRoom();
            
            if ( g_bConnectingSpr ) {
                m_SPR_Connting->SetPos( 362, 354 );
                m_SPR_Connting->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }

			break;

		case GAME_MODE:         // ����ȭ���̸�
			RenderGameBase();

            if ( g_bConnectingSpr ) {
                m_SPR_Connting->SetPos( 362, 354 );
                m_SPR_Connting->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }
			
			break;
	}

	if ( m_bShowMessageBox ) {      // ����
		DrawMessageBox( MEMO_DLG_POS_X, MEMO_DLG_POS_Y );
	}

    if ( m_bFullScreen ) {
        m_MaxBTN2->DrawButton();    // �ִ�ȭ ��ư ���
    }
    else {
        m_MaxBTN->DrawButton();
    }

	m_ExitBTN->DrawButton();        // ���� ��ư ���
	m_ConfigBTN->DrawButton();      // ���� ��ư�� �׸���.
	m_Exit2BTN->DrawButton();       // ������ ��ư�� �׸���.
    m_CaptureBTN->DrawButton();     // ȭ�� ĸ�� ��ư�� �׸���.
    m_MinBTN->DrawButton();         // â �ּ�ȭ ��ư�� �׸���.

	if( m_bShowConfigBox ) {        // �ɼ�
		m_pConfigBox->RenderConfigBox();
	}

    if( m_Message->GetbShowCheck() ) {
        m_Message->Draw();
    }

	if ( m_bMouseRender ) {         // ���콺 Ŀ�� ���
        if( m_bMouseDown ) m_SPR_Cursor->SetFrame( 1 );
        else               m_SPR_Cursor->SetFrame( 0 );
        m_SPR_Cursor->SetPos( m_mX, m_mY );
        m_SPR_Cursor->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}
	
    g_pCDXScreen->Flip(FALSE, FALSE, FALSE, TRUE);      // Blt�� ȭ�鿡 �׸���.
}



void CMainFrame::CreateRoom()
{
    OpenMakeRoomDlg();      // �游��� â�� ����.
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
	//������ Ŭ��������..���ֱ�
	//������ �ؾ��Ҷ� �����ϱ�.

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
	
	//���� �ϳ� ����
	char szTemp[2] = { 0, };
	int nOneMeth = 0;	

	//�ִϸ� �Ѵ�.
	if ( m_nJackPotStopCnt < m_nJackPotLen ) {
        m_SPR_JackPotMath->SetFrame((((m_nJackPotPos % 3) + 1) * 12)   +   (m_nJackPotPos / 3));
        m_SPR_JackPotMath->SetPos( m_JakcPotMathPoint[m_nJackPotStopCnt].x, m_JakcPotMathPoint[m_nJackPotStopCnt].y );
        m_SPR_JackPotMath->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		//m_nJackPotPos : �ѹ��� �ִϸ��̼�, m_nJackPotStopCnt : �ѹ������ѹ�����
		m_nJackPotPos++;

		if ( m_nJackPotPos > 29 ) {
			m_nJackPotPos = 0;
			m_nJackPotStopCnt++;
		}
	}

	//�ִϰ� ���� ���ڸ� ��´�.
	int nJackPotLen = m_nJackPotLen;
		
	for ( int i = 0; i < m_nJackPotStopCnt; i++ ) {
		nJackPotLen--;

		//���� �ϳ� ����
		szTemp[0] = m_szJackPotMoney[nJackPotLen];
		nOneMeth = atoi(szTemp);

        m_SPR_JackPotMath->SetFrame( nOneMeth );
        m_SPR_JackPotMath->SetPos( m_JakcPotMathPoint[i].x, m_JakcPotMathPoint[i].y );
        m_SPR_JackPotMath->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}

	//�ִϰ� ������ ���� ���� �޸��� ��´�.
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

	//�߰��� �������� �ֱ� ������.
	if ( g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_sUserInfo.nUserNo > 0 &&
		 g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_nCardCnt > 0 )
    {
        // �¸��� ������ ��ġ�� "Win" �̹��� ���
        RenderWinnerMark();
    }

    // ���â ���
    PutSprite( m_SPR_WindowWin, 360, 284 );

    // �¸����� �г���
	NickSavePrint( g_sWinnerCardInfo.szNick_Name, 482, 316 );	

	// �¸����� �̱� ��.
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

	//�¸��� ����.
	JokboShow( g_sWinnerCardInfo.szJokboCard, m_szTempText );
	
    if ( g_szResultCmt[0] == NULL ) {
        DrawText( m_szTempText, 512, 458, RGB( 0, 252, 255 ), FW_NORMAL, TA_CENTER );
    }
	else {
		sprintf( m_szTempText2, "(%s)", g_szResultCmt );
        DrawText( m_szTempText2, 512, 458, RGB( 0, 252, 255 ), FW_NORMAL, TA_CENTER );
	}

	//������ �����̸�.
	if ( m_bJackPot ) {
		m_SPR_JackPot->SetPos( 412, 220 );
        m_SPR_JackPot->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		I64toA_Money( g_biUserJackPotMoney, m_szTempText );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 509, 258, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}
}



//========================================================================
// "WIN" �̹��� ���
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
	if( nCardNo >= MAX_CARD ) {	        //�޸��̸� 255�̱� ������.
        m_SPR_CardBack->SetPos( x, y );
        m_SPR_CardBack->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		return;
	}

    int nFrame = 0;

	if ( bUp ) nFrame = (nCardNo % 13);        // �Ϲ� ī��
	else       nFrame = 13 + (nCardNo % 13);   // ��ο� ī��

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
	//�ð� �۵� ����.
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
	//�ð� Ÿ�� ����.
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



void CMainFrame::UserCardAdd( BYTE nCardNo , int nUserArrayNo, BOOL bAniMation ) // �ѿ��� �������� �� ������
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
    // ���� ȭ�� ��� �̹���
	m_SPR_GameBase->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );

    // ���� ��� �̹���
    PutSpriteBlk( m_SPR_JackPotBack, 16, 27, m_nJackpotBackFrm );

    // ä��â�� �׸���.
    RenderGameChat();

	// ���ӹ濡�� ��ư��..
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

    // �׿��ִ� ���� �׸���.
    if ( m_bRenderPanMoney ) {
        RenderPanMoney();
    }

	// ����� ������ �Ѹ���.
	RenderUserCard();

    // Ÿ�̸Ӹ� �׸���.
    if ( m_bUserCallTime && ( m_nUserCallDefineTime - m_nUserCallTime ) <= 500 ) {
        RenderTimePos();
    }

    //���� ����.    
    RenderUserInfo();

#ifndef SHOWUSERCARD
    // ����Ű �̹���
    if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt >= MAX_USER_CARD ) {
        m_SPR_Enter->SetPos( 627, 687 );
        m_SPR_Enter->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
    }

    // �̵����� ���� ī�带 �׸���.
    if ( m_bMoveHiddenCard ) {
        PutCardImage( m_sHiddenCardInfo.Pos.x, m_sHiddenCardInfo.Pos.y, 255 );
    }
#endif

    // �� �����̵� �׸���
    RenderThrowChip();      // chonkw  ������ ��ư(����, ���� ��)�� ������ ���� ���� �׸���.










    // �̵����� ī�� �׸���. ���徿 �Ѹ������� ���ÿ� �ִ°͸� �Ѹ���.
	sUserCardInfo *card = m_CAniMation.GetCard(0);
    if ( card != NULL ) {
        PutCardImage( card->Pos.x, card->Pos.y, card->nCardNo );
    }

    // ���� ī�峪 ������ ī�带 ���� ��ȭ���ڸ� �����ش�.
    RenderCardSelectionDlg();

    // ��� ȭ�� �׸���
    if ( m_bEnddingShow ) {
        RenderWinner();
    }

    // ����
    if ( m_bRoomJackPotRender ) {
        RenderRoomJackPot();
    }

    // ���� ���� â
	if ( m_bShowUserInfo )
		RenderShowUserInfo();



	// ���� ��ư�� �Ѹ���.
	if( m_bShowStartBTN ) {		
		m_StartBTN->DrawButton(); // ���� ��ư ���	
		
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

	if( m_bShowInviteBox ) {    // �ʴ� �ڽ��� �׷�����
		RenderInviteBox();
	}
/*
	memset( m_szTempText, 0x00, sizeof(m_szTempText) );
	sprintf( m_szTempText , "��Ŀ[%dä��]" , g_szGameServerChannel + 1 );
    DrawText( m_szTempText, 13, 6, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );
*/	
	sprintf( m_szTempText , "%d����  ���� - %s   [�⺻ : %d��]" , g_sRoomInfo.nRoomNo + 1, g_sRoomInfo.szRoomName, g_sRoomInfo.biRoomMoney );	
    DrawText( m_szTempText, 300, 6, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );
}



//========================================================================
// ���� �г��� 10����Ʈ�� ������ �ٿ��� �����Ѵ�.
//========================================================================

void CMainFrame::NickSave( char *pShortNick, char *pNickName )
{
	// ����
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
// ���� �г��� 10����Ʈ�� ������ �ٿ���´�
//========================================================================

void CMainFrame::NickSavePrint( char *pNickName, int x, int y, COLORREF rgbCol )
{
	char szTemp[255];
	memset( szTemp, 0x00, sizeof(szTemp) );

	// ����
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
	//�������̰ų� ī�����̽����� ȥ�� ���������.
    if ( cUserSelect == '8' ) {
        return;
    }

	//�ð� �ִϸ��̼� �ʱ�ȭ.
	m_nUserCallTime = 0;
	m_bUserCallTime = FALSE;
	m_nTemp         = 0;
	m_nTimePos      = 0;

	//������ ���� �ߴٴ°��� �����س�ƾ� �߰��� ���� ������� ���ذ���. ������.
    if ( cUserSelect == '7' ) {
        g_CGameUserList.m_GameUserInfo[nSlotNo].m_bDie = TRUE;
    }

	// '0':����, '1':üũ, '2':����, '3':��, '4':����, '5':��, '6':Ǯ, '7':����
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
		//�ջ���� Ǯ�߳�
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

	// ����
	if ( strlen( g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.szNick_Name ) > 10 ) {
		memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
		memcpy( g_szTempStr, g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.szNick_Name, 10 );
		strcat( g_szTempStr, "..." );
        DrawText( g_szTempStr, 559, 344, RGB( 0 , 0, 0 ), FW_NORMAL, TA_LEFT );
	}
	else {
        DrawText( g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.szNick_Name, 559, 344, RGB( 0 , 0 , 0 ), FW_NORMAL, TA_LEFT );
	}

	// �ݾ�
	I64toA_Money(  g_CGameUserList.m_GameUserInfo[m_nShowUserSlot].m_sUserInfo.biUserMoney, m_szTempText, 1 );
    DrawText( m_szTempText, 559, 368, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
	
	// ���
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

	// ����
	sprintf( m_szTempText , "%d�� %d��" , nAllCnt, nWinCnt );
    DrawText( m_szTempText, 559, 416, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

	sprintf( m_szTempText , "%d�� %d��" , nLoseCnt, nDrawCnt );
    DrawText( m_szTempText, 559, 442, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

	PutSpriteAvatar( 392, 333, m_SPR_GameAvatar[m_nShowUserSlot], m_GameAvaImg[m_nShowUserSlot], ORIGINAL_SIZE );
}



//========================================================================
// �׿� �ִ� ���� �׸���.
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

        if ( nSlotNo >= 3 ) nUserSelect += 8;   // 0, 1, 2�� ������ ` ���.... 3, 4�� ������ ' ���
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
	//�ǵ� ���. ( �̰͸� Ǯ�̴�. )
	I64toA_Money( g_biPanMoney, m_szTempText );
    DrawText( m_szTempText, 670, 458, RGB( 241, 178, 9 ), FW_BOLD, TA_RIGHT, 16 );

    // ���� �� �Ӵ� ��� - ham 2005.09.26
    I64toA_Money( g_biCallMoney, m_szTempText );
    DrawText( m_szTempText, 670, 492, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT, 16 );

	//ó�� ī�带 �����ְ� ���� ȭ�� 
	int i = 0;
	int nSunArrayNo = FindSunArrayNo();
	int nUserArrayNo = 0;
	BYTE nCardNo = 0;

	//����� ��� �������� ���徿 ���ʴ�� ������ �ϱ� ������.
	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		nUserArrayNo = ( nSunArrayNo + i ) % MAX_ROOM_IN;

		//ī�尡 �������� ���� ��´�.
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.nUserNo > 0 ) {
            // ���õ� ���� ���� �׵θ� ���
            if ( m_nUserCallSlotNo == nUserArrayNo ) {
                m_SPR_SelectUser->SetPos( m_sRoomInUserPosition[nUserArrayNo].pCardInfo.x, m_sRoomInUserPosition[nUserArrayNo].pCardInfo.y );
                m_SPR_SelectUser->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
            }

            I64toA_Money( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.biUserMoney, m_szTempText );
            DrawText( m_szTempText, m_sRoomInUserPosition[nUserArrayNo].pUserMoneyPos.x, m_sRoomInUserPosition[nUserArrayNo].pUserMoneyPos.y, RGB( 255, 255, 0 ), FW_NORMAL, TA_RIGHT );
		}
        
		//���� ī����� ����ش�.
		for ( int j = 0; j < g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt; j++ )
        {
			if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].ctCardType == USERCARD )
            {
#ifdef SHOWUSERCARD
                nCardNo = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].nCardNo;
#else
				//�ڱ� �ڽ��϶��� ī�带 �����ְ�
				if ( nUserArrayNo == 0 ) {
					nCardNo = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].nCardNo;
				}
				else {
					//���� ī��� �ڽ��� ���̽��� ���� ���� ���̵��� �Ѵ�.
                    if ( g_CGameUserList.m_GameUserInfo[0].m_bChoioce == TRUE ) {
                        nCardNo = g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[j].nCardNo;		
                    }
                    else {
                        nCardNo = 255;
                    }
				}
#endif

				//ī�带 �� �޾Ұ� ������ ���� �����̸�. ���â�� ��� ���ÿ�.
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
					//��ī�� �϶� ó�� ������ ��ο� ī��
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
        


		//��
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bSun == TRUE &&
			 g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nCardCnt > 0 ) 
		{
			m_SPR_Sun->SetPos( m_sRoomInUserPosition[nUserArrayNo].pBossPos.x, m_sRoomInUserPosition[nUserArrayNo].pBossPos.y );
            m_SPR_Sun->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		}


		//����, ����(��,����)���� ���.
		if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.nUserNo > 0 ) {
			//����
			if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bBBangJjang == TRUE ) {
				m_SPR_BangJang->SetPos( m_sRoomInUserPosition[nUserArrayNo].pBangJangPos.x, m_sRoomInUserPosition[nUserArrayNo].pBangJangPos.y );
                m_SPR_BangJang->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			}

            //Ÿ�̸ӿ� �������� ������. ���� ����� ���⿡�� ���...ū ����� Ÿ�̸� ��� ���� ��´�.
   			if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bUserSelectShow ) {
                if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_nUserSelectCnt <= 0 ) {
                    UserSelectPutSprite( nUserArrayNo, g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_cUserSelct, FALSE );
                    DrawText( m_szUserAddMoney[nUserArrayNo], m_sRoomInUserPosition[nUserArrayNo].pUserSelectSmallPos.x - 3, m_sRoomInUserPosition[nUserArrayNo].pUserSelectSmallPos.y + 2, RGB( 153, 255, 255 ), FW_BOLD, TA_RIGHT );
                }
			}
		}

#ifndef SHOWUSERCARD
        // ���� ī�带 �̵��� �� �ִٴ� ������ ȭ��ǥ�� ǥ�����ش�.

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
	
	//���� ������ �ѷ��� �ϴ����� �Ǵ�.
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo > 0 ) {
			//���ξƹ�Ÿ
			PutSpriteAvatar( m_sRoomInUserPosition[i].pAvatarPos.x, m_sRoomInUserPosition[i].pAvatarPos.y, m_SPR_GameAvatar[i], m_GameAvaImg[i] );

            // Ŭ����
            PutSprite( m_SPR_Class, m_sRoomInUserPosition[i].pNickPos.x - 18, m_sRoomInUserPosition[i].pNickPos.y - 2, g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nClass );

			//���δг���.
			NickSavePrint( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.szNick_Name, m_sRoomInUserPosition[i].pNickPos.x , m_sRoomInUserPosition[i].pNickPos.y, RGB( 255, 255, 0 ) );
	
#ifdef SHOWUSERCARD
            JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
            if ( i == 0 ) {
                // ������ �� ���� ������ ���� �ڽ��� ���� ǥ��
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_CENTER );
            }
            else {
                // ������ ������ ���� �ٸ� ������ ���� ǥ��
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
                DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
            }
#else
			// �ڽſ��� ī�尡 ������ �ٸ� ������ ������ �����Ѵ�.
			if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt > 0 ) {
				if ( g_CGameUserList.m_GameUserInfo[i].m_bEnd && g_CGameUserList.m_GameUserInfo[i].m_nCardCnt > 0  ) { 
					JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
                    if ( i == 0 ) {
                        // ������ �� ���� ������ ���� �ڽ��� ���� ǥ��
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_CENTER );
                    }
                    else {
                        // ������ ������ ���� �ٸ� ������ ���� ǥ��
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
                        DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
                    }
				}
			}

            // �߰��� ���ͼ� ī�尡 ���� ����� �ٸ������ ������ ������ �Ѵ�. ��, ���������� �����Ѵ�.
			else {
				if ( g_CGameUserList.m_GameUserInfo[i].m_nCardCnt == 7 && g_CGameUserList.m_GameUserInfo[i].m_bEnd && g_CGameUserList.m_GameUserInfo[i].m_nCardCnt > 0  ) { 
					JokboShow( g_CGameUserList.m_GameUserInfo[i].m_szJokboCard, m_szTempText );
                    DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x + 1, m_sRoomInUserPosition[i].pJokboPos.y + 1, RGB( 0, 0, 0 ), FW_BOLD, TA_RIGHT );
					DrawText( m_szTempText, m_sRoomInUserPosition[i].pJokboPos.x, m_sRoomInUserPosition[i].pJokboPos.y, RGB( 255, 255, 255 ), FW_BOLD, TA_RIGHT );
				}
			}
#endif

			//���� ��ũ ǥ��. ī�� ī��Ʈ�� �������� ������ ������ Ÿ������ ������ �Ǿ �濡�� �Ѱܳ��� m_ballin�� false�� �Ǿ �Ҹ��� �ѹ�������.
			if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney <= 0 && g_CGameUserList.m_GameUserInfo[i].m_nCardCnt > 0 ) {
				m_SPR_AllIn->SetPos( m_sRoomInUserPosition[i].pAllInPos.x, m_sRoomInUserPosition[i].pAllInPos.y );
                m_SPR_AllIn->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

				if ( g_CGameUserList.m_GameUserInfo[i].m_bAllin == FALSE ) {
					PlaySound(g_Sound[GS_ALLIN]);
					g_CGameUserList.m_GameUserInfo[i].m_bAllin = TRUE;
				}
			}



            //���� call big markǥ��.
			if ( g_CGameUserList.m_GameUserInfo[i].m_bUserSelectShow ) {
                if ( g_CGameUserList.m_GameUserInfo[i].m_nUserSelectCnt > 0 ) {
                    UserSelectPutSprite( i, g_CGameUserList.m_GameUserInfo[i].m_cUserSelct );
                }
			}

			//���� ���϶��� Ÿ ���� ����ǥ��.
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

	//��ư���� ���콺 �÷����� �ش�ݾ� ����ֱ�.

    // ���� ��� ��ư ���� �ɾ��� ���� �ݾ��� ��µǵ��� ����
    /* [*BTN]
    // ����
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


    // ����
	if ( m_Quater->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_Quater->GetPosX(), m_Quater->GetPosY() - 23 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biQuater, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 850, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}

    // ��
    if ( m_BBing->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_BBing->GetPosX(), m_BBing->GetPosY() - 23 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biBing, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 925, 568, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}
			
    // ����
	if ( m_Half->status == UP ) {
        m_SPR_BetMoneyBack->SetPos( m_Half->GetPosX(), m_Half->GetPosY() + 47 );
        m_SPR_BetMoneyBack->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		I64toA_Money( m_biHalf, m_szTempText, 1 );
		sprintf( m_szTempText2, "%s", m_szTempText );
        DrawText( m_szTempText2, 775, 688, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	}

    // ��
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
			strcpy( szTemp, "�ο���Ʈ����Ʈ �÷���" );
			break;

		case 10:
			strcpy( szTemp, "��Ʈ����Ʈ �÷���" );
			break;

		case 9:
			strcpy( szTemp, "��ī��" );
			break;

		case 8:
			strcpy( szTemp, "Ǯ�Ͽ콺" );
			break;

		case 7:
			strcpy( szTemp, "�÷���" );
			break;

		case 6:
			strcpy( szTemp, "����ƾ" );
			break;

		case 5:
			strcpy( szTemp, "����Ʈ����Ʈ" );
			break;

		case 4:
			strcpy( szTemp, "��Ʈ����Ʈ" );
			break;

		case 3:
			strcpy( szTemp, "Ʈ����" );
			break;

		case 2:
			strcpy( szTemp, "�����" );
			break;

		case 1:
			strcpy( szTemp, "�����" );
			break;

		case 0:
			strcpy( szTemp, "ž" );
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
			strcpy( szTemp, "�ο���Ʈ����Ʈ �÷���" );
			break;

		case 10:
			strcpy( szTemp, "��Ʈ����Ʈ �÷���" );
			break;

		case 9:
			strcpy( szTemp, "��ī��" );
			break;

		case 8:
			strcpy( szTemp, "Ǯ�Ͽ콺" );
			break;

		case 7:
			strcpy( szTemp, "�÷���" );
			break;

		case 6:
			strcpy( szTemp, "����ƾ" );
			break;

		case 5:
			strcpy( szTemp, "����Ʈ����Ʈ" );
			break;

		case 4:
			strcpy( szTemp, "��Ʈ����Ʈ" );
			break;

		case 3:
			strcpy( szTemp, "Ʈ����" );
			break;

		case 2:
			strcpy( szTemp, "�����" );
			break;

		case 1:
			strcpy( szTemp, "�����" );
			break;

		case 0:
			strcpy( szTemp, "ž" );
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
    m_SCB_WaitRoom->Draw();                     // �� ����Ʈ ��ũ�� ��

    if (m_nUserListTitleNum == 0 || m_nUserListTitleNum == 1) {
        m_SPR_UserListTitle[m_nUserListTitleNum]->SetPos( 698, 160 );
        m_SPR_UserListTitle[m_nUserListTitleNum]->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
    }

    RenderWaitChat();                           // ���� ä��â

    if ( m_nUserListTitleNum == 0 ) {           // ����� ��Ͽ����� ��ũ�ѹ� ���. ������ ����� ���� ���� ��� �ʿ����.
        m_SCB_WaitUser->Draw();
    }

    if ( m_nHighlightBarNum != -1 ) {
        m_SPR_HighlightBar->SetPos(713, 231 + (m_nHighlightBarNum * 25));
        m_SPR_HighlightBar->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
    }

    if ( g_nWaitRoomSelIdx != -1 ) {            // ���õ� �� ��� ǥ��
        int nStartElem = m_SCB_WaitRoom->GetStartElem();

        if ( g_nWaitRoomSelIdx >= nStartElem &&
             g_nWaitRoomSelIdx <  nStartElem + MAX_VIEW_WAIT_ROOM)
        {
            m_SPR_SelRoomBar->SetPos(35, 208 + (((g_nWaitRoomSelIdx - nStartElem) % MAX_VIEW_WAIT_ROOM) * 33));
            m_SPR_SelRoomBar->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
        }
    }

    if ( m_pScrollNotice != NULL ) {
        m_pScrollNotice->Draw();                    // ��ũ�ѵǴ� ��������
    }
    
    //========================================================================
	//���ǿ��� �渮��Ʈ �ѷ��ֱ�.
	//========================================================================

	BOOL bBoldType = FALSE;
	int nRoomInCurCnt = 0;
	int nUserNo = 0;
    int nRoomNo = 0;
    int nRoomViewCnt = 0;
    int i = 0;
    int j = 0;
    int nYPos = 217;                            // �ؽ�Ʈ�� �̹��� ����� ���� ��ǥ

    nRoomViewCnt = 0;
    
	for ( i = m_SCB_WaitRoom->GetStartElem(); i < g_nWaitRoomCnt; i++ ) {
        nRoomNo = g_nWaitRoomIdx[i];

        if ( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].nCurCnt <= 0 ) {
            continue;
        }

        // ���� ���� ���� ------------------------------------------------

		nRoomInCurCnt = g_CWaitRoomList.m_sWaitRoomList[nRoomNo].nCurCnt;

		if( nRoomInCurCnt >= MAX_ROOM_IN ) {    //���� FULL �̸� FULL ��ũ ����ش�.
            m_SPR_FullWait->SetFrame( 0 );
			m_SPR_FullWait->SetPos( 69 , nYPos - 5 );
            m_SPR_FullWait->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			bBoldType = FALSE;
		}
		else {                                  //���� FULL �� �ƴϸ� �� ��ũ ����ش�.
            m_SPR_FullWait->SetFrame( 1 );
			m_SPR_FullWait->SetPos( 69 , nYPos - 5 );
            m_SPR_FullWait->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			bBoldType = TRUE;
		}


        // �� ��ȣ -------------------------------------------------------
        
        sprintf( m_szTempText, "%03d", nRoomNo + 1);

        if ( bBoldType ) {
            DrawText( m_szTempText, 132 , nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_LEFT );
        }
        else {
            DrawText( m_szTempText, 132 , nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_LEFT );
        }


		// �� �̸� -------------------------------------------------------

        if ( bBoldType ) {
            DrawText( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].szRoomName, 250 , nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_LEFT );
        }
        else {
            DrawText( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].szRoomName, 250 , nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_LEFT );
        }


		// �� �Ӵ� -------------------------------------------------------

		sprintf( m_szTempText, "%d", g_CWaitRoomList.m_sWaitRoomList[nRoomNo].biRoomMoney );

        if ( bBoldType ) {
            DrawText( m_szTempText, 501 , nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_CENTER );
        }
        else {
            DrawText( m_szTempText, 501 , nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_CENTER );
        }


		// �� �ο� -------------------------------------------------------
        
		sprintf( m_szTempText , "%d/%d" , g_CWaitRoomList.m_sWaitRoomList[nRoomNo].nCurCnt, MAX_ROOM_IN );

        if ( bBoldType ) {
            DrawText( m_szTempText, 593, nYPos, RGB( 255 , 255 , 255 ), FW_BOLD, TA_LEFT );
        }
        else {
            DrawText( m_szTempText, 593, nYPos, RGB( 255 , 255 , 255 ), FW_NORMAL, TA_LEFT );
        }


        // ��й� ���� ---------------------------------------------------

        if ( g_CWaitRoomList.m_sWaitRoomList[nRoomNo].bSecret ) {
            m_SPR_FullWait->SetFrame( 3 );
        }
        else {
            m_SPR_FullWait->SetFrame( 2 );
        }

        m_SPR_FullWait->SetPos( 622 , nYPos - 3 );
        m_SPR_FullWait->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

        //----------------------------------------------------------------
        

        nYPos += 33;                                // ������ ����� ���� Y ��ǥ �̵�
        nRoomViewCnt++;

        if ( nRoomViewCnt >= MAX_VIEW_WAIT_ROOM ) { // �� ȭ�鿡 ���̴� ��ŭ�� ����Ѵ�.
            break;
        }
	}


    //========================================================================
    // ���� ���� ����Ʈ�� �ѷ��ش�.
    //========================================================================

    int nTempClass = 0;
    nYPos = 0;

    if ( m_nUserListTitleNum == 0 ) {
	    nRoomViewCnt = 0;
        nYPos = 233;                                // ���� ���� ����Ʈ�� ù��° ���� Y ��ǥ

	    for( i = m_SCB_WaitUser->GetStartElem(); i < MAX_CHANNELPER; i++ ) {
            if ( g_CUserList.m_WaitUserInfo[i].m_bUser == false ) {
                continue;
            }

            // ��� ------------------------------------------------------
            
            nTempClass = g_CUserList.m_WaitUserInfo[i].m_sUserInfo.nClass;

            m_SPR_Class->SetFrame( nTempClass );
		    m_SPR_Class->SetPos( 729 , nYPos + 2 );
            m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );


            // ���� ------------------------------------------------------
            
		    if( g_CUserList.m_WaitUserInfo[i].m_sUserInfo.cSex == '0' ) {   // ����
                m_SRP_SexIcon->SetFrame( 1 );
			    m_SRP_SexIcon->SetPos( 776, nYPos );
                m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		    }
		    else {                                                          // ����
                m_SRP_SexIcon->SetFrame( 0 );
			    m_SRP_SexIcon->SetPos( 776 , nYPos );
                m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
		    }


            // �г��� ----------------------------------------------------

            NickSave( m_szTempText, g_CUserList.m_WaitUserInfo[i].m_sUserInfo.szNick_Name );

            if ( (i - m_SCB_WaitUser->GetStartElem()) == m_nHighlightBarNum ) {
                DrawText( m_szTempText, 850, nYPos + 5, RGB(5, 26, 51), FW_NORMAL, TA_CENTER );
            }
            else {
                DrawText( m_szTempText, 850, nYPos + 5, RGB( 172 , 246 , 243 ), FW_NORMAL, TA_CENTER );
            }

            // �ʴ� ���� ���� ��� ---------------------------------------

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
    // ���ǿ��� ���õ� ���� ���� ����Ʈ
    //========================================================================

    else if (m_nUserListTitleNum == 1) {
	    if( m_nSelectedRoomNum >= 0 ) {

            nYPos = 233;
				    
		    for( int j = 0 ; j < MAX_ROOM_IN; j++ ) {
                if ( g_ClickRoomUserInfo[j].szNick_Name[0] == NULL ) {
                    continue;
                }


                // ��� --------------------------------------------------

                nUserNo = g_ClickRoomUserInfo[j].nClass;

                m_SPR_Class->SetFrame( nUserNo );
			    m_SPR_Class->SetPos( 729 , nYPos + 2 );
                m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );


                // ���� --------------------------------------------------

			    if( g_ClickRoomUserInfo[j].cSex == '0' ) {  // ����
                    m_SRP_SexIcon->SetFrame( 1 );
				    m_SRP_SexIcon->SetPos( 776, nYPos );
                    m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			    }
			    else {                                      // ����
                    m_SRP_SexIcon->SetFrame( 0 );
				    m_SRP_SexIcon->SetPos( 776 , nYPos );
                    m_SRP_SexIcon->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
			    }


                // �г��� ------------------------------------------------

                NickSave( m_szTempText, g_ClickRoomUserInfo[j].szNick_Name );

                if (j == m_nHighlightBarNum ) {
                    DrawText( m_szTempText, 850, nYPos + 5, RGB( 5 , 26 , 51 ), FW_NORMAL, TA_CENTER );
                }
                else {
                    DrawText( m_szTempText, 850, nYPos + 5, RGB( 172 , 246 , 243 ), FW_NORMAL, TA_CENTER );
                }

                // �ʴ� ���� ���� ��� ---------------------------------------

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
            RenderWaitUser( m_nViewUserIdx );  // ������ ���� ������ �ڼ��� ���� ���
        }
        else {
            RenderWaitUser( 0 );                // ������ ���� ������ ������ ���� �ڽ��� ���� ���
        }
    }
    else if ( m_nUserListTitleNum == 1 ) {      // ������ �� ������ �ڼ��� ���� ���
        if ( m_nViewUserIdx != -1 ) {
            RenderRoomUser( m_nViewUserIdx );
        }
        else {
            RenderWaitUser( 0 );                // ������ �� ������ ������ ���� �ڽ��� ���� ���
        }
    }

	if ( m_bShowMakeRoomDlg ) {
		RenderMakeRoom();                       // �游��� ���̾˷α� ������
	}

	if ( m_bShowChodaeBox ) {
        DrawInvite( CHODAE_RECV_POS_X, CHODAE_RECV_POS_Y );
	}

    if ( m_bPassDlg ) {
        RenderPassDlg();                                // ��й� �� ���� ��й�ȣ �Է� â
    }

    //������ ���� ����Ʈ�� ���콺�� �÷������� ��ư�������� �׸��� ���ؼ�.
	DrawUserState( m_TempMX , m_TempMY );
}



//========================================================================
// �ڼ��� ����� ������ ����Ѵ�.
//========================================================================

void CMainFrame::RenderWaitUser( int nUserNum )
{
    //========================================================================
	// ���������� Ŭ���ϸ� �� ������ �ѷ��ش�.
    // ���� â�� Character Information���� �κ�.
    //========================================================================

    int nWaitUserSelNo = nUserNum;   // ��ü ����Ʈ������ ����� ��ȣ
    int nTempClass = 0;
	int nWinCnt = 0;
	int nLoseCnt = 0;
	int nDrawCnt = 0;
	int nAllCnt = 0;

	if ( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_bUser == TRUE ) {
		if ( nWaitUserSelNo >= 0 ) {
			// ����
			if ( strlen( g_CUserList.m_WaitUserInfo[ nWaitUserSelNo ].m_sUserInfo.szNick_Name ) > 10 ) {
				memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
				memcpy( g_szTempStr, g_CUserList.m_WaitUserInfo[ nWaitUserSelNo].m_sUserInfo.szNick_Name, 10 );
				strcat( g_szTempStr, "..." );
                DrawText( g_szTempStr, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
			}
			else {
                DrawText( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.szNick_Name, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
			}

			// �ݾ�
            if ( MouseIn(832, 592, 978, 606) ) {
                I64toA_Money( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.biUserMoney, m_szTempText );
            }
            else {
                I64toA_Money( g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.biUserMoney, m_szTempText, 2 );
            }
            DrawText( m_szTempText, 885, 594, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
			
			// ���
			nTempClass = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nClass;

            m_SPR_Class->SetFrame( nTempClass );
			m_SPR_Class->SetPos( 885 , 615 );
            m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

			nWinCnt  = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nWin_Num;
			nLoseCnt = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nLose_Num;
			nDrawCnt = g_CUserList.m_WaitUserInfo[nWaitUserSelNo].m_sUserInfo.nDraw_Num;

			nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );

			// ����
			sprintf( m_szTempText , "%d�� %d��" , nAllCnt, nWinCnt );
            DrawText( m_szTempText, 885, 642, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

			sprintf( m_szTempText , "%d�� %d��" , nLoseCnt, nDrawCnt );
            DrawText( m_szTempText, 885, 666, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

			PutSpriteAvatar( 721, 548, m_SPR_WaitAvatar, m_WaitAvaImg, ORIGINAL_SIZE );
		}
	}
}



//========================================================================
// �ڼ��� �� ���� ������ ����Ѵ�.
//========================================================================

void CMainFrame::RenderRoomUser( int nUserNum )
{
	int nWinCnt = 0;
	int nLoseCnt = 0;
	int nDrawCnt = 0;
	int nAllCnt = 0;
    int nUserNo = 0;

	if ( g_ClickRoomUserInfo[nUserNum].szNick_Name[0] != NULL ) {
		// ����
		if ( strlen( g_ClickRoomUserInfo[nUserNum].szNick_Name ) > 10 ) {
			memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
			memcpy( g_szTempStr, g_ClickRoomUserInfo[nUserNum].szNick_Name, 10 );
			strcat( g_szTempStr, "..." );

            DrawText( g_szTempStr, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
		}
		else {
            DrawText( g_ClickRoomUserInfo[nUserNum].szNick_Name, 885, 570, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );
		}

		// �ݾ�
        if ( MouseIn(832, 592, 978, 606) ) {
		    I64toA_Money( g_ClickRoomUserInfo[nUserNum].biUserMoney, m_szTempText );
        }
        else {
            I64toA_Money( g_ClickRoomUserInfo[nUserNum].biUserMoney, m_szTempText, 2 );
        }
        DrawText( m_szTempText, 885, 594, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

		// ���
		nUserNo = g_ClickRoomUserInfo[nUserNum].nClass;
        m_SPR_Class->SetFrame( nUserNo );
		m_SPR_Class->SetPos( 885 , 615 );
        m_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

		nWinCnt  = g_ClickRoomUserInfo[nUserNum].nWin_Num;
		nLoseCnt = g_ClickRoomUserInfo[nUserNum].nLose_Num;
		nDrawCnt = g_ClickRoomUserInfo[nUserNum].nDraw_Num;

		nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );
		
		// ����
		sprintf( m_szTempText , "%d�� %d��" , nAllCnt, nWinCnt );
        DrawText( m_szTempText, 885, 642, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

		sprintf( m_szTempText , "%d�� %d��" , nLoseCnt , nDrawCnt );
        DrawText( m_szTempText, 885, 666, RGB( 172, 246, 243 ), FW_NORMAL, TA_LEFT );

		PutSpriteAvatar( 721, 548, m_SPR_WaitAvatar, m_WaitAvaImg, ORIGINAL_SIZE );
	}
}



//========================================================================
// ������ ����Ѵ�.
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
		sprintf( szTempNick, "%s�Կ��� ������ �����Դϴ�.", m_szMessageRecieveUserNick );
	}
	else {
		sprintf( szTempNick, "%s���� ���� �����Դϴ�.", m_szMessageRecieveUserNick );
		
		// ���� ����� �� �޼����� �����ش�
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
		
        DrawText( "�޼��� ������", X + 4, Y + 4, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
	}
}



//========================================================================
// �ʴ� �޾��� ���� ��ȭ���ڸ� �׸���.
//========================================================================

void CMainFrame::DrawInvite( int X , int Y )
{
	char szTempText[MAX_CHATMSG]; // �ӽú��� ( �޼��� ���� )
	memset( szTempText, 0x00, sizeof( szTempText ));

    PutSprite( m_SPR_Invitation, X, Y );

	m_ChodaeYesBTN->SetPos( X + 41, Y + 176 );
	m_ChodaeYesBTN->DrawButton();

	m_ChodaeNoBTN->SetPos( X + 178, Y + 176 );
	m_ChodaeNoBTN->DrawButton();

	sprintf( szTempText , "%d���濡�� �ʴ��ϼ̽��ϴ�." , m_nInviteRoomNo + 1 );
    DrawText( szTempText, X + 163, Y + 92, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
    DrawText( "�ʴ뿡 ���Ͻðڽ��ϱ�?", X + 163, Y + 120, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
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
// �游��� â�� ����.
//========================================================================

void CMainFrame::OpenMakeRoomDlg()
{
    m_bPassRoomChk = FALSE;             // ��й� üũ ����
    m_bShowRoomTitleList = FALSE;       // �� �̸� ����� �Ⱥ����ֵ��� �ʱ�ȭ
    m_nSelectedRoomTitleList = -1;      // ���õ� ���̸� ��� �ʱ�ȭ

    // �⺻ ���̸� 5�� �߿��� ���Ƿ� �ϳ� �����ؼ� ����.

	int nTemp = GetRandom( 0, 4 );

    m_IME_InputRoomTitle->SetIMEText( g_RoomTitle[nTemp] );
    m_IME_InputRoomTitle->InputBegin( false );

	if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
		m_bUserState = FALSE;
		m_biMakeRoomMoneyNo = 0;
        m_nCardOption = CO_CHOICE_MODE; // �游�鶧 �⺻ ���� ���̽� ���
		m_bShowMakeRoomDlg = TRUE;

        // ä��â�� �Է»��¸� �Է� �Ұ��� ���·� �����.
        m_IME_WaitChat->InputEnd();
	}
	else {
		m_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 132" );
	}
}



//========================================================================
// ���� �����.
//========================================================================

void CMainFrame::MakeRoom( BOOL bSecret ) //true ���
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

        // ���̽� �������, �Ϲݸ������ �����Ѵ�.
        if ( m_nCardOption == CO_CHOICE_MODE ) {
            l_sCreateRoom.bChoiceRoom = TRUE;
        }
        else {
            l_sCreateRoom.bChoiceRoom = FALSE;
        }

		//���� �ӴϿ� ���� ���� ����� �ִ� ������ ��������.
		if ( AvailableRoomIn( l_sCreateRoom.biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) == FALSE ) {
			sprintf( m_szTempText, "������ ���� ���� �ش����ذ� ���� �ʽ��ϴ�." );
			m_Message->PutMessage( m_szTempText, "�ٽ� Ȯ���� �ּ���!!!", false );
			return;
		}

		if ( !g_pClient->CreateRoom(&l_sCreateRoom) ) {
			m_Message->PutMessage( "����� ���� ����", "Code - 124" );		
		}
	}
	else {
		m_Message->PutMessage( "�������� �Է��ϼ���.!!!", "Code - 125" );
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
    // ���â�� ���ִ� ���
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
		// �ʴ� "��" ��ư
		if( SendBtnTime( m_ChodaeYesBTN , GTC()) )
		{
			if( m_ChodaeYesBTN->status == DOWN )
			{
				m_ChodaeYesBTN->SetMouseLBDownStatus( FALSE );

				m_bShowChodaeBox = FALSE;
				if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
				
					//�ڽ��� ���� �ִ� �������� �����Ѵ�.
					if ( AvailableRoomIn( m_biInviteRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
					
						sRoomIn TempRoom;
						memset( &TempRoom , 0x00 , sizeof( TempRoom ));
						
						TempRoom.nRoomNo = m_nInviteRoomNo;			
						strcpy( TempRoom.szRoomPass , m_szInviteRoomPass );
						
						g_pClient->RoomIn( &TempRoom );
					}
					else {
						m_Message->PutMessage( "������ �����ӴϷ� �ش����", "�����ϽǼ� �����ϴ�.!!!", false );
					}

				}
				else {
					m_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 161" );
				}

				return;
			}			
		}		

		// �ʴ� "�ƴϿ�" ��ư
		if( m_ChodaeNoBTN->status == DOWN )
		{
			m_ChodaeNoBTN->SetMouseLBDownStatus( FALSE );

			m_bShowChodaeBox = FALSE;
			
			return;
		}

        return;
	}
   
	if ( m_nCurrentMode == GAME_MODE) {
        //���ӹ濡�� �ƹ�Ÿ�� Ŭ���ϸ� ������ ���´�.
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

        // ����ȭ�� ä��â ��ũ�ѹ� ���콺 Ŭ�� ó��
        m_SCB_GameChat->MouseClick( m_mX, m_mY );
	}

	//������ ���� ����Ʈ�� ���콺�� �÷������� ��ư�������� �׸��� ���ؼ�.
	//��ư �԰� �ϱ����ؼ�.
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

	//���� �޼��� �ڽ����� �Է�â�� ����.
	if( m_bShowMessageBox ) {
        if ( MouseIn( 382, 436, 643, 456 ) ) {  // ���� �Է� �κ� ������ Ȱ��ȭ
            m_IME_Memo->InputBegin( true );
        }
        else {                                  // �ٸ� �κ� ������ ��Ȱ��ȭ
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
	// ó�� ���� ī�� ���� �κ�.
    // 4 ���� ī�� �� ���� ī�� ���� ����
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
	// ó�� ���� ī�� ���̽� �κ�.
    // 3���� ī�� �� ������ ī�� 1�� ����
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
			//ī����ġ�� �ٽ� ���ڸ��� ���´�.
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
    // �ʴ��ϱ� ��ȭ������ Ŭ�� �� ��ư ó��
    //--------------------------------------

	if( m_bShowInviteBox ) {
		InputInviteChecks( CHODAE_POS_X + 23, CHODAE_POS_Y + 84, 261, 27 );

		// �ʴ� �ڽ� ���� �ʴ�(Ȯ��)��/�� ������.
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

        // ��� ��ư ������ ��

        if ( m_InviteCancelBTN->status == DOWN ) {
            m_InviteCancelBTN->SetMouseLBDownStatus( FALSE );
            m_bUserState = FALSE;
            m_bShowInviteBox = FALSE;
        }

        // ��ũ�� �� Ŭ�� �� ��ư ó��

        m_SCB_Invite->MouseClick( m_mX, m_mY );
        return;
	}	

    //---------------------------------
    // �ʴ��ϱ� ��ư�� ������ ���� ó��
    //---------------------------------

	if( m_InviteBTN->status == DOWN ) {
		m_InviteBTN->SetMouseLBDownStatus( FALSE );
        m_SCB_Invite->StartPage();

		m_nWaitViewPageNo = 0;
		g_pClient->GameViewWait( m_nWaitViewPageNo );

		InitInviteChecks();
		return;
	}

	//���â �������� ��ư�� ����.
    if ( m_bEnddingShow ) {
        return;
    }

    // ���� ī�� �̵����̸� �̵�����
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

	//���� �뿡���� ��ư ������
	if( m_bShowStartBTN ) {
		if( m_StartBTN->status == DOWN) {   // ���� ��ư Ŭ����
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

		if ( m_bFullScreen == TRUE ) { //Ǯ��ũ���϶���.
			m_bFullScreen = !m_bFullScreen;
			ChangeVideoMode( m_bFullScreen );
		}

		::ShowWindow( g_hWnd, SW_MINIMIZE );
		return;
	}

    //------------------------------------
    // ȯ�漳�� ��ȭ���� Ŭ�� �� ��ư ó��
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

		if      ( MouseIn( CONFIG_DLG_POS_X +  93, CONFIG_DLG_POS_Y + 162, CONFIG_DLG_POS_X + 202, CONFIG_DLG_POS_Y + 183 )) { m_pConfigBox->ChangeVoice( 1 ); } // ����1
        else if ( MouseIn( CONFIG_DLG_POS_X + 203, CONFIG_DLG_POS_Y + 162, CONFIG_DLG_POS_X + 301, CONFIG_DLG_POS_Y + 183 )) { m_pConfigBox->ChangeVoice( 0 ); } // ����1
        else if ( MouseIn( CONFIG_DLG_POS_X +  93, CONFIG_DLG_POS_Y + 163, CONFIG_DLG_POS_X + 202, CONFIG_DLG_POS_Y + 207 )) { m_pConfigBox->ChangeVoice( 2 ); } // ����2
        else if ( MouseIn( CONFIG_DLG_POS_X + 203, CONFIG_DLG_POS_Y + 163, CONFIG_DLG_POS_X + 301, CONFIG_DLG_POS_Y + 207 )) { m_pConfigBox->ChangeVoice( 3 ); } // ����2

		if( m_OkBTN->status == DOWN ) {
			m_OkBTN->SetMouseLBDownStatus(FALSE);

			PlaySound(g_Sound[GS_BTNCLICK]);
			
			//���������� ���Ͽ� ����Ѵ�.	

			//BOOL bInvite;         // �ʴ�     TRUE ��� 
			//BOOL bSecretMessage;  // �ӼӸ�   TRUE ���
			//BOOL bWaveSnd;        // ȿ����   TRUE ���
			//BOOL bMessageAgree;   // �޼���   TRUE ���

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
			
			return; // ���� â �������� ���⼭ ����..
		}
        
        return;
	}

	if( m_ConfigBTN->status == DOWN ) {
		m_ConfigBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK]);

		m_bShowConfigBox = TRUE;
		return;
	}

    if( m_CaptureBTN->status == DOWN ) {            // ȭ�� ĸ�� �ϱ�
		m_CaptureBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK]);
		DTRACE("ĸó ��ư ���ȴ�");
        ScreenCapture();
		return;
	}

    // �游��� â�� ��������
	if( m_bShowMakeRoomDlg )
	{
        //-----------------------------------
        // ��й�ȣ �Է� üũ�ڽ� Ŭ���� ���
        //-----------------------------------

        if ( MouseIn( 626, 293, 642, 309)  ) {
            m_bPassRoomChk = !m_bPassRoomChk;

            if (m_bPassRoomChk) {
                m_IME_InputPass->ClearBuffer();
                m_IME_InputPass->InputBegin(true);      // Ȱ��ȭ(��Ŀ�� ����)
            }
            else {
                m_IME_InputPass->InputEnd();
            }
        } 
        else if ( MouseIn( 443, 292, 621, 310 ) ) {     // ��й�ȣ �Է� �κ�
            if ( m_bPassRoomChk ) {
                m_IME_InputPass->InputBegin(true);
            }
        }
        else {
            m_IME_InputPass->InputBegin(false);         // ��Ȱ��ȭ(��Ŀ�� ����)
        }
        

        //---------------------------
        // �� ���� �Է¶� Ŭ���� ���
        //---------------------------

        if ( MouseIn(443, 267, 623, 285 ) ) {
            m_IME_InputRoomTitle->InputBegin(true);     // Ȱ��ȭ
        }
        else {
            m_IME_InputRoomTitle->InputBegin(false);    // ��Ȱ��ȭ
        }

        //------------------------------
        // �� �ݾ� ���� �κ� Ŭ���� ���
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

            // ������ ����Ʈ�� �����ִ� ��� 0�� ������ �⺻������ �����Ѵ�.
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

        // Ȯ�� ��ư Ŭ����
		if( m_MakeRoomOkBTN->status == DOWN ) {
			m_MakeRoomOkBTN->SetMouseLBDownStatus(FALSE);
			PlaySound(g_Sound[GS_BTNCLICK]);

            //��й� ����.
            if ( m_bPassRoomChk ) {
                memset( m_szPass, 0x00, sizeof(m_szPass) );
                strcpy(m_szPass, m_IME_InputPass->GetIMEText());

                if ( m_szPass[0] != NULL ) {
			        MakeRoom(true);
                }
                else {
                    //�޼��� �ڽ�.
                    m_IME_InputPass->InputEnd();
                    m_Message->PutMessage( "��й�ȣ�� �Է��ϼ���.", "Code - 600" );
                    return;
                }
            }
            else {
                MakeRoom(false);
            }

			return;
		}

        // ��� ��ư Ŭ����
		if( m_MakeRoomCancelBTN->status == DOWN ) {
			m_MakeRoomCancelBTN->SetMouseLBDownStatus(FALSE);
			PlaySound(g_Sound[GS_BTNCLICK]);

			WaitRoomModeSet();

            m_IME_InputPass->InputEnd();
            m_IME_InputPass->ClearBuffer();

            m_IME_InputRoomTitle->InputEnd();
            m_IME_InputRoomTitle->ClearBuffer();

			return; // �游��� â�� �������� ���⼭ ����
		}

        return;
	}

    //-----------------
    // ��й�ȣ �Է� â
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

            sRoomIn l_sRoomInData; // ������ ���� ����ü
			memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

            l_sRoomInData.nRoomNo = g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].nRoomNo;
            strcpy( l_sRoomInData.szRoomPass, m_IME_RoomInPassBox->GetIMEText() );

            m_bPassDlg = FALSE;
            m_IME_RoomInPassBox->InputEnd();
            m_IME_RoomInPassBox->ClearBuffer();

            if ( l_sRoomInData.szRoomPass[0] != NULL) {
                m_bUserState = FALSE;
			    g_pClient->RoomIn( &l_sRoomInData ); // ������ ����.
            }
            else {
                m_Message->PutMessage("��й�ȣ�� �Է��ϼ���.", "Code - 600");
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


	///////// ���� ȭ�鿡�� ����ϴ� ��ư�� /////////////
	if( m_JoinBTN->status == DOWN ) {
		m_JoinBTN->SetMouseLBDownStatus(FALSE);

		if( SendBtnTime( m_JoinBTN , GTC()) ) {
			if( m_nSelectedRoomNum >= 0 ) {
				if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
					if ( g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].nCurCnt >= MAX_ROOM_IN ) {
						m_Message->PutMessage( "�濡 �ο��� ��� á���ϴ�.", "Code - 126" );
						return;
					}

					//�ڽ��� ���� �ִ� �������� �����Ѵ�.
					if ( AvailableRoomIn( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
                        if ( g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].bSecret ) {
                            m_IME_RoomInPassBox->ClearBuffer();
                            m_IME_RoomInPassBox->InputBegin(true);
                            m_bPassDlg = TRUE;
                        }
                        else {
                            sRoomIn l_sRoomInData;                  // ������ ���� ����ü
						    memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

						    l_sRoomInData.nRoomNo = g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].nRoomNo;
						    memset( l_sRoomInData.szRoomPass , 0 , sizeof(l_sRoomInData.szRoomPass));

						    m_bUserState = FALSE;
						    g_pClient->RoomIn( &l_sRoomInData );    // ������ ����.
                        }
					}
					else {
						m_Message->PutMessage( "������ �����ӴϷ� �ش����", "�����ϽǼ� �����ϴ�.!!!", false );
					}
				}
				else {
					m_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 133" );
				}
			}
			else {
				m_Message->PutMessage( "������ ���� �������ּ���" , "Code - 127" );
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
				m_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 134" );
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

        if ( MouseIn(700, 165, 820, 195) ) {    // ������ ����Ʈ ���ٰ� ����� ����Ʈ Ÿ��Ʋ Ŭ����
            if ( m_nUserListTitleNum == 1 ) {
                m_nViewUserIdx = -1;
                m_nViewUserNo = -1;
                m_nUserListTitleNum = 0;
                AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
            }
            return;
        }

        if ( MouseIn(821, 165, 921, 195) ) {    // ����� ����Ʈ ���ٰ� �� ���� ����Ʈ Ÿ��Ʋ Ŭ����
            if ( m_nUserListTitleNum == 0 ) {
                m_nViewUserIdx = -1;
                m_nViewUserNo = -1;
                m_nUserListTitleNum = 1;
                AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
            }
            return;
        }

        // IME FOCUS ó��

        // ä��â �κ� Ŭ���ϸ� ä��â �Է»��·� �����.
        if ( MouseIn( 30, 710, 643, 729 ) && !m_IME_WaitChat->IsInputState() ) {
            m_SCB_WaitChat->EndPage();          // �� �Ʒ� �������� �̵�
            m_IME_WaitChat->InputBegin(true);   // �Է� ����
        }

        // ä��â �̿��� �κ��� Ŭ���ϸ� �Է� �Ұ��� ���·� �����.
        else if ( !MouseIn( 23, 601, 680, 737 ) && m_IME_WaitChat->IsInputState() ) {
            m_IME_WaitChat->InputEnd();
            m_IME_WaitChat->ClearBuffer();
        }

        // ���ǿ��� �� Ŭ�� ��
        else if ( MouseIn( 35, 206, 644, 502 - 2 ) ) {
            int nSelectedRoom = (m_mY + 1 - 206)  /  ((502 + 1 - 206) / MAX_VIEW_WAIT_ROOM);

            if ( nSelectedRoom != -1 &&
                 !(m_bShowMakeRoomDlg || m_bShowChodaeBox || m_bShowInviteBox || m_bShowConfigBox || m_bShowMessageBox || m_Message->GetbShowCheck()))
            {
                g_nWaitRoomSelIdx = nSelectedRoom + m_SCB_WaitRoom->GetStartElem();

                // ���� ���� ���� Ŭ���� ���
                if ( g_nWaitRoomSelIdx >= g_nWaitRoomCnt ) {
                    m_nSelectedRoomNum = -1;
                    g_nWaitRoomSelIdx = -1;
                }

                // ���� �ִ� ���� Ŭ���� ���
                else {
                    // ������ ���õ� �� ��ȣ�� �̹��� Ŭ���� ���� ��ȣ�� �ٸ��� ���õ� ���� ���ٰ� ǥ��
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

					    //���⿡�� ������ �濡���� ������ ��û�Ѵ�.
					    g_pClient->GetRoomInfo( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nRoomNo );
				    }
			    }
			    
			    return;
            }
        }

		//���ǿ��� �游��⸦ Ŭ��������
        if( SendBtnTime( m_MakeRoomBTN, GTC()) ) {
		    if( m_MakeRoomBTN->status == DOWN )
		    {
			    m_MakeRoomBTN->SetMouseLBDownStatus(FALSE);

                OpenMakeRoomDlg();                  // �游��� â�� ����.
			    return;
		    }
        }

        if( m_nHighlightBarNum != -1) {
            // ����� Ŭ��
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

            // �� ���� Ŭ��
            else if ( m_nUserListTitleNum == 1 ) {
                if ( m_nSelectedRoomNum != -1 ) {
                    // �濡 ����� �ְ�, ����� �ִ� ��ġ�� �������� ���� ������ �����ش�.
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
        // ä��â �κ� Ŭ���ϸ� ä��â �Է»��·� �����.
        if ( MouseIn( 15, 733, 329, 752 ) && !m_IME_GameChat->IsInputState() ) {
            m_SCB_GameChat->EndPage();          // �� �Ʒ� �������� �̵�
            m_IME_GameChat->InputBegin(true);   // �Է� ����
        }

        // ä��â �̿��� �κ��� Ŭ���ϸ� �Է� �Ұ��� ���·� �����.
        else if ( !MouseIn( 15, 557, 329, 752 ) && m_IME_GameChat->IsInputState() ) {
            m_IME_GameChat->InputEnd();
            m_IME_GameChat->ClearBuffer();
        }
    }

	
	// '0':����, '1':üũ, '2':����, '3':��, '4':����, '5':��, '6':Ǯ, '7':����

    /* [*BTN]
	if( m_DDaDDang->status == DOWN ) {
		DTRACE("���� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '0' );
		g_pClient->UserSelect( '0' );
        g_biTotalBettingMoney += m_biDdaDang;
		return;
	}
    */
	if( m_DDaDDang->status == DOWN ) {
		DTRACE("���� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '0' );
		g_pClient->UserSelect( '0' );
        g_biTotalBettingMoney += m_biDdaDang;
		return;
	}


	if( m_Check->status == DOWN ) {
		DTRACE("üũ ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '1' );
		g_pClient->UserSelect( '1' );
        g_biTotalBettingMoney += m_biCheck;
		return;
	}

	if( m_Quater->status == DOWN ) {
		DTRACE("���� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '2' );
		g_pClient->UserSelect( '2' );
        g_biTotalBettingMoney += m_biQuater;
		return;
	}

	if( m_BBing->status == DOWN ) {
		DTRACE("�� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '3' );
		g_pClient->UserSelect( '3' );
        g_biTotalBettingMoney +=  m_biBing;
		return;
	}
	
	if( m_Half->status == DOWN ) {
		DTRACE("���� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '4' );
		g_pClient->UserSelect( '4' );
        g_biTotalBettingMoney += m_biHalf;
		return;
	}

	if( m_Call->status == DOWN ) {
		DTRACE("�� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '5' );
		g_pClient->UserSelect( '5' );
        g_biTotalBettingMoney += m_biCall;
		return;
	}

    /* [*BTN]
	if( m_Full->status == DOWN ) {
		DTRACE("Ǯ ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '6' );
		g_pClient->UserSelect( '6' );
        g_biTotalBettingMoney += m_biFull;
		return;
	}
    */
	if( m_Full->status == DOWN ) {
		DTRACE("Ǯ ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '6' );
		g_pClient->UserSelect( '6' );
        g_biTotalBettingMoney += m_biFull;
		return;
	}
	if( m_Die->status == DOWN ) {
		DTRACE("���� ���ȴ�.");
		AllCallBtnUp();

		UserSelectView( 0, '7' );
		g_pClient->UserSelect( '7' );
		return;
	}
}



// �ʴ��Ҷ� üũ�� ��ȣ�� �����Ѵ�. ������ ���� ������ ���̳ʽ�( -1 ) ���� ���´�.

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
    int nViewCnt;       // �� ȭ�鿡 ����� �׸�� ��꿡 ����ϴ� ī����
    int nBaseY;         // �������� ��½� ����ϴ� ���� Y ��ǥ
    int nIdx;           // ���� �ε���
	char szChatCopy[255];

    nBaseY = InviteBox_YPos + 84;           // ù���� ������ǥ ����
    nIdx = m_SCB_Invite->GetStartElem();    // ���� �������� ���� ���� �ε��� ����
    nViewCnt = 0;

	for( int i = 0; i < MAX_VIEW_CHODAE_USER; i++ ) {
		if( g_sGameWaitInfo[nIdx].nUserNo > 0 ) {      // ����Ÿ�� �׷����� �����Ѵ�.

            //------------------------------------
            // ������ ������ ǥ�� �̹����� �׸���.
            //------------------------------------
            
            if( m_sInviteCheck[nIdx].bCheck == TRUE ) {
                PutSprite( m_SPR_CheckBall, InviteBox_XPos + 29, nBaseY + 5, 1 );
		    }

            //------------
            // ���� ������
            //------------

			if( g_sGameWaitInfo[nIdx].cSex == '1' ) {  // 1 �̸� �����̹Ƿ� ���� �������� �׸���.
                PutSprite( m_SRP_SexIcon, InviteBox_XPos + 189 , nBaseY + 3, 0 );
			}
			else {                                  // 0 �̴ϱ� ���� �������� �׸���.
                PutSprite( m_SRP_SexIcon, InviteBox_XPos + 189 , nBaseY + 3, 1 );
			}

			nCheckNo = GetInviteCheck();            // üũ�� ��ȣ�� �޴´�.

            //-------------------
            // �г��� & ���� �Ӵ�
            //-------------------
			
			if( strlen( g_sGameWaitInfo[nIdx].szNick_Name ) > 14 ) {
				memset( &szChatCopy, 0x00, sizeof( szChatCopy ));
				memcpy(  szChatCopy, g_sGameWaitInfo[nIdx].szNick_Name, 14 );
				strcat(  szChatCopy, "..." );
			}
			else {
				strcpy( szChatCopy, g_sGameWaitInfo[nIdx].szNick_Name );
			}

			if( nIdx == nCheckNo ) {                   // �̳��� üũ�� ��ȣ�� �ٸ� ������ �׸���.
                // �г���
                DrawText( szChatCopy, InviteBox_XPos + 48 , nBaseY + 7, RGB( 140, 215, 69 ), FW_BOLD, TA_LEFT, 13 );

                // ���� �Ӵ�
                I64toA_Money( g_sGameWaitInfo[nIdx].biUserMoney, szChatCopy, 1 );
                DrawText( szChatCopy, InviteBox_XPos + 277, nBaseY + 7, RGB( 140, 215, 69 ), FW_BOLD, TA_RIGHT );
			}
            else {                                  // �׷��� ������ �׳� ���� ������ ��´�.
                // �г���
                DrawText( szChatCopy, InviteBox_XPos + 48 , nBaseY + 7, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT, 13 );

                // ���� �Ӵ�
                I64toA_Money( g_sGameWaitInfo[nIdx].biUserMoney, szChatCopy, 1 );
                DrawText( szChatCopy, InviteBox_XPos + 277, nBaseY + 7, RGB( 255, 255, 255 ), FW_NORMAL, TA_RIGHT );
			}

            nBaseY += 27;   // ��¿� ���� ��ǥ�� ���� �ٷ� �̵�
            nIdx++;         // ���� ���� ����
            
            nViewCnt++;
            if ( nViewCnt >= MAX_VIEW_CHODAE_USER ) {
                break;
            }
		}
	}
}



//========================================================================
// �ʴ��ϱ� â�� �׸���.
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
    int nLine = 0;  // �ʴ�â�� �� ��ȣ(ù��°���� 0)
    int nUserCnt = m_SCB_Invite->GetTotalElem();

	for( int i = m_SCB_Invite->GetStartElem(); i < nUserCnt; i++ )  {
		nTempY = Y + ( nLine * SizeY );

        // ���콺 Ŀ���� nLine��° ���� Ŭ���� ���
		if( MouseIn( X, nTempY, X + SizeX - 1, nTempY + SizeY - 1 )) {
			if( g_sGameWaitInfo[i].nUserNo > 0 ) {
                // ���� Ŭ���� ��ġ�� �����ϰ� ��� FALSE�� �����.
				for( int j = 0; j < MAX_GAMEVIEWWAIT; j++ ) {
                    if( i != j ) {
                        m_sInviteCheck[j].bCheck = FALSE;
                    }
				}

                if( m_bUserState ) {
                    m_bUserState = FALSE;
                }

                // Ŭ���� ��ġ�� ���¸� ������Ų��.
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
// �ʴ�â ���� ���û��¸� �ʱ�ȭ�Ѵ�.
//========================================================================

void CMainFrame::InitInviteChecks( void )
{
    // �ʴ�â �»���� ���������� ����� ��ġ

    for( int i = 0 ; i < MAX_GAMEVIEWWAIT; i++ ) {
		m_sInviteCheck[i].bCheck = FALSE;
	}
}



//========================================================================
// ���� ��ư Ŭ����
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
	if( m_nCurrentMode == GAME_MODE ) {     //������.
        if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt <= 0 ) {
            g_pClient->RoomOut();
        }
	}
}



void CMainFrame::OnExitBtnDown()
{
	char szTempStr2[255];

	if( m_nCurrentMode == GAME_MODE) {      //������.
        if ( g_CGameUserList.m_GameUserInfo[0].m_nCardCnt <= 0 ) {
            g_pClient->RoomOut();
        }
		else {
			g_CGameUserList.m_GameUserInfo[0].m_bExitReserve = !g_CGameUserList.m_GameUserInfo[0].m_bExitReserve;

			if ( g_CGameUserList.m_GameUserInfo[0].m_bExitReserve ) {
				sprintf( szTempStr2, "�� %s�� ������ ����", g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );					
				SetGameChatText( szTempStr2, RGB( 255, 255, 255 ));
				g_pClient->GameChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, szTempStr2 , 0 );
			}
			else {
				sprintf( szTempStr2, "�� %s�� ������ �������", g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );					
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

	//������ ������ ���� ����.
	if ( m_bShowMakeRoomDlg && !m_bShowRoomTitleList) {
        // ��Ӵ� ����
		if      ( MouseIn( 374, 357, 507, 377 ) ) m_biMakeRoomMoneyNo = 0;
	//	else if ( MouseIn( 514, 357, 647, 377 ) ) m_biMakeRoomMoneyNo = 1;
		else if ( MouseIn( 374, 378, 507, 398 ) ) m_biMakeRoomMoneyNo = 2;
	//	else if ( MouseIn( 514, 378, 647, 398 ) ) m_biMakeRoomMoneyNo = 3;
        else if ( MouseIn( 374, 399, 507, 419 ) ) m_biMakeRoomMoneyNo = 4;
//		else if ( MouseIn( 514, 399, 647, 419 ) ) m_biMakeRoomMoneyNo = 5;
        else if ( MouseIn( 374, 420, 507, 440 ) ) m_biMakeRoomMoneyNo = 6;
//		else if ( MouseIn( 514, 420, 647, 440 ) ) m_biMakeRoomMoneyNo = 7;
/*
        // �пɼ� ����
        if ( MouseIn( 449, 461, 539, 485 ) ) {
            m_nCardOption = CO_NORMAL_MODE;     // �Ϲ� ���
        }
        else if ( MouseIn( 543, 461, 633, 485 ) ) {
            m_nCardOption = CO_CHOICE_MODE;     // ���̽� ���
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
        //���� ���� ����.
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
// �ʴ븦 �޾��� ���� ó��
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

    // ä��â �Է��� ������.
    if ( m_nCurrentMode == WAITROOM_MODE ) {
        m_IME_WaitChat->InputEnd();
    }
    else if ( m_nCurrentMode == GAME_MODE ) {
        m_IME_GameChat->InputEnd();
    }

	PlaySound( g_Sound[GS_POPUP] );
	m_bShowChodaeBox = TRUE;
	
	m_nInviteRoomNo = nRoomNo; // ���ȣ�� �����Ѵ�.
	m_biInviteRoomMoney = biRoomMoney;
	
	// �� ��й�ȣ�� �����Ѵ�.
	memset( m_szInviteRoomPass , 0x00 ,sizeof( m_szInviteRoomPass ));
	strcpy( m_szInviteRoomPass , pRoomPass );
	
	// ���� �޼����� �����صд�.
	memset( m_szInviteMsg , 0x00 ,sizeof( m_szInviteMsg ));
	strcpy( m_szInviteMsg , pInviteMsg );

}

void CMainFrame::ButtonDownCheck(int mX, int mY) // ��ư�� ���������� Ȯ��
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
		case WAITROOM_MODE: // �����̸�

			m_ConfigBTN->CheckMouseOver( mX , mY , TRUE ); // ���� ��ư
			m_MakeRoomBTN->CheckMouseOver( mX , mY , TRUE );
			m_JoinBTN->CheckMouseOver( mX , mY , TRUE );
			m_QuickJoinBTN->CheckMouseOver( mX , mY , TRUE );
            m_CaptureBTN->CheckMouseOver( mX , mY , TRUE );
			m_UserStateBTN->CheckMouseOver( mX , mY , TRUE );

            m_SCB_WaitChat->BTN_MouseDown( mX, mY );        // ä��â ��ũ�� ��
            m_SCB_WaitRoom->BTN_MouseDown( mX, mY );

            if ( m_nUserListTitleNum == 0 ) {
                m_SCB_WaitUser->BTN_MouseDown( mX, mY );        // ����� ����Ʈ ��ũ�� ��
            }

			break;

		case GAME_MODE: // ����ȭ���̸�

			m_ConfigBTN->CheckMouseOver( mX , mY , TRUE ); // ���� ��ư
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
		//��ư ���� ó��
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
		case WAITROOM_MODE: // �����̸�
			m_MakeRoomBTN->SetMouseLBDownStatus(FALSE);
			m_JoinBTN->SetMouseLBDownStatus(FALSE);
			m_QuickJoinBTN->SetMouseLBDownStatus( FALSE );
			m_UserStateBTN->SetMouseLBDownStatus( FALSE );	
            m_CaptureBTN->SetMouseLBDownStatus(FALSE);

            m_SCB_WaitChat->BTN_MouseUp();          // ä��â ��ũ�� ��
            m_SCB_WaitRoom->BTN_MouseUp();

            if ( m_nUserListTitleNum == 0 ) {
                m_SCB_WaitUser->BTN_MouseUp();      // ����� �ý�Ʈ ��ũ�� ��
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
	// ���콺�� ��Ʈ�� �ȿ� ������
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
        
        // ���϶���Ʈ�� 
        if (MouseIn(713, 231, 949, 530)) {
            m_nHighlightBarNum = (m_mY - 231) / 25;    // 25�� ��ĭ�� ���� ����
        }
        else {
            m_nHighlightBarNum = -1;
        }
	}

	m_mX = point.x;
	m_mY = point.y;
	
	CheckButtons(m_mX, m_mY);

    if ( !g_bConnectingSpr ) {          // '�������Դϴ�.' �޽��� ������ ������ �Է� ���� ����
        // ��ũ�� �� �巡�� ó��
        m_SCB_WaitChat->Drag( m_mX, m_mY );
        m_SCB_WaitRoom->Drag( m_mX, m_mY );
        
        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->Drag( m_mX, m_mY );
        }
    }

    // �� ����� â���� �� ���� ����Ʈ�� ���ִ� ���
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
        // ���� ī�� �̵� ó��
        if ( m_bMoveHiddenCard ) {
            // ���� ī�尡 ������ �������ִ� ��ġ���� �������� �̵����� ���ϰ� �Ѵ�.
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
// ��й濡 ���� �н����带 �Է��ϴ� â�� �׸���.
//========================================================================

void CMainFrame::RenderPassDlg()
{
    PutSprite( m_SPR_PassDlg, PASS_DLG_POS_X, PASS_DLG_POS_Y );

    // �Է� IME�� ��Ŀ���� ������ ����� ������� ĥ�Ѵ�.
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



void CMainFrame::RenderMakeRoom() // �游��� â ������
{
    m_SPR_MakeRoom->SetPos( 349, 208 );
	m_SPR_MakeRoom->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
    m_RoomTitleBTN->DrawButton();

	m_MakeRoomOkBTN->DrawButton();
	m_MakeRoomCancelBTN->DrawButton();

    //�� ������ ����Ѵ�.
    if ( m_IME_InputRoomTitle->IsInputState() ) {
        if ( !m_IME_InputRoomTitle->IsFocus() ) {
            g_pCDXScreen->GetBack()->FillRect( 445, 269, 626, 285, m_rgbGray );
        }
        m_IME_InputRoomTitle->PutIMEText( 447, 272, RGB(0, 0, 0), RGB(0, 0, 0) );
    }
	
	//�游��⿡�� ���콺�� Ŭ���ϸ� �ش� ǥ�ÿ� ���� ��´�.
    PutSprite( m_SPR_CheckBall,
               m_sMakeRoomMoneyPoint[m_biMakeRoomMoneyNo].x,
               m_sMakeRoomMoneyPoint[m_biMakeRoomMoneyNo].y,
               1 );
/*
    // �пɼǿ� ���� �׸���.
    if ( m_nCardOption == CO_NORMAL_MODE ) { 
        PutSprite( m_SPR_CheckBall, 454, 467, 1 );  // �Ϲ� ���
    } else {
        PutSprite( m_SPR_CheckBall, 548, 467, 1 );  // ���̽� ���
    }
*/
//	PutSprite( m_SPR_CheckBall, 548, 467, 1 );

    // �н����� üũ�� ������� ��Ŀ���� ������ ȸ������� ĥ�Ѵ�.
    if ( !m_IME_InputPass->IsFocus() ) {
        g_pCDXScreen->GetBack()->FillRect( 445, 294, 621, 310, m_rgbGray );
    }

    // �н����� üũ ǥ��
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
        // �� ���� ����Ʈ�� �׵θ��� �׸���.
        g_pCDXScreen->GetBack()->FillRect(
                ROOM_TITLE_LIST_X,
                ROOM_TITLE_LIST_Y,
                ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH,
                ROOM_TITLE_LIST_Y + ROOM_TITLE_LIST_HEIGHT,
                m_rgbMakeRoomTitleListBorder
        );

        // �� ���� ����Ʈ�� ����� �׸���.
        g_pCDXScreen->GetBack()->FillRect(
                ROOM_TITLE_LIST_X + 1,
                ROOM_TITLE_LIST_Y + 1,
                ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH - 1,
                ROOM_TITLE_LIST_Y + ROOM_TITLE_LIST_HEIGHT - 1,
                RGB( 255, 255, 255 )
        );

        // ���õ� �׸� ��ġ�� �簢�� �ٸ� �׸���.
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



void CMainFrame::CheckButtons(int mX, int mY) // ���콺 Ŀ���� ��ư���� �ִ��� üũ
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
		case WAITROOM_MODE: // �����̸�

			m_ConfigBTN->CheckMouseOver( mX , mY ); // ���� ��ư
			m_MakeRoomBTN->CheckMouseOver( mX , mY );
			m_JoinBTN->CheckMouseOver( mX , mY );
			m_QuickJoinBTN->CheckMouseOver( mX , mY );
			m_UserStateBTN->CheckMouseOver( mX , mY );
            m_CaptureBTN->CheckMouseOver( mX , mY );

            m_SCB_WaitChat->BTN_MouseOver( mX, mY );          // ä��â ��ũ�� ��
            m_SCB_WaitRoom->BTN_MouseOver( mX, mY );

            if ( m_nUserListTitleNum == 0) {
                m_SCB_WaitUser->BTN_MouseOver( mX, mY );          // ����� ����Ʈ ��ũ�� ��
            }
			
			break;

		case GAME_MODE: // ����ȭ���̸�

			m_ConfigBTN->CheckMouseOver( mX , mY ); // ���� ��ư
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
	
    pDC->SetTextColor(RGB(0, 0, 0));            //���� ����
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

    if( !m_bMoveDlg && !m_bFullScreen && nHit == HTCLIENT ) {   // ���콺�� �����϶� �����찡 �����̰� �� ����
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
        // â�� ������ �� ����Ŭ���ϸ� �����Ѵ�.s
        if (  m_bShowMakeRoomDlg 
           || m_bShowChodaeBox 
           || m_bShowInviteBox 
           || m_bShowConfigBox 
           || m_bShowMessageBox
           || m_Message->GetbShowCheck())
        {
            return;
        }

        int nSelRoomIdx = -1;           // ����Ŭ���� ���� �ε���(����Ʈ�� �� ó������ ���� ���°������ ��Ÿ��)
        int nSelRoomNo = 0;             // ����Ŭ���� ���� ��ȣ(���� ���� ��ȣ)
		int nTempViewRoomMessage = -1;
		BOOL bRoomClick = false;

        // ������ �� ����Ŭ��
        if (MouseIn(35, 206, 644, 502 - 2)) {
            nSelRoomIdx = (m_mY + 1 - 206)  /  ((502 + 1 - 206) / MAX_VIEW_WAIT_ROOM);
            nSelRoomIdx += m_SCB_WaitRoom->GetStartElem();

            if ( nSelRoomIdx >= 0 && nSelRoomIdx < g_nWaitRoomCnt ) {   // �����ϴ� ���� ���� ������ Ŭ���� ���
                nSelRoomNo = g_nWaitRoomIdx[ nSelRoomIdx ];             // ���� ���� ��ȣ ����
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
						m_Message->PutMessage( "�濡 �ο��� ��� á���ϴ�.", "Code - 126" );
						return;
					}

					//�ڽ��� ���� �ִ� �������� �����Ѵ�.
					if ( AvailableRoomIn( g_CWaitRoomList.m_sWaitRoomList[nTempViewRoomMessage].biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
                        if ( g_CWaitRoomList.m_sWaitRoomList[m_nSelectedRoomNum].bSecret ) {
                            m_IME_RoomInPassBox->ClearBuffer();
                            m_IME_RoomInPassBox->InputBegin(true);
                            m_bPassDlg = TRUE;
                        }
                        else {
                            sRoomIn l_sRoomInData;                  // ������ ���� ����ü
						    memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

						    l_sRoomInData.nRoomNo = g_CWaitRoomList.m_sWaitRoomList[nTempViewRoomMessage].nRoomNo;
						    memset( l_sRoomInData.szRoomPass , 0 , sizeof(l_sRoomInData.szRoomPass));

						    m_bUserState = FALSE;
						    g_pClient->RoomIn( &l_sRoomInData );    // ������ ����.
                        }
					}
					else {
						m_Message->PutMessage( "������ �����ӴϷ� �ش����", "�����ϽǼ� �����ϴ�.!!!", false );
					}
				}
				else {
					m_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 133" );
				}
			}
			else {
				m_Message->PutMessage( "���� �� ���� �������ּ���" , "Code - 127" );
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
        // ����� ����Ʈ�� ���� Ŭ���� ���
        if ( m_nUserListTitleNum == 0 ) {
            m_nUserStatus[ m_nHighlightBarNum + m_SCB_WaitUser->GetStartElem() ] = DOWN;
            m_nRButtonWaitUserNo = m_nHighlightBarNum + m_SCB_WaitUser->GetStartElem();
        }

        // �� ���� ����Ʈ�� ���� Ŭ���� ���
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
    ::SetFocus( g_hWnd );       // CHttpView�� ���� Ű �Է� �ȵǴ� ���� ����

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
				//�� �޼����� ������ ���̴�.
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
            // �濡 ����� �ְ�, ����� �ִ� ��ġ�� �������� ���� ������ �����ش�.
			if ( g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt > 0 ) {
				if ( m_nHighlightBarNum >= 0 && m_nHighlightBarNum < g_CWaitRoomList.m_sWaitRoomList[ m_nSelectedRoomNum ].nCurCnt ) {
                    if( m_nUserStatus[m_nRButtonWaitUserNo] == DOWN ) {
				        // �޴� ������ �̸��� �̸� �����صд�.
				        strcpy( m_szMessageRecieveUserNick, g_ClickRoomUserInfo[ m_nHighlightBarNum ].szNick_Name );
				        m_bMessageSend = TRUE;  //�� �޼����� ������ ���̴�.
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
// __int64 Ÿ���� ���ӸӴϸ� ���ڿ��� ��ȯ�Ѵ�.
//
// input:   *szBuff = ��ȯ�� ���ڿ��� ����� ����
//          nMoney = ���ڿ��� ��ȯ�� ���ӸӴ�
//          nUnitCnt = ����� ������ ����
//                      0 ������ ��� : ��� �ڸ��� ��� (�⺻��)
//                      1 �̻� : ������ ���ڸ�ŭ�� ���� ���(��: 2��
//                               ������ ��� �ֻ��� 2������ ���)
//////////////////////////////////////////////////////////////////////////
void CMainFrame::I64toA_Money(BigInt biMoney, char *szBuff, int nUnitCnt )
{

    static BigInt nTemp = 0;			// ����� ���ڸ� �����ϱ� ���� �ӽ� ����
    static int nUnits = 0;              // ���� ��(���ӸӴϸ� ª�� ǥ���ϴ� ��� �� ������ ����ϱ� ���ؼ� ���)
    static int nIdx = 0;                // �迭 �ε��� & ������ �� �ڸ���
    static int nNumArray[10];           // �� ������ ���ڰ� �� �迭(�迭�� �� ĭ�� �� ������ ����(��: 5643)�� ����)
    static char *szUnits[5] = { "", "��", "��", "��", "��" }; // ����
    static char szTemp[255];

    nIdx = 0;
    nUnits = 0;
    nTemp = biMoney;

    do {
        nNumArray[nIdx++] = (int)(nTemp % 10000);
        nTemp /= 10000;
    } while(nTemp > 0);

    strcpy(szBuff, "");

    // �� �ڸ� �̸��̸� ������ ���� ǥ��(���� 0�� ��쿡��)
    if (nIdx == 1) {
        _itoa(nNumArray[0], szTemp, 10);
        strcat(szBuff, szTemp);
    }

    // �� �ڸ� �̻��̸�
    else {
        for ( int i = nIdx - 1; i >= 0; i-- ) {
            // ���� 0�� �������� ������� �ʴ´�.
            if (nNumArray[i] == 0) continue;

            // �ֻ��� �ڸ��� ������ ������ ���ڵ��� �տ� ������ ���δ�.
            if (i < nIdx - 1) strcat(szBuff, " ");

            // �ش� ������ ���ڸ� ���ڿ��� ��ȯ�ؼ� ����
            _itoa(nNumArray[i], szTemp, 10);
            strcat(szBuff, szTemp);
            strcat(szBuff, szUnits[i]);

            if ( nUnitCnt > 0 ) {   // Ư�� ������ŭ�� ������ ����ϱ� ���ؼ� ������ ���( 0 ���ϸ� ��� ���� ���)
                nUnits++;
                if ( nUnits >= nUnitCnt ) {
                    break;
                }
            }
        }
    }

    strcat(szBuff, "��");

}



LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
		case WM_ASYNC:
			switch(WSAGETSELECTEVENT(lParam))
			{		
				//���� ���� �޽���
				case FD_CONNECT:
					g_pClient->OnConnect( WSAGETSELECTERROR(lParam) );
					break;

				case FD_CLOSE:
					g_pClient->Destroy();
					Sleep(1000);	//�̹̻������ ���̵��Դϴ�. ���� ������ �ϱ����ؼ�.
					m_Message->PutMessage( "�����Ϳ�������. �ٽ� �����Ͽ��ּ���", "Code - 136", true );
					break;

				//������ �۽� �޽���
				case FD_WRITE:
					g_pClient->Send();
					break;

				//������ ���� �޽���
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
                m_IME_WaitChat->OnImeComposition( g_hWnd, lParam );         // ���� ä��â �ѱ� �Է� ó��
                m_IME_InputRoomTitle->OnImeComposition( g_hWnd, lParam );   // �� ���� �ѱ� �Է� ó��
                m_IME_RoomInPassBox->OnImeComposition( g_hWnd, lParam );    // ��й�ȣ �ѱ� �Է� ó��
            }
            else if ( m_nCurrentMode == GAME_MODE ) {
                m_IME_GameChat->OnImeComposition( g_hWnd, lParam );         // ����ȭ�� ä��â �ѱ� �Է� ó��
            }

            m_IME_Memo->OnImeComposition( g_hWnd, lParam );                 // ���� �Է¿� IME �ѱ� �Է� ó��
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

	m_bActive = FALSE;	//�̰� �־����� ������...���ξ����忡�� ������ �ϰ� GameModeSet�� �����ε� ������.

	//�ִ�ȭ �� �׸��� �ٲ��ش�. 
    
	//���
	m_MinBTN->SetPos( 958, 5 );
	m_MaxBTN->SetPos( 979, 5 );
	m_MaxBTN2->SetPos( 979, 5 );
	m_ExitBTN->SetPos( 1000, 5 );
	
	//�ϴ�
	m_ConfigBTN->SetPos( 814, 721 );
	m_Exit2BTN->SetPos( 955, 721 );  //���ӷ�
//	m_Exit2BTN->SetPos( 555, 721 );
    m_CaptureBTN->SetPos( 767, 721 );

	//���
	m_MinBTN->ReGetButtonImage(".\\image\\game\\btn_bar_minimize.spr",  17, 17 );
	m_MaxBTN->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr",  17, 17 );
	m_MaxBTN2->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr", 17, 17 );
	m_ExitBTN->ReGetButtonImage(".\\image\\game\\btn_bar_close.spr",    17, 17 );

	//�ϴ�
	m_ConfigBTN->ReGetButtonImage(".\\image\\game\\btn_board_option.spr",    45, 26 );
	m_Exit2BTN->ReGetButtonImage(".\\image\\game\\btn_board_close.spr",      45, 26 );
    m_CaptureBTN->ReGetButtonImage(".\\image\\game\\btn_board_capture.spr",   45, 26 );

	m_bActive = TRUE;
    
    //////////////////////////////////////////////////////////////////////////

	m_bStartInit = FALSE;
	m_bCardChoice = FALSE;
	m_bCardDump = FALSE;

	AvatarPlay( 0, g_szAvaUrl );
	
	m_FrameSkip.Clear();            //ó������ �ٽ� ���ش�.

	g_bConnectingSpr = false;
    g_nWaitRoomSelIdx = -1;         // ���õ� �� ��ȣ �ʱ�ȭ
	m_nStartBtnCnt = 0;
    m_nWinnerMarkPos = 0;           // �̰��� ���� �̹����� ������ ��ȣ �ʱ�ȭ
    m_nWinnerMarkPosCnt = 0;

	g_pCMainFrame->m_nCurrentMode = GAME_MODE;

    m_IME_WaitChat->InputEnd();     // ���� ä��â IME�� �ʱ�ȭ �Ѵ�.
    m_IME_WaitChat->ClearBuffer();

    m_IME_GameChat->InputEnd();     // ����ȭ�� ä��â IME �ʱ�ȭ
    m_IME_GameChat->ClearBuffer();

    m_pGameChat->Init();
    m_SCB_GameChat->SetElem( 0 );
    m_SCB_GameChat->StartPage();

    //CloseBanner();
}



void CMainFrame::WaitRoomModeSet()
{
	//////////////////////////////////////////////////////////////////////////

	m_bActive = FALSE;	//�̰� �־����� ������...���ξ����忡�� ������ �ϰ� GameModeSet�� �����ε� ������.

	//�ִ�ȭ �� �׸��� �ٲ��ش�. 
	//���
	m_MinBTN->SetPos( 962, 5 );
	m_MaxBTN->SetPos( 981, 5 );
	m_MaxBTN2->SetPos( 981, 5 );
	m_ExitBTN->SetPos( 1000, 5 );
	
	//�ϴ�
	m_ConfigBTN->SetPos( 881, 4 );
	m_Exit2BTN->SetPos( 29, 555 );  //���� 
    m_CaptureBTN->SetPos( 804, 4 );

	//���
	m_MinBTN->ReGetButtonImage(".\\image\\lobby\\btn_min.spr",   16, 16 );
	m_MaxBTN->ReGetButtonImage(".\\image\\lobby\\btn_max.spr",   16, 16 );
	m_MaxBTN2->ReGetButtonImage(".\\image\\lobby\\btn_max2.spr", 16, 16 );
	m_ExitBTN->ReGetButtonImage(".\\image\\lobby\\btn_exit.spr", 16, 16 );

	//�ϴ�
	m_ConfigBTN->ReGetButtonImage(".\\image\\lobby\\btn_config.spr",    71, 19 );
	m_Exit2BTN->ReGetButtonImage(".\\image\\lobby\\btn_exit2.spr",     107, 30 );
    m_CaptureBTN->ReGetButtonImage(".\\image\\lobby\\btn_capture.spr",  71, 19 );

	m_bActive = TRUE;
    
	//////////////////////////////////////////////////////////////////////////

	//m_ConfigBTN->SetPos( 804 ,   4 );
	//m_Exit2BTN->SetPos(   29 , 555 );
	m_bShowMakeRoomDlg = FALSE;

	//ä�� �ƹ�Ÿ�� �Ѹ���.
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
    m_nUserListTitleNum = 0;            // ����� ����Ʈ
    m_nCardOption = CO_CHOICE_MODE;     // ���Ƿ� ���� ������ �⺻ �пɼ��� ���̽� ���� �����.

    m_nJackpotBackFrm = 0;              // ���� ��� �̹��� ������ ��ȣ �ʱ�ȭ
    m_nJackpotBackFrmCnt = 0;           // ���� ��� �̹��� ������ ��ȣ�� �ٲٴµ� ����ϴ� �ð���� ī���� �ʱ�ȭ
    
    m_pWaitChat->Init();                // ���� ä��â �ؽ�Ʈ �ʱ�ȭ
    m_SCB_WaitChat->SetElem( 0 );
    m_SCB_WaitChat->StartPage();
    m_SCB_WaitRoom->StartPage();

	m_nCurrentMode = WAITROOM_MODE;

    m_IME_WaitChat->ClearBuffer();
    m_IME_WaitChat->InputBegin(true);

	//�������� ����Ʈ �ʱ�ȭ.
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
// ��� ���带 �����Ѵ�.
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
// ������ �޾��� ���� ó��
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

    // ä�� �Է��� ������.
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
	
	
	m_bMessageSend = FALSE;                                 // �� �޼����� �޴°��̴�.
	strcpy( m_szMessageRecieveUserNick , szSendUserNick );  // ���� ����� �������� �޴´�.
	strcpy( m_szGetMessageData , szMessage );               // ���� ����� �� �޼����� �޴´�.
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
		//ó�� ���� ī�� ���̽� �κ�.
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
				//ī����ġ�� �ٽ� ���ڸ��� ���´�.
				g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;

				for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.x = m_sRoomInUserPosition[0].pCardStartPos.x + ( i * CARD_DISTANCE_B );
					g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].Pos.y = m_sRoomInUserPosition[0].pCardStartPos.y;
				}
			}
		}

		//ó�� ���� ī�� ������ �κ�.
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
	
		//�����ϱ� 
		// '0':����, '1':üũ, '2':����, '3':��, '4':����, '5':��, '6':Ǯ, '7':����
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
		//���翣��Ű�� ����.
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
// ȭ���� ĸ���ؼ� ���� ���������� �ִ� ���丮�� capture ���丮�� �����Ѵ�.
//========================================================================

BOOL CMainFrame::ScreenCapture()
{
    char        szFileName[256];
    time_t      sTIME = time( NULL );
    struct tm  *tTIME = localtime( &sTIME );
    DWORD       dwCurrTime = GetTickCount();

    if(-1 == GetFileAttributes(".\\capture")) {     // Capture ���丮�� ������ �����.
        CreateDirectory(".\\capture", NULL);
    }

    sprintf(szFileName, ".\\capture\\%04d%02d%02d_%ld.bmp", tTIME->tm_year + 1900, tTIME->tm_mon + 1, tTIME->tm_mday, dwCurrTime);
    HRESULT rval = g_pCDXScreen->GetBack()->SaveAsBMP(szFileName);

    if (rval >= 0) {
        char szFullPath[512];
        sprintf( szFullPath, "%s%s�� ����Ǿ����ϴ�.", g_szProgPath, szFileName + 2 );

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
// ���� ����
//========================================================================

void CMainFrame::PlaySound( CDXSoundBuffer *pSound )
{
    if ( pSound == NULL || g_sClientEnviro.bOption[2] == FALSE || !g_bSoundCard ) {
        return;
    }

    pSound->Play();
}



//========================================================================
// ���� ���� �ε�
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
// ��ư �ε�
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
// ��������Ʈ �ε�
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
// ��ũ�� �� �ε�
//
// input:   szUpBTN = UP BUTTON ���ϸ�
//          szDownBTN = DOWN BUTTON ���ϸ�
//          szDragBTN = DRAG BUTTON ���ϸ�
//          pScrollBarRect = ��ũ�� �� ��ü ����
//          nUpDownBTNWidth = UP/DOWN BUTTON ���� ũ��
//          nUpDownBTNHeight = UP/DOWN BUTTON ���� ũ��
//          nDragBTNWidth = DRAG BUTTON ���� ũ��
//          nDragBTNHeight = DRAG BUTTON ���� ũ��
//          nViewNum = ȭ�鿡 ���̴� �׸��� ����
//          nMaxNum = ���� ������ �ִ� �׸��� ��
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

    int nUpDownBTNX = 0;                    // UP/DOWN ��ư X ��ǥ
    int nDragBTNX   = 0;                    // DRAG ��ư X ��ǥ
    int nUpBTNY     = 0;                    // UP ��ư Y ��ǥ
    int nDownBTNY   = 0;                    // DOWN ��ư Y ��ǥ
    int nDragBTNY   = 0;                    // DRAG ��ư Y ��ǥ

    
    nUpDownBTNX = pScrollBarRect->left;

    if (nUpDownBTNWidth > nDragBTNWidth) {  // UP/DOWN ��ư ���� DRAG ��ư�� ���� ���
        nDragBTNX = nUpDownBTNX + (( nUpDownBTNWidth - nDragBTNWidth ) >> 1);
    }
    else {                                  // ū ���
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
    COLORREF rgbRetVal = 0;     // SetTextColor()�� ���ϰ� ����
    BOOL bRetApiFunc = TRUE;    // Api �Լ��� ���ϰ� ����
    SIZE sTextSize;
    HDC hDC = NULL;
    int nTxtX = 0;
    int nTxtY = 0;
    int nStrLen = 0;            // ���ڿ� ����
    int nRetVal = 0;            // TextOut() �Լ��� ���ϰ� ����

    if (szText == NULL) {
        DTRACE2("CMainFrame::DrawText() : �׸� ���ڿ��� ����.");
        return;
    }

    nStrLen = lstrlen(szText);  // ���ڿ��� ���̸� ���Ѵ�.
    pSurface = g_pCDXScreen->GetBack();

	hDC = pSurface->GetDC();

    if ( hDC == NULL ) {
        DTRACE2("CMainFrame::DrawText() : ���ϵ� DC ���� NULL�̶� �ؽ�Ʈ�� ���׸��ڴ�!!, nStrLen = %d", nStrLen);
        return;
    }

    nRetVal = pSurface->ChangeFont("����ü", 0, nFontSize, nStyle);   // ��Ʈ �ٲ�

    if ( nRetVal < 0 ) {
        DTRACE2("CMainFrame::DrawText() : ��Ʈ �ٲٱ� ���� - ���ϰ� %d", nRetVal);
    }

    nRetVal = pSurface->SetFont();                                    // �ٲ� ��Ʈ ����

    if ( nRetVal < 0 ) {
        DTRACE2("CMainFrame::DrawText() : ��Ʈ ���� ���� - ���ϰ� %d", nRetVal);
    }

    bRetApiFunc = GetTextExtentPoint32(hDC, szText, nStrLen, &sTextSize); // ��Ʈ ũ�� ������

    if ( bRetApiFunc == FALSE ) {
        DTRACE2("CMainFrame::DrawText() : ��Ʈ ���� �������� ����");
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
        DTRACE2("CMainFrame::DrawText() : �ؽ�Ʈ ��� �����ϰ� �����ϱ� ���� - ���ϰ� %d", nRetVal);
    }

	rgbRetVal = SetTextColor(hDC, rgbColor);

    if ( rgbRetVal == CLR_INVALID ) {
        DTRACE2(" CMainFrame::DrawText() : �ؽ�Ʈ ���� ���� ����!!!!! - ���ϰ� %ld", rgbRetVal );
    }

	bRetApiFunc = ::TextOut(hDC, nTxtX, nTxtY, szText, nStrLen);
    
    if ( bRetApiFunc == 0 ) {
        DTRACE2(" CMainFrame::DrawText() : �ؽ�Ʈ ��� ����!!!!! - ���ϰ� %d", bRetApiFunc );
    }

    nRetVal = pSurface->ReleaseDC();

    if ( nRetVal < 0 ) {
        DTRACE2("CMainFrame::DrawText() : DC �����ϱ� ���� - ���ϰ� %d", nRetVal);
    }
}



void CMainFrame::ChangeVideoMode(BOOL bFullScreen)
{
    // ��üȭ�� ���� ����
	if (bFullScreen) {
		g_pCDXScreen->GetDD()->RestoreDisplayMode();
        AdjustWinStyle();
		if (FAILED(g_pCDXScreen->ChangeVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32))) {
			DTRACE("��üȭ����� �ٲ� �� �����ϴ�.");
            return;
		}
	}

    // â ���� ����
	else {
		g_pCDXScreen->GetDD()->RestoreDisplayMode();
		AdjustWinStyle();

        if (FAILED(g_pCDXScreen->ChangeVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0))) {
			DTRACE("â���ȭ������ �ٲ� �� �����ϴ�.");
		}

		CenterWindow();
	}

    //------------------------------------------------
    // !! ȭ�� ��� �ٲ�� ��� �̹����� �÷�Ű �缳��
    //------------------------------------------------
    SetColorKey();

    if ( m_pScrollNotice != NULL ) {
        m_pScrollNotice->Reload();
    }

    // �ƹ�Ÿ�� �����Ѵ�.
    RecoveryAvatar();
}



void CMainFrame::AdjustWinStyle()
{
	DWORD dwStyle;

    if ( m_bFullScreen ) {    // ���� ��üȭ�����̸�, ��üȭ�鿡 �°� ������ �Ӽ��� �ٲپ� �ش�.
		dwStyle = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
        ::SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	else {
		RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

		// ���� â���ȭ���̸�, â��忡 �°� ������ �Ӽ��� �����Ѵ�.
		dwStyle = GetWindowStyle(g_hWnd);
        
		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle); // �������� �Ӽ��� �ٲ۴�.
        AdjustWindowRectEx(&rect, GetWindowStyle(g_hWnd), ::GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));
        ::SetWindowPos(g_hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        ::SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}



void CMainFrame::DrawAvatarScale( CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight )
{
    pSprite->SetPos(nX, nY);
	pSprite->SetStretchWidth(nWidth);   // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
    pSprite->SetStretchHeight(nHeight); // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
    pSprite->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLKSTRETCHED);
}



//========================================================================
// ���� ä��â�� �׸���.
//========================================================================

void CMainFrame::RenderWaitChat()
{
    int nCnt = 0;                // �� ȭ�� �з��� ����ϱ� ���� ī����
    int nTxtX = 40;              // �ؽ�Ʈ ��� ��ǥ(X ��ǥ�� ����)
    int nTxtY = 0;               // �ؽ�Ʈ ��� ��ǥ(Y ��ǥ�� �� �� ���� ����)
    int nTxtStyle = 0;           // �ؽ�Ʈ ��Ÿ��
    STextInfo *pTextInfo = NULL; // �ؽ�Ʈ �� ���� ����Ű�� ������

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
// ����ȭ�� ä��â�� �׸���.
//========================================================================

void CMainFrame::RenderGameChat()
{
    int nCnt = 0;                // �� ȭ�� �з��� ����ϱ� ���� ī����
    int nTxtX = 20;              // �ؽ�Ʈ ��� ��ǥ(X ��ǥ�� ����)
    int nTxtY = 0;               // �ؽ�Ʈ ��� ��ǥ(Y ��ǥ�� �� �� ���� ����)
    int nTxtStyle = 0;           // �ؽ�Ʈ ��Ÿ��
    STextInfo *pTextInfo = NULL; // �ؽ�Ʈ �� ���� ����Ű�� ������

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

    // IME Ŀ�� ������ ó��
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
    // ���� Ű�Է� ó��
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
                // ����Ű�� �Էµ� ���
                //---------------------

                if ( nChar == VK_RETURN ) {
                    if ( !m_bShowMakeRoomDlg ) {
                        if ( m_IME_WaitChat->IsFocus() ) {
                            if ( g_pCMainFrame->SendChatTime( GTC() ) ) {
                                char *pIMEText;
                                pIMEText = m_IME_WaitChat->GetIMEText();
        
                                if (strlen( pIMEText ) > 0) {     // �Է��� ���� ���� ���� �߰�
                                    g_pClient->WaitChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, pIMEText , 0 );
                                    m_IME_WaitChat->ClearBuffer();
                                }
                            }
                        }

                        // �Է� ���°� �ƴ� �� '����'Ű�� ������ �Է� ���·� �����.
                        else {
                            m_SCB_WaitChat->EndPage();
                            m_IME_WaitChat->ClearBuffer();
                            m_IME_WaitChat->InputBegin( true );
                        }
                    }
                }

                //----------------------------------
                // ����Ű�� �ƴ� �ٸ� Ű�� ���� ���
                //----------------------------------

                else {
                    if ( m_bShowMakeRoomDlg ) {
                        // ��Ŀ���� �ִ� IME �ϳ� ���� �Էµ� Ű�� ó���Ѵ�.
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
                // ����Ű�� �Էµ� ���
                //---------------------

                if ( nChar == VK_RETURN ) {
                    char szChatCopy[512];
                    char *pIMEText = NULL;
                    
                    memset( szChatCopy, 0x00, sizeof( szChatCopy ));
                    pIMEText = m_IME_GameChat->GetIMEText();            // �Էµ� ���ڿ��� ���Ѵ�.

                    if ( m_IME_GameChat->IsFocus()) {
                        // �Է¹��� �� �ִ� �ð��̰�, �Է¹��� ���ڿ��� �ִ� ��쿡�� ����
                        if ( g_pCMainFrame->SendChatTime( GTC() ) && strlen( pIMEText ) > 0) {

#ifdef CHANGEUSERCARD
                            // ä��â���� �Է¹��� ���ڿ��� ��ɾ��̸� ���� ó���� �� ����
                            // ä�ù��ڿ� ó���� ���� �ʴ´�.
                            if ( CheckChatCommand( pIMEText ) ) {
                                m_IME_GameChat->ClearBuffer();
                                return;
                            }
#endif
                            if( strlen( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name ) > 14 ) {
							    memcpy( szChatCopy, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, 14 );
							    strcat( szChatCopy, "..." );
							    strcat( szChatCopy, "���� ��: " );
						    }
                            else {
                                sprintf( szChatCopy, "%s���� ��: ", g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );
                            }
						     
						    strcat( szChatCopy , pIMEText );
						    
                            g_pCMainFrame->SetGameChatText( szChatCopy, RGB( 204, 255, 255 ));
						    
						    g_pClient->GameChat( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name, szChatCopy , 0 );
                            m_IME_GameChat->ClearBuffer();
                        }
                    }

                    // �Է»��°� �ƴ� �� ����Ű�� ������ �Է»��·� �����.
                    // ����Ű�� ���� ������ ī�带 ������ �� �� �ִ� ��쿡�� ä��â�� �Է»��·� ������ �ʴ´�.
                    else if (g_CGameUserList.m_GameUserInfo[0].m_nCardCnt < MAX_USER_CARD) {
                        m_SCB_GameChat->EndPage();
                        m_IME_GameChat->ClearBuffer();
                        m_IME_GameChat->InputBegin( true );
                    }
                }

                //----------------------------------
                // ����Ű�� �ƴ� �ٸ� Ű�� ���� ���
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
            // CDXIME�� �Է°��ɻ��°�, ��Ŀ���� ���� ����
            // ���������� �Է� ó���� �Ѵ�.
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
            m_IME_GameChat->OnChar( nChar );        // ���������� ��Ŀ���� ���� ���� ó��
            return;
    }
    
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}



//========================================================================
// ���� �ε����� �����Ѵ�.
//
// 1. ������ ������ �� �ִ� ���� ���ʿ� ��ġ��Ų��.
// 2. �� �� ���� ���ʿ� ��ġ��Ų��.
//========================================================================

void CMainFrame::WaitRoomIndexRefresh()
{
    int i = 0;
    int nRoomIdx = 0;           // �� ���Ľ� �ε����� ���(HAM)
    memset(g_nWaitRoomIdx, 0x00, sizeof(g_nWaitRoomIdx));

    // 1. �� ���� ���� ���� �ε������� �����Ѵ�.
    for (i = 0; i < MAX_ROOMCNT; i++) {
        if ( g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt <= 0 ) continue;    // ������ ������ ó�� ����

        if (g_CWaitRoomList.m_sWaitRoomList[i].nTotCnt > g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt) {
            g_nWaitRoomIdx[nRoomIdx++] = i;
        }
    }

    // 2. �� ���� �� �� ���� �ε������� ���ʴ�� �����Ѵ�.
    for (i = 0; i < MAX_ROOMCNT; i++) {
        if ( g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt <= 0 ) continue;    // ������ ������ ó�� ����

        if (g_CWaitRoomList.m_sWaitRoomList[i].nTotCnt <= g_CWaitRoomList.m_sWaitRoomList[i].nCurCnt) {
            g_nWaitRoomIdx[nRoomIdx++] = i;
        }
    }

    g_nWaitRoomCnt = nRoomIdx;              // ���� �� ���� ����
    m_SCB_WaitRoom->SetElem( nRoomIdx );    // ���� ��ũ�� �� ��ü �׸� �� ����

    // ���� ������� ���� �������� ���� ���� ���
    // �� ����Ʈ�� ù �������� �̵��Ѵ�.
    if ( m_SCB_WaitRoom->GetStartElem() >= nRoomIdx ) {
        m_SCB_WaitRoom->StartPage();
    }

    // ������ ���� ���õǾ� �ִ� ���.
    if ( g_nWaitRoomSelIdx != -1 ) {
        // ���ùٰ� ���� �ִ� ��ġ�� �ִ� ���
        if ( g_nWaitRoomSelIdx < g_nWaitRoomCnt ) {
            m_nSelectedRoomNum = g_nWaitRoomIdx[ g_nWaitRoomSelIdx ];   // ���õ� �� ��ȣ ����
        }

        // ���ùٰ� ���� ���� ��ġ�� �ִ� ���
        else {
            m_nSelectedRoomNum = -1;
            g_nWaitRoomSelIdx = -1;
        }
    }
}



//========================================================================
// ���� ���� �ִ� ���� ������ �����Ѵ�.
//
// input:   nRoomNo     = ������ �� ��ȣ
//========================================================================

void CMainFrame::ViewRoomInfoRefresh( ush_int nRoomNo )
{
    if ( nRoomNo < g_nWaitRoomCnt && g_nWaitRoomSelIdx >= 0 && g_nWaitRoomSelIdx < g_nWaitRoomCnt ) {
        if ( g_nWaitRoomIdx[ g_nWaitRoomSelIdx ] == nRoomNo )
            g_pClient->GetRoomInfo( nRoomNo );
    }
}



//========================================================================
// ä��â�� �ؽ�Ʈ�� �߰��Ѵ�.
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
    ::SetFocus( g_hWnd );           // CHttpView�� ���� Ű �Է��� �ȵǴ� ���� ����
}
*/



//========================================================================
// ���� ����� ������.
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
    srand( (unsigned)time( NULL ));                     // seed �ʱ�ȭ

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
    // ��ü ���� ������ ���Ѵ�.
    //-------------------------

    while ( nTempMoney > 0 ) {
        nCnt += (int)( nTempMoney % 10 );
        nTempMoney /= 10;
    }

    //------------------------------------
    // ���� ������ ��ġ�� ������ �����Ѵ�.
    //------------------------------------

    if ( nCnt > 5 ) {   // ���� 5�� ���� ���� ��
        nBaseX = 412;
        nBaseY = 130;
        nWidth = 200;
        nHeight = 200;
    }
    else {              // ���� 5�� ������ ��
        nBaseX = 462;
        nBaseY = 180;
        nWidth = 100;
        nHeight = 100;
    }

    nTempMoney = nMoney;
    nCnt = 0;

    while ( nTempMoney > 0 ) {
        nCnt = (int)( nTempMoney % 10 );                // ���� ������ �� ������ ���Ѵ�.(���� ���� �������� ����)

        for ( int i = 0; i < nCnt; i++ ) {              // �� ������ŭ ���ϸ��̼� ���ÿ� �߰�
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
// ������ ���� ����� �׸���.
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
// ���� ī��� ������ ī�带 �����ϴ� ��ȭ���ڸ� �׸���.
//========================================================================

void CMainFrame::RenderCardSelectionDlg()
{
    BYTE nCardNo = 0;

    //���� ȭ���� ���߿� ���� �ٸ� ī�忡 �Ȱ�������.
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
// ��������Ʈ�� �׸���.
//
// input:
//      pSprite     = ����� ��������Ʈ
//      nX, nY      = ����� ��ǥ
//      nFrame      = ��������Ʈ ������ ��ȣ
//========================================================================

void CMainFrame::PutSprite( CDXSprite *pSprite, int nX, int nY, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
}




//========================================================================
// �÷�Ű�� �ʿ���� �̹��� ��¿� ���
//
// input:
//      pSprite:    ����� ��������Ʈ
//      nX, nY:     ����� ��ǥ
//      nFrame:     ��������Ʈ ������ ��ȣ
//========================================================================

void CMainFrame::PutSpriteBlk( CDXSprite *pSprite, int nX, int nY, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );
}



//========================================================================
// ��������Ʈ�� �׸���.
//
// input:
//      pSprite     = ����� ��������Ʈ
//      nX, nY      = ����� ��ǥ
//      srcReect    = �������� ����� ���� ����
//========================================================================

void CMainFrame::PutSprite( CDXSprite *pSprite, int nX, int nY, RECT *srcRect )
{
    pSprite->GetTile()->DrawTrans( g_pCDXScreen->GetBack(), nX, nY, srcRect );
}



//========================================================================
// �̹����� Ȯ�� �Ǵ� ����ؼ� ����Ѵ�.
//========================================================================

void CMainFrame::DrawScale(CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame)
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos(nX, nY);
	pSprite->SetStretchWidth(nWidth);   // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
    pSprite->SetStretchHeight(nHeight); // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
    pSprite->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANSSTRETCHED);
}



//========================================================================
// ���� ����(WM_CLOSE �޽����� ������.)
//========================================================================

void CMainFrame::ExitGame()
{
    g_bRealExit = true;     // ���α׷����� �����ٴ� ǥ��( �������Ḧ �����ϱ� ���� ���)
    ::PostMessage( g_hWnd, WM_CLOSE, 0, 0 );
}



//========================================================================
// ���α׷� ���� ���� ����
//
// WM_CLOSE �޽��� �߻��� ���α׷����� �����û�� �� ���̸�
// �����Ű��, �ܺο��� ���������Ϸ��� �õ��ߴ� ���̸�
// �޽����� �����Ѵ�.
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
// ȭ���� ��ȯ������ ���� ǥ�� �ı��� �ƹ�Ÿ �̹����� �����Ѵ�.
// ���ϸ��̼� GIF�� ��� �ڵ����� ��� �׷��ֹǷ� ���ϸ��̼� GIF��
// �ƴ� ��쿡�� �������ָ� �ȴ�.
//========================================================================

void CMainFrame::RecoveryAvatar()
{
    // ���� �ƹ�Ÿ ����
    if ( m_SPR_WaitAvatar != NULL ) {
        if ( m_WaitAvaImg != NULL ) {
            if ( !m_WaitAvaImg->IsAnimatedGIF() ) {
                m_WaitAvaImg->InitAnimation( g_hWnd, CPoint(0, 0) );
            }
        }
    }

    // ���ӹ� �ƹ�Ÿ ����
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
// ä�� ���ڿ��� �м��ؼ� ��ɾ��̸� ���� ó���ؼ� ������ ��Ŷ�� ������
//
// input:
//      szText : ä��â���� �Է��� ���ڿ��� ������
//
// output:
//      TRUE = �Է��� ���� ��ɾ��̰� ���������� ó���� �Ͽ���.
//      FALSE = �Է��� ���� �Ϲ� ä�� ���ڿ��̰ų�, �߸��� ��ɾ��̴�.
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

    // EOF(-1)�� �ƴϰ� 3�� �� ����� �Ľ��� ���
    if (nParamNum >= 3) {

        if ((strcmp(szCommBuf, "/C") == 0) || (strcmp(szCommBuf, "/c") == 0))
        { 
            // ���⼭ �ٲ��ڸ� ��ȣ�� ��ī�� ��ȣ ����
            // ������ ���̸� ������ ī�� �ٲٱ� ��Ŷ�� ������.
            DTRACE2("��ɾ��.... %s, %s, %s", szCommBuf, szParam1, szParam2);

            SendChangeUserCardInfo( szParam1, szParam2 );
                     
            return TRUE;
        }
        // "/"�� �����ϴ� ���ڿ��� ä�� ó������ �ʴ´�.
        else if ( strlen(szCommBuf) >= 1 && szCommBuf[0] == '/' ) {
            SetGameChatText( "�� �߸��� ��ɾ��Դϴ�." , RGB( 255, 255, 255 ));
            return TRUE;
        }
    }
    else if ( nParamNum >= 1 ) {
        // �߸��� ��ɾ�
        if ( strlen(szCommBuf) >= 1 ) {
            if ( szCommBuf[0] == '/' ) {
                SetGameChatText( "�� �߸��� ��ɾ��Դϴ�." , RGB( 255, 255, 255 ));
                return TRUE;
            }
        }
    }

    // �Ϲ� ä�� �޽���
    return FALSE;
}

#endif



#ifdef CHANGEUSERCARD

//========================================================================
// ���޵� ������ �̿��Ͽ� ������ ī�带 �� ī��� �ٲ۴�.
//
// input:
//        typedef struct tagUserChange
//        {
//            BYTE bUerCardNo;      // �� ī�� ��ȣ
//            BYTE bChangeCardNo;   // ���� �ٲ� ī�� ��ȣ
//        } sUserChange;
//
//========================================================================

void CMainFrame::ChangeUserCard( int nSlotNo, sUserChange *pChgCardInfo )
{
    if ( pChgCardInfo == NULL ) {
        return;
    }

    // ī�带 ã�Ƽ� ���ο� ī��� �ٲ��ش�.

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
// ������ �ٲ� ī�� ������ ������.
//
// input:
//      szParam1 : �ٲ� �� ī�� ��ȣ
//      szParam2 : ���� ���� ī�� ��ȣ
//========================================================================

void CMainFrame::SendChangeUserCardInfo(char *szParam1, char *szParam2)
{
    bool bExistCard = false;
    int nCurCardNo = ConvCardNum( szParam1 );   // �ٲٰ� ���� ������ ī�� ��ȣ
    int nNewCardNo = ConvCardNum( szParam2 );   // ���� �ް� ���� ī�� ��ȣ

    DTRACE2 (" :::::ī���ȣ : %d, %d", nCurCardNo, nNewCardNo );

    // ��� �ϳ��� �߸��� �����̸� ī�带 �ٲ��� �ʴ´�.
    if ( nCurCardNo == -1 || nNewCardNo == -1 ) {
        SetGameChatText( "�� �߸��� ��ɾ��Դϴ�." , RGB( 255, 255, 255 ));
        return;
    }

    for ( int i = 0; i < g_CGameUserList.m_GameUserInfo[0].m_nCardCnt; i++ ) {
        if ( g_CGameUserList.m_GameUserInfo[0].m_sUserCardInfo[i].nCardNo == (BYTE)nCurCardNo ) {
            if ( i < 2 || i == 6 ) {
                // ī�带 �ٲ۴�.
                sUserChange l_sTempCardInfo;
                memset( &l_sTempCardInfo, 0x00, sizeof( l_sTempCardInfo ));
                l_sTempCardInfo.bUerCardNo = (BYTE)nCurCardNo;
                l_sTempCardInfo.bChangeCardNo = (BYTE)nNewCardNo;

                g_pClient->UserChange( &l_sTempCardInfo );
            }
            else {
                SetGameChatText( "�� �� ī��� �ٲ� �� �����ϴ�." , RGB( 255, 255, 255 ));
            }

            bExistCard = true;
            break;
        }
    }

    if ( !bExistCard ) {
        SetGameChatText( "�� ���� ī���Դϴ�." , RGB( 255, 255, 255 ));
    }
}

#endif



#ifdef CHANGEUSERCARD

//========================================================================
// �Է¹��� ī������ ���ڿ��� ī�� ��ȣ�� ��ȯ�Ѵ�.
//
// input:
//      szCardInfo : ī�� ���� ���ڿ�(C2 = Ŭ�ι�2, SK = �����̵� ŷ, HA = ��Ʈ ���̽�
//
// return:
//      -1 : ī�� ���� ���ڿ��� �ƴϰų� �߸��� ����
//      0 �̻� : ī�� ��ȣ
//      
//========================================================================

int CMainFrame::ConvCardNum( char *szCardInfo )
{
    if ( szCardInfo == NULL ) {
        return -1;
    }

    int nLen = strlen( szCardInfo );    // ���ڿ��� ���̸� ���Ѵ�.

    // ī�� ��ȣ ���ڿ��� 2~3 ���ڷ� �Ǿ� �ִ�.
    if ( nLen < 2 || nLen > 3 ) {
        return -1;
    }

    int nCardNo = 0;                // ī�� ��ȣ

    // �ش� ������ ī���� ���۹�ȣ�� ���Ѵ�.

    switch ( szCardInfo[0] ) {
        // �����̵�
        case 'S':
        case 's':
            nCardNo = 13 * 3;            // �ش� ������ ù ī�� ��ȣ
            break;

        // ���̾�
        case 'D':
        case 'd':
            nCardNo = 13 * 2;       // �ش� ������ ù ī�� ��ȣ
            break;

        // ��Ʈ
        case 'H':
        case 'h':
            nCardNo = 13 * 1;       // �ش� ������ ù ī�� ��ȣ
            break;

        // Ŭ�ι�
        case 'C':
        case 'c':
            nCardNo = 0;            // �ش� ������ ù ī�� ��ȣ
            break;

        default:
            return -1;
    }

    // 3������ ��� 10�� ī������ �˻�
    if ( nLen == 3 ) {
        if ( szCardInfo[1] == '1' && szCardInfo[2] == '0' ) {
            nCardNo += 8;           // ��Ŀ������ 8�� 10�� ī��
        }
        // 3���� �̰� "10"�� �ƴ� ���� �߸��� ����
        else {
            return -1;
        }
    }
    // 2������ ��� 10 ī�带 ������ ������ ī�� �˻�
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

    // ���������� ��ȯ�� ī���ȣ ����
    return nCardNo;
}

#endif
