#include "CmdProcess.h"
#include "Manager.h"
#include "NGFrame.h"

#include "dtrace.h"

extern World g_World;
extern CManager *l_pCManager;
extern CDbQ *g_pDbQ;
extern sJackPotInfo g_sJackPotInfo[JACKPOT_CNT];



void CmdProcess( CUserDescriptor *d, char *pReceBuffer )
{
	int nIndex = 0;

	BYTE bCmd, bErrorCode, bMode;
	sCSHeaderPkt l_sCSHeaderPkt;
	int nSendPacketLen = 0;
	char *pPayLoad;

	nIndex = NGDeFrameHeader( pReceBuffer, &l_sCSHeaderPkt );

	bErrorCode = l_sCSHeaderPkt.cErrorCode;

	bMode = (BYTE)l_sCSHeaderPkt.cMode;
	bCmd = (BYTE)l_sCSHeaderPkt.cCmd;


	if (  ( ( l_sCSHeaderPkt.szHeader[0] != d->m_szPureHeader[0]) ||
		    ( l_sCSHeaderPkt.szHeader[1] != d->m_szPureHeader[1]) ) ||
		    ( bErrorCode != ERROR_NOT ) ) {

		sprintf( a_Log, "(FAIL)Packet Client Cmd : %c실패 UserIp : %s UserId : %s", bCmd, d->m_Host, d->m_MyInfo.szUser_Id );
		LogFile (a_Log);
			
		sprintf( a_Log, "CmdProcess :: FAIL - %d, %d, %d, %d, %d", l_sCSHeaderPkt.szHeader[0], l_sCSHeaderPkt.szHeader[1],
			l_sCSHeaderPkt.cMode, l_sCSHeaderPkt.cCmd, l_sCSHeaderPkt.cErrorCode );
		LogFile (a_Log);



		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);
		d->m_bClosed = true;
		l_pCManager->CloseSocket(d);

		return;
	}

	//Header를 제외한 곳의 포인터...
	pPayLoad = pReceBuffer + sizeof(sCSHeaderPkt);

	//command
	cmdMode_list[bMode][bCmd].Cmd_pt( d, pPayLoad );

}


void WaitInput( CUserDescriptor *d, char *pReceBuffer )
{
	int nSendPacketLen = 0;

	sPKWaitInputCS l_sPKWaitInputCS;

	memset( &l_sPKWaitInputCS, 0x00, sizeof(l_sPKWaitInputCS) );
	memcpy( &l_sPKWaitInputCS, pReceBuffer, sizeof(l_sPKWaitInputCS) );
	

#ifdef PACKETANALY
	sprintf( a_Log, "(WaitInput_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
	sprintf( a_Log, "(Packet) szNick_Name = %s, nbUserMoney = %I64d, nClass = %d, nbMaxMoney =  %I64d, fWinRate = %f, nWin_Num = %d, nLose_Num = %d, nDraw_Num = %d, nCut_Time = %d, nChannel = %d, nUserNo = %d, cSex = %c, szPreAvatar = %s, szUser_Id = %s", 
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.szNick_Name, 
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.biUserMoney,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nClass,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.biMaxMoney,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.fWinRate,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nWin_Num,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nLose_Num,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nDraw_Num,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nCut_Time,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nChannel,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nUserNo,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.cSex,
							  l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.szPreAvatar,
							  l_sPKWaitInputCS.l_sUserInfo.szUser_Id );
	LogFile (a_Log);
#endif


	memcpy( &d->m_MyInfo, &l_sPKWaitInputCS.l_sUserInfo, sizeof(l_sPKWaitInputCS.l_sUserInfo) );
	memcpy( &d->m_sOption, &l_sPKWaitInputCS.l_sOption, sizeof(l_sPKWaitInputCS.l_sOption) );

	d->m_MyInfo.l_sUserInfo.bInvite = d->m_sOption.bInvite;
	d->m_nUserPage = 0;		//유저의 Page( 첫 Page )
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	if ( !UserCheck( d, 'N' ) )
		return;

	if ( !LimitUserCheck(nUserChannel) ) {			

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_LIMIT_USER );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);
		
		sprintf( a_Log, "(WaitInput_FAIL)\t User초과 UserId : %s \t SendLen = %d\t WaitInput", d->m_MyInfo.szUser_Id, nSendPacketLen );
		LogFile (a_Log);

		return;
	}

	memset( g_ClientSendLargeBuf, 0x00, LARGE_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInput( g_ClientSendLargeBuf, d );
	SEND_TO_Q(g_ClientSendLargeBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	
#ifdef REALTIME_REFRESH	
	UserWaitRefresh( d, d->m_MyInfo.l_sUserInfo.nChannel, 'A', -1 );  
#endif

	USERPOSITION(d) = USERWAIT;  	
	g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].AddWaitChannelUser(d);   

	LogFile ("(WaitInputReP)\n");

	return;
}

void GetUserInfo( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(GetUserInfo_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	sGetUserInfo l_sGetUserInfo;


	memcpy( &l_sGetUserInfo, pReceBuffer, sizeof(l_sGetUserInfo) );
	
	memcpy( d->m_MyInfo.szUser_Id, l_sGetUserInfo.szUser_Id, sizeof(l_sGetUserInfo.szUser_Id) );

	d->m_MyInfo.l_sUserInfo.nChannel = 	l_sGetUserInfo.nUserChannel;

	STATE(d) = GETUSER;
	STATEDBMODE(d) = GETUSER;
	WAITTING_DB(d) = true;
	g_pDbQ->AddToQ(d);

	LogFile ("(GetUserInfo)\n");

	return;	
}




void CreateGame( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(CreateGame_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nRe = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	sCreateRoom l_sCreateRoom;
	int nCreateRoomPage = 0;

	memset( &l_sCreateRoom, 0x00, sizeof(l_sCreateRoom) );
	memcpy( &l_sCreateRoom, pReceBuffer, sizeof(l_sCreateRoom) );

	nRe = g_World.m_Channel[nUserChannel].CreateRoom( d, &l_sCreateRoom );

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) nRoomNo = %d, bSecret = %d, szRoomName = %s, szRoomPass = %s, nTotCnt = %d, ", 
							  nRe,
							  l_sCreateRoom.bSecret, 
							  l_sCreateRoom.szRoomName,
							  l_sCreateRoom.szRoomPass,
							  l_sCreateRoom.nTotCnt );
	LogFile (a_Log);
#endif

	if ( nRe == ERROR_NOCREATEROOM ) {

		#ifdef PACKETANALY
			LogFile ("(CreateGame_FAIL) - ERROR_NOCREATEROOM");
		#endif

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_CREATEROOM, nRe );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);
		return;		

	}

	nCreateRoomPage = nRe / PAGE_ROOMCNT;
	d->m_nUserPage = nCreateRoomPage;

	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameCreateGame( g_ClientSendBuf, nRe, nUserChannel );
	SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	d->m_bPlay = true;			
	USERPOSITION(d) = USERGAME;  	
	g_World.m_Channel[nUserChannel].DeleteWaitChannelUser(d);  


#ifdef REALTIME_REFRESH	
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, nRe, 0, d->m_MyInfo.l_sUserInfo.nUserNo, 'C' );
	BroadCastUserPage( g_ClientSendBuf, nUserChannel, nCreateRoomPage, nSendPacketLen );			//대기실 해당페이지만.

#endif	

	LogFile ("(CreateGameReP)\n");

	return;
}

void RoomOut( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(RoomOut_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif
	

	int nRoomNo = 0;
	int nRoomInUserNo = 0;
	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nCreateRoomPage = 0;

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	nRoomNo = d->m_nRoomNo;
	nRoomInUserNo = d->m_nRoomInUserNo;

#ifdef PACKETANALY
	sprintf( a_Log, "(NoPacket) Data nRoomNo = %d, nRoomInUserNo = %d", nRoomNo, nRoomInUserNo );
	LogFile (a_Log);
#endif
	
	g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].RoomOutUser( nRoomNo, nRoomInUserNo, d );

	d->m_nUserPage = 0;

	memset( g_ClientSendLargeBuf, 0x00, LARGE_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInput( g_ClientSendLargeBuf, d );
	SEND_TO_Q(g_ClientSendLargeBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);


	nCreateRoomPage = nRoomNo / PAGE_ROOMCNT;
	
#ifdef REALTIME_REFRESH	
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, nRoomNo, nRoomInUserNo, d->m_MyInfo.l_sUserInfo.nUserNo, 'R' );
	BroadCastUserPage( g_ClientSendBuf, d->m_MyInfo.l_sUserInfo.nChannel, nCreateRoomPage, nSendPacketLen );			//대기실 해당페이지만.

#endif	


	USERPOSITION(d) = USERWAIT;  	
	g_World.m_Channel[nUserChannel].AddWaitChannelUser(d);   

	LogFile ("(RoomOutReP)\n");

	return;

}



void RoomIn( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(RoomIn_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif
	
	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nRe = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nCreateRoomPage = 0;
	sRoomIn l_sRoomIn;

	memset( &l_sRoomIn, 0x00, sizeof(l_sRoomIn) );
	memcpy( &l_sRoomIn, pReceBuffer, sizeof(l_sRoomIn) );

	CUserDescriptor *pTempD;
	int nRoomNo = l_sRoomIn.nRoomNo;


	if ( !strcmp( g_ServerInfo.szIpCheck, "TRUE") ) {
		BOOL bCheck = FALSE;
		bCheck = UserIpCheck( d, nRoomNo );

		if ( bCheck ==  TRUE ) {
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameIpCheck( g_ClientSendBuf );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput (d);		

			return;
		}
	}

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		pTempD = g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[i];
		if ( pTempD == NULL || pTempD == d  )
			continue;
		
		
		if ( pTempD->m_bClosed == true ) {

			for ( int nTemp = 0; nTemp < MAX_ROOM_IN; nTemp++ ) {
				if ( g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[nTemp] == NULL )
					continue;

				g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[nTemp]->m_bBbangJjang = false;
				g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[nTemp]->m_bRoomMaster = false;

			}
			
			pTempD->m_bBbangJjang = true;


			g_World.m_Channel[nUserChannel].RoomOutUser( pTempD->m_nRoomNo, pTempD->m_nRoomInUserNo, pTempD );

			pTempD->m_nUserPage = 0;

		#ifdef REALTIME_REFRESH	//방에서 CloseSocket을 했음을 대기실 유저게에 뿌린다.
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, pTempD, pTempD->m_nRoomNo, 
				pTempD->m_nRoomInUserNo,  pTempD->m_MyInfo.l_sUserInfo.nUserNo, 'X' );
			BroadCastUserPage( g_ClientSendBuf, nUserChannel, pTempD->m_nUserPage, nSendPacketLen );			//대기실 해당페이지만.
		#endif	

			STATE(pTempD) = USER_UPDATE;
			STATEDBMODE(pTempD) = USER_UPDATE;
			WAITTING_DB(pTempD) = true;
			g_pDbQ->AddToQ(pTempD);				


			sprintf( a_Log, "(RoomIn유령유저 - STATEDBMODE(d) = GAMING_EXITUPDATE)\t UserId : %s \t GameEnd", pTempD->m_MyInfo.szUser_Id );
			LogFile (a_Log);
			
		}

	}

	nRe = g_World.m_Channel[nUserChannel].RoomIn( d, &l_sRoomIn );

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) nRoomNo = %d, szRoomPass = %s, RoomCurCnt = %d",
						l_sRoomIn.nRoomNo,
						l_sRoomIn.szRoomPass,
						g_World.m_Channel[nUserChannel].m_RoomList[l_sRoomIn.nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt  );
	LogFile (a_Log);

	for ( int k = 0; k < MAX_ROOM_IN; k++ ) {

		if ( g_World.m_Channel[nUserChannel].m_RoomList[l_sRoomIn.nRoomNo].m_User[k] == NULL ) {
			LogFile ("RoomIn - 한놈비었다.");
			continue;

		}

		sprintf( a_Log, "(RoomInUser) RoomIn UserId : %s", g_World.m_Channel[nUserChannel].m_RoomList[l_sRoomIn.nRoomNo].m_User[k]->m_MyInfo.szUser_Id );
		LogFile (a_Log);
	}
#endif

	switch(nRe) {
		case ERROR_NOROOM:
		case ERROR_ROOMFULL:
		case ERROR_NOTPWD:

			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ROOMIN, nRe );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput (d);

			#ifdef PACKETANALY
				sprintf( a_Log, "(RoomIn_FAIL) 방에못들어감. nRe = %d", nRe );
				LogFile (a_Log);
			#endif

			return;		
	}


	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameRoomIn( g_ClientSendBuf, d, nUserChannel, l_sRoomIn.nRoomNo );
	SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	d->m_bPlay = true;			
	USERPOSITION(d) = USERGAME;  	//유저의 위치를 게임으로
	g_World.m_Channel[nUserChannel].DeleteWaitChannelUser(d);   //해당 Client채널에서 뺀다.

	nCreateRoomPage = l_sRoomIn.nRoomNo / PAGE_ROOMCNT;

	d->m_nUserPage = nCreateRoomPage;
#ifdef REALTIME_REFRESH	
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, l_sRoomIn.nRoomNo, 0, d->m_MyInfo.l_sUserInfo.nUserNo, 'A' );
	BroadCastUserPage( g_ClientSendBuf, nUserChannel, nCreateRoomPage, nSendPacketLen );			//대기실 해당페이지만.

#endif	

	LogFile ("(RoomInReP)\n");

	return;
}

bool UserIpCheck( CUserDescriptor *d, int nRoomNo )
{
	//return FALSE;
	
	CUserDescriptor *pTempD;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		pTempD = g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[i];
		if ( pTempD == NULL || pTempD == d  )
			continue;

		//C Class로 비교한다.
		if ( !strcmp( pTempD->m_HostCClass, d->m_HostCClass ) )
			return TRUE;
	}

	return FALSE;
}

void GameViewWait( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(GameViewWait_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nViewPageNo = 0;
	int nIndex = 0;

	nViewPageNo = MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) nViewPageNo = %d", nViewPageNo );
	LogFile (a_Log);
#endif


	if ( nViewPageNo < 0 || nViewPageNo >= MAX_GAMEVIEWWAIT ) {	//0부터 9page까지만 본다.

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGCS_VIEWWAIT, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

	
		LogFile ("(GameViewWait_FAIL)이상");

		return;
	}

	
	//방에들어온 사람에게 다른사람들의 정보를 보내준다. ( 자신의 정보도 포함이 된다. )
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameGameViewWait( g_ClientSendBuf, nUserChannel, nViewPageNo );
	SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	LogFile ("(GameViewWait)\n");

	return;		
}

void UserInvite( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(UserInvite_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	int nUserInviteNo = 0;
	int nInviteMsgLen = 0;
	char szInviteMsg[MAX_CHATMSG];
	CUserDescriptor *pInviteUser = NULL;

	memset( szInviteMsg, 0x00, MAX_CHATMSG );	

	int nReadLen = NGDeFrameUserInvite( pReceBuffer, &nUserInviteNo, &nInviteMsgLen, szInviteMsg );

	pInviteUser = FindUser( nUserInviteNo, d->m_MyInfo.l_sUserInfo.nChannel );


#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) nUserInviteNo = %d, nInviteMsgLen = %d, szInviteMsg = %s", 
			nUserInviteNo, 
			nInviteMsgLen,
			szInviteMsg );
	LogFile (a_Log);
#endif

	if ( pInviteUser == NULL ) {
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_NOTOPTION );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);
	}
	else if ( pInviteUser != NULL ) {

		if ( pInviteUser->m_sOption.bInvite == false ) {
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_NOTOPTION );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput (d);
		}
		else {

			//방에들어온 사람에게 다른사람들의 정보를 보내준다. ( 자신의 정보도 포함이 된다. )
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameUserInvite( g_ClientSendBuf, d, nInviteMsgLen, szInviteMsg );
			SEND_TO_Q(g_ClientSendBuf, pInviteUser, nSendPacketLen);
			l_pCManager->SendOutput (pInviteUser);

		}
	}

	LogFile ("(UserInviteReP)\n");

	return;
}

void GetOutQuesRepl( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GetOutQuesRepl_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nRoomNo = d->m_nRoomNo;

	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo];

	if ( l_CRoomList->m_nGetOutRepl == -1 )
		return;

	l_CRoomList->m_nGetOutRepl++;

	if ( l_CRoomList->m_nGetOutCnt <= l_CRoomList->m_nGetOutRepl ) {
	
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			if ( l_CRoomList->m_User[i] == NULL ) continue;

			//이제 더이상 패킷을 받을 필요가 없다.
			l_CRoomList->m_nGetOutRepl = -1;

			if ( l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo == l_CRoomList->m_nOutUserNo ) {

				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameGetOut( g_ClientSendBuf, d, l_CRoomList->m_nOutUserNo );
				BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );


				break;
			}
		}
	}

	LogFile ("(GetOutQuesReplReP)\n");

	return;	
}

void GetOut( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GetOut_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nRoomNo = d->m_nRoomNo;

	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo];

	int nIndex = 0;
	int nOutUserNo = MAKELONG( MAKEWORD( pReceBuffer[nIndex + 3], pReceBuffer[nIndex + 2] ) ,
				 MAKEWORD( pReceBuffer[nIndex + 1], pReceBuffer[nIndex + 0] ) );

	if ( l_CRoomList->l_sRoomList.l_sRoomInfo.nCurCnt == 2 ) {
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

			if ( l_CRoomList->m_User[i] == NULL ) continue;

			if ( l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo == nOutUserNo ) {

				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameGetOut( g_ClientSendBuf, d, nOutUserNo );
				BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );

				break;
			}
		}
	}
	else {

		l_CRoomList->m_nOutUserNo = nOutUserNo;

		int nGameJoinCnt = 0;

		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			if ( l_CRoomList->m_User[i] == NULL ) continue; 

			if ( l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo != nOutUserNo 
				&& l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.nUserNo != d->m_MyInfo.l_sUserInfo.nUserNo ) {

				nGameJoinCnt++;

				l_CRoomList->m_nGetOutRepl = 0;

				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameGetOutQues( g_ClientSendBuf, l_CRoomList->m_User[i], nOutUserNo );
				SEND_TO_Q(g_ClientSendBuf, l_CRoomList->m_User[i], nSendPacketLen);
				l_pCManager->SendOutput (l_CRoomList->m_User[i]);
			}
		}

		nGameJoinCnt += 2;

		l_CRoomList->m_nGetOutRepl++;
		l_CRoomList->m_nGetOutCnt = (float)nGameJoinCnt * 0.5 + 0.5;
	}

	

	LogFile ("(GetOutReP)\n");

	return;	
}

void QuickJoin( CUserDescriptor *d, char *pReceBuffer ) //방에 들어간다.
{

#ifdef PACKETANALY
	sprintf( a_Log, "(QuickJoin_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	TRACE("d->m_MyInfo.l_sUserInfo.biUserMoney %d\n",d->m_MyInfo.l_sUserInfo.biUserMoney);

	int nRoomNo = g_World.m_Channel[nUserChannel].QuickJoinRoomNo( d->m_MyInfo.l_sUserInfo.biUserMoney );

#ifdef PACKETANALY
	sprintf( a_Log, "(NoPacket) nRoomNo = %d, Data nUserChannel = %d", nRoomNo, nUserChannel );
	LogFile (a_Log);
#endif

	if ( nRoomNo < 0 ) {

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_QUICKJOIN, ERROR_NOQUICK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		#ifdef PACKETANALY
			LogFile ("(QuickJoin_FAIL) QuickJoin할방없음" );
		#endif

		return;			
	}

	sRoomIn l_sRoomIn;	

	memset( &l_sRoomIn, 0x00, sizeof(l_sRoomIn) );
	l_sRoomIn.nRoomNo = nRoomNo;

	RoomIn( d, (char *)&l_sRoomIn );

	LogFile ("(QuickJoinRep)\n");
}

void WaitChat( CUserDescriptor *d, char *pReceBuffer )// 대기실에서 채팅을 한다
{
#ifdef PACKETANALY
	sprintf( a_Log, "(WaitChat_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	int nUserNo = 0;
	int nChatMsgLen = 0;
	char szChatMsg[MAX_CHATMSG];
	CUserDescriptor *pEarTalkUser = NULL;
	char szNickName[MAX_NICK_LENGTH];

	memset( szNickName, 0x00, sizeof(MAX_NICK_LENGTH) );
	memset( szChatMsg, 0x00, MAX_CHATMSG );	

	int nReadLen = NGDeFrmaeChat( pReceBuffer, szNickName, &nUserNo, &nChatMsgLen, szChatMsg );

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) UserId = %s, szNickName = %s, nUserNo = %d, nChatMsgLen = %d, szChatMsg = %s",
						d->m_MyInfo.szUser_Id, szNickName, nUserNo, nChatMsgLen, szChatMsg );
	LogFile (a_Log);
#endif

	if ( nUserNo == 0 ) {
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameWaitChat( g_ClientSendBuf, 0, szNickName, nChatMsgLen, szChatMsg );
		BroadCastChannel( g_ClientSendBuf, d->m_MyInfo.l_sUserInfo.nChannel, nSendPacketLen, false, d );
	}
	else {
	
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER );

		pEarTalkUser = FindUser( nUserNo, d->m_MyInfo.l_sUserInfo.nChannel );

		if ( pEarTalkUser == NULL ) {
			
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameWaitChat( g_ClientSendBuf, nUserNo, szNickName, nChatMsgLen, szChatMsg );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput(d);

		}
		else {
			
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameWaitChat( g_ClientSendBuf, nUserNo, szNickName, nChatMsgLen, szChatMsg );
			SEND_TO_Q(g_ClientSendBuf, pEarTalkUser, nSendPacketLen);
			l_pCManager->SendOutput(pEarTalkUser);
		}
	}

	LogFile ("(WaitChatRep)\n");

	return;
}

void GameChat( CUserDescriptor *d, char *pReceBuffer )// 대기실에서 채팅을 한다
{
#ifdef PACKETANALY
	sprintf( a_Log, "(GameChat_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	int nUserNo = 0;
	int nChatMsgLen = 0;
	char szChatMsg[MAX_CHATMSG];
	CUserDescriptor *pEarTalkUser = NULL;
	char szNickName[MAX_NICK_LENGTH];

	memset( szNickName, 0x00, sizeof(MAX_NICK_LENGTH) );
	memset( szChatMsg, 0x00, MAX_CHATMSG );	

	int nReadLen = NGDeFrmaeChat( pReceBuffer, szNickName, &nUserNo, &nChatMsgLen, szChatMsg );

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) UserId = %s, szNickName = %s, nUserNo = %d, nChatMsgLen = %d, szChatMsg = %s",
						d->m_MyInfo.szUser_Id, szNickName, nUserNo, nChatMsgLen, szChatMsg );
	LogFile (a_Log);
#endif


	if ( nUserNo == 0 ) {
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameGameChat( g_ClientSendBuf, 0, szNickName, nChatMsgLen, szChatMsg );
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );
	}
	else {
	
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER );

		pEarTalkUser = FindUser( nUserNo, d->m_MyInfo.l_sUserInfo.nChannel );

		if ( pEarTalkUser == NULL ) {
			
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameGameChat( g_ClientSendBuf, nUserNo, szNickName, nChatMsgLen, szChatMsg );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput(d);

		}
		else {
			
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameGameChat( g_ClientSendBuf, nUserNo, szNickName, nChatMsgLen, szChatMsg );
			SEND_TO_Q(g_ClientSendBuf, pEarTalkUser, nSendPacketLen);
			l_pCManager->SendOutput(pEarTalkUser);
		}
	}

	LogFile ("(GameChatRep)\n");

	return;
}

void GameStart( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GameStart_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nIndex = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nUserRoomNo = d->m_nRoomNo;
	int nRoomMasterUniqNo = 0;
	int nBbangJjangUniqNo = 0;
	CUserDescriptor *pTempD;


	for ( int i = 0; i < MAX_ROOM_IN; i ++ ) {

		if ( g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i] == NULL ) continue;
		
		g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i]->m_nGameJoin = 1;
	}

	int nRe = g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_CPokerEngine.GameVerify( nUserChannel, nUserRoomNo );

	if ( nRe < 0 )
		return;

	g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_CPokerEngine.CreateGame( 52 );
	g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_CPokerEngine.StartDeal( nUserChannel, nUserRoomNo );


	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i] != NULL  )
		{
			if ( g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i]->m_bRoomMaster ) {
				nRoomMasterUniqNo = g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			}

			if ( g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i]->m_bBbangJjang ) {
				nBbangJjangUniqNo = g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.nUserNo;
			}
		}
	}


	int nPrizeNo = 0;
	int nPrizeDecide = GetRandom( 0, 4 );


	g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_nPrizeDecide = nPrizeDecide;

	if ( nPrizeDecide == 0 ) {
		nPrizeNo = GetRandom( 0, MAX_PIRZECNT - 1 );

		g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_nPrizeNo = nPrizeNo;
	}

	g_pDbQ->GetJackPotMoney();

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i] != NULL  )
		{
			pTempD = g_World.m_Channel[nUserChannel].m_RoomList[nUserRoomNo].m_User[i];

			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameStartGame( g_ClientSendBuf, pTempD, nRoomMasterUniqNo, nBbangJjangUniqNo, nPrizeDecide, nPrizeNo );
			SEND_TO_Q(g_ClientSendBuf, pTempD, nSendPacketLen);
			l_pCManager->SendOutput (pTempD);
		}
	}


	LogFile ("(GameStartRep)\n");

	return;
}


void ViewPage( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(ViewPage_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nUserReqPage = 0;
	int nIndex = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	nUserReqPage = 	MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) nUserReqPage = %d", nUserReqPage );
	LogFile (a_Log);
#endif

	if ( nUserReqPage < 0 ) { //|| nUserReqPage >= MAX_PAGE_CNT ) {
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_VIEWPAGE, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		LogFile ("(ViewPage_FAIL)이상");

		return;
	}

	//방에들어온 사람에게 다른사람들의 정보를 보내준다. ( 자신의 정보도 포함이 된다. )
	memset( g_ClientSendLargeBuf, 0x00, LARGE_SEND_BUFFER);
	nSendPacketLen = NGFrameViewPage( g_ClientSendLargeBuf, nUserChannel, nUserReqPage );
	SEND_TO_Q(g_ClientSendLargeBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	//현재 유저가 보고있는 페이지를 바꾼다.
	d->m_nUserPage = nUserReqPage;

	LogFile ("(ViewPageRep)\n");

	return;
}

void CardDump( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(CardDump_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nCardDumpSlot = 0;
	BYTE nCardNo = 0;
	int nIndex = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	nCardDumpSlot = 	MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

	//선택된 유저의 카드를 3번째로 놓는다.
	d->UserCardDump( nCardDumpSlot );

	//방안에 있는 (자신제외) 사람에게 선택카드를 날려준다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameCardDump( g_ClientSendBuf, d->m_MyInfo.l_sUserInfo.nUserNo );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );


	//오픈 유저를 위해 유저 카드를 한번씩 더 날려준다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameInitUserCard( g_ClientSendBuf, d );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );

	LogFile ("(CardDumpRep)\n");

	return;
}



void CardChoice( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(CardChoice_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nCardChoiceSlot = 0;
	BYTE nCardNo = 0;
	int nIndex = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	nCardChoiceSlot = 	MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

	//선택된 유저의 카드를 3번째로 놓는다.
	d->UserCardChoice( nCardChoiceSlot );
	d->m_bChoice = TRUE;
	nCardNo = d->m_nUserCard[2];

	//방안에 있는 (자신제외) 사람에게 선택카드를 날려준다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameCardChoice( g_ClientSendBuf, d->m_MyInfo.l_sUserInfo.nUserNo, nCardNo );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );


	//오픈 유저를 위해 유저 카드를 한번씩 더 날려준다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameInitUserCard( g_ClientSendBuf, d );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );




	CRoomList *l_CRoomList;
	int nChannel, nRoomNo;
	BOOL bAllChoice = TRUE;

	nChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	nRoomNo = d->m_nRoomNo;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];
	
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( l_CRoomList->m_User[i] == NULL ) continue;

		//참가자인데 아직 초이스가 FALSE이면 카드를 날리면 안된다.
		//0 : 대기자, 1 : 참가자
		if ( l_CRoomList->m_User[i]->m_nGameJoin == 1 ) {
			if ( l_CRoomList->m_User[i]->m_bChoice == FALSE ) {
				bAllChoice = FALSE;
				break;
			}
		}
	}

	//모두가 초이스를 했으면 선부터 카드를 나누어준다.
	if ( bAllChoice ) {

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameChoiceCardView( g_ClientSendBuf, d, nChannel, nRoomNo );
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );


		int nRe = l_CRoomList->m_CPokerEngine.GameVerify( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

		//다 죽었을 경우. 처음 부터 다시 시작한다.
		if ( nRe == -1 ) {
			//나가리 작업.
			EndGameProc( d, '8', 2 );

		}
		else if ( nRe == -2 ) {	//한명만 남았을 경우.
			//게임종료.
			for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			
				if ( l_CRoomList->m_User[i] == NULL ) continue;

				if ( l_CRoomList->m_User[i]->m_sGameCurUser.bDie == FALSE )
					EndGameProc( l_CRoomList->m_User[i], '8', 1 );	//한명남은 사람. 유저 셀렉트는 표시안한다.
			}
		}
		else {
			int nRe = l_CRoomList->m_CPokerEngine.ResetMaster( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );
			
			if ( nRe < 0 ) {
				//나가리 작업.
				EndGameProc( d, '8', 2 );
				return;
			}

			int nBeforeRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );

			l_CRoomList->m_CPokerEngine.UserCardDeal( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );
			nRe = l_CRoomList->m_CPokerEngine.ResetMaster( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

			if ( nRe < 0 ) {
				//나가리 작업.
				EndGameProc( d, '8', 2 );
				return;
			}
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameGetOneCard( g_ClientSendBuf, d, nChannel, nRoomNo, nBeforeRoomMasterRoomInUserNo );
			BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );

			MyUserJokBoSend( nChannel, nRoomNo );
		}
	}

	LogFile ("(CardChoiceRep)\n");

	return;
}

void UserSelect( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(UserSelect_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	int nIndex = 0;
	BigInt biUserBettingMoney = 0;
	char cUserSelect = pReceBuffer[nIndex++];
	CRoomList *l_CRoomList;
	int nChannel, nRoomNo;
	int nSendPacketLen = 0;


	nChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	nRoomNo = d->m_nRoomNo;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	//유저가 select한것에 대하여.금액을 계산하고 다음유저가 어떤 액션을 취할수 있는지 보낸다.
	// '0':따당, '1':체크, '2':쿼터, '3':삥, '4':하프, '5':콜, '6':풀, '7':다이
	d->m_sGameCurUser.bBing = FALSE;
	d->m_sGameCurUser.bCall = FALSE;
	d->m_sGameCurUser.bCheck = FALSE;
	d->m_sGameCurUser.bDdaDang = FALSE;
	d->m_sGameCurUser.bDie = FALSE;
	d->m_sGameCurUser.bFull = FALSE;
	d->m_sGameCurUser.bHalf = FALSE;
	d->m_sGameCurUser.bQuater = FALSE;


	switch( cUserSelect ) {
		case '0':
			biUserBettingMoney = d->m_sGameCurUser.biDdaDang;
			d->m_sGameCurUser.bDdaDang = TRUE;

			break;

		case '1':
			biUserBettingMoney = d->m_sGameCurUser.biCheck;
			d->m_sGameCurUser.bCheck = TRUE;

			break;

		case '2':
			biUserBettingMoney = d->m_sGameCurUser.biQuater;
			d->m_sGameCurUser.bQuater = TRUE;
			break;
			
		case '3':
			biUserBettingMoney = d->m_sGameCurUser.biBing;
			d->m_sGameCurUser.bBing = TRUE;
			break;

		case '4':
			biUserBettingMoney = d->m_sGameCurUser.biHalf;
			d->m_sGameCurUser.bHalf = TRUE;
			break;

		case '5':
			biUserBettingMoney = d->m_sGameCurUser.biCall;
			d->m_sGameCurUser.bCall = TRUE;
			break;

		case '6':
			biUserBettingMoney = d->m_sGameCurUser.biFull;
			d->m_sGameCurUser.bFull = TRUE;
			break;

		case '7':
			biUserBettingMoney = d->m_sGameCurUser.biDie;
			d->m_sGameCurUser.bDie = TRUE;
			break;
	}		
	

	if ( d->m_sGameCurUser.bCheck )
		d->m_bCheck = TRUE;

	if ( biUserBettingMoney > d->m_MyInfo.l_sUserInfo.biUserMoney ) {

		if ( d->m_biSideMoney > 0 ) {
			d->m_biSideMoney += biUserBettingMoney;
		}
		else {	
			d->m_biSideMoney = biUserBettingMoney - d->m_MyInfo.l_sUserInfo.biUserMoney;
			d->m_MyInfo.l_sUserInfo.biUserMoney = 0;
		}

		DTRACE("(방:%d) %s유저 사이드머니로 적립한다. 사이드머니 : %I64d, 원래콜머니 : %I64d, 사이드머니뺀후콜머니 : %I64d", 
			d->m_nRoomNo,
			d->m_MyInfo.l_sUserInfo.szNick_Name, 
			d->m_biSideMoney,
			biUserBettingMoney, 
			d->m_MyInfo.l_sUserInfo.biUserMoney );

	}
	else {
	
		d->m_MyInfo.l_sUserInfo.biUserMoney -= biUserBettingMoney;
	}
		
	if ( biUserBettingMoney < d->m_biCallMoney ) {
		d->m_biRoundMoney += d->m_biCallMoney;
	}
	else {
		d->m_biRoundMoney += biUserBettingMoney;
	
		l_CRoomList->m_biBeforeMoney += biUserBettingMoney - d->m_biCallMoney;
	}
	
	DTRACE("(방:%d) %s유저  받아야할 머니 : %I64u 레이스 %I64u",
		d->m_nRoomNo,
		d->m_MyInfo.l_sUserInfo.szNick_Name, 
		d->m_biCallMoney,
		( biUserBettingMoney - d->m_biCallMoney > 0 ) ? biUserBettingMoney - d->m_biCallMoney : 0 );


	l_CRoomList->m_biRoomCurMoney += biUserBettingMoney;


	d->m_sUserDBUpdateInfo.m_biRaceMoney += biUserBettingMoney;

	DTRACE("(방:%d) %s유저가 %I64u베팅했다. 유저 남은 돈 : %I64u, 총베팅한 금액 : %I64u",
		d->m_nRoomNo,
		d->m_MyInfo.l_sUserInfo.szNick_Name, 
		biUserBettingMoney, 
		d->m_MyInfo.l_sUserInfo.biUserMoney,
		d->m_sUserDBUpdateInfo.m_biRaceMoney );

	d->m_sUserDBUpdateInfo.biRealRaceMoney = d->m_sUserDBUpdateInfo.m_biRaceMoney - d->m_biSideMoney;
	d->m_biTempRealRaceMoney = d->m_sUserDBUpdateInfo.biRealRaceMoney;

	DTRACE("(방:%d) %s유저가 실제 베팅한 금액 %I64u = 총레이스 머니 %I64u - 사이드머니 %I64u" ,
		d->m_nRoomNo,
		d->m_MyInfo.l_sUserInfo.szNick_Name, d->m_sUserDBUpdateInfo.biRealRaceMoney, d->m_sUserDBUpdateInfo.m_biRaceMoney, d->m_biSideMoney );

	if ( d->m_sGameCurUser.bDie ) {

		if ( d->m_bRoomMaster ) {
			int nNewSun = 0;
			int nTempRoomInUserNo = d->m_nRoomInUserNo;

			for ( int i = 1; i < MAX_ROOM_IN; i ++ ) {

				nNewSun = ( nTempRoomInUserNo + i ) % MAX_ROOM_IN;

				if ( l_CRoomList->m_User[nNewSun] == NULL || 
					 l_CRoomList->m_User[nNewSun]->m_nGameJoin == 0 
					) continue;

				l_CRoomList->m_User[nNewSun]->m_bRoomMaster = TRUE;
				d->m_bRoomMaster = false;

				break;
			}
		}

		DTRACE("(방:%d) %s가 다이했다.  베팅머니 : %I64u, 유저 남은 머니 : %I64u",
			d->m_nRoomNo,
			d->m_MyInfo.l_sUserInfo.szNick_Name, 
			d->m_sUserDBUpdateInfo.m_biRaceMoney, 
			d->m_MyInfo.l_sUserInfo.biUserMoney );

		d->m_nGameJoin = 0;
	}

	int nNextSlotNo = g_World.FindNextUserSltNo( nChannel, nRoomNo, d->m_nRoomInUserNo );

	if ( nNextSlotNo < 0 ) {
		EndGameProc( d, '8', 2 );
		return;
	}

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( l_CRoomList->m_User[i] == NULL ) continue;

		l_CRoomList->m_User[i]->m_bMyOrder = FALSE;
	}

	l_CRoomList->m_User[nNextSlotNo]->m_bMyOrder = TRUE;

	if ( nNextSlotNo < 0 ) {

		EndGameProc( d, cUserSelect, 1 );

	}
	else {

		int nRe = l_CRoomList->m_CPokerEngine.GameVerify( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

		if ( nRe == -1 ) {
			EndGameProc( d, '8', 2 );
		}
		else if ( nRe == -2 ) {	
			EndGameProc( d, cUserSelect, 1 );
		}
		else {

			BOOL bRe = l_CRoomList->m_CPokerEngine.PokerBtnSet( nChannel, nRoomNo, nNextSlotNo );
			
			if ( bRe ) { 
				l_CRoomList->m_CPokerEngine.PokerCalcuRaceMoney( nChannel, nRoomNo, nNextSlotNo );

				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameSetGameCurUser( g_ClientSendBuf, d, cUserSelect, nNextSlotNo );
				BroadCastRoomInUser( g_ClientSendBuf, d, nChannel, nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );
			}
			else {

		
				if ( l_CRoomList->m_nRound <= 6 ) {

					int nBeforeRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );

					l_CRoomList->m_CPokerEngine.UserCardDeal( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

					if ( l_CRoomList->m_nRound <= 6  ) {
						int nRe = l_CRoomList->m_CPokerEngine.ResetMaster( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

						if ( nRe < 0 ) {
							EndGameProc( d, '8', 2 );
							return;
						}
					}
									
					memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
					nSendPacketLen = NGFrameGetOneCard( g_ClientSendBuf, d, nChannel, nRoomNo, nBeforeRoomMasterRoomInUserNo, cUserSelect );
					BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );

					MyUserJokBoSend( nChannel, nRoomNo );
				}
				else {
					EndGameProc( d, cUserSelect, 0 );
				}
			}
		}
	}
		
	LogFile ("(UserSelectRep)\n");

	return;	
}


void MemoSend( CUserDescriptor *d, char *pReceBuffer )
{
	
#ifdef PACKETANALY
	sprintf( a_Log, "(MemoSend_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif
	
	int nSendPacketLen = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	int nMemoMsgLen = 0;
	char szMemo[MAX_CHATMSG];
	char szSendNickName[MAX_NICK_LENGTH];
	char szRecvNickName[MAX_NICK_LENGTH];

	memset( szMemo, 0x00, MAX_CHATMSG );	
	memset( szSendNickName, 0x00, MAX_NICK_LENGTH );
	memset( szRecvNickName, 0x00, MAX_NICK_LENGTH );

	int nReadLen = NGDeFrmaeMemo( pReceBuffer, szSendNickName, szRecvNickName, &nMemoMsgLen, szMemo );
	

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) UserId : %s, szSendNickName = %s, szRecvNickName = %s, nMemoMsgLen = %d, szMemo = %s", 
						d->m_MyInfo.szUser_Id,
						szSendNickName,
						szRecvNickName,
						nMemoMsgLen,
						szMemo );
	LogFile (a_Log);
#endif	


	CUserDescriptor *pTempd;
	CUserDescriptor *pRecvD;
	bool bIsUser = false;

	for ( pTempd = l_pCManager->m_UserDescriptorList; pTempd; pTempd = pTempd->m_pNext ) {

		if ( !strcmp( pTempd->m_MyInfo.l_sUserInfo.szNick_Name, szRecvNickName ) ) {
			
			if ( pTempd->m_sOption.bMemoRecv == true ) {
				bIsUser = true;
				pRecvD = pTempd;

			}
		}
	}

	if ( bIsUser ) {
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameMemo( g_ClientSendBuf, szSendNickName, nMemoMsgLen, szMemo );
		SEND_TO_Q(g_ClientSendBuf, pRecvD, nSendPacketLen);
		l_pCManager->SendOutput(pRecvD);

	}
	else {
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_NOTOPTION );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);
	}
	

	LogFile ("(MemoSendReP)\n");

	return;
}

void UserChange( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(UserChange_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;

	sUserChange l_sUserChange;

	memset( &l_sUserChange, 0x00, sizeof(l_sUserChange) );
	memcpy( &l_sUserChange, pReceBuffer, sizeof(l_sUserChange) );


	//유저에게 바꿀 카드가 있는지 조사한다.
	BOOL bRe = FALSE;
	int nUserSlot = 0;

	for ( int i = 0; i < MAX_USER_CARD; i++ )
	{
		if ( d->m_nUserCard[i] == l_sUserChange.bUerCardNo )
		{
			nUserSlot = i;
			bRe = TRUE;
			break;
		}
	}

	if ( bRe == FALSE )
	{
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_NOCARD );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		LogFile ("(UserChangeRep - ERROR)\n");
		return;

	}

	//유저가 원하는 카드가 카드팩에 있는지 찾는다.
	bRe = FALSE;

	CRoomList *l_CRoomList;
	int nCardPackSlot = 0;

	l_CRoomList = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];


	for ( i = l_CRoomList->m_CPokerEngine.m_pCardPack->m_nReadNo; i < l_CRoomList->m_CPokerEngine.m_pCardPack->m_nCardCnt; i++ )
	{
		if ( l_CRoomList->m_CPokerEngine.m_pCardPack->m_pCardPack[i] == l_sUserChange.bChangeCardNo )
		{
			nCardPackSlot = i;
			bRe = TRUE;
			break;
		}
	}
	
	if ( bRe == FALSE )
	{
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_NOCARDPACK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		LogFile ("(UserChangeRep - ERROR)\n");
		return;

	}

	//유저의 카드와 카드팩의 카드를 바꾼다.
	d->m_nUserCard[nUserSlot] = l_sUserChange.bChangeCardNo;
	l_CRoomList->m_CPokerEngine.m_pCardPack->m_pCardPack[nCardPackSlot] = l_sUserChange.bUerCardNo;

	l_sUserChange.nUserUnqiNo = d->m_MyInfo.l_sUserInfo.nUserNo;


	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameUserChange( g_ClientSendBuf, &l_sUserChange );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );

	MyUserJokBoSend( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

	LogFile ("(UserChangeRep)\n");

	return;

}

void MyUserJokBoSend( int nChannel, int nRoomNo )
{
	CRoomList *l_CRoomList;
	int nSendPacketLen = 0;
	CUserDescriptor *d;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameMyUserJokBo( g_ClientSendBuf, l_CRoomList->m_User[i] );
		SEND_TO_Q(g_ClientSendBuf, l_CRoomList->m_User[i], nSendPacketLen);
		l_pCManager->SendOutput (l_CRoomList->m_User[i]);	
	}

	//2006.2.24
	BOOL bRe = FALSE;
	for ( i = 0; i < MAX_ROOM_IN; i++ )
	{
		if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;

		if ( bRe == FALSE )
		{
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameAllUserJokBo( g_ClientSendBuf, nChannel, nRoomNo );
			bRe = TRUE;
		}

		SEND_TO_Q(g_ClientSendBuf, l_CRoomList->m_User[i], nSendPacketLen);
		l_pCManager->SendOutput (l_CRoomList->m_User[i]);	
	}

}

BigInt JackPotCalcu( CRoomList *l_CRoomList, int nMasterSlotNo )
{
	BigInt biUserJackPotMoney = 0;
	int nJackPotPerCent = 0;
	float fJackPotPerCent = 0.2;
	
/*
	if ( l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot == 1 )
		nJackPotPerCent = JACKPOT_STIFUL;
	else if ( l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot == 2 )
		nJackPotPerCent = JACKPOT_ROTIFUL;
	else if ( l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot == 3 )
		fJackPotPerCent = 0.2;
		nJackPotPerCent = JACKPOT_FOUR_CARD;	
*/
	if ( l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot == 1 )
		fJackPotPerCent = 4.0;
	else if ( l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot == 2 )
		fJackPotPerCent = 2.0;
	else if ( l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot == 3 )
		fJackPotPerCent = 0.2;



	for ( int k = 0; k < JACKPOT_CNT; k++ ) {
		TRACE("%d , %d    \n",g_sJackPotInfo[k].biJackPotRoomMoney,l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney);


	//	if ( g_sJackPotInfo[k].biJackPotRoomMoney == l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney ) {
			biUserJackPotMoney = (float)g_sJackPotInfo[k].biJackPotMoney * ((float)fJackPotPerCent / 100.0);

			l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney += biUserJackPotMoney;

			l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.biUserMoney += biUserJackPotMoney;
			l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.nJackPot = l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot;
			l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.biUserJackPotMoney = biUserJackPotMoney;
			g_sJackPotInfo[k].biJackPotMoney -= biUserJackPotMoney;

			DTRACE("(방:%d) %s유저가 잭팟을 했다. 잭팟머니 : %I64u, 유저총딴머니 : %I64u",
				l_CRoomList->m_User[nMasterSlotNo]->m_nRoomNo,
				l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.szNick_Name,
				biUserJackPotMoney, 
				l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney );

			

			break;
	//	}
	}

	return biUserJackPotMoney;
}

void EndGameProc( CUserDescriptor *d, char cUserSelect, int nEndDivi )
{
	int nSendPacketLen = 0;
	int nChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nRoomNo = d->m_nRoomNo;
	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	int nBeforeRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );

	int nRe = l_CRoomList->m_CPokerEngine.ResetMaster( d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo );

	if ( nRe < 0 ) {
		nEndDivi = 2;
	}

	int nMasterSlotNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );

	if ( nEndDivi == 0 || nEndDivi == 1 ) {

		BigInt biCallBackTotalMoney = 0; //타 유저에게 돌려줘야 할머니 총합.
		BigInt biCallBackMoney = 0; //타 유저에게 돌려줘야 할머니

			for ( int nSideTemp = 0; nSideTemp < MAX_ROOM_IN; nSideTemp++ ) {

				//카드가 있는 사람은 게임에 무조건 참여한 사람이다.
				if ( l_CRoomList->m_User[nSideTemp] == NULL ||
					 l_CRoomList->m_User[nSideTemp]->m_nCardCnt <= 0 ) continue;

				DTRACE("%s유저카드:%d,%d,%d,%d,%d,%d", 
					l_CRoomList->m_User[nSideTemp]->m_MyInfo.l_sUserInfo.szNick_Name,
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[0], 
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[1], 
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[2],
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[3], 
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[4], 
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[5],
					l_CRoomList->m_User[nSideTemp]->m_nUserCard[6]
					);

				if ( nMasterSlotNo != nSideTemp ) {

					int nOtherRaceRealMoney = l_CRoomList->m_User[nSideTemp]->m_sUserDBUpdateInfo.m_biRaceMoney
						- l_CRoomList->m_User[nSideTemp]->m_biSideMoney;

					int nMyRaceRealMoney = l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney
						- l_CRoomList->m_User[nMasterSlotNo]->m_biSideMoney;

				
					if ( nOtherRaceRealMoney > nMyRaceRealMoney ) {

						biCallBackMoney = nOtherRaceRealMoney - nMyRaceRealMoney;

						biCallBackTotalMoney+= biCallBackMoney;
						l_CRoomList->m_User[nSideTemp]->m_MyInfo.l_sUserInfo.biUserMoney += biCallBackMoney;
						l_CRoomList->m_User[nSideTemp]->m_sUserDBUpdateInfo.m_biRaceMoney -= 
							( biCallBackMoney + l_CRoomList->m_User[nSideTemp]->m_biSideMoney );

						l_CRoomList->m_User[nSideTemp]->m_sUserDBUpdateInfo.biRealRaceMoney = l_CRoomList->m_User[nSideTemp]->m_sUserDBUpdateInfo.m_biRaceMoney;
						l_CRoomList->m_User[nSideTemp]->m_biTempRealRaceMoney = l_CRoomList->m_User[nSideTemp]->m_sUserDBUpdateInfo.biRealRaceMoney;

						DTRACE("(방:%d) %s유저가 올인을해서 %s유저에게 돌려준다! 돌려줄머니 : %I64u, 돌려준유저결과 - 총돈 : %I64u, 총배팅돈 : %I64u",
							l_CRoomList->m_User[nMasterSlotNo]->m_nRoomNo,
							l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.szNick_Name,
							l_CRoomList->m_User[nSideTemp]->m_MyInfo.l_sUserInfo.szNick_Name,
							biCallBackMoney, 
							l_CRoomList->m_User[nSideTemp]->m_MyInfo.l_sUserInfo.biUserMoney,
							l_CRoomList->m_User[nSideTemp]->m_sUserDBUpdateInfo.m_biRaceMoney );
					}

					biCallBackTotalMoney+= l_CRoomList->m_User[nSideTemp]->m_biSideMoney;
				}
			}

		biCallBackTotalMoney += l_CRoomList->m_User[nMasterSlotNo]->m_biSideMoney;


		BigInt biMyBetTotMoney = l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney;

		l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney = 
			l_CRoomList->m_biRoomCurMoney - biCallBackTotalMoney - 
			( l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney - l_CRoomList->m_User[nMasterSlotNo]->m_biSideMoney );
/*
		BigInt biSystemMoney = (float)(l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney + biMyBetTotMoney )
								* ((float) ((float)SYSTEM_MONEY_RATE/100.0));
*/
		float Rate_Sys = g_pDbQ->g_ChonSystemMoneyRate;
		BigInt biSystemMoney = (float)(l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney + biMyBetTotMoney )
								* ((float) ((float)Rate_Sys/100.0));


		TRACE("AAAAAAA(방:%d) 승리자(%s)의 머니 계산 - 판머니(승리) : %I64u, 유저가 딴순머니 : %I64u,유저가건 순 베팅머니 : %I64u \n",
			l_CRoomList->m_User[nMasterSlotNo]->m_nRoomNo,
			l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.szNick_Name,
			l_CRoomList->m_biRoomCurMoney,
			l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney, 
			biMyBetTotMoney );

		TRACE("BBBBBB(방:%d) 딴순머니 : %I64u, SYSTEM MONEY : %I64u \n",
			l_CRoomList->m_User[nMasterSlotNo]->m_nRoomNo,
			l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney, biSystemMoney );

		l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney -= biSystemMoney;

		l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.biRealRaceMoney = l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney;
		l_CRoomList->m_User[nMasterSlotNo]->m_biTempRealRaceMoney = l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.biRealRaceMoney;

		l_CRoomList->m_User[nMasterSlotNo]->m_MyInfo.l_sUserInfo.biUserMoney += 
			l_CRoomList->m_biRoomCurMoney - biCallBackTotalMoney - biSystemMoney;


		float Rate_JackPot = g_pDbQ->g_ChonSystemJackPotRate;
		BigInt biJackPotMoney = (float)biSystemMoney * ((float) ((float)Rate_JackPot/100.0));
		
		TRACE("(방:%d) 시스머니뺀딴순머니 : %I64u, RoomMoney : %I64u, 잭팟머니 : %I64u", 
			l_CRoomList->m_User[nMasterSlotNo]->m_nRoomNo,
			l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.m_biRaceMoney, 
			l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney, biJackPotMoney );

		for ( int i = 0; i < JACKPOT_CNT; i++ ) {
			if ( g_sJackPotInfo[i].biJackPotRoomMoney == l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney ) {
				g_sJackPotInfo[i].biJackPotMoney += biJackPotMoney;

				DTRACE("(방:%d) 현재 잭팟 적립 머니 : RoomMoney : %I64u, JackPotMoney : %I64u" , 
					l_CRoomList->m_User[nMasterSlotNo]->m_nRoomNo, 
					g_sJackPotInfo[i].biJackPotRoomMoney, g_sJackPotInfo[i].biJackPotMoney);
				break;
			}
		}


		g_pDbQ->m_CJackPotQ.AddToQ( l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney, biJackPotMoney );

		l_CRoomList->m_User[nMasterSlotNo]->m_sUserDBUpdateInfo.biSystemMoney = biSystemMoney;
	}

	if ( nEndDivi == 0 ) {//정상엔딩

		BigInt biUserJackPotMoney = 0;
    //    l_CRoomList->m_User[0]->m_CPokerEvalution.m_nJackPot = 3;  //Chon JackPot Test


		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nCardCnt <= 0 ) continue;

			if ( i == nMasterSlotNo ) {

				if ( l_CRoomList->m_User[i]->m_CPokerEvalution.m_nJackPot == 1 ||
					 l_CRoomList->m_User[i]->m_CPokerEvalution.m_nJackPot == 2 ||
					 l_CRoomList->m_User[i]->m_CPokerEvalution.m_nJackPot == 3  )

					biUserJackPotMoney = JackPotCalcu( l_CRoomList, i );

				if ( l_CRoomList->m_nPrizeDecide == 0 )	//이벤트 발생했으면 승리자가 상품획득이므로
					l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.nSucceedCode = 1;
				else
					l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.nSucceedCode = 0;


				l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.nPrizeNo = l_CRoomList->m_nPrizeNo;
				l_CRoomList->m_User[i]->DBUpdate( 1, 0 ); // 승리, 정상
			}
			else 
				l_CRoomList->m_User[i]->DBUpdate( 0, 0 ); // 패배, 정상.

			DTRACE( "(방:%d) Update %s유저 머니 : %I64u", 
				l_CRoomList->m_User[i]->m_nRoomNo,
				l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.szNick_Name,
				l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.biUserMoney );
		}

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);

		nSendPacketLen = NGFrameEndGame( g_ClientSendBuf, nChannel, nRoomNo, nMasterSlotNo, nBeforeRoomMasterRoomInUserNo, 
			d, cUserSelect, l_CRoomList->m_User[nMasterSlotNo]->m_CPokerEvalution.m_nJackPot, biUserJackPotMoney );


	}
	else if ( nEndDivi == 1 ) {	
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nCardCnt <= 0 ) continue;

			if ( i == nMasterSlotNo ) {

				if ( l_CRoomList->m_nPrizeDecide == 0 )
					l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.nSucceedCode = 1;
				else
					l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.nSucceedCode = 0;


				l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.nPrizeNo = l_CRoomList->m_nPrizeNo;

				l_CRoomList->m_User[i]->DBUpdate( 1, 0 ); 
			}
			else 
				l_CRoomList->m_User[i]->DBUpdate( 0, 0 ); 

			DTRACE( "(방:%d) Update %s유저 머니 : %I64u", 
				l_CRoomList->m_User[i]->m_nRoomNo,
				l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.szNick_Name,
				l_CRoomList->m_User[i]->m_MyInfo.l_sUserInfo.biUserMoney );
		}

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameMyWinGame( g_ClientSendBuf, nChannel, nRoomNo, nMasterSlotNo, d, cUserSelect );

	}
	else {	
		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

			if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;

			l_CRoomList->m_User[i]->DBUpdate( 0, 0 );
		}

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameNagariGame( g_ClientSendBuf );
		
	}

	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );

	l_CRoomList->l_sRoomList.l_sRoomInfo.nGameDivi = 3;

}

void GameReStart( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GameReStart_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nRoomNo = d->m_nRoomNo;
	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	if ( l_CRoomList->l_sRoomList.l_sRoomInfo.nGameDivi != 3 )
		return;

	if (  l_CRoomList->l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_GAMESTART_MEM ) {

		int nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameStartBtnOnOff( g_ClientSendBuf, l_CRoomList->m_User[nRoomMasterRoomInUserNo], '1' );
		SEND_TO_Q(g_ClientSendBuf, l_CRoomList->m_User[nRoomMasterRoomInUserNo], nSendPacketLen);
		l_pCManager->SendOutput (l_CRoomList->m_User[nRoomMasterRoomInUserNo]);


		l_CRoomList->l_sRoomList.l_sRoomInfo.nGameDivi = 1;
	}
	else {	
		l_CRoomList->l_sRoomList.l_sRoomInfo.nGameDivi = 0;
	}

	for ( int i = 0; i < MAX_ROOM_IN; i ++ ) { 

		if ( l_CRoomList->m_User[i] == NULL ) continue;
		
		l_CRoomList->m_User[i]->m_nGameJoin = 0;
	}

	LogFile ("(GameReStartRep)\n");

	return;
}

void KeepAlive( CUserDescriptor *d, char *pReceBuffer )
{
	sprintf( a_Log, "(KeepAliveReQ)\t UserId : %s \t0\tKeepAlive요청이 왔다", d->m_MyInfo.szUser_Id );
	LogFile (a_Log);

	d->m_IdleTics = 0;
}

void SetClientOption( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(SetClientOption_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	int nSendPacketLen = 0;

	memcpy( &d->m_sOption, pReceBuffer, sizeof(sOption) );

	d->m_MyInfo.l_sUserInfo.bInvite = d->m_sOption.bInvite;

	LogFile ("(SetClientOptionReP)");

	return;

}

void GetRoomInfo( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GetRoomInfo_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif 

	//UserMode Check
	if ( !UserCheck( d, 'W' ) )
		return;

	int nRoomNo = 0;
	int nIndex = 0;
	int nSendPacketLen = 0;

	nRoomNo = MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameGetRoomInfo( g_ClientSendBuf, d, nRoomNo );
	SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	LogFile ("(GetRoomInfo)\n");

	return;	

}

void AllCmt( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(AllCmt_REQ)" );
	LogFile (a_Log);
#endif

	l_pCManager->m_ShutdownRemain = 1;
	memset( g_ClientAllSendBuf, 0x00, NORMAL_SEND_BUFFER);

	g_ClientAllSendLen = NGFrameClientAllCmt( g_ClientAllSendBuf, pReceBuffer, d );

	LogFile ("(AllCmtReP)\n");

	return;

}

void UserWaitRefresh( CUserDescriptor *d, int nUserChannel, char cUserAddMius, int nDelUserNo, bool bViewUserPageEx )
{
	int nSendPacketLen = 0;

	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER );

	if ( cUserAddMius == 'A' ) {
		nSendPacketLen = NGFrameUserAdd( g_ClientSendBuf, d );
		BroadCastChannel( g_ClientSendBuf, nUserChannel, nSendPacketLen, bViewUserPageEx, d );			
	}
	else {
		nSendPacketLen = NGFrameUserDel( g_ClientSendBuf, nDelUserNo );
		BroadCastChannel( g_ClientSendBuf, nUserChannel, nSendPacketLen, bViewUserPageEx, d );			
	}
}

bool UserCheck( CUserDescriptor *d, char cWorR )  
{
	int nSendPacketLen = 0;

	if ( 0 > d->m_MyInfo.l_sUserInfo.nChannel || d->m_MyInfo.l_sUserInfo.nChannel >= MAX_CHANNEL ) {

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		sprintf( a_Log, "(UserCheck)UserId : %s UserChannel 이상", d->m_MyInfo.szUser_Id );
		LogFile (a_Log);	
		
		sprintf( a_Log, "UserCheck :: FAIL - d->m_MyInfo.l_sUserInfo.nChannel = %d", d->m_MyInfo.l_sUserInfo.nChannel );
		LogFile (a_Log);	

		return false;
	}


	if ( cWorR == 'N' ) {
		if ( USERPOSITION(d) != USERNONE ) {


			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_PK );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput (d);

			sprintf( a_Log, "(UserCheck)UserId : %s USERNONE 이상", d->m_MyInfo.szUser_Id );
			LogFile (a_Log);

			return false;
		}
	}else if ( cWorR == 'W' ) {
		if (( USERPOSITION(d) != USERWAIT )|| 
			( 0 <= d->m_nRoomNo && d->m_nRoomNo < MAX_ROOMCNT ) ||
			( 0 <= d->m_nRoomInUserNo && d->m_nRoomInUserNo < MAX_ROOM_IN )) {
				
				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_PK );
				SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
				l_pCManager->SendOutput (d);

				sprintf( a_Log, "(UserCheck)UserId : %s USERWAIT 이상", d->m_MyInfo.szUser_Id );
				LogFile (a_Log);

				return false;		
		}
	}
	else {

		if (( USERPOSITION(d) != USERGAME )|| 
			( 0 > d->m_nRoomNo && d->m_nRoomNo >= MAX_ROOMCNT ) ||
			( 0 > d->m_nRoomInUserNo && d->m_nRoomInUserNo >= MAX_ROOM_IN )) {
				
				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_PK );
				SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
				l_pCManager->SendOutput (d);

				sprintf( a_Log, "(UserCheck)UserId : %s USERGAME 이상", d->m_MyInfo.szUser_Id );
				LogFile (a_Log);

				return false;		
		}

	}

	return true;
}

bool LimitUserCheck( int nChannel )
{
	
	if ( g_World.m_Channel[nChannel].m_nCurChannelCnt >=  ( MAX_CHANNELPER ) ) {
		return false;
	}
	return true;
}

void BroadCastChannel( char *pSendBuff, int nChannel, int nSendPacketLen, bool bViewUserPageEx, CUserDescriptor *d )
{
	CChannelUserList *pUserList, *pNextUserList, *pTemp;

	if ( bViewUserPageEx == false ) {

		pTemp = g_World.m_Channel[nChannel].m_pWaitChannelUserList;
		for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {	//유저정보를 담는다.

			SEND_TO_Q(pSendBuff, pUserList->m_User , nSendPacketLen);
			l_pCManager->SendOutput (pUserList->m_User);		
		}	
	}

	else {

		pTemp = g_World.m_Channel[nChannel].m_pWaitChannelUserList;
		for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {	//유저정보를 담는다.

		if ( d->m_nUserPage == pUserList->m_User->m_nUserPage ) continue;  //같은 Page가 아니면 뿌리지 않는다.

			SEND_TO_Q(pSendBuff, pUserList->m_User , nSendPacketLen);
			l_pCManager->SendOutput (pUserList->m_User);		
		}

	}

}

void BroadCastUserPage( char *pSendBuff, int nChannel, int nUserPage, int nSendPacketLen )
{
	CChannelUserList *pUserList, *pNextUserList, *pTemp;

	pTemp = g_World.m_Channel[nChannel].m_pWaitChannelUserList;
	for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {	//유저정보를 담는다.

		SEND_TO_Q(pSendBuff, pUserList->m_User , nSendPacketLen);
		l_pCManager->SendOutput (pUserList->m_User);		
	}	
}

void BroadCastRoomInUser( char *pReceBuffer, CUserDescriptor *d, int nChannel, int nRoomNo , int nRoomInUserNo, int nSendLength, bool bSelfEx ) //방안에모두
{
	int nCurCnt = 0;
	nCurCnt = g_World.m_Channel[nChannel].m_RoomList[nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt;

	if ( bSelfEx == false ) {

		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

			if ( nCurCnt <= 0 ) break;

			if ( g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] != NULL  )
			{
				SEND_TO_Q( pReceBuffer, g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i], nSendLength );
				l_pCManager->SendOutput(g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i]);
		
				nCurCnt--;
			}
		}
	}
	else {

		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

			if ( nCurCnt <= 0 ) break;

			if ( g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] != NULL )
			{
				
				if ( g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] == d ) continue;					

				SEND_TO_Q( pReceBuffer, g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i], nSendLength );
				l_pCManager->SendOutput(g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i]);

			}

			if ( g_World.m_Channel[nChannel].m_RoomList[nRoomNo].m_User[i] != NULL )
				nCurCnt--;

		}
	}
}

CUserDescriptor *FindUser( int nUserNo, int nChannel )
{
	CChannelUserList *pUserList, *pNextUserList, *pTemp;

	pTemp = g_World.m_Channel[nChannel].m_pWaitChannelUserList;
	for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {
		if ( pUserList->m_User->m_MyInfo.l_sUserInfo.nUserNo == nUserNo ) return pUserList->m_User;
	}	

	return NULL;
}

