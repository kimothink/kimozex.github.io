// Animation.h: interface for the CChipAni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CHIP_ANI_H__)
#define __CHIP_ANI_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientDefine.h"
#include "..\PokerCommonDefine\PokerCommonDefine.h"



class CChipAni  
{
public:

	CChipAni();
	virtual ~CChipAni();

	void        AddStack( sChipInfo *chip );
	void        DeleteStack( int nStackNo );
	void        EmptyStack();
	sChipInfo  *GetChip( int nStackNo );
	int         GetAniCount();
	bool        CheckChip( sChipInfo *chip );

public:
	int         m_nStackTop;
	sChipInfo   m_chip[MAX_CHIP_ANI];
};

#endif // !defined(__CHIP_ANI_H__)
