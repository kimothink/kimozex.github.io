#include "StdAfx.h"
#include "UserList.h"
#include "ClientDefine.h"

#include "dtrace.h"
#include "GoStopEngine.h"

extern CGoStopEngine *g_pGoStop;
//////////////////////////////////////////////////////////////////////////
//
// ���� ���� ����Ʈ
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
	
	for ( i = 0; i < m_nCurMax; i++ ) {	//�̹� �� �ִ� ����� ������.. �� ��ȣ�� ���� �����Ѵ�. �ֳ��ϸ� �׻���� ������ �ٲ������ �ֱ� ������...�������� ���������� �׻� �ֽ��̴�.
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
// ���� �� ����Ʈ
//////////////////////////////////////////////////////////////////////////
CGameWaitRoomList::CGameWaitRoomList()
{
	memset( &m_sRoomStateInfo, 0x00, sizeof(m_sRoomStateInfo) );
}



CGameWaitRoomList::~CGameWaitRoomList()
{
}



//////////////////////////////////////////////////////////////////////////
// �濡���� ������
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
	//���� ������ �ʱ�ȭ.
	// bUser ������ �̷��� �Ѵ�.
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

	//���� ������ ã�´�.
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
	//DTRACE("������ ��������");
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
