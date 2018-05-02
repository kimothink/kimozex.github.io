// AdminLogin.cpp : implementation file
//

#include "stdafx.h"
#include "NewGostopClient.h"
#include "AdminLogin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdminLogin dialog


CAdminLogin::CAdminLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CAdminLogin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdminLogin)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAdminLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdminLogin)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdminLogin, CDialog)
	//{{AFX_MSG_MAP(CAdminLogin)
	ON_BN_CLICKED(IDOK, OnBtnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdminLogin message handlers

CString CAdminLogin::QueryHttp(CString url)
{


	url.TrimLeft();
	url.TrimRight();
	if(url.GetLength() <=0) { return "ERROR";}


	 CInternetSession session; 
	 CInternetFile* file=NULL; 

	 session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 3000);
	 session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 0);

	 CString data="",totalData="";

		file=(CInternetFile *) session.OpenURL(url); 
		file->SetReadBufferSize(4096); 
 
		if(!file) { return "ERROR";}

		do
		{  
			totalData += data;

		} while(file->ReadString(data) != NULL); 

		session.Close();
		file->Close();
		delete file;



		return totalData;
}


void CAdminLogin::OnBtnOK() 
{
	 CString adminID,adminPass,url;
	 GetDlgItemText(IDC_ADMIN_ID,adminID);        adminID.TrimLeft();   adminID.TrimRight();
	 GetDlgItemText(IDC_ADMIN_PASS,adminPass);    adminPass.TrimLeft(); adminPass.TrimRight();

	 int logcheck = 0;


	 url.Format("http://www.clubgaon.com/messenger_new/game_admin_check.asp?game=baduki&user_id=%s&login_pass=%s",adminID,adminPass);


//	 AfxMessageBox(url);
	CString strCheck = QueryHttp(url);
	strCheck.TrimLeft();
	strCheck.TrimRight();
	// AfxMessageBox(strCheck);
	if(strCheck.Compare("AdminOK") !=0 ) {
		AfxMessageBox("ID,Pass가 맞지 않읍니다..");
		SetDlgItemText(IDC_ADMIN_ID,"");
		SetDlgItemText(IDC_ADMIN_PASS,"");
		return;
	}

	m_AdminID = adminID;

	
	CDialog::OnOK();
}
