// World.cpp: implementation of the World class.
//
//////////////////////////////////////////////////////////////////////

#include "World.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

World::World()
{

}

World::~World()
{

}



int World::FindRoomMasterUniNo( int nChannel, int nRoomNo )
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
	
		if ( m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] != NULL ) {

			if ( m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i]->m_bRoomMaster == TRUE ) 
				return m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
		}
	}

	return -1;
}

int World::FindRoomMasterSlotNo( int nChannel, int nRoomNo )
{

	CRoomList *l_CRoomList;
	int nRoomMasterRoomInUserNo = 0;

	l_CRoomList = &m_Channel[nChannel].m_RoomList[nRoomNo];

	//���� ã�´�.
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( l_CRoomList->m_User[i] != NULL && 
			l_CRoomList->m_User[i]->m_bRoomMaster == TRUE ) {
			
			nRoomMasterRoomInUserNo = i;
			
			return nRoomMasterRoomInUserNo;
		}
	}

	//���� ���� ������ ù��° ������ 
	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( l_CRoomList->m_User[i] != NULL )			
			return i;
	}

	return -1;
}



int World::FindNextUserSltNo( int nChannel, int nRoomNo, int nMySlotNo )
{

	CRoomList *l_CRoomList;
	int nNextSlotNo = 0;

	l_CRoomList = &m_Channel[nChannel].m_RoomList[nRoomNo];

	//���� ã�´�.
	for ( int i = 1; i < MAX_ROOM_IN; i++ ) {

		nNextSlotNo =  ( nMySlotNo + i ) % MAX_ROOM_IN;
		
		if ( l_CRoomList->m_User[nNextSlotNo] == NULL || l_CRoomList->m_User[nNextSlotNo]->m_nGameJoin == 0 ) continue;

		return nNextSlotNo;
	}

	//������ ������ �濡 ȥ�� �ִ°Ŵ�.
	return -1;
}

int World::FindNextUserUniqNo( int nChannel, int nRoomNo, int nMySlotNo )
{

	CRoomList *l_CRoomList;
	int nNextSlotNo = 0;

	l_CRoomList = &m_Channel[nChannel].m_RoomList[nRoomNo];

	//���� ã�´�.
	for ( int i = 1; i < MAX_ROOM_IN; i++ ) {

		nNextSlotNo =  ( nMySlotNo + i ) % MAX_ROOM_IN;
		
		if ( l_CRoomList->m_User[nNextSlotNo] == NULL || l_CRoomList->m_User[nNextSlotNo]->m_nGameJoin == 0 ) continue;

		return l_CRoomList->m_User[nNextSlotNo]->m_MyInfo.l_sUserInfo.nUserNo;
	}

	//������ ������ �濡 ȥ�� �ִ°Ŵ�.
	return -1;
}


int World::FindSlot( int nChannel, int nRoomNo, int nUserUniQNo )
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
	
		if ( m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] != NULL ) {

			if ( m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo == nUserUniQNo )
				return i;
		}
	}

	return -1;
}