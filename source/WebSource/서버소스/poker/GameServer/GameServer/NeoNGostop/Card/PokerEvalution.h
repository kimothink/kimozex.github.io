#ifndef __POKER_EVALUTON_H__
#define __POKER_EVALUTON_H__

#include "..\Common.h"
#include "..\define.h"

class CPokerEvalution
{
public:
	CPokerEvalution();
	virtual ~CPokerEvalution();

	BYTE m_szJokboCard[8];	
	BYTE m_szFixJokboCard[8];	
	int m_nJokboCardCnt;
	ush_int	m_nJackPot;		


	int  Calcul(const BYTE* pCard, const int nCard);
	int	 GetEval()	{	return m_nEvalValues[0];	}

	bool operator<(const CPokerEvalution &rValue);
	bool operator>(const CPokerEvalution &rValue);
	bool operator==(const CPokerEvalution &rValue);
	

protected:
	void InitCard();
	int Evalution();
	void InitEvalution();
	int CheckFlushEtc();
	int CheckStraight();
	int CheckStraight(int sharp);
	int CheckPair();
	void SortArray(int *buff, int no);
	void EvalJokbo( BYTE nCardNo, BOOL bJobko = FALSE );

protected:
	int 	m_nCard[7];
	int		m_nHaveCnt;
	
	int		m_nValPos;
	int		m_nFixJokboPos;
	int		m_nEvalValues[MAX_EVALUTON_CNT];
};

#endif
