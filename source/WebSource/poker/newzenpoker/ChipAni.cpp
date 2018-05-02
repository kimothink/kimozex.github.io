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
}



CChipAni::~CChipAni()
{
}



void CChipAni::AddStack( sChipInfo *chip )
{
    if ( m_nStackTop < MAX_CHIP_ANI - 1) {
        memcpy( &m_chip[++m_nStackTop], chip, sizeof( sChipInfo ));
    }
}



void CChipAni::DeleteStack(int nStackNo)
{
	if(nStackNo < 0) return;

    memset( &m_chip[nStackNo], 0x00, sizeof( sChipInfo ));

	if(nStackNo == m_nStackTop) {
		m_nStackTop--;
		return;
	}
	
	for(int i = nStackNo; i < m_nStackTop; i++) {
        memcpy( &m_chip[i], &m_chip[i+1], sizeof( sChipInfo ));
	}

    memset( &m_chip[m_nStackTop], 0x00, sizeof( sChipInfo ));
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

