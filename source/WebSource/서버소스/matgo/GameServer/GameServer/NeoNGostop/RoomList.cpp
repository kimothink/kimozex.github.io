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
	m_nReadyCnt = 0;
	memset( szSunPack, 0x00, sizeof(szSunPack) );
	memset( szUserChoiceSunCard, 0xff, sizeof(szUserChoiceSunCard) );
}

CRoomList::~CRoomList()
{

}
