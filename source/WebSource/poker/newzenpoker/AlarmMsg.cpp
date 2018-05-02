
//#include <cdx.h>
//#include <cdxsprite.h>
//#include <cdxtile.h>
//#include "GlovalVar.h"

//extern CDirectX g_dx2d;
//extern CDXSprite *l_SPR_Message; // 메세지 박스 그림
//extern CDXButton *l_MessageOKBTN;

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
    g_pCMainFrame->PutSprite( g_pCMainFrame->m_SPR_Message, ALARM_MSG_POS_X, ALARM_MSG_POS_Y );

	g_pCMainFrame->m_MessageOKBTN->SetPos( ALARM_MSG_POS_X + 109, ALARM_MSG_POS_Y + 176 );
	g_pCMainFrame->m_MessageOKBTN->DrawButton();
	
    g_pCMainFrame->DrawText(szText,  ALARM_MSG_POS_X + 163, ALARM_MSG_POS_Y +  92, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
	g_pCMainFrame->DrawText(szText2, ALARM_MSG_POS_X + 163, ALARM_MSG_POS_Y + 120, RGB( 255, 255, 255 ), FW_NORMAL, TA_CENTER );
}



void GAMEMESSAGE::PutMessage( char *szString , char *szString2, bool bCloseGame )
{
	if ( m_bCloseGame != TRUE ) {   //어떠한 메세지가 들어와서 이미 메세지 박스가 떠있는 상태이고 그걸 클릭하면
								    //플밍이 종료될때. 또 메세지가 들어오면 찍지 않는다.
		g_pCMainFrame->m_bShowMakeRoomDlg = FALSE;
        g_pCMainFrame->m_IME_InputPass->InputEnd();
        g_pCMainFrame->m_IME_InputRoomTitle->InputEnd();
		g_pCMainFrame->m_IME_Memo->InputEnd();
		g_pCMainFrame->m_bShowMessageBox = FALSE;

		//g_pCMainFrame->m_pMsgChatBox->ShowWindow( SW_HIDE );
		//*****************
		
		bShowCheck = TRUE;

		strcpy( szText , szString );
		strcpy( szText2 , szString2 );

		m_bCloseGame = bCloseGame;
	}
}



BOOL GAMEMESSAGE::GetbShowCheck( void ) 
{
	return bShowCheck;
}



void GAMEMESSAGE::SetbShowCheck( BOOL Check )
{
	bShowCheck = Check;
}




