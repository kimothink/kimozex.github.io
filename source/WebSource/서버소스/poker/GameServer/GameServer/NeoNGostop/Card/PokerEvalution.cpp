#include "PokerEvalution.h"



// public
CPokerEvalution::CPokerEvalution()
{
	InitCard();
	
	memset( m_szJokboCard, 0xff, sizeof(m_szJokboCard) );
	memset( m_szFixJokboCard, 0xff, sizeof(m_szFixJokboCard) );

	m_nJokboCardCnt = 0;
	m_nJackPot = 0;
	m_nFixJokboPos = 0;
}

// public
CPokerEvalution::~CPokerEvalution()
{
}

// public
int CPokerEvalution::Calcul(const BYTE* pCard, const int nCard)
{
	for (int i = 0 ; i < nCard ; ++i)
		m_nCard[i] = pCard[i];
	
	m_nHaveCnt = nCard;
	SortArray(m_nCard, m_nHaveCnt);	// 높은 카드 부터 순서대로 변경
	
	return Evalution();
}

// public
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

// public
bool CPokerEvalution::operator>(const CPokerEvalution &rValue)
{
	for (int i = 0 ; i < MAX_EVALUTON_CNT ; ++i)
	{
		if (m_nEvalValues[i] == rValue.m_nEvalValues[i])
			continue;
			
		if (m_nEvalValues[i] > rValue.m_nEvalValues[i])
			return true;
			
		return false;
	}
	return false;
}

// public
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

// protected
void CPokerEvalution::InitCard()
{
	m_nHaveCnt = 0;
	InitEvalution();
}

// protected
int CPokerEvalution::Evalution()
{
	InitEvalution();

	if (CheckFlushEtc() == POKER_NO_PAIR && CheckStraight() == POKER_NO_PAIR)
		CheckPair();

	for (int i = 0 ; i < m_nHaveCnt ; ++i)
		m_nEvalValues[m_nValPos++] = m_nCard[i] % 13;

	for ( i = 0 ; i < m_nHaveCnt ; ++i)
		m_nEvalValues[m_nValPos++] = m_nCard[i];


#ifdef _DEBUG

	char szTemp[255];
	int nTemp[7];
	memset( nTemp, 0xff, sizeof(nTemp) );

	switch(m_szJokboCard[0]) {
		case 11:
			strcpy( szTemp, "ROYAL_STRAIGHT_FLUSH" );
			break;

		case 10:
			strcpy( szTemp, "STRAIGHT_FLUSH" );
			break;

		case 9:
			strcpy( szTemp, "FOUR_CARD" );
			break;

		case 8:
			strcpy( szTemp, "FULL_HOUSE" );
			break;

		case 7:
			strcpy( szTemp, "FLUSH" );
			break;

		case 6:
			strcpy( szTemp, "MOUNTAIN" );
			break;

		case 5:
			strcpy( szTemp, "BACK_STRAIGHT" );
			break;

		case 4:
			strcpy( szTemp, "STRAIGHT" );
			break;

		case 3:
			strcpy( szTemp, "THREE_CARD" );
			break;

		case 2:
			strcpy( szTemp, "TWO_PAIR" );
			break;

		case 1:
			strcpy( szTemp, "ONE_PAIR" );
			break;

		case 0:
			strcpy( szTemp, "NO_PAIR" );
			break;
	}

	nTemp[0] = m_szJokboCard[1] % 13 + 2;
	nTemp[1] = m_szJokboCard[2] % 13 + 2;
	nTemp[2] = m_szJokboCard[3] % 13 + 2;
	nTemp[3] = m_szJokboCard[4] % 13 + 2;
	nTemp[4] = m_szJokboCard[5] % 13 + 2;
	nTemp[5] = m_szJokboCard[6] % 13 + 2;
	nTemp[6] = m_szJokboCard[7] % 13 + 2;
	
#endif


	return m_nEvalValues[0];
}


// protected
void CPokerEvalution::InitEvalution()
{
	for (int i = 0 ; i < MAX_EVALUTON_CNT ; ++i)
		m_nEvalValues[i] = POKER_NO_PAIR;
	
	memset( m_szFixJokboCard, 0xff, sizeof(m_szFixJokboCard) );

	m_nValPos = 0;
	m_nFixJokboPos = 0;
	m_nJokboCardCnt = 0;
}

////////////////////////////////////
// POKER_ROYAL_STRAIGHT_FLUSH
// POKER_STRAIGHT_FLUSH
// POKER_FLUSH
// protected
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

	// Check flush	모양이 5개이면 플러쉬 이다.
	for (sharp = 0 ; sharp < 4 ; ++sharp)
		if (checkSharp[sharp] > 4)
			break;
	
	if (sharp < 4)		// It's flush
	{
		int straight;

		straight = CheckStraight (sharp);
		if ( straight != -1 )		// It's straight
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



			///fixjokbo///////////////////////////////////////////////////////////////////////
			int nCurStraight = straight;

			for (i = 0 ; i < m_nHaveCnt ; ++i) {

				if (m_nCard[i] / 13 != sharp) continue;

				if ( straight == 3 ) {
					
					nCurStraight = 12; 

					for ( i = 0; i < m_nHaveCnt; i++ ) {

						if ( m_nFixJokboPos < 5 && m_nCard[i] % 13 == nCurStraight ) {
							m_szFixJokboCard[m_nFixJokboPos++] = m_nCard[i];

							//A를 담고 나서 3,2,1,0 을 담으면 된다.
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
							nCurStraight--;		// 12, 11, 10, 9, 8순서대로 받으면 된다. 어차피 크기대로 정렬되있고 
												// 같은 12가 나오더라도 12는 제일 큰수 한번만 받기때문에..괜찮다.
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

////////////////////////////////////
// POKER_STRAIGHT
// protected
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
		m_nJackPot = 3;


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

void CPokerEvalution::SortArray(int *buff, int no)
{
	int tmp;
	int nChangeCardNo = 0;
	int nTempChangeCardNo = 0;

	for (int j = 0; j < no - 1 ; ++j)
	{
		tmp = j;
		for (int i = j + 1 ; i < no ; ++i)
		{
			nTempChangeCardNo = buff[tmp] % 13;
			nChangeCardNo = buff[i] % 13;

			if ( nTempChangeCardNo < nChangeCardNo ) {
				tmp = i;
			}
			else if ( nTempChangeCardNo == nChangeCardNo ) { //같은 족보일때 모양을 비교.
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
