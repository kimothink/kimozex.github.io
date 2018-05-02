#include "ClientDefine.h"

class CDXSurface;


class CScrollText
{
private:
    char        m_szText[512];              // ����� �ؽ�Ʈ

    int         m_nCurrDrawPos;             // �ؽ�Ʈ �׸��� ���������� ���� ��ġ(����� ��ġ : 0���� ����)
    int         m_nCurrTextPos;             // �ؽ�Ʈ ��ü ���������� ���� ��ġ(Pixel)
    int         m_nDrawWidth;               // �׸��� ������ ���� ũ��
    int         m_nDrawHeight;              // �׸��� ������ ���� ũ��
    int         m_nDrawPosX;                // ȭ��� �׸� ��ǥ(X)
    int         m_nDrawPosY;                // ȭ��� �׸� ��ǥ(Y)
    int         m_nDistance;                // �ؽ�Ʈ �̵� �ӵ�
    int         m_nSpeed;                   // �ؽ�Ʈ �̵�(����) �ӵ� - �� �����ӿ� 1���� ������ ���� ����(�⺻�� 1)
    int         m_nSpeedCnt;                // �ؽ�Ʈ ���ſ� ����ϴ� ī����
    int         m_nTextWidth;               // �ؽ�Ʈ�� ���� ũ��(Pixel)
    int         m_nTextHeight;              // �ؽ�Ʈ�� ���� ũ��(Pixel)
    COLORREF    m_rgbTextColor;             // ���� ��

    RECT        m_sRealDrawRect;            // ���� �׸��� ����(���� ����)

    CDXSurface *m_pSurface;

public:
    CScrollText();
    CScrollText(char *szText, COLORREF rgbTextCol, int nX1, int nY1, int nX2, int nY2);
    CScrollText(char *szText, COLORREF rgbTextCol, RECT *pRect);
   ~CScrollText();

    void    Create();                       // �ؽ�Ʈ ��ũ�� ǥ���� �����Ѵ�.
    void    Draw();                         // ȭ�鿡 �׸���.
    void    GetTextSize(CDXSurface *pSurface, LPCTSTR szString, LPSIZE size);   // �ؽ�Ʈ�� �ȼ����� ũ�⸦ ���Ѵ�.
    void    Reload();                       // �ؽ�Ʈ ��ũ�� ǥ���� �ٽ� �ε��Ѵ�.
    void    SetDistance(int nDistance);     // �ѹ��� ��ũ�� �Ǵ� ���̸� �����Ѵ�.
    void    SetSpeed(int nSpeed);           // ��ũ�� �ӵ� ����
    void    Update();                       // m_nDistance ��ŭ ��ũ�ѽ�Ų��.
};
