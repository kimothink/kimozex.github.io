// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C0A2BD9E_BBE7_4626_B011_87C0F43DE322__INCLUDED_)
#define AFX_STDAFX_H__C0A2BD9E_BBE7_4626_B011_87C0F43DE322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <winsock2.h>
#include <afxsock.h>		// MFC socket extensions
#include "NetworkSock.h"
#include "..\NetworkDefine\NetWorkDefine.h"

#import <mshtml.tlb> // Internet Explorer 5
#import <shdocvw.dll>



#pragma comment(lib,"gdiplus.lib")
#include "GDINew.h"
using namespace Gdiplus;
#include "ImageEx.h"


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Imm32.lib")



extern int g_nSystemMoneyRate;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C0A2BD9E_BBE7_4626_B011_87C0F43DE322__INCLUDED_)
