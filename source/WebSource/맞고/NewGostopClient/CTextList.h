
#ifndef __CTEXTLIST_H__
#define __CTEXTLIST_H__



#include "ClientDefine.h"


class CTextList
{
private:

    // ��ü�� ������ �� �ѹ��� �ʱ�ȭ�Ǵ� ���
    
    int         m_nMaxTextLen;              // �� ���� �ִ� ����
    int         m_nMaxLine;                 // ���� ������ �ִ� �� ��

    // �ʿ信 ���� �ʱ�ȭ�Ǵ� ���

    STextInfo  *m_sTextInfo;                // ����� �ؽ�Ʈ
    int         m_nTextLineNum;             // ����� �� ��

    bool        Add(STextInfo *pTextInfo);

public:
                CTextList(int nMaxLen, int nMaxLine);
    virtual    ~CTextList();
    
    void        AddText(STextInfo *pTextInfo);
    void        Init();
    STextInfo  *GetTextInfo(int nIdx);
    int         GetTextCnt();
};

#endif
