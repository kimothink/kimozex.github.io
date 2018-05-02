#include "PokerEvalution.h"
#include "..\dtrace.h"

CPokerEvalution::CPokerEvalution()
{
	InitCard();
	
	memset( m_szJokboCard, 0xff, sizeof(m_szJokboCard) );
	memset( m_szFixJokboCard, 0xff, sizeof(m_szFixJokboCard) );

	m_nJokboCardCnt = 0;
	m_nJackPot = 0;
	m_nFixJokboPos = 0;
}

CPokerEvalution::~CPokerEvalution()
{
}

int CPokerEvalution::Calcul(const BYTE* pCard, const int nCard)
{
	for (int i = 0 ; i < nCard ; ++i)
		m_nCard[i] = pCard[i];
	
	m_nHaveCnt = nCard;
	SortArray(m_nCard, m_nHaveCnt);	// 높은 카드 부터 순서대로 변경
	
	return Evalution();
}


bool CPokerEvalution::operator<(const CPokerEvalution &rValue)
{
	for (int i = 0 ; i < MAX_EVALUTON_CNT ; ++i)
	{
		if (m_nEvalValues[i] == rValue.m_nEvalValues[i])
			continue;
			
		if (m_nEvalValues[i] < rValue.m_nEvalValues[i])
			return true;

		return false;
	}
	return false;
}

bool CPokerEvalution::operator>(const CPokerEvalution &rValue)
{
	for (int i = 0 ; i < MAX_EVALUTON_CNT ; ++i)
	{
		if ( m_nEvalValues[i] < 0 )
			return TRUE;

		if ( i == 0 ) 
		{
			if (m_nEvalValues[i] > rValue.m_nEvalValues[i])
			{
				m_nRank++;
				return TRUE;
			}
			else if ( m_nEvalValues[i] < rValue.m_nEvalValues[i])
				return TRUE;
		}
		else
		{
			if (m_nEvalValues[i] % 13 > rValue.m_nEvalValues[i] % 13) 
			{
				m_nRank++;
				return TRUE;
			}
			else if ( m_nEvalValues[i] % 13 < rValue.m_nEvalValues[i] % 13) 
				return TRUE;
		}
	}

	return TRUE;
}

bool CPokerEvalution::operator==(const CPokerEvalution &rValue)
{
	for (int i = 0 ; i < MAX_EVALUTON_CNT ; ++i)
	{
		if (m_nEvalValues[i] == rValue.m_nEvalValues[i])
			continue;
		
		return false;
	}
	return true;
}

void CPokerEvalution::InitCard()
{
	m_nHaveCnt = 0;
	InitEvalution();
}


void CPokerEvalution::BadukiAI()
{
	
	int checkSharp[4];
	int sharp;
	int i, j, k;
	int checkNum[13];
	int nOneCardNo = 0;
	int nOneCardShrp = 0;
	int nOneCardDiviNo = 0;
	int nFrCardNo = 0;
	int nSeCardNo = 0;
	int nTemp = 0;
	BOOL bTowNo = FALSE;
	BOOL bFrEqualCard = FALSE;
	BOOL bSeEqualCard = FALSE;
	BOOL bBothEqualCard = FALSE;
	int nTemp2 = 0;
	int nSpareFrCardNo = 0;
	int nSpareSeCardNo = 0;
	
	memset( checkNum, 0x00, sizeof(checkNum) );

	for (i = 0 ; i < m_nHaveCnt ; ++i)
		++checkNum[m_nCard[i] % 13];

	for (i = 0 ; i < 13 ; ++i) 
	{
		if (checkNum[i] >= 4)  
		{
			m_nEvalValues[m_nValPos++] = BADUKI_TOP;
			m_nEvalValues[m_nValPos++] = i;
			return;
		}
	}

	for ( i = 0 ; i < 13 ; ++i) {
		checkNum[i] = 0;
	}

	for (sharp = 0 ; sharp < 4 ; ++sharp)
		checkSharp[sharp] = 0;
	
	for (i = 0 ; i < m_nHaveCnt ; ++i)
		++checkSharp[m_nCard[i] / 13];

	for (sharp = 0 ; sharp < 4 ; ++sharp) {
		if (checkSharp[sharp] >= 4)  {
			m_nEvalValues[m_nValPos++] = BADUKI_TOP;

			for ( j = m_nHaveCnt - 1; j >= 0; j--  ) 
			{
				if ( m_nCard[j] >= 0 )
				{
					m_nEvalValues[m_nValPos++] = m_nCard[j];
					return;
				}
			}
		}
	}

	nOneCardNo = 0;
	nOneCardShrp = 0;
	nOneCardDiviNo = 0;

	for ( i = 0 ; i < 13 ; ++i) {
		checkNum[i] = 0;
	}
	for (sharp = 0 ; sharp < 4 ; ++sharp)
		checkSharp[sharp] = 0;


	for (i = 0 ; i < m_nHaveCnt ; ++i)
		++checkNum[m_nCard[i] % 13];

	for (i = 0 ; i < 13 ; ++i) 
	{
		if (checkNum[i] >= 3)  
		{
			m_nEvalValues[m_nValPos++] = BADUKI_TWO_BASE;


	
			for ( j = 0; j < m_nHaveCnt; j++ ) 
			{
				if ( m_nCard[j] % 13 != i ) {	
					nOneCardNo = m_nCard[j];	
					nOneCardShrp = m_nCard[j] / 13;	
					break;
				}
			}

			for ( j = 0; j < m_nHaveCnt; j++ ) 
			{
				if ( m_nCard[j] / 13 != nOneCardShrp ) 
				{
					if ( m_nCard[j] > nOneCardNo ) {
						m_nEvalValues[m_nValPos++] = m_nCard[j];
						m_nEvalValues[m_nValPos++] = nOneCardNo;
					}
					else {
						m_nEvalValues[m_nValPos++] = nOneCardNo;
						m_nEvalValues[m_nValPos++] = m_nCard[j];
					}

					return;
				}

			}

			return;
		}
	}	

	for ( i = 0 ; i < 13 ; ++i) {
		checkNum[i] = 0;
	}
	for (sharp = 0 ; sharp < 4 ; ++sharp)
		checkSharp[sharp] = 0;


	for (i = 0 ; i < m_nHaveCnt ; ++i)
		++checkSharp[m_nCard[i] / 13];

	for (sharp = 0 ; sharp < 4 ; ++sharp) {
		if (checkSharp[sharp] >= 3)  {
			m_nEvalValues[m_nValPos++] = BADUKI_TWO_BASE;

			for ( j = 0; j < m_nHaveCnt; j++ ) 
			{
				if ( (m_nCard[j] / 13) != sharp ) 
				{
					nOneCardNo = m_nCard[j];
					nOneCardDiviNo = m_nCard[j] % 13;
					break;
				}
			}

			for ( j = m_nHaveCnt - 1; j >= 0; j--  ) 
			{
				if ( (m_nCard[j] / 13) == sharp ) 
				{
					if ( m_nCard[j] != nOneCardNo )
					{
						if ( m_nCard[j] > nOneCardNo ) {
							m_nEvalValues[m_nValPos++] = m_nCard[j];
							m_nEvalValues[m_nValPos++] = nOneCardNo;
						}
						else {
							m_nEvalValues[m_nValPos++] = nOneCardNo;
							m_nEvalValues[m_nValPos++] = m_nCard[j];
						}

						return;						


					}
				}
			}
		}
	}	

	nFrCardNo = 0;
	nSeCardNo = 0;
	nTemp = 0;
	bTowNo = FALSE;


	for ( i = 0 ; i < 13 ; ++i) {
		checkNum[i] = 0;
	}
	for (sharp = 0 ; sharp < 4 ; ++sharp)
		checkSharp[sharp] = 0;

	for (i = 0 ; i < m_nHaveCnt ; ++i)
		++checkNum[m_nCard[i] % 13];
	
	for ( i = 12 ; i >= 0 ; i-- ) 
	{
		if (checkNum[i] >= 2)
		{
			bTowNo = TRUE;
			break;
		}
	}

	if ( bTowNo == TRUE )
	{
		m_nEvalValues[m_nValPos++] = BADUKI_TOP;

		nTemp = 0;
	
		for ( j = m_nHaveCnt - 1; j >= 0; j-- )	
		{
			if ( m_nCard[j] % 13 == i ) 
			{
				if ( nTemp == 0 )
					nFrCardNo = m_nCard[j];
				else 
					nSeCardNo = m_nCard[j];

				nTemp++;

				if ( nTemp >= 2  )
					break;
			}
		}


		bFrEqualCard = FALSE;
		bSeEqualCard = FALSE;
		bBothEqualCard = FALSE;

		for ( j = m_nHaveCnt - 1; j >= 0; j-- )			
		{

			if ( m_nCard[j] != nFrCardNo && m_nCard[j] != nSeCardNo  
				 && m_nCard[j] / 13 == nFrCardNo / 13 )
			{
				bFrEqualCard = TRUE;
				break;
			}
		}


		for ( k = m_nHaveCnt - 1; k >= 0; k-- )	
		{
			if ( m_nCard[k] != nFrCardNo && m_nCard[k] != nSeCardNo  
				 && m_nCard[k] / 13 == nSeCardNo / 13 )
			{
				bSeEqualCard = TRUE;
				break;
			}
		}

		if ( bFrEqualCard == FALSE || bSeEqualCard == FALSE )
		{
			if ( bFrEqualCard == FALSE )
			{
				m_nEvalValues[m_nValPos++] = nFrCardNo;
			}
			else if ( bSeEqualCard == FALSE  ) 
			{
				m_nEvalValues[m_nValPos++] = nSeCardNo;
			}

			for ( j = m_nHaveCnt - 1; j >= 0; j-- )
			{
				if ( m_nCard[j] != nFrCardNo && m_nCard[j] != nSeCardNo )
				{
					if ( nTemp2 == 0 )
					{
						nSpareFrCardNo = m_nCard[j];
					}
					else
					{
						nSpareSeCardNo = m_nCard[j];
					}

					nTemp2++;

					if ( nTemp2 >= 2 )
						break;
				}
			}

			if ( nSpareFrCardNo % 13 != nSpareSeCardNo % 13 && 
				 nSpareFrCardNo / 13 != nSpareSeCardNo / 13 )
			{
				m_nEvalValues[m_nValPos++] = nSpareFrCardNo;
				m_nEvalValues[m_nValPos++] = nSpareSeCardNo;
			}
			else
			{
				if ( nSpareFrCardNo % 13 < nSpareSeCardNo % 13 )
					m_nEvalValues[m_nValPos++] = nSpareFrCardNo;
				else
					m_nEvalValues[m_nValPos++] = nSpareSeCardNo;
				
			}
		}
		else
		{
			nTemp2 = 0;
			nSpareFrCardNo = 0;
			nSpareSeCardNo = 0;

			for ( j = m_nHaveCnt - 1; j >= 0; j-- )
			{
				if ( m_nCard[j] != nFrCardNo && m_nCard[j] != nSeCardNo )
				{
					if ( nTemp2 == 0 )
					{
						nSpareFrCardNo = m_nCard[j];
					}
					else
					{
						nSpareSeCardNo = m_nCard[j];
					}

					nTemp2++;

					if ( nTemp2 >= 2 )
						break;
				}
			}

			if ( nSpareFrCardNo % 13 != nSpareSeCardNo % 13 && 
				 nSpareFrCardNo / 13 != nSpareSeCardNo / 13 )
			{

				if ( nSpareFrCardNo % 13 < nSpareSeCardNo % 13 )
				{
					m_nEvalValues[m_nValPos++] = nSpareFrCardNo;

					if ( nSpareSeCardNo % 13 < nFrCardNo % 13 )
					{
						m_nEvalValues[m_nValPos++] = nSpareSeCardNo;
					}
					else
					{	
						if ( nSpareFrCardNo / 13 != nFrCardNo / 13 )
						{
							m_nEvalValues[m_nValPos++] = nFrCardNo;
						}
						else
						{
							m_nEvalValues[m_nValPos++] = nSeCardNo;
						}
					}
						
				}
				else
				{
					m_nEvalValues[m_nValPos++] = nSpareSeCardNo;

					if ( nSpareSeCardNo / 13 != nFrCardNo / 13 )
					{
						m_nEvalValues[m_nValPos++] = nFrCardNo;
					}
					else
					{
						m_nEvalValues[m_nValPos++] = nSeCardNo;
					}

				}

			}
			else	
			{
				m_nEvalValues[m_nValPos++] = nSpareFrCardNo;

				if ( nSpareFrCardNo / 13 != nFrCardNo / 13 )
					m_nEvalValues[m_nValPos++] = nFrCardNo;
				else
					m_nEvalValues[m_nValPos++] = nSeCardNo;
			}
		}
	
		nTemp = 0;
		for ( k = MAX_EVALUTON_CNT - 1; k > 0; k-- )
		{
			if ( m_nEvalValues[k] >= 0 ) 
			{
				nTemp++;
			}
		}

		switch(nTemp) {
			case 3:
				m_nEvalValues[0] = BADUKI_BASE;
				break;

			case 2:
				m_nEvalValues[0] = BADUKI_TWO_BASE;
				break;

			case 1:
				m_nEvalValues[0] = BADUKI_TOP;
				break;
			default:
				m_nEvalValues[0] = BADUKI_ERROR;
				break;
		}

		return;


	}

	nFrCardNo = 0;
	nSeCardNo = 0;
	nTemp = 0;
	bTowNo = FALSE;

	for ( i = 0 ; i < 13 ; ++i) {
		checkNum[i] = 0;
	}
	for (sharp = 0 ; sharp < 4 ; ++sharp)
		checkSharp[sharp] = 0;

	for (i = 0 ; i < m_nHaveCnt ; ++i)
		++checkSharp[m_nCard[i] / 13];


	for (sharp = 0 ; sharp < 4 ; ++sharp) 
	{
		if (checkSharp[sharp] >= 2)  
		{
			bTowNo = TRUE;
			break;
		}
	}

	if ( bTowNo == TRUE )
	{

		m_nEvalValues[m_nValPos++] = BADUKI_TOP;

		for ( j = m_nHaveCnt - 1; j >= 0; j-- )	
		{
			if ( m_nCard[j] / 13 == sharp ) 
			{
				if ( nTemp == 0 )
					nFrCardNo = m_nCard[j];
				else 
					nSeCardNo = m_nCard[j];

				nTemp++;

				if ( nTemp >= 2  )
					break;
			}
		}
		
		if ( nFrCardNo < nSeCardNo ) 
		{
			m_nEvalValues[m_nValPos++] = nFrCardNo;
		}
		else
		{
			m_nEvalValues[m_nValPos++] = nSeCardNo;
		}

		nSpareFrCardNo = 0;
		nSpareSeCardNo = 0;

		nTemp = 0;

		for (  j = m_nHaveCnt - 1; j >= 0; j-- )	
		{
			if ( m_nCard[j] != nFrCardNo && m_nCard[j] != nSeCardNo )
			{
				if ( nTemp == 0 )
					nSpareFrCardNo = m_nCard[j];
				else 
					nSpareSeCardNo = m_nCard[j];

				nTemp++;

				if ( nTemp >= 2  )
					break;				
			}
		}

		if ( nSpareFrCardNo / 13 == nSpareSeCardNo / 13 )
		{

			if ( nSpareFrCardNo < nSpareSeCardNo )
			{
				m_nEvalValues[m_nValPos++] = nSpareFrCardNo;
			}
			else
			{
				m_nEvalValues[m_nValPos++] = nSpareSeCardNo;
			}
		}
		else
		{
			m_nEvalValues[m_nValPos++] = nSpareFrCardNo;
			m_nEvalValues[m_nValPos++] = nSpareSeCardNo;
		}

		nTemp = 0;
		for ( k = MAX_EVALUTON_CNT - 1; k > 0; k-- )
		{
			if ( m_nEvalValues[k] >= 0 ) 
			{
				nTemp++;
			}
		}

		switch(nTemp) {
			case 3:
				m_nEvalValues[0] = BADUKI_BASE;
				break;

			case 2:
				m_nEvalValues[0] = BADUKI_TWO_BASE;
				break;

			case 1:
				m_nEvalValues[0] = BADUKI_TOP;
				break;
			default:
				m_nEvalValues[0] = BADUKI_ERROR;
				break;
		}

		return;

	}


	if ( ( m_nCard[0] % 13 ) == 3 && ( m_nCard[1] % 13 ) == 2 && 
		 ( m_nCard[2] % 13 ) == 1 && ( m_nCard[3] % 13 ) == 0  )
	{
		m_nEvalValues[m_nValPos++] = BADUKI_MADE_GOLF;
	}
	else if ( ( m_nCard[0] % 13 ) == 4 && ( m_nCard[1] % 13 ) == 2 && 
			  ( m_nCard[2] % 13 ) == 1 && ( m_nCard[3] % 13 ) == 0  )
	{
		m_nEvalValues[m_nValPos++] = BADUKI_MADE_SECOND;
	}
	else if ( ( m_nCard[0] % 13 ) == 4 && ( m_nCard[1] % 13 ) == 3 && 
			  ( m_nCard[2] % 13 ) == 1 && ( m_nCard[3] % 13 ) == 0  )
	{
		m_nEvalValues[m_nValPos++] = BADUKI_MADE_THIRD;
	}
	else 
	{
		m_nEvalValues[m_nValPos++] = BADUKI_MADE;
	}

	for ( i = 0 ; i < m_nHaveCnt ; ++i)
		m_nEvalValues[m_nValPos++] = m_nCard[i];

	return;
	


}

void CardLog( BYTE nCardNo )
{
	char pTemp[255];
	memset( pTemp, 0x00, sizeof(pTemp) );

	switch( nCardNo / 13 ) {
		case 0:
			sprintf( pTemp, "종류 : 크로버, %d", nCardNo % 13 + 1 );
			break;
		
		case 1:
			sprintf( pTemp, "종류 : 하트, %d", nCardNo % 13 + 1 );
			break;
		
		case 2:
			sprintf( pTemp, "종류 : 다이아, %d", nCardNo % 13 + 1 );
			break;

		case 3:
			sprintf( pTemp, "종류 : 스페이드, %d", nCardNo % 13 + 1 );
			break;
	}
	DTRACE( "%s", pTemp );
}

int CPokerEvalution::Evalution()
{
	InitEvalution();

	BadukiAI();

	SortArray2( m_nEvalValues, m_nValPos );	// 높은 카드 부터 다시한번 정렬.

	m_szJokboCard[0] = m_nEvalValues[0];
	m_szJokboCard[1] = m_nEvalValues[1];
	m_szJokboCard[2] = m_nEvalValues[2];
	m_szJokboCard[3] = m_nEvalValues[3];
	m_szJokboCard[4] = m_nEvalValues[4];



#ifdef _DEBUG

	char szTemp[255];
	int nTemp[7];
	memset( nTemp, 0xff, sizeof(nTemp) );

	switch(m_nEvalValues[0]) {
		case 7:
			strcpy( szTemp, "BADUKI_ERROR" );
			break;

		case 6:
			strcpy( szTemp, "BADUKI_TOP" );
			break;

		case 5:
			strcpy( szTemp, "BADUKI_TWO_BASE" );
			break;

		case 4:
			strcpy( szTemp, "BADUKI_BASE" );
			break;

		case 3:
			strcpy( szTemp, "BADUKI_MADE" );
			break;

		case 2:
			strcpy( szTemp, "BADUKI_MADE_THIRD" );
			break;

		case 1:
			strcpy( szTemp, "BADUKI_MADE_SECOND" );
			break;

		case 0:
			strcpy( szTemp, "BADUKI_MADE_GOLF" );
			break;
	}


	DTRACE( "%s", szTemp );


	CardLog( m_nEvalValues[1] );
	CardLog( m_nEvalValues[2] );
	CardLog( m_nEvalValues[3] );
	CardLog( m_nEvalValues[4] );

	
#endif

	return m_nEvalValues[0];

}


void CPokerEvalution::InitEvalution()
{

	for (int i = 0 ; i < MAX_EVALUTON_CNT ; ++i)
		m_nEvalValues[i] = -1;

	m_nEvalValues[0] = BADUKI_TOP;

	
	memset( m_szFixJokboCard, 0xff, sizeof(m_szFixJokboCard) );

	m_nValPos = 0;
	m_nFixJokboPos = 0;
	m_nJokboCardCnt = 0;
}


int CPokerEvalution::CheckFlushEtc()
{

	int checkSharp[4];
	int sharp;
	int i;
	int nTemp;
	
	for (sharp = 0 ; sharp < 4 ; ++sharp)
		checkSharp[sharp] = 0;
	
	for (i = 0 ; i < m_nHaveCnt ; ++i)

		++checkSharp[m_nCard[i] / 13];

	for (sharp = 0 ; sharp < 4 ; ++sharp)
		if (checkSharp[sharp] > 4)
			break;
	
	if (sharp < 4)		
	{
		int straight;


		straight = CheckStraight (sharp);
		if ( straight != -1 )	
		{

			if (straight == 12) {
				m_nEvalValues[m_nValPos++] = POKER_ROYAL_STRAIGHT_FLUSH;
				
				EvalJokbo( POKER_ROYAL_STRAIGHT_FLUSH, TRUE );
				m_nJackPot = 2;
			}
			else {
				m_nEvalValues[m_nValPos++] = POKER_STRAIGHT_FLUSH; 

				EvalJokbo( POKER_STRAIGHT_FLUSH, TRUE );
				m_nJackPot = 1;
			}


			for (i = 0 ; i < m_nHaveCnt ; ++i)
			{
	
				if (m_nCard[i] / 13 != sharp)
					continue;

				m_nEvalValues[m_nValPos++] = m_nCard[i] % 13;

				m_nEvalValues[m_nValPos++] = m_nCard[i];

				break;
			}


			int nCurStraight = straight;

			for (i = 0 ; i < m_nHaveCnt ; ++i) {

				if (m_nCard[i] / 13 != sharp) continue;
				if ( straight == 3 ) {
					
					nCurStraight = 12; 

					for ( i = 0; i < m_nHaveCnt; i++ ) {

						if ( m_nFixJokboPos < 5 && m_nCard[i] % 13 == nCurStraight ) {
							m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[i];

							if ( nCurStraight == 12 )
								nCurStraight = 4;

							nCurStraight--;		
						}
					}
				}
				else {
					for ( i = 0; i < m_nHaveCnt; i++ ) {

						if ( m_nFixJokboPos < 5 && m_nCard[i] % 13 == nCurStraight ) {
							m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[i];
							nCurStraight--;		
												
						}
					}
				}
			}

		}
		else
		{
			m_nEvalValues[m_nValPos++] = POKER_FLUSH;
			EvalJokbo( POKER_FLUSH, TRUE );


			for (i = 0 ; i < m_nHaveCnt ; ++i)
			{

				nTemp = m_nCard[i] / 13;

				if (nTemp != sharp)
					continue;

				m_nEvalValues[m_nValPos++] = m_nCard[i] % 13;


				if ( m_nFixJokboPos < 5 )
					EvalJokbo(m_nCard[i]);


				if ( m_nFixJokboPos < 5 )
					m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[i];
			}
			for (i = 0 ; i < m_nHaveCnt ; ++i)
			{
				nTemp = m_nCard[i] / 13;

				if (nTemp != sharp)
					continue;

				m_nEvalValues[m_nValPos++] = m_nCard[i];
				break;
			}			
		}
	}
	return m_nEvalValues[0];
}

void CPokerEvalution::EvalJokbo( BYTE nCardNo, BOOL bJobko )
{
	if ( bJobko ) {
		m_szJokboCard[0] = nCardNo;
	}
	else {
		m_szJokboCard[++m_nJokboCardCnt] = nCardNo;
	}
}

int CPokerEvalution::CheckStraight()
{
	int straight = CheckStraight(-1);
	
	if (straight != -1)
	{


		if (straight == 12) {
			m_nEvalValues[m_nValPos++] = POKER_MOUNTAIN;
			EvalJokbo( POKER_MOUNTAIN, TRUE );
		}
		else if (straight == 3) {
			m_nEvalValues[m_nValPos++] = POKER_BACK_STRAIGHT;
			EvalJokbo( POKER_BACK_STRAIGHT, TRUE );
		}
		else {
			m_nEvalValues[m_nValPos++] = POKER_STRAIGHT;
			EvalJokbo( POKER_STRAIGHT, TRUE );
		}

		for (int i = 0 ; i < m_nHaveCnt ; ++i)
		{
			if ( m_nCard[i] % 13 == straight )
			{
				m_nEvalValues[m_nValPos++] = m_nCard[i] % 13;

				m_nEvalValues[m_nValPos++] = m_nCard[i];
				break;
			}
		}

	
		int nCurStraight = straight;


		if ( straight == 3 ) {
			
			nCurStraight = 12; 

			for ( i = 0; i < m_nHaveCnt; i++ ) {

				if ( m_nFixJokboPos < 5 && m_nCard[i] % 13 == nCurStraight ) {
					m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[i];

					if ( nCurStraight == 12 )
						nCurStraight = 4;

					nCurStraight--;		
				}
			}
		}
		else {
			for ( i = 0; i < m_nHaveCnt; i++ ) {

				if ( m_nFixJokboPos < 5 && m_nCard[i] % 13 == nCurStraight ) {
					m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[i];
					nCurStraight--;		
				}
			}
		}
	}
	return m_nEvalValues[0];
}


int CPokerEvalution::CheckStraight(int sharp)
{
	int straight = -1;
	int checkNum[13];

	for (int i = 0 ; i < 13 ; ++i) {
		checkNum[i] = 0;
	}
	
	for (i = 0 ; i < m_nHaveCnt ; ++i)
	{
		if (sharp != -1 && (m_nCard[i] / 13) != sharp)
			continue;

		++checkNum[m_nCard[i] % 13];
	}


	if ( checkNum[8] > 0 && checkNum[9] > 0 && checkNum[10] > 0 && checkNum[11] > 0 && checkNum[12] > 0 ) {
		EvalJokbo(12);
		EvalJokbo(11);
		EvalJokbo(10);
		EvalJokbo(9);
		EvalJokbo(8);

		return 12;
	} 
	else if ( checkNum[12] > 0 && checkNum[0] > 0 && checkNum[1] > 0 && checkNum[2] > 0 && checkNum[3] > 0 ) {
		EvalJokbo(12);
		EvalJokbo(0);
		EvalJokbo(1);
		EvalJokbo(2);
		EvalJokbo(3);

		return 3;
	}

	
	int cnt;
	for (i = 12 ; i > 2 ; --i)
	{
		cnt = 0;
		for (int j = 0 ; j < 5 ; ++j)
		{
			if (checkNum[(i-j+13)%13] > 0) {
				++cnt;
				
				EvalJokbo( (i-j+13)%13 );
			}
			else
				break;
		}

		if (cnt >= 5)
		{
			straight = i;
			break;
		}
		else {
			memset( m_szJokboCard, 0xff, sizeof(m_szJokboCard) );
			m_nJokboCardCnt = 0;
		}
	}
	return straight;
}

int CPokerEvalution::CheckPair()
{

	int checkNum[13];
	int pair[5];
	int nTempCardNo = 0;
	
	for (int i = 0 ; i < 13 ; ++i)
	{
		checkNum[i] = 0;
		if (i < 5)
			pair[i] = 0;
	}

	for (i = 0 ; i < m_nHaveCnt ; ++i)

		++checkNum[m_nCard[i] % 13];
 

	for (i = 0 ; i < 13 ; ++i)
		pair[checkNum[i]]++;

	if (pair[4] >= 1)
	{
		m_nEvalValues[m_nValPos++] = POKER_FOUR_CARD;
		EvalJokbo( POKER_FOUR_CARD, TRUE );

		for (i = 12 ; i >= 0 ; --i)
		{
			if ( checkNum[i] == 4 ) {
				EvalJokbo( i );


				for ( int nq = 0; nq < m_nHaveCnt; nq++ )
				{
					if ( m_nCard[nq] % 13 == i )
						m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
				}

				break;
			}
		}
		m_nEvalValues[m_nValPos++] = i;




	}
	else if (pair[3] >= 2)
	{
		m_nEvalValues[m_nValPos++] = POKER_FULL_HOUSE;
		EvalJokbo( POKER_FULL_HOUSE, TRUE );
		for (i = 12 ; i >= 0 ; --i)
		{
			if (checkNum[i] == 3) {
				m_nEvalValues[m_nValPos++] = i;
				EvalJokbo(i);

				for (int nq = 0 ; nq < m_nHaveCnt ; nq++)
				{
					if ( m_nFixJokboPos < 5 &&m_nCard[nq] % 13 == i )
						m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
				}

			}
		}



	}
	else if (pair[3] >= 1)
	{
		if (pair[2] >= 1)
		{
			m_nEvalValues[m_nValPos++] = POKER_FULL_HOUSE;
			EvalJokbo( POKER_FULL_HOUSE, TRUE );
			for (i = 12 ; i >= 0 ; --i)
			{
				if (checkNum[i] == 3) {
					EvalJokbo(i);
					m_nEvalValues[m_nValPos++] = i;



					for (int nq = 0 ; nq < m_nHaveCnt ; nq++)
					{
						if ( m_nFixJokboPos < 5 &&m_nCard[nq] % 13 == i )
							m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
					}

				}

				if (checkNum[i] == 2) { 
					
					for (int nq = 0 ; nq < m_nHaveCnt ; nq++)
					{
						if ( m_nFixJokboPos < 5 &&m_nCard[nq] % 13 == i )
							m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
					}
				}
			}


	
		}
		else
		{
			m_nEvalValues[m_nValPos++] = POKER_THREE_CARD;
			EvalJokbo( POKER_THREE_CARD, TRUE );
			for (i = 12 ; i >= 0 ; --i)
			{
				if (checkNum[i] == 3) {
					m_nEvalValues[m_nValPos++] = i;
					EvalJokbo(i);
					
					for (int nq = 0 ; nq < m_nHaveCnt ; nq++)
					{
						if ( m_nFixJokboPos < 5 &&m_nCard[nq] % 13 == i )
							m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
					}

					break;
				}
			}
		}
	}
	else if (pair[2] >= 2)
	{
		int nTemp = 0;
		int nPairCardNo[2] = {0, };

		m_nEvalValues[m_nValPos++] = POKER_TWO_PAIR;
		EvalJokbo( POKER_TWO_PAIR, TRUE );
		for (i = 12 ; i >= 0 ; --i)
		{
			if (checkNum[i] == 2)
			{
				m_nEvalValues[m_nValPos++] = i;

				EvalJokbo(i);
				
				nPairCardNo[nTemp] = i;

				nTemp++;		
				

				for (int nq = 0 ; nq < m_nHaveCnt ; nq++)
				{
					if ( m_nFixJokboPos < 4 && m_nCard[nq] % 13 == i )
						m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
				}

				if ( nTemp >= 2 ) 
					break;
			}
		}

		for (int j = 0 ; j < m_nHaveCnt ; ++j)
		{
			if ( m_nCard[j] % 13 == nPairCardNo[0] )
			{
				m_nEvalValues[m_nValPos++] = m_nCard[j];
				break;
			}
		}
	}
	else if (pair[2] >= 1)
	{
		m_nEvalValues[m_nValPos++] = POKER_ONE_PAIR;
		EvalJokbo( POKER_ONE_PAIR, TRUE );

		for (i = 12 ; i >= 0 ; --i)
		{
			if (checkNum[i] == 2)
			{
				m_nEvalValues[m_nValPos++] = i;

				EvalJokbo(i);

				for (int nq = 0 ; nq < m_nHaveCnt ; nq++)
				{
					if ( m_nFixJokboPos < 2 &&m_nCard[nq] % 13 == i )
						m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[nq];
				}

				break;
			}
		}

		
		for ( int j = 0 ; j < m_nHaveCnt ; ++j )
		{
			if ( m_nCard[j] % 13 != i )	
			{
				m_nEvalValues[m_nValPos++] = m_nCard[j];
			}
		}
	}
	else {
		m_nEvalValues[m_nValPos++] = POKER_NO_PAIR;
		for (i = 12 ; i >= 0 ; --i)
		{
			if (checkNum[i] >= 1) {
				EvalJokbo( i );				
				break;
			}
		}
		EvalJokbo( POKER_NO_PAIR, TRUE );
	}
	
	return m_nEvalValues[0];
}

void CPokerEvalution::SortArray(int *buff, int no )
{
	int tmp;
	int nChangeCardNo = 0;
	int nTempChangeCardNo = 0;
	int j = 0;

	for ( j = 0; j < no - 1 ; ++j)
	{
		tmp = j;
		for (int i = j + 1 ; i < no ; ++i)
		{
			nTempChangeCardNo = buff[tmp] % 13;
			nChangeCardNo = buff[i] % 13;

			if ( nTempChangeCardNo < nChangeCardNo ) {
				tmp = i;
			}
			else if ( nTempChangeCardNo == nChangeCardNo ) { 
				if (buff[tmp] < buff[i])
					tmp = i;
			}
		}

		if (tmp != j)
		{
			int tt = buff[tmp];
			buff[tmp] = buff[j];
			buff[j] = tt;
		}
	}
}


void CPokerEvalution::SortArray2(int *buff, int no )
{
	int tmp;
	int nChangeCardNo = 0;
	int nTempChangeCardNo = 0;
	int j = 0;

	for ( j = 1; j < no - 1; ++j)
	{
		tmp = j;
		for (int i = j + 1 ; i < no ; ++i)
		{
			nTempChangeCardNo = buff[tmp] % 13;
			nChangeCardNo = buff[i] % 13;

			if ( nTempChangeCardNo < nChangeCardNo ) {
				tmp = i;
			}

		}

		if (tmp != j)
		{
			int tt = buff[tmp];
			buff[tmp] = buff[j];
			buff[j] = tt;
		}
	}
}
