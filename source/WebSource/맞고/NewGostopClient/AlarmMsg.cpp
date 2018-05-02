#include "stdafx.h"
#include "MainFrm.h"
#include "AlarmMsg.h"


extern CDXScreen *g_pCDXScreen;
extern CMainFrame *g_pCMainFrame;
extern HWND g_hWnd;



GAMEMESSAGE::GAMEMESSAGE()
{
	memset( szText , 0 , sizeof( szText ) );
	memset( szText2 , 0 , sizeof( szText2 ) );

	bShowCheck = FALSE;
	m_bCloseGame = false;
}



void GAMEMESSAGE::Draw()
{
    g_pCMainFrame->PutSprite( g_pCMainFrame->l_SPR_Message, ALARM_MSG_POS_X, ALARM_MSG_POS_Y );

	g_pCMainFrame->l_MessageOKBTN->SetPos( ALARM_MSG_POS_X + 109, ALARM_MSG_POS_Y + 176 );
	g_pCMainFrame->l_MessageOKBTN->DrawButton();
	
    g_pCMainFrame->DrawText(szText,  ALARM_MSG_POS_X + 163, ALARM_MSG_POS_Y +  92, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	g_pCMainFrame->DrawText(szText2, ALARM_MSG_POS_X + 163, ALARM_MSG_POS_Y + 120, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
}



void GAMEMESSAGE::PutMessage( char *szString , char *szString2, bool bCloseGame )
{
	g_pCMainFrame->l_bShowMakeRoomDlg = FALSE;	
	g_pCMainFrame->l_bShowMessageBox = FALSE;

    g_pCMainFrame->m_IME_InputPass->InputEnd();
    g_pCMainFrame->m_IME_InputRoomTitle->InputEnd();
    g_pCMainFrame->m_IME_Memo->InputEnd();
	
	bShowCheck = TRUE;

	strcpy( szText , szString );
	strcpy( szText2 , szString2 );

	m_bCloseGame = bCloseGame;
}



BOOL GAMEMESSAGE::GetbShowCheck( void ) 
{
	return bShowCheck;
}



void GAMEMESSAGE::SetbShowCheck( BOOL Check )
{
	bShowCheck = Check;
}




