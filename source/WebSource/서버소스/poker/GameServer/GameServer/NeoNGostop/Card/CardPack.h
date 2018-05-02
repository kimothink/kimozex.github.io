#ifndef __CARD_PACK_H__
#define __CARD_PACK_H__

#include "..\Common.h"

class CCardPack
{
public:
	CCardPack(const BYTE cardCnt);
	virtual ~CCardPack();

	void Shuffle();
	int GetCard();
	void Init();

	int	m_nReadNo;
	int m_nCardCnt;
	BYTE *m_pCardPack;
	
};

#endif
