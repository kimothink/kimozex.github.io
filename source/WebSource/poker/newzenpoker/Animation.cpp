// Animation.cpp: implementation of the CAnimation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Animation.h"
#include "dtrace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnimation::CAnimation()
{
	m_nStackTop = -1;
		
	memset( m_card, NULL, sizeof( m_card ) );
}

CAnimation::~CAnimation()
{
}

void CAnimation::AddStack(sUserCardInfo *card)
{
	for(int i = 0; i <= m_nStackTop; i++)
	{
		if(m_card[i] == card)
		{
			m_card[i]->NewPos = card->NewPos;

			return;
		}
	}
	
	m_card[++m_nStackTop] = card;
	DTRACE("m_nStackTop = %d", m_nStackTop);
}

void CAnimation::DeleteStack(int nStackNo)
{
	if(nStackNo < 0) return;
	
	m_card[nStackNo] = NULL;

	if(nStackNo == m_nStackTop)
	{
		m_nStackTop--;
		
		return;
	}
	
	for(int i = nStackNo; i < m_nStackTop; i++)
	{
		m_card[i] = m_card[i+1];
	}

	m_card[m_nStackTop] = NULL;
	
	m_nStackTop--;

	if(m_nStackTop < -1) m_nStackTop = -1;
}

void CAnimation::EmptyStack()
{
	m_nStackTop = -1;
	memset(m_card, 0, sizeof(m_card));
}

sUserCardInfo* CAnimation::GetCard(int nStackNo)
{
	if((nStackNo > m_nStackTop) || (nStackNo < 0)) return NULL;

	return m_card[nStackNo];
}

int CAnimation::GetAniCount()
{
	return m_nStackTop;
}

bool CAnimation::CheckCard(sUserCardInfo *card) // bsw 01.06 stack에 card가 존재 하는지 확인
{
	for(int i = 0; i <= m_nStackTop; i++)
	{
		if(m_card[i] == card)
		{
			return true;
		}
	}

	return false;
}