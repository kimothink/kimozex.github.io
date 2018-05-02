// zenpoker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "zenpoker.h"

#include "MainFrm.h"
// ham 2005.10.27
#include "Externvar.h"
////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CZenpokerApp

BEGIN_MESSAGE_MAP(CZenpokerApp, CWinApp)
	//{{AFX_MSG_MAP(CZenpokerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZenpokerApp construction

CZenpokerApp::CZenpokerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CZenpokerApp object

CZenpokerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CZenpokerApp initialization

BOOL CZenpokerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	char *szProgName = "NiliryGames";

	//////////////////////////////////////////////////////////////////////////
	//프로그램의 중복실행을 방지하기 위해서.....
	CreateMutex(NULL, TRUE, szProgName);

#ifndef _DEBUG
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		::MessageBox( NULL, "하나이상의 게임이 이미 실행중입니다.", szProgName, MB_OK );

        g_bRealExit = true;

		::PostMessage( NULL, WM_CLOSE, 0, 0 );
		return -1;
	}

#endif

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);


	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;


	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CZenpokerApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CZenpokerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CZenpokerApp message handlers


int CZenpokerApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	GdiplusShutdown(m_gdiplusToken);	
	
	return CWinApp::ExitInstance();
}


BOOL CZenpokerApp::OnIdle(LONG lCount) 
{
    // TODO: Add your specialized code here and/or call the base class
	CMainFrame *g_pCMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;

    /*
	if( g_pCMainFrame->m_bActive ) {
		g_pCMainFrame->Render();
	}

	Sleep(5);
    */

    g_pCMainFrame->m_fSkipTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

	if(g_pCMainFrame->m_bActive)
	{
		if( g_pCMainFrame->m_FrameSkip.Update( g_pCMainFrame->m_fSkipTime ) )
		{
            g_pCMainFrame->AnimationUpdate();
		}


        if( !g_pCMainFrame->m_FrameSkip.IsFrameSkip() ) {
            g_pCMainFrame->Render();
    		Sleep(20);
        }
	}
    
	return TRUE;
   
    //return CWinApp::OnIdle(lCount);
}
