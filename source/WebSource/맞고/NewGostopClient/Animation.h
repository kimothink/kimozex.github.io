// Animation.h: interface for the CAnimation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATION_H__BBB15B00_D9DF_4E18_9557_1F3B10FE3660__INCLUDED_)
#define AFX_ANIMATION_H__BBB15B00_D9DF_4E18_9557_1F3B10FE3660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum CARDTYPE { PACK, FLOOR, EATCARD, USERCARD };

struct CARDPOS
{
	POINT NewPos;
	POINT Pos;
	POINT StartPos;
	CARDTYPE type;
	int nAniCount;
	int nAniMaxCount;
	BOOL bSetAction;
	BOOL bGetOneCardMode;
};

class CAnimation  
{
public:

	CAnimation();
	virtual ~CAnimation();

	void AddStack(CARDPOS *card);
	void DeleteStack(int nStackNo);
	void EmptyStack();
	CARDPOS* GetCard(int nStackNo);
	int GetAniCount();
	bool CheckCard(CARDPOS *card);

public:

	int m_nStackTop;
	CARDPOS *m_card[53];
};

#endif // !defined(AFX_ANIMATION_H__BBB15B00_D9DF_4E18_9557_1F3B10FE3660__INCLUDED_)
