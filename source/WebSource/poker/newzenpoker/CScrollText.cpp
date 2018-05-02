#include <StdAfx.h>
#include "Externvar.h"

//========================================================================
// 생성자
//
// input:   szText                  = 출력할 문자열
//          rgbBackCol              = 배경색
//          rgbTextCol              = 문자색
//          (nX1, nY1)-(nX2, nY2)   = 문자열을 출력할 사각 영역
//========================================================================

CScrollText::CScrollText(char *szText, COLORREF rgbTextCol, int nX1, int nY1, int nX2, int nY2)
{
    SIZE sTextSize;
    HDC hDC = NULL;
    CDXSurface *pSurface = NULL;

    m_nDrawWidth    = nX2 - nX1;
    m_nDrawHeight   = nY2 - nY1;
    m_nDrawPosX     = nX1;
    m_nDrawPosY     = nY1;
    m_nDistance     = 1;
    m_nCurrDrawPos  = m_nDrawWidth;     // 오른쪽에서 왼쪽으로 이동하기 때문
    m_nCurrTextPos  = 0;
    m_nSpeed        = 1;
    m_nSpeedCnt     = 0;
    m_pSurface      = NULL;
    m_rgbTextColor  = rgbTextCol;
    memset(m_szText, 0x00, sizeof(m_szText));
    
    //------------
    // 텍스트 복사
    //------------

    /*
    if (szText == NULL) strcpy(m_szText, "");
    else                strcpy(m_szText, szText);
    */

    if (szText == NULL || szText[0] == NULL) {
        strcpy(m_szText, " ");
    }
    else {
        strcpy(m_szText, szText);
    }

    //-------------------
    // 텍스트 크기 구한다
    //-------------------

    GetTextSize(g_pCDXScreen->GetBack(), m_szText, &sTextSize);

    m_nTextWidth  = sTextSize.cx;
    m_nTextHeight = sTextSize.cy;

    m_sRealDrawRect.left   = 0;
    m_sRealDrawRect.top    = 0;
    m_sRealDrawRect.right  = (m_nTextWidth > m_nDrawWidth) ? m_nDrawWidth : m_nTextWidth;
    m_sRealDrawRect.bottom = m_nTextHeight;
    
    Create();
}



//========================================================================
// 생성자
//
// input:   szText      = 출력할 문자열
//          rgbBackCol  = 배경색
//          rgbTextCol  = 문자색
//          pRect       = 문자열을 출력할 사각 영역
//========================================================================
CScrollText::CScrollText()
{
}

CScrollText::CScrollText(char *szText, COLORREF rgbTextCol, RECT *pRect)
{
    CScrollText(szText, rgbTextCol, 
               (int)pRect->left,
               (int)pRect->top,
               (int)pRect->right,
               (int)pRect->bottom);
}



//========================================================================
// 파괴자
//========================================================================

CScrollText::~CScrollText()
{
    SAFE_DELETE(m_pSurface);
}



//========================================================================
// 표면을 생성한다.
//========================================================================

void CScrollText::Create()
{
    //----------
    // 표면 생성
    //----------

    SAFE_DELETE(m_pSurface);

    m_pSurface = new CDXSurface();
    m_pSurface->Create(g_pCDXScreen, m_nTextWidth, m_nTextHeight, CDXMEM_SYSTEMONLY);
    m_pSurface->Fill(0);
    m_pSurface->SetColorKey(0);

    //-------------------------------
    // 생성한 표면에 텍스트를 그린다.
    //-------------------------------

    HDC hDC = m_pSurface->GetDC();
    m_pSurface->ChangeFont("굴림체", 0, 12, FW_NORMAL);
    m_pSurface->SetFont();
    m_pSurface->TextXY(0, 0, m_rgbTextColor, m_szText);
    m_pSurface->ReleaseDC();
}



//========================================================================
// 화면에 스크롤되는 텍스트를 그린다.
//========================================================================

void CScrollText::Draw()
{
    if (m_nDrawWidth - m_nCurrDrawPos > 0)
    {
        if (m_pSurface->DrawTrans(g_pCDXScreen->GetBack(), m_nDrawPosX + m_nCurrDrawPos, m_nDrawPosY, &m_sRealDrawRect) < 0) {
            Reload();
        }
    }
}



//========================================================================
// 텍스트의 픽셀단위 크기를 구한다.
//========================================================================

void CScrollText::GetTextSize(CDXSurface *pSurface, LPCTSTR szString, LPSIZE size)
{
    if (pSurface == NULL) return;
	
    HDC hDC = pSurface->GetDC();
    pSurface->ChangeFont("굴림체", 0, 12, FW_NORMAL);                   // 폰트 바꿈
    pSurface->SetFont();                                                // 바꾼 폰트 적용
    GetTextExtentPoint32(hDC, szString, lstrlen(szString), size); // 폰트 크기 가져옴
    pSurface->ReleaseDC();
}



//========================================================================
// 텍스트 스크롤 표면을 다시 만든다.(복구한다.)
//========================================================================

void CScrollText::Reload()
{
    Create();
}



//========================================================================
// 텍스트의 이동 거리를 설정한다.
//
// input:   nDistance = 텍스트 이동 거리(단위: pixel)
//========================================================================

void CScrollText::SetDistance(int nDistance)
{
    m_nDistance = nDistance;
}



//========================================================================
// 스크롤 속도를 설정한다.
// 이 함수를 사용하지 않은 경우 기본값이 1이다.
//
// input:   nSpeed = 스크롤 갱신 속도(단위: 프레임 - 숫자가 클 수록 느리다.)
//========================================================================

void CScrollText::SetSpeed(int nSpeed)
{
    m_nSpeed = nSpeed;
}



//========================================================================
// 텍스트를 스크롤 시킨다.
//========================================================================

void CScrollText::Update()
{
    m_nSpeedCnt++;
    if (m_nSpeedCnt < m_nSpeed) return;

    m_nSpeedCnt = 0;

    int nCurrTextWidth = m_nTextWidth - m_nCurrTextPos + 1;
    int nCurrDrawWidth = m_nDrawWidth - m_nCurrDrawPos + 1;

    //-------------------------------------------------------
    // 텍스트가 맨 왼쪽 끝에 도착했으면 텍스트를 스크롤시키고
    // 도착하지 않았으면 출력위치를 왼쪽으로 이동시킨다.
    //-------------------------------------------------------

    if (m_nCurrDrawPos == 0)                    // 맨 왼쪽이면 스크롤
    {
        if (nCurrTextWidth - m_nDistance > 0) { // 맨 왼쪽이면 스크롤
            m_nCurrTextPos += m_nDistance;
        } else {                                // 처음부터 다시 스크롤
            m_nCurrTextPos = 0;
            m_nCurrDrawPos = m_nDrawWidth;
        }
    }
    else                                        // 맨 왼쪽이 아니면 왼쪽으로 이동
    {
        if (m_nCurrDrawPos - m_nDistance <= 0) {
            m_nCurrDrawPos = 0;
        } else {
            m_nCurrDrawPos -= m_nDistance;
        }
    }

    //-----------------
    // 출력할 영역 갱신
    //-----------------

    m_sRealDrawRect.left = m_nCurrTextPos;
    m_sRealDrawRect.top = 0;
    m_sRealDrawRect.right = (nCurrTextWidth > nCurrDrawWidth) ? m_nCurrTextPos + nCurrDrawWidth : m_nTextWidth;
    m_sRealDrawRect.bottom = m_nTextHeight;
}
