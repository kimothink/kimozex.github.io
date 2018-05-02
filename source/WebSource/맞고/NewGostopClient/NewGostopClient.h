// NewGostopClient.h : main header file for the NEWGOSTOPCLIENT application
//

#if !defined(AFX_NEWGOSTOPCLIENT_H__DC10514B_A978_468E_A772_B95173DA620A__INCLUDED_)
#define AFX_NEWGOSTOPCLIENT_H__DC10514B_A978_468E_A772_B95173DA620A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CNewGostopClientApp:
// See NewGostopClient.cpp for the implementation of this class
//

class CNewGostopClientApp : public CWinApp
{
public:
	CNewGostopClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewGostopClientApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CNewGostopClientApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    GdiplusStartupInput		m_gdiplusStartupInput;
    ULONG_PTR				m_gdiplusToken;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWGOSTOPCLIENT_H__DC10514B_A978_468E_A772_B95173DA620A__INCLUDED_)
