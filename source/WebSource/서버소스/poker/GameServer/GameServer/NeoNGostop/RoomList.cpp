// RoomList.cpp: implementation of the RoomList class.
//
//////////////////////////////////////////////////////////////////////

#include "RoomList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRoomList::CRoomList()
{
	
	memset( &l_sRoomList, 0x00, sizeof(l_sRoomList) );
	l_sRoomList.l_sRoomInfo.nRoomNo = -1;
	memset( m_User, 0x00, sizeof(m_User));
	RoomInit();

}

CRoomList::~CRoomList()
{

}

void CRoomList::RoomInit()
{
	m_nOutUserNo = 0;
	m_nGetOutCnt = 0;
	m_nPrizeDecide = 0;
	m_nPrizeNo = 0;
	m_nGetOutRepl = -1;	
						
	m_nRound = 0;
	m_biBeforeMoney = 0;
	m_biRoomCurMoney = 0;
}
