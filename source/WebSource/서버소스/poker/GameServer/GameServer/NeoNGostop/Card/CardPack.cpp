#include "CardPack.h"

CCardPack::CCardPack(const BYTE cardCnt)
{
	srand( (unsigned)time( NULL ) );
	m_nCardCnt = cardCnt;
	m_pCardPack = NULL;
	m_nReadNo  = 0;

	m_pCardPack = new BYTE[m_nCardCnt];
	if (m_pCardPack == NULL)
		throw -1;
	
	Init();
}

CCardPack::~CCardPack()
{
	if (m_pCardPack)
		delete [] m_pCardPack;
	
	m_pCardPack = NULL;
}

#ifdef _DEBUG

	#define	CLOVER	0
	#define	HEART	1
	#define DIA		2
	#define SPADE	3


	int imsicard( int nCardNo, int nCardDivi )
	{
		return ( nCardNo - 2 ) + ( 13 * nCardDivi );
	}

#endif

void CCardPack::Init()
{
	for (BYTE i = 0 ; i < m_nCardCnt ; ++i)
		m_pCardPack[i] =i;


}

void CCardPack::Shuffle()
{

	for (BYTE i = 0 ; i < m_nCardCnt ; ++i)
	{
		BYTE j = GetRandom( 0, m_nCardCnt - 1 );
		BYTE tmp = m_pCardPack[i];
		m_pCardPack[i] = m_pCardPack[j];
		m_pCardPack[j] = tmp;
	}

	
	m_nReadNo  = 0;
}

int CCardPack::GetCard()
{
	return (m_nReadNo < m_nCardCnt) ?  m_pCardPack[m_nReadNo++] : -1;
}


