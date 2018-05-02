
#ifndef __CTEXTLIST_H__
#define __CTEXTLIST_H__



#include "ClientDefine.h"


class CTextList
{
private:

    // 객체가 생성될 때 한번만 초기화되는 멤버
    
    int         m_nMaxTextLen;              // 한 줄의 최대 길이
    int         m_nMaxLine;                 // 저장 가능한 최대 줄 수

    // 필요에 따라 초기화되는 멤버

    STextInfo  *m_sTextInfo;                // 저장된 텍스트
    int         m_nTextLineNum;             // 저장된 줄 수

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
