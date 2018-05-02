




#if !defined(AFX_ADMINLOGIN_H__2B24CAEA_3622_4615_8C6B_37B4BB6856BB__INCLUDED_)
#define AFX_ADMINLOGIN_H__2B24CAEA_3622_4615_8C6B_37B4BB6856BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdminLogin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdminLogin dialog
#include "resource.h"

#include <afxinet.h> 


class CAdminLogin : public CDialog
{
// Construction
public:
	CAdminLogin(CWnd* pParent = NULL);   // standard constructor
    CString QueryHttp(CString url);
	

	CString m_AdminID;
// Dialog Data
	//{{AFX_DATA(CAdminLogin)
	enum { IDD = IDD_ADMIN_LOGIN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdminLogin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdminLogin)
	afx_msg void OnBtnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADMINLOGIN_H__2B24CAEA_3622_4615_8C6B_37B4BB6856BB__INCLUDED_)
