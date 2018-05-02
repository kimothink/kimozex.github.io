#include "NGFrame.h"
#include "World.h"
#include "Channel.h"

extern World g_World;
extern World g_AdminWorld;
extern CUserDescriptor *g_AdminUser;

//extern CUserDescriptor 

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

	sWaitUserInfo l_sWaitUserInfo;
	
	memset( &l_sWaitUserInfo, 0x00, sizeof(l_sWaitUserInfo) );


	pBuffer[nIndex++] = HIBYTE(HIWORD(d->m_MyInfo.l_sUserInfo.nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(d->m_MyInfo.l_sUserInfo.nUserNo));

	
	nRoomTemp = nIndex;
	nIndex += sizeof(ush_int); 

	for ( int i = 0; i < MAX_ROOMCNT; i++ ) {	
		
		pTempRoom = &g_World.m_Channel[nUserChannel].m_RoomList[i];

		if ( pTempRoom->l_sRoomList.l_sRoomInfo.nRoomNo == -1 ) continue; 

		nRoomCnt++;

		memcpy( &pBuffer[nIndex], &pTempRoom->l_sRoomList.l_sRoomInfo, sizeof(sRoomInfo) );
		nIndex += sizeof(sRoomInfo);

	}


	pBuffer[nRoomTemp++] = HIBYTE(LOWORD(nRoomCnt));
	pBuffer[nRoomTemp++] = LOBYTE(LOWORD(nRoomCnt));	

	nTemp =  nIndex; 
	nIndex += sizeof(ush_int); 
	
	pTemp = g_World.m_Channel[nUserChannel].m_pWaitChannelUserList;  	//해당채널에서 대기실의 유저를 가져온다.
	for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {	//유저정보를 담는다.	

		nUserCnt++;

		l_sWaitUserInfo.cSex = pUserList->m_User->m_MyInfo.l_sUserInfo.cSex;
		l_sWaitUserInfo.fWinRate = pUserList->m_User->m_MyInfo.l_sUserInfo.fWinRate;
		l_sWaitUserInfo.biMaxMoney = pUserList->m_User->m_MyInfo.l_sUserInfo.biMaxMoney;
		l_sWaitUserInfo.biUserMoney = pUserList->m_User->m_MyInfo.l_sUserInfo.biUserMoney;
		l_sWaitUserInfo.nChannel = pUserList->m_User->m_MyInfo.l_sUserInfo.nChannel;
		l_sWaitUserInfo.nClass = pUserList->m_User->m_MyInfo.l_sUserInfo.nClass;
		l_sWaitUserInfo.nCut_Time = pUserList->m_User->m_MyInfo.l_sUserInfo.nCut_Time;
		l_sWaitUserInfo.nDraw_Num = pUserList->m_User->m_MyInfo.l_sUserInfo.nDraw_Num;
		l_sWaitUserInfo.nLose_Num = pUserList->m_User->m_MyInfo.l_sUserInfo.nLose_Num;
		l_sWaitUserInfo.nUserNo = pUserList->m_User->m_MyInfo.l_sUserInfo.nUserNo;
		l_sWaitUserInfo.nWin_Num = pUserList->m_User->m_MyInfo.l_sUserInfo.nWin_Num;
		l_sWaitUserInfo.bInvite = pUserList->m_User->m_sOption.bInvite;
		
		strcpy( l_sWaitUserInfo.szGameGubun, pUserList->m_User->m_MyInfo.l_sUserInfo.szGameGubun );
		strcpy( l_sWaitUserInfo.szNick_Name, pUserList->m_User->m_MyInfo.l_sUserInfo.szNick_Name );

		memcpy( &pBuffer[nIndex], &l_sWaitUserInfo, sizeof(l_sWaitUserInfo) );
		nIndex += sizeof(l_sWaitUserInfo);

		if ( nUserCnt > 20 ) break;

	}

	pBuffer[nTemp++] = HIBYTE(LOWORD(nUserCnt));
	pBuffer[nTemp++] = LOBYTE(LOWORD(nUserCnt));	


	NGFrameHeader( pBuffer, nIndex, NGSC_WAITINPUT, ERROR_NOT );



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


	pBuffer[nIndex++] = HIBYTE(LOWORD(nRoomNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nRoomNo));	

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
	
			if ( pTempd == d ) continue;

			l_sRoomInUserSend[i].bRoomMaster = pTempd->m_bRoomMaster;
			l_sRoomInUserSend[i].bBbangJjang = pTempd->m_bBbangJjang;


			memcpy( &l_sRoomInUserSend[i].l_sUserInfo, &pTempd->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );

			memcpy( &l_sRoomInUserSend[i].nVoice, &pTempd->m_sOption.nVoice, sizeof(pTempd->m_sOption.nVoice) );
		}

	}

	memcpy( &pBuffer[nIndex], &l_sRoomInUserSend, sizeof(l_sRoomInUserSend) );
	nIndex += sizeof(l_sRoomInUserSend);

	l_sRoomInAdd.nRoomInUserNo = d->m_nRoomInUserNo;
	memcpy( &pBuffer[nIndex], &l_sRoomInAdd, sizeof(l_sRoomInAdd) );
	nIndex += sizeof(l_sRoomInAdd);

	NGFrameHeader( pBuffer, nIndex, NGSC_ROOMIN, ERROR_NOT );
	
	return nIndex;	

}

	int NGFrameRoomInOtherSend( char *pBuffer, CUserDescriptor *d, int nBangJangUniqNo, int nNextNo, int nRoomCurCnt )
	{

		int nIndex = SC_HEADER_SIZE;

		sRoomInOtherSend l_sRoomInOtherSend;

		memset( &l_sRoomInOtherSend, 0x00, sizeof(l_sRoomInOtherSend) );

		l_sRoomInOtherSend.nNextNo = nNextNo;
		l_sRoomInOtherSend.nRoomCurCnt = nRoomCurCnt;
		l_sRoomInOtherSend.nBbangJjangUniqNo = nBangJangUniqNo;
		
		memcpy( &l_sRoomInOtherSend.l_sUserInfo, &d->m_MyInfo.l_sUserInfo, sizeof(sUserInfo) );
		
		memcpy( &pBuffer[nIndex], &l_sRoomInOtherSend, sizeof(l_sRoomInOtherSend) );
		nIndex += sizeof(l_sRoomInOtherSend);

		pBuffer[nIndex++] = HIBYTE(HIWORD(d->m_sOption.nVoice));  	//사용자의 고유 번호를 넘겨준다.
		pBuffer[nIndex++] = LOBYTE(HIWORD(d->m_sOption.nVoice));
		pBuffer[nIndex++] = HIBYTE(LOWORD(d->m_sOption.nVoice));
		pBuffer[nIndex++] = LOBYTE(LOWORD(d->m_sOption.nVoice));

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

int NGFrmaeResultUpdateEnd( char *pBuffer )
{
	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_GAMEEND, ERROR_NOT );
	
	return nIndex;
}

int NGFrameGameViewWait( char *pBuffer, int nUserChannel, int nUserReqPage )
{
	int nIndex = SC_HEADER_SIZE;

	int nDataCnt = 0;

	CChannelUserList *pUserList, *pNextUserList, *pTemp;

	sGameWaitInfo l_sGameWaitInfo[MAX_GAMEVIEWWAIT];

	memset( l_sGameWaitInfo, 0x00, sizeof(l_sGameWaitInfo) );

	pTemp = g_World.m_Channel[nUserChannel].m_pWaitChannelUserList;  	
	for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {	

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

int NGFrameUserInvite( char *pBuffer, CUserDescriptor *d, int nInviteMsgLen, char *pInviteMsg )
{
	int nIndex = SC_HEADER_SIZE;

	sUserInvite l_sUserInvite;

	memset( &l_sUserInvite, 0x00, sizeof(l_sUserInvite) );

	l_sUserInvite.nRoomNo = d->m_nRoomNo;
	l_sUserInvite.biRoomMoney = g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney;
	memcpy( &l_sUserInvite.szRoomPass, g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.szRoom_Pass, MAX_ROOM_PASSWORD );
	l_sUserInvite.nInviteLen = nInviteMsgLen;

	memcpy( &pBuffer[nIndex], &l_sUserInvite, sizeof(l_sUserInvite) );
	nIndex += sizeof(l_sUserInvite);


	memcpy( &pBuffer[nIndex], pInviteMsg, l_sUserInvite.nInviteLen );
	nIndex += l_sUserInvite.nInviteLen;

	NGFrameHeader( pBuffer, nIndex, NGSC_USERINVITE, ERROR_NOT );
	
	return nIndex;
}

int NGFrameGameingExiUser( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;
	
	int nRoomNo = d->m_nRoomNo;
	sGamingExitUser l_sGamingExitUser;
	memset( &l_sGamingExitUser, 0x00, sizeof(l_sGamingExitUser) );

	l_sGamingExitUser.nNewBbangJjnag = 0;
	
	if ( d->m_bBbangJjang ) {
		for ( int i = 0; i < MAX_ROOM_IN; i ++ ) {

			if (  g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo].m_User[i] == NULL
				|| g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo].m_User[i] == d
				|| g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo].m_User[i]->m_bClosed ) continue;

			g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo].m_User[i]->m_bBbangJjang = true;
			d->m_bBbangJjang = false;
			l_sGamingExitUser.nNewBbangJjnag = g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			break;
		}
	}

	l_sGamingExitUser.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;

	memcpy( &pBuffer[nIndex], &l_sGamingExitUser, sizeof(l_sGamingExitUser) );
	nIndex += sizeof(sGamingExitUser);

	NGFrameHeader( pBuffer, nIndex, NGSC_GAMINGEXITUSER, ERROR_NOT );

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


int NGFrameGameStart( char *pBuffer )
{
	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_STARTGAME, ERROR_NOT );
	
	return nIndex;
}



int NGFrameGetAvatarUrl( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex] = d->m_MyInfo.l_sUserInfo.cSex;
	nIndex++;

	memcpy( &pBuffer[nIndex], d->m_MyInfo.l_sUserInfo.szPreAvatar, MAX_AVAURLLEN );
	nIndex += MAX_AVAURLLEN;

	memcpy( &pBuffer[nIndex], d->m_MyInfo.l_sUserInfo.szGameGubun, 5 );
	nIndex += 5;

	NGFrameHeader( pBuffer, nIndex, NGSC_GETAVAURL, ERROR_NOT );

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

int NGFrameGameSunSet( char *pBuffer, BYTE *pSunPack )
{
	int nIndex = SC_HEADER_SIZE;

	memcpy( &pBuffer[nIndex], pSunPack, MAX_SUNCARD_CNT );
	nIndex += MAX_SUNCARD_CNT;

	NGFrameHeader( pBuffer, nIndex, NGSC_GAMESUNSET, ERROR_NOT );

	return nIndex;
}

int NGFrameChioceCardNo( char *pBuffer, int nChoiceNo, int nUniqUserNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(LOWORD(nChoiceNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nChoiceNo));

	pBuffer[nIndex++] = HIBYTE(HIWORD(nUniqUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nUniqUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nUniqUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nUniqUserNo));

	NGFrameHeader( pBuffer, nIndex, NGSC_CHOICECARDNO, ERROR_NOT );

	return nIndex;
}

int NGFrameSunDecide( char *pBuffer, int nSunUniqNo )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(HIWORD(nSunUniqNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(nSunUniqNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(nSunUniqNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(nSunUniqNo));

	NGFrameHeader( pBuffer, nIndex, NGSC_SUNDECIDE, ERROR_NOT );

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

int NGFrameReChioceCard( char *pBuffer )
{
	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_RECHOICE, ERROR_NOT );

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

int NGFrameRealStartGame( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;

	int nBbangJjangUniqNo = 0;
	int nRoomMasterUniqNo = 0;

	CRoomList *pTempRoom;
	pTempRoom = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( pTempRoom->m_User[i] != NULL && pTempRoom->m_User[i]->m_bBbangJjang == TRUE ) {
			
			nBbangJjangUniqNo = pTempRoom->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			break;
		}
	}

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( pTempRoom->m_User[i] != NULL && pTempRoom->m_User[i]->m_bRoomMaster == TRUE ) {
			
			nRoomMasterUniqNo = pTempRoom->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			break;
		}
	}
	
	sRealGameStart l_sRealGameStart;
	memset( &l_sRealGameStart, 0x00, sizeof(l_sRealGameStart) );

	l_sRealGameStart.nBbangJjangUniqNo = nBbangJjangUniqNo;
	l_sRealGameStart.nRoomMasterUniqNo = nRoomMasterUniqNo;

	memcpy( &pBuffer[nIndex], &l_sRealGameStart, sizeof(l_sRealGameStart) );
	nIndex += sizeof(l_sRealGameStart);

	NGFrameHeader( pBuffer, nIndex, NGSC_GAMEREALSTARTBTN, ERROR_NOT );
	
	return nIndex;
}

int NGFrameSetVoice( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;

	pBuffer[nIndex++] = HIBYTE(HIWORD(d->m_MyInfo.l_sUserInfo.nUserNo));  	//사용자의 고유 번호를 넘겨준다.
	pBuffer[nIndex++] = LOBYTE(HIWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = LOBYTE(LOWORD(d->m_MyInfo.l_sUserInfo.nUserNo));
	pBuffer[nIndex++] = HIBYTE(LOWORD(d->m_sOption.nVoice));
	pBuffer[nIndex++] = LOBYTE(LOWORD(d->m_sOption.nVoice));

	NGFrameHeader( pBuffer, nIndex, NGSC_SETVOICE, ERROR_NOT );

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
