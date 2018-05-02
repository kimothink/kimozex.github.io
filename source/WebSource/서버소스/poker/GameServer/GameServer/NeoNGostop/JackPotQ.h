#ifndef _JACKPOTQ_H_
#define _JACKPOTQ_H_

#include "Common.h"


class CJackPotQ
{
public:
	CJackPotQ();
	virtual ~CJackPotQ();




	bool AddToQ( BigInt biRoomMoney, BigInt biJackPotMoney );
	bool GetFromQ( sJackPotQ *asJackPotQ );

	bool IsFull();
	bool IsEmpty();


	int m_Head;
	int m_Tail;
	bool m_Semaphore;


	sJackPotQ m_sJackPotQ[MAX_JACKPOTQ_NUM];

};








#endif