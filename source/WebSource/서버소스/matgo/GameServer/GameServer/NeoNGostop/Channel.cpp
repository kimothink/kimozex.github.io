// Channel.cpp: implementation of the World class.
//
//////////////////////////////////////////////////////////////////////

#include "Channel.h"
#include "Define.h"
#include "NGFrame.h"
#include "CmdProcess.h"

#include "..\..\..\NetworkDefine\serverclientcommon.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannelUserList::CChannelUserList()
{
	m_User = NULL;
	m_NextList = NULL;

}

CChannelUserList::~CChannelUserList()
{
	
}


CViewWaitUserList::CViewWaitUserList()
{
	m_User = NULL;
	m_NextList = NULL;

}

CViewWaitUserList::~CViewWaitUserList()
{
	
}


CChannel::CChannel()
{
	m_pWaitChannelUserList = NULL;
	m_pViewWaitUserList = NULL;

	memset( m_szChannelName, 0x00, sizeof(m_szChannelName) );
	m_nCurChannelCnt = 0;
}

CChannel::~CChannel()
{

}

void CChannel::AddWaitChannelUser( CUserDescriptor *d )
{
	
	CChannelUserList *pNewUser;
	pNewUser = new CChannelUserList;

	pNewUser->m_User = d;
	pNewUser->m_NextList = m_pWaitChannelUserList;

	m_pWaitChannelUserList = pNewUser;

	m_nCurChannelCnt++;

}

void CChannel::DeleteWaitChannelUser( CUserDescriptor *d )
{
	CChannelUserList *pTemp;
	CChannelUserList *pTempNext;


	REMOVE_CHANNEL_LIST( d, m_pWaitChannelUserList, m_User, m_NextList );

	if ( m_nCurChannelCnt > 0 ) 
		m_nCurChannelCnt--;
}

void CChannel::AddViewWaitUserList( CUserDescriptor *d )
{
	CViewWaitUserList *pNewUser;
	pNewUser = new CViewWaitUserList;

	pNewUser->m_User = d;
	pNewUser->m_NextList = m_pViewWaitUserList;

	m_pViewWaitUserList = pNewUser;
}

void CChannel::DeleteViewWaitUserList( CUserDescriptor *d )
{
	CViewWaitUserList *pTemp;
	CViewWaitUserList *pTempNext;


	REMOVE_CHANNEL_LIST( d, m_pViewWaitUserList, m_User, m_NextList );
}


int CChannel::CreateRoom( CUserDescriptor *d, sCreateRoom *pCreateGame, ush_int nReqRoomNo )
{
	//빈방이 아니면 
	if ( m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.nRoomNo != -1 ) 
		return ERROR_NOCREATEROOM;
	else {
		m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.nRoomNo = nReqRoomNo;

		memcpy( m_RoomList[nReqRoomNo].l_sRoomList.szRoom_Pass , pCreateGame->szRoomPass, MAX_ROOM_PASSWORD );
		memcpy( m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.szRoomName , pCreateGame->szRoomName, MAX_ROOM_NAME );
		m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.nTotCnt = pCreateGame->nTotCnt;
		m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.cGameDivi = '0';
		m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.bSecret = pCreateGame->bSecret;

		m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney = pCreateGame->biRoomMoney;

		m_RoomList[nReqRoomNo].m_User[0] = d;

		d->m_nRoomNo = nReqRoomNo;
		d->m_nRoomInUserNo = 0;
		d->m_bBbangJjang = true;
		d->m_bRoomMaster = true;

		m_RoomList[nReqRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt++;

		m_RoomList[nReqRoomNo].m_nReadyCnt = 0;

		memset( m_RoomList[nReqRoomNo].szSunPack, 0x00, sizeof(m_RoomList[nReqRoomNo].szSunPack) );
		memset( m_RoomList[nReqRoomNo].szUserChoiceSunCard, 0xff, sizeof(m_RoomList[nReqRoomNo].szUserChoiceSunCard) );
		
		return nReqRoomNo;
	}


	return ERROR_NOCREATEROOM;

}

void CChannel::RoomOutUser( int nRoomNo, int nRoomInUserNo, CUserDescriptor *d )
{
	sRoomList *psRoomList;
	sRoomOutOtherSend l_sRoomOutOtherSend;
	memset( &l_sRoomOutOtherSend, 0x00, sizeof(l_sRoomOutOtherSend) );
	int nSendPacketLen = 0;

	m_RoomList[nRoomNo].m_nReadyCnt = 0;
	psRoomList = &m_RoomList[nRoomNo].l_sRoomList;

	m_RoomList[nRoomNo].m_User[nRoomInUserNo] = NULL;
	psRoomList->l_sRoomInfo.nCurCnt--;
	psRoomList->l_sRoomInfo.cGameDivi = '0';
		
	memset( m_RoomList[nRoomNo].szSunPack, 0x00, sizeof(m_RoomList[nRoomNo].szSunPack) );
	memset( m_RoomList[nRoomNo].szUserChoiceSunCard, 0xff, sizeof(m_RoomList[nRoomNo].szUserChoiceSunCard) );

	d->m_nRoomNo = -1;
	d->m_nRoomInUserNo = -1;

	if ( psRoomList->l_sRoomInfo.nCurCnt <= 0  ) {
		DeleteRoom(nRoomNo);
	}
	else {
		if ( d->m_bRoomMaster ) {
			for ( int i = 0; i < MAX_ROOM_IN; i ++ ) {
				if ( m_RoomList[nRoomNo].m_User[i] == NULL ) continue;
				
				m_RoomList[nRoomNo].m_User[i]->m_bRoomMaster = true;
				d->m_bRoomMaster = false;

				l_sRoomOutOtherSend.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;
				l_sRoomOutOtherSend.nNewHostNo = m_RoomList[nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;

				break;
			}

			l_sRoomOutOtherSend.nNewBbangJjnag = 0;
			if ( d->m_bBbangJjang ) {
				for ( int i = 0; i < MAX_ROOM_IN; i ++ ) {
					if ( m_RoomList[nRoomNo].m_User[i] == NULL ) continue;
					
					m_RoomList[nRoomNo].m_User[i]->m_bBbangJjang = true;
					d->m_bBbangJjang = false;

					l_sRoomOutOtherSend.nNewBbangJjnag = m_RoomList[nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;

					break;
				}
			}
		} 
		else {

				l_sRoomOutOtherSend.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;
				l_sRoomOutOtherSend.nNewHostNo = 0;
				l_sRoomOutOtherSend.nNewBbangJjnag = 0;

				if ( d->m_bBbangJjang ) {
					for ( int i = 0; i < MAX_ROOM_IN; i ++ ) {
						if ( m_RoomList[nRoomNo].m_User[i] == NULL ) continue;
						
						m_RoomList[nRoomNo].m_User[i]->m_bBbangJjang = true;
						d->m_bBbangJjang = false;

						l_sRoomOutOtherSend.nNewBbangJjnag = m_RoomList[nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;

						break;
					}
				}
		}


		for ( int h = 0; h < MAX_ROOM_IN; h++ ) {
			if ( m_RoomList[nRoomNo].m_User[h] == NULL ) continue;	
			
			m_RoomList[nRoomNo].m_User[h]->m_bRoomMaster = true;
			m_RoomList[nRoomNo].m_User[h]->m_bBbangJjang = true;

			l_sRoomOutOtherSend.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;
			l_sRoomOutOtherSend.nNewHostNo = m_RoomList[nRoomNo].m_User[h]->m_MyInfo.l_sUserInfo.nUserNo;
			l_sRoomOutOtherSend.nNewBbangJjnag = m_RoomList[nRoomNo].m_User[h]->m_MyInfo.l_sUserInfo.nUserNo;
		}



		l_sRoomOutOtherSend.nRoomCurCnt = psRoomList->l_sRoomInfo.nCurCnt;

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRoomOutOtherSend( g_ClientSendBuf, &l_sRoomOutOtherSend );
		
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, nRoomNo, nRoomInUserNo, nSendPacketLen );

	}

	d->m_bRoomMaster = false;
	d->m_bBbangJjang = false;
	return;
}


int CChannel::DeleteRoom( int nRoomNo )
{
	sRoomList *psRoomList;
	
	psRoomList = &m_RoomList[nRoomNo].l_sRoomList;

	psRoomList->l_sRoomInfo.nRoomNo = -1;
	memset( psRoomList->l_sRoomInfo.szRoomName, 0x00, MAX_ROOM_NAME );
	memset( psRoomList->szRoom_Pass, 0x00, MAX_ROOM_PASSWORD );
	psRoomList->l_sRoomInfo.nCurCnt = 0;
	psRoomList->l_sRoomInfo.nTotCnt = 0;
	psRoomList->l_sRoomInfo.bSecret = false;
	psRoomList->l_sRoomInfo.cGameDivi = 0x00;
	psRoomList->l_sRoomInfo.biRoomMoney = 0;

	m_RoomList[nRoomNo].m_nReadyCnt = 0;

	memset( m_RoomList[nRoomNo].m_User, 0x00, sizeof(m_RoomList[nRoomNo].m_User) );

	return false;
}
int CChannel::RoomView( CUserDescriptor *d, sRoomIn *psRoomIn )
{
//	g_AdminUser = d;
/*

	sRoomList *psRoomList;
	int nSendPacketLen = 0;

	psRoomList = &m_RoomList[psRoomIn->nRoomNo].l_sRoomList;
	m_RoomList[psRoomIn->nRoomNo].m_nReadyCnt = 0;

	if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nRoomNo == -1  ) 
		return ERROR_NOROOM;
//	else if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nTotCnt ) 
//		return ERROR_ROOMFULL;
	else if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.bSecret == true ) {
		if ( strcmp( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.szRoom_Pass, psRoomIn->szRoomPass ) )
			return ERROR_NOTPWD;
	}

	int nTemp = 0;
	int nBangJangRoomInUserNo = 0;
	int nBangJangUniQNo = 0;
	int i = 0;

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_RoomList[psRoomIn->nRoomNo].m_User[i] != NULL && 
			m_RoomList[psRoomIn->nRoomNo].m_User[i]->m_bBbangJjang == TRUE ) {
			
			nBangJangUniQNo = m_RoomList[psRoomIn->nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			nBangJangRoomInUserNo = i;
			
			break;
		}
			
	}

	for ( i = 1; i <  MAX_ROOM_IN ; i++ ) {

		nTemp = ( nBangJangRoomInUserNo + i ) % MAX_ROOM_IN;
			
		if ( m_RoomList[psRoomIn->nRoomNo].m_User[nTemp]->m_MyInfo.szUser_Id != NULL ) continue;
			
		m_RoomList[psRoomIn->nRoomNo].m_User[nTemp] = d;
		m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt++;

		d->m_nRoomNo = psRoomIn->nRoomNo;
		d->m_nRoomInUserNo = nTemp;
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRoomInOtherSend( g_ClientSendBuf, d, nBangJangUniQNo, i, m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt );
		
		//BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, psRoomIn->nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );


		return nTemp;		
		
	}	
//	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, psRoomIn->nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );
*/




	return 0;

}


int CChannel::RoomIn( CUserDescriptor *d, sRoomIn *psRoomIn )
{
	sRoomList *psRoomList;
	int nSendPacketLen = 0;

	psRoomList = &m_RoomList[psRoomIn->nRoomNo].l_sRoomList;
	m_RoomList[psRoomIn->nRoomNo].m_nReadyCnt = 0;

	if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nRoomNo == -1  ) 
		return ERROR_NOROOM;
	else if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nTotCnt ) 
		return ERROR_ROOMFULL;
	else if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.bSecret == true ) {
		if ( strcmp( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.szRoom_Pass, psRoomIn->szRoomPass ) )
			return ERROR_NOTPWD;
	}

	int nTemp = 0;
	int nBangJangRoomInUserNo = 0;
	int nBangJangUniQNo = 0;
	int i = 0;

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_RoomList[psRoomIn->nRoomNo].m_User[i] != NULL && 
			m_RoomList[psRoomIn->nRoomNo].m_User[i]->m_bBbangJjang == TRUE ) {
			
			nBangJangUniQNo = m_RoomList[psRoomIn->nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			nBangJangRoomInUserNo = i;
			
			break;
		}
			
	}

	for ( i = 1; i <  MAX_ROOM_IN ; i++ ) {

		nTemp = ( nBangJangRoomInUserNo + i ) % MAX_ROOM_IN;
			
		if ( m_RoomList[psRoomIn->nRoomNo].m_User[nTemp] != NULL ) continue;
			
		m_RoomList[psRoomIn->nRoomNo].m_User[nTemp] = d;
		m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt++;

		d->m_nRoomNo = psRoomIn->nRoomNo;
		d->m_nRoomInUserNo = nTemp;
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRoomInOtherSend( g_ClientSendBuf, d, nBangJangUniQNo, i, m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt );
		
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, psRoomIn->nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );


		return nTemp;		
		
	}	


	return 0;
}

int CChannel::QuickJoinRoomNo( BigInt biUserMoney )
{
	for ( int i = 0; i < MAX_ROOMCNT; i++ ) {
		if ( m_RoomList[i].l_sRoomList.l_sRoomInfo.bSecret == true ||
			 m_RoomList[i].l_sRoomList.l_sRoomInfo.nRoomNo == -1 ||
			 m_RoomList[i].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_ROOM_IN || 
			 !AvailableRoomIn( m_RoomList[i].l_sRoomList.l_sRoomInfo.biRoomMoney, biUserMoney )
			 ) continue;
			 
			return i;
	}
	return -1;
}

