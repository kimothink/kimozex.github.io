#if !defined(AFX_HTTPVIEW_H__0B06AEC8_2840_409B_A231_6FFEBB9378AE__INCLUDED_)
#define AFX_HTTPVIEW_H__0B06AEC8_2840_409B_A231_6FFEBB9378AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HttpView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHttpView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

class CHttpView : public CHtmlView
{
public:
	CHttpView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHttpView)

// html Data
public:
	//{{AFX_DATA(CHttpView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHttpView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHttpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CHttpView)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	BOOL CreateWnd(CRect rcWnd, CWnd *pParent);
	CHttpView* GetHtmlView();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTTPVIEW_H__0B06AEC8_2840_409B_A231_6FFEBB9378AE__INCLUDED_)
