#include "stdafx.h"
#include <time.h>
#include "CardPack.h"
#include "dtrace.h"
#include "GoStopEngine.h"


#define MAX_CARDCNT						51
// public
CCardPack::CCardPack(BYTE cardCnt)
{
	srand( (unsigned)time( NULL ) );
	m_nCardCnt = cardCnt;
	m_pCardPack = NULL;
	m_nReadNo  = 0;

	m_pCardPack = new BYTE[m_nCardCnt];
	if (m_pCardPack == NULL)
		throw -1;
	

	if ( cardCnt > 0 )
		Init();
}

// public
CCardPack::~CCardPack()
{
	if (m_pCardPack)
		delete [] m_pCardPack;
	
	m_pCardPack = NULL;
}

// protected
void CCardPack::Init()
{
     /*
     // 디버깅용

     //판
     m_pCardPack[0] = 41;
     m_pCardPack[1] = 21;
     m_pCardPack[2] = 36;
     m_pCardPack[3] = 8;

     //1슬록
     m_pCardPack[4] = 43;
     m_pCardPack[5] = 46;
     m_pCardPack[6] = 35;
     m_pCardPack[7] = 50;
     m_pCardPack[8] = 25;

     //나 
     m_pCardPack[9] = 1;
     m_pCardPack[10] = 3;
     m_pCardPack[11] = 10;
     m_pCardPack[12] = 7;
     m_pCardPack[13] = 14;
 
     //판
     m_pCardPack[14] = 13;
     m_pCardPack[15] = 17;
     m_pCardPack[16] = 6;
     m_pCardPack[17] = 16;

     //1슬록
     m_pCardPack[18] = 18;
     m_pCardPack[19] = 19;
     m_pCardPack[20] = 20;
     m_pCardPack[21] = 22;
     m_pCardPack[22] = 27;

     //나 
     m_pCardPack[23] = 48; 
     m_pCardPack[24] = 49;
     m_pCardPack[25] = 5;
     m_pCardPack[26] = 38;
     m_pCardPack[27] = 28;


     m_pCardPack[28] = 11;
     m_pCardPack[29] = 26;
     m_pCardPack[30] = 47;
     m_pCardPack[31] = 32;
     m_pCardPack[32] = 31;
     m_pCardPack[33] = 9;
     m_pCardPack[34] = 33;
     m_pCardPack[35] = 34;
     m_pCardPack[36] = 23;
     m_pCardPack[37] = 39;
     m_pCardPack[38] = 37;
     m_pCardPack[39] = 40;
     m_pCardPack[40] = 24;
     m_pCardPack[41] = 42;
     m_pCardPack[42] = 44;
     m_pCardPack[43] = 2;
     m_pCardPack[44] = 0;
     m_pCardPack[45] = 15;
     m_pCardPack[46] = 4;
     m_pCardPack[47] = 45;
     m_pCardPack[48] = 12;
     m_pCardPack[49] = 29;
     m_pCardPack[50] = 30;
     */

    
	m_pCardPack[0] = 0;
	m_pCardPack[1] = 10;
	m_pCardPack[2] = 2;
	m_pCardPack[3] = 8;
	m_pCardPack[4] = 3;
	m_pCardPack[5] = 7;
	m_pCardPack[6] = 4;
	m_pCardPack[7] = 1;
	m_pCardPack[8] = 5;
	m_pCardPack[9] = 6;
	m_pCardPack[10] = 9;

	m_pCardPack[11] = 12;
	m_pCardPack[12] = 11;
	m_pCardPack[13] = 14;	
	m_pCardPack[14] = 13;
	m_pCardPack[15] = 17;
	m_pCardPack[16] = 49;
	m_pCardPack[17] = 16;
	m_pCardPack[18] = 18;
	m_pCardPack[19] = 19;
	m_pCardPack[20] = 20;

	m_pCardPack[21] = 22;
	m_pCardPack[22] = 21;
	m_pCardPack[23] = 24;
	m_pCardPack[24] = 23;
	m_pCardPack[25] = 25;
	m_pCardPack[26] = 27;
	m_pCardPack[27] = 28;
	m_pCardPack[28] = 29;
	m_pCardPack[29] = 26;
	m_pCardPack[30] = 50;
	
	m_pCardPack[31] = 32;
	m_pCardPack[32] = 31;
	m_pCardPack[33] = 38;
	m_pCardPack[34] = 33;
	m_pCardPack[35] = 34;
	m_pCardPack[36] = 36;
	m_pCardPack[37] = 37;
	m_pCardPack[38] = 39;
	m_pCardPack[39] = 40;
	m_pCardPack[40] = 35;

	m_pCardPack[41] = 42;
	m_pCardPack[42] = 41;
	m_pCardPack[43] = 44;
	m_pCardPack[44] = 43;
	m_pCardPack[45] = 15;
	m_pCardPack[46] = 30;
	m_pCardPack[47] = 45;
	m_pCardPack[48] = 46;
	m_pCardPack[49] = 47;
	// Edited by shkim
	//m_pCardPack[50] = 30;
    
}


// public
void CCardPack::Shuffle()
{
	//return;
	BYTE tmp;

	for (BYTE i = 0 ; i < m_nCardCnt ; ++i)
	{
		BYTE j = GetRandom(0, (MAX_CARDCNT - 1));

		if ( j >= m_nCardCnt  ) continue;

		tmp = m_pCardPack[i];
		m_pCardPack[i] = m_pCardPack[j];
		m_pCardPack[j] = tmp;
	}
	
	
	m_nReadNo  = 0;
}

// public
int CCardPack::GetCard()
{
	return (m_nReadNo < m_nCardCnt) ?  m_pCardPack[m_nReadNo++] : -1;
}

// public
bool CCardPack::SetCardPack(BYTE *cardPack, int len)
{
	if (m_nCardCnt != len)
	{
		if (m_pCardPack)
			delete [] m_pCardPack;
		
		m_pCardPack = new BYTE[len];
		if (m_pCardPack == NULL)
			return false;

		m_nCardCnt = len;
	}
	
	memcpy(m_pCardPack, cardPack, len);
	m_nReadNo  = 0;

	return true;
}

BYTE *CCardPack::GetCardPack()	
{ 
	return m_pCardPack;	
}

void CCardPack::SetCardNo(int pos, int val)	
{	
	m_pCardPack[pos] = val;	
}

int	 CCardPack::RemainCardCnt()	
{	
	return m_nCardCnt - m_nReadNo;	
}
