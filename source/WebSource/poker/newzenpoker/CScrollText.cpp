#include <StdAfx.h>
#include "Externvar.h"

//========================================================================
// ������
//
// input:   szText                  = ����� ���ڿ�
//          rgbBackCol              = ����
//          rgbTextCol              = ���ڻ�
//          (nX1, nY1)-(nX2, nY2)   = ���ڿ��� ����� �簢 ����
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
    m_nCurrDrawPos  = m_nDrawWidth;     // �����ʿ��� �������� �̵��ϱ� ����
    m_nCurrTextPos  = 0;
    m_nSpeed        = 1;
    m_nSpeedCnt     = 0;
    m_pSurface      = NULL;
    m_rgbTextColor  = rgbTextCol;
    memset(m_szText, 0x00, sizeof(m_szText));
    
    //------------
    // �ؽ�Ʈ ����
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
    // �ؽ�Ʈ ũ�� ���Ѵ�
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
// ������
//
// input:   szText      = ����� ���ڿ�
//          rgbBackCol  = ����
//          rgbTextCol  = ���ڻ�
//          pRect       = ���ڿ��� ����� �簢 ����
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
// �ı���
//========================================================================

CScrollText::~CScrollText()
{
    SAFE_DELETE(m_pSurface);
}



//========================================================================
// ǥ���� �����Ѵ�.
//========================================================================

void CScrollText::Create()
{
    //----------
    // ǥ�� ����
    //----------

    SAFE_DELETE(m_pSurface);

    m_pSurface = new CDXSurface();
    m_pSurface->Create(g_pCDXScreen, m_nTextWidth, m_nTextHeight, CDXMEM_SYSTEMONLY);
    m_pSurface->Fill(0);
    m_pSurface->SetColorKey(0);

    //-------------------------------
    // ������ ǥ�鿡 �ؽ�Ʈ�� �׸���.
    //-------------------------------

    HDC hDC = m_pSurface->GetDC();
    m_pSurface->ChangeFont("����ü", 0, 12, FW_NORMAL);
    m_pSurface->SetFont();
    m_pSurface->TextXY(0, 0, m_rgbTextColor, m_szText);
    m_pSurface->ReleaseDC();
}



//========================================================================
// ȭ�鿡 ��ũ�ѵǴ� �ؽ�Ʈ�� �׸���.
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
// �ؽ�Ʈ�� �ȼ����� ũ�⸦ ���Ѵ�.
//========================================================================

void CScrollText::GetTextSize(CDXSurface *pSurface, LPCTSTR szString, LPSIZE size)
{
    if (pSurface == NULL) return;
	
    HDC hDC = pSurface->GetDC();
    pSurface->ChangeFont("����ü", 0, 12, FW_NORMAL);                   // ��Ʈ �ٲ�
    pSurface->SetFont();                                                // �ٲ� ��Ʈ ����
    GetTextExtentPoint32(hDC, szString, lstrlen(szString), size); // ��Ʈ ũ�� ������
    pSurface->ReleaseDC();
}



//========================================================================
// �ؽ�Ʈ ��ũ�� ǥ���� �ٽ� �����.(�����Ѵ�.)
//========================================================================

void CScrollText::Reload()
{
    Create();
}



//========================================================================
// �ؽ�Ʈ�� �̵� �Ÿ��� �����Ѵ�.
//
// input:   nDistance = �ؽ�Ʈ �̵� �Ÿ�(����: pixel)
//========================================================================

void CScrollText::SetDistance(int nDistance)
{
    m_nDistance = nDistance;
}



//========================================================================
// ��ũ�� �ӵ��� �����Ѵ�.
// �� �Լ��� ������� ���� ��� �⺻���� 1�̴�.
//
// input:   nSpeed = ��ũ�� ���� �ӵ�(����: ������ - ���ڰ� Ŭ ���� ������.)
//========================================================================

void CScrollText::SetSpeed(int nSpeed)
{
    m_nSpeed = nSpeed;
}



//========================================================================
// �ؽ�Ʈ�� ��ũ�� ��Ų��.
//========================================================================

void CScrollText::Update()
{
    m_nSpeedCnt++;
    if (m_nSpeedCnt < m_nSpeed) return;

    m_nSpeedCnt = 0;

    int nCurrTextWidth = m_nTextWidth - m_nCurrTextPos + 1;
    int nCurrDrawWidth = m_nDrawWidth - m_nCurrDrawPos + 1;

    //-------------------------------------------------------
    // �ؽ�Ʈ�� �� ���� ���� ���������� �ؽ�Ʈ�� ��ũ�ѽ�Ű��
    // �������� �ʾ����� �����ġ�� �������� �̵���Ų��.
    //-------------------------------------------------------

    if (m_nCurrDrawPos == 0)                    // �� �����̸� ��ũ��
    {
        if (nCurrTextWidth - m_nDistance > 0) { // �� �����̸� ��ũ��
            m_nCurrTextPos += m_nDistance;
        } else {                                // ó������ �ٽ� ��ũ��
            m_nCurrTextPos = 0;
            m_nCurrDrawPos = m_nDrawWidth;
        }
    }
    else                                        // �� ������ �ƴϸ� �������� �̵�
    {
        if (m_nCurrDrawPos - m_nDistance <= 0) {
            m_nCurrDrawPos = 0;
        } else {
            m_nCurrDrawPos -= m_nDistance;
        }
    }

    //-----------------
    // ����� ���� ����
    //-----------------

    m_sRealDrawRect.left = m_nCurrTextPos;
    m_sRealDrawRect.top = 0;
    m_sRealDrawRect.right = (nCurrTextWidth > nCurrDrawWidth) ? m_nCurrTextPos + nCurrDrawWidth : m_nTextWidth;
    m_sRealDrawRect.bottom = m_nTextHeight;
}
