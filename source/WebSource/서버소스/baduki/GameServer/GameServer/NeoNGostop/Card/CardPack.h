#ifndef __CARD_PACK_H__
#define __CARD_PACK_H__

#include "..\Common.h"


//2006.2.23
class CChngeCardPack{

public:


	CChngeCardPack();
	virtual ~CChngeCardPack();


	BYTE m_pChangeCard[52];
	int m_nTop;
	

	void AddCard( BYTE bCardNo );
	int GetCard();
	void Init();
	void Shuffle();
};



class CCardPack
{
public:
	CCardPack(const BYTE cardCnt, const BYTE nPakcCnt );	// 카드수 , 몇묶음.
	virtual ~CCardPack();

	void Shuffle();
	int GetCard();
	void Init();

	int	m_nReadNo;
	int m_nCardCnt;
	BYTE *m_pCardPack;
	
};

#endif
