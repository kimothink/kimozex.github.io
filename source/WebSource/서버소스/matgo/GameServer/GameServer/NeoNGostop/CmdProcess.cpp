#include "CmdProcess.h"
#include "Manager.h"
#include "NGFrame.h"
#include "dtrace.h"

extern World g_World;
extern World g_AdminWorld;
extern CUserDescriptor *g_AdminUser;

extern CManager *l_pCManager;
extern CDbQ *g_pDbQ;

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
	sprintf( a_Log, "(Packet) szNick_Name = %s, biUserMoney = %I64u, nClass = %d, biMaxMoney = %I64u, fWinRate = %f, nWin_Num = %d, nLose_Num = %d, nDraw_Num = %d, nCut_Time = %d, nChannel = %d, nUserNo = %d, cSex = %c, szPreAvatar = %s, szUser_Id = %s", 
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

	//UserMode Check
	if ( !UserCheck( d, 'N' ) )
		return;

	if ( !LimitUserCheck(nUserChannel) ) {			//채널당 유저를 체크한다.

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

	
#ifdef REALTIME_REFRESH	//유저가 들어왔음을 다른유저에게 실시간 보내준다.
	UserWaitRefresh( d, d->m_MyInfo.l_sUserInfo.nChannel, 'A', -1 );  //유저추가 
#endif

	USERPOSITION(d) = USERWAIT;  	//유저의 위치를 대기실로.
	g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].AddWaitChannelUser(d);   //해당 Client채널에 넣는다.

	LogFile ("(WaitInputReP)\n");

	return;
}

void CreateGame( CUserDescriptor *d, char *pReceBuffer )
{
	//UserMode Check
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
	int nIndex = 0;

	memset( &l_sCreateRoom, 0x00, sizeof(l_sCreateRoom) );
	memcpy( &l_sCreateRoom, pReceBuffer, sizeof(l_sCreateRoom) );

	nIndex += sizeof(l_sCreateRoom);
	int nReqRoomNo = 	MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );	

	nRe = g_World.m_Channel[nUserChannel].CreateRoom( d, &l_sCreateRoom, nReqRoomNo );

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
	USERPOSITION(d) = USERGAME;  	//유저의 위치를 게임으로
	g_World.m_Channel[nUserChannel].DeleteWaitChannelUser(d);   //해당 Client채널에서 뺀다.

#ifdef REALTIME_REFRESH	//방을 만들었음을 실시간 뿌린다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, nRe, 0, d->m_MyInfo.l_sUserInfo.nUserNo, 'C' );
	BroadCastUserPage( g_ClientSendBuf, nUserChannel, nCreateRoomPage, nSendPacketLen );			//대기실 해당페이지만.


#endif	

	LogFile ("(CreateGameReP)\n");

	return;
}

void ExitGame( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(ExitGame_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif
	//UserMode Check
	if ( !UserCheck( d, 'W' ) )
		return;

	int nDelUserno = d->m_MyInfo.l_sUserInfo.nUserNo;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

#ifdef PACKETANALY
	sprintf( a_Log, "(NoPacket) Data nDelUserno = %d, nUserChannel = %d", nDelUserno, nUserChannel );
	LogFile (a_Log);
#endif


	d->m_bClosed = true;				// 유저소켓을 닫으면 된다.
	l_pCManager->CloseSocket(d);

	LogFile ("(ExitGame)\n");
	
#ifdef REALTIME_REFRESH	//유저가 나갔왔음을 다른유저에게 실시간 보내준다.
	UserWaitRefresh( d, nUserChannel, 'D', nDelUserno );  //유저추가 
#endif

}

void RoomOut( CUserDescriptor *d, char *pReceBuffer )//방에서 나간다
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
	if ( !UserCheck( d, 'R' ) )
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
	
#ifdef REALTIME_REFRESH	//방에서 CloseSocket을 했음을 대기실 유저게에 뿌린다.
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, nRoomNo, nRoomInUserNo, d->m_MyInfo.l_sUserInfo.nUserNo, 'R' );
	BroadCastUserPage( g_ClientSendBuf, d->m_MyInfo.l_sUserInfo.nChannel, nCreateRoomPage, nSendPacketLen );			//대기실 해당페이지만.


	
#endif	

	USERPOSITION(d) = USERWAIT;  
	g_World.m_Channel[nUserChannel].AddWaitChannelUser(d);   

	LogFile ("(RoomOutReP)\n");

	return;

}

void RoomIn( CUserDescriptor *d, char *pReceBuffer ) //방에 들어간다.
{
#ifdef PACKETANALY
	sprintf( a_Log, "(RoomIn_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif
	
	//UserMode Check
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
			pTempD->m_bRoomMaster = true;

			g_World.m_Channel[nUserChannel].RoomOutUser( pTempD->m_nRoomNo, pTempD->m_nRoomInUserNo, pTempD );

			pTempD->m_nUserPage = 0;

		#ifdef REALTIME_REFRESH	//방에서 CloseSocket을 했음을 대기실 유저게에 뿌린다.
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, pTempD, pTempD->m_nRoomNo, 
				pTempD->m_nRoomInUserNo,  pTempD->m_MyInfo.l_sUserInfo.nUserNo, 'X' );
			BroadCastUserPage( g_ClientSendBuf, nUserChannel, pTempD->m_nUserPage, nSendPacketLen );			//대기실 해당페이지만.
		#endif	

			
			STATEDBMODE(pTempD) = GAMING_EXITUPDATE;
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

	if ( g_World.m_Channel[nUserChannel].m_RoomList[l_sRoomIn.nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_ROOM_IN ) 
	{
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRealStartGame( g_ClientSendBuf, d );
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );
	}

	d->m_bPlay = true;			
	USERPOSITION(d) = USERGAME;  	
	g_World.m_Channel[nUserChannel].DeleteWaitChannelUser(d);   

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




void RoomView( CUserDescriptor *d, char *pReceBuffer ) //방을 관전한다.
{
    g_AdminUser = new CUserDescriptor;
	g_AdminUser = d;

#ifdef PACKETANALY
	sprintf( a_Log, "(RoomIn_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif
	
	//UserMode Check
	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nRe = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	int nCreateRoomPage = 0;
	sRoomIn l_sRoomIn;

	memset( &l_sRoomIn, 0x00, sizeof(l_sRoomIn) );
	memcpy( &l_sRoomIn, pReceBuffer, sizeof(l_sRoomIn) );

/*
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
			pTempD->m_bRoomMaster = true;

			g_World.m_Channel[nUserChannel].RoomOutUser( pTempD->m_nRoomNo, pTempD->m_nRoomInUserNo, pTempD );

			pTempD->m_nUserPage = 0;

		#ifdef REALTIME_REFRESH	//방에서 CloseSocket을 했음을 대기실 유저게에 뿌린다.
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, pTempD, pTempD->m_nRoomNo, 
				pTempD->m_nRoomInUserNo,  pTempD->m_MyInfo.l_sUserInfo.nUserNo, 'X' );
			BroadCastUserPage( g_ClientSendBuf, nUserChannel, pTempD->m_nUserPage, nSendPacketLen );			//대기실 해당페이지만.
		#endif	

			
			STATEDBMODE(pTempD) = GAMING_EXITUPDATE;
			g_pDbQ->AddToQ(pTempD);				

			sprintf( a_Log, "(RoomIn유령유저 - STATEDBMODE(d) = GAMING_EXITUPDATE)\t UserId : %s \t GameEnd", pTempD->m_MyInfo.szUser_Id );
			LogFile (a_Log);
			
		}

	}

	nRe = g_World.m_Channel[nUserChannel].RoomView( d, &l_sRoomIn );

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
*/
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameRoomIn( g_ClientSendBuf, d, nUserChannel, l_sRoomIn.nRoomNo );
	SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);
/*
	if ( g_World.m_Channel[nUserChannel].m_RoomList[l_sRoomIn.nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt >= MAX_ROOM_IN ) 
	{
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameRealStartGame( g_ClientSendBuf, d );
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );
	}

	d->m_bPlay = true;			
	USERPOSITION(d) = USERGAME;  	
	g_World.m_Channel[nUserChannel].DeleteWaitChannelUser(d);   

	nCreateRoomPage = l_sRoomIn.nRoomNo / PAGE_ROOMCNT;

	d->m_nUserPage = nCreateRoomPage;
#ifdef REALTIME_REFRESH	
	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, d, l_sRoomIn.nRoomNo, 0, d->m_MyInfo.l_sUserInfo.nUserNo, 'A' );
	BroadCastUserPage( g_ClientSendBuf, nUserChannel, nCreateRoomPage, nSendPacketLen );			//대기실 해당페이지만.

#endif	

	LogFile ("(RoomInReP)\n");
*/


	return;
}












bool UserIpCheck( CUserDescriptor *d, int nRoomNo )
{
	CUserDescriptor *pTempD;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		pTempD = g_World.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[i];
		if ( pTempD == NULL || pTempD == d  )
			continue;

		if ( !strcmp( pTempD->m_HostCClass, d->m_HostCClass ) )
			return TRUE;
	}

	return FALSE;
}

bool UserIpCheckAdmin( CUserDescriptor *d, int nRoomNo )
{
	CUserDescriptor *pTempD;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		pTempD = g_AdminWorld.m_Channel[nUserChannel].m_RoomList[nRoomNo].m_User[i];
		if ( pTempD == NULL || pTempD == d  )
			continue;

		if ( !strcmp( pTempD->m_HostCClass, d->m_HostCClass ) )
			return TRUE;
	}

	return FALSE;
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

bool DataCheck( int nData, int nFrom, int nTo )
{
	if ( nFrom <= nData && nData < nTo )
		return true;

	return false;

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

	if ( nUserReqPage < 0 ) { 
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_VIEWPAGE, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		LogFile ("(ViewPage_FAIL)이상");

		return;
	}

	memset( g_ClientSendLargeBuf, 0x00, LARGE_SEND_BUFFER);
	nSendPacketLen = NGFrameViewPage( g_ClientSendLargeBuf, nUserChannel, nUserReqPage );
	SEND_TO_Q(g_ClientSendLargeBuf, d, nSendPacketLen);
	l_pCManager->SendOutput (d);

	//현재 유저가 보고있는 페이지를 바꾼다.
	d->m_nUserPage = nUserReqPage;

	LogFile ("(ViewPageRep)\n");

	return;
}

void KeepAlive( CUserDescriptor *d, char *pReceBuffer )
{
	sprintf( a_Log, "(KeepAliveReQ)\t UserId : %s \t0\tKeepAlive요청이 왔다", d->m_MyInfo.szUser_Id );
	LogFile (a_Log);

	d->m_IdleTics = 0;
	//d->m_bKeepAliveSend = false;
}

void TurnMsg( CUserDescriptor *d, char *pReceBuffer ) //Client에서 온 Packet를 방안의 모든 클라이언트에게 보내준다
{
	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nIndex = 0;
	int nUserUniqNo = 0;
	int nUserNo = 0;

	char *pSendPacket;

	nSendPacketLen = MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

#ifdef PACKETANALY

	int nLogIndex = 0;

#define CCPAKETLEN		2  //클라이언트에도 정의되어 있어,

	nLogIndex = SC_HEADER_SIZE + CCPAKETLEN;


	switch( pReceBuffer[6] ) {
		case 0x60:

			char szSendNickName[MAX_NICK_LENGTH];
			int nEarTalkUserNo;
			int nChatLen;
			char szChatMsg[MAX_CHATMSG];

			memcpy( szSendNickName, &pReceBuffer[nLogIndex], MAX_NICK_LENGTH );
			nLogIndex += MAX_NICK_LENGTH;
			
			nEarTalkUserNo = MAKELONG( MAKEWORD( pReceBuffer[nLogIndex + 3], pReceBuffer[nLogIndex + 2] ) ,
						 MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] ) );

			nLogIndex += 4;

			nChatLen = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;	

			memcpy( szChatMsg, &pReceBuffer[nLogIndex], nChatLen );
			nLogIndex += nChatLen;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_GAMECHAT) - nRoomNo = %d, UserId : %s, szSendNickName = %s, nEarTalkUserNo = %d, nChatLen = %d, szChatMsg = %s",
				d->m_nRoomNo, d->m_MyInfo.szUser_Id, szSendNickName, nEarTalkUserNo, nChatLen, szChatMsg );
			LogFile (a_Log);

			break;

		case 0x61:
			int nRoomSeed;
			int nCardCnt;
			BYTE g_szRecvCardPack[60];

			nRoomSeed = MAKELONG( MAKEWORD( pReceBuffer[nLogIndex + 3], pReceBuffer[nLogIndex + 2] ) ,
						 MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] ) );
			nLogIndex += 4;

			nCardCnt = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;

			memset( g_szRecvCardPack, 0x00, sizeof(BYTE) * 60 );
			memcpy( g_szRecvCardPack, &pReceBuffer[nLogIndex], sizeof(BYTE) * 60 );
			nLogIndex += nCardCnt;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_STARTGAME) - nRoomNo = %d, szUser_Id : %s, nRoomSeed = %d, nCardCnt = %d, nCardInfo %d,%d,%d nCardNo %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				d->m_nRoomNo, d->m_MyInfo.szUser_Id,
				nRoomSeed, nCardCnt, 
				g_szRecvCardPack[0], 
				g_szRecvCardPack[1],
				g_szRecvCardPack[2],
				g_szRecvCardPack[3],
				g_szRecvCardPack[4],
				g_szRecvCardPack[5],
				g_szRecvCardPack[6],
				g_szRecvCardPack[7],
				g_szRecvCardPack[8],
				g_szRecvCardPack[9],
				g_szRecvCardPack[10],
				g_szRecvCardPack[11],
				g_szRecvCardPack[12],
				g_szRecvCardPack[13],
				g_szRecvCardPack[14],
				g_szRecvCardPack[15],
				g_szRecvCardPack[16],
				g_szRecvCardPack[17],
				g_szRecvCardPack[18],
				g_szRecvCardPack[19],
				g_szRecvCardPack[20],
				g_szRecvCardPack[21],
				g_szRecvCardPack[22],
				g_szRecvCardPack[23],
				g_szRecvCardPack[24],
				g_szRecvCardPack[25],
				g_szRecvCardPack[26],
				g_szRecvCardPack[27],
				g_szRecvCardPack[28],
				g_szRecvCardPack[29],
				g_szRecvCardPack[30],
				g_szRecvCardPack[31],
				g_szRecvCardPack[32],
				g_szRecvCardPack[33],
				g_szRecvCardPack[34],
				g_szRecvCardPack[35],
				g_szRecvCardPack[36],
				g_szRecvCardPack[37],
				g_szRecvCardPack[38],
				g_szRecvCardPack[39],
				g_szRecvCardPack[40],
				g_szRecvCardPack[41],
				g_szRecvCardPack[42],
				g_szRecvCardPack[43],
				g_szRecvCardPack[44],
				g_szRecvCardPack[45],
				g_szRecvCardPack[46],
				g_szRecvCardPack[47],
				g_szRecvCardPack[48],
				g_szRecvCardPack[49],
				g_szRecvCardPack[50],
				g_szRecvCardPack[51],
				g_szRecvCardPack[52],
				g_szRecvCardPack[53] );
			LogFile (a_Log);

			break;

		case 0x62:

			break;

		case 0x63:
			
			nUserUniqNo = MAKELONG( MAKEWORD( pReceBuffer[nLogIndex + 3], pReceBuffer[nLogIndex + 2] ) ,
						 MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] ) );
			nLogIndex += 4;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_RESULTOK) - nRoomNo = %d, szUser_Id : %s, nUserNo = %d ", 
				d->m_nRoomNo, d->m_MyInfo.szUser_Id, nUserUniqNo );
			LogFile (a_Log);


			
			break;

		case 0x64:
			int nShakeCard;
			int g_nRecvShakeCardArr[5];

			nShakeCard = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;
			
			memset( g_nRecvShakeCardArr, -1, sizeof(g_nRecvShakeCardArr) );
			memcpy( g_nRecvShakeCardArr, &pReceBuffer[nLogIndex], sizeof(g_nRecvShakeCardArr) );

			nLogIndex += sizeof(g_nRecvShakeCardArr);

			sprintf( a_Log, "(TurnMsg_REQ_TurnMsg_REQ_NGCC_CARDSHAKE) - nRoomNo = %d, szUser_Id : %s, nShakeCard = %d, g_nRecvShakeCardArr[0] = %d, g_nRecvShakeCardArr[1] = %d, g_nRecvShakeCardArr[2] = %d, g_nRecvShakeCardArr[3] = %d ", 
						d->m_nRoomNo, d->m_MyInfo.szUser_Id, nShakeCard, g_nRecvShakeCardArr[0], g_nRecvShakeCardArr[1], g_nRecvShakeCardArr[2], g_nRecvShakeCardArr[3] );
			LogFile (a_Log);
			break;

		case 0x65:
			int nGoStop;
			nGoStop = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_GOSTOP) - nRoomNo = %d, szUser_Id : %s, nGoStop = %d",
				d->m_nRoomNo, d->m_MyInfo.szUser_Id, nGoStop );
			LogFile (a_Log);
			break;

		case 0x66:
			int nNineCard;
			nNineCard = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_SENDNINECARD) - nRoomNo = %d, szUser_Id : %s, nNineCard = %d", 
				d->m_nRoomNo, d->m_MyInfo.szUser_Id, nNineCard );
			LogFile (a_Log);
			break;

		case 0x67:
			int nUniQNo;
			bool bExit;
				
			nUniQNo = MAKELONG( MAKEWORD( pReceBuffer[nLogIndex + 3], pReceBuffer[nLogIndex + 2] ) ,
						 MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] ) );

			nLogIndex += 4;
			
			memcpy( &bExit, &pReceBuffer[nLogIndex], sizeof(bExit) );
			nLogIndex += sizeof(bExit);

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_USEREXITRESERV) - nRoomNo = %d, szUser_Id : %s, nUniQNo = %d, bExit = %d", 
				d->m_nRoomNo, d->m_MyInfo.szUser_Id, nUniQNo, bExit );
			LogFile (a_Log);
			break;
			
		case 0x68:
			nUserNo = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_ALLYOK) - nRoomNo = %d, szUser_Id : %s, nUserNo = %d", 
				d->m_nRoomNo,  d->m_MyInfo.szUser_Id, nUserNo );
			LogFile (a_Log);
			break;
			
		case 0x69:
			break;

		case 0x6a:
			int nMissionType, nMissionMultiply;

			nMissionType = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;

			nMissionMultiply = MAKEWORD( pReceBuffer[nLogIndex + 1], pReceBuffer[nLogIndex + 0] );
			nLogIndex += 2;

			sprintf( a_Log, "(TurnMsg_REQ_NGCC_MISSIONSET) - nRoomNo = %d, szUser_Id : %s, nMissionType = %d, nMissionMultiply = %d", 
				d->m_nRoomNo, d->m_MyInfo.szUser_Id, nMissionType, nMissionMultiply );
			LogFile (a_Log);
			break;

		case 0x6b:

			break;
			
		default:
			LogFile ("(NGCC_NONOPACKET) 없는 Packet" );
			return;
	}
#endif

	if ( nSendPacketLen <= 0 ) {
		
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_TURNMSG, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		int nTemp = 0;
		LogFile ("(TurnMsg_FAIL)");

		return;
	}

	if ( pReceBuffer[6] == 0x61 ) {		
		g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_nReadyCnt = 1;

		if ( g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt < MAX_ROOM_IN ) {
			g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_nReadyCnt = 0;
			return;
		}
		else
			BroadCastRoomInUser( &pReceBuffer[2], d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );

	}
	else  {
		BroadCastRoomInUser( &pReceBuffer[2], d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );
	}

	return;
}

void GameEnd( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(GameEnd_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	if ( !UserCheck( d, 'G' ) )
		return;

	sGameEnd l_sGameEnd;

	memset( &l_sGameEnd, 0x00, sizeof(l_sGameEnd) );
	memcpy( &l_sGameEnd, pReceBuffer, sizeof(l_sGameEnd) );

#ifdef PACKETANALY
		sprintf( a_Log, "(Packet) biObtainMoney = %I64u, biLoseMoney = %I64u, nWinner = %d, biSystemMoney = %I64u"
					,l_sGameEnd.biObtainMoney
					,l_sGameEnd.biLoseMoney
					,l_sGameEnd.nWinner
					,l_sGameEnd.biSystemMoney );
		LogFile (a_Log);


		sprintf( a_Log, "(유저카드) 광 : %d,%d,%d,%d,%d 십 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d 오 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d 피 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d ",
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[0][0], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[0][1], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[0][2], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[0][3], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[0][4], 

			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][0], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][1], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][2], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][3], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][4], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][5], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][6], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][7], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][8], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][9], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][10], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[1][11], 

			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][0], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][1], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][2], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][3], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][4], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][5], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][6], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][7], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][8], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][9], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][10],
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[2][11],

			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][0], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][1], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][2], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][3], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][4], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][5], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][6], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][7], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][8], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][9], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][10],
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][11],
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][12], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][13], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][14], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][15], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][16], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][17], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][18], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][19], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][20], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][21], 
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][22],
			l_sGameEnd.l_sGameEndHistory.m_nEatCard[3][23] );
		LogFile (a_Log);

		sprintf( a_Log, "(내용)판돈:%I64u, 점수:%d, 흔듦수:%d, 뻑수:%d, 고수:%d, 고도리:%d, 홍단:%d, 청단:%d, 구사:%d, 멍따:%d, 흔듦:%d, 미션:%d, 피박:%d, 광박:%d, 독박:%d, 곱:%d, 미션곱:%d",
			    g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney,
				l_sGameEnd.l_sGameEndHistory.m_nEvalution,
				l_sGameEnd.l_sGameEndHistory.m_nShakeCnt,
				l_sGameEnd.l_sGameEndHistory.m_nBbukCnt,
				l_sGameEnd.l_sGameEndHistory.m_nGoCnt,
				l_sGameEnd.l_sGameEndHistory.m_bGodori,
				l_sGameEnd.l_sGameEndHistory.m_bHongdan,
				l_sGameEnd.l_sGameEndHistory.m_bChongdan,
				l_sGameEnd.l_sGameEndHistory.m_bGusa,	
				l_sGameEnd.l_sGameEndHistory.m_bMmong,
				l_sGameEnd.l_sGameEndHistory.m_bShake,
				l_sGameEnd.l_sGameEndHistory.m_bMission,
				l_sGameEnd.l_sGameEndHistory.m_bPbak,
				l_sGameEnd.l_sGameEndHistory.m_bKwangbak,
				l_sGameEnd.l_sGameEndHistory.m_bDokbak,
				l_sGameEnd.l_sGameEndHistory.m_nMulti,
				l_sGameEnd.l_sGameEndHistory.m_nMissionMultiply);
		LogFile (a_Log);

#endif


	d->m_sUserDBUpdateInfo.biRoomMoney = g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney;
	d->m_sUserDBUpdateInfo.biSystemMoney = l_sGameEnd.biSystemMoney;
	d->m_sUserDBUpdateInfo.biObtainMoney = l_sGameEnd.biObtainMoney;
	d->m_sUserDBUpdateInfo.biLoseMoney = l_sGameEnd.biLoseMoney;
	d->m_sUserDBUpdateInfo.nWinLose = l_sGameEnd.nWinner;
	memcpy( d->m_sUserDBUpdateInfo.szOtherGameUser, l_sGameEnd.szOtherNick_Name, sizeof(l_sGameEnd.szOtherNick_Name) );
	

	if ( l_sGameEnd.nWinner == 1 ) {
		d->m_bRoomMaster = TRUE;
		d->m_MyInfo.l_sUserInfo.nWin_Num++;
	}
	else if ( l_sGameEnd.nWinner == 0 ) {
		d->m_bRoomMaster = FALSE;
		d->m_MyInfo.l_sUserInfo.nLose_Num++;
	}
	else if ( l_sGameEnd.nWinner == 2 ) {
		d->m_MyInfo.l_sUserInfo.nDraw_Num++;
	}
	
	d->m_MyInfo.l_sUserInfo.biUserMoney += l_sGameEnd.biObtainMoney - l_sGameEnd.biLoseMoney;

	int nTotNum = d->m_MyInfo.l_sUserInfo.nWin_Num + 
		d->m_MyInfo.l_sUserInfo.nLose_Num + d->m_MyInfo.l_sUserInfo.nDraw_Num;

	if ( nTotNum > 0 ) 
		d->m_MyInfo.l_sUserInfo.fWinRate = ( (float)d->m_MyInfo.l_sUserInfo.nWin_Num / (float)nTotNum ) * 100;


	d->m_biGameRoomMoney = g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney;
	d->m_biSystemMoney = l_sGameEnd.biSystemMoney;
	memcpy( d->m_szOtherNick_Name, l_sGameEnd.szOtherNick_Name, sizeof(l_sGameEnd.szOtherNick_Name) );

	d->m_nPrizeNo = l_sGameEnd.nPrizeNo;
	d->m_nSucceedCode = l_sGameEnd.nSucceedCode;

	g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.cGameDivi = '0';

	STATEDBMODE(d) = GAME_RESULT_UPDATE;
	g_pDbQ->AddToQ(d);

	LogFile ("(GameEndReP)\n");

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

void GamingExitUser( CUserDescriptor *d, char *pReceBuffer ) 
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GamingExitUser_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;	

	int nSendPacketLen = 0;

	sEndGameOther l_EndGameOther[MAX_ROOM_IN];
	CUserDescriptor *Tempd;
	int nRoomPage = 0;
	int nRoomInUserNo = 0;
	int nUserNo = 0;
	int nRoomNo = 0;

	memset( &l_EndGameOther, 0x00, sizeof(l_EndGameOther) );
	memcpy( &l_EndGameOther, pReceBuffer, sizeof(l_EndGameOther) );
	

	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;


	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( l_EndGameOther[i].szNick_Name[0] == 0x00 ) 
			continue;

#ifdef PACKETANALY
	sprintf( a_Log, "(Packet) OtherUserNick = %s, biObtainMoney = %I64u, biLoseMoney = %I64u, nWinner = %d, biSystemMoney = %I64u"
				,l_EndGameOther[i].szNick_Name
				,l_EndGameOther[i].l_sGameEnd.biObtainMoney
				,l_EndGameOther[i].l_sGameEnd.biLoseMoney
				,l_EndGameOther[i].l_sGameEnd.nWinner
				,l_EndGameOther[i].l_sGameEnd.biSystemMoney );
	LogFile (a_Log);

	
#endif	



		for ( int j = 0; j < MAX_ROOM_IN; j++ ) {

			if ( g_World.m_Channel[nUserChannel].m_RoomList[d->m_nRoomNo].m_User[j] == NULL ) {
				#ifdef PACKETANALY
					LogFile ("(GamingExitUser)이미 소켓이 닫혀" );
				#endif
					
				continue;

			}

			if ( !strcmp( l_EndGameOther[i].szNick_Name, g_World.m_Channel[nUserChannel].m_RoomList[d->m_nRoomNo].m_User[j]->m_MyInfo.l_sUserInfo.szNick_Name ) ) {
				
				#ifdef PACKETANALY
					sprintf( a_Log, "(GamingExitUser) 이놈 szNick_Name = %s", l_EndGameOther[i].szNick_Name );
					LogFile(a_Log);
				#endif				

				Tempd = g_World.m_Channel[nUserChannel].m_RoomList[d->m_nRoomNo].m_User[j];

				nRoomPage = Tempd->m_nUserPage;
				nRoomNo = Tempd->m_nRoomNo;
				nRoomInUserNo = Tempd->m_nRoomInUserNo;
				nUserNo = Tempd->m_MyInfo.l_sUserInfo.nUserNo;

				//////////////////////////////////////////////////////////////////////////
				//DB Update자료
				Tempd->m_sUserDBUpdateInfo.biRoomMoney = g_World.m_Channel[Tempd->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[Tempd->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney;
				Tempd->m_sUserDBUpdateInfo.biSystemMoney =  l_EndGameOther[i].l_sGameEnd.biSystemMoney;
				Tempd->m_sUserDBUpdateInfo.biObtainMoney =  l_EndGameOther[i].l_sGameEnd.biObtainMoney;
				Tempd->m_sUserDBUpdateInfo.biLoseMoney =  l_EndGameOther[i].l_sGameEnd.biLoseMoney;
				Tempd->m_sUserDBUpdateInfo.nWinLose =  l_EndGameOther[i].l_sGameEnd.nWinner;
				memcpy( Tempd->m_sUserDBUpdateInfo.szOtherGameUser,  l_EndGameOther[i].l_sGameEnd.szOtherNick_Name, sizeof( l_EndGameOther[i].l_sGameEnd.szOtherNick_Name) );
				//////////////////////////////////////////////////////////////////////////
				
				//////////////////////////////////////////////////////////////////////////
				//유저 정보갱신
				if (  l_EndGameOther[i].l_sGameEnd.nWinner == 1 ) {
					Tempd->m_bRoomMaster = TRUE;
					Tempd->m_MyInfo.l_sUserInfo.nWin_Num++;
				}
				else if (  l_EndGameOther[i].l_sGameEnd.nWinner == 0 ) {
					Tempd->m_bRoomMaster = FALSE;
					Tempd->m_MyInfo.l_sUserInfo.nLose_Num++;
				}
				else if ( l_EndGameOther[i].l_sGameEnd.nWinner == 2 ) {
					Tempd->m_MyInfo.l_sUserInfo.nDraw_Num++;
				}
					
				Tempd->m_MyInfo.l_sUserInfo.biUserMoney +=  l_EndGameOther[i].l_sGameEnd.biObtainMoney - l_EndGameOther[i].l_sGameEnd.biLoseMoney;
					
				int nTotNum = Tempd->m_MyInfo.l_sUserInfo.nWin_Num + 
					Tempd->m_MyInfo.l_sUserInfo.nLose_Num + Tempd->m_MyInfo.l_sUserInfo.nDraw_Num;

				if ( nTotNum > 0 ) 
					Tempd->m_MyInfo.l_sUserInfo.fWinRate = ( (float)Tempd->m_MyInfo.l_sUserInfo.nWin_Num / (float)nTotNum ) * 100;


				Tempd->m_biGameRoomMoney = g_World.m_Channel[Tempd->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[Tempd->m_nRoomNo].l_sRoomList.l_sRoomInfo.biRoomMoney;
				Tempd->m_biSystemMoney =  l_EndGameOther[i].l_sGameEnd.biSystemMoney;
				Tempd->m_nPrizeNo = l_EndGameOther[i].l_sGameEnd.nPrizeNo;
				Tempd->m_nSucceedCode = l_EndGameOther[i].l_sGameEnd.nSucceedCode;

				memcpy( Tempd->m_szOtherNick_Name, l_EndGameOther[i].l_sGameEnd.szOtherNick_Name, sizeof(l_EndGameOther[i].l_sGameEnd.szOtherNick_Name) );
				//////////////////////////////////////////////////////////////////////////


				g_World.m_Channel[nUserChannel].RoomOutUser( Tempd->m_nRoomNo, Tempd->m_nRoomInUserNo, Tempd );
				Tempd->m_nUserPage = 0;

				//대기실의 그 페이지를 보고 있는 사람들에게 방의 정보가 변경됨을 Send( Client는 방의 인원이 0이면 방을 지우면된다. )
			#ifdef REALTIME_REFRESH	//방에서 CloseSocket을 했음을 대기실 유저게에 뿌린다.
				memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
				nSendPacketLen = NGFrameWaitInfoChange( g_ClientSendBuf, Tempd, nRoomNo, nRoomInUserNo, nUserNo, 'X' );
				BroadCastUserPage( g_ClientSendBuf, nUserChannel, nRoomPage, nSendPacketLen );			//대기실 해당페이지만.
			#endif	

				
				STATEDBMODE(Tempd) = GAMING_EXITUPDATE;
				g_pDbQ->AddToQ(Tempd);				

				sprintf( a_Log, "(STATEDBMODE(d) = GAMING_EXITUPDATE)\t UserId : %s \t GameEnd", Tempd->m_MyInfo.szUser_Id );
				LogFile (a_Log);
			}
		}
	}

	LogFile ("(GamingExitUserReP)\n");

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

void SetClientOption( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(SetClientOption_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	int nSendPacketLen = 0;

	memcpy( &d->m_sOption, pReceBuffer, sizeof(sOption) );

	d->m_MyInfo.l_sUserInfo.bInvite = d->m_sOption.bInvite;

	if ( ( 0 <= d->m_nRoomNo && d->m_nRoomNo < MAX_ROOMCNT ) &&
	 ( 0 <= d->m_nRoomInUserNo && d->m_nRoomInUserNo < MAX_ROOM_IN )) {

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameSetVoice( g_ClientSendBuf, d );
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen, true );
	}

	LogFile ("(SetClientOptionReP)");

	return;
}

void GameReady( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GameReady_REQ) - nRoomNo = %d, UserIp : %s, UserId : %s", 
			d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	int nSendPacketLen = 0;


	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;	

	g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_nReadyCnt++;

#ifdef PACKETANALY
	sprintf( a_Log, "(NoPacket) RoomInUserCnt = %d, Data m_nReadyCnt = %d", 
		g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.nCurCnt,
		g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_nReadyCnt );
	LogFile (a_Log);
#endif

	if ( g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_nReadyCnt >= MAX_ROOM_IN ) {

		sprintf( a_Log, "(GameReady) (D)UserId : %s게임을 시작하라 보낸다.", d->m_MyInfo.szUser_Id );
		LogFile (a_Log);

		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

			if ( g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_User[i] == NULL ) {
				LogFile ("GameReady - 한놈비었다.");
				continue;
			}

			sprintf( a_Log, "(GameReady - RoomInUser) RoomIn UserId : %s UserMoney : %I64u", 
				g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_User[i]->m_MyInfo.szUser_Id,
				g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_User[i]->m_MyInfo.l_sUserInfo.biUserMoney );
			LogFile (a_Log);
		}

	
		g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].l_sRoomList.l_sRoomInfo.cGameDivi = '1';

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameGameStart( g_ClientSendBuf );
		BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );		
		g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo].m_nReadyCnt = 0;
	}

	LogFile ("(GameReadyReP)\n");

	return;

}

void GetAvatarUrl( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GetAvatarUrl_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'W' ) )
		return;

	int nSendPacketLen = 0;
	int nUserNo = 0;
	int nIndex = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;

	nUserNo = 	MAKELONG( MAKEWORD( pReceBuffer[nIndex + 3], pReceBuffer[nIndex + 2] ) ,
						 MAKEWORD( pReceBuffer[nIndex + 1], pReceBuffer[nIndex + 0] ) );



	CUserDescriptor *pFindUser = NULL;

	pFindUser = FindUser( nUserNo, nUserChannel );

	if ( pFindUser != NULL ) {
		//방에들어온 사람에게 다른사람들의 정보를 보내준다. ( 자신의 정보도 포함이 된다. )
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameGetAvatarUrl( g_ClientSendBuf, pFindUser );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

	}

	LogFile ("(GetAvatarUrlReP)\n");

	return;

}

void Shuffle( BYTE *pSunPack )
{
	BYTE tmp;

	for (BYTE i = 0 ; i < MAX_SUNCARD_CNT ; ++i)
	{
		BYTE j = GetRandom(0, (MAX_SUNCARD_CNT - 1));

		tmp = pSunPack[i];
		pSunPack[i] = pSunPack[j];
		pSunPack[j] = tmp;
	}
}

void GameSunSet( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(GameSunSet_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	int nSendPacketLen = 0;
	int nIndex = 0;
	int nUserChannel = d->m_MyInfo.l_sUserInfo.nChannel;
	CRoomList *l_CRoomList = &g_World.m_Channel[nUserChannel].m_RoomList[d->m_nRoomNo];

	BYTE szSunPack[MAX_SUNCARD_CNT];

	memset( szSunPack, 0x00, sizeof(szSunPack) );
	

	szSunPack[0] = 2;
	szSunPack[1] = 5;
	szSunPack[2] = 8;
	szSunPack[3] = 11;
	szSunPack[4] = 14;
	szSunPack[5] = 17;
	szSunPack[6] = 20;
	szSunPack[7] = 23;
	szSunPack[8] = 27;
	szSunPack[9] = 30;

	Shuffle( szSunPack );
	Shuffle( szSunPack );

	//방정보에 카피해놓는다.
	memcpy( l_CRoomList->szSunPack, szSunPack, sizeof(szSunPack) );

	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameGameSunSet( g_ClientSendBuf, szSunPack );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );

	LogFile ("(GameSunSetRep)\n");

	return;	
}

void SunChoice( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(SunChoice_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	//UserMode Check
	if ( !UserCheck( d, 'G' ) )
		return;

	CRoomList *l_CRoomList;
	int nSendPacketLen = 0;
	int nIndex = 0;
	int nSunChoiceNo = MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );

	l_CRoomList = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];

	l_CRoomList->szUserChoiceSunCard[d->m_nRoomInUserNo] = l_CRoomList->szSunPack[nSunChoiceNo];

	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameChioceCardNo( g_ClientSendBuf, nSunChoiceNo, d->m_MyInfo.l_sUserInfo.nUserNo );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );


	int nSunUniqNo = 0;
	if ( l_CRoomList->szUserChoiceSunCard[0] != 0xff && l_CRoomList->szUserChoiceSunCard[1] != 0xff ) 
	{

		if ( l_CRoomList->szUserChoiceSunCard[0] == l_CRoomList->szUserChoiceSunCard[1] ) {
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameReChioceCard( g_ClientSendBuf );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput (d);			
		}
		else {


			if ( l_CRoomList->szUserChoiceSunCard[0] > l_CRoomList->szUserChoiceSunCard[1] ) 
			{
				nSunUniqNo = l_CRoomList->m_User[0]->m_MyInfo.l_sUserInfo.nUserNo;
				DTRACE("선 아이디 : %s", l_CRoomList->m_User[0]->m_MyInfo.l_sUserInfo.szNick_Name);
			}
			else 
			{
				nSunUniqNo = l_CRoomList->m_User[1]->m_MyInfo.l_sUserInfo.nUserNo;
				DTRACE("선 아이디 : %s", l_CRoomList->m_User[1]->m_MyInfo.l_sUserInfo.szNick_Name);
			}

			DTRACE("게임서버시작한다 시작 고유 넘버 = %d", nSunUniqNo );

			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameSunDecide( g_ClientSendBuf, nSunUniqNo );
			BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );
		}
	}

	LogFile ("(SunChoiceRep)\n");

	return;	
}

void AutoChoice( CUserDescriptor *d, char *pReceBuffer )
{
#ifdef PACKETANALY
	sprintf( a_Log, "(SunChoice_REQ) - UserIp : %s UserId : %s", d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif

	if ( !UserCheck( d, 'G' ) )
		return;

	CRoomList *l_CRoomList;
	int nSendPacketLen = 0;
	int nIndex = 0;
	int nSunChoiceNo = 0;



	l_CRoomList = &g_World.m_Channel[d->m_MyInfo.l_sUserInfo.nChannel].m_RoomList[d->m_nRoomNo];

	for ( int i = 0; i < MAX_SUNCARD_CNT; i++ ) {

		if ( d->m_nRoomInUserNo == 0 ) {
			if ( l_CRoomList->szSunPack[i] == l_CRoomList->szUserChoiceSunCard[1] ) continue;
			nSunChoiceNo = i;
			break;
		}
		else {
			if ( l_CRoomList->szSunPack[i] == l_CRoomList->szUserChoiceSunCard[0] ) continue;
			nSunChoiceNo = i;
			break;
		}
	}

	l_CRoomList->szUserChoiceSunCard[d->m_nRoomInUserNo] = l_CRoomList->szSunPack[nSunChoiceNo];

	memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
	nSendPacketLen = NGFrameChioceCardNo( g_ClientSendBuf, nSunChoiceNo, d->m_MyInfo.l_sUserInfo.nUserNo );
	BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );


	int nSunUniqNo = 0;
	if ( l_CRoomList->szUserChoiceSunCard[0] != 0xff && l_CRoomList->szUserChoiceSunCard[1] != 0xff ) 
	{

		if ( l_CRoomList->szUserChoiceSunCard[0] == l_CRoomList->szUserChoiceSunCard[1] ) {
			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameReChioceCard( g_ClientSendBuf );
			SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
			l_pCManager->SendOutput (d);			
		}
		else {


			//크기 비교.
			if ( l_CRoomList->szUserChoiceSunCard[0] > l_CRoomList->szUserChoiceSunCard[1] ) 
			{
				nSunUniqNo = l_CRoomList->m_User[0]->m_MyInfo.l_sUserInfo.nUserNo;
				DTRACE("선 아이디 : %s", l_CRoomList->m_User[0]->m_MyInfo.l_sUserInfo.szNick_Name);
			}
			else 
			{
				nSunUniqNo = l_CRoomList->m_User[1]->m_MyInfo.l_sUserInfo.nUserNo;
				DTRACE("선 아이디 : %s", l_CRoomList->m_User[1]->m_MyInfo.l_sUserInfo.szNick_Name);
			}

			DTRACE("게임서버시작한다 시작 고유 넘버 = %d", nSunUniqNo );

			memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
			nSendPacketLen = NGFrameSunDecide( g_ClientSendBuf, nSunUniqNo );
			BroadCastRoomInUser( g_ClientSendBuf, d, d->m_MyInfo.l_sUserInfo.nChannel, d->m_nRoomNo, d->m_nRoomInUserNo, nSendPacketLen );
		}
	}

	LogFile ("(SunChoiceRep)\n");

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

	if ( cUserAddMius == 'A' ) { //유저추가.
		nSendPacketLen = NGFrameUserAdd( g_ClientSendBuf, d );
		BroadCastChannel( g_ClientSendBuf, nUserChannel, nSendPacketLen, bViewUserPageEx, d );			//대기실 모두에게.
	}
	else {
		nSendPacketLen = NGFrameUserDel( g_ClientSendBuf, nDelUserNo );
		BroadCastChannel( g_ClientSendBuf, nUserChannel, nSendPacketLen, bViewUserPageEx, d );			//대기실 모두에게.
	}
}

void GetRoomInfo( CUserDescriptor *d, char *pReceBuffer )
{

#ifdef PACKETANALY
	sprintf( a_Log, "(GetRoomInfo_REQ) - nRoomNo = %d, UserIp : %s UserId : %s", d->m_nRoomNo, d->m_Host, d->m_MyInfo.szUser_Id );
	LogFile (a_Log);
#endif 

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

bool UserCheck( CUserDescriptor *d, char cWorR )  // true : 맞는 패킷, false : 틀린패킷   cWorR : 대기실(W) 모드  룸 모드(R) 
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

			//빙에 있는 사람모두에게 보낸다.
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
			//자기자신은 제외한다.
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
	if(g_AdminUser) {
		DTRACE( "투명인간성공 User_Id = %s", g_AdminUser->m_MyInfo.szUser_Id );
		
		SEND_TO_Q( pReceBuffer, g_AdminUser, nSendLength );
		l_pCManager->SendOutput(g_AdminUser);
	}


}

void BroadCastGameViewWait( char *pSendBuff, int nChannel, int nSendPacketLen )
{
	CViewWaitUserList *pUserList, *pNextUserList, *pTemp;

	pTemp = g_World.m_Channel[nChannel].m_pViewWaitUserList;

	for ( pUserList = pTemp; pUserList; pUserList = pUserList->m_NextList ) {
		SEND_TO_Q(pSendBuff, pUserList->m_User , nSendPacketLen);
		l_pCManager->SendOutput (pUserList->m_User);		
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

