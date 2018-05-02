// TxtQ.cpp: implementation of the CTxtQ class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "TxtQ.h"
//#include "MainFrm.h"
#include "Globalvar.h"
//#include "Externvar.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define SAFE_DELETE(pointer)				if ( (pointer) != NULL ) { delete (pointer);	(pointer) = NULL; }
#define SAFE_DELETE_ARRAY(pointer)			if ( (pointer) != NULL ) { delete [] (pointer);	(pointer) = NULL; }

//extern CMainFrame *g_pCMainFrame;

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

	//strcpy(dest, m_pHead->m_Text);
	//*aliased = head->aliased;
	m_pHead = m_pHead->m_pNext;

	SAFE_DELETE_ARRAY(tmp->m_Text);
	SAFE_DELETE_ARRAY(tmp);

	return (1);
}

void CTxtQ::WriteToQ(const char *txt, int ntxtSize )
{
	CTxtBlock *newt;

	//newt = new CTxtBlock;
	newt = new CTxtBlock [1];
	newt->m_Text = new char[ntxtSize +1];

	memset( newt->m_Text, 0x00, ntxtSize + 1);
	newt->m_nBlockSize = ntxtSize;


	memcpy( newt->m_Text, txt, ntxtSize );
	//newt->m_Text = StrDup(txt);

	//newt->aliased = aliased;

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

//=========================================================================================
//=========================================================================================
//=========================================================================================
//=========================================================================================

CRecvCardQ::CRecvCardQ()
{
	m_pHead = NULL;
	m_pTail = NULL;
}

CRecvCardQ::~CRecvCardQ()
{
}

int CRecvCardQ::GetFromQ( int *nRecvCard, int *nDivi )
{
	CRecvCardQBlock *tmp;

	/* queue empty? */
	if (!m_pHead)
		return (0);

	tmp = m_pHead;

	memcpy( nRecvCard, &m_pHead->m_nRecvCard, sizeof(int) );
	memcpy( nDivi, &m_pHead->m_nDivi, sizeof(int) );

	m_pHead = m_pHead->m_pNext;

	SAFE_DELETE_ARRAY(tmp);

	return (1);
}

void CRecvCardQ::WriteToQ( const int nRecvCard, int nDivi )
{
	CRecvCardQBlock *newt;

	newt = new CRecvCardQBlock [1];

	newt->m_nRecvCard = nRecvCard;
	newt->m_nDivi = nDivi;
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


void CRecvCardQ::FlushQueues()
{
	int nRecvUserCard;
	int nDivi;
	int nCnt = 0;
	
	while (GetFromQ(&nRecvUserCard, &nDivi))
	{
		nCnt++;

		if ( nCnt >= 100 )
			g_pCMainFrame->m_Message->PutMessage( "≈•¿ÃªÛ", "Code - 160", true );
	}
}