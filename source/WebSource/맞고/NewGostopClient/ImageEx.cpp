// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageEx.h"
#include <process.h>


#include "MainFrm.h"
#include "dtrace.h"

extern CMainFrame *g_pCMainFrame;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ImageEx
// 
// DESCRIPTION:	Constructor for constructing images from a resource 
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

ImageEx::ImageEx(LPCTSTR sResourceType, LPCTSTR sResource)
{
	Initialize();

	if ( Load(sResourceType, sResource) ) {
	   nativeImage = NULL;
	   lastResult = DllExports::GdipLoadImageFromStreamICM(m_pStream, &nativeImage);
	   TestForAnimatedGIF();
    }
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ImageEx
// 
// DESCRIPTION:	Constructor for constructing images from a file
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

ImageEx::ImageEx(const WCHAR* filename, CDXSprite *pCDXSprite, BOOL useEmbeddedColorManagement) : Image(filename, useEmbeddedColorManagement)
{
	Initialize();

	m_bIsInitialized = true;
    l_SPR_Avatar = pCDXSprite;

	TestForAnimatedGIF();
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::~ImageEx
// 
// DESCRIPTION:	Free up fresources
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

ImageEx::~ImageEx()
{
	Destroy();
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	InitAnimation
// 
// DESCRIPTION:	Prepare animated GIF
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

bool ImageEx::InitAnimation(HWND hWnd, CPoint pt)
{
	m_hWnd = hWnd;
	m_pt = pt;

	if (!m_bIsInitialized) {
		TRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (IsAnimatedGIF()) {
		if (m_hThread == NULL) {
		
			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED, &nTID );
			
			if (!m_hThread) {
				TRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
            else {
                ResumeThread(m_hThread);
            }
		}
	}
    else {
        DrawFrameGIF();
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	LoadFromBuffer
// 
// DESCRIPTION:	Helper function to copy phyical memory from buffer a IStream
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

bool ImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);

	if (hGlobal) {
		void* pData = GlobalLock(hGlobal);

        if (pData) {
            memcpy(pData, pBuff, nSize);
        }
		
		GlobalUnlock(hGlobal);

        if (CreateStreamOnHGlobal(hGlobal, TRUE, &m_pStream) == S_OK) {
            bResult = true;
        }
	}

	return bResult;
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	GetResource
// 
// DESCRIPTION:	Helper function to lock down resource
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

bool ImageEx::GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	// Find the resource

	hResInfo = FindResource(m_hInst , lpName, lpType);
	if (hResInfo == NULL) 
	{
		DWORD dwErr = GetLastError();
		return false;
	}

	// Load the resource
	hRes = LoadResource(m_hInst , hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(m_hInst , hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(m_hInst , hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	Load
// 
// DESCRIPTION:	Helper function to load resource from memory
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

bool ImageEx::Load(CString sResourceType, CString sResource)
{
	bool    bResult = false;
	BYTE*	pBuff = NULL;
	int		nSize = 0;

	if (GetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize)) {
		if (nSize > 0) {
			pBuff = new BYTE[nSize];

			if (GetResource(sResource, sResourceType.GetBuffer(0), pBuff, nSize)) {
				if (LoadFromBuffer(pBuff, nSize)) {
					bResult = true;
				}
			}

			delete [] pBuff;
		}
	}

	m_bIsInitialized = bResult;

	return bResult;
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	GetSize
// 
// DESCRIPTION:	Returns Width and Height object
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

CSize ImageEx::GetSize()
{
	return CSize(GetWidth(), GetHeight());
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	TestForAnimatedGIF
// 
// DESCRIPTION:	Check GIF/Image for avialability of animation
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

bool ImageEx::TestForAnimatedGIF()
{
	/*
   UINT count = 0;
   count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);

   GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   
   delete  pDimensionIDs;

   return m_nFrameCount > 1;
   */
	m_nFrameCount = GetFrameCount(&FrameDimensionTime);	
	int nSize = GetPropertyItemSize(PropertyTagFrameDelay);
	m_pPropertyItem = (PropertyItem*) malloc(nSize);

	GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
		
	return m_nFrameCount > 1;
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::Initialize
// 
// DESCRIPTION:	Common function called from Constructors
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

void ImageEx::Initialize()
{
	m_pStream        = NULL;
	m_nFramePosition = 0;
	m_nFrameCount    = 0;
	m_pStream        = NULL;
	lastResult       = InvalidParameter;
	m_hThread        = NULL;
	m_bIsInitialized = false;
	m_pPropertyItem  = NULL;
    l_SPR_Avatar     = NULL;
	
#ifdef INDIGO_CTRL_PROJECT
	m_hInst = _Module.GetResourceInstance();
#else
	m_hInst = AfxGetResourceHandle();
#endif

	m_bPause     = false;
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hPause     = CreateEvent(NULL, TRUE, TRUE,  NULL);
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::_ThreadAnimationProc
// 
// DESCRIPTION:	Thread to draw animated gifs
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

UINT WINAPI ImageEx::_ThreadAnimationProc(LPVOID pParam)
{
	ASSERT(pParam);
	ImageEx *pImage = reinterpret_cast<ImageEx *> (pParam);
	pImage->ThreadAnimation();

	return 0;
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ThreadAnimation
// 
// DESCRIPTION:	Helper function
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

void ImageEx::ThreadAnimation()
{
	m_nFramePosition = 0;

	bool bExit = false;

	while (bExit == false) {
		bExit = DrawFrameGIF();
	}
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::DrawFrameGIF
// 
// DESCRIPTION:	
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

bool ImageEx::DrawFrameGIF()
{
    if ( l_SPR_Avatar == NULL ) {
		return false;
	}

	::WaitForSingleObject(m_hPause, INFINITE);

	GUID   pageGuid = FrameDimensionTime;

	long hmWidth = GetWidth();
	long hmHeight = GetHeight();
	HDC hDC = NULL;

    if ( l_SPR_Avatar == NULL ) {
        return true;
    }
	
    hDC = l_SPR_Avatar->GetTile()->GetDC();

    // l_SPR_Avatar의 표면이 파괴되면 아바타 에니메이션 스레드를 멈춘다.

    if ( hDC == NULL ) {
		Sleep(100);
		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause);

		return dwErr == WAIT_OBJECT_0;
	}

    // l_SPR_Avatar에 아바타의 현재 프레임을 그린다.

    else {
		Graphics graph( hDC );
		graph.Clear(Color( 161, 207, 99 ));
        graph.DrawImage( this, m_pt.x, m_pt.y, hmWidth, hmHeight );
		l_SPR_Avatar->GetTile()->ReleaseDC();
	}
	
	SelectActiveFrame(&pageGuid, m_nFramePosition++);
	
	if (m_nFramePosition == m_nFrameCount)
		m_nFramePosition = 0;

	long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;

    if ( m_nFrameCount == 1 ) {
        Sleep(100);
    }
		
	DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause);

	return dwErr == WAIT_OBJECT_0;
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::SetPause
// 
// DESCRIPTION:	Toggle Pause state of GIF
// 
// RETURNS:		
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////

void ImageEx::SetPause(bool bPause)
{
	if (!IsAnimatedGIF())
		return;

    // GIF 에니메이션 중인 상태이면 이벤트를 비신호상태로 만든다.
	if (bPause && !m_bPause) {
		::ResetEvent(m_hPause);
	}

    // GIF 에니메이션이 멈춘 상태이면 
	else {
		if (m_bPause && !bPause) {
			::SetEvent(m_hPause);   // 이벤트를 신호상태로 만든다.
		}
	}

	m_bPause = bPause;
}



//////////////////////////////////////////////////////////////////////////
// 종료시 초기화
//////////////////////////////////////////////////////////////////////////

void ImageEx::Destroy()
{
	if (m_hThread) {
		// If pause un pause
		SetPause(false);

		SetEvent(m_hExitEvent);
		WaitForSingleObject(m_hThread, INFINITE);
	}

	CloseHandle(m_hThread);
	CloseHandle(m_hExitEvent);
	CloseHandle(m_hPause);

	free(m_pPropertyItem);

	m_pPropertyItem = NULL;
	m_hThread       = NULL;
	m_hExitEvent    = NULL;
	m_hPause        = NULL;
    l_SPR_Avatar    = NULL;

    if (m_pStream) {
        m_pStream->Release();
    }
}