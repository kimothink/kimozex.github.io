// zenpoker.h : main header file for the ZENPOKER application
//

#if !defined(AFX_ZENPOKER_H__BA3915C0_5778_4C6E_B782_F6648DFF0801__INCLUDED_)
#define AFX_ZENPOKER_H__BA3915C0_5778_4C6E_B782_F6648DFF0801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CZenpokerApp:
// See zenpoker.cpp for the implementation of this class
//

class CZenpokerApp : public CWinApp
{
public:
	CZenpokerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZenpokerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CZenpokerApp)
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

#endif // !defined(AFX_ZENPOKER_H__BA3915C0_5778_4C6E_B782_F6648DFF0801__INCLUDED_)
