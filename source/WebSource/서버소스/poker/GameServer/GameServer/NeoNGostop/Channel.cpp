// Channel.cpp: implementation of the World class.
//
//////////////////////////////////////////////////////////////////////

#include "Channel.h"
#include "Define.h"
#include "NGFrame.h"
#include "CmdProcess.h"
#include "Manager.h"
#include "..\..\..\PokerCommonDefine\serverclientcommon.h"
#include "dtrace.h"


extern World g_World;
extern CManager *l_pCManager;

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


int CChannel::CreateRoom( CUserDescriptor *d, sCreateRoom *pCreateGame )
{
	for ( int i = 0; i < MAX_ROOMCNT; i++ ) {
		DTRACE( "CChannel::CreateRoom RoomNo = %d , i = %d, nCurCnt = %d", 
			m_RoomList[i].l_sRoomList.l_sRoomInfo.nRoomNo, i, m_RoomList[i].l_sRoomList.l_sRoomInfo.nCurCnt );
		if ( m_RoomList[i].l_sRoomList.l_sRoomInfo.nRoomNo == -1 ) {
			
			m_RoomList[i].l_sRoomList.l_sRoomInfo.nRoomNo = i;

			memcpy( m_RoomList[i].l_sRoomList.szRoom_Pass , pCreateGame->szRoomPass, MAX_ROOM_PASSWORD );
			memcpy( m_RoomList[i].l_sRoomList.l_sRoomInfo.szRoomName , pCreateGame->szRoomName, MAX_ROOM_NAME );
			m_RoomList[i].l_sRoomList.l_sRoomInfo.nTotCnt = pCreateGame->nTotCnt;
			m_RoomList[i].l_sRoomList.l_sRoomInfo.nGameDivi = 0;

			m_RoomList[i].l_sRoomList.l_sRoomInfo.bChoiceRoom = pCreateGame->bChoiceRoom;
			m_RoomList[i].l_sRoomList.l_sRoomInfo.biRoomMoney = pCreateGame->biRoomMoney;

			m_RoomList[i].l_sRoomList.l_sRoomInfo.bSecret = pCreateGame->bSecret;

			m_RoomList[i].m_User[0] = d;

			d->m_nRoomNo = i;
			d->m_nRoomInUserNo = 0;
			d->m_bBbangJjang = true;
			d->m_bRoomMaster = true;
			d->m_nGameJoin = 0; //게임대기자.

			m_RoomList[i].l_sRoomList.l_sRoomInfo.nCurCnt++;
			
			return i;
		}
	}

	return ERROR_NOCREATEROOM;

}

void CChannel::RoomOutUser( int nRoomNo, int nRoomInUserNo, CUserDescriptor *d )
{
	sRoomList *psRoomList;
	sRoomOutOtherSend l_sRoomOutOtherSend;
	memset( &l_sRoomOutOtherSend, 0x00, sizeof(l_sRoomOutOtherSend) );
	int nSendPacketLen = 0;
	int nNewBangJang = 0;
	int nNewSun = 0;
	int nTempRoomInUserNo = d->m_nRoomInUserNo;

	psRoomList = &m_RoomList[nRoomNo].l_sRoomList;

	int nChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	m_RoomList[nRoomNo].m_CPokerEngine.UserInit( nChannel, nRoomNo, nRoomInUserNo, TRUE );


	m_RoomList[nRoomNo].m_User[nRoomInUserNo] = NULL;
	psRoomList->l_sRoomInfo.nCurCnt--;

	if ( psRoomList->l_sRoomInfo.nCurCnt <= 0  ) {
		if ( m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 2 )	
			d->DBUpdate( 0, 0 ); 

		DeleteRoom( nChannel, nRoomNo );
	}
	else {

		d->m_nGameJoin = -1;
		l_sRoomOutOtherSend.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;
		if ( d->m_bBbangJjang ) {
			for ( int i = 1; i < MAX_ROOM_IN; i ++ ) {

				nNewBangJang = ( nTempRoomInUserNo + i ) % MAX_ROOM_IN;

				if ( m_RoomList[nRoomNo].m_User[nNewBangJang] == NULL ) continue;
				
				m_RoomList[nRoomNo].m_User[nNewBangJang]->m_bBbangJjang = true;
				d->m_bBbangJjang = false;

				l_sRoomOutOtherSend.nNewBbangJjnag = m_RoomList[nRoomNo].m_User[nNewBangJang]->m_MyInfo.l_sUserInfo.nUserNo;

				break;
			}
		}


		if ( d->m_bRoomMaster ) {

			if ( m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 0  ||
				 m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 1 )
			{
				for ( int i = 1; i < MAX_ROOM_IN; i ++ ) { 

					//방장의 다음부터 찾는다.
					nNewSun = ( nTempRoomInUserNo + i ) % MAX_ROOM_IN;

					if ( m_RoomList[nRoomNo].m_User[nNewSun] == NULL ) continue;

					m_RoomList[nRoomNo].m_User[nNewSun]->m_bRoomMaster = TRUE;
					d->m_bRoomMaster = false;

					l_sRoomOutOtherSend.nNewRoomMaster = m_RoomList[nRoomNo].m_User[nNewSun]->m_MyInfo.l_sUserInfo.nUserNo;

					break;
				}


				if ( m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 1 && 
					 m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_GAMESTART_MEM ) {

					memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
					nSendPacketLen = NGFrameStartBtnOnOff( g_ClientSendBuf, m_RoomList[nRoomNo].m_User[nNewSun], '1' );
					SEND_TO_Q(g_ClientSendBuf, m_RoomList[nRoomNo].m_User[nNewSun], nSendPacketLen);
					l_pCManager->SendOutput (m_RoomList[nRoomNo].m_User[nNewSun]);


					m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi = 1;
				}

			}
			else {
				for ( int i = 1; i < MAX_ROOM_IN; i ++ ) { 

					nNewSun = ( nTempRoomInUserNo + i ) % MAX_ROOM_IN;

					if ( m_RoomList[nRoomNo].m_User[nNewSun] == NULL ||
						 m_RoomList[nRoomNo].m_User[nNewSun]->m_nGameJoin == 0 ) continue;

					m_RoomList[nRoomNo].m_User[nNewSun]->m_bRoomMaster = TRUE;
					d->m_bRoomMaster = false;

					l_sRoomOutOtherSend.nNewRoomMaster = m_RoomList[nRoomNo].m_User[nNewSun]->m_MyInfo.l_sUserInfo.nUserNo;

					break;
				}			
			}
		}

		if ( m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 1 &&
			 m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt <= 1 ) 
		{
			int nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( d->m_MyInfo.l_sUserInfo.nChannel, nRoomNo );

			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameStartBtnOnOff( g_ClientSendBuf, m_RoomList[nRoomNo].m_User[nRoomMasterRoomInUserNo], '0' );
			SEND_TO_Q(g_ClientSendBuf, m_RoomList[nRoomNo].m_User[nRoomMasterRoomInUserNo], nSendPacketLen);
			l_pCManager->SendOutput (m_RoomList[nRoomNo].m_User[nRoomMasterRoomInUserNo]);			

			m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi = 0;
		}
		else if ( m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 2 )
		{

			if ( d->m_bMyOrder ) {
				char szTemp[10];

				memset( szTemp, 0x00, sizeof(szTemp) );
				szTemp[0] = '7';
				UserSelect( d, szTemp );
			}
			else {

				int nRe = m_RoomList[nRoomNo].m_CPokerEngine.GameVerify( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

				if ( nRe == -2 ) {	//한명만 남은경우. 기권승 처리.
					EndGameProc( d, '8', 1 );	//한명남은 사람.		
				}

				d->DBUpdate( 0, 0 ); // 패배, 정상

			}
		}
		else if ( m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 3 )
		{
			int nRe = m_RoomList[nRoomNo].m_CPokerEngine.GameVerify( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

			if ( nRe == -1 ) {	
					
				for ( int i = 0; i < MAX_ROOM_IN; i ++ ) { 

					if ( m_RoomList[nRoomNo].m_User[i] == NULL  ) continue;

					m_RoomList[nRoomNo].m_User[i]->m_bRoomMaster = TRUE;
					d->m_bRoomMaster = false;

					m_RoomList[nRoomNo].m_User[i]->m_bBbangJjang = TRUE;
					d->m_bBbangJjang = false;

					break;
				}			

				if (  m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_GAMESTART_MEM ) {

					int nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );
					
					memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
					nSendPacketLen = NGFrameStartBtnOnOff( g_ClientSendBuf, m_RoomList[nRoomNo].m_User[nRoomMasterRoomInUserNo], '1' );
					SEND_TO_Q(g_ClientSendBuf, m_RoomList[nRoomNo].m_User[nRoomMasterRoomInUserNo], nSendPacketLen);
					l_pCManager->SendOutput (m_RoomList[nRoomNo].m_User[nRoomMasterRoomInUserNo]);

					m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi = 1;
				}
				else {	
					m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi = 0;
				}
			}
		}

		l_sRoomOutOtherSend.nRoomCurCnt = m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt;

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRoomOutOtherSend( g_ClientSendBuf, &l_sRoomOutOtherSend );
		
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, nRoomNo, nRoomInUserNo, nSendPacketLen );
	}
	
	d->m_nRoomNo = -1;
	d->m_nRoomInUserNo = -1;
	d->m_bRoomMaster = false;
	d->m_bBbangJjang = false;

	return;
}


int CChannel::DeleteRoom( int nChannel, int nRoomNo )
{
	sRoomList *psRoomList;
	
	psRoomList = &m_RoomList[nRoomNo].l_sRoomList;

	m_RoomList[nRoomNo].RoomInit();
	m_RoomList[nRoomNo].m_CPokerEngine.UserInit( nChannel, nRoomNo );

	psRoomList->l_sRoomInfo.nRoomNo = -1;
	memset( psRoomList->l_sRoomInfo.szRoomName, 0x00, MAX_ROOM_NAME );
	memset( psRoomList->szRoom_Pass, 0x00, MAX_ROOM_PASSWORD );
	psRoomList->l_sRoomInfo.nCurCnt = 0;
	psRoomList->l_sRoomInfo.nTotCnt = 0;
	psRoomList->l_sRoomInfo.bSecret = false;
	psRoomList->l_sRoomInfo.nGameDivi = 0x00;
	psRoomList->l_sRoomInfo.biRoomMoney = 0;

	memset( m_RoomList[nRoomNo].m_User, 0x00, sizeof(m_RoomList[nRoomNo].m_User) );

	return false;
}

int CChannel::RoomIn( CUserDescriptor *d, sRoomIn *psRoomIn )
{
	sRoomList *psRoomList;
	int nSendPacketLen = 0;

	psRoomList = &m_RoomList[psRoomIn->nRoomNo].l_sRoomList;

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
	int i = 0;

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_RoomList[psRoomIn->nRoomNo].m_User[i] != NULL && 
			m_RoomList[psRoomIn->nRoomNo].m_User[i]->m_bBbangJjang == TRUE ) {
			
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
		d->m_nGameJoin = 0;	//대기자.
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRoomInOtherSend( g_ClientSendBuf, d, i );
		
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, psRoomIn->nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );


		if ( m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi == 0 && 
			 m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_GAMESTART_MEM ) {

			int nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( d->m_MyInfo.l_sUserInfo.nChannel, psRoomIn->nRoomNo );
			
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameStartBtnOnOff( g_ClientSendBuf, m_RoomList[psRoomIn->nRoomNo].m_User[nRoomMasterRoomInUserNo], '1' );
			SEND_TO_Q(g_ClientSendBuf, m_RoomList[psRoomIn->nRoomNo].m_User[nRoomMasterRoomInUserNo], nSendPacketLen);
			l_pCManager->SendOutput (m_RoomList[psRoomIn->nRoomNo].m_User[nRoomMasterRoomInUserNo]);


			m_RoomList[psRoomIn->nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi = 1;
		}


		return nTemp;		
		
	}

	return 0;
}

int CChannel::QuickJoinRoomNo( BigInt biUserMoney )
{
	for ( int i = 0; i < MAX_ROOMCNT; i++ ) {
		//비밀방은 조인할수가 없다. 게임중인방.
		if ( m_RoomList[i].l_sRoomList.l_sRoomInfo.bSecret == true ||
			 m_RoomList[i].l_sRoomList.l_sRoomInfo.nRoomNo == -1 ||
			 m_RoomList[i].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_ROOM_IN || 
			 !AvailableRoomIn( m_RoomList[i].l_sRoomList.l_sRoomInfo.biRoomMoney, biUserMoney )
			 ) continue;
			 
			return i;
	}
	return -1;
}

