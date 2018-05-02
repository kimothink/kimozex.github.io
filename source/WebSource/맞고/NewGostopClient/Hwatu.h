#ifndef __HWATU_H__
#define __HWATU_H__

#include "CardPack.h"

class CHwatu
{

public:
	CCardPack *m_pCardPack;

	int m_nDouble;
	int m_nTriple;

public:
	CHwatu(int nDouble = 0, int nTriple = 0);
	virtual ~CHwatu();

	bool Create();
	bool JoinCreate();	//���������ÿ� �̸� ī�带 �����س��´�.

	bool ReCreate(int nDouble = 0, int nTriple = 0);
	void Shuffle();
	int  GetCard();

	int GetPackCnt();
	int  GetCardPack(BYTE *pGetCardPack);
	bool SetCardPack(BYTE *PutCardPack);
	int	 RemainCardCnt();
	

};

#endif

