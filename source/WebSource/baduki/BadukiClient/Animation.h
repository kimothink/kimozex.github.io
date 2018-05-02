// Animation.h: interface for the CAnimation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATION_H__BBB15B00_D9DF_4E18_9557_1F3B10FE3660__INCLUDED_)
#define AFX_ANIMATION_H__BBB15B00_D9DF_4E18_9557_1F3B10FE3660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientDefine.h"
#include "..\BadukiCommonDefine\BadukiCommonDefine.h"



class CAnimation  
{
public:

	CAnimation();
	virtual ~CAnimation();

	void AddStack(sUserCardInfo *card);
	void DeleteStack(int nStackNo);
	void EmptyStack();
	sUserCardInfo* GetCard(int nStackNo);
	int GetAniCount();
	bool CheckCard(sUserCardInfo *card);

public:

	int m_nStackTop;
	sUserCardInfo *m_card[MAX_CARD];
};

#endif // !defined(AFX_ANIMATION_H__BBB15B00_D9DF_4E18_9557_1F3B10FE3660__INCLUDED_)
