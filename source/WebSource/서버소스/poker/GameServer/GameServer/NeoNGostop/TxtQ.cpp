// TxtQ.cpp: implementation of the CTxtQ class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TxtQ.h"
#include "comfunc.h"
#include "Define.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTxtQ::CTxtQ()
{
	m_pHead = NULL;
	m_pTail = NULL;
}

CTxtQ::~CTxtQ()
{
}

int CTxtQ::GetFromQ(char *dest, int *nQSize )
{
	CTxtBlock *tmp;

	/* queue empty? */
	if (!m_pHead)
		return (0);

	tmp = m_pHead;

	memset( dest, 0x00, m_pHead->m_nBlockSize +  1);

	memcpy( dest, m_pHead->m_Text, m_pHead->m_nBlockSize );

	memcpy( nQSize, &m_pHead->m_nBlockSize, sizeof(int) );

	m_pHead = m_pHead->m_pNext;

	SAFE_DELETE_ARRAY(tmp->m_Text);
	SAFE_DELETE_ARRAY(tmp);

	return (1);
}

void CTxtQ::WriteToQ(const char *txt, int ntxtSize )
{
	CTxtBlock *newt;

	newt = new CTxtBlock [1];
	newt->m_Text = new char[ntxtSize +1];

	memset( newt->m_Text, 0x00, ntxtSize + 1);
	newt->m_nBlockSize = ntxtSize;


	memcpy( newt->m_Text, txt, ntxtSize );

	/* queue empty? */
	if (!m_pHead) {
		newt->m_pNext = NULL;
		m_pHead = m_pTail = newt;
	} else {
		m_pTail->m_pNext = newt;
		m_pTail = newt;
		newt->m_pNext = NULL;
	}
}

