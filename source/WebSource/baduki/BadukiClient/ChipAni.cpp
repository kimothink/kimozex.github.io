// ChipAni.cpp: implementation of the CChipAni class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChipAni.h"
#include "dtrace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CChipAni::CChipAni()
{
	m_nStackTop = -1;
    memset( m_chip, 0x00, sizeof( m_chip ) );
	//memset( m_chip, NULL, sizeof( m_chip ) );
}



CChipAni::~CChipAni()
{
}



void CChipAni::AddStack( sChipInfo *chip )
{
    /*
    // 기존에 있던 톨이면 새 위치 다시 갱신
	for ( int i = 0; i <= m_nStackTop; i++ ) {
		if(m_chip[i] == chip) {
			m_chip[i]->NewPos = chip->NewPos;
			return;
		}
	}
    */
	
    if ( m_nStackTop < MAX_CHIP_ANI - 1) {
        memcpy( &m_chip[++m_nStackTop], chip, sizeof( sChipInfo ));
    }
	//m_chip[++m_nStackTop] = chip;
}



void CChipAni::DeleteStack(int nStackNo)
{
	if(nStackNo < 0) return;
	//m_chip[nStackNo] = NULL;
    memset( &m_chip[nStackNo], 0x00, sizeof( sChipInfo ));

	if(nStackNo == m_nStackTop) {
		m_nStackTop--;
		return;
	}
	
	for(int i = nStackNo; i < m_nStackTop; i++) {
        memcpy( &m_chip[i], &m_chip[i+1], sizeof( sChipInfo ));
		//m_chip[i] = m_chip[i+1];
	}

    memset( &m_chip[m_nStackTop], 0x00, sizeof( sChipInfo ));
	//m_chip[m_nStackTop] = NULL;
	m_nStackTop--;

	if(m_nStackTop < -1) m_nStackTop = -1;
}



void CChipAni::EmptyStack()
{
	m_nStackTop = -1;
	memset( m_chip, 0x00, sizeof( m_chip ));
}



sChipInfo *CChipAni::GetChip( int nStackNo )
{
	if((nStackNo > m_nStackTop) || (nStackNo < 0)) return NULL;
	return &m_chip[nStackNo];
}



int CChipAni::GetAniCount()
{
	return m_nStackTop;
}


/*
bool CChipAni::CheckChip( sChipInfo *chip ) // stack에 chip이 존재 하는지 확인
{
	for ( int i = 0; i <= m_nStackTop; i++ ) {
		if( m_chip[i] == chip ) {
			return true;
		}
	}

	return false;
}
*/
