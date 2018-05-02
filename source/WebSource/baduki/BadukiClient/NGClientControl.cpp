#include "stdafx.h"
#include "NGClientControl.h"
#include "dtrace.h"
#include "NetworkSock.h"
#include "Externvar.h"
#include <process.h>


int UserSort( void );



void GetErrorMessage(BYTE nErrorCode)
{
    g_bConnectingSpr = false;

	switch(nErrorCode) {
		case ERROR_ACCESS_DBFAIL: 
			g_pCMainFrame->m_Message->PutMessage( "데이타베이스 접근을 실패하였습니다.", "Code - 100", true );
			break;

		case ERROR_LIMIT_USER:
			g_pCMainFrame->m_Message->PutMessage( "유저의 수가 너무 많습니다.", "Code - 102" );
			break;

		case ERROR_NOCREATEROOM:
			g_pCMainFrame->m_Message->PutMessage( "더이상 방을 생성할수 없습니다.", "Code - 103" );
			break;

		case ERROR_NOROOM:
			g_pCMainFrame->m_Message->PutMessage( "존재하지 않는 방입니다.", "Code - 104" );
			break;
		
		case ERROR_ROOMFULL:
			g_pCMainFrame->m_Message->PutMessage( "방에 인원이 모두 찼습니다.", "Code - 105" );
			break;
		
		case ERROR_NOTPWD:
			g_pCMainFrame->m_Message->PutMessage( "비밀번호가 틀렸습니다.", "Code - 106" );
			break;

		case ERROR_NOQUICK:
			PostMessage( g_hWnd, WM_OUICKFAILCREATEROOM, 0, 0 );
			break;

		case ERROR_NOTVER:
			g_pCMainFrame->m_Message->PutMessage( "버전이 다릅니다. 지우고 새로설치하세요!!!", "Code - 139", TRUE );
			break;

		case ERROR_MULTILOGIN:
			g_pCMainFrame->m_Message->PutMessage( "이미 사용중인 아이디입니다.!!!", "Code - 140", TRUE );
			break;

		case ERROR_NOTOPTION:
			g_pCMainFrame->m_Message->PutMessage( "수신,초대거부 또는 게임상태입니다.", "Code - 137" );
			break;

        // CHANGEUSERCARD
        //유저가 카드를 바꾸려 했는데 유저에게 없는 카드이다.
        case ERROR_NOCARD:
            g_pCMainFrame->SetGameChatText( "◎ 현재 가지고 있지 않은 카드입니다." , RGB( 255, 255, 255 ));
            break;

        //유저가 바꾸길 원하는 카드였는데 카드팩에 없는 카드이다.
        case ERROR_NOCARDPACK:
            g_pCMainFrame->SetGameChatText( "◎ 카드팩에 바꿀 카드가 없습니다." , RGB( 255, 255, 255 ));
            break;
        ////
	}

}


//DWORD WINAPI ThreadProc(LPVOID lpParameter)
unsigned __stdcall ThreadProc(LPVOID lpParameter)  // ham 2005.11.29
{
	CNetworkSock *pClientSocket = (CNetworkSock *)lpParameter;
	

	char szRecvBuf[MAX_BUFFER_SIZE];
	char *szRealBufpos = NULL;
	char szSendNickName[MAX_NICK_LENGTH];
	char szChatMsg[MAX_CHATMSG];
	char ptemp[255];	
	char cUserAddMius;
	char cWaitInfoDivi;
	char cStartBtnOnOff;
	char szInviteMsg[MAX_CHATMSG];
	char szAllCmt[1000];
	char szMemoMsg[MAX_CHATMSG];

	ush_int nGameDivi;

	int nInput_Q_BufSize;
	int nRead = 0;
	int nIndex = 0;
	int nRoomCnt = 0;
	int nWaitUserCnt = 0;
	int nUserNo = 0;
	int i = 0, j = 0;
	int nNoRoom = 0;
	int nEarTalkUserNo = 0;
	int nChatLen = 0;
	int nDeleteUserNo = 0;
	int nUserArrayNo = 0;
	int nUniqueUserNo = 0;
	int nRoomNo = 0;
	int nSlotNo = 0;
	int nNextNo = 0;
	int nRoomInUserNo = 0;
	int nRoomMasterUniNo = 0;
	int nRoomMasterArrayNo = 0;
	int nBeforeGameUserUniNo = 0;
	int nTempNo = 0;
	int nInviteLen = 0;
	int nCurCnt = 0;
	int nBbangJjangUniqNo = 0;
	int nMemoLen = 0;
	int nInitCardCnt = 0;

	BigInt biRoomMoney = 0;

	BYTE bErrorCode;
	BYTE bCmd;

	sSCHeaderPkt l_sSCHeaderPkt;
	sPKWaitInputCS l_sPKWaitInputCS;
	sBroadServerInfo l_sBroadServerInfo;
	sUserInfo l_sUserInfo;
	sUserInfo l_sAddUserInfo;
	sOnCreateRoom l_sOnCreateRoom;
	sRoomInfoToWaitUserAX l_sRoomInfoToWaitUserAX;
	sRoomInfoToWaitUserR l_sRoomInfoToWaitUserR;
	sRoomInfo *l_spWaitRoomInfo;
	sRoomInUserSend l_sRoomInUserSend[MAX_ROOM_IN];
	sRoomInfo l_sMyRoomInfo;
	sRoomOutOtherSend l_sRoomOutOtherSend;
	sStartUserCard l_sStartUserCard;
	sIsLogIn l_sIsLogIn;
	sUserList l_sUserList;
	sGetOneCard l_sGetOneCard;
	sSetGameCurUser l_sSetGameCurUser;
	sEndGame l_sEndGame;
	sMyWinGame l_sMyWinGame;
	sRoomInAdd l_sRoomInAdd;
	sUserInvite l_sUserInvite;
	sRoomInOtherSend l_sRoomInOtherSend;
	sNotice l_sNotice;
	sChoiceViewCard l_sChoiceViewCard[MAX_ROOM_IN];
	sMyUserJokBo l_sMyUserJokBo;
	sGameWaitInfo l_sGameWaitInfo[MAX_GAMEVIEWWAIT];
    sGameStart l_sGameStart;
    sChangeOkAll l_sChangeOkAll;
    sUserNewCardPacket l_sUserNewCardPacket;
    sAllUserJokBo l_sAllUserJokBo;
    sUserChange l_sUserChange;

	while( g_bPaketThredOn )	
	{
		if( pClientSocket->m_input.GetFromQ(szRecvBuf, &nInput_Q_BufSize) )
		{
			g_ServerSend = true;	//서버에서 뭔가를 받았으니 다시 보낼수 있는상태로 만든다.

			DTRACE("g_ServerSend = true");
			
			nIndex = 0;
			nRead = NGNCSDeframeHeader(szRecvBuf, &l_sSCHeaderPkt);
			
			bCmd = (BYTE)l_sSCHeaderPkt.cCmd;
			bErrorCode = l_sSCHeaderPkt.cErrorCode;
			
			szRealBufpos = szRecvBuf + nRead;
			
			//bErrorCode= ERROR_NOCREATEROOM;
			if ( bErrorCode != ERROR_NOT ) {
				GetErrorMessage(bErrorCode);
				nRead = sizeof(sSCHeaderPkt);
			}
			else {
				switch(bCmd)
				{
					case NGSC_HEADERSEND:
						DTRACE("Packet Recv - NGSC_HEADERSEND");
						sPKFirst l_sPKFirst;
						memcpy( &l_sPKFirst, szRealBufpos, sizeof(l_sPKFirst) );

						g_szHeader[0] = l_sPKFirst.szHeader[0];
						g_szHeader[1] = l_sPKFirst.szHeader[1];
						g_nUserNo = l_sPKFirst.nUserNo;

						nIndex += sizeof(sPKFirst);

						DTRACE("NGSC_HEADERSEND - g_szHeader[0] :%d, g_szHeader[1] : %d", l_sPKFirst.szHeader[0], l_sPKFirst.szHeader[1]  );					

						switch(g_cServerDivi)
						{
							case '0':

								if ( !pClientSocket->LogInServer( g_szUser_ID ) ) {
									g_pCMainFrame->m_Message->PutMessage( "로그인 실패", "Code - 109", true );
								}
								break;

							case '1':

								pClientSocket->CSGetServerList( g_szUser_ID );
								break;

							case '2':

								pClientSocket->GetUserInfo( g_szUser_ID, g_szGameServerChannel );

								break;
						}
						break;	

					case NGSC_GETUSERINFO:
						//자신의 정보를 받는다.
						memcpy( &l_sUserList, &szRealBufpos[nIndex], sizeof(l_sUserList) );
						nIndex += sizeof(l_sUserList);
				
						l_sUserList.l_sUserInfo.nChannel = g_szGameServerChannel;
						//게임내의 유저리스트에 넣는다. g_CGameUserList의 첫번째는 항상 자기자신이 들어간다.
						memcpy( l_sUserList.l_sUserInfo.szPreAvatar, g_szAvaUrl, sizeof(g_szAvaUrl) );
						g_CGameUserList.AddUser( &l_sUserList.l_sUserInfo ); 

						memset( &l_sPKWaitInputCS, 0x00, sizeof(l_sPKWaitInputCS) );
						//로그인서버에서 받은정보가 아니라 게임서버에서 받은 유저넘버를 갱신하기 위해...
						g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.nUserNo = g_nUserNo;
						memcpy( l_sPKWaitInputCS.l_sUserInfo.szUser_Id, g_szUser_ID, MAX_ID_LENGTH );

						
						memcpy( &l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo, &g_CGameUserList.m_GameUserInfo[0].m_sUserInfo, sizeof(g_CGameUserList.m_GameUserInfo[0].m_sUserInfo) );

						l_sPKWaitInputCS.l_sOption.bInvite = g_sClientEnviro.bOption[0];
						l_sPKWaitInputCS.l_sOption.bEarTalk = g_sClientEnviro.bOption[1];
						l_sPKWaitInputCS.l_sOption.bMemoRecv = g_sClientEnviro.bOption[3];


						if(!pClientSocket->WaitInput(&l_sPKWaitInputCS))
						{
							g_pCMainFrame->m_Message->PutMessage( "서버로의 정보전송 실패", "Code - 110" );

						}
						break;
							
	
					case NGSC_LOGIN:
						memset( &l_sIsLogIn, 0x00, sizeof(l_sIsLogIn) );
						memcpy( &l_sIsLogIn, &szRealBufpos[nIndex], sizeof(l_sIsLogIn) );

						nIndex += sizeof(l_sIsLogIn);

						memset( &l_sNotice, 0x00, sizeof(l_sNotice) );
						memcpy( &l_sNotice, &szRealBufpos[nIndex], sizeof(l_sNotice) );
						nIndex += sizeof(l_sNotice);

                        {
                            if ( l_sNotice.m_Notice1[0] != NULL )
                                g_pCMainFrame->m_pScrollNotice = new CScrollText(l_sNotice.m_Notice1, RGB(255, 255, 0), 707, 117, 987, 130);
                        }

						/*
						//유저의 채널을 여기에서 넣어줘야한다.
						l_sLogIn.l_sUserList.l_sUserInfo.nChannel = g_szGameServerChannel;

						//게임내의 유저리스트에 넣는다. g_CGameUserList의 첫번째는 항상 자기자신이 들어간다.
						memcpy( l_sLogIn.l_sUserList.l_sUserInfo.szPreAvatar, g_szAvaUrl, sizeof(g_szAvaUrl) );
						g_CGameUserList.AddUser( &l_sLogIn.l_sUserList.l_sUserInfo, 1 ); 
						*/

						pClientSocket->Destroy();
						g_cServerDivi = '2';
						
						//Counter Server로 접속.
						pClientSocket->InitSocket( g_hWnd, l_sIsLogIn.szGameServerIp, l_sIsLogIn.nGameServerPort );
						g_ServerSend = false;
						break;

					case NGSC_GETSERVERLIST:
						memcpy( g_sServrMashineList, &szRealBufpos[nIndex], sizeof(g_sServrMashineList) );
						nIndex += sizeof(g_sServrMashineList);

						SendMessage( g_hWnd, WM_CHANNELMODESET, 0, 0 );
						
						g_bConnectingSpr = FALSE;

						break;

					case NGCSC_BROADSERVERINFO:
						memset( &l_sBroadServerInfo, 0x00, sizeof(l_sBroadServerInfo) );
						memcpy( &l_sBroadServerInfo, &szRealBufpos[nIndex], sizeof(l_sBroadServerInfo) );

						g_sServrMashineList[l_sBroadServerInfo.nGSNo].nCurCnt[l_sBroadServerInfo.nChannel] = l_sBroadServerInfo.nCurCnt;

						break;	
						

					case NGSC_WAITINPUT:

						//////////////////////////////////////////////////////////////////////////
						/*들어오는 패킷
						nUserNo : 유저고유넘버
						nRoomCnt : 1페이지 방수
						sRoomInfo : 방정보	( 방안의 인물정보는 모른다. )
						nWaitUserCnt : 대기실 사람수
						sUserInfo : 대기실 유저정보
						*/
						//////////////////////////////////////////////////////////////////////////
						
						nUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += sizeof(int);

						nRoomCnt = 	MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						//대기실 유저리스트를 초기화한다.
						memset( &g_CUserList.m_WaitUserInfo , 0x00 , sizeof(g_CUserList.m_WaitUserInfo));

						//대기실 방리스트를 초기화한다.
						g_CWaitRoomList.Init();

						g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.bInvite = g_sClientEnviro.bOption[0];

						//자기자신의 정보를 넣는다.
						g_CUserList.AddUser( &g_CGameUserList.m_GameUserInfo[0].m_sUserInfo );

						//방정보.				
						for ( i = 0; i < nRoomCnt; i++ ) {
							g_CWaitRoomList.AddRoom( (sRoomInfo *)&szRealBufpos[nIndex] );
							nIndex += sizeof(sRoomInfo);
						}

                        // 대기실 인덱스 정렬
                        g_pCMainFrame->WaitRoomIndexRefresh();

						nWaitUserCnt = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						//대기실 사람수
						g_pCMainFrame->m_nWaitUserCurCnt = nWaitUserCnt + 1 ; // + 1 은 유저(자기자신)까지

						memset( &l_sUserInfo, 0x00, sizeof(l_sUserInfo) );

						//대기실 유저를 유저리스트에 넣는다.
						for ( i = 0;  i < nWaitUserCnt; i++ ) {
							memcpy( &l_sUserInfo, &szRealBufpos[nIndex], sizeof(sUserInfo) );
							nIndex += sizeof(sUserInfo);

							if ( g_CUserList.AddUser( &l_sUserInfo ) < 0 ) {
								g_pCMainFrame->m_Message->PutMessage( "유저가 너무 많습니다.", "Code - 112" );
                                
                                _endthreadex( 0 ); // ham 2005.11.29
								return 0;
							}
						}
						
						nIndex += sizeof(sUserInfo) * nWaitUserCnt;

						// 첫번째 유저가 있으면 그 유저를 셀렉트 해놓는다.
                        if ( g_pCMainFrame->m_nUserListTitleNum == 0 ) {
						    if( g_CUserList.m_WaitUserInfo[0].m_bUser == TRUE ) {
							    g_pCMainFrame->m_nViewUserIdx = 0;
						    }
                        }
                        
                        // 스크롤 바의 전체 항목 수 초기화
                        g_pCMainFrame->m_SCB_WaitUser->SetElem( nWaitUserCnt + 1 ); // +1은 유저 자신

						//방에서 나와도 여기가 발생하기 때문에....게임에 들어간부분을 모두 초기화 시킨다.
						SendMessage( g_hWnd, WM_WAITMODESET, 0, 0 );

						//유저의 정보를 지운다.
						g_CGameUserList.UserAllClear();

                        {
                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);

                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_WAITINPUT - CreateEvent() 오류 발생 : 오류코드 %ld", g_szUser_ID, dwErr );
                            }

                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);

                            g_pCMainFrame->m_bGameInit = TRUE;
                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );
		                    
                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] 게임 초기화 실패", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "게임 초기화 실패하였습니다.", "Code - 1000", true );    
		                    }
                        }

						break;

					case NGSC_VIEWPAGE:
						nRoomCnt = 	MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						l_spWaitRoomInfo = new sRoomInfo[nRoomCnt];
						memset( l_spWaitRoomInfo, 0x00, sizeof(sRoomInfo) * nRoomCnt );

						memcpy( l_spWaitRoomInfo, &szRealBufpos[nIndex], sizeof(sRoomInfo) * nRoomCnt );
						nIndex += sizeof(sRoomInfo) * nRoomCnt;

						//대기실 방리스트를 초기화한다.
						g_CWaitRoomList.Init();
						
						for ( i = 0; i < nRoomCnt; i++ ) {
							
							g_CWaitRoomList.AddRoom( &l_spWaitRoomInfo[i] );

						}

						if ( g_nViewPageNo != 0 ) {
							
							if ( nRoomCnt == 0 ) {
								
								//두번연속 방이 없으면 처음으로 넘긴다.
								nNoRoom++;

								if ( nNoRoom >= 2 ) {
									
									pClientSocket->ViewPage(0);
									g_nViewPageNo = 0;
									nNoRoom = 0;
								}
								break;
							}
							nNoRoom = 0;
						}
						break;

					case NGSC_KEEPALIVE:

						DTRACE("Packet Recv - NGSC_KEEPALIVE");

						pClientSocket->KeepAlive();
						break;

					case NGSC_WAITCHAT:
						memset( szSendNickName, 0x00, sizeof(szSendNickName) );
						memcpy( szSendNickName, &szRealBufpos[nIndex], MAX_NICK_LENGTH );
						nIndex += MAX_NICK_LENGTH;						

						nEarTalkUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						nChatLen = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						memcpy( szChatMsg, &szRealBufpos[nIndex], nChatLen );
						nIndex += nChatLen;
						
						memset( ptemp, 0x00, sizeof(ptemp) );
						sprintf( ptemp, "%s님의 말: ", szSendNickName );					

						// 보낸 사람 닉네임과 내 닉네임이 같으면
						strcat( ptemp , szChatMsg );
						if( !strcmp( szSendNickName , g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.szNick_Name ))
						{
                            STextInfo _sTextInfo;
                            memset( &_sTextInfo, 0x00, sizeof(STextInfo) );
                            strcpy( _sTextInfo.szText, ptemp );
                            _sTextInfo.nStyle = 0;
                            _sTextInfo.rgbColor = RGB( 255 , 255 , 255 );
                            g_pCMainFrame->m_pWaitChat->AddText( &_sTextInfo );
                            g_pCMainFrame->m_SCB_WaitChat->SetElem(g_pCMainFrame->m_pWaitChat->GetTextCnt());
                            g_pCMainFrame->m_SCB_WaitChat->EndPage();
						}
						else
						{
                            STextInfo _sTextInfo;
                            memset( &_sTextInfo, 0x00, sizeof(STextInfo) );
                            strcpy( _sTextInfo.szText, ptemp );
                            _sTextInfo.nStyle = 0;
                            _sTextInfo.rgbColor = RGB( 243 , 234 , 162 );
                            g_pCMainFrame->m_pWaitChat->AddText( &_sTextInfo );
                            g_pCMainFrame->m_SCB_WaitChat->SetElem(g_pCMainFrame->m_pWaitChat->GetTextCnt());
                            g_pCMainFrame->m_SCB_WaitChat->EndPage();
						}
						break;	

					case NGSC_USERINFOCHANGE:
						cUserAddMius = szRealBufpos[nIndex++];

						if ( cUserAddMius == 'A' ) {
							memcpy( &l_sAddUserInfo, &szRealBufpos[nIndex], sizeof(sUserInfo) );
							nIndex += sizeof(sUserInfo);

							g_CUserList.AddUser( &l_sAddUserInfo );

							g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
							g_pCMainFrame->DeleteUserForScroll();
						}
						else {
							nDeleteUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
							nIndex += sizeof(int);
							
							if( nDeleteUserNo >= 0 )
							{							
								g_CUserList.DeleteUser( nDeleteUserNo );
								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();
							}
						}
						break;


					case NGSC_WAITINFOCHANGE:
						cWaitInfoDivi = szRealBufpos[nIndex++];
					
						//cCmdDivi   ( 'C' : RoomCreate(Userlist),
						//             'A' : join    'R' : exit, roomdel은 Client에서 처리), 
						//			   'X' : RoomInCloseSoket(방에서접속Close)
						switch(cWaitInfoDivi) {
							case 'C':
								memcpy( &l_sOnCreateRoom, &szRealBufpos[nIndex], sizeof(l_sOnCreateRoom) );
								nIndex += sizeof(l_sOnCreateRoom);
								
								//유저 정보를 삭제한다. 대기실 유저가 방을 만들었을테니.
								nUniqueUserNo = l_sOnCreateRoom.nUserNo; // 1. 유니크 넘버를 받는다.
								g_CUserList.DeleteUser(nUniqueUserNo);
								
								//대기실 룸리스트에 넣는다.
								g_CWaitRoomList.AddRoom( &l_sOnCreateRoom.l_sRoomInfo );
								
								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();
                                g_pCMainFrame->WaitRoomIndexRefresh();

								break;

							case 'A':

								memcpy( &l_sRoomInfoToWaitUserAX, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserAX) );
								nIndex += sizeof(l_sRoomInfoToWaitUserAX);
								
								//유저 정보를 삭제한다. 대기실 유저가 방을 만들었을테니.
								nUniqueUserNo = l_sRoomInfoToWaitUserAX.nUserNo; // 1. 유니크 넘버를 받는다.
								g_CUserList.DeleteUser( nUniqueUserNo );
								
								//대기실 방리스트에 인원을 증가한다.
								g_CWaitRoomList.AddUser( l_sRoomInfoToWaitUserAX.nRoomNo, l_sRoomInfoToWaitUserAX.nRoomCnt );
								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();

                                //방인원이 꽉 찼을때만 리플레쉬 해주면 된다.
                                if ( l_sRoomInfoToWaitUserAX.nRoomCnt >= MAX_ROOM_IN ) {
                                    g_pCMainFrame->WaitRoomIndexRefresh();
                                }

                                // 현재 보고있는 방의 정보 갱신
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserAX.nRoomNo );
								break;

							case 'X':

								memcpy( &l_sRoomInfoToWaitUserAX, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserAX) );
								nIndex += sizeof(l_sRoomInfoToWaitUserAX);

								if( l_sRoomInfoToWaitUserAX.nRoomCnt > 0 )	//방에서 접속이 끊어지고 한명이상이 남아있을때. 해당방의 인원만 감소한다.
								{	
									g_CWaitRoomList.DeleteUser( l_sRoomInfoToWaitUserAX.nRoomNo, l_sRoomInfoToWaitUserAX.nRoomCnt );
								}
								else //게임방에서 유저가 바로 끊어지고 방이 바로 사라질때..  
								{
									g_CWaitRoomList.DeleteRoom( l_sRoomInfoToWaitUserAX.nRoomNo );
                                   
								}

                                g_pCMainFrame->WaitRoomIndexRefresh();

                                // 현재 보고있는 방의 정보 갱신
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserAX.nRoomNo );
								break;

							case 'R':
								memcpy( &l_sRoomInfoToWaitUserR, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserR) );
								nIndex += sizeof(l_sRoomInfoToWaitUserR);

								if ( l_sRoomInfoToWaitUserR.nRoomCnt > 0 ) { //방에 한명이상이 남아있을때.
									//방에서 한명을 감소
									g_CWaitRoomList.DeleteUser( l_sRoomInfoToWaitUserR.nRoomNo, l_sRoomInfoToWaitUserR.nRoomCnt );
								}
								else {
									//방 폭파.
									g_CWaitRoomList.DeleteRoom( l_sRoomInfoToWaitUserR.nRoomNo );

								}

								//대기실에 유저를 넣고.
								g_CUserList.AddUser( &l_sRoomInfoToWaitUserR.l_sUserInfo );

								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();
                                g_pCMainFrame->WaitRoomIndexRefresh();

                                // 현재 보고있는 방의 정보 갱신
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserR.nRoomNo );

								break;
								

						}
						break;

					case NGSC_CREATEROOM:
						memcpy( &l_sMyRoomInfo, &szRealBufpos[nIndex], sizeof(l_sMyRoomInfo) );
						nIndex += sizeof(l_sMyRoomInfo);

						memcpy( &g_sRoomInfo, &l_sMyRoomInfo, sizeof(l_sMyRoomInfo) );


						//내가 방을 만들었으므로 내가 방장이고 선이다.
						g_CGameUserList.BangJangDel();
						g_CGameUserList.m_GameUserInfo[0].m_bBBangJjang = TRUE;

						g_CGameUserList.SunDel();
						g_CGameUserList.m_GameUserInfo[0].m_bSun = TRUE;
	
						//방에서 나와도 여기가 발생하기 때문에....게임에 들어간부분을 모두 초기화 시킨다.
						SendMessage( g_hWnd, WM_GAMEMODESET, 0, 0 );
                        g_pCMainFrame->m_pGameChat->Init();
						break;

					case NGSC_ROOMINFO:
                        {
						    memset( g_ClickRoomUserInfo, 0x00, sizeof(g_ClickRoomUserInfo) );
						    memcpy( g_ClickRoomUserInfo, &szRealBufpos[nIndex], sizeof(g_ClickRoomUserInfo) );
						    nIndex += sizeof(g_ClickRoomUserInfo);

                            bool bExist = false;    // 기존 선택 유저가 아직도 있는지 검사

                            // 방 정보가 갱신되어도 선택되어 있던 유저가 아직 있으면 선택된 유저 정보는 그대로 유지된다.
                            if ( g_pCMainFrame->m_nUserListTitleNum == 1 && g_pCMainFrame->m_nViewUserNo != -1 )  {
                                for ( i = 0; i < MAX_ROOMCNT; i++ ) {
                                    if ( g_ClickRoomUserInfo[i].szNick_Name[0] == NULL) continue;

                                    if ( g_ClickRoomUserInfo[i].nUserNo == g_pCMainFrame->m_nViewUserNo ) {
                                        bExist = true;
                                    }
                                }
                            }

                            // 처음부터 선택된 유저가 없거나, 선택된 유저가 방을 나가서 정보가 남아있지 않은 경우에는
                            // 선택된 유저 정보가 없다고 표시한다.
                            if ( !bExist ) {
                                g_pCMainFrame->m_nViewUserIdx = -1;
                                g_pCMainFrame->m_nViewUserNo = -1;
                                g_pCMainFrame->AvatarPlay( -1, g_CUserList.m_WaitUserInfo[0].m_sUserInfo.szPreAvatar );
                            }
                        }
						break;

					case NGSC_ROOMIN:
						memset( &l_sMyRoomInfo, 0x00, sizeof(l_sMyRoomInfo) );
						memcpy( &l_sMyRoomInfo, &szRealBufpos[nIndex], sizeof(l_sMyRoomInfo) );
						nIndex += sizeof(l_sMyRoomInfo);

						memcpy( &g_sRoomInfo, &l_sMyRoomInfo, sizeof(l_sMyRoomInfo) );

						memset( &l_sRoomInUserSend, 0x00, sizeof(l_sRoomInUserSend) ); 
						memcpy( &l_sRoomInUserSend, &szRealBufpos[nIndex], sizeof(l_sRoomInUserSend) );
						nIndex += sizeof(l_sRoomInUserSend);

						memcpy( &l_sRoomInAdd, &szRealBufpos[nIndex], sizeof(l_sRoomInAdd) );
						nIndex += sizeof(l_sRoomInAdd);
						

						nRoomInUserNo = l_sRoomInAdd.nRoomInUserNo;
						nGameDivi = l_sRoomInAdd.nGameDivi;
						

						/*
						//게임중에 내가 들어왔으면 초이스를 TRUE로 해야 다른사람카드를 볼수가 있다.
						//방정보중 0 : 대기중, 1 : 스타트버튼대기중, 2 : 게임중, 3 : 엔딩중
						if ( nGameDivi == 2 || nGameDivi == 3 ) {
							g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;

                            g_pCMainFrame->SetPanMoney( 0, l_sRoomInAdd.biRoomCurMoney, FALSE ); 
						}
                        else {
                            g_pCMainFrame->SetPanMoney( 0, l_sRoomInAdd.biRoomCurMoney ); 
                        }
						*/
						
						//yuno 2005.10.21
						if ( nGameDivi == 2 )
							g_pCMainFrame->SetPanMoney( 0, l_sRoomInAdd.biRoomCurMoney, FALSE );

						if ( nGameDivi == 2 || nGameDivi == 3 ) {
							g_CGameUserList.m_GameUserInfo[0].m_bChoioce = TRUE;
						}
						//////////////////////////////////////////////////////////////////////////

						//유저들을 넣는다. 자신은 현재 어레이가 0번이기때문에 i번째가 어레이 해당 위치가 된다.
						for ( i = 1; i < MAX_ROOM_IN; i++ ) {

							//자신의 번호 다음 부터 넣으면 된다.
							nUserArrayNo = ( nRoomInUserNo + i ) % MAX_ROOM_IN;
							
							if ( l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.nUserNo > 0 ) {

								DTRACE("NGSC_ROOMIN - AddUser  UserNo = %d, UserNick = %s",
									l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.nUserNo, l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.szNick_Name );
								
								nSlotNo = g_CGameUserList.AddUser( &l_sRoomInUserSend[nUserArrayNo].l_sUserInfo, i );

								//방장을 넣어준다.
								if ( l_sRoomInUserSend[nUserArrayNo].bBbangJjang == TRUE ) {
									g_CGameUserList.BangJangDel();
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_bBBangJjang = TRUE;
								}

								if ( l_sRoomInUserSend[nUserArrayNo].bRoomMaster == TRUE ) {
									g_CGameUserList.SunDel();
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_bSun = TRUE;
								}


								g_CGameUserList.m_GameUserInfo[nSlotNo].m_bChoioce = l_sRoomInUserSend[nUserArrayNo].bChoice;
								
								if ( nGameDivi == 2 ) {
									//유저 카드를 넣준다.
									for ( j = 0; j < l_sRoomInUserSend[nUserArrayNo].nCardCnt; j++ ) {
#ifdef SHOWUSERCARD
                                        g_pCMainFrame->UserCardAdd( l_sRoomInUserSend[nUserArrayNo].nUserCard[j], nSlotNo, FALSE );
#else
										//유저가 초이스를 했으면 1,2번카드는 뒷면 5-6번은 앞면, 7번은 뒷면을 보여주고
										//초이스를 하지 않았으면 모두 뒷면을 보여준다.
										if ( g_CGameUserList.m_GameUserInfo[nSlotNo].m_bChoioce &&
											 l_sRoomInUserSend[nUserArrayNo].nGameJoin == 1 )	//중간에 들어온놈이 다른 사람이 죽어있는 사람(대기자)이면 카드 뒷면을 보여줘야 한다.
                                        {
                                            if ( j < 2 || j > 5 )
												g_pCMainFrame->UserCardAdd( 255, nSlotNo, FALSE );
											else
												g_pCMainFrame->UserCardAdd( l_sRoomInUserSend[nUserArrayNo].nUserCard[j], nSlotNo, FALSE );
                                        }
										else
                                        {
                                            g_pCMainFrame->UserCardAdd( 255, nSlotNo, FALSE );
                                        }
#endif
									}
								}

								SendMessage( g_hWnd, WM_AVATARPLAY, 0, nSlotNo );
							}
						}

						memcpy( g_szRoomName, l_sMyRoomInfo.szRoomName, MAX_ROOM_NAME );

						//방에서 나와도 여기가 발생하기 때문에....게임에 들어간부분을 모두 초기화 시킨다.
						SendMessage( g_hWnd, WM_GAMEMODESET, 0, 0 );
                        g_pCMainFrame->m_pGameChat->Init();

						g_pCMainFrame->PlaySound(g_Sound[GS_ROOMIN]);
						break;

					case NGSC_ROOMIN_OTHERSEND:
						memset( &l_sUserInfo, 0x00, sizeof(l_sUserInfo) ); 
						memcpy( &l_sUserInfo, &szRealBufpos[nIndex], sizeof(l_sUserInfo) );
						nIndex += sizeof(l_sUserInfo);

						memcpy( &l_sRoomInOtherSend, &szRealBufpos[nIndex], sizeof(l_sRoomInOtherSend) );
						nIndex += sizeof(l_sRoomInOtherSend);

						nNextNo = l_sRoomInOtherSend.nNextNo;
						nCurCnt = l_sRoomInOtherSend.nRoomCurCnt;
					    nBbangJjangUniqNo = l_sRoomInOtherSend.nBbangJjangUniqNo;
						g_sRoomInfo.nCurCnt = nCurCnt;

						g_CGameUserList.NewBangJangSet( nBbangJjangUniqNo );

						nSlotNo = g_CGameUserList.AddRoomInOtherUser( &l_sUserInfo, nNextNo );
						SendMessage( g_hWnd, WM_AVATARPLAY, 0, nSlotNo );

						g_pCMainFrame->PlaySound(g_Sound[GS_ROOMIN]);

						//sprintf( ptemp, "◎ %s님이 입장하셨습니다.", l_sUserInfo.szNick_Name );	
						sprintf( ptemp, "◎ 닐리리님이 입장하셨습니다.");	
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 255 ));
						break;
						
					case NGSC_IPCHECK:
						g_bConnectingSpr = false;
						g_pCMainFrame->m_Message->PutMessage( "근접 아이피로 같은 방에서", "게임을 할수 없습니다." );
						break;

					case NGSC_ROOMOUT_OTHERSEND:
						memcpy( &l_sRoomOutOtherSend, &szRealBufpos[nIndex], sizeof(l_sRoomOutOtherSend) );
						nIndex += sizeof(l_sRoomOutOtherSend);


						//방장이 바뀌었으면 
						if ( l_sRoomOutOtherSend.nNewBbangJjnag != 0 ) {
							g_CGameUserList.NewBangJangSet( l_sRoomOutOtherSend.nNewBbangJjnag );
						}

						//게임 대기중 선이 나갔으면.
						if ( l_sRoomOutOtherSend.nNewRoomMaster != 0 ) {
							g_CGameUserList.NewSunSet( l_sRoomOutOtherSend.nNewRoomMaster );
						}


						g_sRoomInfo.nCurCnt = l_sRoomOutOtherSend.nRoomCurCnt;

						g_pCMainFrame->PlaySound(g_Sound[GS_ROOMEXIT]);


						nSlotNo = g_CGameUserList.FindUser( l_sRoomOutOtherSend.nUserNo );
						//sprintf( ptemp, "◎ %s님이 나가셨습니다.", g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.szNick_Name );					
						sprintf( ptemp, "◎ 닐리리님이 나가셨습니다." );					
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 200 ));
						g_CGameUserList.DeleteUser( l_sRoomOutOtherSend.nUserNo );
						break;

					case NGSC_GAMECHAT:

						DTRACE("Packet Recv - NGCC_GAMECHAT");

						memcpy( szSendNickName, &szRealBufpos[nIndex], MAX_NICK_LENGTH );
						nIndex += MAX_NICK_LENGTH;
						
						nEarTalkUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						nChatLen = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						memcpy( szChatMsg, &szRealBufpos[nIndex], nChatLen );
						nIndex += nChatLen;

                        g_pCMainFrame->SetGameChatText( szChatMsg, RGB(255, 255, 204) );
						break;

					case NGSC_CARDCHOICE:
                        //DTRACE2("[%s] NGSC_CARDCHOICE", g_szUser_ID );
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CARDCHOICE \n");
						nUniqueUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;
						
						//날아온 유저의 슬롯을 찾는다.
						nSlotNo = g_CGameUserList.FindUser( nUniqueUserNo );

						g_CGameUserList.m_GameUserInfo[nSlotNo].m_bChoioce = TRUE;

						//유저의 벌어져있는 카드를 오프린다.
						g_CGameUserList.UserCardPosRefresh( nSlotNo );
						g_pCMainFrame->PlaySound(g_Sound[GS_DEAL]);
						break;

					case NGSC_CHOICECARDVIEW:
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CHOICECARDVIEW \n");
						memset( l_sChoiceViewCard, 0x00, sizeof(l_sChoiceViewCard) );
						memcpy( &l_sChoiceViewCard, &szRealBufpos[nIndex], sizeof(l_sChoiceViewCard) );
						nIndex += sizeof(l_sChoiceViewCard);

						for ( i = 0; i < MAX_ROOM_IN; i++ ) {
							if ( l_sChoiceViewCard[i].nCallUserUniNo > 0 ) {
								nSlotNo = g_CGameUserList.FindUser( l_sChoiceViewCard[i].nCallUserUniNo );
								g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserCardInfo[2].nCardNo = l_sChoiceViewCard[i].byCardNo;
							}
						}
						break;
	
					case NGSC_CARDDUMP:
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CARDDUMP \n");
						nUniqueUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
						nIndex += 4;

						//날아온 유저의 슬롯을 찾는다.
						nSlotNo = g_CGameUserList.FindUser( nUniqueUserNo );
						g_CGameUserList.m_GameUserInfo[nSlotNo].m_nCardCnt--;
						break;

					case NGSC_GETONECARD:
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_GETONECARD \n");
						memcpy( &l_sGetOneCard, &szRealBufpos[nIndex], sizeof(l_sGetOneCard) );
						nIndex += sizeof(l_sGetOneCard);

						//처음 카드 초이스가 아니고 4구째부터 진행이면.
						if ( l_sGetOneCard.cUserSelect != -1 ) {

							//이전유저 콜, 등 선택 표시
							nSlotNo = g_CGameUserList.FindUser(  l_sGetOneCard.nCallUserUniNo );

                            g_pCMainFrame->SetPanMoney( nSlotNo, l_sGetOneCard.biRoomCurMoney );

							g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sGetOneCard.biCallUserMoney;

							g_pCMainFrame->UserSelectView( nSlotNo, l_sGetOneCard.cUserSelect, TRUE );						

#ifndef SHOWUSERCARD
							//이전 유저가 Die를 하였으면 Die한 유저의 카드를 덮는다.
							if ( l_sGetOneCard.cUserSelect == '7' ) {
								for ( i = 0; i < g_CGameUserList.m_GameUserInfo[nSlotNo].m_nCardCnt; i++ ) {
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserCardInfo[i].nCardNo = 255;
								}
							}
#endif
						}
                        else {
                            g_pCMainFrame->SetPanMoney( 0, l_sGetOneCard.biRoomCurMoney );
                        }

						//새로운 선을 임명.
						g_CGameUserList.NewSunSet( l_sGetOneCard.nRoomMasterUniqNo );

						for ( i = 0; i < MAX_ROOM_IN; i++ ) {
							if ( l_sGetOneCard.l_sUserOneCardInfo[i].nUserUniqueNo > 0 ) 
							{
								nSlotNo = g_CGameUserList.FindUser( l_sGetOneCard.l_sUserOneCardInfo[i].nUserUniqueNo );

#ifdef SHOWUSERCARD
                                g_pCMainFrame->UserCardAdd( l_sGetOneCard.l_sUserOneCardInfo[i].byCardNo, nSlotNo );

                                if ( nSlotNo != 0 ) {
                                    memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szJokboCard, l_sGetOneCard.szJokboCard[i], sizeof(l_sGetOneCard.szJokboCard[i]) );
                                }
#else
								if ( l_sGetOneCard.nRound <= 6 ) {
									g_pCMainFrame->UserCardAdd( l_sGetOneCard.l_sUserOneCardInfo[i].byCardNo, nSlotNo );

									//자신은 다른 패킷으로 온다. 내자신은 모든 족보를 봐야하기때문.
                                    if ( nSlotNo != 0 ) {
                                        memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szJokboCard, l_sGetOneCard.szJokboCard[i], sizeof(l_sGetOneCard.szJokboCard[i]) );
                                    }
								}
								else { 
									//막판이면..카드를 리얼쪽에다가 넣어놓느다.
									g_pCMainFrame->UserCardAdd( 255, nSlotNo );
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_byUserRealCardNo[6] = l_sGetOneCard.l_sUserOneCardInfo[i].byCardNo;
                                }
#endif
							}
						}

						nSlotNo = g_CGameUserList.FindUser( l_sGetOneCard.nRoomMasterUniqNo );
						g_pCMainFrame->SetUserCall( l_sGetOneCard.l_sGameCurUser, nSlotNo );
						g_pCMainFrame->m_bSelectionStart = TRUE;

						break;
                        
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BADUKI
					case NGSC_CHANGECARD:
                        //---------------------------------------------------------------
                        // 한 유저가 서버에 보낸 바꿀 카드 정보를 모든 유저에게 뿌려준다.
                        //---------------------------------------------------------------

						memset( &l_sUserNewCardPacket, 0x00, sizeof(l_sUserNewCardPacket) );
						memcpy( &l_sUserNewCardPacket, &szRealBufpos[nIndex], sizeof(l_sUserNewCardPacket) );
						nIndex += sizeof(l_sUserNewCardPacket);

                        nSlotNo = g_CGameUserList.FindUser( l_sUserNewCardPacket.nUniqUserNo );

                        if ( l_sUserNewCardPacket.bPass == TRUE ) {
                            g_pCMainFrame->SetPassUser( nSlotNo );
                            //g_pCMainFrame->ChgUserCard( &l_sUserNewCardPacket, 0 );
                        }
                        else {
							if(nSlotNo==0 && g_pCMainFrame->m_sTimeMode.nTime ==TTIME_EVENING) {
								for(int cnt=0;cnt<4;cnt++) {
									g_pCMainFrame->m_MyHiddenCardNo[cnt] = 255;
									g_pCMainFrame->m_MyHiddenPosition[cnt] = -1;
								}
							}
										
                            g_pCMainFrame->ChgUserCard( &l_sUserNewCardPacket, nSlotNo );

						    if(nSlotNo==0)
								memset( g_pCMainFrame->m_sChgCardInfo, 0x00, sizeof(g_pCMainFrame->m_sChgCardInfo ));
                        }

						break;


					case NGSC_CHANGEOKALL:
                        //-------------------------------------------------
                        // 아침, 점심, 저녁이 되었음을 알리는 패킷.
                        // 아침/점심/저녁이 시작될 때마다 한 번씩 전달된다.
                        //-------------------------------------------------
						g_pCMainFrame->m_ChonCurMousePointYOnHidden = -1;
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CHANGEOKALL \n");
						memset( &l_sChangeOkAll, 0x00, sizeof(l_sChangeOkAll) );
						memcpy( &l_sChangeOkAll, &szRealBufpos[nIndex], sizeof(l_sChangeOkAll) );
						nIndex += sizeof(l_sChangeOkAll);


						//이전유저 콜, 등 선택 표시
						nSlotNo = g_CGameUserList.FindUser( l_sChangeOkAll.nBeforeGameUserUniNo );
						g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sChangeOkAll.biBeforeGameUserMoney;

						g_pCMainFrame->UserSelectView( nSlotNo, l_sChangeOkAll.cBeforeUserSelect, TRUE );
						g_pCMainFrame->SetPanMoney( nSlotNo, l_sChangeOkAll.biRoomCurMoney);
						

						//////////////////////////////////////////////////////////////////////////
						//l_sChangeOkAll.nRound;  0: 아침, 1 : 점심, 2 : 저녁
   						//////////////////////////////////////////////////////////////////////////
                        // 아침, 점심, 저녁 여부 설정

                        if ( l_sChangeOkAll.nRound == 0 ) {
							TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  TTIME_MORNING \n");
                            g_pCMainFrame->m_sTimeMode.nTime = TTIME_MORNING;
                        }
                        else if ( l_sChangeOkAll.nRound == 1 ) {
                            g_pCMainFrame->m_sTimeMode.nTime = TTIME_AFTERNOON;
                        }
                        else if ( l_sChangeOkAll.nRound == 2 ) {
                            g_pCMainFrame->m_sTimeMode.nTime = TTIME_EVENING;
                        }
                        
						TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  TTIME_MORNING Before %d \n",nSlotNo);

					//	if(nSlotNo == 0 && g_pCMainFrame->m_IsMyTurn == TRUE)
					//		memset( g_pCMainFrame->m_sChgCardInfo, 0x00, sizeof( g_pCMainFrame->m_sChgCardInfo ));

					//	if(nSlotNo != 0)
					//		memset( g_pCMainFrame->m_sChgCardInfo, 0x00, sizeof( g_pCMainFrame->m_sChgCardInfo ));

						TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  TTIME_MORNING  After %d \n",nSlotNo);
						

						break;

					case NGSC_CHANGEOKUSER:
                        //-------------------------
                        // 카드 바꿀 유저 정보 전달
                        //-------------------------
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CHANGEOKUSER \n");

						//카드를 바꿀차례 유저
						nUniqueUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
						MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
						nIndex += 4;

						//카드를 바꿀차례 유저 슬롯.
						nSlotNo = g_CGameUserList.FindUser( nUniqueUserNo );
                        g_pCMainFrame->SetChgCardUser( nSlotNo );


						
						break;

                    /*
					case NGSC_STARTGAME:

						//BADUKI
						memset( &l_sGameStart, 0x00, sizeof(l_sGameStart) );
						memcpy( &l_sGameStart, &szRealBufpos[nIndex], sizeof(l_sGameStart) );
						nIndex += sizeof(l_sGameStart);

						g_biJackPotMoney = l_sGameStart.l_sStartUserCard.biJackPotMoney;
						g_bChoiceRoom = l_sGameStart.l_sStartUserCard.bChoiceRoom;

                        if ( g_bChoiceRoom ) {
                            nInitCardCnt = 4;
                        }
                        else {
                            nInitCardCnt = 3;
                        }
                        
						//////////////////////////////////////////////////////////////////////////
						//게임관련초기화.
						//////////////////////////////////////////////////////////////////////////
                        {
                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);

                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_STARTGAME - CreateEvent() 오류 발생 : 오류코드 %ld", g_szUser_ID, dwErr );
                            }

                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);        // 이벤트를 비신호상태로 만든다.

							DTRACE2("[%s]NGSC_STARTGAME - 이벤트 시작한다.", g_szUser_ID);

                            g_pCMainFrame->m_bGameInit = TRUE;

                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );

                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] 게임 초기화 실패", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "게임 초기화 실패하였습니다.", "Code - 1001", true );    
		                    }
                        }

						nRoomMasterArrayNo = g_CGameUserList.FindUser( l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo );

						//게임이끝이나면 초기화를 하기때문에..임명해준다.
						//새로운 선을 임명.
						g_CGameUserList.NewSunSet( l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo );

						//새로운 방장을 임명.
						g_CGameUserList.NewBangJangSet( l_sGameStart.l_sStartUserCard.nBbanJjangUniqNo );

						//카드 초기화를 모두 시킨다.
						for ( i = 0; i < MAX_ROOM_IN; i++  ) {
							//판돈 뺀다. 학교간다.
                            if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo <= 0 ) {
                                continue; 
                            }

							g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney -= g_sRoomInfo.biRoomMoney;
						}

						for ( i = 0; i < nInitCardCnt; i++ ) {
							for ( j = 0; j < MAX_ROOM_IN; j++ ) {
								//선다음부터 넣으면 된다.
								nUserArrayNo = ( nRoomMasterArrayNo + j + 1) % MAX_ROOM_IN;

                                if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.nUserNo <= 0 ) {
                                    continue;
                                }
#ifdef SHOWUSERCARD
                                g_pCMainFrame->UserCardAdd( l_sGameStart.l_sStartUserCard.byCardNo[i], nUserArrayNo );
#else
								//자기 자신이 아니면.
                                if ( nUserArrayNo != 0 ) {
                                    g_pCMainFrame->UserCardAdd( 255, nUserArrayNo );
                                }
                                else {
                                    g_pCMainFrame->UserCardAdd( l_sGameStart.l_sStartUserCard.byCardNo[i], nUserArrayNo );
                                }
#endif
							}
						}

						g_pCMainFrame->SetJackPotMoney( l_sGameStart.l_sStartUserCard.biJackPotMoney );
						g_pCMainFrame->SetPanMoney( 0, l_sGameStart.l_sStartUserCard.biRoomCurMoney ); 

						strcpy( ptemp, "◎ 게임시작 ◎" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						strcpy( ptemp, "◎ 베팅횟수 : 1-2-2-3◎" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						nSlotNo = g_CGameUserList.FindUser( l_sGameStart.l_sSetGameCurUser.nCurGameUserUniNo );

                        // 초기 카드가 다 이동한 다음에 게임이 진행되도록 받은 정보를 임시 장소에 저장해두고,
                        // 플래그를 설정한다.

                        memset( &(g_pCMainFrame->m_sStartUserSync), 0x00, sizeof( g_pCMainFrame->m_sStartUserSync ));
                        memcpy( &(g_pCMainFrame->m_sStartUserSync.l_sCurUser),
                                &(l_sGameStart.l_sSetGameCurUser.l_sGameCurUser),
                                sizeof( g_pCMainFrame->m_sStartUserSync.l_sCurUser ));
                        g_pCMainFrame->m_sStartUserSync.nSlotNo = nSlotNo;
                        g_pCMainFrame->m_sStartUserSync.bInsertInitCard = TRUE;

						break;
                        */

                    case NGSC_STARTGAME:
					//	memcpy( &l_sStartTotUserCard, &szRealBufpos[nIndex], sizeof(l_sStartTotUserCard) );
					//	nIndex += sizeof(l_sStartTotUserCard);

						g_pCMainFrame->m_ChonHiddenOpenMode = FALSE;
						
						for(i=0;i<4;i++) {
							g_pCMainFrame->m_MyHiddenPosition[i] = -1;
                        }

						memset( g_pCMainFrame->m_sChgCardInfo, 0x00, sizeof(g_pCMainFrame->m_sChgCardInfo ));	 //ChonKW 06.03.21
						memset( &l_sGameStart, 0x00, sizeof(l_sGameStart) );
						memcpy( &l_sGameStart, &szRealBufpos[nIndex], sizeof(l_sGameStart) );
						nIndex += sizeof(l_sGameStart);

						g_biJackPotMoney = l_sGameStart.l_sStartUserCard.biJackPotMoney;
						g_bChoiceRoom = l_sGameStart.l_sStartUserCard.bChoiceRoom;

                        if ( g_bChoiceRoom ) {
                            nInitCardCnt = 4;
                        }
                        else {
                            nInitCardCnt = 3;
                        }
                        
                        {
                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);

                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_STARTGAME - CreateEvent() 오류 발생 : 오류코드 %ld", g_szUser_ID, dwErr );
                            }

                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);

							DTRACE2("[%s]NGSC_STARTGAME - 이벤트 시작한다.", g_szUser_ID);

                            g_pCMainFrame->m_bGameInit = TRUE;

                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );

                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] 게임 초기화 실패", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "게임 초기화 실패하였습니다.", "Code - 1001", true );    
		                    }
                        }

						nRoomMasterArrayNo = g_CGameUserList.FindUser( l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo );

						g_CGameUserList.NewSunSet( l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo );

						g_CGameUserList.NewBangJangSet( l_sGameStart.l_sStartUserCard.nBbanJjangUniqNo );

						for ( i = 0; i < MAX_ROOM_IN; i++  ) {
                            if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo <= 0 ) {
                                continue; 
                            }

							g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney -= g_sRoomInfo.biRoomMoney;
						}


						for ( i = 0; i < nInitCardCnt; i++ ) {
							for ( j = 0; j < MAX_ROOM_IN; j++ ) {
								
								if ( l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].nUserUniqNo <= 0 ) 
									continue;
								
								//패킷에 담아져 있는 순서대로 넣어주면 된다.
								nSlotNo = g_CGameUserList.FindUser( l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].nUserUniqNo );

#ifdef SHOWUSERCARD
								g_pCMainFrame->UserCardAdd( l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].byCardNo[i], nSlotNo );
#else								
								if ( nSlotNo != 0 ) 
								{
									g_pCMainFrame->UserCardAdd( 255, nSlotNo );
								}
								else
								{
									g_pCMainFrame->UserCardAdd( l_sGameStart.l_sStartUserCard.l_sStartPerUserCard[j].byCardNo[i], nSlotNo );
								}
#endif

							}
						}

						g_pCMainFrame->SetJackPotMoney( l_sGameStart.l_sStartUserCard.biJackPotMoney );
						g_pCMainFrame->SetPanMoney( 0, l_sGameStart.l_sStartUserCard.biRoomCurMoney ); 

						strcpy( ptemp, "◎ 게임시작 ◎" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						strcpy( ptemp, "◎ 베팅횟수 : 1-2-2-3◎" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						nSlotNo = g_CGameUserList.FindUser( l_sGameStart.l_sSetGameCurUser.nCurGameUserUniNo );

                        memset( &(g_pCMainFrame->m_sStartUserSync), 0x00, sizeof( g_pCMainFrame->m_sStartUserSync ));
                        memcpy( &(g_pCMainFrame->m_sStartUserSync.l_sCurUser),
                                &(l_sGameStart.l_sSetGameCurUser.l_sGameCurUser),
                                sizeof( g_pCMainFrame->m_sStartUserSync.l_sCurUser ));
                        g_pCMainFrame->m_sStartUserSync.nSlotNo = nSlotNo;
                        g_pCMainFrame->m_sStartUserSync.bInsertInitCard = TRUE;

						break;

//BADUKI
////////////////////////////////////////////////////////////////////////////////////////////////////////////




					case NGSC_SETGAMECURUSER:
                        TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_SETGAMECURUSER \n");
							
						memcpy( &l_sSetGameCurUser, &szRealBufpos[nIndex], sizeof(l_sSetGameCurUser) );
						nIndex += sizeof(l_sSetGameCurUser);

						//이전유저 콜, 등 선택 표시
						nSlotNo = g_CGameUserList.FindUser( l_sSetGameCurUser.nBeforeGameUserUniNo );

                        // 동기화를 위해서 다음 유저의 정보를 임시 저장
                        memset( &(g_pCMainFrame->m_sNextUserInfo), 0x00, sizeof( g_pCMainFrame->m_sNextUserInfo ));
                        memcpy( &(g_pCMainFrame->m_sNextUserInfo.l_sNextUser), &l_sSetGameCurUser, sizeof( l_sSetGameCurUser ));
                        g_pCMainFrame->m_sNextUserInfo.nSlotNo = nSlotNo;
                        g_pCMainFrame->m_sNextUserInfo.bChange = TRUE;
						break;

					case NGSC_ENDGAME:
                        //DTRACE2("[%s] NGSC_ENDGAME", g_szUser_ID );
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_ENDGAME \n");

						memcpy( &l_sEndGame, &szRealBufpos[nIndex], sizeof(l_sEndGame) );
						nIndex += sizeof(l_sEndGame);


						//이전유저 콜, 등 선택 표시
						nSlotNo = g_CGameUserList.FindUser( l_sEndGame.nCallUserUniNo ); 


						g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sEndGame.biCallGameUserMoney;


						g_pCMainFrame->UserSelectView( nSlotNo, l_sEndGame.cUserSelect, TRUE);

#ifndef SHOWUSERCARD
						//이전 유저가 Die를 하였으면 Die한 유저의 카드를 덮는다.
						if ( l_sEndGame.cUserSelect == '7' ) {
							for ( i = 0; i < g_CGameUserList.m_GameUserInfo[nSlotNo].m_nCardCnt; i++ ) {
								g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserCardInfo[i].nCardNo = 255;
							}
						}
#endif

						g_pCMainFrame->SetPanMoney(nSlotNo, l_sEndGame.biRoomCurMoney);

						g_biPureWinMoney = l_sEndGame.biPureWinnerMoney;
						
						g_nCurMasterSlotNo = g_CGameUserList.FindUser( l_sEndGame.nBeforeGameUserUniNo );
						
						g_nWinnerSlotNo = g_CGameUserList.FindUser( l_sEndGame.nWinnerUserUniNo );

						for ( i = 0; i < MAX_ROOM_IN; i++ ) {

							if ( l_sEndGame.l_sUserTotalCardInfo[i].nUserUniqueNo > 0 ) {
								nSlotNo = g_CGameUserList.FindUser( l_sEndGame.l_sUserTotalCardInfo[i].nUserUniqueNo );

                                g_CGameUserList.m_GameUserInfo[nSlotNo].m_bChoioce = TRUE;

								memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_byUserRealCardNo, l_sEndGame.l_sUserTotalCardInfo[i].byCardNo, sizeof(l_sEndGame.l_sUserTotalCardInfo[i].byCardNo) );
								g_CGameUserList.m_GameUserInfo[nSlotNo].m_biRealUserMoney = l_sEndGame.l_biUserMoney[i];
								g_CGameUserList.m_GameUserInfo[nSlotNo].m_biPureUserWinLoseMoney = l_sEndGame.l_biPureUserWinLoseMoney[i];

								//유저들 승패갱신
                                if ( g_nWinnerSlotNo == nSlotNo ) { //승자이면.
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nWin_Num++;
                                }
                                else {
                                    g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nLose_Num++;
                                }

								//자신은 다른 패킷으로 온다.
								//임시에 저장해놓고 카드를 오픈하면 보여주기위해서.
								memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szTempJokboCard, l_sEndGame.szJokboCard[i], sizeof(l_sEndGame.szJokboCard[i]) );

								//족보에 해당하는 카드를 담아놓는다.
								memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szFixJokboCard, l_sEndGame.szFixJokboCard[i], sizeof(l_sEndGame.szFixJokboCard[i]) );
							}
						}

                        // 비긴 게임이면 비긴 유저들의 번호를 저장하고, 플래그를 설정한다.

                        // 모든 비긴 우승자 값 초기화
                        memset( g_pCMainFrame->m_nDrawWinner, 0x00, sizeof( g_pCMainFrame->m_nDrawWinner ));

                        if ( l_sEndGame.bDrawGame == TRUE ) {
                            for ( i = 0; i < MAX_ROOM_IN; i++ ) {
                                if ( l_sEndGame.nDrawWinnerUniq[i] != -1 ) {
                                    nSlotNo = g_CGameUserList.FindUser( l_sEndGame.nDrawWinnerUniq[i] );
                                    g_pCMainFrame->m_nDrawWinner[nSlotNo] = 1;  // 비긴 우승자 설정
                                }
                            }

                            g_pCMainFrame->m_bDrawGame = TRUE;
                        }

						memset( g_szResultCmt, 0x00, sizeof(g_szResultCmt) );
                        g_pCMainFrame->SetGameChatText( "◎---게임결과----◎", RGB( 255, 255, 51) );
						g_pCMainFrame->m_bEndding = TRUE;
                        g_pCMainFrame->ClearTimeMode();
						break;


					case NGSC_MYWINGAME:
						memcpy( &l_sMyWinGame, &szRealBufpos[nIndex], sizeof(l_sMyWinGame) );


						nIndex += sizeof(l_sMyWinGame);

						//이전유저 콜, 등 선택 표시
						nSlotNo = g_CGameUserList.FindUser( l_sMyWinGame.nCallUserUniNo ); 

						g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sMyWinGame.biCallGameUserMoney;

						g_pCMainFrame->UserSelectView( nSlotNo, l_sMyWinGame.cUserSelect, TRUE );

#ifndef SHOWUSERCARD
						//이전 유저가 Die를 하였으면 Die한 유저의 카드를 덮는다.
						if ( l_sMyWinGame.cUserSelect == '7' ) {
							for ( i = 0; i < g_CGameUserList.m_GameUserInfo[nSlotNo].m_nCardCnt; i++ ) {
								g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserCardInfo[i].nCardNo = 255;
							}
						}
#endif

						g_pCMainFrame->SetPanMoney(nSlotNo, l_sMyWinGame.biRoomCurMoney);

						g_biPureWinMoney = l_sMyWinGame.biPureWinnerMoney;
						g_nWinnerSlotNo = g_CGameUserList.FindUser( l_sMyWinGame.nWinnerUserUniNo );
						g_CGameUserList.m_GameUserInfo[g_nWinnerSlotNo].m_biPureUserWinLoseMoney = l_sMyWinGame.biPureWinnerMoney;

						memset( g_szResultCmt, 0x00, sizeof(g_szResultCmt) );
						strcpy( g_szResultCmt, "기권승" );
						
						//유저들 돈 및 승패.갱신.
						for ( i = 0; i < MAX_ROOM_IN; i++ ) {

							if ( l_sMyWinGame.l_nUserUniqNo[i] > 0 ) {

								nSlotNo = g_CGameUserList.FindUser( l_sMyWinGame.l_nUserUniqNo[i] );

								g_CGameUserList.m_GameUserInfo[nSlotNo].m_biRealUserMoney = l_sMyWinGame.l_biUserMoney[i];
                                g_CGameUserList.m_GameUserInfo[nSlotNo].m_biPureUserWinLoseMoney = l_sMyWinGame.l_biPureUserWinLoseMoney[i];

								//유저들 승패갱신
                                if ( g_nWinnerSlotNo == nSlotNo ) { //승자이면.
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nWin_Num++;
                                }
                                else {
                                    g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nLose_Num++;
                                }

							}

						}


                        g_pCMainFrame->SetGameChatText( "◎----기권승-----◎", RGB( 180, 180, 180 ) );

						g_bMyWin = TRUE;
						g_pCMainFrame->m_bEnddingShow = TRUE;
                        g_pCMainFrame->m_bMoveHiddenCard = FALSE;
						g_pCMainFrame->m_ChonHiddenMode = FALSE;
                        g_pCMainFrame->ClearTimeMode();
						break;


					case NGSC_NAGARIGAME:
						
						memset( g_szResultCmt, 0x00, sizeof(g_szResultCmt) );
						strcpy( g_szResultCmt, "나가리게임" );
						g_pCMainFrame->m_bEnddingShow = TRUE;
                        g_pCMainFrame->m_bMoveHiddenCard = FALSE;
						g_pCMainFrame->m_ChonHiddenMode = FALSE;
                        g_pCMainFrame->ClearTimeMode();
						break;
						

					case NGSC_STARTBTN_ONOFF:

						//게임중에 두명이 있고 한명이 다이해서 결과창이 출력중에 한명이 들어오는상황
						//결과창 출력중에 이긴놈이 억지로 게임을 종료하면 서버는 방에 대기자만 있는것으로 생각한다.
						//왜냐하면 다이 한놈은 대기자이기때문이다.
						//그래서 결과창이 떠있는 동안 스타트가 날아온다. 이때를 대비해서 그냥 게임을 초기화 하자.
						//g_pCMainFrame->GameInit();
                        {

                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);


                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_STARTBTN_ONOFF - CreateEvent() 오류 발생 : 오류코드 %ld", g_szUser_ID, dwErr );
                            }


                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);

							DTRACE2("[%s]NGSC_STARTBTN_ONOFF - 이벤트 시작한다.", g_szUser_ID);

                            g_pCMainFrame->m_bGameInit = TRUE;

                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );
		                    
                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] 게임 초기화 실패", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "게임 초기화 실패하였습니다.", "Code - 1002", true );    
		                    }

                        }

						cStartBtnOnOff = szRealBufpos[nIndex++];

						if ( cStartBtnOnOff == '0' ) {
							g_pCMainFrame->m_bShowStartBTN = FALSE;
						}
						else {
							g_pCMainFrame->m_bShowStartBTN = TRUE;
						}

						g_pCMainFrame->m_nStartBtnCnt = 0;
						break;

					case NGSC_VIEWWAIT:

						DTRACE("Packet Recv - NGSC_VIEWWAIT");
						
						nWaitUserCnt = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

                        memset( g_sGameWaitInfo, 0x00, sizeof(g_sGameWaitInfo) );
                        memcpy( g_sGameWaitInfo, &szRealBufpos[nIndex], sizeof(sGameWaitInfo) * nWaitUserCnt );

						nIndex += sizeof(sGameWaitInfo) * nWaitUserCnt;

                        //체크된 것은 FALSE 로 만든다. ( NEXT 나 PREV 를 눌렀을 경우를 대비 )
						nTempNo	= g_pCMainFrame->GetInviteCheck();
						g_pCMainFrame->m_sInviteCheck[nTempNo].bCheck = FALSE;
						g_pCMainFrame->PlaySound(g_Sound[GS_POPUP]);
						g_pCMainFrame->m_bShowInviteBox = TRUE;
                        g_pCMainFrame->m_SCB_Invite->SetElem( nWaitUserCnt );
                        g_pCMainFrame->m_SCB_Invite->StartPage();
						break;


					case NGSC_USERINVITE:

						DTRACE("Packet Recv - NGSC_USERINVITE");
						memset( &l_sUserInvite, 0x00, sizeof(l_sUserInvite) );
						memcpy( &l_sUserInvite, &szRealBufpos[nIndex],  sizeof(l_sUserInvite) );
						nIndex += sizeof(l_sUserInvite);

						memcpy( szInviteMsg, &szRealBufpos[nIndex], l_sUserInvite.nInviteMsgLen );
						nIndex += l_sUserInvite.nInviteMsgLen;

						g_pCMainFrame->PlaySound(g_Sound[GS_POPUP]);
						g_pCMainFrame->RecieveInvite( l_sUserInvite.nRoomNo , l_sUserInvite.biRoomMoney, 
							l_sUserInvite.szRoomPass , szInviteMsg );
						
						break;

					case NGSC_ALLCMT:

						memset( szAllCmt, 0x00, 1000 );
						memcpy( szAllCmt, &szRealBufpos[nIndex], 1000 );
						nIndex += 1000;

						DTRACE("NGSC_ALLCMT - %s", szAllCmt);
                        if ( g_pCMainFrame->m_nCurrentMode == WAITROOM_MODE ) {
                            STextInfo _sTextInfo;
                            memset( &_sTextInfo, 0x00, sizeof(STextInfo) );
                            strcpy( _sTextInfo.szText, szAllCmt );
                            _sTextInfo.nStyle = 0;
                            _sTextInfo.rgbColor = RGB( 73 , 73 , 73 );
                            g_pCMainFrame->m_pWaitChat->AddText( &_sTextInfo );
                        }
                        else if ( g_pCMainFrame->m_nCurrentMode == GAME_MODE ) {
                            g_pCMainFrame->SetGameChatText( szAllCmt, RGB(132, 255, 200) );
                        }

						break;


					case NGSC_MYUSERJOKBO:

						memset( &l_sMyUserJokBo, 0x00, sizeof(l_sMyUserJokBo) );
						memcpy( &l_sMyUserJokBo, &szRealBufpos[nIndex], sizeof(l_sMyUserJokBo) );
						nIndex += sizeof(l_sMyUserJokBo);

                        // 내 족보 패킷이 전달되면 임시 저장 장소에 족보를 복사하고, 플래그를 켠다.
                        memset( &( g_pCMainFrame->m_sTempMyUserJokBo ), 0x00, sizeof( g_pCMainFrame->m_sTempMyUserJokBo ));
                        memcpy( &( g_pCMainFrame->m_sTempMyUserJokBo ), &l_sMyUserJokBo, sizeof( l_sMyUserJokBo ));
                        g_pCMainFrame->m_bChgMyJokBo = TRUE;
						break;

					case NGSC_ALLUSERJOKBO:
						memset( &l_sAllUserJokBo, 0x00, sizeof(l_sAllUserJokBo) );
						memcpy( &l_sAllUserJokBo, &szRealBufpos[nIndex], sizeof(l_sAllUserJokBo) );
						nIndex += sizeof(l_sAllUserJokBo);

#ifdef SHOWUSERCARD

						for ( i = 0; i < MAX_ROOM_IN; i++ )
						{
                            if ( l_sAllUserJokBo.nUserUniqNo[i] <= 0 ) {
                                continue;
                            }

							//패킷에 담아져 있는 순서대로 넣어주면 된다.
							nSlotNo = g_CGameUserList.FindUser( l_sAllUserJokBo.nUserUniqNo[i] );

							memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szJokboCard, &l_sAllUserJokBo.l_sMyUserJokBo[i], sizeof(l_sAllUserJokBo.l_sMyUserJokBo[i]) );

							g_CGameUserList.m_GameUserInfo[nSlotNo].m_bEnd = true;
						}
#endif
										
						break;

					case NGSC_GETOUT:
						break;

					case NGSC_MEMO:
                        
						DTRACE("Packet Recv - NGSC_MEMO");

						memcpy( szSendNickName, &szRealBufpos[nIndex], MAX_NICK_LENGTH );
						nIndex += MAX_NICK_LENGTH;

						nMemoLen = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						memcpy( szMemoMsg, &szRealBufpos[nIndex], nMemoLen );
						nIndex += nMemoLen;

						g_pCMainFrame->RecieveMemo( szSendNickName , szMemoMsg );

						break;

                    case NGSC_GETOUTQUES:
                        break;

                    case NGSC_USERCHANGE:
                        memset( &l_sUserChange, 0x00, sizeof( l_sUserChange ));
                        memcpy( &l_sUserChange, &szRealBufpos[nIndex], sizeof( l_sUserChange ));
                        nIndex += sizeof( l_sUserChange );

#ifdef CHANGEUSERCARD
                        nSlotNo = g_CGameUserList.FindUser( l_sUserChange.nUserUniqNo );

                        g_pCMainFrame->ChangeUserOneCard( nSlotNo, &l_sUserChange );
#endif
                        break;
                        
				}

			}
		}
	
		Sleep(50);
	}

    _endthreadex( 0 );
	return 0;
}



//유저리스트에 이빨이 빠져있으므로 정렬한다.
int UserSort( void )
{
	int nWaitUserNo = 0 ;       // 대기자 수
	sUserInfo l_sTempUserInfo;

	memset( &l_sTempUserInfo , 0x00 , sizeof(sUserInfo));

	for( int i = 0 ; i < MAX_CHANNELPER; i ++ ) {
		if( g_CUserList.m_WaitUserInfo[i].m_bUser == FALSE ) {
			if( g_CUserList.m_WaitUserInfo[ i + 1 ].m_bUser == TRUE ) {
				memcpy( &l_sTempUserInfo, &g_CUserList.m_WaitUserInfo[i + 1].m_sUserInfo, sizeof(g_CUserList.m_WaitUserInfo[i + 1].m_sUserInfo) );
				g_CUserList.DeleteUser( g_CUserList.m_WaitUserInfo[i + 1].m_sUserInfo.nUserNo );
				g_CUserList.AddUser( &l_sTempUserInfo );
			}			
		}
		
		if( g_CUserList.m_WaitUserInfo[i].m_bUser == TRUE ) {
			nWaitUserNo++;
		}
	}

    g_pCMainFrame->m_SCB_WaitUser->SetElem( nWaitUserNo );  // 대기자 리스트 스크롤 바 갱신(전체 대기자 수)

	return nWaitUserNo;
}


