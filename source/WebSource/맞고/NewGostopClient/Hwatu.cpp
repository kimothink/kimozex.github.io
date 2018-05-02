#include "stdafx.h"
#include "Hwatu.h"
#include "dtrace.h"

// public
CHwatu::CHwatu(int nDouble, int nTriple)
{
	m_pCardPack = NULL;
	m_nDouble = nDouble;
	m_nTriple = nTriple;
}

// public
CHwatu::~CHwatu()
{
	if (m_pCardPack)
		delete m_pCardPack;
	m_pCardPack = NULL;
}

// public
bool CHwatu::Create()
{
	if (m_pCardPack)
		delete m_pCardPack;
	m_pCardPack = NULL;
	
	m_pCardPack = new CCardPack(48 + m_nDouble + m_nTriple);
	if (m_pCardPack == NULL)
		return false;

	Shuffle();	Shuffle();	Shuffle();

	return true;
}

bool CHwatu::JoinCreate()	//조인했을시에 미리 카드를 생성해놓는다.
{
	if (m_pCardPack)
		delete m_pCardPack;
	m_pCardPack = NULL;
	
	m_pCardPack = new CCardPack(0);
	if (m_pCardPack == NULL)
		return false;

	return true;
}

// public
bool CHwatu::ReCreate(int nDouble, int nTriple)
{
	m_nDouble = nDouble;
	m_nTriple = nTriple;
	return Create();
}

// public
void CHwatu::Shuffle()
{
	m_pCardPack->Shuffle();
	m_pCardPack->Shuffle();
	m_pCardPack->Shuffle();
}

// public
int CHwatu::GetCard()
{

	BYTE nInitCardNo = 0;
	BYTE nFinalCardNo = m_pCardPack->m_nCardCnt;
	int nGetCard = m_pCardPack->GetCard();
TRACE("\n GetCard : %d", nGetCard);
	if (  (nGetCard < nInitCardNo) || (nGetCard > nFinalCardNo) )
		return -1;

	return nGetCard;
}

// public
int CHwatu::GetCardPack(BYTE *pGetCardPack)
{
	if (!m_pCardPack)
		return 0;
		
	pGetCardPack[0] = 48 + m_nDouble + m_nTriple;
	pGetCardPack[1] = m_nDouble;
	pGetCardPack[2] = m_nTriple;
	
	memcpy(pGetCardPack + 3, m_pCardPack->GetCardPack(), 48 + m_nDouble + m_nTriple);
	
	return 48 + m_nDouble + m_nTriple + 3;
}

// public
bool CHwatu::SetCardPack(BYTE *PutCardPack)
{
	if (PutCardPack[0] != ( 48 + PutCardPack[1] + PutCardPack[2] ))
		return false;
	

	m_nDouble = PutCardPack[1];
	m_nTriple = PutCardPack[2];

	
	return m_pCardPack->SetCardPack( PutCardPack + 3, PutCardPack[0] );
}


int CHwatu::GetPackCnt()	
{	
	return 48 + m_nDouble + m_nTriple;	
}


int	 CHwatu::RemainCardCnt()	
{	
	return m_pCardPack->RemainCardCnt();	
}