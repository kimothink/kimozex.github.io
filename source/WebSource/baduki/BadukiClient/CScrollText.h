#include "ClientDefine.h"

class CDXSurface;


class CScrollText
{
private:
    char        m_szText[512];              // 출력할 텍스트

    int         m_nCurrDrawPos;             // 텍스트 그리는 영역에서의 현재 위치(상대적 위치 : 0부터 시작)
    int         m_nCurrTextPos;             // 텍스트 전체 영역에서의 현재 위치(Pixel)
    int         m_nDrawWidth;               // 그리는 영역의 가로 크기
    int         m_nDrawHeight;              // 그리는 영역의 세로 크기
    int         m_nDrawPosX;                // 화면상에 그릴 좌표(X)
    int         m_nDrawPosY;                // 화면상에 그릴 좌표(Y)
    int         m_nDistance;                // 텍스트 이동 속도
    int         m_nSpeed;                   // 텍스트 이동(갱신) 속도 - 몇 프레임에 1번씩 갱신할 지를 결정(기본값 1)
    int         m_nSpeedCnt;                // 텍스트 갱신에 사용하는 카운터
    int         m_nTextWidth;               // 텍스트의 가로 크기(Pixel)
    int         m_nTextHeight;              // 텍스트의 세로 크기(Pixel)
    COLORREF    m_rgbTextColor;             // 글자 색

    RECT        m_sRealDrawRect;            // 실제 그리는 영역(원본 영역)

    CDXSurface *m_pSurface;

public:
    CScrollText();
    CScrollText(char *szText, COLORREF rgbTextCol, int nX1, int nY1, int nX2, int nY2);
    CScrollText(char *szText, COLORREF rgbTextCol, RECT *pRect);
   ~CScrollText();

    void    Create();                       // 텍스트 스크롤 표면을 생성한다.
    void    Draw();                         // 화면에 그린다.
    void    GetTextSize(CDXSurface *pSurface, LPCTSTR szString, LPSIZE size);   // 텍스트의 픽셀단위 크기를 구한다.
    void    Reload();                       // 텍스트 스크롤 표면을 다시 로딩한다.
    void    SetDistance(int nDistance);     // 한번에 스크롤 되는 길이를 설정한다.
    void    SetSpeed(int nSpeed);           // 스크롤 속도 설정
    void    Update();                       // m_nDistance 만큼 스크롤시킨다.
};
