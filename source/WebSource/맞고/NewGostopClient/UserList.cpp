#include "StdAfx.h"
#include "UserList.h"
#include "ClientDefine.h"

#include "dtrace.h"
#include "GoStopEngine.h"

extern CGoStopEngine *g_pGoStop;
//////////////////////////////////////////////////////////////////////////
//
// 대기실 유저 리스트
//
//////////////////////////////////////////////////////////////////////////



CUserList::CUserList()
{
	m_nCurMax = 0;
}



CUserList::~CUserList()
{
}



int CUserList::AddUser( sUserInfo *pUserInfo )
{
	int i;
	
	for ( i = 0; i < m_nCurMax; i++ ) {	//이미 들어가 있는 사람이 있으면.. 그 번호에 새로 복사한다. 왜냐하면 그사람의 정보가 바뀌었을수 있기 때문에...서버에서 받은정보가 항상 최신이다.
		if ( m_pUserInfo[i].m_sUser.nUserNo == pUserInfo->nUserNo ) {
			m_pUserInfo[i].m_bUser = true;
			memcpy( &m_pUserInfo[i].m_sUser, pUserInfo, sizeof(sUserInfo) );
			return i;
		}
	}
	
	for ( i = 0; i < MAX_CHANNELPER; i++ ) {
		if ( m_pUserInfo[i].m_bUser == false ) {
			m_pUserInfo[i].m_bUser = true;
			memcpy( &m_pUserInfo[i].m_sUser, pUserInfo, sizeof(sUserInfo) );

            if ( i == m_nCurMax ) {
                m_nCurMax++;
            }
			
			return i;
		}
	}

	return -1;
}



void CUserList::DeleteUser( int nUserNo )
{
	m_pUserInfo[nUserNo].Init();

    if ( nUserNo == m_nCurMax ) {
        m_nCurMax--;
    }
}



int CUserList::FindUser( int nUniUserNo )
{
	for ( int i = 0; i < m_nCurMax; i++) {
        if ( m_pUserInfo[i].m_sUser.nUserNo == nUniUserNo ) {
            return i;
        }
	}

	return -1;

}



//////////////////////////////////////////////////////////////////////////
// 대기실 방 리스트
//////////////////////////////////////////////////////////////////////////
CGameWaitRoomList::CGameWaitRoomList()
{
	memset( &m_sRoomStateInfo, 0x00, sizeof(m_sRoomStateInfo) );
}



CGameWaitRoomList::~CGameWaitRoomList()
{
}



//////////////////////////////////////////////////////////////////////////
// 방에서의 유저들
//////////////////////////////////////////////////////////////////////////

CGameUserInfo::CGameUserInfo()
{
}

CGameUserInfo::~CGameUserInfo()
{
}

CGameUserInfo::Init()
{
	m_bUser = FALSE;
	m_bBBangJjang = FALSE;
	m_bSun = FALSE;
	m_biObtainMoney = 0;
	m_biLoseMoney = 0;

	memset( &m_sUserInfo, 0x00, sizeof(m_sUserInfo) );
	memset( &l_sGameEndHistory, 0x00, sizeof(l_sGameEndHistory) );
}



//////////////////////////////////////////////////////////////////////////
CGameUserList::CGameUserList()
{
	memset( &m_GameUserInfo, 0x00, sizeof(m_GameUserInfo) );
}



CGameUserList::~CGameUserList()
{
}

void CGameUserList::UserAllClear()
{
	//유저 정보들 초기화.
	// bUser 때문에 이렇게 한다.
	m_GameUserInfo[0].m_bBBangJjang = FALSE;
	m_GameUserInfo[0].m_bSun = FALSE;
	m_GameUserInfo[0].m_biObtainMoney = 0;
	m_GameUserInfo[0].m_biLoseMoney = 0;

    memset( &m_GameUserInfo[0].l_sGameEndHistory, 0x00, sizeof(m_GameUserInfo[0].l_sGameEndHistory) );

	for ( int i = 1; i < MAX_ROOM_IN; i++ ) {
		m_GameUserInfo[i].Init();
	}
}

int CGameUserList::AddGameUser( sUserInfo *pUserInfo, int nVoice, int nSlotNo )
{
	int i;


	//DTRACE2("CGameUserList::AddGameUser - UserNick = %s, nSlotNo = %d", pUserInfo->szNick_Name, nSlotNo );

	if ( nSlotNo < 0 ) {

		for ( i = 0; i < MAX_ROOM_IN; i++ ) {	
			if ( m_GameUserInfo[i].m_bUser == false ) {
				memcpy( &m_GameUserInfo[i].m_sUserInfo, pUserInfo, sizeof(sUserInfo) );
				m_GameUserInfo[i].m_nVoice = nVoice;
				m_GameUserInfo[i].m_bUser = true;
				return i;
			}
		}
	}
	else {
		memcpy( &m_GameUserInfo[nSlotNo].m_sUserInfo, pUserInfo, sizeof(sUserInfo)  );
		m_GameUserInfo[nSlotNo].m_bUser = true;
        m_GameUserInfo[nSlotNo].m_nVoice = nVoice;
		return nSlotNo;
	}


	return -1;
}

int CGameUserList::AddRoomInOtherUser( sUserInfo *pUserInfo, int nVoice, int nNextNo )
{
	int nBangJangArrayNo = 0;
	int nSetArrayNo = 0;
	
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_GameUserInfo[i].m_bBBangJjang == TRUE ) {
			nBangJangArrayNo = i;
			break;
		}
	}

	//방장 다음을 찾는다.
	nSetArrayNo = ( nBangJangArrayNo + nNextNo ) % MAX_ROOM_IN;
	memcpy( &m_GameUserInfo[nSetArrayNo].m_sUserInfo, pUserInfo, sizeof(sUserInfo)  );
	m_GameUserInfo[nSetArrayNo].m_nVoice = nVoice;
	m_GameUserInfo[nSetArrayNo].m_bUser = true;

	return nSetArrayNo;


}

void CGameUserList::DeleteUser( int nUniqueUserNo )
{
	int i;

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_GameUserInfo[i].m_sUserInfo.nUserNo == nUniqueUserNo ) {
			m_GameUserInfo[i].Init();
		}

	}
}

int CGameUserList::FindUser( int nUniqueUserNo )
{
	for ( int i = 0; i < MAX_ROOM_IN; i++) {
		if ( m_GameUserInfo[i].m_sUserInfo.nUserNo == nUniqueUserNo )
			return i;
	}
	return -1;
}

void CGameUserList::BangJangDel()
{
	//DTRACE("방장이 지워졌다");
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_GameUserInfo[i].m_bBBangJjang = FALSE;
	}
}

void CGameUserList::SunDel()
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_GameUserInfo[i].m_bSun = FALSE;
	}
}


void CGameUserList::NewBangJangSet( int nUniqueUserNo )
{

	BangJangDel();

	int nUsetArrayNo = FindUser(nUniqueUserNo);
	m_GameUserInfo[nUsetArrayNo].m_bBBangJjang = TRUE;

	g_pGoStop->m_pFloor.m_nBbangJjangSlotNo = nUsetArrayNo;

}

void CGameUserList::NewSunSet( int nUniqueUserNo )
{

	SunDel();

	int nUsetArrayNo = FindUser(nUniqueUserNo);
	m_GameUserInfo[nUsetArrayNo].m_bSun = TRUE;

	g_pGoStop->m_pFloor.m_nMasterSlotNo = nUsetArrayNo;
}

void CGameUserList::UserVoiceSet( int nUserUniqNo, int nUserVoice )
{
	int nUsetArrayNo = FindUser(nUserUniqNo);

	m_GameUserInfo[nUsetArrayNo].m_nVoice = nUserVoice;
}
