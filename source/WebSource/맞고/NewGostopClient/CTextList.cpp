
#include "stdafx.h"
#include "CTextList.h"

// DEFAULT VALUE (잘못된 값으로 객체를 생성하려고 했을 때 사용됨)

#define DEFAULT_TEXT_LEN        40      // 텍스트 한 줄의 최대 길이
#define DEFAULT_MAX_LINE        30      // 최대 저장 가능한 텍스트 줄 수



//////////////////////////////////////////////////////////////////////////
// 생성자
//
// input:   nMaxLen = 텍스트 한 줄의 최대 길이
//          nMaxLine = 저장 가능한 텍스트 줄 수
//////////////////////////////////////////////////////////////////////////

CTextList::CTextList(int nMaxLen, int nMaxLine)
{
    if (nMaxLen < 2) m_nMaxTextLen = DEFAULT_TEXT_LEN;
    else             m_nMaxTextLen = nMaxLen;

    if (nMaxLine < 1)
    {
        m_nMaxLine  = DEFAULT_MAX_LINE;
        m_sTextInfo = new STextInfo[DEFAULT_MAX_LINE];
    }
    else
    {
        m_nMaxLine  = nMaxLine;
        m_sTextInfo = new STextInfo[nMaxLine];
    }
    
    Init();
}



//////////////////////////////////////////////////////////////////////////
// 소멸자
//
// 객체 생성시 동적으로 할당한 메모리를 해제한다.
//////////////////////////////////////////////////////////////////////////

CTextList::~CTextList()
{
    if (m_sTextInfo != NULL)
    {
        delete [] m_sTextInfo;
        m_sTextInfo = NULL;
    }
}



//////////////////////////////////////////////////////////////////////////
// 텍스트를 추가한다.
//
// 입력된 텍스트의 길이가 현재 저장 가능한 텍스트의 길이보다 긴 경우
// 여러 줄로 나누어서 저장한다.
// 채팅창처럼 입력한 글이 여러 줄에 걸쳐서 보일 수 있도록 이런 방식으로
// 구현한 것이다.
//
// input: *pTextInfo = 텍스트 정보 구조체의 주소
//
//          typedef struct tagSTextInfo
//          {
//              COLORREF    rgbColor;    // 텍스트의 색깔( RGB(red, green, blue) )
//              int         nStyle;      // 텍스트 스타일(0=보통, 1=굵게)
//              char        szText[255]; // 텍스트
//          } STextInfo;
//
//////////////////////////////////////////////////////////////////////////

void CTextList::AddText(STextInfo *pTextInfo)
{
    int nStrLen;
    int nLen;
    int nIdx;
    int nStartIdx;
    char szBuff[256];
    STextInfo sTextInfo;

    if (pTextInfo == NULL) return;

    sTextInfo.nStyle = pTextInfo->nStyle;
    sTextInfo.rgbColor = pTextInfo->rgbColor;
    nStrLen = strlen(pTextInfo->szText);
    nStartIdx = 0;

    strcpy (szBuff, pTextInfo->szText);

    //-[S1]
    while (nStartIdx < nStrLen)
    {
        nIdx = nStartIdx;
        nLen = 0;

        //-[S2]
        while (nLen < m_nMaxTextLen && nIdx < nStrLen)
        {
            // 알파벳
            if (szBuff[nIdx] > 31 && szBuff[nIdx] < 127)
            {
                nIdx++;
                nLen++;
            }

            // 한글
            else
            {
                if (nLen + 2 > m_nMaxTextLen || // 경계면에 한글이 있어서 잘리는 경우
                    nIdx + 1 >= nStrLen)        // 문자열 끝에 반쪽짜리 한글이 있는 경우
                {
                    // 문자열의 갯수 구하는 루프는 종료한다.
                    break;
                }

                else
                {
                    nIdx += 2;
                    nLen += 2;
                }
            }
        }
        //-[E2]

        if (szBuff[nIdx] >= 127 &&      // 문자열 끝에 반쪽짜리 한글이 있는 경우
            (nIdx + 1) >= nStrLen)      // 입력하지 않고 그냥 종료한다.
        {
            return;
        }

        else
        {
            strncpy(sTextInfo.szText, szBuff + nStartIdx, nLen);
            sTextInfo.szText[nLen] = NULL;
            Add(&sTextInfo);

            nStartIdx += nLen;          // 다음 줄을 저장하기 위해 시작 위치 이동
        }
    }
    //-[E1]
}



//////////////////////////////////////////////////////////////////////////
// 텍스트를 추가한다. (내부적으로만 사용)
//
// input: *pTextInfo = 텍스트 정보 구조체의 주소
//
//          typedef struct tagSTextInfo
//          {
//              COLORREF    rgbColor;    // 텍스트의 색깔( RGB(red, green, blue) )
//              int         nStyle;      // 텍스트 스타일(0=보통, 1=굵게)
//              char        szText[255]; // 텍스트
//          } STextInfo;
//
// return:  true = 텍스트 줄 수가 증가했다.
//          false = 텍스트 줄 수가 증가하지 않았다.(꽉 차서)
//
//////////////////////////////////////////////////////////////////////////

bool CTextList::Add(STextInfo *pTextInfo)
{
    if (m_nTextLineNum >= m_nMaxLine)
    {
        for (int i = 1; i < m_nMaxLine; i++)
        {
            memcpy(&m_sTextInfo[i - 1], &m_sTextInfo[i], sizeof(STextInfo));
        }

        memcpy(&m_sTextInfo[m_nMaxLine - 1], pTextInfo, sizeof(STextInfo));
        return false;
    }
    
    else
    {
        memcpy(&m_sTextInfo[m_nTextLineNum], pTextInfo, sizeof(STextInfo));
        m_nTextLineNum++;
        return true;
    }
}



//////////////////////////////////////////////////////////////////////////
// 지정된 위치의 텍스트 정보를 리턴한다.
//////////////////////////////////////////////////////////////////////////

STextInfo *CTextList::GetTextInfo(int nIdx)
{
    if (nIdx < 0 || nIdx >= m_nTextLineNum) return NULL;
    else return &m_sTextInfo[nIdx];
}

    

//////////////////////////////////////////////////////////////////////////
// 채팅창(출력창)을 초기화한다.
//////////////////////////////////////////////////////////////////////////

void CTextList::Init()
{
    memset(m_sTextInfo, 0x00, sizeof(STextInfo) * m_nMaxLine);
    m_nTextLineNum = 0;
}



//////////////////////////////////////////////////////////////////////////
// 저장된 텍스트의 줄 수를 리턴한다.
//
// return: 저장된 텍스트 줄 수
//////////////////////////////////////////////////////////////////////////

int CTextList::GetTextCnt()
{
    return m_nTextLineNum;
}
