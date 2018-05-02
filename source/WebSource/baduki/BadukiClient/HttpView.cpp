// HttpView.cpp : implementation file
//

#include "stdafx.h"
//#include "Neo3GoMfc.h"
#include "HttpView.h"
#include "dtrace.h"

#include "MainFrm.h"

extern CMainFrame *g_pCMainFrame;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHttpView

IMPLEMENT_DYNCREATE(CHttpView, CHtmlView)

CHttpView::CHttpView()
{
	//{{AFX_DATA_INIT(CHttpView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CHttpView::~CHttpView()
{
}

void CHttpView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHttpView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHttpView, CHtmlView)
	//{{AFX_MSG_MAP(CHttpView)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHttpView diagnostics

#ifdef _DEBUG
void CHttpView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CHttpView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHttpView message handlers

BOOL CHttpView::CreateWnd(CRect rcWnd, CWnd *pParent)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	LPCTSTR lpWndCls;
	lpWndCls = AfxRegisterWndClass(0);
	BOOL bRet = CHtmlView::Create(NULL, lpWndCls, dwStyle, rcWnd, pParent, 9090);
	return bRet;
}

CHttpView* CHttpView::GetHtmlView()
{
	CHttpView* pView = new CHttpView;
	return pView;
}

BOOL CHttpView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message ) {
		case WM_MOUSEMOVE:
			
			//hy_v0023_04.03.30
			g_pCMainFrame->SetGameCursor( TRUE, FALSE, FALSE );
			//*****************
			
			//DTRACE( "CHttpView::PreTranslateMessage - WM_MOUSEMOVE" );
			break;	
	}

	return CHtmlView::PreTranslateMessage(pMsg);
}

void CHttpView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CHttpView::OnDestroy() 
{
	CHtmlView::OnDestroy();
	
	// TODO: Add your message handler code here
	//this->onclo
}
