; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAdminLogin
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "NewGostopClient.h"
LastPage=0

ClassCount=6
Class1=CNewGostopClientApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=3
Resource1=IDR_MAINFRAME
Class2=CChildView
Class5=CUserDetailInfo
Resource2=IDD_ABOUTBOX
Class6=CAdminLogin
Resource3=IDD_ADMIN_LOGIN

[CLS:CNewGostopClientApp]
Type=0
HeaderFile=NewGostopClient.h
ImplementationFile=NewGostopClient.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CNewGostopClientApp

[CLS:CChildView]
Type=0
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=N

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=W
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=NewGostopClient.cpp
ImplementationFile=NewGostopClient.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_EDIT_UNDO
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[CLS:CUserDetailInfo]
Type=0
HeaderFile=UserDetailInfo.h
ImplementationFile=UserDetailInfo.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CUserDetailInfo

[DLG:IDD_ADMIN_LOGIN]
Type=1
Class=CAdminLogin
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ADMIN_ID,edit,1350631552
Control4=IDC_ADMIN_PASS,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[CLS:CAdminLogin]
Type=0
HeaderFile=AdminLogin.h
ImplementationFile=AdminLogin.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

