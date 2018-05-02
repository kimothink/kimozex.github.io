// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NewGostopClient.h"

#include "MainFrm.h"
#include "dtrace.h"
#include "..\NetworkDefine\serverclientcommon.h"
#include <io.h>
#include <math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern int g_nMakeRoomNo;       // �� ����� ��ư�� ������ �� �� ���� ��ȣ�� ����
extern int g_nWaitRoomCnt;
extern char szProgPath[512];
extern sUserInfo g_ClickRoomUserInfo[MAX_ROOM_IN];
extern sMissionCardInfo g_sMissionCardInfo;

extern ush_int g_nServiceDiv;
extern char g_szTempParam[256];
extern bool g_bSoundCard;
extern bool g_bRefreshTransDlg;
extern bool g_bRealExit; // true�� ���α׷����� ���� ��û�� ������ �Ǵ�, �������� ������ ���

char g_RoomTitle[5][20] = 
{   
    "�����սô�.", 
    "����� �޲ٸ�", 
	"�ٴ������ƿ�!",
	"�ų��� ���Ǵ޷�!",
	"�ųʰ��� �սô�."
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame
#include "GlovalVar.h"
#include <Wininet.h>

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_CLOSE()
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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
//////////////////////////////////////////////////////////////////////////
//
// CDXChat ��ȭ������ �׸��� ����ְ� �����Ѵ�.
//
//////////////////////////////////////////////////////////////////////////

CDXChat::CDXChat( int Y )//( int Y )
{
	for( int i = 0 ; i < MAX_CHAT_COUNT ; i ++ )
	{
		strcpy( Talk[i].szChat , "" );		
		Talk[i].bShow = FALSE;
		Talk[i].Y = Y;
		TempY = Y;
		Count = 0;
	}
}

CDXChat::~CDXChat()
{
}

void CDXChat::DrawTalks( int X , int MaxY )
{	
	for( int i = 0 ; i < MAX_CHAT_COUNT ; i++ )
	{
		if( strcmp( Talk[i].szChat , "" ) == 1 )
		{
			if( Talk[i].Y < MaxY )
			{
				Talk[i].bShow = FALSE;				
				strcpy( Talk[i].szChat , "" );				
				Talk[i].Y = TempY;
			}

			if( Talk[i].bShow )
                g_pCMainFrame->DrawText( Talk[i].szChat, X, Talk[i].Y, Talk[i].rgb, FW_NORMAL, TA_LEFT );
		}
	}
}

void CDXChat::SetText( char *szText, COLORREF rgb )
{	

	int nMaxLine;

	int nlen = 0 , nCopySize = 0;

	char szJunkText[512] , szPrintText[512];
	int nLineCount = 0;
	memset( &szJunkText , 0x00 , sizeof( szJunkText ));
	memset( &szPrintText , 0x00 , sizeof( szPrintText ));

	strcpy( szJunkText , szText );

	nlen = strlen( szJunkText );

	switch( g_pCMainFrame->l_nCurrentMode )
	{
		case WAITROOM_MODE:
			
			nMaxLine = 36;
			break;
		case GAME_MODE:

			nMaxLine = 22;
			break;
	}

	int nBufPtr = 0;

	if( nlen > nMaxLine )
	{
		for( int t = 0 ; t < nlen ; t += nMaxLine )
		{
			nLineCount ++;
		}
		
		int nSaveSize = 0;

		for( int b = 0 ; b < nLineCount ; b ++ )
		{
			nSaveSize = nCopySize;
			nCopySize = 0;

			for( int k = ( nSaveSize * b ) ; k < ( nSaveSize * b ) + nMaxLine; k ++ )
			{
				if( szJunkText[k] == NULL )								
					break;			
				
				if( szJunkText[k] & 0x80 ) // �ѱ��̸�
				{
					nCopySize += 2;
					k ++;
				}
				else // �ѱ��� �ƴϸ�..
				{
					nCopySize += 1;
				}
			}
			
			memset( &szPrintText , 0x00 , sizeof( szPrintText ));
			memcpy( szPrintText , szJunkText + nBufPtr , nCopySize );
			nBufPtr += nCopySize;			
			
			if( szJunkText[nBufPtr] == NULL )
			{
				Talk[Count].bShow = TRUE;
				
				strcpy( Talk[Count].szChat , szPrintText );
				Talk[Count].rgb = rgb;
				
				Count++;
				
				if( Count >= MAX_CHAT_COUNT )
					Count = 0;
				
				MoveTalks();
				return;
			}

			Talk[Count].bShow = TRUE;
			
			strcpy( Talk[Count].szChat , szPrintText );
			Talk[Count].rgb = rgb;
			
			Count++;
			
			if( Count >= MAX_CHAT_COUNT )
				Count = 0;

			MoveTalks();			
		}	

	}
	else // �� �̸��� ���
	{
		Talk[Count].bShow = TRUE;
		
		strcpy( Talk[Count].szChat , szText );
		Talk[Count].rgb = rgb;
		
		Count++;
		
		if( Count >= MAX_CHAT_COUNT )
			Count = 0;

		MoveTalks();
	}
}

void CDXChat::MoveTalks( void )
{
	for( int i = 0 ; i < MAX_CHAT_COUNT ; i ++ )
	{
		if( Talk[i].bShow )
			Talk[i].Y -= 18;
	}
}

void CDXChat::ClearAll( void )
{
	for( int i = 0 ; i < MAX_CHAT_COUNT ; i ++ )
	{
		memset( Talk[i].szChat , 0 , sizeof( Talk[i].szChat ));
		Talk[i].Y = TempY;
		Talk[i].bShow = FALSE;
		Count = 0;
	}
}



CONFIGBOX::CONFIGBOX( BOOL btInvite , BOOL btSecretMessage , BOOL btWaveSnd , BOOL btMessageAgree , int ntVoice )
{
	bCheck[0] = btInvite;
	bCheck[1] = btSecretMessage;
	bCheck[2] = btWaveSnd;
	bCheck[3] = btMessageAgree;
	nVoice = ntVoice;
}



//========================================================================
// �ɼ� â�� �׸���.
//========================================================================

void CONFIGBOX::RenderConfigBox( void )
{	
    g_pCMainFrame->PutSprite( g_pCMainFrame->l_SPR_ConBox, CONFIG_POS_X, CONFIG_POS_Y );
	g_pCMainFrame->l_ConfigOkBTN->DrawButton();

	for( int i = 0; i < 4; i++ ) {
		if( g_pCMainFrame->l_ConfigBox->bCheck[i] == TRUE ) {
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_POS_X + 100, CONFIG_POS_Y + 60 + ( i * 27 ), 1 );
		}
		else {
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_POS_X + 205, CONFIG_POS_Y + 60 + ( i * 27 ), 1 );
		}
	}

	switch( nVoice ) {
		case 0: // ����1
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_POS_X + 205, CONFIG_POS_Y + 168, 1 );
            break;

		case 1: // ����1
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_POS_X + 100, CONFIG_POS_Y + 168, 1 );
			break;

		case 2: // ����2
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_POS_X + 100, CONFIG_POS_Y + 187, 1 );
            break;

		case 3: // ����2
            g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_CheckBall, CONFIG_POS_X + 205, CONFIG_POS_Y + 187, 1 );
			break;
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





void CDXTime::Init(double tfps)
{
	targetfps = tfps;
	
	QueryPerformanceCounter(&framedelay);
	QueryPerformanceFrequency(&tickspersecond);
}

void CDXTime::SetSpeedFactor()
{
	QueryPerformanceCounter(&currentticks);
	
	speedfactor = (double)(currentticks.QuadPart-framedelay.QuadPart)/((double)tickspersecond.QuadPart/targetfps);
	
	fps = targetfps/speedfactor;
	
	if(speedfactor <= 0) speedfactor = 1;
	
	framedelay = currentticks;
} 



/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	g_pCMainFrame = this;

	l_SPR_WaitBase = NULL;
	l_SPR_GameBase = NULL;
	l_SPR_MakeRoom = NULL;
	l_SPR_Result = NULL;
	l_SPR_HwatuBackSmall = NULL;
	l_SPR_Hwatu = NULL;
	l_SPR_MissionTimes = NULL;
	l_SPR_Test = NULL;
	l_SPR_Cover = NULL;
	l_SPR_BombLine = NULL;
	l_SPR_Dialog = NULL;
	l_SPR_Cursor = NULL;
	l_SPR_Cursor2 = NULL;
	l_SPR_EVENT = NULL;
	l_SPR_Connting = NULL;
	l_SPR_Wait = NULL;
	l_SPR_NotifyText = NULL;
	l_SPR_HwatuSelect = NULL;
	l_SPR_Time = NULL;
	l_SPR_LoGo = NULL;
	l_SPR_WaitAvatar = NULL;
	l_SPR_Start = NULL;
	l_SPR_Baks = NULL;          // ����, �ǹ�, ���ڵ�.
	l_SPR_DlgBar = NULL;        // ���̾� �α׿� ����ϴ� �� �� �� ǥ��
	l_SPR_P_Back = NULL;        // �� ���� ���
	l_SPR_FullWait = NULL;
	l_SPR_ConBox = NULL;        // ���� â �׸� ����
	l_SPR_KingFirst = NULL;     // ����� �� �׸�
	l_SPR_InviteBox = NULL;     // ���� �ʴ� �ڽ� �׸�
	l_SPR_InviteIcon = NULL;    // ���� �ʴ� �ڽ� �׸��� ����, ���� ������
	l_SPR_MessageBox = NULL;    // ���� â
	l_SPR_Invitation = NULL;    // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? â )
	l_SPR_UserState = NULL;     // ���콺 ������ ��ư���� ���� �κ��� ������ �ߴ� â �׸�
	l_SPR_Message = NULL;       // �޼��� �ڽ� �׸�
	l_SPR_AffairNum = NULL;     // ������ ���� �̹��� ( ��, ��,  ��� ��..)
	
	l_SPR_Jumsu = NULL;
	l_SPR_MissionOK = NULL;
	l_SPR_Class = NULL;
	l_SPR_ResultMessage = NULL;
	l_SPR_ResultDojang = NULL;
	l_SPR_MissionNext = NULL;
    m_SPR_ResultNum[0] = NULL;
    m_SPR_ResultNum[1] = NULL;

    m_SPR_UserListTitle[0] = NULL;
    m_SPR_UserListTitle[1] = NULL;
    m_SPR_ExitReserv = NULL;
    m_SPR_WaitDlg = NULL;
    m_SPR_CheckBall = NULL;
    m_SPR_HighlightBar = NULL;
    m_SFC_DialogBack = NULL;    //[alpha]
    m_SPR_PassDlg = NULL;
    m_SPR_CardBlink = NULL;
    m_SPR_ChodaeOX = NULL;
    m_SPR_RoomBack = NULL;  // ham 2006.01.18
    l_SPR_COM = NULL;
    m_SPR_UserIcon = NULL;  // ham 2006.01.18

	l_MinBTN = NULL;
	l_MaxBTN = NULL;
	l_MaxBTN2 = NULL;
	l_ExitBTN = NULL;
	l_StartBTN = NULL;
	l_MakeRoomOkBTN = NULL;
	l_MakeRoomCancelBTN = NULL;
	l_GoBTN = NULL;             // 01.08
	l_StopBTN = NULL;           // 01.08
	l_GYESBTN = NULL;           // 01.12
	l_GNOBTN = NULL;            // 01.12
	l_ConfigBTN = NULL;         // 01.13 ���� ��ư( ���ӷ� )
	l_Exit2BTN = NULL;          // 01.14 ������ ��ư ( ���ӷ� )
	l_ConfigOkBTN = NULL;          // 01.14 ��� Ȯ�� ��ư( ���� �� )
    
    m_MakeRoomBTN[0] = NULL;
    m_MakeRoomBTN[1] = NULL;
    m_MakeRoomBTN[2] = NULL;
    m_MakeRoomBTN[3] = NULL;
    m_MakeRoomBTN[4] = NULL;
    m_MakeRoomBTN[5] = NULL;
    m_MakeRoomBTN[6] = NULL;
    m_MakeRoomBTN[7] = NULL;
    m_MakeRoomBTN[8] = NULL;

    m_JoinBTN[0] = NULL;
    m_JoinBTN[1] = NULL;
    m_JoinBTN[2] = NULL;
    m_JoinBTN[3] = NULL;
    m_JoinBTN[4] = NULL;
    m_JoinBTN[5] = NULL;
    m_JoinBTN[6] = NULL;
    m_JoinBTN[7] = NULL;
    m_JoinBTN[8] = NULL;

	l_ResultOKBTN = NULL;       // ���â OK ��ư
	l_InviteBTN = NULL;         // ���� �ʴ� ��ư
	l_InviteOKBTN = NULL;       // ���� �ʴ� �ڽ����� OK ��ư
    l_InviteCancelBTN = NULL;   // ���� �ʴ� �ڽ����� Cancel ��ư
	l_MemoSendBTN = NULL;       // ���� â ������ ��ư
	l_RecvChodaeOkBTN = NULL;   // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? â & �ʴ� ���� â ) Ȯ�� ��ư
	l_MemoCancelBTN = NULL;     // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? ); X ��ư
	l_RecvChodaeCancelBTN = NULL;  // �ʴ� â ( �ʴ��Ͻðڽ��ϱ�? ); ��� ��ư
	l_UserStateBTN = NULL;      // �� ���� ��ư
	l_MessageOKBTN = NULL;      // �޼��� �ڽ� OK ��ư
    l_CaptureBTN = NULL;        // ȭ�� ĸó ��ư
    m_RoomTitleBTN = NULL;
    m_PassDlgOkBTN = NULL;
    m_PassDlgCancelBTN = NULL;

    m_sSmallZoomTable = NULL;
    m_sBigZoomTable = NULL;

    m_pScrollNotice = NULL;
    m_pWaitChat = NULL;
    m_pGameChat = NULL;

    m_SCB_WaitChat = NULL;
    m_SCB_GameChat = NULL;
    m_SCB_WaitUser = NULL;
    m_SCB_WaitRoom = NULL;
    m_SCB_Invite = NULL;

    m_IME_WaitChat = NULL;
    m_IME_GameChat = NULL;
    m_IME_InputRoomTitle = NULL;
    m_IME_RoomInPassBox = NULL;
    m_IME_InputPass = NULL;
    m_IME_Memo = NULL;

    m_SPR_Arrow = NULL;
    m_SPR_MissionDlg = NULL;
    l_SPR_MComplete = NULL;
    m_SPR_SelSunText = NULL;
    m_SPR_SelSunTextBack = NULL;

	l_nDrawEventType = 0, l_nDrawEventSlotNo = 0, l_nDrawEventFrameCnt = 0, l_nDrawEventWaitTime = 0;
	l_nDialogType = 0, l_nPercentage = 0;
	l_nAniEventType = 0;
	l_EventTime = 0;
	l_nNameNo = 0; // �� ���� ��ȣ
	l_nCurName = 0; // 
	l_nAniMaxCount = 0;
	l_nOldSlotNo = 0;
	l_nFloorCnt = 0;
	l_nUserNo = 0;
	l_nTimeValue = 0;
	l_nRoomNo = 0;
	l_nOldSelNo = 0;
	l_nCurRoom = 0;
	l_nBombLineFrameCnt = 0;
	l_nCardFlipFrameCnt = 0;
	l_nDialogWaitFrameCnt = 0;
	l_nDialogResultFrameCnt = 0, l_nDialogResultWaitTime = 0;
	l_nDialgType = 0;
	l_nDlgMode = 0;
	l_mX = 0, l_mY = 0, l_sX = 0, l_sY = 0, l_cX = 0, l_cY = 0; // ���콺 ��ġ ����� ������
	nTempTic1 = 0, nTempTic2 = 0;
	CurT1 = 0 , CurT2 = 0 , CurTime = 0;
	nChatTime = 0; // ä�� ���� �ð�
	l_nViewRoomMessage = 0; // ��� ���� �������� ����.
	l_nViewRoomUser = 0; // ��� ���� ������ �������� ����.
	l_nViewWaitUser = 0; // ������ ������ ���ȴ��� ����.
	l_nOldViewRoomUser = 0; // ��� ���� ������ �������� ����.
	l_nRoomUserNo = 0; // ���� ���� ���� �˾Ƴ���.
	l_nCurRoomUser = 0; // ���� ���� ���� �˾Ƴ���
	l_nWaitUserNo = 0; // ������ ���� ���� �˾Ƴ���
	l_nPageStart = 0;  // ���� ���� �ִ� �������� ���� ��ȣ
	l_nWaitUserCurCnt = 0; // ���� ���� �����
	nDegree = 0; // ������� ������ ���ΰ�.
	g_nPuzzleNo = 0;
	l_nScrollSelNo = 0; // ��ũ�ѿ��� ������ ���� �ڷ� ��ȣ
	l_nMBoxTempX = 0 , l_nMBoxTempY = 0;
	l_TempMX = 0 , l_TempMY  = 0;
	l_nInviteTempX = 0, l_nInviteTempY = 0;
	l_nWaitViewPageNo = 0;
	l_nInviteRoomNo = 0; // �ʴ�� ������ �� ��ȣ
	l_nCurMashine = 0;
	l_nCurChannelNo = 0;
	l_nStartBtnCnt = 0;
	l_StartBTNState = 0;
	l_StartT1 = 0 , l_StartT2 = 0 , l_StartAni = 0;
	m_nCurSorCnt = 0;
	m_nSunRenderDecided = 0;
	m_nSunSetRender = 0;
    m_nHwatuSelectCnt = 0;
    m_nHwatuSelectFrm = 0;
	l_nTempCardNo = -1;
	l_nCurrentMode = LOGO_MODE;
	l_nSecPos = 9;
	l_TempAlpha = 32;
	l_nMBoxXPos = MEMO_DLG_POS_X;
    l_nMBoxYPos = MEMO_DLG_POS_Y;
	l_nRButtonWaitUserNo = -1;
	l_nInviteXPos = CHODAE_RECV_POS_X;      // �ʴ�޾��� ���� ��ȭ���� ��ǥ
    l_nInviteYPos = CHODAE_RECV_POS_Y;      // �ʴ�޾��� ���� ��ȭ���� ��ǥ
	l_nOldViewWaitUser = -1;                // ������ ������ ���ȴ��� ����.
	l_nOldViewRoomMessage = -1;
	nRoomX = 298;
    nRoomY = 248;                           // ���� X , Y ��ǥ
	l_nMouseOverCardNo = -1;
	l_nWaitTime = 600;
	m_nMakeRoomMoneyNo = 0;
	l_biInviteRoomMoney = 0;
	m_nMySunChioce = -1;
	m_nOtherSunChioce = -1;
    m_nHighlightBarNum = -1;
    m_nUserListTitleNum = 0;
    m_nSelectedRoomTitleList = -1;
    m_nCardBlinkCnt = 0;
    m_nCardBlinkFrm = 0;

    m_rgbMakeRoomTitleListBorder = 0;
    m_rgbMakeRoomTitleListBar = 0;

	l_nOrder[0] = 0;
	l_nOrder[1] = 11;
	l_nOrder[2] = 2;
	l_nOrder[3] = 9;
	l_nOrder[4] = 3;
	l_nOrder[5] = 8;
	l_nOrder[6] = 6;
	l_nOrder[7] = 5;
	l_nOrder[8] = 1;
	l_nOrder[9] = 10;
	l_nOrder[10] = 4;
	l_nOrder[11] = 7;

    m_fpPutHwatuZoom = NULL;

	l_sMakeRoomMoney[0] = SetPoint( MAKEROOM_POS_X +  29, MAKEROOM_POS_Y + 153 );
	l_sMakeRoomMoney[1] = SetPoint( MAKEROOM_POS_X + 169, MAKEROOM_POS_Y + 153 );
	l_sMakeRoomMoney[2] = SetPoint( MAKEROOM_POS_X +  29, MAKEROOM_POS_Y + 174 );
	l_sMakeRoomMoney[3] = SetPoint( MAKEROOM_POS_X + 169, MAKEROOM_POS_Y + 174 );
	l_sMakeRoomMoney[4] = SetPoint( MAKEROOM_POS_X +  29, MAKEROOM_POS_Y + 195 );
	l_sMakeRoomMoney[5] = SetPoint( MAKEROOM_POS_X + 169, MAKEROOM_POS_Y + 195 );
    l_sMakeRoomMoney[6] = SetPoint( MAKEROOM_POS_X +  29, MAKEROOM_POS_Y + 216 );
	l_sMakeRoomMoney[7] = SetPoint( MAKEROOM_POS_X + 169, MAKEROOM_POS_Y + 216 );

    // ���� �����ϱ� ���� ����ϴ� �и� ���� ��ġ

    m_szSunPackPoint[0]  = SetPoint( 370, 210 );
    m_szSunPackPoint[1]  = SetPoint( 460, 250 );
    m_szSunPackPoint[2]  = SetPoint( 555, 296 );
    m_szSunPackPoint[3]  = SetPoint( 555, 406 );
    m_szSunPackPoint[4]  = SetPoint( 460, 453 );
    m_szSunPackPoint[5]  = SetPoint( 370, 490 );
    m_szSunPackPoint[6]  = SetPoint( 280, 453 );
    m_szSunPackPoint[7]  = SetPoint( 185, 406 );
    m_szSunPackPoint[8]  = SetPoint( 185, 296 );
    m_szSunPackPoint[9]  = SetPoint( 280, 250 );

	l_bDrawEvent        = FALSE;
    l_bShowDialog       = FALSE;
    l_bShowResult       = FALSE;
	l_bTimeEvent        = FALSE;
	l_bFullScreen       = FALSE;
	l_bActive           = FALSE;
	
	l_bMoveDlg          = FALSE;
	l_bShowStartBTN     = FALSE;
	l_bShowBombLine     = TRUE;
	l_bMouseDown        = FALSE;
	l_bShowTurnLine     = FALSE;
	m_bSunSet           = FALSE;
	
	l_bShowConfigBox    = FALSE;    // ���� â�� ����°�.
	l_bShowUserState    = FALSE;    // ������ ���� ������ ������ , ���� ���� ������ ������ �����Ѵ�
	l_bShowInviteBox    = FALSE;    // ���� �ʴ� �ڽ��� ���� �ݴ� ����
	l_bShowMessageBox   = FALSE;
    l_bMBoxMoveDlg      = FALSE;
	l_bMessageSend      = TRUE;     // ������ �����°��� �޴� ���� üũ // TRUE = ������. FALSE = �޴´�.
	l_bUserState        = FALSE;
	l_bRButtonDown      = FALSE;
	l_bShowChodaeBox    = FALSE;    // �̰� TRUE �̸� �ʴ�ڽ��� ����.
	l_bChodaeDlg        = FALSE;    // ���콺�� �ʴ�â�� �����δ�. = TRUE; ( Drag ȿ�� )
	l_bStartFadeOut[0]  = FALSE;
	l_bStartFadeOut[1]  = FALSE;
	l_bStartFadeOut[2]  = FALSE;
	l_bShowMission      = FALSE;    // �̼� ��� ����
	l_bShowMakeRoomDlg  = FALSE;	
	
    m_bMouseEvent       = TRUE;
	m_bMouseRender      = TRUE;
	m_UserTimerThread   = FALSE;
	m_bSunSetRender     = FALSE;
	m_bSunSetChoice     = FALSE;
	m_bRenderSun        = FALSE;
    m_bShowRoomTitleList = FALSE;
    m_bPassRoomChk      = FALSE;
    m_bPassDlg          = FALSE;
    
	dwTurnFrameCnt = 0;

    l_ConfigBox         = NULL;
    l_Message           = NULL;

	m_hSocketThread     = NULL;
	m_hQThread          = NULL;
	m_hRenderThread     = NULL;
	hUserCardThread     = NULL;
	m_hAniUpdateThread  = NULL;
	//m_pHtmlView         = NULL;

	memset( l_szTemp,     0x00, sizeof( l_szTemp ));
	memset( l_szTempText, 0x00, sizeof( l_szTempText ));	
	memset( m_szBannerIp, 0x00, sizeof( m_szBannerIp ));
	memset( BtnTime,      0x00, sizeof( BtnTime ));	
	memset( m_szSunPack,  0x00, sizeof( m_szSunPack ));	
    memset( m_szPass,     0x00, sizeof( m_szPass ));
    memset( l_SPR_TurnLine, 0x00, sizeof( l_SPR_TurnLine ));
    memset( m_bShowExitReservIcon, 0x00, sizeof( m_bShowExitReservIcon ));
    memset(&m_sMissionDlgInfo,     0x00, sizeof( m_sMissionDlgInfo ));
    memset(&m_sAvatarPos,          0x00, sizeof( m_sAvatarPos ));
    memset(&m_sExitResrv,          0x00, sizeof( m_sExitResrv ));
    memset(&m_sSunMarkPos,         0x00, sizeof( m_sSunMarkPos ));
    memset(&m_sBangJangMarkPos,    0x00, sizeof( m_sBangJangMarkPos ));
    memset(&m_sUserScorePos,       0x00, sizeof( m_sUserScorePos ));
    memset(&m_sGoCntPos,           0x00, sizeof( m_sGoCntPos ));
    memset(&m_sShakeCntPos,        0x00, sizeof( m_sShakeCntPos ));
    memset(&m_sPPuckCntPos,        0x00, sizeof( m_sPPuckCntPos ));
    memset(&m_sTurnLinePos,        0x00, sizeof( m_sTurnLinePos ));
    memset(&m_sTimerPos,           0x00, sizeof( m_sTimerPos ));
    memset(&m_sEventPos,           0x00, sizeof( m_sEventPos ));
    memset(&m_sNickNamePos,        0x00, sizeof( m_sNickNamePos ));
    memset(&m_sClassPos,           0x00, sizeof( m_sClassPos ));
    memset(&m_sUserMoneyPos,       0x00, sizeof( m_sUserMoneyPos ));
    memset(&m_sWinLoseMoneyPos,    0x00, sizeof( m_sWinLoseMoneyPos ));
    memset(&m_sMissionOkPos,       0x00, sizeof( m_sMissionOkPos ));
    
    //[alpha]
    memset( &m_sPrevDlgInfo, 0x00, sizeof( m_sPrevDlgInfo ));

	m_AvaImg0 = NULL;
	m_AvaImg1 = NULL;
    
	m_PrevAvaImg = NULL;

	m_PrevAvaWidth = 0;
	m_AvaWidth0 = 0;
	m_AvaWidth1 = 0;

    memset( l_SPR_GifAvatar, 0x00, sizeof( l_SPR_GifAvatar ));
	l_SPR_GifWaitAvatar = NULL;

	m_FrameSkip.SetFramePerSec( 50 );	
}



CMainFrame::~CMainFrame()
{
	SAFE_DELETE( m_AvaImg0 );
	SAFE_DELETE( m_AvaImg1 );

	SAFE_DELETE( m_PrevAvaImg );
	SAFE_DELETE( l_Message );
	SAFE_DELETE( l_ConfigBox );
	SAFE_DELETE( g_pGoStop );
	SAFE_DELETE( g_pClient );
    SAFE_DELETE( m_pWaitChat );
    SAFE_DELETE( m_pGameChat );
    SAFE_DELETE( m_IME_WaitChat );
    SAFE_DELETE( m_IME_GameChat );
    SAFE_DELETE( m_IME_InputPass );
    SAFE_DELETE( m_IME_InputRoomTitle );
    SAFE_DELETE( m_IME_RoomInPassBox );
    SAFE_DELETE( m_IME_Memo );
}



void CMainFrame::ChannelAvatarPaly()
{
	AvatarPlay( g_sAvatarPlayStr.nSlot, g_sAvatarPlayStr.szAvaUrl, g_sAvatarPlayStr.szGameGubun, g_sAvatarPlayStr.cSex );
}



void CMainFrame::GetAvaTotUrl( char *pAvaUrl, char *pGameGubun, char *pTotUrl )
{
	char szTemp[255];
	char szAvaUrl[255];

	memset( szTemp, 0x00, sizeof(255) );
	strcpy( pTotUrl, szAvaUrl );
}



void CMainFrame::AvarTarProc( int nSlot, char *pAvaUrl,  char *pGameGubun )
{
	char szAvaFileName[255];
	char szTotAvaUrl[255];
	char szPathAvaFileName[255];

	memset( szAvaFileName, 0x00, sizeof(255) );
	memset( szPathAvaFileName, 0x00, sizeof(255) );

	GetAvaTotUrl( pAvaUrl, pGameGubun, szTotAvaUrl );

	//�ٿ���� ������ �������� ������ �����Ѵ�.
	CreateDirectory("..\\Avatar\\", NULL);

	//������ �̸��� ���´�.
	BOOL bRe = GetAvaFileName( pAvaUrl, szAvaFileName );

	if ( bRe ) {
		strcpy( szPathAvaFileName, "..\\Avatar\\" );
		strcat( szPathAvaFileName, szAvaFileName );

        int nTemp = _access( szPathAvaFileName, 0 );
		
		if ( nTemp >= 0 ) {
			//�ִ�.
		}
		else {
			//����.

			BOOL bRe = DeleteUrlCacheEntry( szTotAvaUrl );

			if ( !bRe ) {
				int nError = GetLastError();

				if ( nError == ERROR_ACCESS_DENIED ) {
					//DTRACE("1");
				}
				else if ( nError == ERROR_FILE_NOT_FOUND ) {
					//DTRACE("2");
				}
			}


			HRESULT hr =URLDownloadToFile (NULL, szTotAvaUrl, szPathAvaFileName, 0, NULL);

			if( SUCCEEDED(hr) ) {
				//DTRACE("�ٿ�ε� �Ϸ�.");
			} 
			else {
				//DTRACE("�ٿ�ε� ����.");
				AvatarDelete( nSlot );
			}
		}	
	}
}



void CMainFrame::AvatarDelete( int nSlot )
{
	switch( nSlot ) 
	{
		case -1:
			if ( m_PrevAvaImg != NULL ) {
				m_PrevAvaImg->Destroy();
				SAFE_DELETE(m_PrevAvaImg);
			}

			m_PrevAvaWidth = 0;
			break;
			
		case 0:
			if ( m_AvaImg0 != NULL ) {
				m_AvaImg0->Destroy();
				SAFE_DELETE(m_AvaImg0);
			}

			m_AvaWidth0 = 0;
			break;

		case 1:
			if ( m_AvaImg1 != NULL ) {
				m_AvaImg1->Destroy();
				SAFE_DELETE(m_AvaImg1);
			}

			m_AvaWidth1 = 0;
			break;
	}
}



void CMainFrame::AvatarPlay( int nSlot, char *pAvaUrl, char *pGameGubun, char cSex )
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
			SAFE_DELETE(m_PrevAvaImg);
			csAvaName = szPathAvaFileName;
 			m_PrevAvaImg = new ImageEx( csAvaName.AllocSysString(), l_SPR_GifWaitAvatar ); 
			m_PrevAvaImg->InitAnimation( g_hWnd, CPoint(0,0) );
			m_PrevAvaWidth = m_PrevAvaImg->GetWidth();
			break;
			
		case 0:
			SAFE_DELETE(m_AvaImg0);
			csAvaName = szPathAvaFileName;
 			m_AvaImg0 = new ImageEx( csAvaName.AllocSysString(), l_SPR_GifAvatar[0] ); 
			m_AvaImg0->InitAnimation( g_hWnd, CPoint(0,0) );
			m_AvaWidth0 = m_AvaImg0->GetWidth();
			break;

		case 1:
			SAFE_DELETE(m_AvaImg1);
			csAvaName = szPathAvaFileName;
 			m_AvaImg1 = new ImageEx( csAvaName.AllocSysString(), l_SPR_GifAvatar[1] ); 
			m_AvaImg1->InitAnimation( g_hWnd, CPoint(0,0) );
			m_AvaWidth1 = m_AvaImg1->GetWidth();
			break;
	}

}



void CMainFrame::DefaultAvatarPlay( int nSlot, char *pAvaUrl, char *pGameGubun )
{
	AvarTarProc( nSlot, pAvaUrl, pGameGubun );

	char szAvaFileName[255];
	char szPathAvaFileName[255];
	CString csAvaName;

	memset( szAvaFileName, 0x00, 255 );
	memset( szPathAvaFileName, 0x00, 255 );

	//������ �̸��� ���´�.
	BOOL bRe = GetAvaFileName( pAvaUrl, szAvaFileName );

	if ( bRe ) 
	{
		strcpy( szPathAvaFileName, "..\\Avatar\\" );
		strcat( szPathAvaFileName, szAvaFileName );
		
		int nTemp = _access( szPathAvaFileName, 0 );
		
		if ( nTemp >= 0 ) //���������� ���ϴٿ� �ϸ� ������ �ִ�.
		{

			switch( nSlot ) 
			{
				case -1:
                    // �����
					SAFE_DELETE(m_PrevAvaImg);
					csAvaName = szPathAvaFileName;
 					m_PrevAvaImg = new ImageEx( csAvaName.AllocSysString(), l_SPR_GifWaitAvatar ); 
					m_PrevAvaImg->InitAnimation( g_hWnd, CPoint(0,0) );
					m_PrevAvaWidth = m_PrevAvaImg->GetWidth();
					
					break;
					
				case 0:
					SAFE_DELETE(m_AvaImg0);
					csAvaName = szPathAvaFileName;
 					m_AvaImg0 = new ImageEx( csAvaName.AllocSysString(), l_SPR_GifAvatar[0] ); 
					m_AvaImg0->InitAnimation( g_hWnd, CPoint(0,0) );
					m_AvaWidth0 = m_AvaImg0->GetWidth();

					break;

				case 1:
					SAFE_DELETE(m_AvaImg1);
					csAvaName = szPathAvaFileName;
 					m_AvaImg1 = new ImageEx( csAvaName.AllocSysString(), l_SPR_GifAvatar[1] ); 
					m_AvaImg1->InitAnimation( g_hWnd, CPoint(0,0) );
					m_AvaWidth1 = m_AvaImg1->GetWidth();
					break;
			}
		} 
	}
}



BOOL CMainFrame::GetAvaFileName( char *pAvaUrl, char *pFileName )
{
	int nLoopCnt = 0;
	int nUrlLen = strlen(pAvaUrl);
	nUrlLen--;

	memset( pFileName, 0x00, 255 );

	while( 1 ) {
		
	
		pFileName[0] = pAvaUrl[nUrlLen];


		nLoopCnt++;
		nUrlLen--;


		if ( pAvaUrl[nUrlLen] == '\\' || pAvaUrl[nUrlLen] ==	'/' ) {
			return TRUE;
		}

		if ( nUrlLen < 0 )
			return TRUE;

		for ( int i = nLoopCnt - 1; i >= 0; i-- ) {
			pFileName[i + 1] = pFileName[i];
		}	
		

		if ( nLoopCnt > 200 ) break;
	}

	return FALSE;

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
/*
BOOL CMainFrame::isFloat(CString origin)
{
	if(origin.GetLength()<=0)
		return FALSE;


    CQStringParser pas1(origin,'.','\n');

	CString strInt;
	for(int cnt1=1;cnt1 <= pas1.GetCount();cnt1++) {
		strInt = pas1.GetField(cnt1);

		if(!isInteger(strInt))
			return FALSE;
	}

	return TRUE;
}
*/

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DTRACE2("[�� �°�] ������ �ñ� %s (%s)\n",__DATE__,__TIME__);

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;




	EnableConnections();
	EnableAutomation();

	ConnectToShell();

	HWND hWnd = this->m_hWnd;
	DWORD VerCheck = 0;

	//===========������
	char szTemp[255];
	memset( szTemp, 0x00, sizeof(szTemp) );
	
	int i = 0,nRate;

	g_hWnd = hWnd;





#ifdef SHOWUSERCARD

	CAdminLogin log;
	if(log.DoModal()!=IDOK)
		ExitGame();
    else {

		memset( g_LSIp,      0x00, sizeof( g_LSIp ));
		memset( g_szUser_ID, 0x00, sizeof( g_szUser_ID ));
		memset( g_szAvaUrl,  0x00, sizeof( g_szAvaUrl ));

		strcpy( g_LSIp,                 "61.100.185.202" );
				g_LSPort              = 9000;
		strcpy( g_szUser_ID,            log.m_AdminID );
	//	strcpy( g_szUser_ID,            "test1");

		strcpy( g_szAvaUrl,             "101002.gif" );
				g_szGameJong          = 0;
				g_szGameServerChannel = 0;
				g_nServiceDiv         = 0;
		strcpy( g_szTempParam,         "0" );
		g_nSystemMoneyRate =  5;
    }
#else
	
	if(__argc != 10)		//�������� �����ѹ�, ���̵�
	{
		::MessageBox( hWnd, "������ �������ּ���.", "����!", MB_OK );
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
    DTRACE("================================================");
#endif
    
	//���� �ѹ�
	memset( g_LSIp,        0x00, sizeof( g_LSIp ));
	memset( g_szUser_ID,   0x00, sizeof( g_szUser_ID ));
	memset( g_szAvaUrl,    0x00, sizeof( g_szAvaUrl ));
	memset( g_szGameGubun, 0x00, sizeof( g_szGameGubun ));

//	strcpy( g_LSIp,               __argv[1] );
//	strcpy( g_LSIp,       "222.231.24.127" );

	strcpy( g_LSIp,       "192.168.1.11" );
//	strcpy( g_LSIp,       "127.0.0.1" );
//	g_LSPort              = atoi( __argv[2] );
    g_LSPort              = 9000;
	strcpy( g_szUser_ID,          __argv[3] );
	strcpy( g_szAvaUrl,           __argv[4] );
	g_szGameJong          = atoi( __argv[5] );
	g_szGameServerChannel = atoi( __argv[6] );
    g_nServiceDiv         = atoi( __argv[7] );
    strcpy( g_szTempParam,        __argv[8] );

	memset( szTemp, 0x00, sizeof(szTemp) );
    strcpy( szTemp,        __argv[9] );
#endif
	

	if(isInteger(szTemp))
		g_nSystemMoneyRate = atoi(szTemp);



	strcpy( g_szGameGubun, "1" );

    memset(g_szProgPath, 0x00, sizeof(g_szProgPath));

	//yun ���� ����Ǵ� ��ü��ο� ���� ������ �����´�. c:\aaa\bbb.exe
	GetModuleFileName(::GetModuleHandle(NULL), g_szProgPath, sizeof(g_szProgPath));

	int path_len = strlen(g_szProgPath);

	//yun ��θ� ã�Ƽ� �ڿ��� ���� �о�帰�� ������ ��� �ձ����� ��θ� ã�´�.
	// ������ ��ġ�� ���� c:\aaa\�� ���´�.
	for ( i = path_len - 1; i >= 0; i-- ) {
		if (g_szProgPath[i] == '\\')  {
			g_szProgPath[i+1] = '\0';
			break;
		}
	}
	if (i < 0)
		return false;

	strdup(g_szProgPath);


#ifndef _DEBUG
	SetCurrentDirectory( g_szProgPath );
	DTRACE("SetCurrentDirectory - %s", g_szProgPath);
#endif

	//�⺻�ƹ�Ÿ�� �ϴ� �����..
	DeleteFile( "..\\Avatar\\defaultAvaWoman.gif" );
	DeleteFile( "..\\Avatar\\defaultAvaMan.gif" );

    /*
	VerCheck = g_dx2d.GetDXVerCheck();

	// DX Version�� 7.0 ���� �۰ų� ���ٸ� ������ ǥ���Ѵ�.
	if( VerCheck < 0x700 )
	{
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

	LoadFile();

	g_pClient = new CNetworkSock;


	if( g_pClient->InitSocket( g_hWnd, g_LSIp, g_LSPort ) ) {

		g_ServerSend = false;

        g_pCDXScreen = new CDXScreen();

        if( FAILED(g_pCDXScreen->CreateWindowed(g_hWnd, SCREEN_WIDTH, SCREEN_HEIGHT))) {
            DTRACE("%dx%d â��带 ������ �� �����ϴ�.", SCREEN_WIDTH, SCREEN_HEIGHT);
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

		SetGameCursor( FALSE, TRUE );

		LoadData();

		m_fSkipTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );
		m_bActive = TRUE;

		DWORD dwThreadId;
		//m_hAniUpdateThread = CreateThread( NULL, 0, AniUpdateThreadProc, 0, 0, &dwThreadId );
	}

    /*
	m_pHtmlView = new CHttpView;

	if (!m_pHtmlView->CreateWnd( CRect(13, 13, 682, 144), this ))
		return TRUE;
    
	ExploClose();
    */
    ::SetFocus( g_hWnd );

	return 0;
}



BOOL CMainFrame::ChannelCheck( int nCurMashine )
{
	// 0 : �ʺ�, 1 : �ϼ� , 2 : �߼�, 3 : ���, 4 : Ÿ¥, 5 : ����
	int nMyClass = g_sLogIn.l_sUserList.l_sUserInfo.nClass;
	int nCurCnannelClass = 0;
	char szTemp[255];

	memset( szTemp, 0x00, sizeof(szTemp) );
	memcpy( szTemp, g_sServrMashineList[nCurMashine].szChannelName, 4 );

	if      ( !strcmp( szTemp, "�ʺ�" )) nCurCnannelClass = 0;
	else if ( !strcmp( szTemp, "�ϼ�" )) nCurCnannelClass = 1;
	else if ( !strcmp( szTemp, "�߼�" )) nCurCnannelClass = 2;
	else if ( !strcmp( szTemp, "���" )) nCurCnannelClass = 3;
	else if ( !strcmp( szTemp, "Ÿ¥" )) nCurCnannelClass = 4;
	else if ( !strcmp( szTemp, "����" )) nCurCnannelClass = 5;

	if ( nMyClass < nCurCnannelClass ) { //�ڽ��� ��޺��� ���� Ŭ�����̸�
		
		if ( g_szItem[1] == '1' ) {     //������������ ������ 
			nMyClass++;	                //�ڽ��� ����� �Ѵܰ� �÷��ְ�.

			if ( nMyClass >= nCurCnannelClass )	//�� �ܰ������ ���
				return TRUE;
			else
				return FALSE;
		}
		else	//������������ ������ 
			return FALSE;
	}

	return TRUE;
}







BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	int xx , yy;
		
	xx = ( GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH ) / 2 ;
	yy = ( GetSystemMetrics(SM_CYSCREEN) - SCREEN_HEIGHT ) / 2 ;

	cs.x = xx;
	cs.y = yy;
	
	cs.cx = SCREEN_WIDTH;
	cs.cy = SCREEN_HEIGHT;
	cs.dwExStyle = NULL;
	cs.hMenu = NULL;
	cs.style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
	cs.lpszName = "KibcGames";

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
}



BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::AnimationUpdate() // �ִϸ��̼� ������Ʈ
{
	dwTurnFrameCnt++;

    if ( l_nCurrentMode == WAITROOM_MODE && m_pScrollNotice != NULL) {
        m_pScrollNotice->Update();
    }

    if ( l_nCurrentMode == GAME_MODE ) {
        
        // ���� ������ �� ī�� ������ �����̰� �ϴ� ���ϸ��̼��� ���ؼ� ���Ǵ� ī����
        m_nCardBlinkCnt++;

        if ( m_nCardBlinkCnt >= 20 ) {
            m_nCardBlinkCnt = 0;

            m_nCardBlinkFrm = ( m_nCardBlinkFrm + 1 ) % 2;   // 0, 1 �ݺ�
        }

        // �̼�â �̵� ó��
        if ( l_bShowMission ) {
            if ( m_sMissionDlgInfo.bAni ) {
                m_sMissionDlgInfo.nTimeCnt++;

                if ( m_sMissionDlgInfo.nTimeCnt > 5 ) {  // 2 �����Ӹ��� �̼�â �̵�
                    m_sMissionDlgInfo.nAniCnt++;

                    m_sMissionDlgInfo.nDlgX = MISSION_DLG_POS_X + 
                                              (
                                                  ( m_sMissionDlgInfo.nMaxAniCnt - m_sMissionDlgInfo.nAniCnt ) *
                                                  ( MISSION_DLG_MOVE_DISTANCE / m_sMissionDlgInfo.nMaxAniCnt )
                                              );

                    if ( m_sMissionDlgInfo.nAniCnt >= m_sMissionDlgInfo.nMaxAniCnt ) {
                        m_sMissionDlgInfo.bAni = false;
                    }
                }
            }
        }
    }

    //---------------------------------------------
    // ī�带 �������� ���� ��� �ڵ����� ī�� ����
    //---------------------------------------------

	if ( m_bSunSetRender && m_bSunSetChoice == FALSE ) {
		m_nSunSetRender++;

		if ( m_nSunSetRender > 500 ) {
			m_nSunSetRender = 0;
			m_bSunSetRender = FALSE;
			m_bSunSetChoice = TRUE;
			g_pClient->AutoChoice();
		}
	}

    //------------------------------------
    // ���� ������ ȭ���� ���ִ� �ð� üũ
    //------------------------------------

	if ( m_bRenderSun ) {
		m_nSunRenderDecided++;

		if ( m_nSunRenderDecided > 200 ) {
			m_nSunRenderDecided = 0;
            m_bSunSetRender = FALSE;
			m_bRenderSun = FALSE;

			if ( g_pGoStop->m_pFloor.m_nMasterSlotNo == MASTER_POS )
				OnBtnStart();
		}
	}

	// TurnLine �ӵ� .. �� ������.. 
	if ( dwTurnFrameCnt >= 20 ) {
		l_bShowTurnLine = !l_bShowTurnLine;
		dwTurnFrameCnt = 0;
	}

    // ���콺 �Ʒ��� ���� ī�尡 �ִ� ��� ǥ���̹����� ���ϸ��̼ǿ� ���
    m_nHwatuSelectCnt++;

    if ( m_nHwatuSelectCnt >= 20 ) {
        m_nHwatuSelectCnt = 0;
        m_nHwatuSelectFrm = ( m_nHwatuSelectFrm + 1 ) % 2;
    }

	if(l_bShowDialog)
	{
		l_nDialogWaitFrameCnt++;

		if(l_nWaitTime > 0)
		{
            if(l_nDialogWaitFrameCnt >= l_nWaitTime) {   // 50�� 1��.

				g_pGoStop->m_nGoStop = 0;
				g_pGoStop->m_nShake = 0;
				g_pGoStop->m_nSelectCard = g_TwoPeeCnt[0];	
				g_pGoStop->m_nNineCard = 1;
				memset( g_TwoPeeCnt, -1, sizeof(g_TwoPeeCnt) );
				memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );
				
				l_bShowDialog = FALSE;
				l_nDlgMode = 0;
				l_nWaitTime = 0;
				l_nDialogWaitFrameCnt = 0;

				g_pGoStop->SetActionEvent();
			}
		}
	}
	

	if ( l_bShowResult ) {
		l_nDialogResultFrameCnt++;

		if( l_nDialogResultWaitTime > 0 ) {
			if(l_nDialogResultFrameCnt >= l_nDialogResultWaitTime) {    // 50�� 1��.
				l_bShowResult = FALSE;
				g_pGoStop->m_bGameStop = false;

				//DTRACE("�ð��� �ٵǼ� Ȯ���� ������. ������ ��� : %d", g_pGoStop->m_bMasterMode );
				//�ڽ��� �������̸� ��Ŷ�� ���ö��� �� ��ٸ��� �׷��� ������ ��Ŷ�� ������.
				OnReGames( g_pGoStop->m_bMasterMode, 0 );

				l_nDialogResultWaitTime = 0;
				l_nDialogResultFrameCnt = 0;
			}
		}
	}

	if ( l_bDrawEvent ) {
		l_nDrawEventFrameCnt++;

		if(l_nDrawEventFrameCnt > l_nDrawEventWaitTime) {   // ���ڰ� Ŭ���� �� ���� ������
			l_nDrawEventFrameCnt = 0;
			l_EventTime = 0;
			l_nDrawEventWaitTime = 0;
			l_bDrawEvent = FALSE;
			g_pGoStop->DrawEndEvent();
		}
	}

	if ( l_bShowStartBTN ) {
		if ( g_pGoStop->m_pFloor.RealGame() ) {
			l_nStartBtnCnt++;

			if(l_nStartBtnCnt > 300) {  // ���ڰ� Ŭ���� �� ���� ������
				OnBtnStart();
			}
		}
	}

	if ( g_pGoStop->m_pFloor.m_pGamer[0].m_nBombFlipCnt > 0 ) {
		l_nBombLineFrameCnt++;

		if ( l_nBombLineFrameCnt > 30 ) {
			l_bShowBombLine = !l_bShowBombLine;
			l_nBombLineFrameCnt = 0;
		}
	}

	if ( l_nTempCardNo > -1 ) {     // ī�� Ȯ�� �ִϸ��̼� ó��
		l_nCardFlipFrameCnt++;
		
		if(l_nCardFlipFrameCnt > 5) {
			l_nCardFlipFrameCnt = 0;
			l_nTempCardNo = -1;
			g_pGoStop->SetPaintEvent();
		}
	}

    for(int i = 0; i <= l_AniStack.GetAniCount(); i++) {    // ī�� �̵� �ִϸ��̼� ó��
		CARDPOS *card = l_AniStack.GetCard(i);

		if(card == NULL) continue;
		
		//���̸� ��������.

		if(CheckPosition(card)) {
			card->Pos = card->NewPos; 
			l_nAniMaxCount = card->nAniCount = 0; // bsw 01.06
			card->nAniMaxCount = -1;
			card->bGetOneCardMode = FALSE;

			if(card->bSetAction) {
				g_pGoStop->SetPaintEvent();
				card->bSetAction = FALSE;
			}

			l_AniStack.DeleteStack(i);
		}
	}
}



void CMainFrame::OnLButtonDown()
{
	//int nCardNo;

	ButtonDownCheck(l_mX, l_mY);

	l_cX = l_point.x;
	l_cY = l_point.y;
	l_sX = l_SPR_MakeRoom->GetPosX();
	l_sY = l_SPR_MakeRoom->GetPosY();
	l_nMBoxTempX = l_SPR_MessageBox->GetPosX();
	l_nMBoxTempY = l_SPR_MessageBox->GetPosY();
	
	if( l_bShowMessageBox && !l_bMBoxMoveDlg ) {
		if( MouseIn( l_nMBoxXPos, l_nMBoxYPos, l_nMBoxXPos + 325, l_nMBoxYPos + 40 )) {
			if( l_nCurrentMode != CHANNEL_MODE && l_nCurrentMode != LOGO_MODE ) {
                if( l_nCurrentMode != GAME_MODE ) {
                    l_bMBoxMoveDlg = TRUE;
                }
			}
		}
	}

	l_nInviteTempX = l_SPR_Invitation->GetPosX();
	l_nInviteTempY = l_SPR_Invitation->GetPosY();

	if( l_bShowChodaeBox && !l_bChodaeDlg && !l_bShowMessageBox ) {
		if( MouseIn( l_nInviteXPos, l_nInviteYPos, l_nInviteXPos + 325, l_nInviteYPos + 40 )) {
			if( l_nCurrentMode == WAITROOM_MODE ) {
				l_bChodaeDlg = TRUE;
			}
		}
	}

	if ( l_bShowMakeRoomDlg && !m_bShowRoomTitleList ) {
		if      ( MouseIn( MAKEROOM_POS_X +  24, MAKEROOM_POS_Y + 149, MAKEROOM_POS_X + 163, MAKEROOM_POS_Y + 169 )) m_nMakeRoomMoneyNo = 0;
		else if ( MouseIn( MAKEROOM_POS_X + 164, MAKEROOM_POS_Y + 149, MAKEROOM_POS_X + 299, MAKEROOM_POS_Y + 169 )) m_nMakeRoomMoneyNo = 1;
		else if ( MouseIn( MAKEROOM_POS_X +  24, MAKEROOM_POS_Y + 170, MAKEROOM_POS_X + 163, MAKEROOM_POS_Y + 190 )) m_nMakeRoomMoneyNo = 2;
		else if ( MouseIn( MAKEROOM_POS_X + 164, MAKEROOM_POS_Y + 170, MAKEROOM_POS_X + 299, MAKEROOM_POS_Y + 190 )) m_nMakeRoomMoneyNo = 3;
		else if ( MouseIn( MAKEROOM_POS_X +  24, MAKEROOM_POS_Y + 191, MAKEROOM_POS_X + 163, MAKEROOM_POS_Y + 211 )) m_nMakeRoomMoneyNo = 4;
		else if ( MouseIn( MAKEROOM_POS_X + 164, MAKEROOM_POS_Y + 191, MAKEROOM_POS_X + 299, MAKEROOM_POS_Y + 211 )) m_nMakeRoomMoneyNo = 5;
        else if ( MouseIn( MAKEROOM_POS_X +  24, MAKEROOM_POS_Y + 212, MAKEROOM_POS_X + 163, MAKEROOM_POS_Y + 232 )) m_nMakeRoomMoneyNo = 6;
		else if ( MouseIn( MAKEROOM_POS_X + 164, MAKEROOM_POS_Y + 212, MAKEROOM_POS_X + 299, MAKEROOM_POS_Y + 232 )) m_nMakeRoomMoneyNo = 7;
	}
	TRACE("%d, %d\r\n", l_mX, l_mY);
}



void CMainFrame::OnLButtonUp()
{

	ProcessButtonEvent();
	ButtonUpCheck();
	
	// ������� �����ұ�� �϶�
	if( l_bShowDialog && l_nDialgType == DLGTYPE_TWOCARDCHO )
	{
		if(l_mX >= DEF_DLG_POS_X + 67 &&
           l_mX <= DEF_DLG_POS_X + 67 + HWATU_WIDTH &&
           l_mY >= DEF_DLG_POS_Y + 64 &&
           l_mY <= DEF_DLG_POS_Y + 64 + HWATU_HEIGHT )
		{
			// ���� �� ����
			g_pGoStop->m_nSelectCard = g_TwoPeeCnt[0];
			l_bShowDialog = FALSE;
			l_nDlgMode = 0;
			l_nWaitTime = 0;
			l_nDialogWaitFrameCnt = 0;

			memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );

			g_pGoStop->SetActionEvent();
		}

		if(l_mX >= DEF_DLG_POS_X + 138 &&
           l_mX <= DEF_DLG_POS_X + 138 + HWATU_WIDTH &&
           l_mY >= DEF_DLG_POS_Y + 64 &&
           l_mY <= DEF_DLG_POS_Y + 64 + HWATU_HEIGHT )
		{
			// ������ �� ����
			g_pGoStop->m_nSelectCard = g_TwoPeeCnt[1];
			l_bShowDialog = FALSE;
			l_nDlgMode = 0;
			l_nWaitTime = 0;
			l_nDialogWaitFrameCnt = 0;

			memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );

			g_pGoStop->SetActionEvent();
		}
		return;
	}


	l_bMoveDlg = FALSE;
	l_bMBoxMoveDlg = FALSE;
	l_bChodaeDlg = FALSE;

	if ( l_nCurrentMode == GAME_MODE ) {

        //--------------------------------
		// ���� �����ϱ� ���� �и� ����.
        //--------------------------------

		if ( m_bSunSetRender && m_bSunSetChoice == FALSE && l_AniStack.GetAniCount() < 0 ) {
			for ( int i = 0; i < MAX_SUNCARD_CNT; i++ ) {
				if ( MouseIn( m_szSunPackPoint[i].x,
                              m_szSunPackPoint[i].y,
                              m_szSunPackPoint[i].x + HWATU_WIDTH,
                              m_szSunPackPoint[i].y + HWATU_HEIGHT ))
                {
					g_pClient->SunChoice(i);
					m_bSunSetChoice = TRUE;
				}
			}
		}

		if ( g_CurUser == g_MyUser && l_bShowResult == false && l_bShowDialog == false &&
			 g_pGoStop->m_bMyKetInput == true ) {

			for( g_i = 0; g_i < 10; g_i++ ) // 0 ~ 6 = ������� �� ���� ��ġ
			{
				if((l_mX >= l_UserCardPos[MASTER_POS][g_i].x) && (l_mX <= l_UserCardPos[MASTER_POS][g_i].x + HWATU_WIDTH ) &&
					(l_mY >= l_UserCardPos[MASTER_POS][g_i].y) && (l_mY <= l_UserCardPos[MASTER_POS][g_i].y + HWATU_HEIGHT )) // ��� �и� �����ߴ°�
				{
					if( g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bCard[g_i] >= BOMB_CARDNO ) break;

					//DTRACE("ī�弱�� %d", g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bCard[g_i]);
					//DTRACE("(2)����� �׼ǿ��⿡�� Ǯ��.");

					g_bBomb = g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].UserBomb(g_i);						
					//������ Ű�� �ѹ����� �Ŀ��� �������ʰ� �Ǽ��� Ű�� �Ծ�� �Ѵ�.
					g_pGoStop->m_bMyKetInput = false;

					int nSelectCard = g_i;      // | floorNo;
					g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bUserEvent = true;
					g_pGoStop->m_nSelectCard = nSelectCard;

					//DTRACE("������ Ű SetActionEvent()");
					g_pGoStop->SetActionEvent();
					break;
				}
			}

			if(g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_nBombFlipCnt > 0)
			{
				int nCount = g_pGoStop->m_Hwatu.m_pCardPack->RemainCardCnt();
				int cX = l_CardPackPos.x - nCount / 2;
				int cY = l_CardPackPos.y - nCount / 2;

				if((l_mX >= cX) && (l_mX <= cX + HWATU_WIDTH) &&
                   (l_mY >= cY) && (l_mY <= cY + HWATU_HEIGHT))
				{
					//DTRACE("����� �ѹ��� Ÿ���ϴµ�.");
					//DTRACE("(2)- 1����� �׼ǿ��⿡�� Ǯ��.");
					
					//������ Ű�� �ѹ����� �Ŀ��� �������ʰ� �Ǽ��� Ű�� �Ծ�� �Ѵ�.
					g_pGoStop->m_bMyKetInput = false;
					g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_nBombFlipCnt--;
					g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bUserEvent = true;
					g_pGoStop->m_nSelectCard = BOMB_CARDNO;

					//DTRACE("������ Ű BOMB_CARDNO SetActionEvent()");
					g_pGoStop->SetActionEvent();
				}
			}
		}
	}
}



void CMainFrame::Render()
{
    g_pCDXInput->Update();

    if( g_pCDXInput->MOUSE_LBDOWN() ) {
		if ( m_bMouseEvent ) {
			l_bMouseDown = TRUE;
			OnLButtonDown();        // ���� ��ư���� ��ġ�� ���콺�� �ִ��� üũ�ϰ� ���콺�� ������ �ٿ��� �����ش�.
		}
	}

    g_pCDXInput->Get_MouseState();
	
    if ( g_pCDXInput->MOUSE_LBUP() ) {
        ::SetFocus( g_hWnd );       // CHttpView�� ���� Ű �Է� �ȵǴ� ���� ����
        
		if ( m_bMouseEvent ) {
			l_bMouseDown = FALSE;
			OnLButtonUp();
		}
	}

	//API RENDER()
    g_pCDXScreen->GetBack()->Fill(0);

	switch( l_nCurrentMode )
		{
		case LOGO_MODE:
            PutSpriteBlk( l_SPR_LoGo );

			if( l_Message->GetbShowCheck() )
			    l_Message->Draw();
	
            if( l_bMouseDown )
                PutSprite( l_SPR_Cursor2, l_mX, l_mY ); // ���콺 Ŀ�� ��� // 01.12
			else
                PutSprite( l_SPR_Cursor,  l_mX, l_mY );

            g_pCDXScreen->Flip(FALSE, FALSE, FALSE, TRUE);
			return;

		case WAITROOM_MODE: // �����̸�
			RenderWaitRoom();

			if ( g_bConnectingSpr )
                PutSprite( l_SPR_Connting, 270, 200 );

			break;

		case GAME_MODE: // ����ȭ���̸�
			RenderGameBase();
			break;
	}

	if(l_bFullScreen) l_MaxBTN2->DrawButton(); // �ִ�ȭ ��ư ���
	else l_MaxBTN->DrawButton();

    l_MinBTN->DrawButton();     // �ʼ�ȭ ��ư ���
	l_ExitBTN->DrawButton();    // ���� ��ư ���
	l_ConfigBTN->DrawButton();  // ���� ��ư�� �׸���.
	l_Exit2BTN->DrawButton();   // ������ ��ư�� �׸���.
    l_CaptureBTN->DrawButton(); // ȭ�� ĸó ��ư�� �׸���.

	if( l_nCurrentMode != CHANNEL_MODE && l_nCurrentMode != LOGO_MODE ) {
		DrawInvite( l_nInviteXPos , l_nInviteYPos );
		DrawUserState( l_TempMX , l_TempMY );			
		DrawMessageBox( l_nMBoxXPos, l_nMBoxYPos ); // ���� â
	}

	if( l_bShowConfigBox ) {
		l_ConfigBox->RenderConfigBox();
	}

	if( l_Message->GetbShowCheck() )
		l_Message->Draw();

	if ( m_bMouseRender ) { //HTML�� ���� ������ m_bMouseEvent = false�̴�.
        if(l_bMouseDown) PutSprite( l_SPR_Cursor2, l_mX, l_mY ); // ���콺 Ŀ�� ��� // 01.12
		else             PutSprite( l_SPR_Cursor, l_mX, l_mY );
	}

    // [alpha]
    // ���� ������ ���� ��ȭ���� ��� ���� ����
    m_sPrevDlgInfo.bPrevShowDialog = l_bShowDialog;
    m_sPrevDlgInfo.bPrevRenderSun  = m_bRenderSun;
	
    g_pCDXScreen->Flip(FALSE, FALSE, FALSE, TRUE);
}



void CMainFrame::LoadFile()
{	
	char szBuf[255];
	memset( szBuf, 0x00, sizeof(szBuf) );

	int nTemp = _access( ".\\config.ini", 0 );

	
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
		WritePrivateProfileString("Environment","Invite","1",".\\config.ini");
		WritePrivateProfileString("Environment","Eartalk","1",".\\config.ini");
		WritePrivateProfileString("Environment","Sound","1",".\\config.ini");
		WritePrivateProfileString("Environment","MemoRecv","1",".\\config.ini");
		WritePrivateProfileString("Environment","Voice","0",".\\config.ini");

		g_sClientEnviro.bOption[0] = true;
		g_sClientEnviro.bOption[1] = true;
		g_sClientEnviro.bOption[2] = true;
		g_sClientEnviro.bOption[3] = true;
		g_sClientEnviro.nVoice = 0;
	}
}



void CMainFrame::LoadData()
{
	g_pGoStop = new CGoStopEngine();
	g_pGoStop->SetWndHandle( g_hWnd );

	DWORD dwThreadId;
	m_hSocketThread = CreateThread( NULL, 0, ThreadProc, (LPVOID)g_pClient, 0, &dwThreadId );

	DWORD dwThreadId2;
	m_hQThread = CreateThread( NULL, 0, UserCardRecvThreadProc, (LPVOID)g_pClient, 0, &dwThreadId2 );

    g_sPaintAvatar.bPrint[0] = TRUE;
	g_sPaintAvatar.bPrint[1] = TRUE;

    //-----------------
    // ���� ��� �̹���
    //-----------------

    l_SPR_LoGo              = LoadSprite( ".\\image\\common\\logo.spr",            1024, 768,  1 );
    l_SPR_Cursor            = LoadSprite( ".\\image\\common\\cursor.spr",            32,  32,  1 );
	l_SPR_Cursor2           = LoadSprite( ".\\image\\common\\cursor2.spr",           32,  32,  1 );
    l_SPR_Connting          = LoadSprite( ".\\image\\common\\Connecting.spr",       300,  60,  1 );
    l_SPR_ConBox            = LoadSprite( ".\\image\\common\\config.spr",           325, 267,  1 );
	l_SPR_Message           = LoadSprite( ".\\image\\common\\alram.spr",            325, 230,  1 );
    l_SPR_InviteBox         = LoadSprite( ".\\image\\common\\inviteBox.spr",        325, 420,  1 );
	l_SPR_InviteIcon        = LoadSprite( ".\\image\\common\\Invite_human_icon.spr", 13,  20,  2 );
	l_SPR_MessageBox        = LoadSprite( ".\\image\\common\\dlg_memo.spr",         325, 267,  1 );
	l_SPR_UserState         = LoadSprite( ".\\image\\common\\UserState.spr",         92,  21,  1 );
	l_SPR_Invitation        = LoadSprite( ".\\image\\common\\InviteDialog.spr",     325, 230,  1 );
    l_SPR_Class             = LoadSprite( ".\\image\\common\\class.spr",             16,  16,  6 );

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
	    l_SPR_GifAvatar[i]      = LoadSprite( ".\\image\\common\\defaultuser.spr",      125, 190,  1 );
	}

	l_SPR_GifWaitAvatar     = LoadSprite( ".\\image\\common\\defaultuser.spr",      125, 190,  1 );
    m_SPR_CheckBall         = LoadSprite( ".\\image\\common\\check_ball.spr",        14,  14,  2 );

    //--------------
    // ���� �̹���
    //--------------

    l_SPR_WaitBase          = LoadSprite( ".\\image\\lobby\\waitbase.spr",         1024, 768, 1 );
    l_SPR_FullWait          = LoadSprite( ".\\image\\lobby\\fullwait_btn.spr",       22,  22,  4 );
	l_SPR_MakeRoom          = LoadSprite( ".\\image\\lobby\\makeroom.spr",          325, 315,  1 );
    m_SPR_UserListTitle[0]  = LoadSprite( ".\\image\\lobby\\title_wait_user.spr",   297,  40,  1 );
    m_SPR_UserListTitle[1]  = LoadSprite( ".\\image\\lobby\\title_room_user.spr",   297,  40,  1 );
    m_SPR_HighlightBar      = LoadSprite( ".\\image\\lobby\\highlight_bar.spr",     237,  24,  1 );
    m_SPR_PassDlg           = LoadSprite( ".\\image\\lobby\\dlg_pass.spr",          325, 173,  1 );
    m_SPR_ChodaeOX          = LoadSprite( ".\\image\\lobby\\chodae_ox.spr",          13,  13,  2 );
    m_SPR_RoomBack          = LoadSprite( ".\\image\\lobby\\room_back.spr",  190, 101,  3 ); // ham 2006.01.18
    m_SPR_UserIcon          = LoadSprite( ".\\image\\lobby\\icon_user.spr",   30,  35,  2 ); // ham 2006.01.18

    //----------------------
    // ���� ȭ�� ��� �̹���
    //----------------------

    l_SPR_GameBase          = LoadSprite( ".\\image\\game\\gamebase.spr",            1024, 768, 1 );
    l_SPR_Result            = LoadSprite( ".\\image\\game\\result.spr",               300, 357, 1 );
    m_SPR_Arrow             = LoadSprite( ".\\image\\game\\arrow.spr",                 18,  15, 2 );
    m_SPR_MissionDlg        = LoadSprite( ".\\image\\game\\dlg_mission.spr",          239, 160, 1 );
    l_SPR_MComplete         = LoadSprite( ".\\image\\game\\mission_complete.spr",     239,  35, 1 );
    m_SPR_SelSunText        = LoadSprite( ".\\image\\game\\sel_sun_text.spr",         286,  38, 1 );
    m_SPR_SelSunTextBack    = LoadSprite( ".\\image\\game\\sel_sun_text_back.spr",    286,  38, 1 );

    l_SPR_TurnLine[0]       = LoadSprite( ".\\image\\game\\turn_line_0.spr",          241, 145, 1 );
    l_SPR_TurnLine[1]       = LoadSprite( ".\\image\\game\\turn_line_1.spr",          110,  67, 1 );

	l_SPR_HwatuBackSmall    = LoadSprite( ".\\image\\game\\hwatu_small.spr",         20,  30,  1 );
	l_SPR_Hwatu             = LoadSprite( ".\\image\\game\\hwatu_all3.spr", HWATU_WIDTH, HWATU_HEIGHT, 104);
    l_SPR_BombLine          = LoadSprite( ".\\image\\game\\bomb_line.spr",  HWATU_WIDTH, HWATU_HEIGHT, 1);
	l_SPR_MissionTimes      = LoadSprite( ".\\image\\game\\multiplex.spr",           65,  65, 10 );
	l_SPR_Dialog            = LoadSprite( ".\\image\\game\\dialog.spr",             250, 190,  1 );
	l_SPR_NotifyText        = LoadSprite( ".\\image\\game\\notify.spr",             250,  30,  6 ); // 01.12
	l_SPR_HwatuSelect       = LoadSprite( ".\\image\\game\\hwatoo_select.spr",       26,  15,  2 );
	l_SPR_Time              = LoadSprite( ".\\image\\game\\time.spr",               142,   9, 10 );
	l_SPR_EVENT             = LoadSprite( ".\\image\\game\\event.spr",              240, 160, 29 );
	l_SPR_Baks              = LoadSprite( ".\\image\\game\\bak.spr",                 43,  13,  3 );
	l_SPR_Start             = LoadSprite( ".\\image\\game\\start.spr",              170,  62,  2 );	
	l_SPR_DlgBar            = LoadSprite( ".\\image\\game\\dialog_bar.spr",         160,  11, 10 );
	l_SPR_KingFirst         = LoadSprite( ".\\image\\game\\king_first.spr",          25,  22,  2 );
	l_SPR_COM               = LoadSprite( ".\\image\\game\\com.spr",                 85, 120,  1 );
	l_SPR_WaitAvatar        = LoadSprite( ".\\image\\game\\comno1.spr",             125, 190,  1 );
	l_SPR_P_Back            = LoadSprite( ".\\image\\game\\p_back.spr",              15,  15,  1 );
	l_SPR_AffairNum         = LoadSprite( ".\\image\\game\\num_affair.spr",          13,  12, 13 ); // ������ ���� �̹��� ( �� , ��,  ��� ��..)
	l_SPR_Jumsu             = LoadSprite( ".\\image\\game\\num_score.spr",           13,  12, 13 );
	l_SPR_MissionOK         = LoadSprite( ".\\image\\game\\mission_ok.spr",          80,  22,  1 );
	l_SPR_ResultMessage     = LoadSprite( ".\\image\\game\\result_message.spr",     286,  26,  2 );
	l_SPR_ResultDojang      = LoadSprite( ".\\image\\game\\result_dojang.spr",      196, 177,  3 );
	l_SPR_MissionNext       = LoadSprite( ".\\image\\game\\next_chance.spr",        179,  36,  1 );
    m_SPR_ResultNum[0]      = LoadSprite( ".\\image\\game\\num_result_1.spr",        20,  22, 12 );
    m_SPR_ResultNum[1]      = LoadSprite( ".\\image\\game\\num_result_2.spr",        20,  22, 12 );
    m_SPR_ExitReserv        = LoadSprite( ".\\image\\game\\exit_reserv.spr",         80,  22,  1 );
    m_SPR_WaitDlg           = LoadSprite( ".\\image\\game\\wait_dlg.spr",           250,  99,  5 );
    m_SPR_CardBlink         = LoadSprite( ".\\image\\game\\card_blink.spr",          51,  75,  1 );
    g_sImageData[0].l_SPR_Avatar = LoadSprite( ".\\image\\game\\comno1.spr",        125, 190,  1 );
	g_sImageData[1].l_SPR_Avatar = LoadSprite( ".\\image\\game\\comno1.spr",        125, 190,  1 );
    
    //---------------
    // ���� ��� ��ư
    //---------------

    l_UserStateBTN          = LoadButton(".\\image\\common\\userstate_btn.spr",    0,   0,  87, 18 );
	l_MessageOKBTN          = LoadButton(".\\image\\common\\btn_ok.spr",           0,   0, 107, 30 );
	l_MemoSendBTN           = LoadButton(".\\image\\common\\btn_send.spr",         0,   0, 107, 30 );
	l_MemoCancelBTN         = LoadButton(".\\image\\common\\btn_cancel.spr",       0,   0, 107, 30 );
	l_RecvChodaeOkBTN       = LoadButton(".\\image\\common\\btn_yes.spr",          0,   0, 107, 30 );
	l_RecvChodaeCancelBTN   = LoadButton(".\\image\\common\\btn_no.spr",           0,   0, 107, 30 );
    l_InviteOKBTN           = LoadButton(".\\image\\common\\btn_invite.spr",     CHODAE_POS_X +  41, CHODAE_POS_Y + 364, 107, 30 );
    l_InviteCancelBTN       = LoadButton(".\\image\\common\\btn_cancel.spr",     CHODAE_POS_X + 178, CHODAE_POS_Y + 364, 107, 30 );
    l_ConfigOkBTN           = LoadButton(".\\image\\common\\btn_ok.spr",     CONFIG_POS_X   + 111, CONFIG_POS_Y   + 214, 107, 30 );
    m_PassDlgOkBTN          = LoadButton(".\\image\\common\\btn_ok.spr",     PASS_DLG_POS_X +  45, PASS_DLG_POS_Y + 115, 107, 30 );
    l_MakeRoomCancelBTN     = LoadButton(".\\image\\common\\btn_cancel.spr", MAKEROOM_POS_X + 178, MAKEROOM_POS_Y + 258, 107, 30 );
    m_PassDlgCancelBTN      = LoadButton(".\\image\\common\\btn_cancel.spr", PASS_DLG_POS_X + 173, PASS_DLG_POS_Y + 115, 107, 30 );
    l_MakeRoomOkBTN         = LoadButton(".\\image\\common\\btn_make.spr",   MAKEROOM_POS_X +  41, MAKEROOM_POS_Y + 258, 107, 30 );

    //-----------------
    // ���� ��� ��ư
    //-----------------

    l_MaxBTN         = LoadButton(".\\image\\lobby\\max_btn.spr",         981,   5,  16, 16 );
	l_MaxBTN2        = LoadButton(".\\image\\lobby\\max_btn2.spr",        981,   5,  16, 16 );
    l_MinBTN         = LoadButton(".\\image\\lobby\\min_btn.spr",         962,   5,  16, 16 );
	l_ExitBTN        = LoadButton(".\\image\\lobby\\exit_btn.spr",       1000,   5,  16, 16 );
    l_ConfigBTN      = LoadButton(".\\image\\lobby\\config_btn.spr",      881,   4,  71, 19 );
	l_Exit2BTN       = LoadButton(".\\image\\lobby\\exit2_btn.spr",        29, 555, 107, 30 );
    l_CaptureBTN     = LoadButton(".\\image\\lobby\\btn_capture.spr",     804,   4,  71, 19 );
    m_RoomTitleBTN   = LoadButton(".\\image\\lobby\\btn_room_title.spr",      MAKEROOM_POS_X + 277, MAKEROOM_POS_Y +  61,  16, 16 );

    m_MakeRoomBTN[0] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 115, 240, 50, 25 );
    m_MakeRoomBTN[1] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 315, 240, 50, 25 );
    m_MakeRoomBTN[2] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 515, 240, 50, 25 );
    m_MakeRoomBTN[3] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 115, 351, 50, 25 );
    m_MakeRoomBTN[4] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 315, 351, 50, 25 );
    m_MakeRoomBTN[5] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 515, 351, 50, 25 );
    m_MakeRoomBTN[6] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 115, 462, 50, 25 );
    m_MakeRoomBTN[7] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 315, 462, 50, 25 );
    m_MakeRoomBTN[8] = LoadButton(".\\image\\lobby\\btn_makeroom.spr", 515, 462, 50, 25 );

    m_MakeRoomBTN[0]->DisableButton();
    m_MakeRoomBTN[1]->DisableButton();
    m_MakeRoomBTN[2]->DisableButton();
    m_MakeRoomBTN[3]->DisableButton();
    m_MakeRoomBTN[4]->DisableButton();
    m_MakeRoomBTN[5]->DisableButton();
    m_MakeRoomBTN[6]->DisableButton();
    m_MakeRoomBTN[7]->DisableButton();
    m_MakeRoomBTN[8]->DisableButton();

    m_JoinBTN[0] = LoadButton(".\\image\\lobby\\btn_join.spr", 115, 240, 50, 25 );
    m_JoinBTN[1] = LoadButton(".\\image\\lobby\\btn_join.spr", 315, 240, 50, 25 );
    m_JoinBTN[2] = LoadButton(".\\image\\lobby\\btn_join.spr", 515, 240, 50, 25 );
    m_JoinBTN[3] = LoadButton(".\\image\\lobby\\btn_join.spr", 115, 351, 50, 25 );
    m_JoinBTN[4] = LoadButton(".\\image\\lobby\\btn_join.spr", 315, 351, 50, 25 );
    m_JoinBTN[5] = LoadButton(".\\image\\lobby\\btn_join.spr", 515, 351, 50, 25 );
    m_JoinBTN[6] = LoadButton(".\\image\\lobby\\btn_join.spr", 115, 462, 50, 25 );
    m_JoinBTN[7] = LoadButton(".\\image\\lobby\\btn_join.spr", 315, 462, 50, 25 );
    m_JoinBTN[8] = LoadButton(".\\image\\lobby\\btn_join.spr", 515, 462, 50, 25 );

    m_JoinBTN[0]->DisableButton();
    m_JoinBTN[1]->DisableButton();
    m_JoinBTN[2]->DisableButton();
    m_JoinBTN[3]->DisableButton();
    m_JoinBTN[4]->DisableButton();
    m_JoinBTN[5]->DisableButton();
    m_JoinBTN[6]->DisableButton();
    m_JoinBTN[7]->DisableButton();
    m_JoinBTN[8]->DisableButton();

    //--------------------
    // ���� ȭ�� ��� ��ư
    //--------------------

	l_StartBTN              = LoadButton(".\\image\\game\\start_btn.spr",      427, 353, 170, 62 );
	l_GoBTN                 = LoadButton(".\\image\\game\\btn_go.spr",         DEF_DLG_POS_X + 17, DEF_DLG_POS_Y + 115, 104, 62 );//hy 04.01.16
	l_StopBTN               = LoadButton(".\\image\\game\\btn_stop.spr",       DEF_DLG_POS_X + 129, DEF_DLG_POS_Y + 115, 104, 62 );//hy 04.01.16
	l_GNOBTN                = LoadButton(".\\image\\game\\btn_gno.spr",        DEF_DLG_POS_X + 132, DEF_DLG_POS_Y + 140, 78, 38 );
	l_GYESBTN               = LoadButton(".\\image\\game\\btn_gyes.spr",       DEF_DLG_POS_X + 40, DEF_DLG_POS_Y + 140, 78, 38 );
	l_InviteBTN             = LoadButton(".\\image\\game\\btn_board_invite.spr", 908, 568,  50, 30 );
    l_ResultOKBTN           = LoadButton(".\\image\\game\\result_ok.spr",      RESULT_POS_X + 110, RESULT_POS_Y + 321, 80, 22 );

    RECT sScrollBarRect;
    memset(&sScrollBarRect, 0x00, sizeof(sScrollBarRect));

    // ���� ä��â ��ũ�� ��
    
    SetRect( &sScrollBarRect, 649, 609, 673, 729 );

    m_SCB_WaitChat = LoadScrollBar(
        ".\\image\\lobby\\btn_scroll_up2.spr",
        ".\\image\\lobby\\btn_scroll_down2.spr",
        ".\\image\\lobby\\btn_scroll_boll2.spr",
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
        MAX_CHANNELPER
    );

    
    // �� ����Ʈ ��ũ�� ��

    SetRect( &sScrollBarRect, 651, 175, 675, 531 );

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

    SetRect( &sScrollBarRect, 1005, 404, 1020, 535 );

    m_SCB_GameChat = LoadScrollBar(
        ".\\image\\game\\scb_gamechat_up.spr",
        ".\\image\\game\\scb_gamechat_down.spr",
        ".\\image\\game\\scb_gamechat_bar.spr",
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

    // [alpha]
    // �������� ���� ����� �ӽ� ���ǽ�
    m_SFC_DialogBack = new CDXSurface();
    m_SFC_DialogBack->Create( g_pCDXScreen, l_SPR_Dialog->GetTile()->GetBlockWidth(), l_SPR_Dialog->GetTile()->GetBlockHeight() );
	
	l_ConfigBox = new CONFIGBOX();

	for ( i = 0 ; i < 4; i ++ )
		l_ConfigBox->bCheck[i] = g_sClientEnviro.bOption[i];

	l_ConfigBox->nVoice = g_sClientEnviro.nVoice;

	l_Message = new GAMEMESSAGE();

    m_pWaitChat = new CTextList( 98, MAX_TEXT_LINE );
    m_pGameChat = new CTextList( 34, MAX_TEXT_LINE );

    //---------------------------------
    // �ؽ�Ʈ �Է¿� IME ���� �� �ʱ�ȭ
    //---------------------------------

    // ���� ä��â �޽��� �Է¿� IME �ʱ�ȭ
    m_IME_WaitChat  = new CDXIME( g_pCDXScreen, g_hWnd, 98, TRUE, FALSE );
    m_IME_WaitChat->InitCaret();

    // ����ȭ�� ä��â �޽��� �Է¿� IME �ʱ�ȭ
    m_IME_GameChat  = new CDXIME( g_pCDXScreen, g_hWnd, 36, TRUE, FALSE );
    m_IME_GameChat->InitCaret();

    // �游��� - ��й� ���鶧 ��й�ȣ �Է¿� IME �ʱ�ȭ
    m_IME_InputPass = new CDXIME( g_pCDXScreen, g_hWnd, 4, TRUE, FALSE );
    m_IME_InputPass->SetTextLen( 4 );           // �Է°����� ���� ��
    m_IME_InputPass->InitCaret();               // Ŀ�� �ʱ�ȭ
    m_IME_InputPass->SetPassword( true );       // ��й�ȣ �Է¸��� ����
    m_IME_InputPass->SetPasswordChar( '*' );    // ��й�ȣ ��¿� ���� ���� : �⺻���� '*'

    // �� ����� ��й�ȣ �Է¿� IME �ʱ�ȭ
    m_IME_RoomInPassBox = new CDXIME( g_pCDXScreen, g_hWnd, 4, TRUE, FALSE );
    m_IME_RoomInPassBox->SetTextLen( 4 );
    m_IME_RoomInPassBox->InitCaret();
    m_IME_RoomInPassBox->SetPassword ( true );
    m_IME_RoomInPassBox->SetPasswordChar( '*' );

    // �游��� - ������ �Է¿� IME
    m_IME_InputRoomTitle = new CDXIME( g_pCDXScreen, g_hWnd, MAX_ROOM_NAME - 1, TRUE, FALSE );
    m_IME_InputRoomTitle->SetTextLen( MAX_ROOM_NAME - 1 );
    m_IME_InputRoomTitle->InitCaret();

    // ���� �Է¿� IME �ʱ�ȭ
    m_IME_Memo = new CDXIME( g_pCDXScreen, g_hWnd, 40, TRUE, FALSE );
    m_IME_Memo->SetTextLen( 40 );
    m_IME_Memo->InitCaret();

	l_ConfigBTN->DisableButton();
	
	memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );

    // ī���� ��� ��ġ
	l_CardPackPos = SetPoint( 370, 350 );

    //-------------
    // ���� �� ��ġ
    //-------------

	// �ڽ��� ���� �� ��ġ
	l_UserCardPos[0][0] = SetPoint(537, 616);
	l_UserCardPos[0][1] = SetPoint(585, 616);
	l_UserCardPos[0][2] = SetPoint(633, 616);
	l_UserCardPos[0][3] = SetPoint(681, 616);
	l_UserCardPos[0][4] = SetPoint(729, 616);
	l_UserCardPos[0][5] = SetPoint(537, 688);
	l_UserCardPos[0][6] = SetPoint(585, 688);
	l_UserCardPos[0][7] = SetPoint(633, 688);
	l_UserCardPos[0][8] = SetPoint(681, 688);
	l_UserCardPos[0][9] = SetPoint(729, 688);

	// ������� ���� ���� ��ġ - �������� ���� ��
	l_UserCardPos[1][0] = SetPoint( 540,  81 );
	l_UserCardPos[1][1] = SetPoint( 561,  81 );
	l_UserCardPos[1][2] = SetPoint( 582,  81 );
	l_UserCardPos[1][3] = SetPoint( 603,  81 );
	l_UserCardPos[1][4] = SetPoint( 624,  81 );
	l_UserCardPos[1][5] = SetPoint( 540, 112 );
	l_UserCardPos[1][6] = SetPoint( 561, 112 );
	l_UserCardPos[1][7] = SetPoint( 582, 112 );
	l_UserCardPos[1][8] = SetPoint( 603, 112 );
	l_UserCardPos[1][9] = SetPoint( 624, 112 );

    //-------------
	// ���� �� ��ġ
    //-------------
	
	// �ڽ��� ���� �� ��ġ(���)
	l_EatCardPos[0][0] = SetPoint(  24, 652 ); // �� ��ġ
	l_EatCardPos[0][1] = SetPoint( 169, 615 ); // �׸� ��ġ
	l_EatCardPos[0][2] = SetPoint( 169, 690 ); // �� ��ġ
	l_EatCardPos[0][3] = SetPoint( 359, 615 ); // �� ��ġ(0��)
	l_EatCardPos[0][4] = SetPoint( 359, 630 ); // �� ��ġ(1��)
	l_EatCardPos[0][5] = SetPoint( 359, 645 ); // �� ��ġ(2��)
	l_EatCardPos[0][6] = SetPoint( 359, 660 ); // �� ��ġ(3��)
	l_EatCardPos[0][7] = SetPoint( 359, 675 ); // �� ��ġ(4��)
	l_EatCardPos[0][8] = SetPoint( 359, 690 ); // �� ��ġ(5��)

    // ������� ���� �� ��ġ
	l_EatCardPos[1][0] = SetPoint(  24,  74 ); // �� ��ġ
	l_EatCardPos[1][1] = SetPoint( 169,  37 ); // �׸� ��ġ
	l_EatCardPos[1][2] = SetPoint( 169, 112 ); // �� ��ġ
	l_EatCardPos[1][3] = SetPoint( 359,  37 ); // �� ��ġ(0��)
	l_EatCardPos[1][4] = SetPoint( 359,  52 ); // �� ��ġ(1��)
	l_EatCardPos[1][5] = SetPoint( 359,  67 ); // �� ��ġ(2��)
	l_EatCardPos[1][6] = SetPoint( 359,  82 ); // �� ��ġ(3��)
	l_EatCardPos[1][7] = SetPoint( 359,  97 ); // �� ��ġ(4��)
	l_EatCardPos[1][8] = SetPoint( 359, 112 ); // �� ��ġ(5��)

    //-----------------
    // ������ ��� ��ġ
    //-----------------

    // �ƹ�Ÿ ��� ��ġ
    m_sAvatarPos[0] = SetPoint( 936, 638 );
    m_sAvatarPos[1] = SetPoint( 936,  64 );

    // �� ��ũ
    m_sSunMarkPos[0] = SetPoint( 795, 613 );
    m_sSunMarkPos[1] = SetPoint( 795,  39 );

    // ���� ǥ��
    m_sBangJangMarkPos[0] = SetPoint( 999, 738 );
    m_sBangJangMarkPos[1] = SetPoint( 999, 164 );

    // �̼� ����
    m_sMissionOkPos[0] = SetPoint( 938, 613 );
    m_sMissionOkPos[1] = SetPoint( 938,  39 );

    // ������ ���� �̹��� ǥ�� ��ġ
    m_sExitResrv[0] = SetPoint( 852, 613 );
    m_sExitResrv[1] = SetPoint( 852,  39 );

    // �����̴� ���� ��� ��� ��ǥ
    m_sTurnLinePos[0] = SetPoint( 534, 613 );
    m_sTurnLinePos[1] = SetPoint( 537,  78 );

    // Ÿ�̸� ��� ��ǥ
    m_sTimerPos[0] = SetPoint( 790, 638 );
    m_sTimerPos[1] = SetPoint( 790,  64 );

    // �̺�Ʈ ��� ��ġ
    m_sEventPos[0] = SetPoint( 192, 465 );
    m_sEventPos[1] = SetPoint( 192, 120 );

    //--------------------------------------------------
    // ���� ���� ���� ���� ���� ��ǥ(����, ��, ���, ��)
    //--------------------------------------------------

    // ����
    m_sUserScorePos[0] = SetPoint( 574, 577 );
    m_sUserScorePos[1] = SetPoint( 574, 161 );

    // �� Ƚ��
    m_sGoCntPos[0] = SetPoint( 629, 577 );
    m_sGoCntPos[1] = SetPoint( 629, 161 );

    // ��� Ƚ��
    m_sShakeCntPos[0] = SetPoint( 677, 577 );
    m_sShakeCntPos[1] = SetPoint( 677, 161 );

    // �� Ƚ��
    m_sPPuckCntPos[0] = SetPoint( 741, 577 );
    m_sPPuckCntPos[1] = SetPoint( 741, 161 );

    //--------------------
    // ���� ���� ���� ��ǥ
    //--------------------

    // �г���
    m_sNickNamePos[0] = SetPoint( 860, 671 );
    m_sNickNamePos[1] = SetPoint( 860,  97 );

    // ���
    m_sClassPos[0] = SetPoint( 853, 685 );
    m_sClassPos[1] = SetPoint( 853, 111 );

    // ���� �Ӵ�
    m_sUserMoneyPos[0] = SetPoint( 860, 705 );
    m_sUserMoneyPos[1] = SetPoint( 860, 131 );

    // ���ų� ���� �ݾ�
    m_sWinLoseMoneyPos[0] = SetPoint( 860, 721 );
    m_sWinLoseMoneyPos[1] = SetPoint( 860, 147 );

	InitCardPostion();

    SoundLoading();

    MakeCardZoomTable( &m_sSmallZoomTable, HWATU_WIDTH + 4, HWATU_HEIGHT + 4 ); // [zoom]
    MakeCardZoomTable( &m_sBigZoomTable, HWATU_WIDTH + 20, HWATU_HEIGHT + 20 ); // [zoom]

    SetColorKey();
}



POINT CMainFrame::SetPoint(LONG x, LONG y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	return pt;
}



void CMainFrame::InitCardPostion() // ī�� ��ġ �ʱ�ȭ
{
	for ( int i = 0; i < 53; i++ ) {
		l_CardPos[i].NewPos          = l_CardPackPos;
		l_CardPos[i].StartPos        = l_CardPackPos;
		l_CardPos[i].Pos             = l_CardPackPos;
		l_CardPos[i].bSetAction      = FALSE;
		l_CardPos[i].nAniCount       = 0;
		l_CardPos[i].nAniMaxCount    = -1;
		l_CardPos[i].type            = PACK;
		l_CardPos[i].bGetOneCardMode = FALSE;
	}
}



void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//////////////////////////////////////////////////////////////////////////
	// ���콺�� ��Ʈ�� �ȿ� ������
	//SetCursor( LoadCursor( GetModuleHandle(0) , (LPCSTR)IDC_CURSOR ));
    ShowCursor( FALSE );
    
    /*
	if( l_nCurrentMode == WAITROOM_MODE ) {
        if( MouseIn( 13 - 35, 13 - 35, 682 + 3, 144 + 3) ) {			
			SetGameCursor( FALSE, TRUE, FALSE );
		}
		else {
			SetGameCursor( FALSE, TRUE );
		}
	}
	else */ if( l_nCurrentMode == GAME_MODE ) {
        // ����ȭ�鿡�� ���� ������� ����â�� ����â��
        // �����ư��鼭 ���콺 Ŀ���� �̵���Ű�� ���콺 Ŀ����
        // ������� ������ ����ȭ�鿡���� ���콺 �̵���
        // Ŀ���� ������ �ش�.

        SetGameCursor( FALSE, TRUE );
	}

	l_point.x = point.x;
	l_point.y = point.y;

	l_mX = l_point.x;
	l_mY = l_point.y;

	CheckButtons(l_mX, l_mY);
			
	int newx = (l_sX - (l_cX - l_mX));
	int newy = (l_sY - (l_cY - l_mY));

	int TempX = l_nMBoxTempX - ( l_cX - l_mX);
	int TempY = l_nMBoxTempY - ( l_cY - l_mY);

    // ���� ��ȭ���� �̵� ó��
	if( l_bShowMessageBox && l_bMBoxMoveDlg )
	{
		l_nMBoxXPos = TempX;
		l_nMBoxYPos = TempY;

		if( l_nMBoxXPos < 0 ) l_nMBoxXPos = 0;
		if( l_nMBoxXPos > ( SCREEN_WIDTH - 325 )) l_nMBoxXPos = SCREEN_WIDTH - 325;
		if( l_nMBoxYPos < 160 ) l_nMBoxYPos = 160;
		if( l_nMBoxYPos > ( SCREEN_HEIGHT - 267 )) l_nMBoxYPos = SCREEN_HEIGHT - 267;

		l_SPR_MessageBox->SetPos( l_nMBoxXPos , l_nMBoxYPos );
	}

	int TempX2 = l_nInviteTempX - ( l_cX - l_mX );
	int TempY2 = l_nInviteTempY - ( l_cY - l_mY );

    // �ʴ� ��ȭ���� �̵� ó��
	if( l_bShowChodaeBox && l_bChodaeDlg )
	{
		l_nInviteXPos = TempX2;
		l_nInviteYPos = TempY2;

		if( l_nInviteXPos < 0 ) l_nInviteXPos = 0;
		if( l_nInviteXPos > SCREEN_WIDTH - 325 ) l_nInviteXPos = SCREEN_WIDTH - 325;    // 210�� ��ȭ���� ����ũ��
		if( l_nInviteYPos < 160 ) l_nInviteYPos = 160;
		if( l_nInviteYPos > SCREEN_HEIGHT - 230 ) l_nInviteYPos = SCREEN_HEIGHT - 230;  // 170�� ��ȭ���� ����ũ��
		
		l_SPR_Invitation->SetPos( l_nInviteXPos , l_nInviteYPos );
	}

    // �� ����� â���� �� ���� ����Ʈ�� ���ִ� ���
    if ( m_bShowRoomTitleList ) {
        if (MouseIn( ROOM_TITLE_LIST_X + 2, 
                     ROOM_TITLE_LIST_Y + 2,
                     ROOM_TITLE_LIST_X + ROOM_TITLE_LIST_WIDTH - 3,
                    (ROOM_TITLE_LIST_Y + 2) + (ROOM_TITLE_BAR_HEIGHT * 5) - 1))
        {
            m_nSelectedRoomTitleList = (l_mY - (ROOM_TITLE_LIST_Y + 2))  /  ROOM_TITLE_BAR_HEIGHT;
        }
        else {
            m_nSelectedRoomTitleList = -1;
        }
    }

    if ( l_nCurrentMode == WAITROOM_MODE ) {
        m_SCB_WaitChat->Drag( l_mX, l_mY );
        m_SCB_WaitRoom->Drag( l_mX, l_mY );
        
        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->Drag( l_mX, l_mY );
        }
    }

	GetMouseOverCardNo();
	
	CFrameWnd::OnMouseMove(nFlags, point);
}



void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	g_pCMainFrame->AvatarDelete(0);
	g_pCMainFrame->AvatarDelete(1);
	g_pCMainFrame->AvatarDelete(-1);

	DWORD dwExitCode = 0;

	if ( g_pGoStop ) {
		if ( g_pGoStop->m_hGEThread ) {

			TerminateThread( g_pGoStop->m_hGEThread, dwExitCode );
			WaitForSingleObject( g_pGoStop->m_hGEThread, MAX_DESTROY_TIME );
			CloseHandle( g_pGoStop->m_hGEThread );
		}
	}

	if ( m_hSocketThread ) {
		g_bPaketThredOn = false;
		WaitForSingleObject( m_hSocketThread, MAX_DESTROY_TIME );
		CloseHandle( m_hSocketThread );
	}

	if ( m_hQThread ) {
		g_bUserCardRecvThreadOn = false;
		WaitForSingleObject( m_hQThread, MAX_DESTROY_TIME );
		CloseHandle( m_hQThread );
	}

	if ( m_hRenderThread ) {
		m_bActive = FALSE;
		WaitForSingleObject( m_hRenderThread, MAX_DESTROY_TIME );
		CloseHandle( m_hRenderThread );
	}

	if ( m_hAniUpdateThread ) {
		m_bActive = FALSE;
		WaitForSingleObject( m_hAniUpdateThread, MAX_DESTROY_TIME );
		CloseHandle( m_hAniUpdateThread );
	}

	KillTimer(1);
	ReleaseData();

    ExitGame();
}



void CMainFrame::OnTimer(UINT nIDEvent) 
{
    if (m_IME_Memo->IsInputState()) {
        m_IME_Memo->RunCaretStatus();
    }

    // IME Ŀ�� ������ ó��
    switch ( l_nCurrentMode ) {
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



void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	l_bRButtonDown = TRUE;

	if( l_nCurrentMode == WAITROOM_MODE )
		RButtonDownCheck( l_mX , l_mY );

	CFrameWnd::OnRButtonDown(nFlags, point);
}



void CMainFrame::OnRButtonUp(UINT nFlags, CPoint point) 
{
    ::SetFocus( g_hWnd );       // CHttpView�� ���� Ű �Է� �ȵǴ� ���� ����
    
	if( l_nCurrentMode == WAITROOM_MODE ) {
		RButtonEvent();
		RButtonUpCheck();
	}

	CFrameWnd::OnRButtonUp(nFlags, point);
}



void CMainFrame::RecieveInvite( int nRoomNo , BigInt biRoomMoney, char *pRoomPass , char *pInviteMsg )
{
	if( l_bShowMakeRoomDlg )
	{
		l_bShowMakeRoomDlg = FALSE;

        m_IME_InputPass->InputEnd();
        m_IME_InputRoomTitle->InputEnd();
	}

    if ( m_bPassDlg ) {
        m_bPassDlg = FALSE;
        m_IME_RoomInPassBox->InputEnd();
    }

    // ä��â �Է��� ������.
    if ( l_nCurrentMode == WAITROOM_MODE ) {
        m_IME_WaitChat->InputEnd();
    }
    else if ( l_nCurrentMode == GAME_MODE ) {
        m_IME_GameChat->InputEnd();
    }

	PlaySound( g_Sound[GS_POPUP] );
	l_bShowChodaeBox = TRUE;
	
	l_nInviteRoomNo = nRoomNo; // ���ȣ�� �����Ѵ�.
	l_biInviteRoomMoney = biRoomMoney;
	
	// �� ��й�ȣ�� �����Ѵ�.
	memset( l_szInviteRoomPass , 0x00 ,sizeof( l_szInviteRoomPass ));
	strcpy( l_szInviteRoomPass , pRoomPass );
	
	// ���� �޼����� �����صд�.
	memset( l_szInviteMsg , 0x00 ,sizeof( l_szInviteMsg ));
	strcpy( l_szInviteMsg , pInviteMsg );
}



//========================================================================
// �ʴ��Ҷ� üũ�� ��ȣ�� �����Ѵ�.
// ������ ���� ������ ���̳ʽ�( -1 ) ���� ���´�.
//========================================================================

int CMainFrame::GetInviteCheck( void )
{
    int nUserCnt = m_SCB_Invite->GetTotalElem();

	for( int i = 0; i < nUserCnt; i++ ) {
		if( sInviteCheck[i].bCheck == TRUE ) {
			return i;
		}
	}

	return -1;
}



//========================================================================
// �ε��ߴ� �ڿ� ����
//========================================================================

void CMainFrame::ReleaseData()
{
    int i;

    for ( i = 0; i < MAX_ROOM_IN; i++ ) {
        SAFE_DELETE( l_SPR_TurnLine[i] );
        SAFE_DELETE( g_sImageData[i].l_SPR_Avatar );
    }
	SAFE_DELETE( l_SPR_WaitAvatar );

	SAFE_DELETE( l_SPR_WaitBase );
	SAFE_DELETE( l_SPR_MakeRoom );
	SAFE_DELETE( l_SPR_GameBase );
	SAFE_DELETE( l_SPR_Result );
	SAFE_DELETE( l_SPR_HwatuBackSmall );
	SAFE_DELETE( l_SPR_Hwatu );
	SAFE_DELETE( l_SPR_MissionTimes );
	SAFE_DELETE( l_SPR_BombLine );
	SAFE_DELETE( l_SPR_Dialog );
	SAFE_DELETE( l_SPR_Cursor );
	SAFE_DELETE( l_SPR_Cursor2 );
	SAFE_DELETE( l_SPR_NotifyText );
	SAFE_DELETE( l_SPR_HwatuSelect );
	SAFE_DELETE( l_SPR_Time );
	SAFE_DELETE( l_SPR_LoGo );
	SAFE_DELETE( l_SPR_EVENT );
	SAFE_DELETE( l_SPR_Connting );
    SAFE_DELETE( m_SPR_PassDlg );
    SAFE_DELETE( m_SPR_ChodaeOX );
    SAFE_DELETE( m_SPR_RoomBack );
    SAFE_DELETE( m_SPR_UserIcon );

	SAFE_DELETE( l_MinBTN );
	SAFE_DELETE( l_MaxBTN );
	SAFE_DELETE( l_MaxBTN2 );
	SAFE_DELETE( l_ExitBTN );

	SAFE_DELETE( l_MakeRoomOkBTN );
	SAFE_DELETE( l_MakeRoomCancelBTN );
	SAFE_DELETE( l_StartBTN );
	SAFE_DELETE( l_GoBTN );
	SAFE_DELETE( l_StopBTN );
	SAFE_DELETE( l_GYESBTN );
	SAFE_DELETE( l_GNOBTN );
	SAFE_DELETE( l_ConfigBTN );     // ���� ��ư�� �׸���.
	SAFE_DELETE( l_Exit2BTN );	    // ������ ��ư�� �׸���.
    SAFE_DELETE( l_CaptureBTN );    // ȭ�� ĸó ��ư�� �׸���.
	SAFE_DELETE( l_ConfigOkBTN );   // ��� Ȯ�� ��ư�� �׸���.
	SAFE_DELETE( l_SPR_Start );
	SAFE_DELETE( l_SPR_Baks );
	SAFE_DELETE( l_SPR_P_Back );
    SAFE_DELETE( m_PassDlgOkBTN );
    SAFE_DELETE( m_PassDlgCancelBTN );
	SAFE_DELETE( l_SPR_DlgBar );
	SAFE_DELETE( l_SPR_ConBox );
	SAFE_DELETE( l_SPR_Message );
	SAFE_DELETE( l_MessageOKBTN );
	SAFE_DELETE( l_SPR_FullWait );	
	SAFE_DELETE( l_SPR_KingFirst );
	SAFE_DELETE( l_SPR_COM );
    SAFE_DELETE( m_SPR_ResultNum[0] );
    SAFE_DELETE( m_SPR_ResultNum[1] );
    SAFE_DELETE( m_SPR_UserListTitle[0] );
    SAFE_DELETE( m_SPR_UserListTitle[1] );
    SAFE_DELETE( m_SPR_ExitReserv );
    SAFE_DELETE( m_SPR_WaitDlg );

    SAFE_DELETE( m_MakeRoomBTN[0] );
    SAFE_DELETE( m_MakeRoomBTN[1] );
    SAFE_DELETE( m_MakeRoomBTN[2] );
    SAFE_DELETE( m_MakeRoomBTN[3] );
    SAFE_DELETE( m_MakeRoomBTN[4] );
    SAFE_DELETE( m_MakeRoomBTN[5] );
    SAFE_DELETE( m_MakeRoomBTN[6] );
    SAFE_DELETE( m_MakeRoomBTN[7] );
    SAFE_DELETE( m_MakeRoomBTN[8] );

    SAFE_DELETE( m_JoinBTN[0] );
    SAFE_DELETE( m_JoinBTN[1] );
    SAFE_DELETE( m_JoinBTN[2] );
    SAFE_DELETE( m_JoinBTN[3] );
    SAFE_DELETE( m_JoinBTN[4] );
    SAFE_DELETE( m_JoinBTN[5] );
    SAFE_DELETE( m_JoinBTN[6] );
    SAFE_DELETE( m_JoinBTN[7] );
    SAFE_DELETE( m_JoinBTN[8] );
	
	SAFE_DELETE( l_ResultOKBTN );
	SAFE_DELETE( l_InviteBTN );
	SAFE_DELETE( l_SPR_InviteBox );
	SAFE_DELETE( l_InviteOKBTN );
    SAFE_DELETE( l_InviteCancelBTN );
	SAFE_DELETE( l_SPR_InviteIcon );
	SAFE_DELETE( l_SPR_MessageBox );
	SAFE_DELETE( l_SPR_UserState );
	SAFE_DELETE( l_UserStateBTN );	
	SAFE_DELETE( l_MemoSendBTN );
	SAFE_DELETE( l_MemoCancelBTN );
	SAFE_DELETE( l_SPR_Invitation );
	SAFE_DELETE( l_RecvChodaeOkBTN );
	SAFE_DELETE( l_RecvChodaeCancelBTN );
	SAFE_DELETE( l_SPR_AffairNum );
	SAFE_DELETE( l_SPR_Jumsu );
	SAFE_DELETE( l_SPR_MissionOK );
	SAFE_DELETE( l_SPR_Class );
	SAFE_DELETE( l_SPR_ResultMessage );
	SAFE_DELETE( l_SPR_ResultDojang );
	SAFE_DELETE( l_SPR_MissionNext );
    SAFE_DELETE( m_SPR_CardBlink );
    SAFE_DELETE( m_RoomTitleBTN );
    SAFE_DELETE( m_SPR_CheckBall );
    SAFE_DELETE( m_SPR_HighlightBar );

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		SAFE_DELETE( l_SPR_GifAvatar[i] );
	}

	SAFE_DELETE( l_SPR_GifWaitAvatar );
    SAFE_DELETE( m_SFC_DialogBack );        // [alpha]
    SAFE_DELETE( m_SCB_WaitChat );
    SAFE_DELETE( m_SCB_GameChat );
    SAFE_DELETE( m_SCB_WaitUser );
    SAFE_DELETE( m_SCB_WaitRoom );
    SAFE_DELETE( m_SCB_Invite );

    SAFE_DELETE( m_sSmallZoomTable );
    SAFE_DELETE( m_sBigZoomTable );

    SAFE_DELETE( m_SPR_Arrow );
    SAFE_DELETE( m_SPR_MissionDlg );
    SAFE_DELETE( l_SPR_MComplete );

    SAFE_DELETE( m_SPR_SelSunText );
    SAFE_DELETE( m_SPR_SelSunTextBack );
}



//========================================================================
// __int64 Ÿ���� ���ӸӴϸ� ���ڿ��� ��ȯ�Ѵ�.
//
// input:   *szBuff = ��ȯ�� ���ڿ��� ����� ����
//          nMoney = ���ڿ��� ��ȯ�� ���ӸӴ�
//          nUnitCnt = ����� ������ ����
//                      0 ������ ��� : ��� �ڸ��� ��� (�⺻��)
//                      1 �̻� : ������ ���ڸ�ŭ�� ���� ���(��: 2��
//                               ������ ��� �ֻ��� 2������ ���)
//========================================================================

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

    strcat(szBuff, "Ĩ");
}



void CMainFrame::RenderWaitRoom()
{
    int i = 0;
    int j = 0;
    int nSelRoomNo = -1;     // ���õ� �� ��ȣ
    int nRoomNo = 0;
    int nRoomViewCnt = 0;

    l_SPR_WaitBase->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );

    m_SCB_WaitRoom->Draw();                     // �� ����Ʈ ��ũ�� ��

    if (m_nUserListTitleNum == 0 || m_nUserListTitleNum == 1) {
        m_SPR_UserListTitle[m_nUserListTitleNum]->SetPos( 698, 160 );
        m_SPR_UserListTitle[m_nUserListTitleNum]->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS);
    }

    RenderWaitChat();

    if ( m_nUserListTitleNum == 0 ) {           // ����� ��Ͽ����� ��ũ�ѹ� ���. ������ ����� ���� ���� ��� �ʿ����.
        m_SCB_WaitUser->Draw();
    }

    // �����/������ ����Ʈ�� ���϶���Ʈ �ٸ� �׸���.
    if ( m_nHighlightBarNum != -1 ) {
        PutSprite( m_SPR_HighlightBar, 713, 231 + ( m_nHighlightBarNum * 25 ));
    }


    //---------------------------
    // ���õ� �� ����� ����Ѵ�.
    //---------------------------

    nSelRoomNo = -1;
    nRoomViewCnt = 0;

    if ( m_pScrollNotice != NULL ) {
        m_pScrollNotice->Draw();                    // ��ũ�ѵǴ� ��������
    }

    if( g_ClickRoomUserInfo[l_nViewRoomUser].nUserNo <= 0 )
		l_nViewRoomUser = -1;
	
	if( g_CGameWaitRoomList.m_sRoomStateInfo[l_nViewRoomMessage].l_sRoomInfo.nCurCnt <= 0 )
		l_nViewRoomMessage = -1;
	
	if( g_CUserList.m_pUserInfo[l_nViewWaitUser].m_bUser == FALSE )
		l_nViewWaitUser = -1;

    //========================================================================
	//���ǿ��� �渮��Ʈ �ѷ��ֱ�.
	//========================================================================

    COLORREF rgbTextColor = 0;
	int nTextStyle = FW_BOLD;
	int nRoomInCurCnt = 0;
	int nUserNo = 0;
    int nXPos = 45;                 // ham 2006.01.18
    int nYPos = 192;                // �ؽ�Ʈ�� �̹��� ����� ���� ��ǥ

    nRoomNo = m_SCB_WaitRoom->GetStartElem() * 3;
    nRoomViewCnt = 0;

    while ( nRoomViewCnt < 9 ) {
		// ��ư�� ��ġ�� ���� ������ ��ư�� �۵����� �ʰ� �Ѵ�.
        if ( nRoomNo >= MAX_ROOMCNT ) {
            m_MakeRoomBTN[nRoomViewCnt]->DisableButton();
			m_JoinBTN[nRoomViewCnt]->DisableButton();
        }

		// ��ư�� ��ġ�� ���� �ִ� ���
		else {
			if ( nRoomNo == nSelRoomNo ) {
				rgbTextColor = RGB( 235, 240, 255 );    // ���õ� ���� ���ڻ�
			}
			else {
				rgbTextColor = RGB( 0, 51, 102 );       // ���õ��� ���� ���� ���ڻ�
			}

			// �� ���� ǥ��

			switch ( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.nCurCnt ) {
				case 0:
					
					// �� ����� ��ư�� ���� ������ �Ҵ�.(��ũ�� �� ������ �ǽð����� ����)
					if ( m_MakeRoomBTN[nRoomViewCnt]->status == DISABLED ) {
						m_MakeRoomBTN[nRoomViewCnt]->EnableButton();
					}

					// �����ϱ� ��ư�� ���� ������ ����.(��ũ�� �� ������ �ǽð����� ����)
					if ( m_JoinBTN[nRoomViewCnt]->status != DISABLED ) {
						m_JoinBTN[nRoomViewCnt]->DisableButton();
					}
					
					
					// �� ������ �׸���.
					PutSprite( m_SPR_RoomBack, nXPos, nYPos, 0 );

                    // �� ��ȣ
                    sprintf( m_szTempText, "%03d", nRoomNo + 1);
        			DrawText( m_szTempText, nXPos + 11, nYPos + 8, rgbTextColor, nTextStyle, TA_LEFT );

                    // �� �̸�
                    DrawText( "�� ��", nXPos + 41, nYPos + 8, rgbTextColor, FW_BOLD, TA_LEFT);

					m_MakeRoomBTN[nRoomViewCnt]->DrawButton();
					break;

				case 1:
					// �� ����� ��ư�� ���� ������ ����.(��ũ�� �� ������ �ǽð����� ����)
					if ( m_MakeRoomBTN[nRoomViewCnt]->status != DISABLED ) {
						m_MakeRoomBTN[nRoomViewCnt]->DisableButton();
					}

					// �����ϱ� ��ư�� ���� ������ �Ҵ�.(��ũ�� �� ������ �ǽð����� ����)
					if ( m_JoinBTN[nRoomViewCnt]->status == DISABLED ) {
						m_JoinBTN[nRoomViewCnt]->EnableButton();
					}

					// �� ��� �̹���
					PutSprite( m_SPR_RoomBack, nXPos, nYPos, 1 );

                    // �� ��ȣ
                    sprintf( m_szTempText, "%03d", nRoomNo + 1);
			        DrawText( m_szTempText, nXPos + 11, nYPos + 8, RGB(255, 255, 255), nTextStyle, TA_LEFT );

                    // �� �̸�
                    DrawText( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.szRoomName, nXPos + 41, nYPos + 8, RGB(255, 255, 255), FW_BOLD, TA_LEFT );

					// �� ���� ������
					PutSprite( m_SPR_UserIcon, nXPos + 23, nYPos + 43, 1 );

					// ��й� ����
					if ( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.bSecret ) {
						PutSprite( l_SPR_FullWait, nXPos + 167, nYPos + 6, 3 );
					}
					else {
						PutSprite( l_SPR_FullWait, nXPos + 167, nYPos + 6, 2 );
					}

					if ( m_JoinBTN[nRoomViewCnt]->status != DISABLED ) {
						m_JoinBTN[nRoomViewCnt]->DrawButton();
					}

					DrawText( "�����", nXPos + 95, nYPos + 80, RGB(0, 0, 0), FW_NORMAL, TA_CENTER );
					break;

				case 2:
					// �游���, �����ϱ� ��ư�� ��� ����.(��ũ�� �� ������ �ǽð����� ����)
					m_MakeRoomBTN[nRoomViewCnt]->DisableButton();
					m_JoinBTN[nRoomViewCnt]->DisableButton();



#ifdef ADMINJUSTVIEW
					m_JoinBTN[nRoomViewCnt]->EnableButton();
#endif

					PutSprite( m_SPR_RoomBack, nXPos, nYPos, 0 );

                    // �� ��ȣ
                    sprintf( m_szTempText, "%03d", nRoomNo + 1);
        			DrawText( m_szTempText, nXPos + 11, nYPos + 8, rgbTextColor, nTextStyle, TA_LEFT );

                    // �� �̸�
                    DrawText( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.szRoomName, nXPos + 41, nYPos + 8, rgbTextColor, FW_BOLD, TA_LEFT );

					// �� ���� ������(ù��° ����)
					PutSprite( m_SPR_UserIcon, nXPos +  23, nYPos + 43, 1 );
					PutSprite( m_SPR_UserIcon, nXPos + 137, nYPos + 43, 1 );

					// ��й� ����
					if ( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.bSecret ) {
						PutSprite( l_SPR_FullWait, nXPos + 167, nYPos + 6, 3 );
					}
					else {
						PutSprite( l_SPR_FullWait, nXPos + 167, nYPos + 6, 2 );
					}

					DrawText( "������", nXPos + 95, nYPos + 80, RGB(0, 0, 0), FW_NORMAL, TA_CENTER );
					break;

				default:
					PutSprite( m_SPR_RoomBack, nXPos, nYPos, 0 );
					break;
			}
            
			// ���õ� �� ǥ��
			if ( nRoomNo == l_nViewRoomMessage ) {
				PutSprite( m_SPR_RoomBack, nXPos, nYPos, 2 );
			}
		}

        nRoomViewCnt++;
        nRoomNo++;

        // ���� ����� �� ��� �̹����� ��ġ ���

        if (( nRoomViewCnt % 3 ) == 0 ) {
            nXPos = 45;
            nYPos += 111;
        }
        else {
            nXPos += 200;
        }
    }

    //========================================================================
    // ����� ����Ʈ�� �׸���.
    //========================================================================

    int nTempClass = 0;
    nYPos = 0;

    if ( m_nUserListTitleNum == 0 ) {
	    nRoomViewCnt = 0;
        nYPos = 233;                                // ���� ���� ����Ʈ�� ù��° ���� Y ��ǥ

	    for( i = m_SCB_WaitUser->GetStartElem(); i < MAX_CHANNELPER; i++ ) {
            
            if ( g_CUserList.m_pUserInfo[i].m_bUser == false ) {
                continue;
            }


            // ��� ------------------------------------------------------

            nTempClass = g_CUserList.m_pUserInfo[i].m_sUser.nClass;
            PutSprite( l_SPR_Class,  729, nYPos + 2, nTempClass );


            // ���� ------------------------------------------------------
            
		    if( g_CUserList.m_pUserInfo[i].m_sUser.cSex == '0' ) {   // ����
                PutSprite( l_SPR_InviteIcon, 776, nYPos, 1 );
		    }
		    else {                                                          // ����
                PutSprite( l_SPR_InviteIcon, 776, nYPos, 0 );
		    }


            // �г��� ----------------------------------------------------

            NickSave( m_szTempText, g_CUserList.m_pUserInfo[i].m_sUser.szNick_Name );

            if ( (i - m_SCB_WaitUser->GetStartElem()) == m_nHighlightBarNum ) {
                DrawText( m_szTempText, 850, nYPos + 5, RGB( 0, 0, 0 ), FW_NORMAL, TA_CENTER );
            }
            else {
                DrawText( m_szTempText, 850, nYPos + 5, RGB( 0, 0, 0 ), FW_NORMAL, TA_CENTER );
            }

            // �ʴ� ���� ���� ǥ�� ---------------------------------------

            if ( g_CUserList.m_pUserInfo[i].m_sUser.bInvite ) {
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
        if( l_nViewRoomMessage >= 0 ) {

            nYPos = 233;

		    for( int j = 0 ; j < MAX_ROOM_IN; j++ ) {
                if ( g_ClickRoomUserInfo[j].nUserNo <= 0 ) {
                    continue;
                }


                // ��� --------------------------------------------------

                nUserNo = g_ClickRoomUserInfo[j].nClass;

                l_SPR_Class->SetFrame( nUserNo );
			    l_SPR_Class->SetPos( 729 , nYPos + 2 );
                l_SPR_Class->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );


                // ���� --------------------------------------------------

			    if( g_ClickRoomUserInfo[j].cSex == '0' ) {  // ����
                    PutSprite( l_SPR_InviteIcon, 776, nYPos, 1 );
			    }
			    else {                                      // ����
                    PutSprite( l_SPR_InviteIcon, 776, nYPos, 0 );
			    }


                // �г��� ------------------------------------------------

                NickSave( m_szTempText, g_ClickRoomUserInfo[j].szNick_Name );

                if (j == m_nHighlightBarNum ) {
                    DrawText( m_szTempText, 850, nYPos + 5, RGB( 0, 0, 0 ), FW_NORMAL, TA_CENTER );
                }
                else {
                    DrawText( m_szTempText, 850, nYPos + 5, RGB( 0, 0, 0 ), FW_NORMAL, TA_CENTER );
                }

                // �ʴ� ���� ���� ǥ�� ---------------------------------------

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

    //---------------------------------------
    // ���õ� �� ������ ���� �ڼ��� ���� ���
    //---------------------------------------

    // ������ ���� ������ �ѷ��ش�. PutTextRoomUserState ( �濡 �ִ� )

	int nWinCnt;
    int nLoseCnt;
    int nAllCnt;
    int nDrawCnt;

    l_nScrollSelNo = l_nViewRoomUser;

    if ( ( g_ClickRoomUserInfo[ l_nViewRoomUser ].nUserNo > 0 && !l_bShowUserState) && l_nViewRoomUser >= 0 ) 
	{
		if ( l_nViewRoomMessage >= 0 )  {

			// ����
			if ( strlen( g_ClickRoomUserInfo[l_nViewRoomUser].szNick_Name ) > 10 ) {
				memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
				memcpy( g_szTempStr, g_ClickRoomUserInfo[l_nViewRoomUser].szNick_Name, 10 );
				strcat( g_szTempStr, "..." );

                DrawText( g_szTempStr, 885, 591, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
			}
			else {
                DrawText( g_ClickRoomUserInfo[l_nViewRoomUser].szNick_Name, 885, 591, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
			}

			// �ݾ�
            if ( MouseIn(832, 613, 978, 627) ) {
			    I64toA_Money( g_ClickRoomUserInfo[l_nViewRoomUser].biUserMoney, m_szTempText, 0 );
            }
            else {
                I64toA_Money( g_ClickRoomUserInfo[l_nViewRoomUser].biUserMoney, m_szTempText );
            }
            DrawText( m_szTempText, 885, 615, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

			// ���
			nUserNo = g_ClickRoomUserInfo[l_nViewRoomUser].nClass;
            PutSprite( l_SPR_Class, 885, 636, nUserNo );

			nWinCnt   =  g_ClickRoomUserInfo[l_nViewRoomUser].nWin_Num;
			nLoseCnt  =  g_ClickRoomUserInfo[l_nViewRoomUser].nLose_Num;
			nDrawCnt  =  g_ClickRoomUserInfo[l_nViewRoomUser].nDraw_Num;
			nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );
			
			// ����
			sprintf( m_szTempText , "%d�� %d��" , nAllCnt, nWinCnt );
            DrawText( m_szTempText, 885, 663, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

			sprintf( m_szTempText , "%d�� %d��" , nLoseCnt , nDrawCnt );
            DrawText( m_szTempText, 885, 687, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

			if ( ( l_nViewRoomMessage != l_nOldViewRoomMessage ) || 
				 ( l_nViewRoomUser != l_nOldViewRoomUser ))
            {
				l_nOldViewRoomMessage = l_nViewRoomMessage;
				l_nOldViewRoomUser = l_nViewRoomUser;
				l_nOldViewWaitUser = -1;
			}

            // �ƹ�Ÿ
            if ( l_SPR_GifWaitAvatar != NULL )
                PutSpriteAvatar( l_SPR_GifWaitAvatar, 722, 568, SMALL_AVATAR_SIZE );
		}
	}

    //-------------------------------------
    // ���õ� ����ڿ����� �ڼ��� ���� ���
    //-------------------------------------
	
	// ������ ���� ������ �ѷ��ش�. PutTextWaitUserState ( �濡 ���� )

    l_nScrollSelNo = l_nViewWaitUser;

	if (( g_CUserList.m_pUserInfo[ l_nScrollSelNo ].m_bUser == TRUE && l_bShowUserState ) || 
        ( l_nViewRoomUser < 0 )) 
	{
		if ( l_nScrollSelNo >= 0 ) {

			// ����
			if ( strlen( g_CUserList.m_pUserInfo[ l_nScrollSelNo ].m_sUser.szNick_Name ) > 10 ) {
				memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
				memcpy( g_szTempStr, g_CUserList.m_pUserInfo[ l_nViewWaitUser].m_sUser.szNick_Name, 10 );
				strcat( g_szTempStr, "..." );
                DrawText( g_szTempStr, 885, 591, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
			}
			else {
                DrawText( g_CUserList.m_pUserInfo[ l_nScrollSelNo].m_sUser.szNick_Name, 885, 591, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
			}

			// �ݾ�
            if ( MouseIn(832, 613, 978, 627) ) {
			    I64toA_Money( g_CUserList.m_pUserInfo[ l_nScrollSelNo].m_sUser.biUserMoney, m_szTempText, 0 );
            }
            else {
                I64toA_Money( g_CUserList.m_pUserInfo[ l_nScrollSelNo].m_sUser.biUserMoney, m_szTempText );
            }
            DrawText( m_szTempText, 885, 615, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

			// ���
			nUserNo = g_CUserList.m_pUserInfo[ l_nViewWaitUser].m_sUser.nClass;
            PutSprite( l_SPR_Class, 885, 636, nUserNo );

			nWinCnt  = g_CUserList.m_pUserInfo[ l_nScrollSelNo].m_sUser.nWin_Num;
			nLoseCnt = g_CUserList.m_pUserInfo[ l_nScrollSelNo].m_sUser.nLose_Num;
			nDrawCnt = g_CUserList.m_pUserInfo[ l_nScrollSelNo].m_sUser.nDraw_Num;

			nAllCnt = ( nWinCnt + nLoseCnt + nDrawCnt );

			// ����
			sprintf( m_szTempText , "%d�� %d��" , nAllCnt, nWinCnt );
            DrawText( m_szTempText, 885, 663, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

			sprintf( m_szTempText , "%d�� %d��" , nLoseCnt, nDrawCnt );
            DrawText( m_szTempText, 885, 687, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );

			if ( l_nViewWaitUser != l_nOldViewWaitUser ) {
				l_nOldViewWaitUser = l_nViewWaitUser;
				//��������Ʈ �����ߴٰ� ���ӹ� �����ߴٰ� �ϱ⶧����..
				l_nOldViewRoomMessage = -1;
			}
	
            if ( l_SPR_GifWaitAvatar != NULL ) {
                PutSpriteAvatar( l_SPR_GifWaitAvatar, 722, 568, SMALL_AVATAR_SIZE );
            }
		}
	}

    //--------------------
    // �� ����� â �׸���
    //--------------------
    
	if ( l_bShowMakeRoomDlg ) {
		RenderMakeRoom();                       // �游��� ���̾˷α� ������
	}

    //--------------------------------------
    // ��й� �� �� �н����� �Է��ϴ� â
    //--------------------------------------

    if ( m_bPassDlg ) {
        RenderPassDlg();                        // ��й� �� ���� ��й�ȣ �Է� â
    }
}



void CMainFrame::RenderMakeRoom()   // �游��� â ������
{
    PutSprite( l_SPR_MakeRoom, MAKEROOM_POS_X, MAKEROOM_POS_Y );     // �游��� â

    m_RoomTitleBTN->DrawButton();
	l_MakeRoomOkBTN->DrawButton();
	l_MakeRoomCancelBTN->DrawButton();

    // �н����� üũ�� ������� ��Ŀ���� ������ ��� ����� ĥ�Ѵ�.
    if ( m_IME_InputPass->IsFocus() ) {
        g_pCDXScreen->GetBack()->FillRect(
            MAKEROOM_POS_X + 96,
            MAKEROOM_POS_Y + 86,
            MAKEROOM_POS_X + 96 + 176,
            MAKEROOM_POS_Y + 86 + 16,
            RGB(255, 255, 255)
        );
    }

    // �н����� üũ ǥ��
    if ( m_bPassRoomChk ) {
        if ( m_IME_InputPass->IsInputState() ) {
            m_IME_InputPass->PutIMEText( MAKEROOM_POS_X + 98, MAKEROOM_POS_Y + 89, RGB(0, 0, 0), RGB(0, 0, 0) );
        }

        // �н����� üũ ���
        g_pCDXScreen->GetBack()->FillRect(
            MAKEROOM_POS_X + 279,
            MAKEROOM_POS_Y + 87,
            MAKEROOM_POS_X + 279 + 14,
            MAKEROOM_POS_Y + 87 + 14,
            RGB( 255, 255, 255 )
        );

        // �н����� üũ ǥ��
        DrawText( "V", MAKEROOM_POS_X + 283, MAKEROOM_POS_Y + 89, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
    }
    
    //�� ������ ����Ѵ�.
    if (m_IME_InputRoomTitle->IsInputState()) {
        if ( m_IME_InputRoomTitle->IsFocus() ) {
            g_pCDXScreen->GetBack()->FillRect( MAKEROOM_POS_X + 96, MAKEROOM_POS_Y + 61,
                                               MAKEROOM_POS_X + 277, MAKEROOM_POS_Y + 77,
                                               RGB(255, 255, 255));
        }
        m_IME_InputRoomTitle->PutIMEText( MAKEROOM_POS_X + 98, MAKEROOM_POS_Y + 64, RGB(0, 0, 0), RGB(0, 0, 0) );
    }

    PutSprite( m_SPR_CheckBall, l_sMakeRoomMoney[m_nMakeRoomMoneyNo].x , l_sMakeRoomMoney[m_nMakeRoomMoneyNo].y, 1 );

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



void CMainFrame::RenderPassDlg()
{
    m_SPR_PassDlg->SetPos( PASS_DLG_POS_X, PASS_DLG_POS_Y );
    m_SPR_PassDlg->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );

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
    
	m_PassDlgOkBTN->DrawButton();
	m_PassDlgCancelBTN->DrawButton();
}



void CMainFrame::PutCursorText( int X , int Y , COLORREF rgb , char *szString )
{
	CurT1 = GTC();

	if( CurT1 - CurT2 > 50 ) {
		CurT2 = GTC();
		if( CurTime++ > 5 ) {		
			SetCur = !SetCur;
			CurTime = 0;		
		}
	}
	
	SIZE tsize;

    GDI_GetStringSize( szString , &tsize );

	int nTemp = X + tsize.cx - 2;

    if( SetCur ) {
        DrawText( "|", X + tsize.cx - 2 , Y , rgb, FW_NORMAL, TA_LEFT );
    }
}



void CMainFrame::RenderUserInfo()
{
	char str[256];
	char strtemp[256];
    int nTemp = 0;


    // �г���
	sprintf(str, "%s", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name );

	if ( strlen(str) > 12 ) {
		memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
		memcpy( g_szTempStr, str, 12 );
		strcat( g_szTempStr, "..." );

        DrawText( g_szTempStr, 861, 672, RGB(153, 204, 153), FW_BOLD, TA_CENTER );
        DrawText( g_szTempStr, 860, 671, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
	}
	else {
        DrawText( str, 861, 672, RGB( 153, 204, 153 ), FW_BOLD, TA_CENTER );
        DrawText( str, 860, 671, RGB(   0,   0,   0 ), FW_BOLD, TA_CENTER );
	}

    // ���
    nTemp = g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.nClass;
    PutSprite( l_SPR_Class,  853, 685, nTemp );

    // ���� �Ӵ�
    if ( MouseIn( 860 - 70, 705 - 2, 860 + 70, 705 + 12 )) {
        I64toA_Money( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.biUserMoney, str, 0 );
    }
    else {
        I64toA_Money( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.biUserMoney, str );
    }
    DrawText( str, 860, 705 , RGB( 51, 102, 51 ), FW_BOLD, TA_CENTER );

	if ( g_pGoStop->m_pFloor.m_pGamer[0].m_biCurWinLoseMoney > 0 ) {
		memset( strtemp, 0x00, sizeof(strtemp) );
		I64toA_Money( g_pGoStop->m_pFloor.m_pGamer[0].m_biCurWinLoseMoney, strtemp );
		sprintf(str, "+%s", strtemp );
	}
	else {
		memset( strtemp, 0x00, sizeof(strtemp) );

		if ( g_pGoStop->m_pFloor.m_pGamer[0].m_biCurWinLoseMoney == 0 ) {
			strcpy(str, "0Ĩ" );
		}
		else {
			I64toA_Money( fabs(g_pGoStop->m_pFloor.m_pGamer[0].m_biCurWinLoseMoney), strtemp );
			sprintf(str, "-%s", strtemp );
		}
	}

	//���� + , - ���� �����ش�.
    DrawText( str, 860, 721, RGB( 0, 255, 255 ), FW_BOLD, TA_CENTER );

	// �̼��� ������ Mission Complete �� ����.
	// MASTER_POS SLOT
	if( g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bMission ) {
		PutSprite( l_SPR_MissionOK, 938, 613 );
	}
	
	if ( ( !g_pGoStop->m_pFloor.RealGame() && g_nRoomCurCnt == 1 )
		|| g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo < 0  ) {
		
		if ( g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo < 0 && g_pGoStop->m_pFloor.RealGame() ) {
            DrawText( "WAITUSER1", 861, 98, RGB(153, 204, 153), FW_BOLD, TA_CENTER );
            DrawText( "WAITUSER1", 860, 97, RGB(  0,   0,   0), FW_BOLD, TA_CENTER );
		}
		else {
            DrawText( "Com1", 861, 98, RGB(153, 204, 153), FW_BOLD, TA_CENTER );
            DrawText( "Com1", 860, 97, RGB(  0,   0,   0), FW_BOLD, TA_CENTER );
		}
		
        DrawText( "0Ĩ", 860, 113, RGB(  51, 102, 51 ), FW_BOLD, TA_CENTER );    // 1�� ������ ���� �ִ� ���� �����ش�.
        DrawText( "0Ĩ", 860, 129, RGB( 153, 255,  0 ), FW_BOLD, TA_CENTER );    // 1�� ������ + , - ���� �����ش�.

		// �̼��� ������ Mission Complete �� ����.
		// 1 SLOT
		if( g_pGoStop->m_pFloor.m_pGamer[1].m_bMission ) {
            PutSprite( l_SPR_MissionOK, 938, 39 );
		}
	}
	else {
		sprintf(str, "%s", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo].m_sUserInfo.szNick_Name );
		if ( strlen(str) > 12 ) {
			memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
			memcpy( g_szTempStr, str, 12 );
			strcat( g_szTempStr, "..." );

            DrawText( g_szTempStr, 861, 98, RGB(153, 204, 153), FW_BOLD, TA_CENTER );
            DrawText( g_szTempStr, 860, 97, RGB(  0,   0,   0), FW_BOLD, TA_CENTER );
		}
		else {
            DrawText( str, 861, 98, RGB(153, 204, 153), FW_BOLD, TA_CENTER );
            DrawText( str, 860, 97, RGB(  0,   0,   0), FW_BOLD, TA_CENTER );
		}

        // ���
        nTemp = g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo].m_sUserInfo.nClass;
        PutSprite( l_SPR_Class,  853, 111, nTemp );

        // ���� �Ӵ�
        if ( MouseIn( 860 - 70, 131 - 2, 860 + 70, 131 + 12 )) {
            I64toA_Money( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo].m_sUserInfo.biUserMoney, str, 0 );
        }
        else {
            I64toA_Money( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo].m_sUserInfo.biUserMoney, str );
        }
        DrawText( str, 860, 131, RGB( 51, 102, 51 ), FW_BOLD, TA_CENTER );

		if ( g_pGoStop->m_pFloor.m_pGamer[1].m_biCurWinLoseMoney > 0 ) {
			memset( strtemp, 0x00, sizeof(strtemp) );
			I64toA_Money( g_pGoStop->m_pFloor.m_pGamer[1].m_biCurWinLoseMoney, strtemp );
			sprintf(str, "+%s", strtemp );
		}
		else {
			if ( g_pGoStop->m_pFloor.m_pGamer[1].m_biCurWinLoseMoney == 0 ) {
				strcpy( str, "0Ĩ" );
			}
			else {
				memset( strtemp, 0x00, sizeof(strtemp) );
				I64toA_Money( fabs(g_pGoStop->m_pFloor.m_pGamer[1].m_biCurWinLoseMoney), strtemp );
				sprintf(str, "-%s", strtemp );
			}
		}
		
		// 1�� ������ + , - ���� �����ش�.
        DrawText( str, 860, 147, RGB( 153, 255, 0 ), FW_BOLD, TA_CENTER );

		// �̼��� ������ Mission Complete �� ����.
		// 1 SLOT
		if( g_pGoStop->m_pFloor.m_pGamer[1].m_bMission ) {
            PutSprite( l_SPR_MissionOK, 938, 39 );
		}
	}

    // �ڽ��� �ƹ�Ÿ�� �׸���.
    if ( g_sPaintAvatar.bPrint[0] && l_SPR_GifAvatar[0] != NULL ) {
        DrawScaleBlk( l_SPR_GifAvatar[0], m_sAvatarPos[0].x, m_sAvatarPos[0].y, 85, 120 );
	}

	// �������� �ƹ�Ÿ�� �׸���.
	if ( g_pGoStop->m_pFloor.RealGame() ) { //���� �����̸� ���� �ƹ�ŸŸ�� �Ѹ���.
        if ( g_sPaintAvatar.bPrint[1] && l_SPR_GifAvatar[1] != NULL ) {
            DrawScaleBlk( l_SPR_GifAvatar[1], m_sAvatarPos[1].x, m_sAvatarPos[1].y, 85, 120 );
        }
	}
	else {
        PutSprite( l_SPR_COM, m_sAvatarPos[1].x, m_sAvatarPos[1].y );    //��ǻ�͸� ��ǻ�� �ƹ�Ÿ�� �Ѹ���.
	}

    // ������ ������ �׸���.

    if ( m_bShowExitReservIcon[0] == TRUE ) {
        PutSprite( m_SPR_ExitReserv, m_sExitResrv[0].x, m_sExitResrv[0].y );
    }

    if ( m_bShowExitReservIcon[1] == TRUE ) {
        PutSprite( m_SPR_ExitReserv, m_sExitResrv[1].x, m_sExitResrv[1].y );
    }
}



BOOL CMainFrame::MouseIn( int X , int Y , int X2 , int Y2 )
{
	if( l_mX >= X && l_mY >= Y && l_mX <= X2 && l_mY <= Y2 )
		return TRUE;

	return FALSE;
}

void CMainFrame::RenderGameBase()
{
    PutSpriteBlk( l_SPR_GameBase ); // ���� �� ���

    // ���� ������ �������� ȭ�� ������� ������ �����ش�.
	if( l_bShowTurnLine && g_CurUser >= 0 ) {
		int nSlot = ( g_CurUser + g_pGoStop->m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;
		ShowTurnLine( nSlot );
	}
	
	// ä��â �׸���
    RenderGameChat();


    // ���� ���� ȭ��
    if ( m_bSunSetRender ) {
        RenderSunPack();
    }

    
    // ���� �������� ���� ȭ��
    if ( m_bRenderSun ) {
        RenderSunDecided();
    }
	
	RenderUserInfo();

	if( g_pGoStop->m_bPlaying == TRUE ) // ���� �������̸�
	{
        RenderMission();
		RenderScore();          // ���� ��� 01.08
		RenderCardPack();       // ī���� ������
		RenderUserEatCard();    // ������ ���� �� ������
		RenderFloor();          // �ٴ��� ������
		RenderUserCard();       // ������ ������

		if ( !g_bDeal ) {
			for ( int i = 0; i < 53; i++ ) {
				if ( l_CardPos[i].type != USERCARD ) {  //�̰� �ȳ����� �ٸ������ ���񽺸� �������� �а� �ȴ�.
					if(l_AniStack.CheckCard(&l_CardPos[i])) {
                        if ( l_CardPos[i].type == EATCARD ) {
                            PutHwatuImage(l_CardPos[i].Pos.x, l_CardPos[i].Pos.y, i);
                        }
                        else {
                            PutHwatuImage(l_CardPos[i].Pos.x, l_CardPos[i].Pos.y, i );
                        }
					}
				}
			}
		}


		/* 
			�ִϸ��̼� ���ؿ� ����ִ�(�ִϸ��̼�����) �е��� ���� �������� ��
			�ٸ� �е� �ؿ� �������� �ʰ� �Ѵ�
		*/
		if(l_nTempCardNo > -1)
		{
			int nCount = g_pGoStop->m_Hwatu.m_pCardPack->RemainCardCnt();
			int x = l_CardPos[l_nTempCardNo].Pos.x;
			int y = l_CardPos[l_nTempCardNo].Pos.y;

			if(l_CardPos[l_nTempCardNo].Pos.x == l_CardPackPos.x &&
               l_CardPos[l_nTempCardNo].Pos.y == l_CardPackPos.y)
			{
				x = x - ( nCount >> 1 );        // x = x - ( nCount / 2 )
				y = y - ( nCount >> 1 );        // y = y - ( nCount / 2 )
			}
			// ī�� ���ø� �̹��� ���
			PutHwatuImage(x, y, l_nTempCardNo, TRUE); // bsw 01.05
		}

		if( l_nMouseOverCardNo > -1 ) {         //���콺 ���� ������.....������ �� ��°�..
			PutHwatuImage(l_CardPos[l_nMouseOverCardNo].Pos.x, l_CardPos[l_nMouseOverCardNo].Pos.y, l_nMouseOverCardNo);
		}

		if(l_bDrawEvent) {
			DrawEvent(l_nDrawEventSlotNo, l_nDrawEventType);
		}

		if (l_bTimeEvent) {
			int nSlot = ( g_CurUser + g_pGoStop->m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;
			TimeEvent(nSlot);
		}
	}

	ShowDialog(l_bShowDialog, l_nDialgType, (int)((double)l_nDialogWaitFrameCnt/(l_nWaitTime/100.0)));

	//�� ���� �׸���
	for ( int i = 0 ; i < MAX_ROOM_IN; i++ ) {
		if( l_nPVal[i] > 0 ) {
            PutSpriteAlpha( l_SPR_P_Back, l_nPX[i] + 30, l_nPY[i], 128 );
			sprintf( l_szTemp[i] , "%d" , l_nPVal[i] );
            DrawText( l_szTemp[i], l_nPX[i] + 36, l_nPY[i] + 2, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
		}
	}

    if ( l_bShowResult ) {
        ShowResult();
    }

	if( l_bShowStartBTN )
	{
		l_StartBTN->DrawButton(); // ���� ��ư ���	
		
		if( MouseIn( 427, 353, 427 + 170, 353 + 62 )) {
			l_StartBTNState = 1;
		}
        else {
            l_StartBTNState = 0;
        }
		
		if( l_StartBTNState == 0 ) {
            if( l_bShowStartBTN ) {
                PutSprite( l_SPR_Start, 427, 353, l_StartAni );
            }
			
			l_StartT1 = GTC();
			if( l_StartT1 - l_StartT2 > 300 ) {
				l_StartT2 = GTC();

                if( ++l_StartAni > 1 ) {
                    l_StartAni = 0;
                }
			}
		}
	}

	l_InviteBTN->DrawButton();  // ���� �ʴ� ��ư�� �׸���.

    //-----------------------------------
    // �ʴ��ϴ� ����� ��ȭ���ڸ� �׸���.
    //-----------------------------------

	if( l_bShowInviteBox ) {
        PutSprite( l_SPR_InviteBox, CHODAE_POS_X, CHODAE_POS_Y );
		
		DrawInviteUsers( CHODAE_POS_X, CHODAE_POS_Y );      // �ʴ��ϴ� ����� ��ȭ����

        m_SCB_Invite->Draw();
		l_InviteOKBTN->DrawButton();
        l_InviteCancelBTN->DrawButton();
	}

    //----------------------
    // ���� ǥ�� �׸��� �κ�
    //----------------------

	switch( g_pGoStop->m_pFloor.m_nBbangJjangSlotNo ) {
	    case MASTER_POS:
            PutSprite( l_SPR_KingFirst, m_sBangJangMarkPos[0].x, m_sBangJangMarkPos[0].y, 0 );
		    break;

	    case 1:
            PutSprite( l_SPR_KingFirst, m_sBangJangMarkPos[1].x, m_sBangJangMarkPos[1].y, 0 );
		    break;
	}

	
    //--------------------
	// �� ǥ�� �׸��� �κ�
    //--------------------

	switch( g_pGoStop->m_pFloor.m_nMasterSlotNo ) {
	    case MASTER_POS:
            PutSprite( l_SPR_KingFirst, m_sSunMarkPos[0].x, m_sSunMarkPos[0].y, 1 );
		    break;

	    case 1:
            PutSprite( l_SPR_KingFirst, m_sSunMarkPos[1].x, m_sSunMarkPos[1].y, 1 );
		    break;
	}

	sprintf( l_szTempText, "�°�[%dä��]", g_szGameServerChannel + 1 );
    DrawText( l_szTempText, 13, 10, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );

	//sprintf( l_szTempText, "%d����  ���� - %s   [�� : %dĨ]",  g_nRoomNo + 1, g_szRoomName, g_biRoomMoney );
	sprintf( l_szTempText, "%d����  ���� - %s   [�� : %dĨ]",  g_nRoomNo + 1, g_szRoomName, g_biRoomMoney );
    DrawText( l_szTempText, 300, 10, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );
}



//========================================================================
// �̼� ��ȭ���ڸ� �׸���.
//========================================================================
void CMainFrame::RenderMission()
{
	if( l_bShowMission && g_pGoStop->m_bPlaying ) {     // �̼� ��� ����

        // �̼� ��ȭ���ڸ� �׸���.
        PutSprite( m_SPR_MissionDlg, m_sMissionDlgInfo.nDlgX, m_sMissionDlgInfo.nDlgY );

        //--------------------------------------------------------
        // �̼� ������ mission Complete �� "������ �̼Ǽ���!" ���
        //--------------------------------------------------------

        if( g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bMission ) {
            PutSprite( l_SPR_MComplete, m_sMissionDlgInfo.nDlgX, m_sMissionDlgInfo.nDlgY );

            DrawMissionCompleteUser( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name );
        }
        else if( g_pGoStop->m_pFloor.m_pGamer[1].m_bMission ) {
            
            PutSprite( l_SPR_MComplete, m_sMissionDlgInfo.nDlgX, m_sMissionDlgInfo.nDlgY );

            if ( ( !g_pGoStop->m_pFloor.RealGame() && g_nRoomCurCnt == 1 )
		        || g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo < 0  ) {
		      
		        if ( g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo < 0 && g_pGoStop->m_pFloor.RealGame() ) {
                    DrawMissionCompleteUser( "WAITUSER1" );
		        }
		        else {
                    DrawMissionCompleteUser( "Com1" );
		        }
	        }
	        else {
                DrawMissionCompleteUser( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[1].m_nGamerUserNo].m_sUserInfo.szNick_Name );
            }
        }

        // �̼� ī�� ���

        int nX;
        int nY;
        
        if ( g_sMissionCardInfo.nCardCnt == 5 ) {   // 5���� ���
            nX = m_sMissionDlgInfo.nDlgX + 15;
            nY = m_sMissionDlgInfo.nDlgY + 48;
        }
        else {                                      // 5���� �ƴ� ���
            nX = m_sMissionDlgInfo.nDlgX + 25;
            nY = m_sMissionDlgInfo.nDlgY + 48;
        }

        for ( int i = 0; i < g_sMissionCardInfo.nCardCnt; i++ ) {
            if ( g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bMission || g_pGoStop->m_pFloor.m_pGamer[1].m_bMission ) {
                // �� �� �Ѹ��� �̼��� ���������� ������ ���� ī��� ����Ѵ�.
                PutHwatuImage( nX, nY, g_sMissionCardInfo.nCardNo[i] % 52 );
            }
            else {
                // ���� �̼�ī��� ��Ӱ�, ���� ���� �̼�ī��� ��� ����Ѵ�.
                PutHwatuImage( nX, nY, g_sMissionCardInfo.nCardNo[i] );

                if ( g_sMissionCardInfo.bEat[i] == TRUE ) {
                    if ( g_sMissionCardInfo.nUserNo[i] == g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_nUserNo ) {
                        PutSprite( m_SPR_Arrow, nX, nY + HWATU_HEIGHT - 5, 0 ); // ���� �Ծ��ٴ� ǥ��
                    }
                    else {
                        PutSprite( m_SPR_Arrow, nX, nY - 10, 1 );               // ������ �Ծ��ٴ� ǥ��
                    }
                }
            }

            nX += 25;
        }
        
        //// �̼� ��� ���
        if( g_pGoStop->m_nMissionType == OKWANG ) {             // ����
            PutSprite( l_SPR_MissionTimes, m_sMissionDlgInfo.nDlgX + 160, m_sMissionDlgInfo.nDlgY + 50, 4 );       // x5
        }
		else if( g_pGoStop->m_nMissionType == KKWANG ) {        // ��, ���� ��ȸ��
            PutSprite( l_SPR_MissionNext, m_sMissionDlgInfo.nDlgX + 41, m_sMissionDlgInfo.nDlgY + 64 );           // ���� ��ȸ��...
		}
        else if ( g_pGoStop->m_nMissionType == DAEBAK ) {       // ���
            PutSprite( l_SPR_EVENT, m_sMissionDlgInfo.nDlgX - 27, m_sMissionDlgInfo.nDlgY + 1, 28 );
            PutSprite( l_SPR_MissionTimes, m_sMissionDlgInfo.nDlgX + 160, m_sMissionDlgInfo.nDlgY + 50, g_pGoStop->m_nMissionMultiply - 1 );
        }
        else {
            PutSprite( l_SPR_MissionTimes, m_sMissionDlgInfo.nDlgX + 160, m_sMissionDlgInfo.nDlgY + 50, g_pGoStop->m_nMissionMultiply - 1 );
        }
	}
}



//========================================================================
// ���� ����, ��/���/�� Ƚ�� ���
//========================================================================
void CMainFrame::RenderScore()
{
	ShowUserScore();
}



//========================================================================
// ������ ī������ �׷��ش�
//
// RenderCardPack()�� ���� ���� �������� ī�尡 ���� ���� �ֱ� �빮��
// ���Ƿ� ī������ �׷��ֱ� ���ؼ� �� �Լ��� ����Ѵ�.
//========================================================================
void CMainFrame::RenderDummyCardPack()
{
    for ( int i = 0 ; i < 51; i += 4 ) {
		PutHwatuBackImage(l_CardPackPos.x - ( i >> 1 ), l_CardPackPos.y - ( i >> 1 ));
	}
}



//========================================================================
// ī���� ������
//========================================================================
void CMainFrame::RenderCardPack()
{
	int nCount = g_pGoStop->m_Hwatu.m_pCardPack->RemainCardCnt();

	if ( g_pGoStop->m_Hwatu.m_pCardPack == NULL ) return; //���������� Run�ϼ��� �ִ�.

	for ( int i = 0 ; i < nCount; i += 4 ) {
		PutHwatuBackImage(l_CardPackPos.x - ( i >> 1 ), l_CardPackPos.y - ( i >> 1 ));
	}
	
	if(g_pGoStop->m_pFloor.m_pGamer[0].m_nBombFlipCnt > 0) {
		int nSlot = ( g_CurUser + g_pGoStop->m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;

		if(l_bShowBombLine && nSlot == MASTER_POS && !l_bShowDialog) {
            PutSprite( l_SPR_BombLine, l_CardPackPos.x - ( nCount >> 1 ), l_CardPackPos.y - ( nCount >> 1 ) );
		}
	}

#ifdef SHOWUSERCARD
    // ī������ �� �� ī�� ���
    PutHwatuImage(l_CardPackPos.x - ( nCount >> 1 ), l_CardPackPos.y - ( nCount >> 1 ),
                  g_pGoStop->m_Hwatu.m_pCardPack->m_pCardPack[g_pGoStop->m_Hwatu.m_pCardPack->m_nReadNo]);
#endif

	if(g_nCardPackNo > -1) {
		int nCount = g_pGoStop->m_Hwatu.m_pCardPack->RemainCardCnt();
		int x = l_CardPos[g_nCardPackNo].Pos.x;
		int y = l_CardPos[g_nCardPackNo].Pos.y;

		if (l_CardPos[g_nCardPackNo].Pos.x == l_CardPackPos.x &&
            l_CardPos[g_nCardPackNo].Pos.y == l_CardPackPos.y)
		{
            x -= ( nCount >> 1 );       // x = x - nCount / 2;
            y -= ( nCount >> 1 );       // y = y - nCount / 2;
		}
		
		PutHwatuImage(x, y, g_nCardPackNo, TRUE); // ī�� ���ø� �̹��� ���
	}
}



//========================================================================
// ������ ���� �и� �׸���.
//========================================================================

void CMainFrame::RenderUserEatCard()        // ������ ���� �� ������
{
	int i, j, l, nCardNo;

	for(l = 0; l < MAX_ROOM_IN; l++) {      // ���� ��ȣ
		for(j = 0; j < 3; ++j) {            // ��, ��, �׸�
			for(i = 0; i < g_pGoStop->m_pFloor.m_pGamer[l].m_nEatCnt[j]; ++i) {
				nCardNo = g_pGoStop->m_pFloor.m_pGamer[l].m_nEatCard[j][i];
				
				if( !l_AniStack.CheckCard( &l_CardPos[nCardNo] )) {
					PutHwatuImage( l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo );
				}
			}
		}

		// ��
		for(i = 0; i < g_pGoStop->m_pFloor.m_pGamer[l].m_nEatCnt[j]; ++i) {
			nCardNo = g_pGoStop->m_pFloor.m_pGamer[l].m_nEatCard[j][i];
			
			if( !l_AniStack.CheckCard( &l_CardPos[nCardNo] )) {
				PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo); // bsw 01.05
			}
		}
	}
}



//========================================================================
// ���� �����ϴ� ȭ���� �׸���.
//========================================================================

void CMainFrame::RenderSunPack()
{
	for ( int i = 0; i < MAX_SUNCARD_CNT; i++ ) {
        // ���� �������� ���� ��쿡�� �����Ÿ�
        if ( !m_bRenderSun ) {
            if ( m_nCardBlinkFrm == 0 ) {
                PutSprite( m_SPR_CardBlink, m_szSunPackPos[i].Pos.x - 3, m_szSunPackPos[i].Pos.y - 3 );
            }
        }
        
#ifdef SHOWUSERCARD
        PutHwatuImage( m_szSunPackPos[i].Pos.x,
                       m_szSunPackPos[i].Pos.y,
                       m_szSunPack[i] );
#else
		PutHwatuBackImage( m_szSunPackPos[i].Pos.x, m_szSunPackPos[i].Pos.y );
#endif
	}

    if ( m_nMySunChioce != -1 ) {
        PutHwatuImage( m_szSunPackPos[m_nMySunChioce].Pos.x,
                       m_szSunPackPos[m_nMySunChioce].Pos.y,
                       m_szSunPack[m_nMySunChioce] );
    }

    if ( m_nOtherSunChioce != -1 ) {
        PutHwatuImage( m_szSunPackPos[m_nOtherSunChioce].Pos.x,
                       m_szSunPackPos[m_nOtherSunChioce].Pos.y,
                       m_szSunPack[m_nOtherSunChioce] );
    }

    // ī���� ���
    RenderDummyCardPack();

    // ���� ���մϴ� - �и� ������ �ּ���. ��ȭ����
    PutSpriteAlpha( m_SPR_SelSunTextBack, 249, 366, 100 );
    PutSprite( m_SPR_SelSunText, 249, 366 );
}



//========================================================================
// ���� �����ϱ� ���� �и� ����� �� �ߴ� ȭ��
//========================================================================

void CMainFrame::RenderSunDecided()
{
	char str[256];

    // [alpha]
    DrawTransDialog( TDT_SEL_SUN, SUN_DLG_POS_X, SUN_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );

	sprintf( str , "%s�� ��", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_nMasterSlotNo].m_sUserInfo.szNick_Name );
    DrawText( str, SUN_DLG_POS_X + 124, SUN_DLG_POS_Y + 31, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );

	if ( g_pGoStop->m_pFloor.m_nMasterSlotNo == MASTER_POS ) {
		PutHwatuImage( SUN_DLG_POS_X + 60, SUN_DLG_POS_Y + 63, m_szSunPack[m_nMySunChioce] );
        DrawText( "��", SUN_DLG_POS_X + 82, SUN_DLG_POS_Y + 141, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );

		PutHwatuImage( SUN_DLG_POS_X + 145, SUN_DLG_POS_Y + 63, m_szSunPack[m_nOtherSunChioce] );
        DrawText( "����", SUN_DLG_POS_X + 167, SUN_DLG_POS_Y + 141, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
	}
	else {
		PutHwatuImage( SUN_DLG_POS_X + 60, SUN_DLG_POS_Y + 63, m_szSunPack[m_nOtherSunChioce] );
        DrawText( "����", SUN_DLG_POS_X + 82, SUN_DLG_POS_Y + 141, RGB( 0, 0, 0 ), FW_BOLD , TA_CENTER );

		PutHwatuImage( SUN_DLG_POS_X + 145, SUN_DLG_POS_Y + 63, m_szSunPack[m_nMySunChioce] );
        DrawText( "��", SUN_DLG_POS_X + 167, SUN_DLG_POS_Y + 141, RGB( 0, 0, 0 ), FW_BOLD, TA_CENTER );
	}
}



//========================================================================
// �ٴ� �� ������
//========================================================================

void CMainFrame::RenderFloor()
{
	for(int i = 0 ; i < g_pGoStop->m_pFloor.m_nCardCnt; ++i)
	{
		int nCardNo = g_pGoStop->m_pFloor.m_strCard[i].nCardNo;

		POINT pos = g_pGoStop->m_pFloor.m_pos[g_pGoStop->m_pFloor.m_strCard[i].nPosNo];
		int cnt = g_pGoStop->m_pFloor.GetCurPosCnt(i);

		pos.x += (cnt * 10);

		if(l_CardPos[nCardNo].NewPos.x != pos.x || l_CardPos[nCardNo].NewPos.y != pos.y) // ���ο� ��ǥ�� �ԷµǾ��ٸ�
		{
			l_CardPos[nCardNo].NewPos = pos;
			l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
			l_CardPos[nCardNo].type = FLOOR;

			l_AniStack.AddStack(&l_CardPos[nCardNo]); // �ִϸ��̼� ���ÿ� �ֱ�
		}

		if ( g_bDeal ) {
			PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo); // bsw 01.05
		}
		else {
			if(l_CardPos[nCardNo].type == FLOOR && !l_AniStack.CheckCard(&l_CardPos[nCardNo])) {
				PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo); // bsw 01.05
			}
		}
	}
}



//========================================================================
// �� ������ �� ������
//========================================================================

void CMainFrame::RenderUserCard()
{
	int nTempCardNo = -1;

	for(int k = 0; k < MAX_ROOM_IN; k++) {
		for(int i = 0;  i < g_pGoStop->m_pFloor.m_pGamer[k].m_nCardCnt; ++i) {
			int nCardNo = g_pGoStop->m_pFloor.m_pGamer[k].m_bCard[i];
			
			if(( nCardNo == 0xff ) || ( nCardNo >= BOMB_CARDNO )) {
				//���õ� ī�带 ���� �������� �ѷ��� �ؿ� ī�庸�� ���߿� ������.
                //��źī���̸� �׳� �Ѿ������ ������ ���⿡ �ѹ��� �־��ش�.

				if ( i == (g_pGoStop->m_pFloor.m_pGamer[k].m_nCardCnt - 1 ) && nTempCardNo >= 0 ) {
					PutHwatuImage(l_CardPos[nTempCardNo].Pos.x, l_CardPos[nTempCardNo].Pos.y, nTempCardNo, TRUE); // bsw 01.05
					nTempCardNo = -1;
				}

				continue;
			}

			if( k == MASTER_POS ) {
				if ( g_bDeal ) {	//���� �ً��� �и� ������ ���ѷ���..
					PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo); // bsw 01.05
				}
				else {
                    //��ź�϶��� ��ź ����ִ°��� Ȯ���̹����� �Ѹ���. �Ʒ��� �ߺ��� �Ǳ� ������ else if
					if ( g_nBombCardNo[0] == nCardNo ||
                         g_nBombCardNo[1] == nCardNo ||
                         g_nBombCardNo[2] == nCardNo )
					{
						PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo, TRUE); // bsw 01.05
						continue;
					}
					else if ( g_SelectCardNo == nCardNo && g_bBomb == false ) {	//���õ� ī���϶��� Ȯ���̹����� �Ѹ���.
						nTempCardNo = nCardNo;
						
						// ���õ� ī�带 Ȯ�� �ϴµ� ������ ī�带 �������������� �̺κ��� �̿��ؼ� ������.
						if ( i == (g_pGoStop->m_pFloor.m_pGamer[k].m_nCardCnt - 1 ) ) {
							PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo, TRUE); // bsw 01.05
						}
						continue;
					}

					if (l_mX >= l_CardPos[nCardNo].Pos.x && l_mX <= l_CardPos[nCardNo].Pos.x + HWATU_WIDTH &&
						l_mY >= l_CardPos[nCardNo].Pos.y && l_mY <= l_CardPos[nCardNo].Pos.y + HWATU_HEIGHT &&
						l_bMouseDown == false )
					{
                        // ���콺�� �� ���� ������ Ȯ���ؼ� ���
						PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo, TRUE, 1);
					}
					else {
                        // ���� ������� ���
						PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo ); // bsw 01.05
					}

					//���õ� ī�带 ���� �������� �ѷ��� �ؿ� ī�庸�� ���߿� ������.
					//���õ� ī�尡 ������ ī�尡 �ƴҶ��� �̺κ��� �̿��ؼ� ������.
					if ( i == (g_pGoStop->m_pFloor.m_pGamer[k].m_nCardCnt - 1 ) && nTempCardNo >= 0 ) {
						PutHwatuImage(l_CardPos[nTempCardNo].Pos.x, l_CardPos[nTempCardNo].Pos.y, nTempCardNo, TRUE); // bsw 01.05
						nTempCardNo = -1;
					}
				
					if( g_pGoStop->m_pFloor.IsFloorHas(nCardNo) && g_CurUser == g_MyUser &&
						g_pGoStop->m_bMyKetInput == true ) // �ٴڿ� ���� �а� �ִ� ���
					{
                        if ( MouseIn( l_CardPos[nCardNo].Pos.x,
                                      l_CardPos[nCardNo].Pos.y,
                                      l_CardPos[nCardNo].Pos.x + HWATU_WIDTH,
                                      l_CardPos[nCardNo].Pos.y + HWATU_HEIGHT ))
                        {
                            // ���콺�� ���� ī�� ���� �ִ� ��� ǥ�� ���ϸ��̼�
                            PutSprite( l_SPR_HwatuSelect, l_CardPos[nCardNo].Pos.x + 10, l_CardPos[nCardNo].Pos.y, m_nHwatuSelectFrm );
                        }
                        else {
                            PutSprite( l_SPR_HwatuSelect, l_CardPos[nCardNo].Pos.x + 10, l_CardPos[nCardNo].Pos.y, 0 );
                        }
					}
				}
			}

			else
			{
				if ((l_CardPackPos.x != l_CardPos[nCardNo].Pos.x)  && 
					(l_CardPackPos.y != l_CardPos[nCardNo].Pos.y))
				{

					if ( g_nBombCardNo[0] == nCardNo ||  //��ź�϶��� ��ź ����ִ°��� Ȯ���̹����� �Ѹ���. �Ʒ��� �ߺ��� �Ǳ� ������ else if
						 g_nBombCardNo[1] == nCardNo ||  //���õȰź��� ���� ī�带 ���� ���� ���ؿ��� �־���.
						 g_nBombCardNo[2] == nCardNo ) 
					{
						PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo, TRUE); // bsw 01.05
						continue;
					}
					else if ( g_SelectCardNo == nCardNo && g_bBomb == false ) {	//���õ� ī���϶��� Ȯ���̹����� �Ѹ���.
						nTempCardNo = nCardNo;
						
						if ( i == (g_pGoStop->m_pFloor.m_pGamer[k].m_nCardCnt - 1 ) ) {
							PutHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo, TRUE); // bsw 01.05
						}
						continue;
					}
					else {
#ifdef SHOWUSERCARD
						TRACE("\n Target nCardNo : %d", nCardNo);
						
						// Edited by shkim
						PutSmallHwatuImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, nCardNo ); // bsw 01.05
                        // ���� ī�尡 ���̰� ����ؼ� ���
                        /*l_SPR_Hwatu->SetFrame( nCardNo );
                        l_SPR_Hwatu->SetPos(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y);
	                    l_SPR_Hwatu->SetStretchWidth( 20 );
                        l_SPR_Hwatu->SetStretchHeight( 30 );
                        l_SPR_Hwatu->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANSSTRETCHED );*/
						
#else
                        // ���� ī�� �޸� ���
						PutHwatuBackImage(l_CardPos[nCardNo].Pos.x, l_CardPos[nCardNo].Pos.y, true);
#endif
					}

					//���õ� ī�带 ���� �������� �ѷ��� �ؿ� ī�庸�� ���߿� ������.
					//���õ� ī�尡 ������ ī�尡 �ƴҶ��� �̺κ��� �̿��ؼ� ������.
					if ( i == (g_pGoStop->m_pFloor.m_pGamer[k].m_nCardCnt - 1 ) && nTempCardNo >= 0 ) {
						PutHwatuImage(l_CardPos[nTempCardNo].Pos.x, l_CardPos[nTempCardNo].Pos.y, nTempCardNo, TRUE); // bsw 01.05
						nTempCardNo = -1;
					}
				}
			}
		}
	}
}




// Edited by shkim
HRESULT CMainFrame::PutSmallHwatuImage(long nX, long nY, int nHwatuNo) // bsw 01.05
{    
    l_SPR_Hwatu->SetFrame( nHwatuNo );	
	l_SPR_Hwatu->SetStretchWidth( 20 );
    l_SPR_Hwatu->SetStretchHeight( 30 );
	l_SPR_Hwatu->SetPos( nX, nY );	
    l_SPR_Hwatu->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLKSTRETCHED );
    return 0;
}


//========================================================================
// x, y��ǥ�� ȭ�� �̹��� ��� no�� ���� ȭ�� ��ȣ, bScale�� Ȯ�� ����
//========================================================================

HRESULT CMainFrame::PutHwatuImage(long nX, long nY, int nHwatuNo, bool bScale, int nScaleMode) // bsw 01.05
{
    if(nHwatuNo >= 104 || nHwatuNo < 0) return E_FAIL;  // ��ο� ī�尡 �߰��Ǿ���

    l_SPR_Hwatu->SetFrame( nHwatuNo );

	if ( bScale ) {
        // ũ�� Ȯ��
		if ( nScaleMode == 0 ) {
            // [zoom]
            (*this.*m_fpPutHwatuZoom)( m_sBigZoomTable, nX - 10, nY - 10, nHwatuNo );
		}

        // �۰� Ȯ��
		else {
            // [zoom]
            (*this.*m_fpPutHwatuZoom)( m_sSmallZoomTable, nX - 2, nY - 2, nHwatuNo );
		}
	}

    // ���� ũ���
	else {		
        l_SPR_Hwatu->SetPos( nX, nY );
		l_SPR_Hwatu->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
	}

    return 0;
}



void CMainFrame::ShowTurnLine(int nSlotNo)
{
    if ( nSlotNo >= 0 && nSlotNo < MAX_ROOM_IN ) {
        PutSprite( l_SPR_TurnLine[nSlotNo], m_sTurnLinePos[nSlotNo].x, m_sTurnLinePos[nSlotNo].y );
    }
}



void CMainFrame::DrawEvent(int nSlotNo, int nType)
{
    //nType = 0;
	// 0~9: �� ~ Stop
	// 10: �ʴ�
    // 11: û��
    // 12: ȫ��
    // 13: ����
    // 14: ���Ǿ�
    // 15: �� 
	// 16: ��
    // 17: ����
    // 18: ���Ǿ�
    // 19: ����
    // 20: �Ǿ�
    // 21:��ź
	// 22: ����
    // 23: ���Ǿ�
    // 24: ���Ǿ�
    // 25: �ﱤ
    // 26: �籤
    // 27: �ټ��Ǿ�

    if ( nSlotNo >= 0 && nSlotNo < MAX_ROOM_IN ) {
        PutSprite( l_SPR_EVENT, m_sEventPos[nSlotNo].x, m_sEventPos[nSlotNo].y, nType );
    }
}



//========================================================================
// Ÿ�̸� ���
//========================================================================

void CMainFrame::TimeEvent( int nSlotNo )
{
    if ( nSlotNo >= 0 && nSlotNo < MAX_ROOM_IN ) {
        PutSprite( l_SPR_Time, m_sTimerPos[nSlotNo].x, m_sTimerPos[nSlotNo].y, l_nSecPos );
    }
}



void CMainFrame::ShowDialog( BOOL bShowDlg, int nType, int nPercentage, int nMode )
{
	if(!bShowDlg) return;
	
	bool bProgress = true;

	l_GoBTN->SetButtonHide(true);
	l_StopBTN->SetButtonHide(true);
		
	if( nType == DLGTYPE_SHAKEQ ) {
		l_GYESBTN->SetPos( SHAKE_DLG_POS_X + 40, SHAKE_DLG_POS_Y + 140 );
		l_GNOBTN->SetPos( SHAKE_DLG_POS_X + 132, SHAKE_DLG_POS_Y + 140 );
	}
	else {
		l_GYESBTN->SetPos( SHAKE_DLG_POS_X + 40, SHAKE_DLG_POS_Y + 140 );
		l_GNOBTN->SetPos( SHAKE_DLG_POS_X + 132, SHAKE_DLG_POS_Y + 140 );
	}
	
	l_GYESBTN->SetButtonHide(true);
	l_GNOBTN->SetButtonHide(true);

	switch(nType)
	{
		case DLGTYPE_GOSTOP:    // �� �Ͻðڽ��ϱ�?
            // [alpha]
            DrawTransDialog( TDT_GOSTOP, DEF_DLG_POS_X, DEF_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );
            PutSprite( l_SPR_NotifyText, DEF_DLG_POS_X, DEF_DLG_POS_Y + 7, 4 );
			l_GoBTN->SetButtonHide(false);
			l_StopBTN->SetButtonHide(false);
			l_GoBTN->DrawButton();
			l_StopBTN->DrawButton();
			break;

		case DLGTYPE_PREGIDENT: // �����Դϴ�.
            // [alpha]
            DrawTransDialog( TDT_PREGIDENT, DEF_DLG_POS_X, DEF_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );
            PutSprite( l_SPR_NotifyText, DEF_DLG_POS_X, DEF_DLG_POS_Y + 7, 5 );
            l_nDlgMode = 1;	
			bProgress = false;
			break;
			
		case DLGTYPE_SHAKEQ:    // ���ðڽ��ϱ�?
            // [alpha]
            DrawTransDialog( TDT_SHAKEQ, SHAKE_DLG_POS_X, SHAKE_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );
            PutSprite( l_SPR_NotifyText, SHAKE_DLG_POS_X, SHAKE_DLG_POS_Y + 7, 0 );
			l_nDlgMode = 2;
			l_GYESBTN->SetButtonHide(false);
			l_GNOBTN->SetButtonHide(false);
			l_GYESBTN->DrawButton();
			l_GNOBTN->DrawButton();
			break;

		case DLGTYPE_SHAKE:     // �������ϴ�.
			l_nDlgMode = 5;
            // [alpha]
            DrawTransDialog( TDT_SHAKE, SHAKE_DLG_POS_X, SHAKE_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );
            PutSprite( l_SPR_NotifyText, SHAKE_DLG_POS_X, SHAKE_DLG_POS_Y + 7, 1 );
			bProgress = false;
			break;

		case DLGTYPE_GOSTOPINFO: // ���� ������...
            PutSprite( m_SPR_WaitDlg, DEF_WAIT_DLG_POS_X, DEF_WAIT_DLG_POS_Y, 2 );
			bProgress = false;
			break;

		case DLGTYPE_SSANGPICHO:	//������ ���Ƿ� ���ðڽ��ϱ�?
			l_nDlgMode = 4;
            // [alpha]
            DrawTransDialog( TDT_SSANGPICHO, DEF_DLG_POS_X, DEF_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );
            PutSprite( l_SPR_NotifyText, DEF_DLG_POS_X, DEF_DLG_POS_Y + 7, 2 );
			l_GYESBTN->SetButtonHide(false);
			l_GNOBTN->SetButtonHide(false);
			l_GYESBTN->DrawButton();
			l_GNOBTN->DrawButton();
			break;

		case DLGTYPE_TWOCARDCHO:	//�����ϼ���.(���߿� ��������?)
			l_nDlgMode = 3;
            // [alpha]
            DrawTransDialog( TDT_TWOCARDCHO, DEF_DLG_POS_X, DEF_DLG_POS_Y, TRANS_DLG_ALPHA, 0 );
            PutSprite( l_SPR_NotifyText, DEF_DLG_POS_X, DEF_DLG_POS_Y + 7, 3 );
			break;

		case DLGTYPE_WAITTWOCARDCHO:    // �и� �������Դϴ�.
            PutSprite( m_SPR_WaitDlg, DEF_WAIT_DLG_POS_X, DEF_WAIT_DLG_POS_Y, 3 );
			bProgress = false;
			break;
	}

	nMode = l_nDlgMode;
	if( nMode == 1) //����
	{
		// nCardNo? �� 0���� ������ �и� ���� �ʴ´�

		if(g_nCardNoArr[0] >= 0) PutHwatuImage( DEF_DLG_POS_X +  26, DEF_DLG_POS_Y + 64, g_nCardNoArr[0] );
		if(g_nCardNoArr[1] >= 0) PutHwatuImage( DEF_DLG_POS_X +  77, DEF_DLG_POS_Y + 64, g_nCardNoArr[1] );
		if(g_nCardNoArr[2] >= 0) PutHwatuImage( DEF_DLG_POS_X + 128, DEF_DLG_POS_Y + 64, g_nCardNoArr[2] );
		if(g_nCardNoArr[3] >= 0) PutHwatuImage( DEF_DLG_POS_X + 179, DEF_DLG_POS_Y + 64, g_nCardNoArr[3] );
	}
	else if ( nMode == 2 ) //���ðڽ��ϱ�?
	{
		if(g_nCardNoArr[0] >= 0) PutHwatuImage( DEF_DLG_POS_X +  51, DEF_DLG_POS_Y + 42, g_nCardNoArr[0] );
		if(g_nCardNoArr[1] >= 0) PutHwatuImage( DEF_DLG_POS_X + 102, DEF_DLG_POS_Y + 42, g_nCardNoArr[1] );
		if(g_nCardNoArr[2] >= 0) PutHwatuImage( DEF_DLG_POS_X + 153, DEF_DLG_POS_Y + 42, g_nCardNoArr[2] );

	}
	else if ( nMode == 3 ) //�����ϼ���.(���߿� ��������?)
	{
		if(g_nCardNoArr[0] >= 0) PutHwatuImage(DEF_DLG_POS_X +  67, DEF_DLG_POS_Y + 64, g_nCardNoArr[0]);
		if(g_nCardNoArr[1] >= 0) PutHwatuImage(DEF_DLG_POS_X + 138, DEF_DLG_POS_Y + 64, g_nCardNoArr[1]);
	}
	else if ( nMode == 4 ) //������ ���Ƿ� ���ðڽ��ϱ�?
	{
        PutHwatuImage( DEF_DLG_POS_X + 102, DEF_DLG_POS_Y + 42, 33 );
	}
	else if ( nMode == 5 ) //�������ϴ�.
	{
		if(g_nCardNoArr[0] >= 0) PutHwatuImage( DEF_DLG_POS_X +  51, DEF_DLG_POS_Y + 42, g_nCardNoArr[0] );
		if(g_nCardNoArr[1] >= 0) PutHwatuImage( DEF_DLG_POS_X + 102, DEF_DLG_POS_Y + 42, g_nCardNoArr[1] );
		if(g_nCardNoArr[2] >= 0) PutHwatuImage( DEF_DLG_POS_X + 153, DEF_DLG_POS_Y + 42, g_nCardNoArr[2] );
	}
    else {
        nMode = 0;
    }


	if ( bProgress ) {

        if ( nMode == 4 ) {         //������ ���Ƿ� ���ðڽ��ϱ�?
            PutSprite( l_SPR_DlgBar, DEF_DLG_POS_X + 45, DEF_DLG_POS_Y + 120, (int)(double)(10 * (nPercentage / 100.0)));
		}
        else if( nMode == 2 ) {     // ���ðڽ��ϱ�?
            PutSprite( l_SPR_DlgBar, DEF_DLG_POS_X + 45, DEF_DLG_POS_Y + 120, (int)(double)(10 * (nPercentage / 100.0)));
		}
        else if ( nMode == 3 ) {    // �����ϼ���.
            PutSprite( l_SPR_DlgBar, DEF_DLG_POS_X + 45, DEF_DLG_POS_Y + 153, (int)(double)(10 * (nPercentage / 100.0)));
        }
		else {                      // ��~ �Ͻðڽ��ϱ�?
            PutSprite( l_SPR_DlgBar, DEF_DLG_POS_X + 45, DEF_DLG_POS_Y + 66, (int)(double)(10 * (nPercentage / 100.0)));
		}
	}
}



//========================================================================
// ���� ���â ǥ��
//========================================================================

void CMainFrame::ShowResult()
{
    g_SelectCardNo = -1;

	char str[256];
	RECT rIcon;

	memset( &rIcon , 0x00 , sizeof(rIcon));

	char szTempId[MAX_ID_LENGTH];
	int nWinnerNo = g_pGoStop->m_pFloor.m_nWinerNo;

    //---------------------------------
    // ���̶� ������ ������ ���� ���
    //---------------------------------

	if ( g_pGoStop->m_bGameStop ) {

		// GameStop���� �ΰ����� �ִ�..���� ���̶� ĥ�� �����°Ͱ�
        // ���� ���̶� ĥ�� ������ ��� ���»���..
		// �� �ΰ��� ������ g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv ��
        // true�̸� ���� ���̶� ĥ�� �����°Ŵ�.

		if ( g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv == true ) {

			l_bShowResult = FALSE;
			g_pGoStop->m_bGameStop = false;
			l_nDialogResultWaitTime = 0;
			l_nDialogResultFrameCnt = 0;

			g_pGoStop->Stop();
			g_pClient->RoomOut();

			g_pGoStop->SetActionEvent();
			return;
		}

		l_nDialogResultWaitTime = 10;
		return ;
	} 
	
    //-------------------
    // ���â ��� �̹���
    //-------------------

    PutSprite( l_SPR_Result, RESULT_POS_X, RESULT_POS_Y );

    //------------------------------------------------------
    // �޽���
    // 0. "������ ���� ������ �����մϴ�."
    // 1. "������! �������� �� ���Դϴ�."  <---- �̰͸� ���
    //------------------------------------------------------

	if ( g_pGoStop->m_pFloor.m_nBaseMulti >= 2 || g_pGoStop->m_pFloor.m_nWinerNo < 0 ) {	//��������.
        PutSprite( l_SPR_ResultMessage, RESULT_POS_X + 7, RESULT_POS_Y + 108, 1 );
		l_ResultOKBTN->DrawButton();

		g_CurUser = -1;
		g_pGoStop->m_bPlaying = false;
		return;
	}

    //-------------------
    // �¸��� �г��� ǥ��
    //-------------------

    memset( szTempId, 0x00, sizeof(szTempId) );

    if ( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nGamerUserNo < 0 ) {
		if ( nWinnerNo == 1 )
			strcpy( szTempId, "Com1" );
		else
			strcpy( szTempId, "Com2" );
	}
    else {
        strcpy( szTempId, g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nGamerUserNo].m_sUserInfo.szNick_Name );
    }

	sprintf( str , "%s", szTempId );
    DrawText( str, RESULT_POS_X + 199 , RESULT_POS_Y + 40, RGB( 255, 255, 255 ), FW_BOLD , TA_CENTER );

    //----------------------
    // ��~�ؼ� ������� ���
    //----------------------

	if( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_bGo == TRUE ) {
        if (g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nGoCnt >= 3) {
			int tmpMul = 1;
			tmpMul *= (int)pow(2, g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nGoCnt - 2);

            DrawNumber( l_SPR_AffairNum, RESULT_POS_X + 34, RESULT_POS_Y + 176, tmpMul, MARK_MULTIPLY, TA_CENTER );
		}
	}

    //-------------------------------
    // ��� Ƚ���� ���� �ܹ����� ���
    //-------------------------------

	if( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_bShake == TRUE ) {
        int tmpMul = 1;
		tmpMul *= (int)pow(2, g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nShakeCnt );
        DrawNumber( l_SPR_AffairNum, RESULT_POS_X + 92, RESULT_POS_Y + 176, tmpMul, MARK_MULTIPLY, TA_CENTER );
	}

    //------------------------
    // �� �۵� ��� ( x2 ���)
    //------------------------

	if( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_bMmong == TRUE ) {
        DrawNumber( l_SPR_AffairNum, RESULT_POS_X + 150, RESULT_POS_Y + 176, 2, MARK_MULTIPLY, TA_CENTER );
	}	

    //----------
    // �̼� ���
    //----------

	if ( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_bMission ) {
        DrawNumber( l_SPR_AffairNum, RESULT_POS_X + 208, RESULT_POS_Y + 176, g_pGoStop->m_nMissionMultiply, MARK_MULTIPLY, TA_CENTER );
	}

    //------------------
    // �� �� ������ ���
    //------------------

	if( g_pGoStop->m_nBaseMulti >= 2 ) {
        DrawNumber( l_SPR_AffairNum, RESULT_POS_X + 266, RESULT_POS_Y + 176, g_pGoStop->m_nBaseMulti, MARK_MULTIPLY, TA_CENTER );
	}

    //------------------
	// ���� �� �Ӵ� ���
    //------------------
    
    DrawMoney( m_SPR_ResultNum[1], RESULT_POS_X + 248, RESULT_POS_Y + 110, g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_biWinMoney, 1 );

    //-------------------------
    // �̱� ������ �� ���� ���
    //-------------------------

    int nMutil = 1;

    if ( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_bMission ) {
        nMutil = g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nMulti * g_pGoStop->m_nMissionMultiply * g_pGoStop->m_nBaseMulti;
    }
    else {
        nMutil = g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nMulti * g_pGoStop->m_nBaseMulti;
    }

    DrawMoney( m_SPR_ResultNum[0], RESULT_POS_X + 248, RESULT_POS_Y + 67, nMutil * g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nEvalution, 0 );


    //-------------------------
    // �� ������ ������ ��´�.
    //-------------------------

    int nLoserSeq = 0;     // �� ���� ����

    int nLoseUserSlotNo = 0;

    for ( int i = 1; i < MAX_ROOM_IN; i++ ) {

        nLoseUserSlotNo = (nWinnerNo + i) % MAX_ROOM_IN;

        //--------------------
	    // �� ������ �̸� ���
        //--------------------

	    memset( szTempId, 0x00, sizeof(szTempId) );
    
	    //nUserListNo �� 0���� ������ ������ ���°Ŵ�.
	    if ( g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_nGamerUserNo < 0 ) {
            if ( nLoseUserSlotNo == 1 ) {
                strcpy( szTempId, "Com1" );
            }
            else if ( nLoseUserSlotNo == 2 ) {
                strcpy( szTempId, "Com2" );
            }
	    }
	    else {
			strcpy( szTempId, g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_nGamerUserNo].m_sUserInfo.szNick_Name );
	    }

	    sprintf(str, "%s", szTempId);
        DrawText( str, RESULT_POS_X + 199, RESULT_POS_Y + 221 + ( nLoserSeq * 112 ), RGB( 255, 255, 255 ), FW_BOLD, TA_CENTER );

        //----------------------
        // ����, �ǹ�, ���� ���
        //----------------------

        if( g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_bKwangbak == TRUE ) {   // ����
		    PutSprite( l_SPR_Baks, RESULT_POS_X + 112, RESULT_POS_Y + 252 + ( nLoserSeq * 112 ), 0 );
	    }

        if( g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_bPbak == TRUE ) {       // �ǹ�
		    PutSprite( l_SPR_Baks, RESULT_POS_X + 176, RESULT_POS_Y + 252 + ( nLoserSeq * 112 ), 1 );
	    }

        if( g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_bDokbak == TRUE ) {     // ����
		    PutSprite( l_SPR_Baks, RESULT_POS_X + 240, RESULT_POS_Y + 252 + ( nLoserSeq * 112 ), 2 );
	    }

        //-------------------------
        // �� ������ �ݾ� ���� ���
        //-------------------------

        if( g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_biLoseMoney > 0 ) {
            DrawMoney( m_SPR_ResultNum[1], RESULT_POS_X + 248, RESULT_POS_Y + 283 + ( nLoserSeq * 112 ), g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_biLoseMoney, -1 );
	    }
	    else {
		    DrawMoney( m_SPR_ResultNum[1], RESULT_POS_X + 248, RESULT_POS_Y + 283 + ( nLoserSeq * 112 ), g_pGoStop->m_pFloor.m_pGamer[nLoseUserSlotNo].m_biLoseMoney, 0 );
	    }
       
        // ���� ���� ��ġ
        nLoserSeq++;
    }

    //--------
    // OK ��ư
    //--------

	l_ResultOKBTN->DrawButton();

    //-----------------------------
	// ���â�� ������ �� ��´�.
    //
    // 1�� �̼�, 2�� ���, 3�� ����
    //-----------------------------

	int nDojangStyle = 0;

	if( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_bMission ) {
        if( g_pGoStop->m_nMissionType != DAEBAK ) {
            nDojangStyle = 1;
        }
		else {
			nDojangStyle = 2;
		}
	}
	else if( g_pGoStop->m_bPregident )
		nDojangStyle = 3;

	int nTime1 = 0, nTime2 = 0;
	nTime1 = GTC();
	
	if( nTime1 - nTime2 > 1000 ) {
		nTime2 = GTC();
		
		if( m_nDojangShow++ >= 30 )
			m_nDojangShow = 30;
	}	

	if( m_nDojangShow >= 30 )
		DrawResultDojang( nDojangStyle );
}



//========================================================================
// �ʴ��ϱ� ��ȭ���ڸ� �׸���.
//========================================================================

void CMainFrame::DrawInviteUsers( int InviteBox_XPos, int InviteBox_YPos )
{
	int nCheckNo;
    int nViewCnt;       // �� ȭ�鿡 ����� �׸�� ��꿡 ����ϴ� ī����
    int nBaseY;         // �������� ��½� ����ϴ� ���� Y ��ǥ
    int nIdx;           // ���� �ε���

    nBaseY = InviteBox_YPos + 84;           // ù���� ������ǥ ����
    nIdx = m_SCB_Invite->GetStartElem();    // ���� �������� ���� ���� �ε��� ����
    nViewCnt = 0;

    for( int i = 0; i < MAX_VIEW_CHODAE_USER; i++ ) {
        if( g_sGameWaitInfo[nIdx].nUserNo > 0 ) {          // ����Ÿ�� �׷����� �����Ѵ�.

            //------------------------------------
            // ������ ������ ǥ�� �̹����� �׸���.
            //------------------------------------

            if( sInviteCheck[nIdx].bCheck == TRUE ) {
                PutSprite( m_SPR_CheckBall, InviteBox_XPos + 29, nBaseY + 5, 1 );
		    }
            
            //------------
            // ���� ������
            //------------

			if( g_sGameWaitInfo[nIdx].cSex == '1' ) {       // 1 �̸� �����̹Ƿ� ���� �������� �׸���.
                PutSprite( l_SPR_InviteIcon, InviteBox_XPos + 189, nBaseY + 3, 0 );
			}
			else {                                          // 0 �̴ϱ� ���� �������� �׸���.
                PutSprite( l_SPR_InviteIcon, InviteBox_XPos + 189, nBaseY + 3, 1 );
			}

			nCheckNo = GetInviteCheck();                    // üũ�� ��ȣ�� �޴´�.

            //-------------------
            // �г��� & ���� �Ӵ�
            //-------------------
			
			if( strlen( g_sGameWaitInfo[nIdx].szNick_Name ) > 14 ) {
				memset( &m_szTempText, 0x00, sizeof( m_szTempText ));
				memcpy( m_szTempText, g_sGameWaitInfo[nIdx].szNick_Name, 14 );
				strcat( m_szTempText, "..." );
			}
			else {
				strcpy( m_szTempText, g_sGameWaitInfo[nIdx].szNick_Name );
			}

			if( nIdx == nCheckNo ) {    // �̳��� üũ�� ��ȣ�� �ٸ� ������ �׸���.
                // �г���
                DrawText( m_szTempText, InviteBox_XPos + 48, nBaseY + 7, RGB( 140, 215, 69 ), FW_BOLD, TA_LEFT );

                // ���� �Ӵ�
                I64toA_Money( g_sGameWaitInfo[nIdx].biUserMoney, m_szTempText, 1 );
                DrawText( m_szTempText, InviteBox_XPos + 277, nBaseY + 7, RGB( 140, 215, 69 ), FW_BOLD, TA_RIGHT );
			}
			else {                      // �׷��� ������ �׳� ���� ������ ��´�.
                // �г���
                DrawText( m_szTempText, InviteBox_XPos + 48, nBaseY + 7, RGB( 255, 255, 255 ), FW_NORMAL, TA_LEFT );

                // ���� �Ӵ�
                I64toA_Money( g_sGameWaitInfo[nIdx].biUserMoney, m_szTempText, 1 );
                DrawText( m_szTempText, InviteBox_XPos + 277, nBaseY + 7, RGB( 255, 255, 255 ), FW_NORMAL, TA_RIGHT );
			}

            nBaseY += 27;               // ��¿� ���� ��ǥ�� ���� �ٷ� �̵�
            nIdx++;                     // ���� ���� ����

            nViewCnt++;
            if ( nViewCnt >= MAX_VIEW_CHODAE_USER ) {
                break;
            }
		}
	}
}



//========================================================================
// x, y��ǥ�� ȭ�� �޸� �̹��� ���.
//
// input:
//      x, y = ����� ��ǥ
//      bSmall = true:  ���� �̹����� �޸� ���
//               false: ���� �̹����� �޸� ���
//      nAlpha = ���İ�(������)
//========================================================================

void CMainFrame::PutHwatuBackImage(long x, long y, BOOL bSmall, int nAlpha)
{
    if (bSmall) {
        PutSprite( l_SPR_HwatuBackSmall, x, y );
    }
    else {
        PutSprite( l_SPR_Hwatu, x, y, 51 );
    }
}



//========================================================================
// ȭ�� ĸ��
//========================================================================

BOOL CMainFrame::ScreenCapture()
{
    char        szFileName[256];
    time_t      sTIME = time( NULL );
    struct tm  *tTIME = localtime( &sTIME );
    DWORD       dwCurrTime = GetTickCount();

    if( -1 == GetFileAttributes(".\\capture") ) {   // Capture ���丮�� ������ �����.
        CreateDirectory(".\\capture", NULL);
    }

    sprintf(szFileName, ".\\capture\\%04d%02d%02d_%ld.bmp", tTIME->tm_year + 1900, tTIME->tm_mon + 1, tTIME->tm_mday, dwCurrTime);
    HRESULT rval = g_pCDXScreen->GetBack()->SaveAsBMP(szFileName);

    if ( rval >= 0 ) {
        char szFullPath[512];
        sprintf(szFullPath, "%s%s�� ����Ǿ����ϴ�.", g_szProgPath, szFileName + 2);

        if (l_nCurrentMode == WAITROOM_MODE) {
            AddWaitChatText( szFullPath, RGB( 0, 0, 0 ));
        } else if (l_nCurrentMode == GAME_MODE) {
            AddGameChatText( szFullPath, RGB(255, 255, 255));
        }
        return TRUE;
    }
    else {
        return FALSE;
    }
}



void CMainFrame::CreateRoom()
{
	ProcessButtonEvent();
}



//========================================================================
// ���� ��ư�̳�, ���콺 Ŭ�� ó��(��ư Ŭ���� ��ư �̺�Ʈ ó��)
//========================================================================

void CMainFrame::ProcessButtonEvent()
{
	if( l_Message->GetbShowCheck() )
	{
		if( l_MessageOKBTN->status == DOWN ) {	
			l_MessageOKBTN->SetMouseLBDownStatus(FALSE);
			
			PlaySound(g_Sound[GS_BTNCLICK], false);

			l_Message->SetbShowCheck( FALSE );
			
			if ( l_Message->m_bCloseGame ) {
				g_bPaketThredOn = false;
				g_bUserCardRecvThreadOn = false;

				Sleep(1000);
			
                StopAllSound();
				DestroySound();

                ExitGame();
			}
		}
        return;
	}
    
	if ( g_bConnectingSpr ) return;

	////////// Ÿ��Ʋ ȭ�鿡�� ����ϴ� ��ư�� //////////////

    //------------
    // ������ ��ư
    //------------

	if(l_ExitBTN->status == DOWN )
	{
		l_ExitBTN->SetMouseLBDownStatus(FALSE);

		if( SendBtnTime( l_ExitBTN , GTC()) )
			OnExitBtnDown();

		if( l_bUserState )
			l_bUserState = FALSE;

		return;
	}

	if(l_Exit2BTN->status == DOWN ) {
		
		l_Exit2BTN->SetMouseLBDownStatus(FALSE);

		if( SendBtnTime( l_Exit2BTN , GTC()) )
			OnExitBtnDown();

		if( l_bUserState )
			l_bUserState = FALSE;
	
		return;
	}

    //------------------------------------
    // Ǯ��ũ�� ���/������ ��� ��ȯ ��ư
    //------------------------------------

	if(l_MaxBTN->status == DOWN || l_MaxBTN2->status == DOWN /*|| l_CH_FullBTN->status == DOWN*/ )
	{
		l_MaxBTN->SetMouseLBDownStatus(FALSE);
		l_MaxBTN2->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK], false);
		
		l_bFullScreen = !l_bFullScreen;
        ChangeVideoMode( l_bFullScreen );

		return;
	}

    //---------------
    // â �ּ�ȭ ��ư
    //---------------

	if ( l_MinBTN->status == DOWN ) {
		l_MinBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK], false);

		if ( l_bFullScreen == TRUE ) { //Ǯ��ũ���϶���.
			l_bFullScreen = !l_bFullScreen;
            ChangeVideoMode( l_bFullScreen );
		}

		::ShowWindow( g_hWnd, SW_MINIMIZE );
		return;
	}

    //--------------------------
    // �ɼ� â ��ư �� Ŭ�� ó��
    //--------------------------

	if ( l_bShowConfigBox )
	{
        // �ʴ�, �ӼӸ�, ȿ����, �����ޱ� �ɼ� ���콺 Ŭ�� ó��
		for( int i = 0 ; i < 4 ; i ++ ) {
			if( MouseIn( CONFIG_POS_X +  93, CONFIG_POS_Y + 54 + (i * 27),
                         CONFIG_POS_X + 202, CONFIG_POS_Y + 80 + (i * 27)))
            {
				l_ConfigBox->ChangetCheck( i, TRUE );
			}
			else if( MouseIn( CONFIG_POS_X + 203, CONFIG_POS_Y + 54 + (i * 27),
                              CONFIG_POS_X + 301, CONFIG_POS_Y + 80 + (i * 27)))
            {
				l_ConfigBox->ChangetCheck( i, FALSE );
			}
		}

        // ���� ���콺 Ŭ�� ó��
        
        if      ( MouseIn( CONFIG_POS_X +  93, CONFIG_POS_Y + 162, CONFIG_POS_X + 202, CONFIG_POS_Y + 183 )) { l_ConfigBox->ChangeVoice( 1 ); } // ����1
        else if ( MouseIn( CONFIG_POS_X + 203, CONFIG_POS_Y + 162, CONFIG_POS_X + 301, CONFIG_POS_Y + 183 )) { l_ConfigBox->ChangeVoice( 0 ); } // ����1
        else if ( MouseIn( CONFIG_POS_X +  93, CONFIG_POS_Y + 184, CONFIG_POS_X + 202, CONFIG_POS_Y + 205 )) { l_ConfigBox->ChangeVoice( 2 ); } // ����2
        else if ( MouseIn( CONFIG_POS_X + 203, CONFIG_POS_Y + 184, CONFIG_POS_X + 301, CONFIG_POS_Y + 205 )) { l_ConfigBox->ChangeVoice( 3 ); } // ����2

		if( l_ConfigOkBTN->status == DOWN ) {

			l_ConfigOkBTN->SetMouseLBDownStatus(FALSE);

			PlaySound(g_Sound[GS_BTNCLICK], false);
			
			//���������� ���Ͽ� ����Ѵ�.	

			//BOOL bInvite;         // �ʴ�     TRUE ��� 
			//BOOL bSecretMessage;  // �ӼӸ�   TRUE ���
			//BOOL bWaveSnd;        // ȿ����	TRUE ���
			//BOOL bMessageAgree;   // �޼���   TRUE ���		

			for( int i = 0; i < 4; i++ ) {
				g_sClientEnviro.bOption[i] = l_ConfigBox->bCheck[i];
			}

			g_CGameUserList.m_GameUserInfo[0].m_nVoice = l_ConfigBox->nVoice;

			_itoa( g_sClientEnviro.bOption[0], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","Invite",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.bOption[1], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","Eartalk",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.bOption[2], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","Sound",g_szTempStr,".\\config.ini");

			_itoa( g_sClientEnviro.bOption[3], g_szTempStr, sizeof(g_szTempStr) );
			WritePrivateProfileString("Environment","MemoRecv",g_szTempStr,".\\config.ini");
            
			_itoa( g_CGameUserList.m_GameUserInfo[0].m_nVoice, g_szTempStr, 255 );
			WritePrivateProfileString("Environment","Voice",g_szTempStr,".\\config.ini");

            // ���� ������ �ٲٸ� ���濡�� �ٲ� �������� ���� �� �ֵ��� �����Ѵ�.

			g_pClient->SetClientOption( g_sClientEnviro.bOption[0],
                                        g_sClientEnviro.bOption[1],
                                        g_sClientEnviro.bOption[3],
                                        g_CGameUserList.m_GameUserInfo[0].m_nVoice );

			l_bShowConfigBox = FALSE;
		}
        return;
	}

    //----------
    // �ɼ� ��ư
    //----------

	if( l_ConfigBTN->status == DOWN )
	{
		l_ConfigBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK], false);
		
		//DTRACE("���� ��ư ���ȴ�");
		l_bShowConfigBox = TRUE;
		return;
	}

	if( l_bUserState == TRUE )
	{
		if( l_UserStateBTN->status == DOWN )
		{
			l_UserStateBTN->SetMouseLBDownStatus(FALSE);

            if( strcmp( g_CGameUserList.m_GameUserInfo[MASTER_POS].m_sUserInfo.szNick_Name , l_szMessageRecieveUserNick ) ) {
				l_bShowMessageBox = TRUE;
                m_IME_Memo->ClearBuffer();
                m_IME_Memo->InputBegin( true );
			}
		}

		l_bUserState = FALSE;

		return;
	}

    //------------------------------
    // ���� ������ ��ư �� Ŭ�� ó��
    //------------------------------

	if( l_bShowMessageBox )
	{
        if ( MouseIn( l_nMBoxXPos + 22, l_nMBoxYPos + 181, l_nMBoxXPos + 300, l_nMBoxYPos + 198 ) ) {
            m_IME_Memo->InputBegin( true );     // ���� �Է� �κ� ������ Ȱ��ȭ
        }
        else {
            m_IME_Memo->InputBegin( false );    // �ٸ� �κ� ������ ��Ȱ��ȭ
        }

		if( SendBtnTime( l_MemoSendBTN , GTC()) )
		{
			if( l_MemoSendBTN->status == DOWN )
			{
				l_MemoSendBTN->SetMouseLBDownStatus( FALSE );

                char *pIMEText = m_IME_Memo->GetIMEText();

                if( strlen( pIMEText ) > 0 )
				{
					memset( &l_szMessageSendUserNick, 0x00, sizeof( l_szMessageSendUserNick ));
					memset( &l_szMessageData, 0x00, sizeof( l_szMessageData ));
					
					strcpy( l_szMessageSendUserNick, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name );
					strcpy( l_szMessageData, pIMEText );		
					
					g_pClient->MemoSend( l_szMessageSendUserNick, l_szMessageRecieveUserNick, l_szMessageData );
				}

                m_IME_Memo->InputEnd();
                m_IME_Memo->ClearBuffer();
                l_bShowMessageBox = FALSE;

				return;
			}
		}

		if( l_MemoCancelBTN->status == DOWN )
		{
            l_MemoCancelBTN->SetMouseLBDownStatus( FALSE );
            m_IME_Memo->InputEnd();
            m_IME_Memo->ClearBuffer();
			l_bShowMessageBox = FALSE;
		}
		
		return;
	}

    //------------------------------------
    // �ʴ��ϱ� ��ȭ���� ��ư �� Ŭ�� ó��
    //------------------------------------

	if( l_bShowInviteBox )
	{
        // � ������ �����ߴ��� �˻�
		InputInviteChecks( CHODAE_POS_X + 23, CHODAE_POS_Y + 84, 261, 27 );

		// �ʴ� �ڽ� ���� �ʴ�(Ȯ��)��/�� ������.

		if( l_InviteOKBTN->status == DOWN ) {
			l_InviteOKBTN->SetMouseLBDownStatus( FALSE );

			int nTempInviteUserNo;
			nTempInviteUserNo = GetInviteCheck();

			if( nTempInviteUserNo >= 0 ) {
				g_pClient->UserInvite( g_sGameWaitInfo[nTempInviteUserNo].nUserNo , "" );
			}

			if( l_bUserState )
				l_bUserState = FALSE;

			l_bShowInviteBox = FALSE;
			
			return;
		}
        
        // ��� ��ư ������ ��

        if ( l_InviteCancelBTN->status == DOWN ) {
            l_InviteCancelBTN->SetMouseLBDownStatus( FALSE );
            l_bUserState = FALSE;
            l_bShowInviteBox = FALSE;
        }

        // ��ũ�� �� ��ư Ŭ�� ó��

        m_SCB_Invite->MouseClick( l_mX, l_mY );
        return;
	}
    
    //------------------------
    // �ʴ��ϱ� ��ư Ŭ�� ó��
    //------------------------

	//���� �� �ʴ� ��ư�� ������.
	if( l_InviteBTN->status == DOWN ) {
		l_InviteBTN->SetMouseLBDownStatus( FALSE );
        m_SCB_Invite->StartPage();

		l_nWaitViewPageNo = 0;
		g_pClient->GameViewWait( l_nWaitViewPageNo );

		InitInviteChecks();		
		return;
	}

    //------------------------------------
    // �ʴ� �޾��� ���� ��ȭ���� ��ư ó��
    //------------------------------------

	if( l_bShowChodaeBox )
	{
		// �ʴ� "��" ��ư
		if( SendBtnTime( l_RecvChodaeOkBTN , GTC()) )
		{
			if( l_RecvChodaeOkBTN->status == DOWN )
			{
				l_RecvChodaeOkBTN->SetMouseLBDownStatus( FALSE );

				l_bShowChodaeBox = FALSE;
				if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
				
					//�ڽ��� ���� �ִ� �������� �����Ѵ�.
					if ( AvailableRoomIn( l_biInviteRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
					
						sRoomIn TempRoom;
						memset( &TempRoom , 0x00 , sizeof( TempRoom ));
						
						TempRoom.nRoomNo = l_nInviteRoomNo;			
						strcpy( TempRoom.szRoomPass , l_szInviteRoomPass );
						
						g_pClient->RoomIn( &TempRoom );
					}
					else {
						l_Message->PutMessage( "������ �����ӴϷ� �ش����", "�����ϽǼ� �����ϴ�.!!!", false );
					}
				}
				else {
					l_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 161" );
				}

				return;
			}			
		}		

		// �ʴ� "�ƴϿ�" ��ư
		if( l_RecvChodaeCancelBTN->status == DOWN ) {
			l_RecvChodaeCancelBTN->SetMouseLBDownStatus( FALSE );
			l_bShowChodaeBox = FALSE;
			return;
		}
        
        return;
	}

    //----------
    // ȭ�� ĸ��
    //----------

    if( l_CaptureBTN->status == DOWN ) {
		l_CaptureBTN->SetMouseLBDownStatus(FALSE);

		PlaySound(g_Sound[GS_BTNCLICK], false);

		ScreenCapture();
		return;
	}

	//���� �뿡���� ��ư ������

    //-------------------------
    // ���� ���� ��ư Ŭ�� ó��
    //-------------------------

	if(l_bShowStartBTN) {
		if(l_StartBTN->status == DOWN) {    // ���� ��ư Ŭ����
			l_StartBTN->SetMouseLBDownStatus(FALSE);
			OnBtnStart();
			return;
		}
	}

    //---------------------------------------
    // �� ����� ��ȭ������ ��ư �� Ŭ�� ó��
    //---------------------------------------

	if ( l_bShowMakeRoomDlg )
	{
        // ��й�ȣ �Է� üũ�ڽ� Ŭ���� ���
        if ( MouseIn( MAKEROOM_POS_X + 277,
                      MAKEROOM_POS_Y + 85,
                      MAKEROOM_POS_X + 277 + 16,
                      MAKEROOM_POS_Y + 85 + 16))
        {
            m_bPassRoomChk = !m_bPassRoomChk;

            if (m_bPassRoomChk) {
                m_IME_InputPass->ClearBuffer();
                m_IME_InputPass->InputBegin(true);      // Ȱ��ȭ(��Ŀ�� ����)
            }
            else {
                m_IME_InputPass->InputEnd();
            }
        }
        // ��й�ȣ �Է� �κ�
        else if ( MouseIn( MAKEROOM_POS_X + 94,
                           MAKEROOM_POS_Y + 84,
                           MAKEROOM_POS_X + 94 + 178,
                           MAKEROOM_POS_Y + 84 + 18 ))
        {
            if ( m_bPassRoomChk ) {
                m_IME_InputPass->InputBegin(true);
            }
        }
        else {
            m_IME_InputPass->InputBegin(false);         // ��Ȱ��ȭ(��Ŀ�� ����)
        }

        // �� �ݾ� �κ� Ŭ����
		if( MouseIn( MAKEROOM_POS_X + 24 , MAKEROOM_POS_Y + 143,
                     MAKEROOM_POS_X + 299, MAKEROOM_POS_Y + 242 ))
        {
            if ( !m_bShowRoomTitleList ) {
                PlaySound(g_Sound[GS_BTNCLICK], false);
            }
		}

        // �� Ÿ��Ʋ �κ��� Ŭ������ ��
        if ( MouseIn( MAKEROOM_POS_X +  94, MAKEROOM_POS_Y + 59,
                      MAKEROOM_POS_X + 276, MAKEROOM_POS_Y + 77 ))
        {
            m_IME_InputRoomTitle->InputBegin(true);     // Ȱ��ȭ
        }
        else {
            m_IME_InputRoomTitle->InputBegin(false);    // ��Ȱ��ȭ
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

		if(l_MakeRoomOkBTN->status == DOWN) {       // Ȯ�� ��ư Ŭ����
			l_MakeRoomOkBTN->SetMouseLBDownStatus(FALSE);
			PlaySound(g_Sound[GS_BTNCLICK], false);
            
            //��й� ����.
            if ( m_bPassRoomChk ) {
                memset( m_szPass, 0x00, sizeof(m_szPass) );
                strcpy( m_szPass, m_IME_InputPass->GetIMEText());

                if ( m_szPass[0] != NULL ) {
			        MakeRoom(true);
                }
                else {
                    //�޼��� �ڽ�.
                    m_IME_InputPass->InputEnd();
                    l_Message->PutMessage( "��й�ȣ�� �Է��ϼ���.", "Code - 600" );
                    return;
                }
            }
            else {
                MakeRoom(false);
            }

			return;
		}

		if(l_MakeRoomCancelBTN->status == DOWN) {   // ��� ��ư Ŭ����
			l_MakeRoomCancelBTN->SetMouseLBDownStatus(FALSE);
			PlaySound(g_Sound[GS_BTNCLICK], false);

            m_IME_InputPass->InputEnd();
            m_IME_InputPass->ClearBuffer();

            m_IME_InputRoomTitle->InputEnd();
            m_IME_InputRoomTitle->ClearBuffer();
            
            ::SendMessage( g_hWnd, WM_WAITMODESET, 0, 0 );
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

        if ( m_PassDlgOkBTN->status == DOWN ) {
            m_PassDlgOkBTN->SetMouseLBDownStatus(FALSE);
            PlaySound(g_Sound[GS_BTNCLICK]);

            sRoomIn l_sRoomInData; // ������ ���� ����ü
			memset( &l_sRoomInData, 0x00, sizeof(l_sRoomInData) );

            l_sRoomInData.nRoomNo = g_CGameWaitRoomList.m_sRoomStateInfo[l_nViewRoomMessage].l_sRoomInfo.nRoomNo;
            strcpy( l_sRoomInData.szRoomPass, m_IME_RoomInPassBox->GetIMEText() );

            m_bPassDlg = FALSE;
            m_IME_RoomInPassBox->InputEnd();
            m_IME_RoomInPassBox->ClearBuffer();

            if ( l_sRoomInData.szRoomPass[0] != NULL) {
                l_bUserState = FALSE;
			    g_pClient->RoomIn( &l_sRoomInData ); // ������ ����.
            }
            else {
                l_Message->PutMessage("��й�ȣ�� �Է��ϼ���.", "Code - 600");
            }

            m_bShowRoomTitleList = FALSE;
            m_nSelectedRoomTitleList = -1;
            return;
        }

        if ( m_PassDlgCancelBTN->status == DOWN ) {
            m_PassDlgCancelBTN->SetMouseLBDownStatus(FALSE);
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
    
    //-----------------------------------------------------------------------
    // ���� �� �ߴ� ��ȭ����(��~����~ ����, �� ���� �ϳ� ���� ��)�� ��ư ó��
    //-----------------------------------------------------------------------

	if(l_bShowDialog)
	{
		if ( !l_GoBTN->m_bHide ) {
			if(l_GoBTN->status == DOWN)
			{
				l_GoBTN->SetMouseLBDownStatus(FALSE);
				
				PlaySound(g_Sound[GS_BTNCLICK], false);

				l_bShowDialog = FALSE;
				l_nDlgMode = 0;
				l_nWaitTime = 0;
				l_nDialogWaitFrameCnt = 0;

				g_pGoStop->m_nGoStop = 1;
				g_pGoStop->SetActionEvent();				
				
				return;
			}
		}

		if ( !l_StopBTN->m_bHide ) {
			if(l_StopBTN->status == DOWN)
			{
				l_StopBTN->SetMouseLBDownStatus(FALSE);

				PlaySound(g_Sound[GS_BTNCLICK], false);

				g_pGoStop->m_nGoStop = 0;

				l_bShowDialog = FALSE;
				l_nDlgMode = 0;
				l_nWaitTime = 0;
				l_nDialogWaitFrameCnt = 0;

				g_pGoStop->SetActionEvent();
				
				return;
			}
		}

		if ( !l_GYESBTN->m_bHide ) {
			if(l_GYESBTN->status == DOWN)
			{
				l_GYESBTN->SetMouseLBDownStatus(FALSE);

				PlaySound(g_Sound[GS_BTNCLICK], false);
				
				g_pGoStop->m_nNineCard = 1;
				g_pGoStop->m_nShake = 1;
				l_bShowDialog = FALSE;
				l_nDlgMode = 0;
				l_nWaitTime = 0;
				l_nDialogWaitFrameCnt = 0;

				g_pGoStop->SetActionEvent();

				return;
			}
		}

		if ( !l_GNOBTN->m_bHide ) {
			if(l_GNOBTN->status == DOWN)
			{
				l_GNOBTN->SetMouseLBDownStatus(FALSE);

				PlaySound(g_Sound[GS_BTNCLICK], false);
				
				g_pGoStop->m_nNineCard = 0;
				g_pGoStop->m_nShake = 0;

				l_bShowDialog = FALSE;
				l_nDlgMode = 0;
				l_nWaitTime = 0;
				l_nDialogWaitFrameCnt = 0;

				g_pGoStop->SetActionEvent();
				
				return;
			}
		}
        return;
	}

    //-------------------------
    // ���� ��� ȭ�� ��ư ó��
    //-------------------------

	if(l_bShowResult) {
		if( l_ResultOKBTN->status == DOWN) {
			l_ResultOKBTN->SetMouseLBDownStatus(FALSE);
			
			PlaySound(g_Sound[GS_BTNCLICK], false);

			l_bShowResult = FALSE;
			g_pGoStop->m_bGameStop = false;
			
			//DTRACE("Ȯ�ι�ư���� ���â �ݴ´�. ������ ��� %d", g_pGoStop->m_bMasterMode );
			//�ڽ��� �������̸� ��Ŷ�� ���ö����� ��ٸ��� �׷��� ������ ��Ŷ�� ������.
			OnReGames( g_pGoStop->m_bMasterMode, 0 );

			l_nDialogResultWaitTime = 0;
			l_nDialogResultFrameCnt = 0;
			return;
		}
        return;
	}
	
	///////// ���� ȭ�鿡�� ����ϴ� ��ư�� /////////////

    //--------------
    // ���� �����ϱ�
    //--------------

    int nJoinRoomNo = -1;

    // � ���� ���� ��ư�� ���ȴ��� �˻�

    for (int i = 0; i < 9; i++ ) {
        if ( m_JoinBTN[i]->status == DOWN ) {
            m_JoinBTN[i]->SetMouseLBDownStatus( FALSE );
            nJoinRoomNo = i;        // ������ �� ��ȣ ����
            break;
        }
    }
	
    if ( nJoinRoomNo >= 0 ) {
        // ���� �� ��ȣ ���
        nJoinRoomNo += m_SCB_WaitRoom->GetStartElem() * 3;

        if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {
			int iMaxMan = MAX_ROOM_IN;



			if ( g_CGameWaitRoomList.m_sRoomStateInfo[nJoinRoomNo].l_sRoomInfo.nCurCnt >= iMaxMan ) {

#ifdef ADMINJUSTVIEW
				g_pClient->RoomView( &l_sRoomInData );
#else
				l_Message->PutMessage( "�濡 �ο��� ��� á���ϴ�.", "Code - 126" );
				return;
#endif
			}

			//�ڽ��� ���� �ִ� �������� �����Ѵ�.
			if ( AvailableRoomIn( g_CGameWaitRoomList.m_sRoomStateInfo[nJoinRoomNo].l_sRoomInfo.biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) ) {
                if ( g_CGameWaitRoomList.m_sRoomStateInfo[nJoinRoomNo].l_sRoomInfo.bSecret ) {
                    m_IME_RoomInPassBox->ClearBuffer();
                    m_IME_RoomInPassBox->InputBegin(true);
                    m_bPassDlg = TRUE;
                }
                else {
                    l_sRoomInData.nRoomNo = g_CGameWaitRoomList.m_sRoomStateInfo[nJoinRoomNo].l_sRoomInfo.nRoomNo;
					memset( l_sRoomInData.szRoomPass , 0, sizeof(l_sRoomInData.szRoomPass));

					if( l_bUserState )
						l_bUserState = FALSE;
					
#ifdef ADMINJUSTVIEW
					if ( g_CGameWaitRoomList.m_sRoomStateInfo[nJoinRoomNo].l_sRoomInfo.nCurCnt >= iMaxMan ) {
						g_pClient->RoomView( &l_sRoomInData );
					} else {
						g_pClient->RoomIn( &l_sRoomInData ); // ������ ����.
					}
# else
					g_pClient->RoomIn( &l_sRoomInData ); // ������ ����.
#endif


#ifdef ADMINJUSTVIEW
					if ( g_CGameWaitRoomList.m_sRoomStateInfo[nJoinRoomNo].l_sRoomInfo.nCurCnt >= iMaxMan ) {
						l_nCurrentMode = GAMEVIEW_MODE;
						return;
					}
#endif

                }
			}
			else {
				l_Message->PutMessage( "������ �����ӴϷ� �ش����", "�����ϽǼ� �����ϴ�.!!!", false );
			}
		}
		else {
			l_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 133" );
		}
    }

	// �濡 ���� ������ �ѷ��ش�. PutTextRoomState
	if ( l_nCurrentMode == WAITROOM_MODE ) {
        //---------------
        // ��ũ�� �� ó��
        //---------------

        m_SCB_WaitChat->MouseClick( l_mX, l_mY );
        m_SCB_WaitRoom->MouseClick( l_mX, l_mY );
        
        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->MouseClick( l_mX, l_mY );
        }

        //-----------------------------------------
        // ����� �� ������ ����Ʈ Ÿ��Ʋ Ŭ�� ó��
        //-----------------------------------------

        if ( MouseIn(700, 165, 820, 195) ) {    // ������ ����Ʈ ���ٰ� ����� ����Ʈ Ÿ��Ʋ Ŭ����
            if ( m_nUserListTitleNum == 1 ) {
                if ( l_nViewWaitUser != 0 ) {
                    l_nViewWaitUser = 0;
                    AvatarDelete( -1 );
		            g_pClient->GetAvatarUrl( g_CUserList.m_pUserInfo[ l_nViewWaitUser ].m_sUser.nUserNo );		
                }

                m_nUserListTitleNum = 0;
                m_nHighlightBarNum = -1;
                l_nViewRoomUser = -1;
                l_bShowUserState = TRUE;
            }
            return;
        }

        if ( MouseIn(821, 165, 921, 195) ) {    // ����� ����Ʈ ���ٰ� �� ���� ����Ʈ Ÿ��Ʋ Ŭ����
            if ( m_nUserListTitleNum == 0 ) {
                m_nUserListTitleNum = 1;
                m_nHighlightBarNum = -1;
                l_nViewRoomUser = -1;
                l_bShowUserState = FALSE;
            }
            return;
        }

        //------------------------------------
        // ���� ä��â �Է¿� IME FOCUS ó��
        //------------------------------------

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



        //-------------------------
        // �� ����� ��ư Ŭ�� ó��
        //-------------------------

        int nMakeRoomNo = -1;

        // � ���� �游��� ��ư�� ���ȴ��� �˻�

        for (int i = 0; i < 9; i++ ) {
            if ( m_MakeRoomBTN[i]->status == DOWN ) {
                m_MakeRoomBTN[i]->SetMouseLBDownStatus( FALSE );
                nMakeRoomNo = i;        // ������ �� ��ȣ ����
                break;
            }
        }

        // ���� �游��� ��ư�� ���� ���
        if ( nMakeRoomNo < 0 ) {
            g_nMakeRoomNo = -1;
        }

        // ���� �游��� ��ư�� ������ �游��� ó��
        else {
            g_nMakeRoomNo = nMakeRoomNo + ( m_SCB_WaitRoom->GetStartElem() * 3 );    // ���� ���� ���ȣ ����

            m_bPassRoomChk = FALSE;             // ��й� üũ ����
            m_bShowRoomTitleList = FALSE;       // �� �̸� ����� �Ⱥ����ֵ��� �ʱ�ȭ
            m_nSelectedRoomTitleList = -1;      // ���õ� ���̸� ��� �ʱ�ȭ

			int nTemp = GetRandom( 0, 4 );

            m_IME_InputRoomTitle->SetIMEText( g_RoomTitle[nTemp] );
            m_IME_InputRoomTitle->InputBegin( false );

			if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney >= GAMEROOM_MIN1 ) {

				if( l_bUserState )
					l_bUserState = FALSE;
				
				//10���� ����Ų��.
				m_nMakeRoomMoneyNo = 0;

				//DTRACE("�游��� ���� �Ǿ���");		
				l_bShowMakeRoomDlg = TRUE;

                // ä��â�� �Է»��¸� �Է� �Ұ��� ���·� �����.
                m_IME_WaitChat->InputEnd();
			}
			else {
				l_Message->PutMessage( "������ �����ϰ� ������ �̿��ϼ���!!!", "Code - 132" );
			}
			
			return;
        }

        //------------
        // �� �����ϱ�
        //------------

        int nRoomNo = GetSelRoomIdx();

        if ( nRoomNo >= 0 ) {
            nRoomNo += m_SCB_WaitRoom->GetStartElem() * 3;

			if( g_CGameWaitRoomList.m_sRoomStateInfo[ nRoomNo ].l_sRoomInfo.nCurCnt > 0 ) {
                PlaySound(g_Sound[GS_BTNCLICK], false);

                if ( m_nUserListTitleNum == 1 && l_nViewRoomMessage != nRoomNo ) {
                    l_nViewRoomUser = -1;

                    if ( l_nViewWaitUser != 0 ) {
                        l_nViewWaitUser = 0;
                        AvatarDelete( -1 );
                        g_pClient->GetAvatarUrl( g_CUserList.m_pUserInfo[ l_nViewWaitUser ].m_sUser.nUserNo );
                    }
                }

                l_nViewRoomMessage = nRoomNo;
                g_pClient->GetRoomInfo( l_nViewRoomMessage );
			}
            else {
                l_nViewRoomMessage = -1;

                memset( g_ClickRoomUserInfo, 0x00, sizeof( g_ClickRoomUserInfo ));

				// �⺻ ���� �ƹ�Ÿ�� ������ ����Ѵ�.
				if ( m_nUserListTitleNum == 1 ) {
					if ( g_CUserList.m_pUserInfo[ l_nViewWaitUser ].m_sUser.nUserNo > 0 ) {
                        if ( l_nViewWaitUser != 0 ) {
                            l_nViewWaitUser = 0;
                            AvatarDelete( -1 );
		                    g_pClient->GetAvatarUrl( g_CUserList.m_pUserInfo[ l_nViewWaitUser ].m_sUser.nUserNo );		
                        }
                    }

                    l_nViewRoomUser = -1;
                }
            }
			
			return;
        }

        //---------------------------------
        // ���� ����Ʈ�� ���� �����ϱ� ó��
        //---------------------------------

        m_nHighlightBarNum = GetSelUserIdx();

        int nWaitUserNo = m_nHighlightBarNum;

        if( nWaitUserNo != -1 ) {
			PlaySound(g_Sound[GS_BTNCLICK], false);

			l_nWaitUserNo = nWaitUserNo;

            // ����� ����Ʈ
            
            if ( m_nUserListTitleNum == 0 ) {
                l_nWaitUserNo += m_SCB_WaitUser->GetStartElem();

                if ( l_nWaitUserNo >= 0 && l_nWaitUserNo < MAX_CHANNELPER ) {
				    if( g_CUserList.m_pUserInfo[ l_nWaitUserNo ].m_bUser == TRUE ) {
					    l_bShowUserState = TRUE;
					    l_nViewWaitUser = l_nWaitUserNo;
                        l_nViewRoomUser = -1;

					    AvatarDelete( -1 );
					    g_pClient->GetAvatarUrl( g_CUserList.m_pUserInfo[ l_nViewWaitUser ].m_sUser.nUserNo );		
				    }
			    }
            }

            // �������� ������ ����Ʈ

            else {
                if ( l_nWaitUserNo >= 0 && l_nWaitUserNo < MAX_ROOM_IN )
                {
                    if ( g_ClickRoomUserInfo[l_nWaitUserNo].nUserNo > 0 ) {
                        l_bShowUserState = FALSE;
					    l_nViewRoomUser = l_nWaitUserNo;
                        l_nViewWaitUser = -1;

                        AvatarDelete( -1 );
                        AvatarPlay( -1, g_ClickRoomUserInfo[l_nViewRoomUser].szPreAvatar, 0, g_ClickRoomUserInfo[l_nViewRoomUser].cSex );
				    }
			    }
            }

			return;
		}
	}

    if ( l_nCurrentMode == GAME_MODE ) {
        //---------------------------------
        // ���� ȭ�� ä��â IME ��Ŀ�� ó��
        //---------------------------------

        // ä��â �κ� Ŭ���ϸ� ä��â �Է»��·� �����.
        if ( MouseIn( 787, 538, 1021, 558 ) && !m_IME_GameChat->IsInputState() ) {
            m_SCB_GameChat->EndPage();          // �� �Ʒ� �������� �̵�
            m_IME_GameChat->InputBegin(true);   // �Է� ����
        }

        // ä��â �̿��� �κ��� Ŭ���ϸ� �Է� �Ұ��� ���·� �����.
        else if ( !MouseIn( 787, 403, 1021, 558 ) && m_IME_GameChat->IsInputState() ) {
            m_IME_GameChat->InputEnd();
            m_IME_GameChat->ClearBuffer();
        }

        //------------------------
        // ����ȭ�� ��ũ�� �� ó��
        //------------------------

        // ����ȭ�� ä��â ��ũ�ѹ� ���콺 Ŭ�� ó��
        m_SCB_GameChat->MouseClick( l_mX, l_mY );
    }
}



BOOL CMainFrame::SendBtnTime( CDXButton *Btn , int nTime , BOOL bWavePlay )
{
	int nTempTime;	

    if( bWavePlay ) {
        PlaySound( g_Sound[GS_BTNCLICK], false );
    }
	
	for( int i = 0 ; i < 50 ; i ++ ) {
		if( BtnTime[i].CDXBtn == Btn ) {
			nTempTime = nTime - BtnTime[i].nTime;
			
			if( DEF_TIME < nTempTime ) {
				BtnTime[i].nTime = nTime;
				return TRUE;
			}

			return FALSE;
		}		
	}

	for( i = 0; i < 50; i++ ) {
		if( BtnTime[i].CDXBtn == NULL ) {
			BtnTime[i].CDXBtn = Btn;
			BtnTime[i].nTime = nTime;
			return TRUE;
		}		
	}

    return FALSE;
}



void CMainFrame::OnExitBtnDown()
{
	char szTempStr2[255];
    
	if(l_nCurrentMode == GAME_MODE)		//������.
	{
		if ( g_pGoStop->m_pFloor.RealGame() ) {

			if ( g_pGoStop->m_bPlaying || m_bSunSetRender ) {
				g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv = !g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv;

				if ( g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv ) {
                    m_bShowExitReservIcon[0] = TRUE;
					sprintf( g_szTempStr, "%s��", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name );
					strcpy( szTempStr2, "�����⿹��" );
				}
				else {
                    m_bShowExitReservIcon[0] = FALSE;
					sprintf( g_szTempStr, "%s��", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name );
					strcpy( szTempStr2, "���������" );
				}
				
                AddGameChatText( g_szTempStr, RGB(255, 255, 0));
                AddGameChatText( szTempStr2, RGB(255, 255, 100));
				g_pClient->UserExitReserv( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.nUserNo, g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv );
			}
			else if ( !g_pGoStop->m_bPlaying ) {	//�÷����� �ƴҶ�...

				//������ ��������̸� ��Ŷ�� ������ �ʴ´�.
				if ( g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv == false )
					g_pClient->RoomOut();			

			}

		}
		else {	//���Ͷ� ĥ��...���ڱ� ������ �ϱ� ���ؼ� //g_lpGE->m_pFloor.m_pGamer[0].m_ExitReserv �� �ӽú����� ����Ͽ���.
			
			if ( g_pGoStop->m_bPlaying ) {
				
				g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv = true;
				g_pGoStop->m_bGameStop = true;

				g_pGoStop->SetActionEvent();
			}
			else {
				g_pClient->RoomOut();
			}

            m_IME_GameChat->ClearBuffer();
		}
	}
	else {
        StopAllSound();
		DestroySound();

        ExitGame();
	}
}



//========================================================================
// �ʴ��ϱ� â���� � ��(����)�� ���õǾ����� �˻��Ѵ�.
//========================================================================

void CMainFrame::InputInviteChecks( int X, int Y, int SizeX, int SizeY )
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
                        sInviteCheck[j].bCheck = FALSE;
                    }
				}

                if( l_bUserState ) {
                    l_bUserState = FALSE;
                }

                // Ŭ���� ��ġ�� ���¸� ������Ų��.
				sInviteCheck[i].bCheck = !sInviteCheck[i].bCheck;
			}		
		}
		else {
            if( l_bUserState ) {
                l_bUserState = FALSE;
            }
		}

        nLine++;
        if ( nLine >= MAX_VIEW_CHODAE_USER ) {
            break;
        }
	}
}



//========================================================================
// �ʴ� ��ȭ������ �� ĭ�� ���� Y ��ǥ ����
//========================================================================

void CMainFrame::InitInviteChecks( void )
{
	for( int i = 0 ; i < MAX_GAMEVIEWWAIT ; i++ ) {
		sInviteCheck[i].bCheck = FALSE;
	}
}



//========================================================================
// ���� ��ư ������ ���� ó��
//========================================================================

void CMainFrame::OnBtnStart()
{
	if ( m_bSunSet ) {
		m_bSunSetChoice     = FALSE;
		m_nSunSetRender     = 0;
		m_bSunSetRender     = FALSE;
		m_nSunRenderDecided = 0;
		m_bRenderSun        = FALSE;
		m_nMySunChioce      = -1;
		m_nOtherSunChioce   = -1;
		
		g_pClient->GameSunSet();
		l_nStartBtnCnt = 0;
		l_bShowStartBTN = FALSE;		
	}
	else {
		g_pGoStop->m_bMasterMode = true; 

		if( !g_pGoStop->Start() ) {
			//���������� ����ش�.
			l_Message->PutMessage( "���� ���� ����", "Code - 123" );
			l_nStartBtnCnt = 0;
			l_bShowStartBTN = FALSE;
			g_pClient->RoomOut();
		}
		else {
			l_nStartBtnCnt = 0;
			l_bShowStartBTN = FALSE;
		}
	}
}



//========================================================================
// �� �����
//
// input:
//      bSecret     = true  : ��й�
//                  = false : �Ϲݹ�
//========================================================================

void CMainFrame::MakeRoom( BOOL bSecret )
{	
	char szJunk[512];+
	memset( &szJunk , 0x00 , sizeof( szJunk ));
    strcpy( szJunk, m_IME_InputRoomTitle->GetIMEText() );

	if( strlen( szJunk ) > 0 ) {
		l_bShowMakeRoomDlg = FALSE;
		memset( &l_sCreateRoom, 0x00, sizeof(l_sCreateRoom) );

		if ( !bSecret ) {
			l_sCreateRoom.bSecret = false;			
		}
		else {
			l_sCreateRoom.bSecret = true;
            strcpy( l_sCreateRoom.szRoomPass, m_szPass );
		}

		strcpy( l_sCreateRoom.szRoomName, szJunk );		
		strcpy( l_szTempRoom, szJunk );

        m_IME_InputPass->InputEnd();
        m_IME_InputPass->ClearBuffer();

        m_IME_InputRoomTitle->InputEnd();
        m_IME_InputRoomTitle->ClearBuffer();

		l_sCreateRoom.nTotCnt = MAX_ROOM_IN;

		switch( m_nMakeRoomMoneyNo ) {
			case 0: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY1; break;
			case 1: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY2; break;
			case 2: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY3; break;
			case 3: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY4; break;
			case 4: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY5; break;
			case 5: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY6; break;
            case 6: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY7; break;
			case 7: l_sCreateRoom.biRoomMoney = GAMEROOM_MONEY8; break;
		}

		//���� �ӴϿ� ���� ���� ����� �ִ� ������ ��������.

		if ( AvailableRoomIn( l_sCreateRoom.biRoomMoney, g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney ) == FALSE ) {
			sprintf( l_szTempText, "������ ����Ĩ�� �ش����ذ� ���� �ʽ��ϴ�." );
			l_Message->PutMessage( l_szTempText, "�ٽ� Ȯ���� �ּ���!!!", false );		
			return;
		}

		g_biRoomMoney = l_sCreateRoom.biRoomMoney;

		memcpy( g_szRoomName, l_sCreateRoom.szRoomName, MAX_ROOM_NAME );
	
		if ( !g_pClient->CreateRoom(&l_sCreateRoom, g_nMakeRoomNo) ) {
			l_Message->PutMessage( "����� ���� ����", "Code - 124" );		
		}
	}
	else {
		l_Message->PutMessage( "�������� �Է��ϼ���.!!!", "Code - 125" );
	}
}



void CMainFrame::OnReGames( bool bMasterMode, int nSlotNo ) //���� ������ �ٽ��Ѵ�. ���â Ȯ���� ������ ���� ���´�.
{
	if ( nSlotNo == 0 ) {
		//DTRACE("CMainFrame::OnReGames - �ڽ��� ����������");
		g_pClient->ResultOk( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.nUserNo );
	} 

	if ( g_pGoStop->m_pFloor.RealGame() ) {
		g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bAlly = true;


		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

			if ( !g_pGoStop->m_pFloor.m_pGamer[i].m_bAlly ) {
				//DTRACE( "CMainFrame::OnReGames - ��� ally�� �ƴ϶� ���ϵȴ�. slot = %d", i );
				return;
			}
		}

		for ( i = 0; i < MAX_ROOM_IN; i++ ) {
			g_pGoStop->m_pFloor.m_pGamer[i].m_bAlly = false;
			//DTRACE( "CMainFrame::OnReGames - ��� ally�� false ��Ų��." );
		}
	}

	//DTRACE("CB_GAMEBREAK - �� ������ ��ٸ���. Ȯ�� ��ư�� ������, ���Ⱑ ���;� �ٽý��۵ȴ�. - (1)");

	g_pGoStop->SetActionEvent();

	//////////////////////////////////////////////////////////////////////////
	//yuno 2005.10.28
	//�α� ���Ͽ� ���� ����� ���������.
	if ( g_pGoStop->m_pFloor.RealGame() ) {
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			int nGamerUserNo = g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo;

			memcpy( g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nEatCard, g_pGoStop->m_pFloor.m_pGamer[i].m_nEatCard, sizeof(g_pGoStop->m_pFloor.m_pGamer[i].m_nEatCard) );
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nEvalution = g_pGoStop->m_pFloor.m_pGamer[i].m_nEvalution;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nShakeCnt = g_pGoStop->m_pFloor.m_pGamer[i].m_nShakeCnt;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nBbukCnt = g_pGoStop->m_pFloor.m_pGamer[i].m_nBbukCnt;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nGoCnt = g_pGoStop->m_pFloor.m_pGamer[i].m_nGoCnt;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bGodori = g_pGoStop->m_pFloor.m_pGamer[i].m_bGodori;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bHongdan = g_pGoStop->m_pFloor.m_pGamer[i].m_bHongdan;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bChongdan = g_pGoStop->m_pFloor.m_pGamer[i].m_bChongdan;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bGusa = g_pGoStop->m_pFloor.m_pGamer[i].m_bGusa;
            g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bMmong = g_pGoStop->m_pFloor.m_pGamer[i].m_bMmong;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bShake = g_pGoStop->m_pFloor.m_pGamer[i].m_bShake;
            g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bMission = g_pGoStop->m_pFloor.m_pGamer[i].m_bMission;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bPbak = g_pGoStop->m_pFloor.m_pGamer[i].m_bPbak;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bKwangbak = g_pGoStop->m_pFloor.m_pGamer[i].m_bKwangbak;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_bDokbak = g_pGoStop->m_pFloor.m_pGamer[i].m_bDokbak;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nMulti = g_pGoStop->m_pFloor.m_pGamer[i].m_nMulti;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory.m_nMissionMultiply = g_pGoStop->m_pFloor.m_pGamer[i].m_nMissionMultiply;
		}
	}
	//////////////////////////////////////////////////////////////////////////


	g_pGoStop->Stop();
	InitCardPostion();
	memset( l_nPVal, 0x00, sizeof(l_nPVal) );
    
	PlaySound(g_Sound[GS_GAMECLEAR], false);

    // ������ ���� �������� ���������� �����ϴ� ������ �ʱ�ȭ
    memset( g_pCMainFrame->m_bShowExitReservIcon, 0x00, sizeof( g_pCMainFrame->m_bShowExitReservIcon ));

    int i = 0;
	if ( g_pGoStop->m_pFloor.RealGame() ) {

		//������ ������ ����� �����Ѵ�. �ڽ��� �����.
		sGameEnd l_sGameEnd;
		memset( &l_sGameEnd, 0x00, sizeof(l_sGameEnd) );

		int nGamerUserNo = g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo;

		if ( g_pGoStop->m_pFloor.m_nBaseMulti >= 2  ) {
			l_sGameEnd.nWinner = 2;
			l_sGameEnd.biObtainMoney = 0;
			l_sGameEnd.biLoseMoney = 0;
			l_sGameEnd.biSystemMoney = g_biSystemMoney;
		}
		else {
			if ( g_pGoStop->m_pFloor.m_nMasterSlotNo == MASTER_POS ) {
				l_sGameEnd.nWinner = 1;
				l_sGameEnd.biObtainMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biObtainMoney;
				l_sGameEnd.biLoseMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biLoseMoney;
				l_sGameEnd.biSystemMoney = g_biSystemMoney;
			}
			else {
				l_sGameEnd.nWinner = 0;
				l_sGameEnd.biObtainMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biObtainMoney;
				l_sGameEnd.biLoseMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biLoseMoney;
				l_sGameEnd.biSystemMoney = 0;
			}
		}

		g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biObtainMoney = 0;
		g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biLoseMoney = 0;

		for ( i = 0; i < MAX_ROOM_IN - 1; i++ ) {
			memcpy( l_sGameEnd.szOtherNick_Name[i] , g_CGameUserList.m_GameUserInfo[i+1].m_sUserInfo.szNick_Name, MAX_NICK_LENGTH );
		}

        // ��ǰ���� ������ ������� �����Ƿ� ������ 0�� �ִ´�.
        l_sGameEnd.nPrizeNo = 0;
        l_sGameEnd.nSucceedCode = 0;

        memcpy( &l_sGameEnd.l_sGameEndHistory, &g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory, sizeof(g_CGameUserList.m_GameUserInfo[nGamerUserNo].l_sGameEndHistory) );
            
		g_pClient->GameEnd( &l_sGameEnd );		


		for ( i = 0; i < MAX_ROOM_IN; i++ ) {
			int nGamerUserNo = g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo;
		}

		EndOtherGameUser();	//�߰��� Ƣ�� �������� ������...ó���Ѵ�.
		memset( g_sGamingExitUser, 0x00, sizeof(g_sGamingExitUser) );

        // �ڽ��� ������...
		if ( g_pGoStop->m_pFloor.m_pGamer[0].m_ExitReserv )	{
			g_pClient->RoomOut();
			return;
		}
        // �ٸ� ����� ������...
		else if ( g_pGoStop->m_pFloor.m_pGamer[1].m_ExitReserv ) {
			return;
		}

		memset( g_sGamingExitUser, 0x00, sizeof(g_sGamingExitUser) );

		if ( g_pGoStop->m_bMasterMode == TRUE ) {
			g_pGoStop->GameCreate(g_nDoubleCardCnt, g_nThreeCardCnt, g_biRoomMoney);
			l_bShowStartBTN = TRUE;
		}
	}
	else {
		if ( g_nRoomCurCnt == 2 ) {	//���̶� ġ�µ� �ٸ������ ���°��.
			g_pGoStop->m_pFloor.m_RealMode = true;
		}

		if ( g_pGoStop->m_bMasterMode == TRUE ) {
			g_pGoStop->m_bMyKetInput = true;
			l_bShowStartBTN = TRUE;
			g_pGoStop->GameCreate(g_nDoubleCardCnt, g_nThreeCardCnt, g_biRoomMoney);
		}
	}
}



void CMainFrame::ButtonUpCheck() // ��ư �ö�°� üũ
{
	l_MaxBTN2->SetMouseLBDownStatus(FALSE);
	l_MaxBTN->SetMouseLBDownStatus(FALSE);
	l_MinBTN->SetMouseLBDownStatus(FALSE);	

	if( l_Message->GetbShowCheck() ) {
		l_MessageOKBTN->SetMouseLBDownStatus( FALSE );
		return;
	}

	if( l_bShowMakeRoomDlg ) {
		//��ư ���� ó��
        m_RoomTitleBTN->SetMouseLBDownStatus( FALSE );
		l_MakeRoomOkBTN->SetMouseLBDownStatus(FALSE);
		l_MakeRoomCancelBTN->SetMouseLBDownStatus(FALSE);
		return;
	}

    if( m_bPassDlg ) {
        m_PassDlgOkBTN->SetMouseLBDownStatus(FALSE);
        m_PassDlgCancelBTN->SetMouseLBDownStatus(FALSE);
        return;
    }

	if( l_bShowDialog ) {
		l_GoBTN->SetMouseLBDownStatus(FALSE);
		l_StopBTN->SetMouseLBDownStatus(FALSE);
		l_GYESBTN->SetMouseLBDownStatus(FALSE);
		l_GNOBTN->SetMouseLBDownStatus(FALSE);
		return;
	}

	if( l_bShowResult) {
		l_ResultOKBTN->SetMouseLBDownStatus( FALSE );
		return;
	}
	
	if( l_bShowConfigBox ) {
		l_ConfigOkBTN->SetMouseLBDownStatus(FALSE);
		return;
	}

	if( l_bShowMessageBox ) {
		l_MemoSendBTN->SetMouseLBDownStatus( FALSE );
		l_MemoCancelBTN->SetMouseLBDownStatus( FALSE );
		return;
	}

	if( l_bShowInviteBox ) {
        m_SCB_Invite->BTN_MouseUp();
		l_InviteOKBTN->SetMouseLBDownStatus( FALSE );
        l_InviteCancelBTN->SetMouseLBDownStatus( FALSE );
		l_UserStateBTN->SetMouseLBDownStatus( FALSE );
		return;
	}

	if( l_bShowChodaeBox ) {
		l_RecvChodaeCancelBTN->SetMouseLBDownStatus( FALSE );
		l_RecvChodaeOkBTN->SetMouseLBDownStatus( FALSE );
		return;
	}

	switch(l_nCurrentMode)
	{
	case CHANNEL_MODE:  // ä���̸�
		l_ConfigBTN->DisableButton();
		l_Exit2BTN->SetMouseLBDownStatus(FALSE);
		l_ExitBTN->SetMouseLBDownStatus(FALSE);	
        l_CaptureBTN->SetMouseLBDownStatus(FALSE);
		break;

	case WAITROOM_MODE: // �����̸�
		l_ConfigBTN->SetMouseLBDownStatus(FALSE);
		l_Exit2BTN->SetMouseLBDownStatus(FALSE);
		l_ExitBTN->SetMouseLBDownStatus(FALSE);	
		l_UserStateBTN->SetMouseLBDownStatus( FALSE );
        l_CaptureBTN->SetMouseLBDownStatus(FALSE);

        m_SCB_WaitChat->BTN_MouseUp();          // ä��â ��ũ�� ��
        m_SCB_WaitRoom->BTN_MouseUp();

        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->BTN_MouseUp();      // ����� �ý�Ʈ ��ũ�� ��
        }

        if ( m_MakeRoomBTN[0]->status != DISABLED ) m_MakeRoomBTN[0]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[1]->status != DISABLED ) m_MakeRoomBTN[1]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[2]->status != DISABLED ) m_MakeRoomBTN[2]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[3]->status != DISABLED ) m_MakeRoomBTN[3]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[4]->status != DISABLED ) m_MakeRoomBTN[4]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[5]->status != DISABLED ) m_MakeRoomBTN[5]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[6]->status != DISABLED ) m_MakeRoomBTN[6]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[7]->status != DISABLED ) m_MakeRoomBTN[7]->SetMouseLBDownStatus( FALSE );
        if ( m_MakeRoomBTN[8]->status != DISABLED ) m_MakeRoomBTN[8]->SetMouseLBDownStatus( FALSE );

        if ( m_JoinBTN[0]->status != DISABLED ) m_JoinBTN[0]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[1]->status != DISABLED ) m_JoinBTN[1]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[2]->status != DISABLED ) m_JoinBTN[2]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[3]->status != DISABLED ) m_JoinBTN[3]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[4]->status != DISABLED ) m_JoinBTN[4]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[5]->status != DISABLED ) m_JoinBTN[5]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[6]->status != DISABLED ) m_JoinBTN[6]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[7]->status != DISABLED ) m_JoinBTN[7]->SetMouseLBDownStatus( FALSE );
        if ( m_JoinBTN[8]->status != DISABLED ) m_JoinBTN[8]->SetMouseLBDownStatus( FALSE );

		break;

	case GAME_MODE: // ����ȭ���̸�
		if(l_bShowStartBTN) l_StartBTN->SetMouseLBDownStatus(FALSE);
		l_ConfigBTN->SetMouseLBDownStatus(FALSE);
		l_Exit2BTN->SetMouseLBDownStatus(FALSE);
		l_ExitBTN->SetMouseLBDownStatus(FALSE);
        l_CaptureBTN->SetMouseLBDownStatus(FALSE);
		
		if( g_nRoomCurCnt >= MAX_ROOM_IN )
			l_InviteBTN->DisableButton();		
		else
			l_InviteBTN->SetMouseLBDownStatus(FALSE);	
		
		l_UserStateBTN->SetMouseLBDownStatus( FALSE );
        m_SCB_GameChat->BTN_MouseUp();
		break;
	}
}

void CMainFrame::EndOtherGameUser()
{
	sEndGameOther l_EndGameOther[MAX_ROOM_IN];	
	int nGamerUserNo;
	int nUserSlot;
	bool bSend = false;

	memset( &l_EndGameOther, 0x00, sizeof(l_EndGameOther) );

	//�����߿� �������� �ְ� �ڽ��� �����̸�...
	if ( g_pGoStop->m_pFloor.m_nBbangJjangSlotNo == MASTER_POS ) {
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			if ( g_sGamingExitUser[i].nUserNo > 0 ) {
				nGamerUserNo =  g_sGamingExitUser[i].nGameUserNo;
				nUserSlot = g_pGoStop->m_pFloor.FindUser( nGamerUserNo );

				if ( nGamerUserNo < 0 ) continue; //���Ⱑ ���Դµ� �̰� -�̸� �ٸ��ʿ��� RoomOut�� ó���Ȼ��´�.
				
				bSend = true;

				memcpy( &l_EndGameOther[i].szNick_Name, g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.szNick_Name, sizeof(l_EndGameOther[i].szNick_Name) );

				if ( g_pGoStop->m_pFloor.m_nBaseMulti >= 2  ) {
					l_EndGameOther[i].l_sGameEnd.biObtainMoney = 0;
					l_EndGameOther[i].l_sGameEnd.biLoseMoney = 0;
					l_EndGameOther[i].l_sGameEnd.nWinner = 2;
					l_EndGameOther[i].l_sGameEnd.biSystemMoney = g_biSystemMoney;
				}
				else {
					if ( nUserSlot == g_pGoStop->m_pFloor.m_nMasterSlotNo ) {
						l_EndGameOther[i].l_sGameEnd.biObtainMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biObtainMoney;
						l_EndGameOther[i].l_sGameEnd.biLoseMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biLoseMoney;
						l_EndGameOther[i].l_sGameEnd.nWinner = 1;
						l_EndGameOther[i].l_sGameEnd.biSystemMoney = g_biSystemMoney;
					}
					else {	
						l_EndGameOther[i].l_sGameEnd.biObtainMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biObtainMoney;
						l_EndGameOther[i].l_sGameEnd.biLoseMoney = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biLoseMoney;
						l_EndGameOther[i].l_sGameEnd.nWinner = 2;
						l_EndGameOther[i].l_sGameEnd.biSystemMoney = 0;
					}
				}
				
				//��밡 �̰ܵ� ��ǰ �ڵ� �ȳִ´�. ��������.
				l_EndGameOther[i].l_sGameEnd.nPrizeNo = 0;
				l_EndGameOther[i].l_sGameEnd.nSucceedCode = 0;

				g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biObtainMoney = 0;
				g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_biLoseMoney = 0;


				int nTempCnt = 0;
				for ( int p = 0; p < MAX_ROOM_IN; p++ ) {

					//�ڽ��� ����.
					if ( p == nGamerUserNo ) continue;

					memcpy( l_EndGameOther[i].l_sGameEnd.szOtherNick_Name[nTempCnt] , g_CGameUserList.m_GameUserInfo[p].m_sUserInfo.szNick_Name, MAX_NICK_LENGTH );
					nTempCnt++;
				}
			}
		}

		if ( bSend ) {
			//������ Ŭ���̾�Ʈ ��� �����߿� ���� ������ ������ �����͸� �Ѵ�.
			g_pClient->GameOtherEnd( l_EndGameOther );
		}
	}
}

void CMainFrame::ButtonDownCheck(int mX, int mY) // ��ư�� ���������� Ȯ��
{
	if( l_bFullScreen ) 
		l_MaxBTN2->CheckMouseOver(mX, mY, TRUE);
	else 
		l_MaxBTN->CheckMouseOver(mX, mY, TRUE);

	l_MinBTN->CheckMouseOver(mX, mY, TRUE);
	
	if( l_Message->GetbShowCheck() ) {
		l_MessageOKBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}
	
	if( l_bShowMakeRoomDlg ) {
        m_RoomTitleBTN->CheckMouseOver(mX, mY, TRUE);
		l_MakeRoomOkBTN->CheckMouseOver(mX, mY, TRUE);
		l_MakeRoomCancelBTN->CheckMouseOver(mX, mY, TRUE);
		return;
	}

    if( m_bPassDlg) {
        m_PassDlgOkBTN->CheckMouseOver(mX, mY, TRUE);
		m_PassDlgCancelBTN->CheckMouseOver(mX, mY, TRUE);
		return;
    }

	if( l_bShowConfigBox ) {
		l_ConfigOkBTN->CheckMouseOver(mX, mY, TRUE);
		return;
	}

	if( l_bShowMessageBox ) {
		l_MemoSendBTN->CheckMouseOver( mX , mY , TRUE );
		l_MemoCancelBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}

	if( l_bShowInviteBox ) {
        m_SCB_Invite->BTN_MouseDown( mX, mY );
		l_InviteOKBTN->CheckMouseOver( mX , mY , TRUE );
        l_InviteCancelBTN->CheckMouseOver( mX , mY , TRUE );
		l_UserStateBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}	

	if( l_bShowChodaeBox ) {
		l_RecvChodaeOkBTN->CheckMouseOver( mX , mY , TRUE );
		l_RecvChodaeCancelBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}
	
	if(l_bShowDialog) {         //���⿡�� ����ϴ� ��ư���� �� false.
		l_GoBTN->CheckMouseOver(mX, mY, TRUE);
		l_StopBTN->CheckMouseOver(mX, mY, TRUE);
		l_GYESBTN->CheckMouseOver(mX, mY, TRUE);
		l_GNOBTN->CheckMouseOver(mX, mY, TRUE);
		return;
	}

	if(l_bShowResult) {
		l_ResultOKBTN->CheckMouseOver( mX , mY , TRUE );
		return;
	}

	switch(l_nCurrentMode)
	{
	case CHANNEL_MODE: // ä���̸�
		l_ConfigBTN->DisableButton();
		l_Exit2BTN->CheckMouseOver( mX , mY , TRUE );
		l_ExitBTN->CheckMouseOver(mX, mY, TRUE);
        l_CaptureBTN->CheckMouseOver(mX, mY, TRUE);
		break;

	case WAITROOM_MODE: // �����̸�
		l_ConfigBTN->CheckMouseOver( mX , mY , TRUE ); // ���� ��ư
		l_Exit2BTN->CheckMouseOver( mX , mY , TRUE );
		l_ExitBTN->CheckMouseOver(mX, mY, TRUE);
		l_UserStateBTN->CheckMouseOver( mX , mY , TRUE );
        l_CaptureBTN->CheckMouseOver(mX, mY, TRUE);

        m_SCB_WaitChat->BTN_MouseDown( mX, mY );        // ä��â ��ũ�� ��
        m_SCB_WaitRoom->BTN_MouseDown( mX, mY );
        
        if ( m_nUserListTitleNum == 0 ) {
            m_SCB_WaitUser->BTN_MouseDown( mX, mY );        // ����� ����Ʈ ��ũ�� ��
        }

        if ( m_MakeRoomBTN[0]->status != DISABLED ) 
			m_MakeRoomBTN[0]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[1]->status != DISABLED ) 
			m_MakeRoomBTN[1]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[2]->status != DISABLED ) 
			m_MakeRoomBTN[2]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[3]->status != DISABLED ) 
			m_MakeRoomBTN[3]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[4]->status != DISABLED ) 
			m_MakeRoomBTN[4]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[5]->status != DISABLED ) 
			m_MakeRoomBTN[5]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[6]->status != DISABLED ) m_MakeRoomBTN[6]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[7]->status != DISABLED ) m_MakeRoomBTN[7]->CheckMouseOver( mX, mY, TRUE );
        if ( m_MakeRoomBTN[8]->status != DISABLED ) m_MakeRoomBTN[8]->CheckMouseOver( mX, mY, TRUE );

        if ( m_JoinBTN[0]->status != DISABLED ) m_JoinBTN[0]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[1]->status != DISABLED ) m_JoinBTN[1]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[2]->status != DISABLED ) m_JoinBTN[2]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[3]->status != DISABLED ) m_JoinBTN[3]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[4]->status != DISABLED ) m_JoinBTN[4]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[5]->status != DISABLED ) m_JoinBTN[5]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[6]->status != DISABLED ) m_JoinBTN[6]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[7]->status != DISABLED ) m_JoinBTN[7]->CheckMouseOver( mX, mY, TRUE );
        if ( m_JoinBTN[8]->status != DISABLED ) m_JoinBTN[8]->CheckMouseOver( mX, mY, TRUE );
        
		break;

	case GAME_MODE: // ����ȭ���̸�

		if(l_bShowStartBTN) l_StartBTN->CheckMouseOver(mX, mY, TRUE);

		l_ConfigBTN->CheckMouseOver( mX , mY , TRUE ); // ���� ��ư
		l_Exit2BTN->CheckMouseOver( mX , mY , TRUE );
		l_ExitBTN->CheckMouseOver(mX, mY, TRUE);
        l_CaptureBTN->CheckMouseOver(mX, mY, TRUE);

		if( g_nRoomCurCnt >= MAX_ROOM_IN )
			l_InviteBTN->DisableButton();
		else
			l_InviteBTN->CheckMouseOver( mX , mY , TRUE );

		l_UserStateBTN->CheckMouseOver( mX , mY , TRUE );
        m_SCB_GameChat->BTN_MouseDown( mX, mY );
		break;
	}
}

void CMainFrame::CheckButtons(int mX, int mY) // ���콺 Ŀ���� ��ư���� �ִ��� üũ
{
	if(l_bFullScreen) l_MaxBTN2->CheckMouseOver(mX, mY);
	else l_MaxBTN->CheckMouseOver(mX, mY);

	l_MinBTN->CheckMouseOver(mX, mY);		

	if( l_Message->GetbShowCheck() )
	{
		l_MessageOKBTN->CheckMouseOver( mX , mY );
		return;
	}

	if(l_bShowMakeRoomDlg)
	{
		l_MakeRoomOkBTN->CheckMouseOver(mX, mY);
		l_MakeRoomCancelBTN->CheckMouseOver(mX, mY);
		return;
	}

    if( m_bPassDlg ) {
        m_PassDlgOkBTN->CheckMouseOver( mX, mY );
		m_PassDlgCancelBTN->CheckMouseOver( mX, mY );
		return;
    }
	
	if( l_bShowConfigBox )
	{
		l_ConfigOkBTN->CheckMouseOver( mX , mY );
		return;
	}

	if( l_bShowMessageBox )
	{
		l_MemoSendBTN->CheckMouseOver( mX , mY );
		l_MemoCancelBTN->CheckMouseOver( mX , mY );
		return;
	}

	if( l_bShowInviteBox )
	{
        m_SCB_Invite->Drag( mX, mY );
        m_SCB_Invite->BTN_MouseOver( mX, mY );
		l_InviteOKBTN->CheckMouseOver( mX, mY );
        l_InviteCancelBTN->CheckMouseOver( mX, mY );
		l_UserStateBTN->CheckMouseOver( mX, mY );
		return;
	}	

	if( l_bShowChodaeBox )
	{
		l_RecvChodaeOkBTN->CheckMouseOver( mX , mY );
		l_RecvChodaeCancelBTN->CheckMouseOver( mX , mY );
		return;
	}
	
	if(l_bShowDialog)
	{
		l_GoBTN->CheckMouseOver(mX, mY);
		l_StopBTN->CheckMouseOver(mX, mY);
		l_GYESBTN->CheckMouseOver(mX, mY);
		l_GNOBTN->CheckMouseOver(mX, mY);

		return;
	}
	
	if(l_bShowResult)
	{
		l_ResultOKBTN->CheckMouseOver( mX , mY );
		return;
	}

	switch(l_nCurrentMode)
	{
			
		case CHANNEL_MODE: // ä���̸�
			l_ConfigBTN->DisableButton();
			l_Exit2BTN->CheckMouseOver( mX , mY );
			l_ExitBTN->CheckMouseOver(mX, mY);
            l_CaptureBTN->CheckMouseOver(mX, mY);
			break;

		case WAITROOM_MODE: // �����̸�
			l_ConfigBTN->CheckMouseOver( mX , mY );
			l_Exit2BTN->CheckMouseOver( mX , mY );
			l_ExitBTN->CheckMouseOver(mX, mY);
			l_UserStateBTN->CheckMouseOver( mX , mY );
            l_CaptureBTN->CheckMouseOver(mX, mY);

            m_SCB_WaitChat->BTN_MouseOver( mX, mY );          // ä��â ��ũ�� ��
            m_SCB_WaitRoom->BTN_MouseOver( mX, mY );
            
            if ( m_nUserListTitleNum == 0) {
                m_SCB_WaitUser->BTN_MouseOver( mX, mY );          // ����� ����Ʈ ��ũ�� ��
            }

            if ( m_MakeRoomBTN[0]->status != DISABLED ) m_MakeRoomBTN[0]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[1]->status != DISABLED ) m_MakeRoomBTN[1]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[2]->status != DISABLED ) m_MakeRoomBTN[2]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[3]->status != DISABLED ) m_MakeRoomBTN[3]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[4]->status != DISABLED ) m_MakeRoomBTN[4]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[5]->status != DISABLED ) m_MakeRoomBTN[5]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[6]->status != DISABLED ) m_MakeRoomBTN[6]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[7]->status != DISABLED ) m_MakeRoomBTN[7]->CheckMouseOver( mX, mY );
            if ( m_MakeRoomBTN[8]->status != DISABLED ) m_MakeRoomBTN[8]->CheckMouseOver( mX, mY );

            if ( m_JoinBTN[0]->status != DISABLED ) m_JoinBTN[0]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[1]->status != DISABLED ) m_JoinBTN[1]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[2]->status != DISABLED ) m_JoinBTN[2]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[3]->status != DISABLED ) m_JoinBTN[3]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[4]->status != DISABLED ) m_JoinBTN[4]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[5]->status != DISABLED ) m_JoinBTN[5]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[6]->status != DISABLED ) m_JoinBTN[6]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[7]->status != DISABLED ) m_JoinBTN[7]->CheckMouseOver( mX, mY );
            if ( m_JoinBTN[8]->status != DISABLED ) m_JoinBTN[8]->CheckMouseOver( mX, mY );
            
			break;

		case GAME_MODE: // ����ȭ���̸�
			if(l_bShowStartBTN) l_StartBTN->CheckMouseOver(mX, mY);

			l_ConfigBTN->CheckMouseOver( mX , mY );
			l_Exit2BTN->CheckMouseOver( mX , mY );
			l_ExitBTN->CheckMouseOver(mX, mY);
            l_CaptureBTN->CheckMouseOver(mX, mY);

			if( g_nRoomCurCnt >= MAX_ROOM_IN )
				l_InviteBTN->DisableButton();
			else
				l_InviteBTN->CheckMouseOver( mX , mY );				

			l_UserStateBTN->CheckMouseOver( mX , mY );
            m_SCB_GameChat->Drag( mX, mY );
            m_SCB_GameChat->BTN_MouseOver( mX, mY );

			break;
	}
}


void CMainFrame::SortUserEatCard(int nSlotNo) // ������ ���� �� ��Ʈ
{
	int val = 0;
	int x = 0;
	int y = 0;
	int i = 0;
    int j = 0;
	int nCardNo;
	int nPee_1 = 0;
	int nPee_2 = 0;
	int nPee_3 = 0;
	int nPee_4 = 0;
	int nPee_5 = 0;
	
    // 10¥�� �� ó��
	for(i = 0; i < g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nEatCnt[1]; ++i) {
		nCardNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nEatCard[1][i];

		if(l_CardPos[nCardNo].bGetOneCardMode) continue; // 01.09
		
        x = l_EatCardPos[nSlotNo][1].x + (HWATU_PUT_DISTANCE * i);
		y = l_EatCardPos[nSlotNo][1].y;

		l_CardPos[nCardNo].NewPos.x = x;
		l_CardPos[nCardNo].NewPos.y = y;
		l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos = l_CardPos[nCardNo].NewPos;
		l_CardPos[nCardNo].type = EATCARD;
	}

	// �� �� ó��
	for(i = 0; i < g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nEatCnt[3]; ++i)
	{
		nCardNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nEatCard[3][i];

		if(l_CardPos[nCardNo].bGetOneCardMode) continue; // 01.09
		
		val += ::GetPeeVal(nCardNo); // �� ���� ���

		if ( val > 25 ) {
            x = l_EatCardPos[nSlotNo][3].x + (HWATU_PUT_DISTANCE * nPee_5);
			y = l_EatCardPos[nSlotNo][3].y;
			nPee_5++;
		}
		else if ( val > 20 ) {
            x = l_EatCardPos[nSlotNo][4].x + (HWATU_PUT_DISTANCE * nPee_4);
			y = l_EatCardPos[nSlotNo][4].y;
			nPee_4++;
		}
		else if ( val > 15 ) {
            x = l_EatCardPos[nSlotNo][5].x + (HWATU_PUT_DISTANCE * nPee_3);
			y = l_EatCardPos[nSlotNo][5].y;
			nPee_3++;
		}
		else if ( 10 < val && val <= 15 ) {
            x = l_EatCardPos[nSlotNo][6].x + (HWATU_PUT_DISTANCE * nPee_2);
			y = l_EatCardPos[nSlotNo][6].y;
			nPee_2++;
		}
		else if ( 5 < val && val <= 10 ) {
            x = l_EatCardPos[nSlotNo][7].x + (HWATU_PUT_DISTANCE * nPee_1);
			y = l_EatCardPos[nSlotNo][7].y;
			nPee_1++;
		}
		else {
            x = l_EatCardPos[nSlotNo][8].x + (HWATU_PUT_DISTANCE * i);
			y = l_EatCardPos[nSlotNo][8].y;
		}

		l_CardPos[nCardNo].NewPos.x = x;
		l_CardPos[nCardNo].NewPos.y = y;
		l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos = l_CardPos[nCardNo].NewPos;
		l_CardPos[nCardNo].type = EATCARD;
	}

	l_nPVal[nSlotNo] = val;
	l_nPX[nSlotNo]   = x;
	l_nPY[nSlotNo]   = y;
}



void CMainFrame::RepaintUserCard(int nUserNo) // �� ������ �� ��Ʈ�� �̹� �߰� ī�带 �ٽ� �׸���.
{
	POINT pos;

	int nSlotNo = (nUserNo + g_pGoStop->m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;

	for(int i = 0 ; i < g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nCardCnt; ++i)
	{
		int nCardNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bCard[i];

		pos.x = l_UserCardPos[nSlotNo][i].x;
		pos.y = l_UserCardPos[nSlotNo][i].y;
		
		if(nCardNo == 0xff) continue;

		l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos = l_CardPos[nCardNo].NewPos = pos;
			
		l_CardPos[nCardNo].type = USERCARD;
	}
}

void CMainFrame::AniEvent(int nSlotNo, int nType)
{

	if ( nType != 21 ) //��ź�� �ƴϸ�..
		return;


	switch(nType) {
		case 21:	//��ź�϶�..
			nType = 0;
			break;
	}

	int x, y;
	RECT srcrect;


	srcrect.left = (nType % MAX_ROOM_IN) * 109 + ( l_EventTime * 109 ) ;
	srcrect.right = srcrect.left + 109;
	srcrect.top = (nType / 2) * 86;
	srcrect.bottom = srcrect.top + 86;

	switch(nSlotNo)
	{
	case MASTER_POS:
		x = 197;
		y = 329;
		break;

	case 1:
		x = 460;
		y = 105;
		break;

	}

	
}

void CMainFrame::GetMouseOverCardNo() // 01.12
{
	for(int i = 0; i < 53; i++)
	{
		if(l_mX >= l_CardPos[i].Pos.x && l_mX <= l_CardPos[i].Pos.x + 10 &&
			l_mY >= l_CardPos[i].Pos.y && l_mY <= l_CardPos[i].Pos.y + 55)
		{
			if(l_CardPos[i].type != PACK && l_CardPos[i].type != USERCARD) // �ѿ� �ִ� ī��� ����
			{
				l_nMouseOverCardNo = i;

				return;
			}
		}
	}

	l_nMouseOverCardNo = -1;
}



// ����(�޸�) ��ȭ���ڸ� �׸���.
void CMainFrame::DrawMessageBox( int X, int Y )
{
	if( l_bShowMessageBox ) {
		char szTempNick[128];
        char szTempMsg[256];
		
        PutSprite( l_SPR_MessageBox, X, Y );
		l_MemoSendBTN->SetPos( X + 41, Y + 211 );
		l_MemoSendBTN->DrawButton();

		l_MemoCancelBTN->SetPos( X + 178, Y + 211 );
		l_MemoCancelBTN->DrawButton();

		if( l_bMessageSend ) {
			sprintf( szTempNick, "%s�Կ��� ������ �����Դϴ�." , l_szMessageRecieveUserNick );
		}
		else {
			sprintf( szTempNick, "%s���� ���� �����Դϴ�." , l_szMessageRecieveUserNick );
			
			// ���� ����� �� �޼����� �����ش�
			strcpy( szTempMsg, l_szGetMessageData );
            DrawText( szTempMsg, X + 28, Y + 94, RGB( 0, 0, 0 ), FW_NORMAL, TA_LEFT );
		}

        DrawText( szTempNick, X + 29, Y + 67, RGB( 208, 208, 208 ), FW_NORMAL, TA_LEFT );
        DrawText( szTempNick, X + 28, Y + 66, RGB(   0,   0,   0 ), FW_NORMAL, TA_LEFT );

        if ( m_IME_Memo->IsInputState() ) {
           g_pCDXScreen->GetBack()->FillRect( X + 24, Y + 183, X + 301, Y + 199, RGB(255, 255, 255) );
           m_IME_Memo->PutIMEText( X + 28, Y + 186, RGB( 0, 0, 0 ), RGB( 0, 0, 0 ) );
        }
	}
}



//========================================================================
// �޽��� ������(���� �ڽ�)
//========================================================================

void CMainFrame::DrawUserState( int X, int Y )
{
	if(l_bUserState) {
		int TempX = X - ( SCREEN_WIDTH - 100 );
		
		if (X > ( SCREEN_WIDTH - 100 )) {
			X = ( SCREEN_WIDTH - 100 ) - TempX;
		}

        PutSprite(l_SPR_UserState, X, Y);
		
		l_UserStateBTN->SetPos(X+2, Y+1);
		l_UserStateBTN->DrawButton();
		
        DrawText( "�޼��� ������", X+4, Y+4, RGB(0, 0, 0), FW_NORMAL, TA_LEFT );
	}
}



void CMainFrame::RButtonDownCheck( int MouseX , int MouseY )
{
	if( l_bShowMessageBox )
		return;

    int nSelUser = GetSelUserIdx();

    if ( nSelUser != -1 ) {
        l_nUserStatus[nSelUser] = DOWN;
        l_nRButtonWaitUserNo = nSelUser;
    }
    else {
        l_bUserState = FALSE;
    }
}



void CMainFrame::RButtonUpCheck( void )
{
	if( l_bShowMessageBox )
		return;

    int nSelUser = GetSelUserIdx();

    if ( nSelUser != -1 ) {
        l_nUserStatus[nSelUser] = UP;
    }
}



void CMainFrame::RButtonEvent( void )
{
    if( l_bShowMessageBox || m_nHighlightBarNum == -1 ) {
        return;
    }

    //-----------------------------------------------------
    // ������ ����� ����Ʈ�� �������� ����(�޸�)�� ������.
    //-----------------------------------------------------

    if ( m_nUserListTitleNum == 0 ) {
	    int nTempNo = GetSelUserIdx();      // Ŭ���� ����� ����Ʈ�� �ε����� ���Ѵ�.
	    
	    if( nTempNo >= 0 ) {
            // ����� ����Ʈ�� ��� ȭ�鿡 ���̴� ����Ʈ�� ������ġ�� �����ش�.
            if ( m_nUserListTitleNum == 0 ) {
                nTempNo += m_SCB_WaitUser->GetStartElem();
            }

		    if( g_CUserList.m_pUserInfo[nTempNo].m_bUser ) {
			    l_bShowUserState = TRUE;
			    l_nViewWaitUser  = nTempNo;
		    }

            int nTempScroll = l_nRButtonWaitUserNo;
        
            if ( m_nUserListTitleNum == 0 ) {
                nTempScroll += m_SCB_WaitUser->GetStartElem();
            }
		    
		    if( g_CUserList.m_pUserInfo[nTempScroll].m_bUser ) {
			    if( l_nUserStatus[l_nRButtonWaitUserNo] == DOWN ) {
				    // �޴� ������ �̸��� �̸� �����صд�.
                    strcpy( l_szMessageRecieveUserNick, g_CUserList.m_pUserInfo[nTempScroll].m_sUser.szNick_Name );

				    //�� �޼����� ������ ���̴�.
				    l_bMessageSend = TRUE;
				    l_bUserState = TRUE;
				    l_TempMX = l_mX;
				    l_TempMY = l_mY;
			    }			
		    }
		    else {
			    l_bUserState = FALSE;
		    }
        }
    }

    //------------------------------------------
    // ������ ���� �������� ����(�޸�)�� ������.
    //------------------------------------------

    else if ( m_nUserListTitleNum == 1 ) {
        if ( l_nViewRoomMessage != -1 ) {
            // �濡 ����� �ְ�, ����� �ִ� ��ġ�� �������� ���� ������ �����ش�.
            if ( g_CGameWaitRoomList.m_sRoomStateInfo[l_nViewRoomMessage].l_sRoomInfo.nCurCnt > 0 ) {
				if ( m_nHighlightBarNum >= 0 && m_nHighlightBarNum < g_CGameWaitRoomList.m_sRoomStateInfo[l_nViewRoomMessage].l_sRoomInfo.nCurCnt ) {
                    if( l_nUserStatus[l_nRButtonWaitUserNo] == DOWN ) {
				        // �޴� ������ �̸��� �̸� �����صд�.
				        strcpy( l_szMessageRecieveUserNick, g_ClickRoomUserInfo[ m_nHighlightBarNum ].szNick_Name );
				        l_bMessageSend = TRUE;  //�� �޼����� ������ ���̴�.
				        l_bUserState = TRUE;
				        l_TempMX = l_mX;
				        l_TempMY = l_mY;
			        }
                }
                else {
                    l_bUserState = FALSE;
                }
            }
        }
    }
}



void CMainFrame::RecieveMemo( char *szSendUserNick , char *szMessage )
{
	// �޼����� �� �ڽ��� ����. // �ڽ��� �̹� ����������� 
	// �޼����� ������ �ʴ´�.


	if( l_Message->GetbShowCheck() )
		l_Message->SetbShowCheck( FALSE );

	if( l_bShowMakeRoomDlg )
	{
		l_bShowMakeRoomDlg = FALSE;
        m_IME_InputPass->InputEnd();
        m_IME_InputRoomTitle->InputEnd();
	}

    if ( m_bPassDlg ) {
        m_bPassDlg = FALSE;
        m_IME_RoomInPassBox->InputEnd();
    }

    // ä�� �Է��� ������.
    if ( l_nCurrentMode == WAITROOM_MODE ) {
        m_IME_WaitChat->InputEnd();
    }
    else if ( l_nCurrentMode == GAME_MODE ) {
        m_IME_GameChat->InputEnd();
    }

	PlaySound( g_Sound[GS_POPUP] );

    m_IME_Memo->ClearBuffer();
    m_IME_Memo->InputBegin( true );

	l_bShowMessageBox = TRUE;
	l_bMessageSend = FALSE;                                 // �� �޼����� �޴°��̴�.
	strcpy( l_szMessageRecieveUserNick, szSendUserNick );   // ���� ����� �������� �޴´�.
	strcpy( l_szGetMessageData, szMessage );                // ���� ����� �� �޼����� �޴´�.
}


// ���� ���� ����ϴ� ������ ��ư
void CMainFrame::RButtonDownCheckInGame( int MouseX , int MouseY )
{
}



void CMainFrame::RButtonEventInGame( void )
{
}



void CMainFrame::RButtonUpCheckInGame( void )
{
}



//========================================================================
// ���ӿ� �ʴ�޾��� �� ��µǴ� ȭ��
//========================================================================

void CMainFrame::DrawInvite( int X, int Y )
{
	if( l_bShowChodaeBox )
	{
		char szTempText[MAX_CHATMSG]; // �ӽú��� ( �޼��� ���� )
		memset( szTempText, 0x00, sizeof( szTempText ));

        PutSprite( l_SPR_Invitation, X, Y );

		l_RecvChodaeOkBTN->SetPos( X + 41 , Y + 176 );      // "��" ��ư
		l_RecvChodaeOkBTN->DrawButton();

		l_RecvChodaeCancelBTN->SetPos( X + 178 , Y + 176 );    // "�ƴϿ�" ��ư
		l_RecvChodaeCancelBTN->DrawButton();

		sprintf( szTempText , "%d���濡�� �ʴ��ϼ̽��ϴ�." , l_nInviteRoomNo + 1 );
        DrawText( szTempText, X + 163 , Y + 92 , RGB( 255, 255, 255 ), FW_NORMAL , TA_CENTER );
        DrawText( "�ʴ뿡 ���Ͻðڽ��ϱ�?", X + 163 , Y + 120 , RGB( 255, 255, 255 ), FW_NORMAL , TA_CENTER );
	}
}



void CMainFrame::FuctionKeyDown( WPARAM wParam )
{
	int nChoiceCardNo = -1;
	BOOL bFunKeyDown = FALSE;
	
	if( l_nCurrentMode == GAME_MODE )
	{
		if ( g_CurUser == g_MyUser && l_bShowResult == false && l_bShowDialog == false &&
			 g_pGoStop->m_bMyKetInput == true ) 
		{
			switch( wParam )
			{
				case VK_F1:
					nChoiceCardNo = 0;
					//DTRACE("F1Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F2:
					nChoiceCardNo = 1;
					//DTRACE("F2Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F3:
					nChoiceCardNo = 2;
					//DTRACE("F3Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F4:
					nChoiceCardNo = 3;
					//DTRACE("F4Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F5:
					nChoiceCardNo = 4;
					//DTRACE("F5Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F6:
					nChoiceCardNo = 5;
					//DTRACE("F6Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F7:
					nChoiceCardNo = 6;
					//DTRACE("F7Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F8:
					nChoiceCardNo = 7;
					//DTRACE("F8Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F9:
					nChoiceCardNo = 8;
					//DTRACE("F9Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;

				case VK_F10:
					nChoiceCardNo = 9;
					//DTRACE("F10Ű ���ȴ�");
					bFunKeyDown = TRUE;
					break;
			}

			if ( bFunKeyDown ) {
				if( g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bCard[nChoiceCardNo] >= BOMB_CARDNO ) 
					return;

				g_bBomb = g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].UserBomb(nChoiceCardNo);						

				g_SelectCardNo = g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bCard[nChoiceCardNo];
				int nSelectCard = nChoiceCardNo;// | floorNo;

				g_pGoStop->m_pFloor.m_pGamer[MASTER_POS].m_bUserEvent = true;
				g_pGoStop->m_nSelectCard = nSelectCard;

				//������ Ű�� �ѹ����� �Ŀ��� �������ʰ� �Ǽ��� Ű�� �Ծ�� �Ѵ�.
				g_pGoStop->m_bMyKetInput = false;

				g_pGoStop->SetActionEvent();
			}
		}
	}
}



int CMainFrame::CheckFloorTwoPee(const int nCardNo)
{
	int nRetPos = 0xff;
	int cnt = 0;
	BYTE arrCnt[5];
	
	for(int i = 0 ; i < g_pGoStop->m_pFloor.m_nCardCnt ; ++i)
	{
		if( g_pGoStop->m_pFloor.m_strCard[i].nCardNo / 4 == nCardNo / 4 ) arrCnt[cnt++] = i;
	}
	
	switch(cnt)
	{
		case 1:
		case 3:
			nRetPos = arrCnt[0];
			break;
		case 2:
			{
				l_nDialgType = DLGTYPE_TWOCARDCHO;
				l_nWaitTime = 300;
				l_nDialogWaitFrameCnt = 0;			
				l_bShowDialog = TRUE;
                g_bRefreshTransDlg = true;

				g_nCardNoArr[0] = g_pGoStop->m_pFloor.m_strCard[arrCnt[0]].nCardNo;
				g_nCardNoArr[1] = g_pGoStop->m_pFloor.m_strCard[arrCnt[1]].nCardNo;
				g_nCardNoArr[2] = -1;
				g_nCardNoArr[3] = -1;

				g_TwoPeeCnt[0] = arrCnt[0];
				g_TwoPeeCnt[1] = arrCnt[1];

			}
	}
	return nRetPos;
}



bool CMainFrame::GetPath()
{
	char szBuffer[512];

	//yun ���� ����Ǵ� ��ü��ο� ���� ������ �����´�. c:\aaa\bbb.exe
	GetModuleFileName(::GetModuleHandle(NULL), szBuffer, sizeof(szBuffer));
	
	int path_len = strlen(szBuffer);
	int i;

	//yun ��θ� ã�Ƽ� �ڿ��� ���� �о�帰�� ������ ��� �ձ����� ��θ� ã�´�.
	// ������ ��ġ�� ���� c:\aaa\�� ���´�.
	for (i = path_len - 1; i >= 0; i-- ) {
		if (szBuffer[i] == '\\')  {
			szBuffer[i+1] = '\0';
			break;
		}
	}
	if (i < 0)
		return false;

	memcpy( g_ClientPath, szBuffer, 255 );
	return true;
}



BOOL CMainFrame::SendChatTime( int nTime )
{
	int nTempTime;

	nTempTime = nTime - nChatTime;

	if( DEF_TIME < nTempTime ) {
		nChatTime = nTime;
		return TRUE;
	}

	return FALSE;
}



BOOL CMainFrame::CheckPosition(CARDPOS *pos) // pos->NewPos�� pos->Pos�� �̵�
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



LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bMaxLine = FALSE;

	switch(message)
	{

		case WM_SYSKEYDOWN :
			if(wParam == VK_F4)
				break;

		case WM_ASYNC:
			switch(WSAGETSELECTEVENT(lParam))
			{		
				//���� ���� �޽���
				case FD_CONNECT:
					g_pClient->OnConnect( WSAGETSELECTERROR(lParam) );
					break;

				case FD_CLOSE:
					g_pClient->Destroy();
					l_Message->PutMessage( "�����Ϳ�������. �ٽ� �����Ͽ��ּ���", "Code - 136", true );
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
			ChannelAvatarPaly();
			break;

        case WM_WAITMODESET:
            g_pGoStop->WaitRoomModeSet();
            break;

        case WM_GAMEMODESET:
            g_pGoStop->GameModeSet();
            break;

		case WM_AVATARDEL:
			AvatarDelete(wParam);
			break;

        case WM_IME_COMPOSITION:
        case WM_IME_ENDCOMPOSITION:
            if ( l_nCurrentMode == WAITROOM_MODE ) {
                m_IME_WaitChat->OnImeComposition( g_hWnd, lParam );         // ���� ä��â �ѱ� �Է� ó��
                m_IME_InputRoomTitle->OnImeComposition( g_hWnd, lParam );   // �� ���� �ѱ� �Է� ó��
                m_IME_RoomInPassBox->OnImeComposition( g_hWnd, lParam );    // ��й�ȣ �ѱ� �Է� ó��
            }
            else if ( l_nCurrentMode == GAME_MODE ) {
                m_IME_GameChat->OnImeComposition( g_hWnd, lParam );         // ����ȭ�� ä��â �ѱ� �Է� ó��
            }

            m_IME_Memo->OnImeComposition( g_hWnd, lParam );                 // ���� �Է¿� IME �ѱ� �Է� ó��
            break;

		default:
			return CFrameWnd::DefWindowProc(message, wParam, lParam);
	}

	return 0;
}

void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if ( l_bShowMessageBox ) {
        m_IME_Memo->OnChar( nChar );
        return;
    }

    switch ( l_nCurrentMode ) {
        case WAITROOM_MODE:
            // CDXIME�� �Է°��ɻ��°�, ��Ŀ���� ���� ����
            // ���������� �Է� ó���� �Ѵ�.
            m_IME_WaitChat->OnChar( nChar );

            if (l_bShowMakeRoomDlg) {
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

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // ���� Ű�Է� ó��
    if ( l_bShowMessageBox ) {
        if ( nChar == VK_RETURN ) {
        }
        else {
            m_IME_Memo->OnKeyDown( nChar );
        }

        return;
    }

    switch ( l_nCurrentMode ) {
        case WAITROOM_MODE:
            {
                //---------------------
                // ����Ű�� �Էµ� ���
                //---------------------

                if ( nChar == VK_RETURN ) {
                    if ( !l_bShowMakeRoomDlg ) {
                        if ( m_IME_WaitChat->IsFocus() ) {
                            if ( g_pCMainFrame->SendChatTime( GTC() ) ) {
                                char *pIMEText;
                                pIMEText = m_IME_WaitChat->GetIMEText();
        
                                if ( strlen( pIMEText ) > 0 ) {     // �Է��� ���� ���� ���� �߰�
                                    g_pClient->WaitChat( g_CUserList.m_pUserInfo[0].m_sUser.szNick_Name, pIMEText, 0 );
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
                    if ( l_bShowMakeRoomDlg ) {
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
                            if( strlen( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name ) > 14 ) {
							    memcpy( szChatCopy, g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name, 14 );
							    strcat( szChatCopy, "..." );
							    strcat( szChatCopy, "���� ��: " );
						    }
                            else {
                                sprintf( szChatCopy, "%s���� ��: ", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name );
                            }
						     
						    strcat( szChatCopy , pIMEText );
						    
                            AddGameChatText( szChatCopy, RGB( 204, 255, 255 ));
						    
						    g_pClient->GameChat( g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[0].m_nGamerUserNo].m_sUserInfo.szNick_Name, szChatCopy , 0 );
                            m_IME_GameChat->ClearBuffer();
                        }
                    }

                    else {
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

LRESULT CMainFrame::OnUserCardPaint(WPARAM wParam, LPARAM lParam) // �ѿ��� �������� �� ������
{
	int nCardNo = wParam;
	l_CardPos[nCardNo].NewPos.x = l_UserCardPos[lParam][g_pGoStop->m_pFloor.m_pGamer[lParam].m_nCardCnt - 1].x;
	l_CardPos[nCardNo].NewPos.y = l_UserCardPos[lParam][g_pGoStop->m_pFloor.m_pGamer[lParam].m_nCardCnt - 1].y;
	l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
	l_CardPos[nCardNo].bSetAction = TRUE;
	l_CardPos[nCardNo].type = USERCARD;

	l_AniStack.AddStack(&l_CardPos[nCardNo]);
	return 0;
}


LRESULT CMainFrame::OnCardUp(WPARAM wParam, LPARAM lParam) // �� ���÷��� ������
{
	int nSlotNo = wParam;
	int nPosNo = lParam;
	int nCardNo;

	if ( nSlotNo < 0 ) {
		nCardNo = nPosNo;
	}
	else {
		nCardNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bCard[nPosNo];
	}

	l_nTempCardNo = nCardNo;

	return 0;
}

LRESULT CMainFrame::OnCardThrow(WPARAM wParam, LPARAM lParam) // �� ������ ������ �� �ٴ����� ������
{
	int nSlotNo = wParam;
	int nPosNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nChoiceUserCardPos;
	int nTargetPosNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nChoiceFloorCardPos;

	if ( nTargetPosNo == 0xff || nTargetPosNo > ( MAX_CARD_POS - 1 ) )
		nTargetPosNo = g_pGoStop->m_pFloor.GetEmptyPos();

	int nCardNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bCard[nPosNo];

	g_pGoStop->m_pFloor.AppendAdd( nTargetPosNo, nCardNo );
	g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bCard[nPosNo] = 0xff;
	
	l_CardPos[nCardNo].NewPos.x = g_pGoStop->m_pFloor.m_pos[nTargetPosNo].x + ((g_pGoStop->m_pFloor.m_nPosCnt[nTargetPosNo]-1) * 10);;
	l_CardPos[nCardNo].NewPos.y = g_pGoStop->m_pFloor.m_pos[nTargetPosNo].y;// + ((g_pGoStop->m_pFloor.m_nPosCnt[nTargetPosNo]-1) * 5);;
	l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
	l_CardPos[nCardNo].bSetAction = TRUE;
	l_CardPos[nCardNo].type = FLOOR;

	l_AniStack.AddStack(&l_CardPos[nCardNo]);

	return 0;
}

LRESULT CMainFrame::OnCardThrowFloor(WPARAM wParam, LPARAM lParam) // �ѿ��� �� �� �ٴ����� ������.( ������ ������ �ƴϸ� ��� )
{

	int nSlotNo = (wParam + g_pGoStop->m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;
	int nTargetPosNo = g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_nChoiceFloorCardPos;

	if ( nTargetPosNo == 0xff || nTargetPosNo > ( MAX_CARD_POS - 1 ) )
		nTargetPosNo = g_pGoStop->m_pFloor.GetEmptyPos();

	int nCardNo = lParam;

	g_pGoStop->m_pFloor.AppendAdd( nTargetPosNo, nCardNo );
	
	l_CardPos[nCardNo].NewPos.x = g_pGoStop->m_pFloor.m_pos[nTargetPosNo].x + ((g_pGoStop->m_pFloor.m_nPosCnt[nTargetPosNo]-1) * 10);;
	l_CardPos[nCardNo].NewPos.y = g_pGoStop->m_pFloor.m_pos[nTargetPosNo].y;
	l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
	l_CardPos[nCardNo].bSetAction = TRUE;
	l_CardPos[nCardNo].type = FLOOR;

	l_AniStack.AddStack(&l_CardPos[nCardNo]);
	
	return 0;
}



LRESULT CMainFrame::OnResultOk(WPARAM wParam, LPARAM lParam)
{
	int nGamerUserNo = g_CGameUserList.FindUser( wParam );
	int nUserSlot = g_pGoStop->m_pFloor.FindUser( nGamerUserNo );

	OnReGames( g_pGoStop->m_bMasterMode, nUserSlot );

	return 0;
}



LRESULT CMainFrame::OnGoStop(WPARAM wParam, LPARAM lParam)
{
	l_bShowDialog = FALSE;
	l_nDlgMode = 0;
	l_nWaitTime = 0;
	l_nDialogWaitFrameCnt = 0;
	g_pGoStop->m_nGoStop = wParam;

	g_pGoStop->SetActionEvent();	
	return 0;
}



LRESULT CMainFrame::OnGetOneCard(WPARAM wParam, LPARAM lParam)
{
	int nCardNo = g_pGoStop->m_pFloor.GetOneEachGamer( wParam, lParam, g_pGoStop->m_pFloor.m_pGamer[lParam].m_GetOtherUserCardCnt );

	POINT ePos;

	if ( nCardNo != 0xff )
	{
		int nCardPoint = ::GetPeeVal(nCardNo);

		g_pGoStop->m_pFloor.m_pGamer[lParam].m_GetOtherUserCardCnt -= nCardPoint;

        if ( g_pGoStop->m_pFloor.m_pGamer[lParam].m_GetOtherUserCardCnt < 0 ) {
            g_pGoStop->m_pFloor.m_pGamer[lParam].m_GetOtherUserCardCnt = 0;
        }

		int nCount = g_pGoStop->m_pFloor.m_pGamer[lParam].m_nEatCnt[4]; // 01.09

        ePos.x = l_EatCardPos[wParam][8 - ( nCount / 5 )  ].x + ( nCount - ( nCount / 5 * 5 ) - 1) * HWATU_PUT_DISTANCE;
		ePos.y = l_EatCardPos[wParam][8 - nCount / 5 ].y; // 01.09
		
		l_CardPos[nCardNo].NewPos = ePos;
		l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
		l_CardPos[nCardNo].bSetAction = TRUE;
		l_CardPos[nCardNo].type = EATCARD;
		l_CardPos[nCardNo].bGetOneCardMode = TRUE;

		l_AniStack.AddStack(&l_CardPos[nCardNo]);
	}
	else {
		g_pGoStop->m_pFloor.m_pGamer[lParam].m_GetOtherUserCardCnt = 0;
		g_pGoStop->SetPaintEvent();
	}

	return 0;
}



//========================================================================
// �ٴڿ��� ���� �� ���� �������� ������
//
// input:
//      wParam:     ī�� ����
//      lParam:     ���� ���� ��ȣ
//========================================================================

LRESULT CMainFrame::OnFloorEat(WPARAM wParam, LPARAM lParam)
{
    POINT ePos;
	
    int nCardNo = g_pGoStop->m_pFloor.FloorEat(wParam, lParam);
    int nCount = 0;

	if (g_pGoStopValue[nCardNo] & H_20)         // ���� ���
	{
        ePos.x = l_EatCardPos[lParam][0].x + ((g_pGoStop->m_pFloor.m_pGamer[lParam].m_nEatCnt[0]-1) * HWATU_PUT_DISTANCE );
		ePos.y = l_EatCardPos[lParam][0].y;
	}
	else if (g_pGoStopValue[nCardNo] & H_10)    // �׸��� ���
	{
        ePos.x = l_EatCardPos[lParam][1].x + ((g_pGoStop->m_pFloor.m_pGamer[lParam].m_nEatCnt[1]-1) * HWATU_PUT_DISTANCE );
		ePos.y = l_EatCardPos[lParam][1].y;
	}
	else if (g_pGoStopValue[nCardNo] & H_5)     // ���� ���
	{
        ePos.x = l_EatCardPos[lParam][2].x + ((g_pGoStop->m_pFloor.m_pGamer[lParam].m_nEatCnt[2]-1) * HWATU_PUT_DISTANCE );
		ePos.y = l_EatCardPos[lParam][2].y;
	}
	else // ���� ���
	{
		nCount = g_pGoStop->m_pFloor.m_pGamer[lParam].m_nEatCnt[4];

        ePos.x = l_EatCardPos[lParam][8 - nCount / 5].x + (nCount - (nCount / 5 * 5) - 1) * HWATU_PUT_DISTANCE;
		ePos.y = l_EatCardPos[lParam][8 - nCount / 5].y;
	}

	l_CardPos[nCardNo].NewPos = ePos;
	l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
	l_CardPos[nCardNo].bSetAction = TRUE;
	l_CardPos[nCardNo].type = EATCARD;

	l_AniStack.AddStack(&l_CardPos[nCardNo]);

	return 0;
}



LRESULT CMainFrame::OnNineCardMove(WPARAM wParam, LPARAM lParam)
{
	int nCardNo = 33; //9����.
	int x, y;

	x = l_EatCardPos[wParam][8].x;
	y = l_EatCardPos[wParam][8].y;

	l_CardPos[nCardNo].NewPos.x = x;
	l_CardPos[nCardNo].NewPos.y = y;
	l_CardPos[nCardNo].StartPos = l_CardPos[nCardNo].Pos;
	l_CardPos[nCardNo].bSetAction = true;
	l_CardPos[nCardNo].type = EATCARD;

	l_AniStack.AddStack(&l_CardPos[nCardNo]);
	return 0;
}



LRESULT CMainFrame::OnCBCommand(WPARAM wParam, LPARAM lParam)
{
	if(!g_pClient) return 0;

	return 0;
}



//���Ӱ������ ���̾�α� - 10�� (500)
//������� ���� - 6��(300)
//�������ϴٵ��� �˸� - 2��(100)
void CMainFrame::GameBreak()
{
	//���⿡�� ���� ������ ��ٸ���..
	l_nDialogResultFrameCnt = 0;
	l_nDialogResultWaitTime = 500;

	l_ConfigOkBTN->SetMouseLBDownStatus(FALSE);	//��򰡿��� �ٿ��� �� �ִ� �����ΰ�����...�׷��� �������ش�.
	
	g_SelectCardNo = -1;
	g_nGoCnt = 0;
	l_bShowResult = TRUE;
}



void CMainFrame::WaitGoInfo()
{
	l_nDialgType = DLGTYPE_GOSTOPINFO;
	l_nWaitTime = 0; // 0 �̸� ��� ��ٸ�
	l_nDialogWaitFrameCnt = 0;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



void CMainFrame::SelectTwoCard( int nCurUserNo, int nCardNo )
{
	if ( nCurUserNo == g_MyUser ) CheckFloorTwoPee(nCardNo);	
}



void CMainFrame::SelectGoStop()
{
	l_nDialgType = DLGTYPE_GOSTOP;
	l_nWaitTime = 300;
	l_nDialogWaitFrameCnt = 0;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



void CMainFrame::Effect( int nSlotNo, int nEventDivi, int nDelayTime )
{
	l_nDrawEventSlotNo = nSlotNo;
	l_nDrawEventType = nEventDivi;	
	l_nDrawEventWaitTime = nDelayTime;
	l_bDrawEvent = TRUE;
}



void CMainFrame::PregiDent( int nPregidentCardNo )
{
	memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );

	g_nCardNoArr[0] = nPregidentCardNo;
	g_nCardNoArr[1] = nPregidentCardNo + 1;
	g_nCardNoArr[2] = nPregidentCardNo + 2;
	g_nCardNoArr[3] = nPregidentCardNo + 3;

	l_nDialgType = DLGTYPE_PREGIDENT;
	l_nWaitTime = 100;
	l_nDialogWaitFrameCnt = 0;
	g_pGoStop->m_bPregident = TRUE;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



// �ѹ��� ȣ��Ǵ� �Լ�

void CMainFrame::ShowMission()
{
	l_bShowMission = TRUE;	
    
    //------------------------------------------------------
    // �̼� ī�� ������ �ʱ�ȭ �ϰ�, ���ο� ������ �����Ѵ�.
    //------------------------------------------------------

    memset( &g_sMissionCardInfo, 0x00, sizeof( g_sMissionCardInfo ));

    switch( g_pGoStop->m_nMissionType )
	{
		case KKWANG: // ��
			break;

		case JAN: // 1��
            g_sMissionCardInfo.nCardNo[0] = 0;
            g_sMissionCardInfo.nCardNo[1] = 1;
            g_sMissionCardInfo.nCardNo[2] = 2;
            g_sMissionCardInfo.nCardNo[3] = 3;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case FEB: // 2��
            g_sMissionCardInfo.nCardNo[0] = 4;
            g_sMissionCardInfo.nCardNo[1] = 5;
            g_sMissionCardInfo.nCardNo[2] = 6;
            g_sMissionCardInfo.nCardNo[3] = 7;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case MAR: // 3��
            g_sMissionCardInfo.nCardNo[0] = 8;
            g_sMissionCardInfo.nCardNo[1] = 9;
            g_sMissionCardInfo.nCardNo[2] = 10;
            g_sMissionCardInfo.nCardNo[3] = 11;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case APR: // 4��
            g_sMissionCardInfo.nCardNo[0] = 12;
            g_sMissionCardInfo.nCardNo[1] = 13;
            g_sMissionCardInfo.nCardNo[2] = 14;
            g_sMissionCardInfo.nCardNo[3] = 15;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case MAY: // 5��
            g_sMissionCardInfo.nCardNo[0] = 16;
            g_sMissionCardInfo.nCardNo[1] = 17;
            g_sMissionCardInfo.nCardNo[2] = 18;
            g_sMissionCardInfo.nCardNo[3] = 19;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case JUN: // 6��
            g_sMissionCardInfo.nCardNo[0] = 20;
            g_sMissionCardInfo.nCardNo[1] = 21;
            g_sMissionCardInfo.nCardNo[2] = 22;
            g_sMissionCardInfo.nCardNo[3] = 23;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case JUL: // 7��
            g_sMissionCardInfo.nCardNo[0] = 24;
            g_sMissionCardInfo.nCardNo[1] = 25;
            g_sMissionCardInfo.nCardNo[2] = 26;
            g_sMissionCardInfo.nCardNo[3] = 27;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case AUG: // 8��
            g_sMissionCardInfo.nCardNo[0] = 28;
            g_sMissionCardInfo.nCardNo[1] = 29;
            g_sMissionCardInfo.nCardNo[2] = 30;
            g_sMissionCardInfo.nCardNo[3] = 31;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case SEP: // 9��
            g_sMissionCardInfo.nCardNo[0] = 32;
            g_sMissionCardInfo.nCardNo[1] = 33;
            g_sMissionCardInfo.nCardNo[2] = 34;
            g_sMissionCardInfo.nCardNo[3] = 35;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case OCT: // 10��
            g_sMissionCardInfo.nCardNo[0] = 36;
            g_sMissionCardInfo.nCardNo[1] = 37;
            g_sMissionCardInfo.nCardNo[2] = 38;
            g_sMissionCardInfo.nCardNo[3] = 39;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case NOV: // 11��
            g_sMissionCardInfo.nCardNo[0] = 40;
            g_sMissionCardInfo.nCardNo[1] = 41;
            g_sMissionCardInfo.nCardNo[2] = 42;
            g_sMissionCardInfo.nCardNo[3] = 43;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case DEC: // 12��
            g_sMissionCardInfo.nCardNo[0] = 44;
            g_sMissionCardInfo.nCardNo[1] = 45;
            g_sMissionCardInfo.nCardNo[2] = 46;
            g_sMissionCardInfo.nCardNo[3] = 47;
            g_sMissionCardInfo.nCardCnt = 4;
			break;

		case OKWANG: // ����
            g_sMissionCardInfo.nCardNo[0] = 0;
            g_sMissionCardInfo.nCardNo[1] = 8;
            g_sMissionCardInfo.nCardNo[2] = 28;
            g_sMissionCardInfo.nCardNo[3] = 40;
            g_sMissionCardInfo.nCardNo[4] = 44;
            g_sMissionCardInfo.nCardCnt = 5;
			break;

		case DAEBAK: // ���
			break;

		case HONGDAN:
            g_sMissionCardInfo.nCardNo[0] = 1;
            g_sMissionCardInfo.nCardNo[1] = 4;
            g_sMissionCardInfo.nCardNo[2] = 9;
            g_sMissionCardInfo.nCardCnt = 3;
			break;

		case CHONGDAN:
            g_sMissionCardInfo.nCardNo[0] = 20;
            g_sMissionCardInfo.nCardNo[1] = 32;
            g_sMissionCardInfo.nCardNo[2] = 36;
            g_sMissionCardInfo.nCardCnt = 3;
			break;

		case CHODAN:
            g_sMissionCardInfo.nCardNo[0] = 12;
            g_sMissionCardInfo.nCardNo[1] = 16;
            g_sMissionCardInfo.nCardNo[2] = 24;
            g_sMissionCardInfo.nCardCnt = 3;
			break;

		case GODORI: 
            g_sMissionCardInfo.nCardNo[0] = 5;
            g_sMissionCardInfo.nCardNo[1] = 13;
            g_sMissionCardInfo.nCardNo[2] = 29;
            g_sMissionCardInfo.nCardCnt = 3;
			break;
	}

    //-------------------------------------------
    // �̼��� �߻��ϱ� ���� ���� ī�带 üũ�Ѵ�.
    //-------------------------------------------
    for ( int i = 0; i < g_sMissionCardInfo.nCardCnt; i++ ) {
        // j = UserSlot
        for ( int j = 0; j < MAX_ROOM_IN; j++ ) {
            // k = Card Divi
            for ( int k = 0; k < 4; k++ ) {
                for ( int nUserEatCnt = 0; nUserEatCnt < g_pGoStop->m_pFloor.m_pGamer[j].m_nEatCnt[k]; nUserEatCnt++ ) {
                    if ( g_sMissionCardInfo.nCardNo[i] == g_pGoStop->m_pFloor.m_pGamer[j].m_nEatCard[k][nUserEatCnt] ) {
                        g_sMissionCardInfo.nUserNo[i] = g_pGoStop->m_pFloor.m_pGamer[j].m_nUserNo;
                        g_sMissionCardInfo.nCardNo[i] = (g_sMissionCardInfo.nCardNo[i] % 52 ) + 52;     // ��ο� ī��� ����
                        g_sMissionCardInfo.bEat[i] = TRUE;
                        break;
                    }
                }

                if ( g_sMissionCardInfo.bEat[i] )
                    break;
            }
            
            if ( g_sMissionCardInfo.bEat[i] )
                break;
        }
    }

    //-----------------
    // �̼�â �̵� ����
    //-----------------

    memset( &m_sMissionDlgInfo, 0x00, sizeof( m_sMissionDlgInfo ));
    m_sMissionDlgInfo.nDlgX      = INIT_MISSION_DLG_POS_X;
    m_sMissionDlgInfo.nDlgY      = INIT_MISSION_DLG_POS_Y;
    m_sMissionDlgInfo.nTimeCnt   = 0;
    m_sMissionDlgInfo.nAniCnt    = -1;
    m_sMissionDlgInfo.nMaxAniCnt = 25;
    m_sMissionDlgInfo.bAni       = true;
}



void CMainFrame::ShakeQ()
{
	l_nDialgType = DLGTYPE_SHAKEQ;
	l_nWaitTime = 300;
	l_nDialogWaitFrameCnt = 0;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



void CMainFrame::Shake()
{
	//DTRACE("Shake() �������ϴ�");

	l_nDialgType = DLGTYPE_SHAKE;
	l_nWaitTime = 100;
	l_nDialogWaitFrameCnt = 0;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



void CMainFrame::WaitCardChoice()
{
	l_nDialgType = DLGTYPE_WAITTWOCARDCHO;
	l_nWaitTime = 0;
	l_nDialogWaitFrameCnt = 0;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



void CMainFrame::SelectNine()
{
	l_nDialgType = DLGTYPE_SSANGPICHO;
	l_nWaitTime = 300;
	l_nDialogWaitFrameCnt = 0;
	l_bShowDialog = TRUE;
    g_bRefreshTransDlg = true;
}



void CMainFrame::KillThread(HANDLE Thread_Handle)
{
    DWORD exitcode;
    GetExitCodeThread(Thread_Handle, &exitcode);
    TerminateThread(Thread_Handle, exitcode);
}



void CMainFrame::UserCardSetTime( int nSlotNo )
{
    // [*IMSI]
	DWORD dwThreadId;
	g_bUserCardRecv = false;

	DWORD dwRe = WaitForSingleObject( hUserCardThread, WAIT_1TIME );

	if ( dwRe == WAIT_TIMEOUT ) {
		m_UserTimerThread = TRUE;
		
		dwRe = WaitForSingleObject( hUserCardThread, WAIT_5TIME );
		
		if ( dwRe == WAIT_TIMEOUT ) {
			KillThread(hUserCardThread);  
		}
	}

	m_UserTimerThread = FALSE;
	hUserCardThread = CreateThread( NULL, 0, UserCardThreadProc, (LPVOID)nSlotNo, 0, &dwThreadId );
    ////
}



DWORD WINAPI UserCardThreadProc( LPVOID lParameter )
{
	int nSecPos = 0;
	int nSlotNo = (int)lParameter;
	int i;

	//ó���� 5�ʰ��� ī���͸� ���� �ʴ´�.
	g_pCMainFrame->l_bTimeEvent = TRUE;

	for ( i = 0; i < ( USER_WAIT_TIME * 10); i++ ) {
		if ( g_bUserCardRecv || g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bComputer || g_pCMainFrame->m_UserTimerThread == TRUE  ) { 
			break;
		}
		Sleep(100);
	}

	//10�ʰ� ī���� �Ѵ�.

	for ( i = 0; i < 100; i++ ) {
		if ( g_bUserCardRecv || g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bComputer || g_pCMainFrame->m_UserTimerThread == TRUE ) { 
			break;
		}
		
		if ( !( i % 10 ) )
			g_pCMainFrame->PlaySound( g_Sound[GS_COUNT] );

		Sleep(100);
		nSecPos++;
		if ( nSecPos >= 10 ) {
			nSecPos = 0;
			g_pCMainFrame->l_nSecPos--;	//Ÿ�̸� ��� �������δ�.
			if ( g_pCMainFrame->l_nSecPos < 0 ) {
				g_pCMainFrame->l_nSecPos = 9;
				break;
			}
		}
	}

	g_pCMainFrame->l_bTimeEvent = false;
	g_pCMainFrame->l_nSecPos = 9;

	if ( g_bUserCardRecv ) return 0;

	g_pGoStop->m_bMyKetInput = false;

	if ( g_pCMainFrame->m_UserTimerThread == TRUE ) return 0;
	g_pGoStop->SetActionEvent();

	if ( g_pGoStop->m_pFloor.m_pGamer[nSlotNo].m_bComputer ) {	//��ǻ�ͷ� �ٲ������ �׼��� Ǯ���ش�.	//�濡�� ©������..�ְ� Ǯ���ְ�...
		if ( g_pGoStop->m_bOtherWait )
			g_pGoStop->SetOtherActionEvent();
	}

	return 0;
}



int g_nCnt = 0;

DWORD WINAPI AniUpdateThreadProc( LPVOID lParameter )
{
	while( g_pCMainFrame->m_bActive ) {

		g_pCMainFrame->m_fSkipTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

		if(g_pCMainFrame->m_bActive)
		{
			if( g_pCMainFrame->m_FrameSkip.Update( g_pCMainFrame->m_fSkipTime ) )
			{
				g_pCMainFrame->AnimationUpdate(); // �ִϸ��̼� ������Ʈ bsw 01.06
			}

			Sleep(5);
		}	

	}

	return 0;
}



DWORD WINAPI RenderThreadProc( LPVOID lParameter )
{
	while( g_pCMainFrame->m_bActive ) {
		if(g_pCMainFrame->m_bActive) {
			g_pCMainFrame->Render();
			Sleep(10);
		}
	}

	return 0;
}



void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CFrameWnd::OnLButtonDblClk(nFlags, point);
}



// nGubun : 0 - �� 1 - ����  2 - ��� ( ������ �ٸ��� )  3 - ���.
void CMainFrame::SetMoneyStringToSprite( char szMoneyString[50], int nXPos, int nYPos, int nGubun, BOOL bResult )
{
}



int CMainFrame::GetPixelByString( LPCTSTR szText )
{
	int nCount = 0;
	int nStringLen = 0;
	char szJunkText[255];
	char szCopyText[255];
	SIZE sSize;

	memset( &szJunkText, 0x00, sizeof( szJunkText ));
	memset( &szCopyText, 0x00, sizeof( szCopyText ));
	memset( &sSize, 0x00, sizeof( sSize ));

	strcpy( szJunkText, szText );
	nStringLen = strlen( szJunkText );

	for( int i = 0 ; i < nStringLen; i++ ) {
		if( szJunkText[i] == NULL )
			continue;

		if( szJunkText[i] & 0x80 ) {
			i++;
			continue;
		}
		
		nCount ++;
	}

    if( nCount > 8 ) {  // �� ����..
		memcpy( &szCopyText , szJunkText , nStringLen - 6 );		
	}
    else {
        strcpy( szCopyText , szJunkText );
    }

	strcat( szCopyText , "Ĩ" );
	
	nStringLen = strlen( szCopyText );
	nCount = 0;

	for( int k = 0; k < nStringLen ; k++ ) {
		if( szCopyText[k] == NULL )
			continue;

		if( szCopyText[k] & 0x80 ) {
			k++;
			nCount += 14;
			continue;
		}

		nCount += 8;
	}

	return nCount;
	
}



void CMainFrame::ShowUserScore( void )
{
    char szJumsu[10]; // ����
	int nValue = -1;
    int nLen = 0;
	RECT rScoreRect;

    for (int i = 0; i < MAX_ROOM_IN; i++ ) {
	    memset( &rScoreRect , 0x00 , sizeof( rScoreRect));	
	    memset( &szJumsu , 0x00 , sizeof( szJumsu ));

	    nValue = g_pGoStop->m_pFloor.m_pGamer[i].m_nResultScore;
	    nLen = 0;

	    _itoa( nValue, szJumsu, 10 );
	    nLen = strlen( szJumsu );

	    // �ڱ� ����
	    if( nValue  >= 0 ) {
		    int nAddXPos = m_sUserScorePos[i].x;

		    for( int j = nLen ; j >= 0; j-- ) {
                switch( szJumsu[j] ) {
				    case '0':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 0 ); break;
				    case '1':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 1 ); break;
				    case '2':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 2 ); break;
				    case '3':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 3 ); break;
				    case '4':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 4 ); break;
				    case '5':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 5 ); break;
				    case '6':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 6 ); break;
				    case '7':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 7 ); break;
				    case '8':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 8 ); break;
				    case '9':   PutSprite( l_SPR_Jumsu, nAddXPos, m_sUserScorePos[i].y, 9 ); break;
                    default:    continue;
			    }

                nAddXPos -= 13;
		    }
	    }

	    nValue = g_pGoStop->m_pFloor.m_pGamer[i].m_nGoCnt - 1;

	    // ��
	    if( nValue >= 0 ) {
            nValue++;
        
            if ( nValue >= 10 ) {
                PutSprite( l_SPR_AffairNum, m_sGoCntPos[i].x - 13, m_sGoCntPos[i].y, nValue / 10 );
                PutSprite( l_SPR_AffairNum, m_sGoCntPos[i].x,      m_sGoCntPos[i].y, nValue % 10 );
            }
            else {
                PutSprite( l_SPR_AffairNum, m_sGoCntPos[i].x, m_sGoCntPos[i].y, nValue );
            }
	    }

	    nValue = g_pGoStop->m_pFloor.m_pGamer[i].m_nShakeCnt - 1;

	    // ���
	    if( nValue >= 0 ) {
            PutSprite( l_SPR_AffairNum, m_sShakeCntPos[i].x, m_sShakeCntPos[i].y, nValue + 1 );  // 3, 4�� �������� ���~
	    }

	    nValue = g_pGoStop->m_pFloor.m_pGamer[i].m_nBbukCnt - 1;	
	    
	    // ��
	    if( nValue >= 0 ) {
            PutSprite( l_SPR_AffairNum, m_sPPuckCntPos[i].x, m_sPPuckCntPos[i].y, nValue + 1 );      // 0~2�� �������� ��~
	    }
    }
}



UINT CMainFrame::OnNcHitTest(CPoint point) 
{
    UINT nHit = CFrameWnd::OnNcHitTest(point);

    ScreenToClient(&point);

	l_mX = point.x;
	l_mY = point.y;

	if( !l_bMoveDlg && !l_bFullScreen && nHit == HTCLIENT ) { // ���콺�� �����϶� �����찡 �����̰� �� ����
        if ( l_nCurrentMode == WAITROOM_MODE ) {
            if (( point.x <= 800 && point.y <= 26 ) ||          // Ÿ��Ʋ �� ����
                ( point.x >= 690 && point.y >= 27 && point.y <= 100 ))
            {
                nHit = HTCAPTION;
            }
        }
        else if ( l_nCurrentMode == GAME_MODE ) {
            if ( point.x <= 790 && point.y <= 28 ) {
                nHit = HTCAPTION;
            }
        }
    }

    return nHit;
}



void CMainFrame::SetGameCursor( BOOL bUserCursor , BOOL bClick, BOOL bMouseRender )
{
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
	else 
		if ( m_nCurSorCnt < 0 ) return;		
		
	if( bUserCursor )
	{
		m_nCurSorCnt = ShowCursor( TRUE );
		
		if( m_nCurSorCnt < 0 )
		{
			while( m_nCurSorCnt < 0 )
			{
				m_nCurSorCnt = ShowCursor( TRUE );
			};
		}
	}
	else
	{
		m_nCurSorCnt = ShowCursor( FALSE );

		if( m_nCurSorCnt > 0 )
		{
			while( m_nCurSorCnt > 0 )
			{
				m_nCurSorCnt = ShowCursor( FALSE );
			};
		}
	}
	
}

// 1 �� �̼� , 2�� ��� , 3�� ����
void CMainFrame::DrawResultDojang( int nType )
{
	//nType �� ���� �������Ѱ� �ƴ϶�� ������.
	if( nType <= 0 || nType > 3 )
		return;

	int T1 = 0;
    int T2 = 0;
    int nCnt = 0;
    int nFrame = 0;

	switch( nType )
	{
		case 1: // �̼�
            nFrame = 0;
			break;

		case 2: // ���
            nFrame = 1;
			break;

 		case 3: // ����
            nFrame = 2;
			break;
	}
	
	T1 = GTC();
	if( T1 - T2 > 1000 )
	{
		T2 = GTC();
		
		m_nDojangCount++;
		
		if( m_nDojangCount >= 30 )
		{
			m_nDojangCount = 30;
			nCnt = 1;
			
			if( !m_bDojangSound )
			{
				PlaySound( g_Sound[GS_RESULT] );
				m_bDojangSound = TRUE;
			}
		}
	}
    
    switch ( nCnt ) {
        case 0:
            DrawScale( l_SPR_ResultDojang, RESULT_POS_X - 137, RESULT_POS_Y - 124, 282, 255, nFrame );
            break;

        case 1:
            PutSprite( l_SPR_ResultDojang, RESULT_POS_X - 94, RESULT_POS_Y - 85, nFrame );
            break;
    }
}



void CMainFrame::MakeNickName( int nMaxLine , char szNick[512] )
{	
	char szTempText[512] , szTempId[512];
	
	memset( &szTempText, 0x00, sizeof(szTempText) );
	memset( &szTempId, 0x00, sizeof(szTempId) );

	if ( strlen( szNick ) > 12 ) {		
		
		memcpy( szTempText, szNick , 12 );
		strcat( szTempText, "..." );
		
		strcpy( szTempId , szTempText );
	}
	else {

		strcpy( szTempId, szNick );
	}	

	strcpy( szNick , szTempId );
}



//========================================================================
// ä��â�� ���� ��� ���
//========================================================================

void CMainFrame::PutResultText( void )
{
	int nWinnerNo = g_pGoStop->m_pFloor.m_nWinerNo;
	int nSlotMoney = g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_biWinMoney;	

	char szResultText[512], szNickName[30], szUserMoney[50], szRealMoney[50];
	memset( &szResultText, 0x00, sizeof( szResultText ));
	memset( &szNickName,   0x00, sizeof( szNickName ));
	memset( &szUserMoney,  0x00, sizeof( szUserMoney ));
	memset( &szRealMoney,  0x00, sizeof( szRealMoney ));

    AddGameChatText( "-------���Ӱ��-------", RGB( 255, 255, 255 ));

	// �̱� ������ �г����� ��´�.
	if ( g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nGamerUserNo < 0 ) {

		if ( nWinnerNo == 1 ) 
			strcpy( szNickName, "Com1" );
		else
			strcpy( szNickName, "Com2" );
	}
	else {
		strcpy( szNickName, g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[nWinnerNo].m_nGamerUserNo].m_sUserInfo.szNick_Name );
		MakeNickName( 14, szNickName );
	}

	// �̱� ������ �� ������ ��´�.
	if( nSlotMoney > 0 ) {		
		I64toA_Money( nSlotMoney, szUserMoney );		
		sprintf( szRealMoney, "+%s", szUserMoney );
	}
	else {
		I64toA_Money( nSlotMoney, szUserMoney );
		strcpy( szRealMoney, szUserMoney );
	}

	// �̱� ������ �г��Ӱ� �̱� ���� �ѷ��ش�.
	sprintf( szResultText , "��%s�� ��" , szNickName );
    AddGameChatText( szResultText , RGB( 255 , 255 , 255 ));

	memset( &szResultText , 0x00 , sizeof(szResultText));
    AddGameChatText( szRealMoney , RGB( 255 , 255 , 255 ));

    int nLoserSlot = 0;         // �� ������ ���� ��ȣ

    for ( int i = 1; i < MAX_ROOM_IN; i++ ) {
        nLoserSlot = ( nWinnerNo + i ) % MAX_ROOM_IN;
	    nSlotMoney = g_pGoStop->m_pFloor.m_pGamer[nLoserSlot].m_biLoseMoney;

	    memset( &szResultText, 0x00, sizeof( szResultText));
	    memset( &szNickName,   0x00, sizeof( szNickName));
	    memset( &szUserMoney,  0x00, sizeof( szUserMoney));
	    memset( &szRealMoney,  0x00, sizeof( szRealMoney));

	    // �� ������ �г����� ��´�.
	    if ( g_pGoStop->m_pFloor.m_pGamer[nLoserSlot].m_nGamerUserNo < 0 ) {

		    if (  nLoserSlot == 1 )
			    strcpy( szNickName, "Com1" );
		    else
			    strcpy( szNickName, "Com2" );
	    }
	    else {
		    strcpy( szNickName , g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[nLoserSlot].m_nGamerUserNo].m_sUserInfo.szNick_Name );
		    MakeNickName( 14 , szNickName );
	    }	

	    // �� ������ �� ������ ��´�.
	    if( nSlotMoney > 0 ) {		
		    I64toA_Money( nSlotMoney, szUserMoney );
		    sprintf( szRealMoney, "-%s", szUserMoney );
	    }
	    else {
		    I64toA_Money( nSlotMoney, szUserMoney );
		    strcpy( szRealMoney, szUserMoney );
	    }

	    // �� ������ �г��Ӱ� �̱� ���� �ѷ��ش�.
	    sprintf( szResultText, "��%s�� ��", szNickName );
        AddGameChatText( szResultText, RGB( 255, 255, 255 ));

	    memset( &szResultText, 0x00, sizeof(szResultText));
        AddGameChatText( szRealMoney, RGB( 255, 255, 255 ));
    }

    AddGameChatText( "----------------------" , RGB( 255, 255, 255 ));
}



void CMainFrame::PutTextInGame( char *szNick , char *szText , BigInt biMoneyVal , COLORREF rgb , BOOL bMoneyUse , BOOL bPlus )
{
	char szNickName[30];    // ���� �̸�
	char szContent[255];    // ����
	char szMoneyText[255];  // �� ����
    char szMTemp[255];      // �� ����
	char szPrintText[512];  //��ü ��� �κ�

	memset( &szNickName,  0x00, sizeof( szNickName  ));
	memset( &szContent,   0x00, sizeof( szContent   ));
	memset( &szMoneyText, 0x00, sizeof( szMoneyText ));
	memset( &szMTemp,     0x00, sizeof( szMTemp     ));
	memset( &szPrintText, 0x00, sizeof( szPrintText ));

	strcpy( szNickName, szNick );   // ���� �̸� ī��
	strcpy( szContent, szText );    // ���� ī��

	if( bMoneyUse ) {
		I64toA_Money( biMoneyVal, szMTemp ); // ���� ��´�. ( ������ ���ڿ��� ���� )	
		
		// �������� ����Ѵٸ�
		if( bPlus ) {
			if( biMoneyVal > 0 )
				sprintf( szMoneyText, "+%s" , szMTemp );	
			else
				sprintf( szMoneyText, "%s" , szMTemp );	
		}
        // �������� ����Ѵٸ�
		else {
			if( biMoneyVal > 0 )
				sprintf( szMoneyText , "-%s" , szMTemp );	
			else
				sprintf( szMoneyText , "%s" , szMTemp );
		}
	}
	
	// ������ NULL �̸� ������ ���� ��´�.
	if( szContent == NULL ) {
		if( bMoneyUse )
			sprintf( szPrintText , "��%s��. %s" , szNickName , szMoneyText );
		else
			sprintf( szPrintText , "��%s��." , szNickName );
	}
	else {
		if( bMoneyUse )
			sprintf( szPrintText , "��%s�� %s %s" , szNickName , szContent , szMoneyText );
		else
			sprintf( szPrintText , "��%s�� %s" , szNickName , szContent );
	}
	
    AddGameChatText( szPrintText , rgb );
}



BEGIN_DISPATCH_MAP(CMainFrame, CCmdTarget)
	DISP_FUNCTION_ID(CMainFrame, "WindowRegistered",0x000000c8,WindowRegistered,VT_EMPTY,VTS_I4)
	DISP_FUNCTION_ID(CMainFrame, "WindowRevoked",0x000000c9,WindowRevoked,VT_EMPTY,VTS_I4)
END_DISPATCH_MAP()



static SHDocVw::IShellWindowsPtr m_spSHWinds; 



void CMainFrame::WindowRegistered(long lCookie) 
{
	//OnOK() ;
}



void CMainFrame::WindowRevoked(long lCookie)
{
	//OnOK() ;
}



void CMainFrame::ConnectToShell() 
{
	CoInitialize(NULL);

	if(m_spSHWinds == 0) {
		//
		// Create Instance ShellWindows
		//
		if(m_spSHWinds.CreateInstance(__uuidof(SHDocVw::ShellWindows)) == S_OK) {

			//
			// Sink for Events
			//
			LPCONNECTIONPOINTCONTAINER pConnPtCont;

			if ((m_spSHWinds != NULL) &&
				SUCCEEDED(m_spSHWinds->QueryInterface(IID_IConnectionPointContainer,
					(LPVOID*)&pConnPtCont)))
			{
				ASSERT(pConnPtCont != NULL);
				LPCONNECTIONPOINT pConnPt = NULL;
				DWORD dwCookie = 0;

				if (SUCCEEDED(pConnPtCont->FindConnectionPoint(__uuidof(SHDocVw::DShellWindowsEvents), &pConnPt)))
				{
					ASSERT(pConnPt != NULL);
					pConnPt->Advise( GetIDispatch(FALSE), &dwCookie);
					pConnPt->Release();
				}

				pConnPtCont->Release();
			}
		}
		else {
			AfxMessageBox("Shell Windows interface is not avilable");
		}
	}
}


/*
void CMainFrame::ExploClose() 
{
	IDispatchPtr spDisp;
	long nCount = m_spSHWinds->GetCount();
	CString strExploName;
	//
	// Enum all Shell Windows and list them
	//
	for (long i = 0; i < nCount; i++)
	{
		_variant_t va(i, VT_I4);
		spDisp = m_spSHWinds->Item(va);

		SHDocVw::IWebBrowser2Ptr spBrowser(spDisp);
		if (spBrowser != NULL)
		{
			_bstr_t str = spBrowser->GetLocationName();
			spBrowser->AddRef();

			strExploName = (LPCTSTR)str;

			if ( strExploName == "�׿����� �ε���" )
				spBrowser->Quit();

		}
	}
}
*/



void CMainFrame::SetHumanVoice( int nVoiceNo )
{
}



void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}



void CMainFrame::PutSprite( CDXSprite *pSprite, int nX, int nY, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
}



void CMainFrame::PutSpriteBlk( CDXSprite *pSprite, int nX, int nY, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );
}



void CMainFrame::PutSprite( CDXSprite *pSprite, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANS );
}



void CMainFrame::PutSpriteBlk( CDXSprite *pSprite, int nFrame )
{
    pSprite->SetFrame( nFrame );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );
}



void CMainFrame::PutSprite( CDXSprite *pSprite, int nX, int nY, RECT *srcRect )
{
    pSprite->GetTile()->DrawTrans( g_pCDXScreen->GetBack(), nX, nY, srcRect );
}



void CMainFrame::PutSpriteAlpha( CDXSprite *pSprite, int nX, int nY, int nAlpha, int nFrame )
{
    pSprite->SetAlphaValue( nAlpha );
    pSprite->SetPos( nX, nY );
    pSprite->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANSALPHA );
}



void CMainFrame::DrawScale(CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame)
{
    pSprite->SetFrame( nFrame );
    pSprite->SetPos(nX, nY);
	pSprite->SetStretchWidth(nWidth);   // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
    pSprite->SetStretchHeight(nHeight); // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
    pSprite->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_TRANSSTRETCHED);
}



void CMainFrame::DrawScaleBlk(CDXSprite *pSprite, int nX, int nY, int nWidth, int nHeight, int nFrame)
{
    if ( pSprite != NULL ) {
        pSprite->SetFrame( nFrame );
        pSprite->SetPos(nX, nY);
	    pSprite->SetStretchWidth(nWidth);   // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
        pSprite->SetStretchHeight(nHeight); // ����ȭ�鿡�� �ٿ���� ���� ũ��(����)
        pSprite->Draw(g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLKSTRETCHED);
    }
}



void CMainFrame::PutSpriteAvatar( CDXSprite *aAvatarSPR, int x, int y, int nType )
{
	if ( nType == SMALL_AVATAR_SIZE ) {
        DrawScaleBlk( aAvatarSPR, x, y, 100, 150 );
	}
	else {
		aAvatarSPR->SetPos( x, y );
        aAvatarSPR->Draw( g_pCDXScreen->GetBack(), 0, 0, CDXBLT_BLK );
	}
}



void CMainFrame::DrawText(char *szText, int nX, int nY, COLORREF rgbColor, int nStyle, int nAlign, int nFontSize)
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
        return;
    }

    nStrLen = lstrlen(szText);  // ���ڿ��� ���̸� ���Ѵ�.
    pSurface = g_pCDXScreen->GetBack();

	hDC = pSurface->GetDC();

    if ( hDC == NULL ) {
        return;
    }

    nRetVal = pSurface->ChangeFont("����ü", 0, nFontSize, nStyle);   // ��Ʈ �ٲ�
    nRetVal = pSurface->SetFont();                                    // �ٲ� ��Ʈ ����

    bRetApiFunc = GetTextExtentPoint32(hDC, szText, nStrLen, &sTextSize); // ��Ʈ ũ�� ������

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



CDXSoundBuffer *CMainFrame::LoadSndFile(char *szPath, char *szFilename, int nBuffNum )
{
    if (szPath == NULL || szFilename == NULL || !g_bSoundCard) {
        return NULL;
    }

    char szFullPath[256];
    memset(szFullPath, 0, sizeof(szFullPath));
    strcpy(szFullPath, szPath);
    strcat(szFullPath, szFilename);

    CDXSoundBuffer *pSndBuf = new CDXSoundBuffer();
    pSndBuf->Load(g_pCDXSound, szFullPath, nBuffNum);
    return pSndBuf;
}



void CMainFrame::PlaySound( CDXSoundBuffer *pSound , bool bPlayFlags )
{
    if ( pSound == NULL || g_sClientEnviro.bOption[2] == false || !g_bSoundCard) return;

	if( bPlayFlags == false )
		pSound->Play();
	else
		pSound->Play( DSBPLAY_LOOPING );
}



void CMainFrame::StopSound( CDXSoundBuffer *pSound )
{
    if ( pSound != NULL ) {
        pSound->Stop();
    }
}



void CMainFrame::StopAllSound( void )
{
	for( int i = 0 ; i < MAX_SOUND_CNT ; i ++ )
	{
		if( g_Sound[i] != NULL )
			g_Sound[i]->Stop();
	}
}



void CMainFrame::DestroySound( void )
{
	StopAllSound();
	
	if ( g_pCDXSound != NULL )
	{
		for( int i = 0; i < MAX_SOUND_CNT ; i ++)
		{
			if( g_Sound[i] != NULL )
			{
                SAFE_DELETE( g_Sound[i] );
			}
		}
	}
}



//========================================================================
// ��ư �ε�
//========================================================================
CDXButton *CMainFrame::LoadButton(LPSTR szFilename, int nX, int nY, int nW, int nH, BOOL bMulti, int nCount, int nDirection, int nDistance)
{
    return new CDXButton(g_pCDXScreen, szFilename, nX, nY, nW, nH, bMulti, nCount, nDirection, nDistance);
}



//========================================================================
// ��������Ʈ �ε�
//========================================================================

CDXSprite *CMainFrame::LoadSprite(LPSTR szFilename, int nW, int nH, int nNum)
{
    CDXSprite *pSPR_Temp = new CDXSprite();
    pSPR_Temp->Create(g_pCDXScreen, szFilename, nW, nH, nNum);
    return pSPR_Temp;
}



void CMainFrame::ReGetSprite( CDXSprite **pSprite, char *szFilename, int nW, int nH, int nFrmNum )
{
    if ( *pSprite != NULL ) {
        SAFE_DELETE( *pSprite );
    }

    *pSprite = LoadSprite( szFilename, nW, nH, nFrmNum );
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

    if (nUpDownBTNWidth > nDragBTNWidth)    // UP/DOWN ��ư ���� DRAG ��ư�� ���� ���
    {
        nDragBTNX = nUpDownBTNX + ((nUpDownBTNWidth - nDragBTNWidth) >> 1);
    }
    else                                    // ū ���
    {
        nDragBTNX = nUpDownBTNX - ((nDragBTNWidth - nUpDownBTNWidth) >> 1);
    }
    
    nUpBTNY   = pScrollBarRect->top;
    nDownBTNY = pScrollBarRect->bottom + 1 - nUpDownBTNHeight;
    nDragBTNY = pScrollBarRect->top + nUpDownBTNHeight;
    
    pUpBTN   = new CDXButton( g_pCDXScreen, szUpBTN,   nUpDownBTNX, nUpBTNY,   nUpDownBTNWidth, nUpDownBTNHeight );
    pDownBTN = new CDXButton( g_pCDXScreen, szDownBTN, nUpDownBTNX, nDownBTNY, nUpDownBTNWidth, nUpDownBTNHeight );
    pDragBTN = new CDXButton( g_pCDXScreen, szDragBTN, nDragBTNX,   nDragBTNY, nDragBTNWidth,   nDragBTNHeight   );
    
    pScrollBar = new CDXScrollBar(pUpBTN, pDownBTN, pDragBTN, nViewNum, nMaxNum);
    pScrollBar->SetDragRect(nDragBTNX, nDragBTNY, nDragBTNX + nDragBTNWidth - 1, nDownBTNY - 1, nDragBTNHeight);
    return pScrollBar;
}



void CMainFrame::SetColorKey() 
{
    int i;
    RGBFORMAT pixelFormat;
    DWORD dwColorKey;

    g_pCDXScreen->GetFront()->GetPixelFormat(&pixelFormat);

    // [zoom]
    switch(pixelFormat.bpp) {
      //case  8:    
        case 15:
            dwColorKey = RGB16BIT555(255, 0, 255);
            m_fpPutHwatuZoom = &CMainFrame::PutHwatuZoom16;
            break;

        case 16:
            dwColorKey = RGB16BIT565(255, 0, 255);
            m_fpPutHwatuZoom = &CMainFrame::PutHwatuZoom16;
            break;

        case 24:
            dwColorKey = RGB24BIT(255, 0, 255);
            m_fpPutHwatuZoom = &CMainFrame::PutHwatuZoom32;
            break;

        case 32:
            dwColorKey = RGB32BIT(255, 0, 255);
            m_fpPutHwatuZoom = &CMainFrame::PutHwatuZoom32;
            break;
    }
    /////////


    if ( l_SPR_Result           != NULL ) l_SPR_Result->SetColorKey( dwColorKey );
    if ( l_SPR_HwatuBackSmall   != NULL ) l_SPR_HwatuBackSmall->SetColorKey( dwColorKey );
    if ( l_SPR_Hwatu            != NULL ) l_SPR_Hwatu->SetColorKey( dwColorKey );
    if ( l_SPR_MissionTimes     != NULL ) l_SPR_MissionTimes->SetColorKey( dwColorKey );
    if ( l_SPR_BombLine         != NULL ) l_SPR_BombLine->SetColorKey( dwColorKey );
    if ( l_SPR_Dialog           != NULL ) l_SPR_Dialog->SetColorKey( dwColorKey );
    if ( l_SPR_Cursor           != NULL ) l_SPR_Cursor->SetColorKey( dwColorKey );
    if ( l_SPR_Cursor2          != NULL ) l_SPR_Cursor2->SetColorKey( dwColorKey );
    if ( l_SPR_NotifyText       != NULL ) l_SPR_NotifyText->SetColorKey( dwColorKey );
    if ( l_SPR_HwatuSelect      != NULL ) l_SPR_HwatuSelect->SetColorKey( dwColorKey );
    if ( l_SPR_Time             != NULL ) l_SPR_Time->SetColorKey( dwColorKey );
    if ( l_SPR_EVENT            != NULL ) l_SPR_EVENT->SetColorKey( dwColorKey );
    if ( l_SPR_Connting         != NULL ) l_SPR_Connting->SetColorKey( dwColorKey );
    if ( l_SPR_Baks             != NULL ) l_SPR_Baks->SetColorKey( dwColorKey );
    if ( l_SPR_Start            != NULL ) l_SPR_Start->SetColorKey( dwColorKey );
    if ( l_SPR_DlgBar           != NULL ) l_SPR_DlgBar->SetColorKey( dwColorKey );
    if ( l_SPR_KingFirst        != NULL ) l_SPR_KingFirst->SetColorKey( dwColorKey );
    if ( l_SPR_ConBox           != NULL ) l_SPR_ConBox->SetColorKey( dwColorKey );
    if ( l_SPR_Message          != NULL ) l_SPR_Message->SetColorKey( dwColorKey );
    if ( l_SPR_COM              != NULL ) l_SPR_COM->SetColorKey( dwColorKey );
    if ( l_SPR_InviteBox        != NULL ) l_SPR_InviteBox->SetColorKey( dwColorKey );
    if ( l_SPR_InviteIcon       != NULL ) l_SPR_InviteIcon->SetColorKey( dwColorKey );
    if ( l_SPR_MessageBox       != NULL ) l_SPR_MessageBox->SetColorKey( dwColorKey );
    if ( l_SPR_UserState        != NULL ) l_SPR_UserState->SetColorKey( dwColorKey );
    if ( l_SPR_Invitation       != NULL ) l_SPR_Invitation->SetColorKey( dwColorKey );
    if ( l_SPR_FullWait         != NULL ) l_SPR_FullWait->SetColorKey( dwColorKey );
    if ( l_SPR_P_Back           != NULL ) l_SPR_P_Back->SetColorKey( dwColorKey );
    if ( l_SPR_AffairNum        != NULL ) l_SPR_AffairNum->SetColorKey( dwColorKey );
    if ( l_SPR_Jumsu            != NULL ) l_SPR_Jumsu->SetColorKey( dwColorKey );
    if ( l_SPR_MissionOK        != NULL ) l_SPR_MissionOK->SetColorKey( dwColorKey );
    if ( l_SPR_Class            != NULL ) l_SPR_Class->SetColorKey( dwColorKey );
    if ( l_SPR_ResultMessage    != NULL ) l_SPR_ResultMessage->SetColorKey( dwColorKey );
    if ( l_SPR_ResultDojang     != NULL ) l_SPR_ResultDojang->SetColorKey( dwColorKey );
    if ( l_SPR_MissionNext      != NULL ) l_SPR_MissionNext->SetColorKey( dwColorKey );
    if ( l_SPR_MakeRoom         != NULL ) l_SPR_MakeRoom->SetColorKey( dwColorKey );
    if ( m_SPR_PassDlg          != NULL ) m_SPR_PassDlg->SetColorKey( dwColorKey );
    if ( m_SPR_ResultNum[0]     != NULL ) m_SPR_ResultNum[0]->SetColorKey( dwColorKey );
    if ( m_SPR_ResultNum[1]     != NULL ) m_SPR_ResultNum[1]->SetColorKey( dwColorKey );
    if ( m_SPR_UserListTitle[0] != NULL ) m_SPR_UserListTitle[0]->SetColorKey( dwColorKey );
    if ( m_SPR_UserListTitle[1] != NULL ) m_SPR_UserListTitle[1]->SetColorKey( dwColorKey );
    if ( m_SPR_ExitReserv       != NULL ) m_SPR_ExitReserv->SetColorKey( dwColorKey );
    if ( m_SPR_WaitDlg          != NULL ) m_SPR_WaitDlg->SetColorKey( dwColorKey );
    if ( m_SPR_CheckBall        != NULL ) m_SPR_CheckBall->SetColorKey( dwColorKey );
    if ( m_SPR_HighlightBar     != NULL ) m_SPR_HighlightBar->SetColorKey( dwColorKey );
    if ( m_SPR_CardBlink        != NULL ) m_SPR_CardBlink->SetColorKey( dwColorKey );
    if ( m_SPR_ChodaeOX         != NULL ) m_SPR_ChodaeOX->SetColorKey( dwColorKey );
    if ( m_SPR_RoomBack         != NULL ) m_SPR_RoomBack->SetColorKey( dwColorKey );    // ham 2006.01.18
    if ( m_SPR_UserIcon         != NULL ) m_SPR_UserIcon->SetColorKey( dwColorKey );    // ham 2006.01.18

    if ( m_SPR_Arrow            != NULL ) m_SPR_Arrow->SetColorKey( dwColorKey );
    if ( m_SPR_MissionDlg       != NULL ) m_SPR_MissionDlg->SetColorKey( dwColorKey );
    if ( l_SPR_MComplete        != NULL ) l_SPR_MComplete->SetColorKey( dwColorKey );

    if ( m_SPR_SelSunText       != NULL ) m_SPR_SelSunText->SetColorKey( dwColorKey );
    if ( m_SPR_SelSunTextBack   != NULL ) m_SPR_SelSunTextBack->SetColorKey( dwColorKey );
    
    if ( l_MessageOKBTN         != NULL ) l_MessageOKBTN->SetColorKey( dwColorKey );
    if ( l_MemoSendBTN          != NULL ) l_MemoSendBTN->SetColorKey( dwColorKey );
    if ( l_MemoCancelBTN        != NULL ) l_MemoCancelBTN->SetColorKey( dwColorKey );
    if ( l_UserStateBTN         != NULL ) l_UserStateBTN->SetColorKey( dwColorKey );
    if ( l_RecvChodaeOkBTN      != NULL ) l_RecvChodaeOkBTN->SetColorKey( dwColorKey );
    if ( l_RecvChodaeCancelBTN  != NULL ) l_RecvChodaeCancelBTN->SetColorKey( dwColorKey );
    if ( l_MaxBTN               != NULL ) l_MaxBTN->SetColorKey( dwColorKey );
    if ( l_MaxBTN2              != NULL ) l_MaxBTN2->SetColorKey( dwColorKey );
    if ( l_MinBTN               != NULL ) l_MinBTN->SetColorKey( dwColorKey );
    if ( l_ExitBTN              != NULL ) l_ExitBTN->SetColorKey( dwColorKey );
    if ( l_MakeRoomOkBTN        != NULL ) l_MakeRoomOkBTN->SetColorKey( dwColorKey );
    if ( l_MakeRoomCancelBTN    != NULL ) l_MakeRoomCancelBTN->SetColorKey( dwColorKey );
    if ( l_StartBTN             != NULL ) l_StartBTN->SetColorKey( dwColorKey );
    if ( l_GoBTN                != NULL ) l_GoBTN->SetColorKey( dwColorKey );
    if ( l_StopBTN              != NULL ) l_StopBTN->SetColorKey( dwColorKey );
    if ( l_GNOBTN               != NULL ) l_GNOBTN->SetColorKey( dwColorKey );
    if ( l_GYESBTN              != NULL ) l_GYESBTN->SetColorKey( dwColorKey );
    if ( l_ConfigBTN            != NULL ) l_ConfigBTN->SetColorKey( dwColorKey );
    if ( l_Exit2BTN             != NULL ) l_Exit2BTN->SetColorKey( dwColorKey );
    if ( l_ResultOKBTN          != NULL ) l_ResultOKBTN->SetColorKey( dwColorKey );
    if ( l_ConfigOkBTN          != NULL ) l_ConfigOkBTN->SetColorKey( dwColorKey );
    if ( l_InviteBTN            != NULL ) l_InviteBTN->SetColorKey( dwColorKey );
    if ( l_InviteOKBTN          != NULL ) l_InviteOKBTN->SetColorKey( dwColorKey );
    if ( l_InviteCancelBTN      != NULL ) l_InviteCancelBTN->SetColorKey( dwColorKey );
    if ( l_CaptureBTN           != NULL ) l_CaptureBTN->SetColorKey( dwColorKey );
    if ( m_RoomTitleBTN         != NULL ) m_RoomTitleBTN->SetColorKey( dwColorKey );
    if ( m_PassDlgOkBTN         != NULL ) m_PassDlgOkBTN->SetColorKey( dwColorKey );
    if ( m_PassDlgCancelBTN     != NULL ) m_PassDlgCancelBTN->SetColorKey( dwColorKey );

    if ( m_SCB_WaitChat         != NULL ) m_SCB_WaitChat->SetColorKey( dwColorKey );
    if ( m_SCB_GameChat         != NULL ) m_SCB_GameChat->SetColorKey( dwColorKey );
    if ( m_SCB_WaitUser         != NULL ) m_SCB_WaitUser->SetColorKey( dwColorKey );
    if ( m_SCB_WaitRoom         != NULL ) m_SCB_WaitRoom->SetColorKey( dwColorKey );
    if ( m_SCB_Invite           != NULL ) m_SCB_Invite->SetColorKey( dwColorKey );

    if ( m_MakeRoomBTN[0] != NULL ) m_MakeRoomBTN[0]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[1] != NULL ) m_MakeRoomBTN[1]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[2] != NULL ) m_MakeRoomBTN[2]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[3] != NULL ) m_MakeRoomBTN[3]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[4] != NULL ) m_MakeRoomBTN[4]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[5] != NULL ) m_MakeRoomBTN[5]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[6] != NULL ) m_MakeRoomBTN[6]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[7] != NULL ) m_MakeRoomBTN[7]->SetColorKey( dwColorKey );
    if ( m_MakeRoomBTN[8] != NULL ) m_MakeRoomBTN[8]->SetColorKey( dwColorKey );

    if ( m_JoinBTN[0] != NULL ) m_JoinBTN[0]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[1] != NULL ) m_JoinBTN[1]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[2] != NULL ) m_JoinBTN[2]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[3] != NULL ) m_JoinBTN[3]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[4] != NULL ) m_JoinBTN[4]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[5] != NULL ) m_JoinBTN[5]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[6] != NULL ) m_JoinBTN[6]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[7] != NULL ) m_JoinBTN[7]->SetColorKey( dwColorKey );
    if ( m_JoinBTN[8] != NULL ) m_JoinBTN[8]->SetColorKey( dwColorKey );

    for ( i = 0; i < MAX_ROOM_IN; i++ ) {
        if ( l_SPR_TurnLine[i] != NULL ) l_SPR_TurnLine[i]->SetColorKey( dwColorKey );
    }

    // [alpha]
    if ( m_SFC_DialogBack != NULL ) {
        m_SFC_DialogBack->SetPixelFormat( &pixelFormat );
        m_SFC_DialogBack->SetColorKey( dwColorKey );                // �÷�Ű ����
        m_SFC_DialogBack->FunctionMapper();                         // ��庰 ��� �Լ� �� ����
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
    /*
    #define RGB16BIT555(r, g, b)    ((((r) % 32) << 10) + (((g) % 32) << 5) + ((b) % 32))
    #define RGB16BIT565(r, g, b)    ((((r) % 32) << 11) + (((g) % 64) << 5) + ((b) % 32))
    #define RGB24BIT(r, g, b)       RGB((r), (g), (b))
    #define RGB32BIT(r, g, b)       RGB((r), (g), (b))
    */

    switch( bpp ) {
        case 15:
            m_rgbMakeRoomTitleListBorder = RGB16BIT555( 51, 26, 5 );
            m_rgbMakeRoomTitleListBar = RGB16BIT555( 183, 112, 15 );
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
            break;

        case 24:
            m_rgbMakeRoomTitleListBorder = RGB24BIT( 51, 26, 5 );
            m_rgbMakeRoomTitleListBar = RGB24BIT( 183, 112, 15 );
            break;

        case 32:
            m_rgbMakeRoomTitleListBorder = RGB32BIT( 51, 26, 5 );
            m_rgbMakeRoomTitleListBar = RGB32BIT( 183, 112, 15 );
            break;
    }
}



//========================================================================
// ������ �Ӽ� ����
//
// ȭ�� ��尡 �ٲ� ������ ��忡 �°� ������ �Ӽ��� �����Ѵ�.
// ChangeVideoMode() �Լ����� ȣ��ȴ�.
//========================================================================

void CMainFrame::AdjustWinStyle()
{
	DWORD dwStyle;

	if ( l_bFullScreen )  // ���� ��üȭ�����̸�, ��üȭ�鿡 �°� ������ �Ӽ��� �ٲپ� �ش�.
    {
		dwStyle = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
        ::SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	else
    {
        RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

		// ���� â���ȭ���̸�, â��忡 �°� ������ �Ӽ��� �����Ѵ�.
		dwStyle = GetWindowStyle(g_hWnd);
        
		SetWindowLong(g_hWnd, GWL_STYLE, dwStyle); // �������� �Ӽ��� �ٲ۴�.
        AdjustWindowRectEx(&rect, GetWindowStyle(g_hWnd), ::GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));
        ::SetWindowPos(g_hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        ::SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}



//========================================================================
// ȭ���� ���(������ ���, Ǯ��ũ�� ���)�� �ٲ۴�.
//
// input:
//      bFullScreen:    true = Ǯ��ũ�� ���� �ٲ۴�.
//                      false = ������ ���� �ٲ۴�.
//========================================================================

void CMainFrame::ChangeVideoMode(BOOL bFullScreen)
{
    // ��üȭ�� ���� ����
	if (bFullScreen)
    {
		g_pCDXScreen->GetDD()->RestoreDisplayMode();
        AdjustWinStyle();
		if (FAILED(g_pCDXScreen->ChangeVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32))) {
			//DTRACE("��üȭ����� �ٲ� �� �����ϴ�.");
            return;
		}
	}

    // â ���� ����
	else
    {
		g_pCDXScreen->GetDD()->RestoreDisplayMode();
		AdjustWinStyle();
        if (FAILED(g_pCDXScreen->ChangeVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0))) {
			//DTRACE("â���ȭ������ �ٲ� �� �����ϴ�.");
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



//========================================================================
// ������ ���ڿ��� �ȼ������� ũ�⸦ ���Ѵ�.
//
// input:
//      szString :  �ȼ� ������ ũ�⸦ ���� ���ڿ�
//      size:       ũ�⸦ ������ ����ü
//========================================================================

int CMainFrame::GDI_GetStringSize(LPCTSTR szString, LPSIZE size)
{
	HDC hDC;
	HFONT hFont, hOldFont;
	
    hDC = g_pCDXScreen->GetBack()->GetDC();

	hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, VARIABLE_PITCH, "����ü");

	hOldFont = (HFONT)SelectObject(hDC, hFont);

	GetTextExtentPoint(hDC, szString, strlen(szString), size);

	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	g_pCDXScreen->GetBack()->ReleaseDC();

	return 1;
}



//========================================================================
// ���� ����Ʈ(����� ����Ʈ, ������ ���� ���� ����Ʈ)�� Ŭ�� ��ġ��
// ���ؼ� �����Ѵ�.
//
// 0�̻� : Ŭ���� ĭ�� �ε���
// -1 : ������ ���� Ŭ���� ���
//========================================================================

int CMainFrame::GetSelUserIdx()
{
    // Ŭ���� ����� ����Ʈ ĭ ��ġ ���Ѵ�.
    if (MouseIn(713, 231, 949, 530)) {
        m_nHighlightBarNum = (l_mY - 231) / 25;    // 25�� ��ĭ�� ���� ����
    }
    else {
        m_nHighlightBarNum = -1;
    }

    return m_nHighlightBarNum;
}



//========================================================================
// ������ ���� �ε����� ���ؼ� �����Ѵ�.
//
// return:
//      0 �̻� : Ŭ���� ĭ�� �ε���
//      -1 : ������ ���� Ŭ���� ���
//========================================================================

#define ROOM_BAR_ONE_LINE_HEIGHT    ( (529 - 199) / MAX_VIEW_WAIT_ROOM )    // 33(�� ���� ������ ���� ũ��)

int CMainFrame::GetSelRoomIdx()
{
    int nStartX = 45;       // �˻縦 ������ ��ǥ
    int nStartY = 192;      // �˻縦 ������ ��ǥ
    int nCnt = 0;

    while ( nCnt < 9 ) {
        if ( MouseIn( nStartX, nStartY, nStartX + 190, nStartY + 101)) {
            return nCnt;
        }

        nCnt++;

        // �Ʒ��� �˻��� ������ ���
        if (( nCnt % 3 ) == 0 ) {
            nStartX = 45;
            nStartY += 111;
        }

        // �����ʿ� �ִ� �� �˻��� ������ ���
        else {
            nStartX += 200;
        }
    }

    // �������� ������ -1 ����
    return -1;
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
    bool bExistPrevSel = false; // ������ ���� ������ �������� �˻�

    // 1. �� ���� ���� ���� �ε������� �����Ѵ�.

    bExistPrevSel = false;

	for ( i = 0; i < MAX_ROOMCNT; i++ ) {
		if ( g_CGameWaitRoomList.m_sRoomStateInfo[i].l_sRoomInfo.nCurCnt > 0 ) {
			nRoomIdx = i + 1;   // �� ���� ���

			// ������ ���õ� �濡 ����� �ִ� ���
			if ( l_nViewRoomMessage == i ) {
				bExistPrevSel = true;
			}
		}
	}

    g_nWaitRoomCnt = nRoomIdx;              // ���� �� ���� ����

    // ���� ��ũ�� �� ��ü �׸� �� ����

    if ( nRoomIdx <= 0 ) {
        m_SCB_WaitRoom->SetElem( 0 );
    }
    else {
        m_SCB_WaitRoom->SetElem(( nRoomIdx / 3 ) + 1 );
    }

    // ���� ������� ���� �������� ���� ���� ���
    // �� ����Ʈ�� ù �������� �̵��Ѵ�.
    if (( m_SCB_WaitRoom->GetStartElem() * 3) >= nRoomIdx ) {
        m_SCB_WaitRoom->StartPage();
    }

    // �� ���� �游��� ��ư�� Ȱ��ȭ ���θ� �����Ѵ�.

    int nRoomNo = m_SCB_WaitRoom->GetStartElem() * 3;

    for ( i = 0; i < 9; i++ ) {
		// ��ư�� ��ġ�� ���� ���� ��� ��ư�� �۵����� �ʰ� �Ѵ�.
        if ( nRoomNo >= MAX_ROOMCNT ) {
            m_MakeRoomBTN[i]->DisableButton();
			m_JoinBTN[i]->DisableButton();
        }

		// ��ư ��ġ�� ���� �ִ� ���
		else {
			// �游��� ��ư ����
			if ( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.nCurCnt <= 0 ) {
				m_MakeRoomBTN[i]->EnableButton();
			}
			else {
				m_MakeRoomBTN[i]->DisableButton();
			}			

			// �����ϱ� ��ư ����
			if ( g_CGameWaitRoomList.m_sRoomStateInfo[nRoomNo].l_sRoomInfo.nCurCnt == 1 ) {
				m_JoinBTN[i]->EnableButton();
			}
			else {
				m_JoinBTN[i]->DisableButton();
			}
			
		}

        nRoomNo++;
    }

    // ������ �������� �ʾҰų�, �����߾����� ���� ���������� �ʱ�ȭ
    if ( !bExistPrevSel ) {
        l_nViewRoomMessage = -1;
    }
}



//========================================================================
// ����� ����Ʈ ��ũ�� �� ����
//========================================================================

void CMainFrame::WaitUserScrollBarRefresh( int nUserCnt )
{
    m_SCB_WaitUser->SetElem( nUserCnt );    // ����� �� ����(��ũ�ѹ�)
    m_SCB_WaitUser->StartPage();
}



//========================================================================
// ���� ���� �ִ� ���� ������ �����Ѵ�.
//
// input:   nRoomNo     = ������ �� ��ȣ
//========================================================================

void CMainFrame::ViewRoomInfoRefresh( ush_int nRoomNo )
{
	if ( nRoomNo < g_nWaitRoomCnt && l_nViewRoomMessage == nRoomNo ) {
        g_pClient->GetRoomInfo( nRoomNo );
    }
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

    if ( m_IME_WaitChat->IsInputState() ) {
        m_IME_WaitChat->PutIMEText(40, 712, RGB(0, 0, 0), RGB(0, 0, 0));
    }

    m_SCB_WaitChat->Draw();
}



//========================================================================
// ���� ä��â�� �� �߰�
//========================================================================

void CMainFrame::AddWaitChatText( char *szText, COLORREF rgbTextColor, int nTextType )
{
    STextInfo _sTextInfo;
    memset( &_sTextInfo, 0x00, sizeof(STextInfo) );
    strcpy( _sTextInfo.szText, szText );
    _sTextInfo.nStyle = nTextType;
    _sTextInfo.rgbColor = rgbTextColor;

    m_pWaitChat->AddText( &_sTextInfo );
    m_SCB_WaitChat->SetElem( m_pWaitChat->GetTextCnt() );
    m_SCB_WaitChat->EndPage();
}



//========================================================================
// ����ȭ�� ä��â�� �� �߰�
//========================================================================

void CMainFrame::AddGameChatText( char *szText, COLORREF rgbTextColor, int nTextType )
{
    STextInfo _sTextInfo;
    memset( &_sTextInfo, 0x00, sizeof(STextInfo) );
    strcpy( _sTextInfo.szText, szText );
    _sTextInfo.nStyle = nTextType;
    _sTextInfo.rgbColor = rgbTextColor;

    m_pGameChat->AddText( &_sTextInfo );
    m_SCB_GameChat->SetElem( m_pGameChat->GetTextCnt() );
    m_SCB_GameChat->EndPage();
}



//========================================================================
// ����ȭ�� ä��â�� �׸���.
//========================================================================

void CMainFrame::RenderGameChat()
{
    int nCnt = 0;                // �� ȭ�� �з��� ����ϱ� ���� ī����
    int nTxtX = 790;             // �ؽ�Ʈ ��� ��ǥ(X ��ǥ�� ����)
    int nTxtY = 0;               // �ؽ�Ʈ ��� ��ǥ(Y ��ǥ�� �� �� ���� ����)
    int nTxtStyle = 0;           // �ؽ�Ʈ ��Ÿ��
    STextInfo *pTextInfo = NULL; // �ؽ�Ʈ �� ���� ����Ű�� ������

    nCnt = 0;
    nTxtY = 409;

    for ( int i = m_SCB_GameChat->GetStartElem(); i < m_pGameChat->GetTextCnt(); i++ ) {
        pTextInfo = m_pGameChat->GetTextInfo(i);

        if (pTextInfo != NULL) {
            nTxtStyle = ( pTextInfo->nStyle == 0 ) ? FW_NORMAL : FW_BOLD;
            DrawText( pTextInfo->szText, nTxtX, nTxtY, pTextInfo->rgbColor, nTxtStyle, TA_LEFT );
            nTxtY += 14;

            nCnt++;
            if (nCnt == MAX_VIEW_GAME_CHAT) break;
        }
    }

    if ( m_IME_GameChat->IsInputState() ) {
        m_IME_GameChat->PutIMEText(790, 543, RGB(255, 255, 255), RGB(255, 255, 255));
    }

    m_SCB_GameChat->Draw();
}



//========================================================================
// ���� �̹����� ����Ѵ�. 
//
// �̹����� ���� : 0123456789+-
// ��� ������ǥ�� ������ ������ ��
// nMark ���� 0�̸� ��ȣ ��� ����, 0���� ū ���� +��ȣ, 0���� ������ -��ȣ ���
//========================================================================

void CMainFrame::DrawMoney( CDXSprite *pSprite, int nX, int nY, BigInt nMoney, int nMark )
{
    if ( pSprite == NULL ) return;

    int nNum = 0;
    int nNumX = nX;
    int nWidth = pSprite->GetTile()->GetBlockWidth();
    bool bSign = true;              // ��ȣ: true = plus, false = minus
    BigInt nTempMoney = nMoney;

    if ( nMoney < 0 ) {
        bSign = false;      // minus ��ȣ
        nTempMoney = -nTempMoney;
    }
    else {
        if ( nMark < 0 ) {
            bSign = false;  // minus ��ȣ
        }
        else {
            bSign = true;   // plus ��ȣ
        }
    }

    if ( nMoney == 0 ) {
		PutSprite( pSprite, nNumX, nY, 0 );
    }
    else {
        while ( nTempMoney > 0 ) {
            nNum = (int)( nTempMoney % 10 );
            PutSprite( pSprite, nNumX, nY, nNum );
            nTempMoney /= 10;
            nNumX -= nWidth;
        }

        // ��ȣ ��� : 0�̸� ��ȣ ��� ����
        if ( nMark != 0 ) {
            if ( bSign == true ) {
                PutSprite( pSprite, nNumX, nY, 10 );    // + ��ȣ
            }
            else {
                PutSprite( pSprite, nNumX, nY, 11 );    // - ��ȣ
            }
        }
    }
}



//========================================================================
// ���ڸ� ����Ѵ�.
//
// ���� �̹����� ����:
//      0 1 2 3 4 5 6 7 8 9 + - *
//
// input:
//      pSprite:    ����� ���� �̹���
//      nX, nY:     ����� ��ǥ. ��� ������ ���ڿ��� ������ ��
//      nNum:       ����� ����
//      nMarkType:  ����� ��ȣ(0 = ��¾���, 1 = +��ȣ, 2 = -��ȣ, 3 = * ��ȣ)
//                  MARK_NONE = ��¾���
//                  MARK_PLUS = + ��ȣ
//                  MARK_NINUS = - ��ȣ
//                  MARK_MULTIPLY = * ��ȣ
//      nAlign:     ���� ���
//                  TA_LEFT = ���� ����
//                  TA_CENTER = ��� ����
//                  TA_RIGHT = ������ ����
//========================================================================

void CMainFrame::DrawNumber( CDXSprite *pSprite, int nX, int nY, int nNum, int nMarkType, int nAlign )
{
    if ( pSprite == NULL ) return;

    int nIdx = 0;
    int nNums[32];
    int nTempNum = nNum;
    int nNumX = 0;
    int nWidth = pSprite->GetTile()->GetBlockWidth();

    memset( nNums, 0x00, sizeof( nNums ));

    if ( nTempNum < 0 ) {
        nTempNum = -nTempNum;   // -���̸� ����� �ٲپ� ���
    }

    // ���� ���ϱ�

    do {
        nNums[nIdx++] = (int)(nTempNum % 10);
        nTempNum /= 10;
    } while(nTempNum > 0);

    // ���� ��Ŀ� ���� ó�� ��� ��ġ ����

    switch ( nAlign ) {
        case TA_LEFT:
            nNumX = nX;
            break;

        case TA_CENTER:
            if ( nMarkType != MARK_NONE )
                nNumX = nX - ((( nIdx + 1 ) * nWidth ) >> 1 );
            else
                nNumX = nX - (( nIdx * nWidth ) >> 1 );
            break;

        case TA_RIGHT:
            if ( nMarkType != MARK_NONE )
                nNumX = nX - (( nIdx + 1 ) * nWidth );
            else
                nNumX = nX - ( nIdx * nWidth );
            break;
    }

    // ��ȣ ǥ��

    switch ( nMarkType ) {
        case MARK_NONE:
            break;

        case MARK_PLUS:
            PutSprite( pSprite, nNumX, nY, 10 );
            nNumX += nWidth;
            break;

        case MARK_MINUS:
            PutSprite( pSprite, nNumX, nY, 11 );
            nNumX += nWidth;
            break;

        case MARK_MULTIPLY:
            PutSprite( pSprite, nNumX, nY, 12 );
            nNumX += nWidth;
            break;
    }

    // ���ڸ� �׸���.

    for ( int i = nIdx - 1; i >= 0; i-- ) {
        PutSprite( pSprite, nNumX, nY, nNums[i] );
        nNumX += nWidth;
    }
}



//========================================================================
// �̼� ���� ��� ��ġ�� "XX�� �̼Ǽ���!" �޽����� ����Ѵ�.
//========================================================================

void CMainFrame::DrawMissionCompleteUser( char *szNickname )
{
    char szStr[256];

	if ( strlen( szNickname ) > 13 ) {
		memset( g_szTempStr, 0x00, sizeof(g_szTempStr) );
		memcpy( g_szTempStr, szNickname, 13 );
		strcat( g_szTempStr, "..." );
	}
	else {
		strcpy( g_szTempStr, szNickname );
	}

	sprintf( szStr, "%s�� �̼Ǽ���!", g_szTempStr );

    DrawText( szStr,
              m_sMissionDlgInfo.nDlgX + 119,
              m_sMissionDlgInfo.nDlgY + 139,
              RGB( 255, 255, 255 ),
              FW_NORMAL,
              TA_CENTER );
}



// [alpha]
//========================================================================
// ������ ��ȭ���ڸ� �׸���.
//========================================================================

void CMainFrame::DrawTransDialog( int nDlgType, int nX, int nY, int nAlpha, int nFrame )
{
    CDXSurface *pSrcSurface = g_pCDXScreen->GetBack();

    // ������ ��ȭ���ڰ� ó�� ���̴� �����̸� �����Ѵ�.
    // ��ȭ������ ������ �ٲ���ų�, ó�� ��µǴ� �����̸� �̹��� �ٽ� ����.
    if (( nDlgType != m_sPrevDlgInfo.nPrevDlgType ) ||
        ( g_bRefreshTransDlg ) ||       // ham 2005.11.10
        ( m_sPrevDlgInfo.bPrevShowDialog == FALSE && l_bShowDialog == TRUE ))
    {
        int nDlgWidth = l_SPR_Dialog->GetTile()->GetBlockWidth();
        int nDlgHeight = l_SPR_Dialog->GetTile()->GetBlockHeight();
        RECT sSrcRect;

        // ���� ���� �׷����� ��ȭ���� Ÿ�� ����
        m_sPrevDlgInfo.nPrevDlgType = nDlgType;

        SetRect( &sSrcRect, nX, nY, nX + nDlgWidth, nY + nDlgHeight );

        // ������ ��ġ�� ������ũ�� �̹����� �ӽ� ��ҿ� �׸���.
        pSrcSurface->DrawBlk( m_SFC_DialogBack, 0, 0, &sSrcRect );

        // ��ȭ���ڸ� �ӽ� ��ҿ� �������ϰ� �׸���.
        SetRect( &sSrcRect, 0, 0, nDlgWidth, nDlgHeight );
        l_SPR_Dialog->GetTile()->DrawTransAlpha( m_SFC_DialogBack, 0, 0, &sSrcRect, nAlpha );

        g_bRefreshTransDlg = false;
    }

    m_SFC_DialogBack->DrawBlk( pSrcSurface, nX, nY );
}




// [zoom]
//========================================================================
// ȭ�� �̹����� Ȯ���ؼ� ����� �� �ʿ��� ���̺��� �����.
// �̹��� Ȯ��� ���������� ��Ҵ� �������� �ʴ´�.
//
// input:
//      nZoomWidth:     Ȯ���� �̹����� ���� ũ��
//      nZoomHeight:    Ȯ���� �̹����� ���� ũ��
//========================================================================

void CMainFrame::MakeCardZoomTable( sCardZoomTable **pZoomTable, int nZoomWidth, int nZoomHeight )
{
    int i;
    int nSrcPixelIdx;   // ���� �̹����� �ȼ� ��ġ �����ϴµ� ���Ǵ� �ε���
    int nDestPixelIdx;  // Ȯ�� �̹����� �ȼ� ��ġ �����ϴµ� ���Ǵ� �ε���
    float fRepCnt;      // �ݺ�Ƚ�� ��꿡 ���Ǵ� ����
    float fHRate;       // Ȯ��Ǵ� ���� ũ�� ����
    float fVRate;       // Ȯ��Ǵ� ���� ũ�� ����
    sCardZoomTable *pTempZoomTable = NULL;

    //------------------------
    // Ȯ�� ���̺� ����ü ����
    //------------------------

    pTempZoomTable = new sCardZoomTable;
    memset( pTempZoomTable, 0x00, sizeof( sCardZoomTable ));

    //---------------------------------------------------------------
    // Ȯ���� �̹����� �� ���� Ȯ��� �̹����� �� ���� ũ�� ���� ����
    //---------------------------------------------------------------

    if ( nZoomWidth <= HWATU_WIDTH ) {
        fHRate = 1.0;
        pTempZoomTable->nZoomWidth = HWATU_WIDTH;
    }
    else {
        fHRate = (float)nZoomWidth / (float)HWATU_WIDTH;
        pTempZoomTable->nZoomWidth = nZoomWidth;
    }

    //---------------------------------------------------------------
    // Ȯ���� �̹����� �� �ٴ� Ȯ��� �̹����� �� ���� ũ�� ���� ����
    //---------------------------------------------------------------

    if ( nZoomHeight <= HWATU_HEIGHT ) {
        fVRate = 1.0;
        pTempZoomTable->nZoomHeight = HWATU_HEIGHT;
    }
    else {
        fVRate = (float)nZoomHeight / (float)HWATU_HEIGHT;
        pTempZoomTable->nZoomHeight = nZoomHeight;
    }

    //----------------------------------------
    // Ȯ�� ���̺��� �����ϰ� ���� �ʱ�ȭ�Ѵ�.
    //----------------------------------------

    pTempZoomTable->nRepHTable = new int[pTempZoomTable->nZoomWidth];
    pTempZoomTable->nRepVTable = new int[pTempZoomTable->nZoomHeight];

    memset( pTempZoomTable->nRepHTable, 0x00, sizeof( pTempZoomTable->nRepHTable ));
    memset( pTempZoomTable->nRepVTable, 0x00, sizeof( pTempZoomTable->nRepVTable ));

    //----------------------------------------------------------------
    // Ȯ�� �� �̹����� �� ���� Ȯ��� ����� ��µǾ�� �ϴ����� ���
    //----------------------------------------------------------------

    fRepCnt = fHRate;
    nSrcPixelIdx = 0;
    nDestPixelIdx = 0;
    i = 0;

    while ( nDestPixelIdx < pTempZoomTable->nZoomWidth ) {
        i = (int)fRepCnt;

        while ( i > 0 ) {
            pTempZoomTable->nRepHTable[nDestPixelIdx] = nSrcPixelIdx;      // ���� �̹����� �ȼ� ��ġ(�ε���)�� Ȯ�� ���̺� �ִ´�.
            nDestPixelIdx++;
            i--;
        }

        fRepCnt = ( fRepCnt - ((float)((int)(fRepCnt))) ) + fHRate; // �Ҽ��� ���ϴ� �������Ѽ� ���� ���� �ݺ�Ƚ�� ��꿡 ���
        nSrcPixelIdx++;
    }

    pTempZoomTable->nRepHTable[ pTempZoomTable->nZoomWidth - 1 ] = HWATU_WIDTH - 1;

    //--------------------------------------------------------------
    // Ȯ�� �� �̹����� �� �ٴ� Ȯ��� ����� ����ؾ� �ϴ����� ���
    //--------------------------------------------------------------
    
    fRepCnt = fVRate;
    nSrcPixelIdx = 0;
    nDestPixelIdx = 0;
    i = 0;

    while ( nDestPixelIdx < pTempZoomTable->nZoomHeight ) {
        i = (int)fRepCnt;

        while ( i > 0 ) {
            pTempZoomTable->nRepVTable[nDestPixelIdx] = nSrcPixelIdx;      // ���� �̹����� �ȼ� ��ġ(�ε���)�� Ȯ�� ���̺� �ִ´�.
            nDestPixelIdx++;
            i--;
        }

        fRepCnt = ( fRepCnt - ((float)((int)(fRepCnt))) ) + fVRate; // �Ҽ��� ���ϴ� �������Ѽ� ���� ���� �ݺ�Ƚ�� ��꿡 ���
        nSrcPixelIdx++;
    }

    pTempZoomTable->nRepVTable[ pTempZoomTable->nZoomHeight - 1 ] = HWATU_HEIGHT - 1;

    //---------------------------------------------
    // ������ Ȯ�� ���̺� ����ü�� �����͸� �ѱ��.
    //---------------------------------------------

    *pZoomTable = pTempZoomTable;
}



// [zoom]

//========================================================================
// ȭ�� �̹����� Ȯ���ؼ� ������ũ���� �׸���.
// ���ǽ��� �����ͷ� ���� �����ؼ� �׸���.
//========================================================================

void CMainFrame::PutHwatuZoom16( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo )
{
    static CDX_DDSURFACEDESC ddsdVid;
    static CDX_DDSURFACEDESC ddsdImg;

    int i, j;
    int nOrigX;
    int nOrigY;
    int nDrawX;
    int nDrawY;
    int nDrawWidth;         // Ŭ���ε� ���� �׷����� ���� ũ��
    int nDrawHeight;        // Ŭ���ε� ���� �׷����� ���� ũ��
    CDX_LPDIRECTDRAWSURFACE pSrc = NULL;
    CDX_LPDIRECTDRAWSURFACE pDest = NULL;

    pSrc = l_SPR_Hwatu->GetTile()->GetDDS();    // ȭ�� �̹����� Surface�� ��´�.
    pDest = g_pCDXScreen->GetBack()->GetDDS();  // ������ũ���� Surface�� ��´�.

    memset( &ddsdImg, 0, sizeof( ddsdImg ));
    memset( &ddsdVid, 0, sizeof( ddsdVid ));
    ddsdImg.dwSize = sizeof( ddsdImg );
    ddsdVid.dwSize = sizeof( ddsdVid );

    pSrc->Lock( NULL, &ddsdImg, DDLOCK_SURFACEMEMORYPTR | DDLOCK_READONLY | DDLOCK_WAIT, NULL );
    pDest->Lock( NULL, &ddsdVid, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );

    int nImgPitch = ddsdImg.lPitch >> 1;    // ����Ʈ ������ lPitch�� DWORD ������ �ٲ۴�.
    int nVidPitch = ddsdVid.lPitch >> 1;    // ����Ʈ ������ lPitch�� DWORD ������ �ٲ۴�.
    USHORT *pImgBuff = (USHORT *)ddsdImg.lpSurface;
    USHORT *pVidBuff = (USHORT *)ddsdVid.lpSurface;

    pImgBuff += ( HWATU_WIDTH * ( nHwatuNo % 4 )) + ( HWATU_HEIGHT * nImgPitch * ( nHwatuNo >> 2 ));

    //-------------------------------------------------
    // ȭ�鿡�� �߸��� �κ��� ������ ���� ��ºκ� ����
    //-------------------------------------------------

    if ( nX < 0 ) {
        nDrawX = -nX;                               // �߸���ŭ ��µ�ġ�� ���������� �̵�
        nDrawWidth = pZoomTable->nZoomWidth + nX;   // �߸���ŭ ����ũ�⸦ ���ش�.
    }
    else {
        nDrawX = 0;
        pVidBuff += nX;

        if (( nX + pZoomTable->nZoomWidth ) >= SCREEN_WIDTH ) {
            nDrawWidth = SCREEN_WIDTH - nX;
        }
        else {
            nDrawWidth = pZoomTable->nZoomWidth;
        }
    }

    if ( nY < 0 ) {
        nDrawY = -nY;
        nDrawHeight = pZoomTable->nZoomHeight + nY;
    }
    else {
        nDrawY = 0;
        pVidBuff += ( nY * nVidPitch );

        if (( nY + pZoomTable->nZoomHeight ) >= SCREEN_HEIGHT ) {
            nDrawHeight = SCREEN_HEIGHT - nY;
        }
        else {
            nDrawHeight = pZoomTable->nZoomHeight;
        }
    }

    // ������ ȭ�鿡 �׸���.

    for ( i = 0; i < nDrawHeight; i++ ) {
        nOrigY = pZoomTable->nRepVTable[ nDrawY + i ];

        for ( j = 0; j < nDrawWidth; j++ ) {
            nOrigX = pZoomTable->nRepHTable[ nDrawX + j ];
            pVidBuff[j] = pImgBuff[ nOrigX ];
        }

        pVidBuff += nVidPitch;

        if ( i > 0 && ( nOrigY != pZoomTable->nRepVTable[ nDrawY + i - 1] )) {
            pImgBuff += nImgPitch;
        }
    }

    pSrc->Unlock( NULL );
    pDest->Unlock( NULL );
}




// [zoom]

//========================================================================
// ȭ�� �̹����� Ȯ���ؼ� ������ũ���� �׸���.
// ���ǽ��� �����ͷ� ���� �����ؼ� �׸���.
//========================================================================

void CMainFrame::PutHwatuZoom32( sCardZoomTable *pZoomTable, int nX, int nY, int nHwatuNo )
{
    static CDX_DDSURFACEDESC ddsdVid;
    static CDX_DDSURFACEDESC ddsdImg;

    int i, j;
    int nOrigX;
    int nOrigY;
    int nDrawX;
    int nDrawY;
    int nDrawWidth;         // Ŭ���ε� ���� �׷����� ���� ũ��
    int nDrawHeight;        // Ŭ���ε� ���� �׷����� ���� ũ��
    CDX_LPDIRECTDRAWSURFACE pSrc = NULL;
    CDX_LPDIRECTDRAWSURFACE pDest = NULL;

    pSrc = l_SPR_Hwatu->GetTile()->GetDDS();    // ȭ�� �̹����� Surface�� ��´�.
    pDest = g_pCDXScreen->GetBack()->GetDDS();  // ������ũ���� Surface�� ��´�.

    memset( &ddsdImg, 0, sizeof( ddsdImg ));
    memset( &ddsdVid, 0, sizeof( ddsdVid ));
    ddsdImg.dwSize = sizeof( ddsdImg );
    ddsdVid.dwSize = sizeof( ddsdVid );

    pSrc->Lock( NULL, &ddsdImg, DDLOCK_SURFACEMEMORYPTR | DDLOCK_READONLY | DDLOCK_WAIT, NULL );
    pDest->Lock( NULL, &ddsdVid, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );

    int nImgPitch = ddsdImg.lPitch >> 2;    // ����Ʈ ������ lPitch�� DWORD ������ �ٲ۴�.
    int nVidPitch = ddsdVid.lPitch >> 2;    // ����Ʈ ������ lPitch�� DWORD ������ �ٲ۴�.
    DWORD *pImgBuff = (DWORD *)ddsdImg.lpSurface;
    DWORD *pVidBuff = (DWORD *)ddsdVid.lpSurface;

    pImgBuff += ( HWATU_WIDTH * ( nHwatuNo % 4 )) + ( HWATU_HEIGHT * nImgPitch * ( nHwatuNo >> 2 ));

    //-------------------------------------------------
    // ȭ�鿡�� �߸��� �κ��� ������ ���� ��ºκ� ����
    //-------------------------------------------------

    if ( nX < 0 ) {
        nDrawX = -nX;                               // �߸���ŭ ��µ�ġ�� ���������� �̵�
        nDrawWidth = pZoomTable->nZoomWidth + nX;   // �߸���ŭ ����ũ�⸦ ���ش�.
    }
    else {
        nDrawX = 0;
        pVidBuff += nX;

        if (( nX + pZoomTable->nZoomWidth ) >= SCREEN_WIDTH ) {
            nDrawWidth = SCREEN_WIDTH - nX;
        }
        else {
            nDrawWidth = pZoomTable->nZoomWidth;
        }
    }

    if ( nY < 0 ) {
        nDrawY = -nY;
        nDrawHeight = pZoomTable->nZoomHeight + nY;
    }
    else {
        nDrawY = 0;
        pVidBuff += ( nY * nVidPitch );

        if (( nY + pZoomTable->nZoomHeight ) >= SCREEN_HEIGHT ) {
            nDrawHeight = SCREEN_HEIGHT - nY;
        }
        else {
            nDrawHeight = pZoomTable->nZoomHeight;
        }
    }

    // ������ ȭ�鿡 �׸���.

    for ( i = 0; i < nDrawHeight; i++ ) {
        nOrigY = pZoomTable->nRepVTable[ nDrawY + i ];

        for ( j = 0; j < nDrawWidth; j++ ) {
            nOrigX = pZoomTable->nRepHTable[ nDrawX + j ];
            pVidBuff[j] = pImgBuff[ nOrigX ];
        }

        pVidBuff += nVidPitch;

        if ( i > 0 && ( nOrigY != pZoomTable->nRepVTable[ nDrawY + i - 1] )) {
            pImgBuff += nImgPitch;
        }
    }

    pSrc->Unlock( NULL );
    pDest->Unlock( NULL );
}



//========================================================================
// ���带 �ε��Ѵ�.
//========================================================================

void CMainFrame::SoundLoading() 
{
    // ȿ���� ���丮
    static char *szEffSndDir = ".\\Sound\\Effect\\";

    // ���� ���丮
    static char *szVoiceDir[4] = 
    {
        ".\\Sound\\����\\",     // [0] ����1 - ����
        ".\\Sound\\ȫ���\\",   // [1] ����1 - ȫ���
        ".\\Sound\\Ÿ¥\\",     // [2] ����2 - Ÿ¥
        ".\\Sound\\�Ϳ���\\"    // [3] ����2 - �Ϳ���
    };

    static char *szVoiceFileName[33] = 
    {
        //[0]�ʴ�            [1]û��             [2]ȫ��
        "chodan.wav",        "chungdan.wav",     "hongdan.wav",

        //[3]����          [4]���Ǿ�           [5]���Ǿ�
        "godori.wav",        "gsgetonepee.wav",  "gsgettwopee.wav",

        //[6]���Ǿ�          [7]���Ǿ�           [8]�ټ��Ǿ�
        "gsgetthreepee.wav", "gsgetfourpee.wav", "gsgetfivepee.wav",

        //[9]����            [10]                [11]1��
	    "gsstop.wav",        "gsmade.wav",       "gs1go.wav",

        //[12]2��            [13]3��             [14]4��
        "gs2go.wav",         "gs3go.wav",        "gs4go.wav",

        //[15]5��            [16]6��             [17]7��
        "gs5go.wav",         "gs6go.wav",        "gs7go.wav",

        //[18]8��            [19]9��             [20]���
	    "gs8go.wav",         "gs9go.wav",        "gsshake.wav",

        //[21]�մ�           [22]��              [23]������
        "gsssam.wav",        "jjok.wav",         "nagari.wav",

        //[24]����           [25]����            [26]3��
        "chongtong.wav",     "dokbak.wav",       "3kwang.wav",

        //[27]4��            [28]5��             [29]���ʽ���
        "4kwang.wav",        "5kwang.wav",       "gsbonus.wav",

        //[30]�Ǿ�           [31]                [32]����
        "pansl.wav",         "afterjjok.wav",    "ddadak.wav"
    };

    int nStartSndIdx = 0;       // �� ������ ������ ù��° �ε��� ��ġ
    char szSndPath[512];

    //------------
    // �迭 �ʱ�ȭ
    //------------

    memset( szSndPath, 0x00, sizeof( szSndPath ));
    memset( g_Sound,   0x00, sizeof( g_Sound ));

    //------------
    // ȿ���� �ε�
    //------------

    strcpy(szSndPath, ".\\Sound\\Effect\\");

    g_Sound[0]  = LoadSndFile( szSndPath, "button_click.wav",       2 );
	g_Sound[1]  = LoadSndFile( szSndPath, "room_Join.wav",          2 );
	g_Sound[2]  = LoadSndFile( szSndPath, "room_Exit.wav",          2 );
	g_Sound[3]  = LoadSndFile( szSndPath, "gameStart.wav",          2 );
	g_Sound[4]  = LoadSndFile( szSndPath, "game_clear.wav",         2 );
	g_Sound[5]  = LoadSndFile( szSndPath, "hwatoo_click.wav",       2 );
	g_Sound[6]  = LoadSndFile( szSndPath, "hwatoo_put.wav",         2 );
	g_Sound[7]  = LoadSndFile( szSndPath, "hwatoo_match.wav",       2 );
	g_Sound[8]  = LoadSndFile( szSndPath, "hwatoo_move.wav",        2 );
	g_Sound[9]  = LoadSndFile( szSndPath, "hwatoo_share.wav",       2 );
	g_Sound[10] = LoadSndFile( szSndPath, "gsmiss.wav",             2 );
	g_Sound[11] = LoadSndFile( szSndPath, "my_order.wav",           2 );
	g_Sound[12] = LoadSndFile( szSndPath, "game_win.wav",           2 );
	g_Sound[13] = LoadSndFile( szSndPath, "game_lose.wav",          2 );
	g_Sound[14] = LoadSndFile( szSndPath, "mission.wav",            2 );
	g_Sound[15] = LoadSndFile( szSndPath, "mission_complete.wav",   2 );
	g_Sound[16] = LoadSndFile( szSndPath, "popup.wav",              2 );
	g_Sound[17] = LoadSndFile( szSndPath, "count.wav",              2 ); 
	g_Sound[18] = LoadSndFile( szSndPath, "gscardup.wav",           2 ); 
	g_Sound[19] = LoadSndFile( szSndPath, "hwatoo_match_1goaf.wav", 2 ); 
	g_Sound[20] = LoadSndFile( szSndPath, "hwatoo_match_2goaf.wav", 2 ); 
	g_Sound[21] = LoadSndFile( szSndPath, "hwatoo_match_3goaf.wav", 2 ); 
	g_Sound[22] = LoadSndFile( szSndPath, "bomb.wav",               2 );
	g_Sound[23] = LoadSndFile( szSndPath, "result_dojang.wav",      2 );
	g_Sound[24] = LoadSndFile( szSndPath, "hwatoo_sort.wav",        2 );


    nStartSndIdx = 50;  // ������ �ε��� 50�� ���� �����

    for ( int i = 0; i < 4; i++ ) {             // 4���� ������ ������ �ִ�.
        strcpy( szSndPath, szVoiceDir[i] );
        
        for ( int j = 0; j < 33; j++ ) {        // �� �������� 33���� ���� ������ �ִ�.
            g_Sound[nStartSndIdx + j] = LoadSndFile( szSndPath, szVoiceFileName[j], 2 );
        }

        nStartSndIdx += 50;                     // �� Ÿ��(ȿ����, �� ����Ÿ��)���� 50���� �ε��� ���������� ����Ѵ�.
    }
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
// ȭ���� ��ȯ������ ���� ǥ�� �ı��� �ƹ�Ÿ �̹����� �����Ѵ�.
// ���ϸ��̼� GIF�� ��� �ڵ����� ��� �׷��ֹǷ� ���ϸ��̼� GIF��
// �ƴ� ��쿡�� �������ָ� �ȴ�.
//========================================================================

void CMainFrame::RecoveryAvatar()
{
    // ���� �ƹ�Ÿ ����
    if ( l_SPR_GifWaitAvatar != NULL ) {
        if ( m_PrevAvaImg != NULL ) {
            if ( !m_PrevAvaImg->IsAnimatedGIF() ) {
                m_PrevAvaImg->InitAnimation( g_hWnd, CPoint(0, 0) );
            }
        }
    }

    // ���ӹ� �ƹ�Ÿ 0 ����
    if ( l_SPR_GifAvatar[0] != NULL ) {
        if ( m_AvaImg0 != NULL ) {
            if ( !m_AvaImg0->IsAnimatedGIF() ) {
                m_AvaImg0->InitAnimation( g_hWnd, CPoint(0, 0) );
            }
        }
    }

    // ���ӹ� �ƹ�Ÿ 1 ����
    if ( l_SPR_GifAvatar[1] != NULL ) {
        if ( m_AvaImg1 != NULL ) {
            if ( !m_AvaImg1->IsAnimatedGIF() ) {
                m_AvaImg1->InitAnimation( g_hWnd, CPoint(0, 0) );
            }
        }
    }
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
    int nOldCardNo = 0;
    int nNewCardNo = 0;
    int nParamNum = 0;
    memset(szCommBuf, 0x00, sizeof(szCommBuf));

    nParamNum = sscanf(szText, "%s %d %d", szCommBuf, &nOldCardNo, &nNewCardNo);

    // EOF(-1)�� �ƴϰ� 3�� �� ����� �Ľ��� ���
    if (nParamNum >= 3) {

        if ((strcmp(szCommBuf, "/C") == 0) || (strcmp(szCommBuf, "/c") == 0))
        { 
            // ���⼭ �ٲ��ڸ� ��ȣ�� ��ī�� ��ȣ ����
            // ������ ���̸� ������ ī�� �ٲٱ� ��Ŷ�� ������.
            DTRACE2("��ɾ��.... %s, %d, %d", szCommBuf, nOldCardNo, nNewCardNo);

            SendChangeUserCardInfo( nOldCardNo, nNewCardNo );
                     
            return TRUE;
        }
        // "/"�� �����ϴ� ���ڿ��� ä�� ó������ �ʴ´�.
        else if ( strlen(szCommBuf) >= 1 && szCommBuf[0] == '/' ) {
            AddGameChatText( "�� �߸��� ��ɾ��Դϴ�." , RGB( 255, 255, 255 ));
            return TRUE;
        }
    }
    else if ( nParamNum >= 1 ) {
        // �߸��� ��ɾ�
        if ( strlen(szCommBuf) >= 1 ) {
            if ( szCommBuf[0] == '/' ) {
                AddGameChatText( "�� �߸��� ��ɾ��Դϴ�." , RGB( 255, 255, 255 ));
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
// ������ �ٲ� ī�� ������ ������.
//
// input:
//      nCardNo1 : �ٲ� �� ī�� ��ȣ(�ڽ��� ī��)
//      nCardNo2 : ���� ���� ī�� ��ȣ(ī������ ī��)
//========================================================================

void CMainFrame::SendChangeUserCardInfo(int nCardNo1, int nCardNo2)
{
    int i = 0;
    int nCard1Idx = 0;
    int nCard2Idx = 0;
    bool bExistPackCard = false;
    bool bExistUserCard = false;

    DTRACE2 (" :::::ī���ȣ : %d, %d", nCardNo1, nCardNo2 );

    // ��� �ϳ��� ī�� ��ȣ�� �߸��Ǿ����� ī�带 �ٲ��� �ʴ´�.
    if ( nCardNo1 < 0 || nCardNo1 > 50 || nCardNo2 < 0 || nCardNo2 > 50 ) {
        AddGameChatText( "�� �߸��� ��ɾ��Դϴ�." , RGB( 255, 255, 255 ));
        return;
    }

    // ī�� �ѿ� �ٲ� ī�尡 �ִ��� �˻�
	for ( i = g_pGoStop->m_Hwatu.m_pCardPack->m_nReadNo; i < g_pGoStop->m_Hwatu.m_pCardPack->m_nCardCnt; i++ ) {
		if ( g_pGoStop->m_Hwatu.m_pCardPack->m_pCardPack[i] == nCardNo2 ) {
            nCard2Idx = i;
			bExistPackCard = true;
			break;
		}
	}

    // �ڽſ��� �ٲٱ� ���ϴ� ī�尡 �ִ��� �˻�
	for ( i = 0; i < g_pGoStop->m_pFloor.m_pGamer[0].m_nCardCnt; i++ ) {
		if ( g_pGoStop->m_pFloor.m_pGamer[0].m_bCard[i] == nCardNo1 ) {
            nCard1Idx = i;
			bExistUserCard = true;
			break;
		}
	}

    // ī���ѿ� ī�尡 �ִ� ���
    if ( bExistPackCard ) {
        // ī���ѿ� ī�尡 �ְ� �ڽſ��� �ٲ� ī�尡 �ִ� ���
        if ( bExistUserCard ) {
            long nTempX = 0;
            long nTempY = 0;
            
            // �ڽ��� ī�带 ī������ ī��� �ٲ۴�.
            g_pGoStop->m_pFloor.m_pGamer[0].m_bCard[nCard1Idx] = nCardNo2;

            // ī������ ī�带 �ڽ��� ī��� �ٲ۴�.
            g_pGoStop->m_Hwatu.m_pCardPack->m_pCardPack[nCard2Idx] = nCardNo1;

            // �ٲ� ī���� ��ġ ��ȯ
            nTempX = l_CardPos[nCardNo2].Pos.x;
            nTempY = l_CardPos[nCardNo2].Pos.y;
            l_CardPos[nCardNo2].Pos.x = l_CardPos[nCardNo1].Pos.x;
            l_CardPos[nCardNo2].Pos.y = l_CardPos[nCardNo1].Pos.y;
            l_CardPos[nCardNo1].Pos.x = nTempX;
            l_CardPos[nCardNo1].Pos.y = nTempY;

            // �ٲ� ī�� ������ ������ ������.
            sUserChange sTempUserChange;
            memset( &sTempUserChange, 0x00, sizeof( sTempUserChange ));
            sTempUserChange.bUerCardNo = (BYTE)nCardNo1;
            sTempUserChange.bChangeCardNo = (BYTE)nCardNo2;
            g_pClient->UserChange( &sTempUserChange );
        }
        // ī���ѿ� ī�尡 ������ �ڽſ��� ���� ī�� ��ȣ�� ������ ���
        else {
            AddGameChatText( "�� ������ ���� ���� ���Դϴ�." , RGB( 255, 255, 255 ));
        }
    }
    // ī���ѿ� ī�尡 ���� ���
    else {
        // �ڽ��� ī�带 ������ �ְ� ī���ѿ� �ٲ� ī�尡 ���� ���
        if ( bExistUserCard ) {
            AddGameChatText( "�� �ٲ� �� �ִ� �а� �����ϴ�." , RGB( 255, 255, 255 ));
        }
        // �ڽſ��� ī�尡 ����, ī���ѿ� �ٲ� ī�嵵 ���� ���
        else {
            AddGameChatText( "�� ������ ��ȣ�� ��ġ�ϴ� �а� �����ϴ�." , RGB( 255, 255, 255 ));
        }
    }
}

#endif
