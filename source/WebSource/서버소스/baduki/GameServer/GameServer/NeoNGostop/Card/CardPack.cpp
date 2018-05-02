#include "CardPack.h"
#include "..\\dtrace.h"


CChngeCardPack::CChngeCardPack()
{
}

CChngeCardPack::~CChngeCardPack()
{
}

void CChngeCardPack::Init()
{
	for ( int i = 0; i < 52; i++ )
	{
		m_pChangeCard[i] = 0;
	}

	m_nTop = 0;
}

void CChngeCardPack::Shuffle()
{
	for (BYTE i = 0 ; i < m_nTop ; ++i)
	{
		BYTE j = GetRandom( 0, m_nTop - 1 );
		BYTE tmp = m_pChangeCard[i];
		m_pChangeCard[i] = m_pChangeCard[j];
		m_pChangeCard[j] = tmp;	
	}
}

void CChngeCardPack::AddCard( BYTE bCardNo )
{
	m_pChangeCard[m_nTop++] = bCardNo;

	char szTemp[255];
	memset( szTemp, 0x00, sizeof(szTemp) );

	switch( bCardNo / 13 ) {
		case 0:
			sprintf( szTemp, "종류 : 크로버 %d", bCardNo % 13 + 1 );
			break;
		
		case 1:
			sprintf( szTemp, "종류 : 하트 %d", bCardNo % 13 + 1 );
			break;
		
		case 2:
			sprintf( szTemp, "종류 : 다이아 %d", bCardNo % 13 + 1 );
			break;

		case 3:
			sprintf( szTemp, "종류 : 스페이드 %d", bCardNo % 13 + 1 );
			break;
	}
	DTRACE( "예비카드수 : %d, 예비카드 들어감 %s", m_nTop, szTemp );
}



int CChngeCardPack::GetCard()
{
	int nCardNo = m_pChangeCard[m_nTop];

	m_pChangeCard[m_nTop] = 0;
	m_nTop--;

	Shuffle();	
	
	char szTemp[255];
	memset( szTemp, 0x00, sizeof(szTemp) );

	switch( nCardNo / 13 ) {
		case 0:
			sprintf( szTemp, "종류 : 크로버 %d", nCardNo % 13 + 1 );
			break;
		
		case 1:
			sprintf( szTemp, "종류 : 하트 %d", nCardNo % 13 + 1 );
			break;
		
		case 2:
			sprintf( szTemp, "종류 : 다이아 %d", nCardNo % 13 + 1 );
			break;

		case 3:
			sprintf( szTemp, "종류 : 스페이드 %d", nCardNo % 13 + 1 );
			break;
	}
	DTRACE( "예비카드수 : %d, 예비카드에서 나감 %s", m_nTop, szTemp );



	return nCardNo;
}













CCardPack::CCardPack(const BYTE cardCnt, const BYTE nPakcCnt )
{
	srand( (unsigned)time( NULL ) );
	m_nCardCnt = cardCnt * nPakcCnt;
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
	int nTemp = 0;
	for (BYTE i = 0 ; i < m_nCardCnt ; ++i) {
		m_pCardPack[i] = nTemp;
		nTemp++;

		if ( nTemp >= 52 )
			nTemp = 0;

	}

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

	DTRACE("m_nCardCnt = %d", m_nCardCnt);
	
	
	m_nReadNo  = 0;
}

int CCardPack::GetCard()
{
	return (m_nReadNo < m_nCardCnt) ?  m_pCardPack[m_nReadNo++] : -1;
}

