
#include "stdafx.h"
#include "CTextList.h"

// DEFAULT VALUE (�߸��� ������ ��ü�� �����Ϸ��� ���� �� ����)

#define DEFAULT_TEXT_LEN        40      // �ؽ�Ʈ �� ���� �ִ� ����
#define DEFAULT_MAX_LINE        30      // �ִ� ���� ������ �ؽ�Ʈ �� ��



//////////////////////////////////////////////////////////////////////////
// ������
//
// input:   nMaxLen = �ؽ�Ʈ �� ���� �ִ� ����
//          nMaxLine = ���� ������ �ؽ�Ʈ �� ��
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
// �Ҹ���
//
// ��ü ������ �������� �Ҵ��� �޸𸮸� �����Ѵ�.
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
// �ؽ�Ʈ�� �߰��Ѵ�.
//
// �Էµ� �ؽ�Ʈ�� ���̰� ���� ���� ������ �ؽ�Ʈ�� ���̺��� �� ���
// ���� �ٷ� ����� �����Ѵ�.
// ä��âó�� �Է��� ���� ���� �ٿ� ���ļ� ���� �� �ֵ��� �̷� �������
// ������ ���̴�.
//
// input: *pTextInfo = �ؽ�Ʈ ���� ����ü�� �ּ�
//
//          typedef struct tagSTextInfo
//          {
//              COLORREF    rgbColor;    // �ؽ�Ʈ�� ����( RGB(red, green, blue) )
//              int         nStyle;      // �ؽ�Ʈ ��Ÿ��(0=����, 1=����)
//              char        szText[255]; // �ؽ�Ʈ
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
            // ���ĺ�
            if (szBuff[nIdx] > 31 && szBuff[nIdx] < 127)
            {
                nIdx++;
                nLen++;
            }

            // �ѱ�
            else
            {
                if (nLen + 2 > m_nMaxTextLen || // ���鿡 �ѱ��� �־ �߸��� ���
                    nIdx + 1 >= nStrLen)        // ���ڿ� ���� ����¥�� �ѱ��� �ִ� ���
                {
                    // ���ڿ��� ���� ���ϴ� ������ �����Ѵ�.
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

        if (szBuff[nIdx] >= 127 &&      // ���ڿ� ���� ����¥�� �ѱ��� �ִ� ���
            (nIdx + 1) >= nStrLen)      // �Է����� �ʰ� �׳� �����Ѵ�.
        {
            return;
        }

        else
        {
            strncpy(sTextInfo.szText, szBuff + nStartIdx, nLen);
            sTextInfo.szText[nLen] = NULL;
            Add(&sTextInfo);

            nStartIdx += nLen;          // ���� ���� �����ϱ� ���� ���� ��ġ �̵�
        }
    }
    //-[E1]
}



//////////////////////////////////////////////////////////////////////////
// �ؽ�Ʈ�� �߰��Ѵ�. (���������θ� ���)
//
// input: *pTextInfo = �ؽ�Ʈ ���� ����ü�� �ּ�
//
//          typedef struct tagSTextInfo
//          {
//              COLORREF    rgbColor;    // �ؽ�Ʈ�� ����( RGB(red, green, blue) )
//              int         nStyle;      // �ؽ�Ʈ ��Ÿ��(0=����, 1=����)
//              char        szText[255]; // �ؽ�Ʈ
//          } STextInfo;
//
// return:  true = �ؽ�Ʈ �� ���� �����ߴ�.
//          false = �ؽ�Ʈ �� ���� �������� �ʾҴ�.(�� ����)
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
// ������ ��ġ�� �ؽ�Ʈ ������ �����Ѵ�.
//////////////////////////////////////////////////////////////////////////

STextInfo *CTextList::GetTextInfo(int nIdx)
{
    if (nIdx < 0 || nIdx >= m_nTextLineNum) return NULL;
    else return &m_sTextInfo[nIdx];
}

    

//////////////////////////////////////////////////////////////////////////
// ä��â(���â)�� �ʱ�ȭ�Ѵ�.
//////////////////////////////////////////////////////////////////////////

void CTextList::Init()
{
    memset(m_sTextInfo, 0x00, sizeof(STextInfo) * m_nMaxLine);
    m_nTextLineNum = 0;
}



//////////////////////////////////////////////////////////////////////////
// ����� �ؽ�Ʈ�� �� ���� �����Ѵ�.
//
// return: ����� �ؽ�Ʈ �� ��
//////////////////////////////////////////////////////////////////////////

int CTextList::GetTextCnt()
{
    return m_nTextLineNum;
}
