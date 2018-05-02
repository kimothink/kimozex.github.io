#include "NGFrame.h"
#include "World.h"
#include "Channel.h"
#include "dtrace.h"

extern World g_World;
extern sJackPotInfo g_sJackPotInfo[JACKPOT_CNT];

int NGFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode )
{
	int nIndex = 0;

	sSCHeaderPkt l_sSCHeaderPkt;

	l_sSCHeaderPkt.szHeader[0] = 'N';
	l_sSCHeaderPkt.szHeader[1] = 'G';
	l_sSCHeaderPkt.nTPacketSize = nPayLoadLength;
	l_sSCHeaderPkt.cCmd = bCmd;
	l_sSCHeaderPkt.cErrorCode = bErrorCode;

	memcpy( pBuffer, &l_sSCHeaderPkt, sizeof(l_sSCHeaderPkt) );
	nIndex += sizeof(l_sSCHeaderPkt);


	return nIndex;	
}

int NGCSFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode )
{
	int nIndex = 0;

	sCSHeaderPkt l_sCSHeaderPkt;

	l_sCSHeaderPkt.szHeader[0] = '0';
	l_sCSHeaderPkt.szHeader[1] = '0';
	l_sCSHeaderPkt.nTPacketSize = nPayLoadLength;
	l_sCSHeaderPkt.cMode = '0';
	l_sCSHeaderPkt.cCmd = bCmd;
	l_sCSHeaderPkt.cErrorCode = bErrorCode;


	memcpy( pBuffer, &l_sCSHeaderPkt, sizeof(l_sCSHeaderPkt) );
	nIndex += sizeof(l_sCSHeaderPkt);


	return nIndex;
}


int NGFrameFirst( char *pBuffer, CUserDescriptor *d )
{
	sPKFirst l_sPKFirst;
	int nIndex = 0;
	
	memset( &l_sPKFirst, 0, sizeof(l_sPKFirst));

	nIndex += SC_HEADER_SIZE;

	memcpy( &l_sPKFirst, d->m_szPureHeader, sizeof(d->m_szPureHeader) );
	l_sPKFirst.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;

	memcpy( &pBuffer[nIndex], &l_sPKFirst, sizeof(l_sPKFirst) );
	nIndex += sizeof(l_sPKFirst);

	NGFrameHeader( pBuffer, nIndex, NGSC_HEADERSEND, ERROR_NOT );

	return nIndex;
}

int NGFrameGetUserInfo( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = 0;

	nIndex += SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], &d->m_MyInfo, sizeof(d->m_MyInfo) );
	nIndex += sizeof(d->m_MyInfo);

	NGFrameHeader( pBuffer, nIndex, NGSC_GETUSERINFO, ERROR_NOT );

	return nIndex;
}


int NGFrameFail( char *pBuffer, CUserDescriptor *d, char cCmd, char cErrorCode  )
{
	int nIndex = 0;
	int nPayLoadLength = SC_HEADER_SIZE;

	nIndex = NGFrameHeader( pBuffer, nPayLoadLength, cCmd, cErrorCode );

	return nIndex;
}

int NGFrameWaitInput( char *pBuffer, CUserDescriptor *d )
{

	int nIndex = SC_HEADER_SIZE;
	int nTemp = 0;
	int nRoomTemp = 0;
	int nRoomCnt = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	int nUserCnt = 0;
	CChannelUserList *pUserList, *pNextUserList, *pTemp;
	CRoomList *pTempRoom;

	pBuffer[nIndex++] = HIBYTE(HIWORD(d->m_MyInfo.l_sUserInfo.nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(d->m_MyInfo.l_sUserInfo.nUserNo));

	nRoomTemp = nIndex;
	nIndex += sizeof(ush_int); // 방의 개수를 넣어줄 자리를 비워둔다. ( 한페이지가 무조건 8개일수는 없다. )

	for ( int i = 0; i < MAX_ROOMCNT; i++ ) {	//방정보를 가져온다.
		
		pTempRoom = &g_World.m_Channel[nUserChannel].m_RoomList[i];

		if ( pTempRoom->l_sRoomList.l_sRoomInfo.nRoomNo == -1 ) continue;  //빈방이면 할필요가 없겠지??

		nRoomCnt++;

		memcpy( &pBuffer[nIndex], &pTempRoom->l_sRoomList.l_sRoomInfo, sizeof(sRoomInfo) );
		nIndex += sizeof(sRoomInfo);

	}


	pBuffer[nRoomTemp++] = HIBYTE(LOWORD(nRoomCnt));
	pBuffer[nRoomTemp++] = LOBYTE(LOWORD(nRoomCnt));

	nTemp =  nIndex; 
	nIndex += sizeof(ush_int); 
	
	pTemp = g_World.m_Channel[nUserChannel].m_pWaitChannelUserList;  	

	for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {
		nUserCnt++;
		memcpy( &pBuffer[nIndex], &pUserList->m_User->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );
		nIndex += sizeof(sUserInfo);
	}

	pBuffer[nTemp++] = HIBYTE(LOWORD(nUserCnt));
	pBuffer[nTemp++] = LOBYTE(LOWORD(nUserCnt));	


	NGFrameHeader( pBuffer, nIndex, NGSC_WAITINPUT, ERROR_NOT );



	return nIndex;
}

int NGFrameGetRoomInfo( char *pBuffer, CUserDescriptor *d, int nRoomNo )
{
	int nIndex = SC_HEADER_SIZE;
	
	CRoomList *pTempRoom;
	pTempRoom = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo];

	int nUserCnt = pTempRoom->l_sRoomList.l_sRoomInfo.nCurCnt;
	int nTemp = 0;
	sUserInfo l_sUserInfo[MAX_ROOM_IN];

	memset( l_sUserInfo, 0x00, sizeof(l_sUserInfo) );

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( pTempRoom->m_User[i] == NULL ) continue;

		memcpy( &l_sUserInfo[nTemp], &pTempRoom->m_User[i]->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );			
		nTemp++;

	}
	memcpy( &pBuffer[nIndex], l_sUserInfo, sizeof(l_sUserInfo) );
	nIndex += sizeof(l_sUserInfo);

	NGFrameHeader( pBuffer, nIndex, NGSC_ROOMINFO, ERROR_NOT );
	return nIndex;
	
}

int NGFrameUserAdd( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = 'A';			//유저추가

	memcpy( &pBuffer[nIndex], &d->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );
	nIndex += sizeof(sUserInfo);

	NGFrameHeader( pBuffer, nIndex, NGSC_USERINFOCHANGE, ERROR_NOT );
	
	return nIndex;
}

int NGFrameUserDel( char *pBuffer, int nDelUserNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = 'D';			//유저추가

	pBuffer[nIndex++] = HIBYTE(HIWORD(nDelUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nDelUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nDelUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nDelUserNo));

	NGFrameHeader( pBuffer, nIndex, NGSC_USERINFOCHANGE, ERROR_NOT );
	
	return nIndex;
}


int NGFrameCreateGame( char *pBuffer, int nRoomNo, int nUserChannel )
{
	int nIndex = SC_HEADER_SIZE;

	CRoomList *pTempRoom = &g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo];

	memcpy( &pBuffer[nIndex], &pTempRoom->l_sRoomList.l_sRoomInfo, sizeof(pTempRoom->l_sRoomList.l_sRoomInfo) );

	nIndex += sizeof(pTempRoom->l_sRoomList.l_sRoomInfo);
	
	NGFrameHeader( pBuffer, nIndex, NGSC_CREATEROOM, ERROR_NOT );
	
	return nIndex;
}

int NGFrameWaitInfoChange( char *pBuffer, CUserDescriptor *d, int nRoomNo, int nRoomInUserNo, int nUserNo, char cCmdDivi )   //cCmdDivi   ( 'C' : RoomCreate, 'M' : RoomModify, 'D' : RoomDel )
{
	int nIndex = SC_HEADER_SIZE;
	CRoomList *pTempRoom = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo];
	int i = 0;

	pBuffer[nIndex++] = cCmdDivi;

	switch(cCmdDivi) {
		case 'C':
			sOnCreateRoom l_sOnCreateRoom;

			memset( &l_sOnCreateRoom, 0x00, sizeof(l_sOnCreateRoom) );
			memcpy( &l_sOnCreateRoom.l_sRoomInfo , &pTempRoom->l_sRoomList.l_sRoomInfo, sizeof(l_sOnCreateRoom.l_sRoomInfo) );
			l_sOnCreateRoom.nUserNo = pTempRoom->m_User[0]->m_MyInfo.l_sUserInfo.nUserNo;

			memcpy( &pBuffer[nIndex], &l_sOnCreateRoom, sizeof(l_sOnCreateRoom) );
			nIndex += sizeof(l_sOnCreateRoom);

			break;

		case 'A':
		case 'X':
			sRoomInfoToWaitUserAX l_sRoomInfoToWaitUserAX;

			l_sRoomInfoToWaitUserAX.nRoomNo = nRoomNo;
			l_sRoomInfoToWaitUserAX.nRoomCnt = pTempRoom->l_sRoomList.l_sRoomInfo.nCurCnt;
			l_sRoomInfoToWaitUserAX.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;

			memcpy( &pBuffer[nIndex], &l_sRoomInfoToWaitUserAX, sizeof(l_sRoomInfoToWaitUserAX) );
			nIndex += sizeof(l_sRoomInfoToWaitUserAX);

			break;

		case 'R':
			sRoomInfoToWaitUserR l_sRoomInfoToWaitUserR;

			l_sRoomInfoToWaitUserR.nRoomNo = nRoomNo;
			l_sRoomInfoToWaitUserR.nRoomCnt = pTempRoom->l_sRoomList.l_sRoomInfo.nCurCnt;
			memcpy( &l_sRoomInfoToWaitUserR.l_sUserInfo, &d->m_MyInfo.l_sUserInfo, sizeof(d->m_MyInfo.l_sUserInfo) );

			memcpy( &pBuffer[nIndex], &l_sRoomInfoToWaitUserR, sizeof(l_sRoomInfoToWaitUserR) );
			nIndex += sizeof(l_sRoomInfoToWaitUserR);

			break;
		
		case 'G':
		case 'W':
			sRoomGameWait l_sRoomGameWait;

			l_sRoomGameWait.nRoomNo = nRoomNo;
			l_sRoomGameWait.cGameWait = cCmdDivi;

			memcpy( &pBuffer[nIndex], &l_sRoomGameWait, sizeof(l_sRoomGameWait) );
			nIndex += sizeof(l_sRoomGameWait);

			break;

		case 'M':

			break;

	}

	NGFrameHeader( pBuffer, nIndex, NGSC_WAITINFOCHANGE, ERROR_NOT );
	return nIndex;

}

int NGFrameRoomOutOtherSend( char *pBuffer, sRoomOutOtherSend *asRoomOutOtherSend )
{
	int nIndex = SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], asRoomOutOtherSend, sizeof(sRoomOutOtherSend) );
	nIndex += sizeof(sRoomOutOtherSend);


	NGFrameHeader( pBuffer, nIndex, NGSC_ROOMOUT_OTHERSEND, ERROR_NOT );
	
	return nIndex;

}

int NGFrameRoomIn( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo  )
{
	int nIndex = SC_HEADER_SIZE;
	CUserDescriptor *pTempd;
	sRoomInUserSend l_sRoomInUserSend[MAX_ROOM_IN];
	sRoomInAdd l_sRoomInAdd;

	memset( &l_sRoomInUserSend, 0x00, sizeof(l_sRoomInUserSend) );
	memset( &l_sRoomInAdd, 0x00, sizeof(l_sRoomInAdd) );
	

	memcpy( &pBuffer[nIndex], &g_World.m_Channel[nChannel].m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo, sizeof(sRoomInfo) );
	nIndex += sizeof(sRoomInfo);
	

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] != NULL ) {

			pTempd = g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i];

			l_sRoomInUserSend[i].bBbangJjang = pTempd->m_bBbangJjang;
			l_sRoomInUserSend[i].bRoomMaster = pTempd->m_bRoomMaster;

			memcpy( &l_sRoomInUserSend[i].l_sUserInfo, &pTempd->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );
			
			l_sRoomInUserSend[i].bChoice = pTempd->m_bChoice;
			l_sRoomInUserSend[i].nCardCnt = pTempd->m_nCardCnt;
			l_sRoomInUserSend[i].nGameJoin = pTempd->m_nGameJoin;
			memcpy( &l_sRoomInUserSend[i].nUserCard, &pTempd->m_nUserCard, sizeof(l_sRoomInUserSend[i].nUserCard) );				
			//memcpy( &l_sRoomInUserSend[i].szJokboCard, &pTempd->m_CPokerEvalution.m_szJokboCard, sizeof(pTempd->m_CPokerEvalution.m_szJokboCard) );
		}
	}

	memcpy( &pBuffer[nIndex], &l_sRoomInUserSend, sizeof(l_sRoomInUserSend) );
	nIndex += sizeof(l_sRoomInUserSend);

	l_sRoomInAdd.nRoomInUserNo = d->m_nRoomInUserNo;
	l_sRoomInAdd.nGameDivi = g_World.m_Channel[nChannel].m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nGameDivi;
	l_sRoomInAdd.biRoomCurMoney = g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_biRoomCurMoney;


	

	memcpy( &pBuffer[nIndex], &l_sRoomInAdd, sizeof(l_sRoomInAdd) );
	nIndex += sizeof(l_sRoomInAdd);

	NGFrameHeader( pBuffer, nIndex, NGSC_ROOMIN, ERROR_NOT );
	
	return nIndex;
}

/*
int NGFrameStartGame( char *pBuffer, CUserDescriptor *d, int nRoomMasterUniqNo, int nBbangJjangUniqNo, int nPrizeDecide, int nPrizeNo ) 
{
	int nIndex = SC_HEADER_SIZE;

	CRoomList *l_CRoomList;
	l_CRoomList = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];

	sStartUserCard l_sStartUserCard;

	memset( &l_sStartUserCard, 0x00, sizeof(l_sStartUserCard) );

	for ( int i = 0; i < d->m_nCardCnt; i++  ) {
		l_sStartUserCard.byCardNo[i] = d->m_nUserCard[i];
	}

	l_sStartUserCard.bChoiceRoom = l_CRoomList->l_sRoomList.l_sRoomInfo.bChoiceRoom;
	l_sStartUserCard.nCardCnt = d->m_nCardCnt;
	l_sStartUserCard.nRoomMasterUniqNo = nRoomMasterUniqNo;
	l_sStartUserCard.nBbanJjangUniqNo = nBbangJjangUniqNo;
	l_sStartUserCard.nPrizeDecide = nPrizeDecide;
	l_sStartUserCard.nPrizeNo = nPrizeNo;
	l_sStartUserCard.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;

	
	for ( i = 0; i < JACKPOT_CNT; i++ ) {
		if ( g_sJackPotInfo[i].biJackPotRoomMoney == l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney ) {
			l_sStartUserCard.biJackPotMoney = g_sJackPotInfo[i].biJackPotMoney;
			break;
		}
	}
	

	memcpy( &pBuffer[nIndex], &l_sStartUserCard, sizeof(l_sStartUserCard) );
	nIndex += sizeof(l_sStartUserCard);

	NGFrameHeader( pBuffer, nIndex, NGSC_STARTGAME, ERROR_NOT );
	
	return nIndex;
}
*/

/*
int NGFrameBadukiStart( char *pBuffer, CUserDescriptor *d, int nRoomMasterUniqNo, int nBbangJjangUniqNo, int nPrizeDecide, int nPrizeNo, int nMasterSlot )
{
	int nIndex = SC_HEADER_SIZE;

	CRoomList *l_CRoomList;
	l_CRoomList = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];

	sGameStart l_sGameStart;

	memset( &l_sGameStart, 0x00, sizeof(l_sGameStart) );

	for ( int i = 0; i < d->m_nCardCnt; i++  ) {
		l_sGameStart.l_sStartUserCard.byCardNo[i] = d->m_nUserCard[i];
	}

	l_sGameStart.l_sStartUserCard.bChoiceRoom = l_CRoomList->l_sRoomList.l_sRoomInfo.bChoiceRoom;
	l_sGameStart.l_sStartUserCard.nCardCnt = d->m_nCardCnt;
	l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo = nRoomMasterUniqNo;
	l_sGameStart.l_sStartUserCard.nBbanJjangUniqNo = nBbangJjangUniqNo;
	l_sGameStart.l_sStartUserCard.nPrizeDecide = nPrizeDecide;
	l_sGameStart.l_sStartUserCard.nPrizeNo = nPrizeNo;
	l_sGameStart.l_sStartUserCard.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;

	
	for ( i = 0; i < JACKPOT_CNT; i++ ) {
		if ( g_sJackPotInfo[i].biJackPotRoomMoney == l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney ) {
			l_sGameStart.l_sStartUserCard.biJackPotMoney = g_sJackPotInfo[i].biJackPotMoney;
			break;
		}
	}
	

	l_sGameStart.l_sSetGameCurUser.nBeforeGameUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
	l_sGameStart.l_sSetGameCurUser.cBeforeUserSelect = '0';
	l_sGameStart.l_sSetGameCurUser.biBeforeGameUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sGameStart.l_sSetGameCurUser.nCurGameUserUniNo = l_CRoomList->m_User[nMasterSlot]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sGameStart.l_sSetGameCurUser.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;
	memcpy( &l_sGameStart.l_sSetGameCurUser.l_sGameCurUser, &l_CRoomList->m_User[nMasterSlot]->m_sGameCurUser, sizeof(l_sGameStart.l_sSetGameCurUser.l_sGameCurUser) );

	memcpy( &pBuffer[nIndex], &l_sGameStart, sizeof(l_sGameStart) );
	nIndex += sizeof(l_sGameStart);

	NGFrameHeader( pBuffer, nIndex, NGSC_STARTGAME, ERROR_NOT );
	
	return nIndex;
}
*/

int NGFrameBadukiStart( char *pBuffer, CUserDescriptor *d, int nRoomMasterUniqNo, int nBbangJjangUniqNo, int nPrizeDecide, int nPrizeNo, int nMasterSlot )
{
	int nIndex = SC_HEADER_SIZE;

	CRoomList *l_CRoomList;
	l_CRoomList = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];

	sGameStart l_sGameStart;

	memset( &l_sGameStart, 0x00, sizeof(l_sGameStart) );

	/*
	for ( int i = 0; i < d->m_nCardCnt; i++  ) {
		l_sGameStart.l_sStartUserCard.byCardNo[i] = d->m_nUserCard[i];
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	// 선 다음 부터 카드를 담는다.
	int nRoomMasterRoomInUserNo = 0;

	for ( int j = 0; j < MAX_ROOM_IN; j++ ) 
	{
		if ( l_CRoomList->m_User[j] == NULL || l_CRoomList->m_User[j]->m_nGameJoin == 0 ) continue;

		if ( l_CRoomList->m_User[j]->m_MyInfo.l_sUserInfo.nUserNo == nRoomMasterUniqNo )
		{
			nRoomMasterRoomInUserNo = j;
			break;
		}

	}

	int nTemp = 0;
	for ( j = 0; j < MAX_ROOM_IN; j++ ) 
	{

		// 선 다음 부터 카드를 담는다.
		nTemp = ( nRoomMasterRoomInUserNo + j + 1) % MAX_ROOM_IN;
		
		if ( l_CRoomList->m_User[nTemp] == NULL || l_CRoomList->m_User[nTemp]->m_nGameJoin == 0 ) continue;
			 
		l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].nUserUniqNo = l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.nUserNo;
		l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].nCardCnt = l_CRoomList->m_User[nTemp]->m_nCardCnt;
		
		for ( int k = 0; k < l_CRoomList->m_User[nTemp]->m_nCardCnt; k++  ) {
			l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].byCardNo[k] =  l_CRoomList->m_User[nTemp]->m_nUserCard[k];
		}

	}
	//////////////////////////////////////////////////////////////////////////




	l_sGameStart.l_sStartUserCard.bChoiceRoom = l_CRoomList->l_sRoomList.l_sRoomInfo.bChoiceRoom;
	l_sGameStart.l_sStartUserCard.nCardCnt = d->m_nCardCnt;
	l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo = nRoomMasterUniqNo;
	l_sGameStart.l_sStartUserCard.nBbanJjangUniqNo = nBbangJjangUniqNo;
	l_sGameStart.l_sStartUserCard.nPrizeDecide = nPrizeDecide;
	l_sGameStart.l_sStartUserCard.nPrizeNo = nPrizeNo;
	l_sGameStart.l_sStartUserCard.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;

	/*
	for ( int i = 0; i < JACKPOT_CNT; i++ ) {
		if ( g_sJackPotInfo[i].biJackPotRoomMoney == l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney ) {
			l_sGameStart.l_sStartUserCard.biJackPotMoney = g_sJackPotInfo[i].biJackPotMoney;
			break;
		}
	}
	*/

//	g_pDbQ->GetJackPotMoney();
	
	l_sGameStart.l_sStartUserCard.biJackPotMoney = g_sJackPotInfo[0].biJackPotMoney;


	l_sGameStart.l_sSetGameCurUser.nBeforeGameUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
	l_sGameStart.l_sSetGameCurUser.cBeforeUserSelect = '0';
	l_sGameStart.l_sSetGameCurUser.biBeforeGameUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sGameStart.l_sSetGameCurUser.nCurGameUserUniNo = l_CRoomList->m_User[nMasterSlot]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sGameStart.l_sSetGameCurUser.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;
	memcpy( &l_sGameStart.l_sSetGameCurUser.l_sGameCurUser, &l_CRoomList->m_User[nMasterSlot]->m_sGameCurUser, sizeof(l_sGameStart.l_sSetGameCurUser.l_sGameCurUser) );

	memcpy( &pBuffer[nIndex], &l_sGameStart, sizeof(l_sGameStart) );
	nIndex += sizeof(l_sGameStart);

	NGFrameHeader( pBuffer, nIndex, NGSC_STARTGAME, ERROR_NOT );
	
	return nIndex;
}





	int NGFrameGameViewWait( char *pBuffer, int nUserChannel, int nUserReqPage )
	{
		int nIndex = SC_HEADER_SIZE;

		int nDataCnt = 0;

		CChannelUserList *pUserList, *pNextUserList, *pTemp;

		sGameWaitInfo l_sGameWaitInfo[MAX_GAMEVIEWWAIT];

		memset( l_sGameWaitInfo, 0x00, sizeof(l_sGameWaitInfo) );

		pTemp = g_World.m_Channel[nUserChannel].m_pWaitChannelUserList;  	//해당채널에서 대기실의 유저를 가져온다.
		for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {	//유저정보를 담는다.	

			if ( pUserList->m_User->m_sOption.bInvite == false ) continue;
			
			l_sGameWaitInfo[nDataCnt].cSex = pUserList->m_User->m_MyInfo.l_sUserInfo.cSex;
			l_sGameWaitInfo[nDataCnt].biUserMoney = pUserList->m_User->m_MyInfo.l_sUserInfo.biUserMoney;
			l_sGameWaitInfo[nDataCnt].nUserNo = pUserList->m_User->m_MyInfo.l_sUserInfo.nUserNo;
			strcpy( l_sGameWaitInfo[nDataCnt].szNick_Name, pUserList->m_User->m_MyInfo.l_sUserInfo.szNick_Name );

			nDataCnt++;

			if ( nDataCnt >= MAX_GAMEVIEWWAIT  ) break;

		}

		pBuffer[nIndex++] = HIBYTE(LOWORD(nDataCnt));
		pBuffer[nIndex++] = LOBYTE(LOWORD(nDataCnt));	

		memcpy( &pBuffer[nIndex], l_sGameWaitInfo, sizeof(sGameWaitInfo) * nDataCnt );
		nIndex += sizeof(sGameWaitInfo) * nDataCnt;
		
		NGFrameHeader( pBuffer, nIndex, NGSC_VIEWWAIT, ERROR_NOT );
		
		return nIndex;
	}


int NGFrameRoomInOtherSend( char *pBuffer, CUserDescriptor *d, int nNextNo )
{

	int nIndex = SC_HEADER_SIZE;

	sUserInfo l_sUserInfo;
	sRoomInOtherSend l_sRoomInOtherSend;

	memset( &l_sUserInfo, 0x00, sizeof(l_sUserInfo) );
	memset( &l_sRoomInOtherSend, 0x00, sizeof(l_sRoomInOtherSend) );

	memcpy( &pBuffer[nIndex], &d->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );
	nIndex += sizeof(sUserInfo);

	l_sRoomInOtherSend.nNextNo = nNextNo;
	l_sRoomInOtherSend.nRoomCurCnt = g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt;

	CRoomList *pTempRoom;

	pTempRoom = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( pTempRoom->m_User[i] != NULL ) {

			if ( pTempRoom->m_User[i]->m_bBbangJjang ) {

				l_sRoomInOtherSend.nBbangJjangUniqNo = pTempRoom->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
				break;
			}

		}

	}

	memcpy( &pBuffer[nIndex], &l_sRoomInOtherSend, sizeof(l_sRoomInOtherSend) );
	nIndex += sizeof(l_sRoomInOtherSend);
	

	NGFrameHeader( pBuffer, nIndex, NGSC_ROOMIN_OTHERSEND, ERROR_NOT );
	
	return nIndex;
}

int NGFrameWaitChat( char *pBuffer, int nUserNo, char *pNickName, int nChatMsgLen, char *pChatMsg )
{
	int nIndex = SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], pNickName, MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;	

	pBuffer[nIndex++] = HIBYTE(HIWORD(nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nUserNo));

	pBuffer[nIndex++] = HIBYTE(LOWORD(nChatMsgLen));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nChatMsgLen));		

	memcpy( &pBuffer[nIndex], pChatMsg, nChatMsgLen );
	nIndex += nChatMsgLen;

	NGFrameHeader( pBuffer, nIndex, NGSC_WAITCHAT, ERROR_NOT );
	
	return nIndex;
}

int NGFrameGameChat( char *pBuffer, int nUserNo, char *pNickName, int nChatMsgLen, char *pChatMsg )
{
	int nIndex = SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], pNickName, MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;	

	pBuffer[nIndex++] = HIBYTE(HIWORD(nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nUserNo));

	pBuffer[nIndex++] = HIBYTE(LOWORD(nChatMsgLen));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nChatMsgLen));		

	memcpy( &pBuffer[nIndex], pChatMsg, nChatMsgLen );
	nIndex += nChatMsgLen;

	NGFrameHeader( pBuffer, nIndex, NGSC_GAMECHAT, ERROR_NOT );
	
	return nIndex;
}

int NGFrameViewPage( char *pBuffer, int nUserChannel, int nUserReqPage )
{
	int nIndex = SC_HEADER_SIZE;

	int nInit = nUserReqPage * PAGE_ROOMCNT;
	int nFinal = nInit + PAGE_ROOMCNT;
	int nRoomTemp = 0;
	int nRoomCnt = 0;
	CRoomList *pTempRoom;
	
	nRoomTemp = nIndex;
	nIndex += sizeof(ush_int); 

	for ( int i = nInit; i < nFinal; i++ ) {
		
		pTempRoom = &g_World.m_Channel[nUserChannel].m_RoomList[i];

		if ( pTempRoom->l_sRoomList.l_sRoomInfo.nRoomNo == -1 ) continue;  

		nRoomCnt++;

		memcpy( &pBuffer[nIndex], &pTempRoom->l_sRoomList.l_sRoomInfo, sizeof(sRoomInfo) );
		nIndex += sizeof(sRoomInfo);
	}

	pBuffer[nRoomTemp++] = HIBYTE(LOWORD(nRoomCnt));
	pBuffer[nRoomTemp++] = LOBYTE(LOWORD(nRoomCnt));

	NGFrameHeader( pBuffer, nIndex, NGSC_VIEWPAGE, ERROR_NOT );
	
	return nIndex;

}


int NGFrameKeepAlive( char *pBuffer )
{
	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_KEEPALIVE, ERROR_NOT );
	
	return nIndex;
}

int NGFrameClientAllCmt( char *pBuffer, char *pAllSendCmt, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;


	memcpy( &pBuffer[nIndex], pAllSendCmt, strlen(pAllSendCmt) + 1 );
	nIndex += strlen(pAllSendCmt) + 1;


	NGFrameHeader( pBuffer, nIndex, NGSC_ALLCMT, ERROR_NOT );

	return nIndex;
}

int NGFrameIpCheck( char *pBuffer )
{

	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_IPCHECK, ERROR_NOT );
	
	return nIndex;	
}

int NGFrameCardChoice( char *pBuffer, int nUserNo, BYTE nCardNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(HIWORD(nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nUserNo));


	NGFrameHeader( pBuffer, nIndex, NGSC_CARDCHOICE, ERROR_NOT );

	return nIndex;
}

int NGFrameChoiceCardView( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo )
{
	int nIndex = SC_HEADER_SIZE;

	CRoomList *l_CRoomList;
	sChoiceViewCard l_sChoiceViewCard[MAX_ROOM_IN];

	memset( l_sChoiceViewCard, 0x00, sizeof(l_sChoiceViewCard) );

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;

		l_sChoiceViewCard[i].nCallUserUniNo = l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
		l_sChoiceViewCard[i].byCardNo = l_CRoomList->m_User[i]->m_nUserCard[2];
	}

	memcpy( &pBuffer[nIndex], &l_sChoiceViewCard, sizeof(l_sChoiceViewCard) );
	nIndex += sizeof(l_sChoiceViewCard);

	NGFrameHeader( pBuffer, nIndex, NGSC_CHOICECARDVIEW, ERROR_NOT );

	return nIndex;
}

int NGFrameCardDump( char *pBuffer, int nUserNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(HIWORD(nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nUserNo));

	NGFrameHeader( pBuffer, nIndex, NGSC_CARDDUMP, ERROR_NOT );

	return nIndex;
}

int NGFrameCardChange( char *pBuffer, int nUserNo, sUserNewCard *psUserNewCard, BOOL bPass  )
{
	int nIndex = SC_HEADER_SIZE;

	sUserNewCardPacket l_sUserNewCardPacket;
	memset( &l_sUserNewCardPacket, 0x00, sizeof(l_sUserNewCardPacket) );

	l_sUserNewCardPacket.bPass = bPass;
	l_sUserNewCardPacket.nUniqUserNo = nUserNo;
	memcpy( &l_sUserNewCardPacket.l_sUserNewCard, psUserNewCard, sizeof(sUserNewCard) );

	

	memcpy( &pBuffer[nIndex], &l_sUserNewCardPacket, sizeof(l_sUserNewCardPacket) );
	nIndex += sizeof(l_sUserNewCardPacket);

	NGFrameHeader( pBuffer, nIndex, NGSC_CHANGECARD, ERROR_NOT );

	return nIndex;
}

int NGFrameGetOneCard( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo, int nBeforeRoomMasterRoomInUserNo, char cUserSelect )
{
	int nIndex = SC_HEADER_SIZE;


	int nUserCardCnt = 0;
	int nTemp = 0;
	CRoomList *l_CRoomList;

	sGetOneCard l_sGetOneCard;
	memset( &l_sGetOneCard, 0x00, sizeof(l_sGetOneCard) );

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	for ( int j = 0; j < MAX_ROOM_IN; j++ ) {

		nTemp = ( nBeforeRoomMasterRoomInUserNo + j ) % MAX_ROOM_IN;
		
		if ( l_CRoomList->m_User[nTemp] == NULL || l_CRoomList->m_User[nTemp]->m_nGameJoin == 0 ) continue;
			 
		nUserCardCnt = l_CRoomList->m_User[nTemp]->m_nCardCnt;
		l_sGetOneCard.l_sUserOneCardInfo[j].byCardNo = l_CRoomList->m_User[nTemp]->m_nUserCard[nUserCardCnt - 1];
		l_sGetOneCard.l_sUserOneCardInfo[j].nUserUniqueNo = l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.nUserNo;
		
		memcpy( l_sGetOneCard.szJokboCard[j], l_CRoomList->m_User[nTemp]->m_CPokerEvalution.m_szJokboCard, sizeof(l_CRoomList->m_User[nTemp]->m_CPokerEvalution.m_szJokboCard) );
	}
	int nRoomMasterRoomInUserNo = 0;

	nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );
	l_sGetOneCard.nRoomMasterUniqNo = l_CRoomList->m_User[nRoomMasterRoomInUserNo]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sGetOneCard.nRound = l_CRoomList->m_nRound;

	DTRACE("PokerBtnSet - 4");
	l_CRoomList->m_CPokerEngine.PokerBtnSet( nChannel, nRoomNo, nRoomMasterRoomInUserNo );

	l_CRoomList->m_CPokerEngine.PokerCalcuRaceMoney( nChannel, nRoomNo, nRoomMasterRoomInUserNo );

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( l_CRoomList->m_User[i] == NULL ) continue;

		l_CRoomList->m_User[i]->m_bMyOrder = FALSE;
	}

	l_CRoomList->m_User[nRoomMasterRoomInUserNo]->m_bMyOrder = TRUE;



	memcpy( &l_sGetOneCard.l_sGameCurUser, &l_CRoomList->m_User[nRoomMasterRoomInUserNo]->m_sGameCurUser, sizeof(l_CRoomList->m_User[nRoomMasterRoomInUserNo]->m_sGameCurUser) );

	l_sGetOneCard.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;
	l_sGetOneCard.cUserSelect = cUserSelect;
	l_sGetOneCard.biCallUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;

	if ( cUserSelect != -1 ) {
		int nCallUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
		l_sGetOneCard.nCallUserUniNo = nCallUserUniNo;
	}
	else {
		l_sGetOneCard.nCallUserUniNo = 0;
	}
	
	memcpy( &pBuffer[nIndex], &l_sGetOneCard, sizeof(l_sGetOneCard) );
	nIndex += sizeof(l_sGetOneCard);


	NGFrameHeader( pBuffer, nIndex, NGSC_GETONECARD, ERROR_NOT );

	return nIndex;
}


int NGFrameMyUserJokBo( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;

	sMyUserJokBo l_sMyUserJokBo;
	memset( &l_sMyUserJokBo, 0x00, sizeof(l_sMyUserJokBo) );

	d->m_CPokerEvalution.Calcul( d->m_nUserCard, d->m_nCardCnt );
	memcpy( &l_sMyUserJokBo, d->m_CPokerEvalution.m_szJokboCard, sizeof(d->m_CPokerEvalution.m_szJokboCard) );


	memcpy( &pBuffer[nIndex], &l_sMyUserJokBo, sizeof(l_sMyUserJokBo) );
	nIndex += sizeof(l_sMyUserJokBo);	

	NGFrameHeader( pBuffer, nIndex, NGSC_MYUSERJOKBO, ERROR_NOT );
	return nIndex;


}


int NGFrameAllUserJokBo( char *pBuffer, int nChannel, int nRoomNo )
{
	int nIndex = SC_HEADER_SIZE;

	sAllUserJokBo l_sAllUserJokBo;
	memset( &l_sAllUserJokBo, 0x00, sizeof(l_sAllUserJokBo) );

	CRoomList *l_CRoomList;
	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];
	

	for ( int j = 0; j < MAX_ROOM_IN; j++ ) 
	{
		if ( l_CRoomList->m_User[j] == NULL || l_CRoomList->m_User[j]->m_nGameJoin == 0 ) continue;

		l_sAllUserJokBo.nUserUniqNo[j] = l_CRoomList->m_User[j]->m_MyInfo.l_sUserInfo.nUserNo;
		memcpy( &l_sAllUserJokBo.l_sMyUserJokBo[j], l_CRoomList->m_User[j]->m_CPokerEvalution.m_szJokboCard, sizeof(l_CRoomList->m_User[j]->m_CPokerEvalution.m_szJokboCard) );
	}

	memcpy( &pBuffer[nIndex], &l_sAllUserJokBo, sizeof(l_sAllUserJokBo) );
	nIndex += sizeof(l_sAllUserJokBo);	

	NGFrameHeader( pBuffer, nIndex, NGSC_ALLUSERJOKBO, ERROR_NOT );
	return nIndex;

}


int NGFrameGetOutQues( char *pBuffer, CUserDescriptor *d, int nGetOutUserNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(HIWORD(nGetOutUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nGetOutUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nGetOutUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nGetOutUserNo));	

	NGFrameHeader( pBuffer, nIndex, NGSC_GETOUTQUES, ERROR_NOT );
	
	return nIndex;
}

int NGFrameGetOut( char *pBuffer, CUserDescriptor *d, int nGetOutUserNo )
{
	int nIndex = SC_HEADER_SIZE;


	pBuffer[nIndex++] = HIBYTE(HIWORD(nGetOutUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nGetOutUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nGetOutUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nGetOutUserNo));	

	NGFrameHeader( pBuffer, nIndex, NGSC_GETOUT, ERROR_NOT );
	
	return nIndex;
}

int NGFrameEndGame( char *pBuffer, int nChannel, int nRoomNo, int nMasterSlotNo, int nBeforeRoomMasterRoomInUserNo, CUserDescriptor *d, 
				   char cUserSelect, ush_int nJackPot, BigInt biUserJackPotMoney, int *nDrawWinnerUniq, BOOL bDrawGame )
{
	int nIndex = SC_HEADER_SIZE;

	sEndGame l_sEndGame;

	memset( &l_sEndGame, 0x00, sizeof(l_sEndGame) );

	CRoomList *l_CRoomList;
	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];
	
	l_sEndGame.nBeforeGameUserUniNo = l_CRoomList->m_User[nBeforeRoomMasterRoomInUserNo]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sEndGame.nWinnerUserUniNo = l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sEndGame.biWinnerUserMoney = l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sEndGame.biPureWinnerMoney = l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney;

	l_sEndGame.nCallUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
	l_sEndGame.biCallGameUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sEndGame.cUserSelect = cUserSelect;
	l_sEndGame.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;

	int nTemp = 0;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		nTemp = ( nMasterSlotNo + i ) % MAX_ROOM_IN;


		if ( l_CRoomList->m_User[nTemp] != NULL && l_CRoomList->m_User[nTemp]->m_nCardCnt >= 0 ) {
			l_sEndGame.l_sUserTotalCardInfo[i].nUserUniqueNo = l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.nUserNo;
			memcpy( l_sEndGame.l_sUserTotalCardInfo[i].byCardNo, l_CRoomList->m_User[nTemp]->m_nUserCard, sizeof(l_CRoomList->m_User[nTemp]->m_nUserCard) );

			l_sEndGame.l_biUserMoney[i] = l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.biUserMoney;
			l_sEndGame.l_biPureUserWinLoseMoney[i] = l_CRoomList->m_User[nTemp]->m_biTempRealRaceMoney;

			memcpy( l_sEndGame.szJokboCard[i], l_CRoomList->m_User[nTemp]->m_CPokerEvalution.m_szJokboCard, sizeof(l_CRoomList->m_User[nTemp]->m_CPokerEvalution.m_szJokboCard) );
			memcpy( l_sEndGame.szFixJokboCard[i], l_CRoomList->m_User[nTemp]->m_CPokerEvalution.m_szFixJokboCard, sizeof(l_CRoomList->m_User[nTemp]->m_CPokerEvalution.m_szFixJokboCard) );
		}
	}

	l_sEndGame.nJackPot = nJackPot;
	l_sEndGame.biUserJackPotMoney = biUserJackPotMoney;

	l_sEndGame.bDrawGame = bDrawGame;
	memcpy( l_sEndGame.nDrawWinnerUniq, nDrawWinnerUniq, sizeof(l_sEndGame.nDrawWinnerUniq) );


	memcpy( &pBuffer[nIndex], &l_sEndGame, sizeof(l_sEndGame) );
	nIndex += sizeof(l_sEndGame);

	NGFrameHeader( pBuffer, nIndex, NGSC_ENDGAME, ERROR_NOT );

	return nIndex;


}

int NGFrameMyWinGame( char *pBuffer, int nChannel, int nRoomNo, int nMasterSlotNo, CUserDescriptor *d, char cUserSelect  )
{
	int nIndex = SC_HEADER_SIZE;

	sMyWinGame l_sMyWinGame;

	memset( &l_sMyWinGame, 0x00, sizeof(l_sMyWinGame) );

	CRoomList *l_CRoomList;
	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	l_sMyWinGame.nWinnerUserUniNo = l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sMyWinGame.biWinnerUserMoney = l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sMyWinGame.biPureWinnerMoney = l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney;

	l_sMyWinGame.nCallUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
	l_sMyWinGame.biCallGameUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sMyWinGame.cUserSelect = cUserSelect;
	l_sMyWinGame.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( l_CRoomList->m_User[i] != NULL && l_CRoomList->m_User[i]->m_nCardCnt >= 0 ) {

			l_sMyWinGame.l_nUserUniqNo[i] = l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;	
			l_sMyWinGame.l_biUserMoney[i] = l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.biUserMoney;
			l_sMyWinGame.l_biPureUserWinLoseMoney[i] = l_CRoomList->m_User[i]->m_biTempRealRaceMoney;
		}
	}


	memcpy( &pBuffer[nIndex], &l_sMyWinGame, sizeof(l_sMyWinGame) );
	nIndex += sizeof(l_sMyWinGame);

	NGFrameHeader( pBuffer, nIndex, NGSC_MYWINGAME, ERROR_NOT );

	return nIndex;


}

int NGFrameMemo( char *pBuffer, char *pSendNickName, int nMsgLen, char *pMsg )
{
	int nIndex = SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], pSendNickName, MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;

	pBuffer[nIndex++] = HIBYTE(LOWORD(nMsgLen));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nMsgLen));		

	memcpy( &pBuffer[nIndex], pMsg, nMsgLen );
	nIndex += nMsgLen;

	NGFrameHeader( pBuffer, nIndex, NGSC_MEMO, ERROR_NOT );
	
	return nIndex;
}

int NGFrameNagariGame( char *pBuffer )
{
	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_NAGARIGAME, ERROR_NOT );
	
	return nIndex;	
}

int NGFrameSetGameCurUser( char *pBuffer, CUserDescriptor *d, char cBeforeUserSelect, int nCurUserSlotNo )
{
	int nIndex = SC_HEADER_SIZE;

	int nSetUserUniNo = 0;
	int nUserCardCnt = 0;
	int nTemp = 0;
	CRoomList *l_CRoomList;
	int nChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nRoomNo = d->m_nRoomNo;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];


	sSetGameCurUser l_sSetGameCurUser;
	memset( &l_sSetGameCurUser, 0x00, sizeof(l_sSetGameCurUser) );

	l_sSetGameCurUser.nBeforeGameUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
	l_sSetGameCurUser.cBeforeUserSelect = cBeforeUserSelect;
	l_sSetGameCurUser.biBeforeGameUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sSetGameCurUser.nCurGameUserUniNo = l_CRoomList->m_User[nCurUserSlotNo]->m_MyInfo.l_sUserInfo.nUserNo;
	l_sSetGameCurUser.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;

	memcpy( &l_sSetGameCurUser.l_sGameCurUser, &l_CRoomList->m_User[nCurUserSlotNo]->m_sGameCurUser, sizeof(l_sSetGameCurUser.l_sGameCurUser) );

	
	memcpy( &pBuffer[nIndex], &l_sSetGameCurUser, sizeof(l_sSetGameCurUser) );
	nIndex += sizeof(l_sSetGameCurUser);	

	NGFrameHeader( pBuffer, nIndex, NGSC_SETGAMECURUSER, ERROR_NOT );

	return nIndex;
}

int NGFrameStartBtnOnOff( char *pBuffer, CUserDescriptor *d, char cBtnOnOff )	//cBtnOnOff - 0 : Off, 1 : On
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = cBtnOnOff;

	NGFrameHeader( pBuffer, nIndex, NGSC_STARTBTN_ONOFF, ERROR_NOT );

	return nIndex;
	
}

int NGFrameUserInvite( char *pBuffer, CUserDescriptor *d, int nInviteMsgLen, char *pInviteMsg )
{
	int nIndex = SC_HEADER_SIZE;

	sUserInvite l_sUserInvite;

	memset( &l_sUserInvite, 0x00, sizeof(l_sUserInvite) );

	l_sUserInvite.nRoomNo = d->m_nRoomNo;
	l_sUserInvite.biRoomMoney = g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney;
	memcpy( l_sUserInvite.szRoomPass, g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.szRoom_Pass, MAX_ROOM_PASSWORD );
	l_sUserInvite.nInviteMsgLen = nInviteMsgLen;

	memcpy( &pBuffer[nIndex], &l_sUserInvite, sizeof(l_sUserInvite) );
	nIndex += sizeof(l_sUserInvite);


	memcpy( &pBuffer[nIndex], pInviteMsg, nInviteMsgLen );
	nIndex += nInviteMsgLen;

	NGFrameHeader( pBuffer, nIndex, NGSC_USERINVITE, ERROR_NOT );
	
	return nIndex;
}

int NGFrameChangeOkAll( char *pBuffer, CUserDescriptor *d, int nRound, char cBeforeUserSelect )
{
	int nIndex = SC_HEADER_SIZE;

	CRoomList *l_CRoomList;
	int nChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nRoomNo = d->m_nRoomNo;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];


	sChangeOkAll l_sChangeOkAll;

	l_sChangeOkAll.biBeforeGameUserMoney = d->m_MyInfo.l_sUserInfo.biUserMoney;
	l_sChangeOkAll.biRoomCurMoney = l_CRoomList->m_biRoomCurMoney;
	l_sChangeOkAll.cBeforeUserSelect = cBeforeUserSelect;
	l_sChangeOkAll.nBeforeGameUserUniNo = d->m_MyInfo.l_sUserInfo.nUserNo;
	l_sChangeOkAll.nRound = nRound;

	memcpy( &pBuffer[nIndex], &l_sChangeOkAll, sizeof(l_sChangeOkAll) );
	nIndex += sizeof(l_sChangeOkAll);


	NGFrameHeader( pBuffer, nIndex, NGSC_CHANGEOKALL, ERROR_NOT );
	
	return nIndex;
}

int NGFrameChangeOkUser( char *pBuffer, int nMasterUniqUserNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(HIWORD(nMasterUniqUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nMasterUniqUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nMasterUniqUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nMasterUniqUserNo));	

	NGFrameHeader( pBuffer, nIndex, NGSC_CHANGEOKUSER, ERROR_NOT );
	
	return nIndex;
}


int NGFrameUserChange( char *pBuffer, sUserChange *pUserChange )
{
	int nIndex = SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], pUserChange, sizeof(sUserChange) );
	nIndex += sizeof(sUserChange);

	NGFrameHeader( pBuffer, nIndex, NGSC_USERCHANGE, ERROR_NOT );
	
	return nIndex;

}
//***************************************************
int NGDeFrameHeader( char *pBuffer, sCSHeaderPkt *asCSHeaderPkt )
{

	int index = 0;

	memset( asCSHeaderPkt, 0x00, sizeof(sCSHeaderPkt) );

	memcpy( asCSHeaderPkt, pBuffer, sizeof(sCSHeaderPkt) );

	return sizeof(sCSHeaderPkt);

}


int NGDeFrmaeChat( char *pBuffer, char *pNickName, int *nUserNo,  int *pChatMsgLen, char *pMsg )
{

	int nIndex = 0;

	memcpy( pNickName, &pBuffer[nIndex], MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;	

	*nUserNo = MAKELONG( MAKEWORD( pBuffer[nIndex + 3], pBuffer[nIndex + 2] ) ,
				 MAKEWORD( pBuffer[nIndex + 1], pBuffer[nIndex + 0] ) );
	nIndex += 4;

	*pChatMsgLen = MAKEWORD( pBuffer[nIndex+1], pBuffer[nIndex] );
	nIndex += 2;

	memcpy( pMsg, &pBuffer[nIndex], *pChatMsgLen );
	nIndex += *pChatMsgLen;

	return nIndex;
}



int NGDeFrameUserInvite(  char *pBuffer, int *nUserInviteNo,  int *pInviteMsgLen, char *pInviteMsg )
{

	int nIndex = 0;

	*nUserInviteNo = MAKELONG( MAKEWORD( pBuffer[nIndex + 3], pBuffer[nIndex + 2] ) ,
				 MAKEWORD( pBuffer[nIndex + 1], pBuffer[nIndex + 0] ) );
	nIndex += 4;

	*pInviteMsgLen = MAKEWORD( pBuffer[nIndex+1], pBuffer[nIndex] );
	nIndex += 2;

	memcpy( pInviteMsg, &pBuffer[nIndex], *pInviteMsgLen );
	nIndex += *pInviteMsgLen;

	return nIndex;

}

int NGDeFrmaeMemo( char *pBuffer, char *pSendNickName, char *pRecvNickName, int *pChatMsgLen, char *pMsg )
{

	int nIndex = 0;

	memcpy( pSendNickName, &pBuffer[nIndex], MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;

	memcpy( pRecvNickName, &pBuffer[nIndex], MAX_NICK_LENGTH );
	nIndex += MAX_NICK_LENGTH;

	*pChatMsgLen = MAKEWORD( pBuffer[nIndex+1], pBuffer[nIndex] );
	nIndex += 2;

	memcpy( pMsg, &pBuffer[nIndex], *pChatMsgLen );
	nIndex += *pChatMsgLen;

	return nIndex;
}