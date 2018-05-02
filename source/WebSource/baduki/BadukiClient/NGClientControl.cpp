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
			g_pCMainFrame->m_Message->PutMessage( "����Ÿ���̽� ������ �����Ͽ����ϴ�.", "Code - 100", true );
			break;

		case ERROR_LIMIT_USER:
			g_pCMainFrame->m_Message->PutMessage( "������ ���� �ʹ� �����ϴ�.", "Code - 102" );
			break;

		case ERROR_NOCREATEROOM:
			g_pCMainFrame->m_Message->PutMessage( "���̻� ���� �����Ҽ� �����ϴ�.", "Code - 103" );
			break;

		case ERROR_NOROOM:
			g_pCMainFrame->m_Message->PutMessage( "�������� �ʴ� ���Դϴ�.", "Code - 104" );
			break;
		
		case ERROR_ROOMFULL:
			g_pCMainFrame->m_Message->PutMessage( "�濡 �ο��� ��� á���ϴ�.", "Code - 105" );
			break;
		
		case ERROR_NOTPWD:
			g_pCMainFrame->m_Message->PutMessage( "��й�ȣ�� Ʋ�Ƚ��ϴ�.", "Code - 106" );
			break;

		case ERROR_NOQUICK:
			PostMessage( g_hWnd, WM_OUICKFAILCREATEROOM, 0, 0 );
			break;

		case ERROR_NOTVER:
			g_pCMainFrame->m_Message->PutMessage( "������ �ٸ��ϴ�. ����� ���μ�ġ�ϼ���!!!", "Code - 139", TRUE );
			break;

		case ERROR_MULTILOGIN:
			g_pCMainFrame->m_Message->PutMessage( "�̹� ������� ���̵��Դϴ�.!!!", "Code - 140", TRUE );
			break;

		case ERROR_NOTOPTION:
			g_pCMainFrame->m_Message->PutMessage( "����,�ʴ�ź� �Ǵ� ���ӻ����Դϴ�.", "Code - 137" );
			break;

        // CHANGEUSERCARD
        //������ ī�带 �ٲٷ� �ߴµ� �������� ���� ī���̴�.
        case ERROR_NOCARD:
            g_pCMainFrame->SetGameChatText( "�� ���� ������ ���� ���� ī���Դϴ�." , RGB( 255, 255, 255 ));
            break;

        //������ �ٲٱ� ���ϴ� ī�忴�µ� ī���ѿ� ���� ī���̴�.
        case ERROR_NOCARDPACK:
            g_pCMainFrame->SetGameChatText( "�� ī���ѿ� �ٲ� ī�尡 �����ϴ�." , RGB( 255, 255, 255 ));
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
			g_ServerSend = true;	//�������� ������ �޾����� �ٽ� ������ �ִ»��·� �����.

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
									g_pCMainFrame->m_Message->PutMessage( "�α��� ����", "Code - 109", true );
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
						//�ڽ��� ������ �޴´�.
						memcpy( &l_sUserList, &szRealBufpos[nIndex], sizeof(l_sUserList) );
						nIndex += sizeof(l_sUserList);
				
						l_sUserList.l_sUserInfo.nChannel = g_szGameServerChannel;
						//���ӳ��� ��������Ʈ�� �ִ´�. g_CGameUserList�� ù��°�� �׻� �ڱ��ڽ��� ����.
						memcpy( l_sUserList.l_sUserInfo.szPreAvatar, g_szAvaUrl, sizeof(g_szAvaUrl) );
						g_CGameUserList.AddUser( &l_sUserList.l_sUserInfo ); 

						memset( &l_sPKWaitInputCS, 0x00, sizeof(l_sPKWaitInputCS) );
						//�α��μ������� ���������� �ƴ϶� ���Ӽ������� ���� �����ѹ��� �����ϱ� ����...
						g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.nUserNo = g_nUserNo;
						memcpy( l_sPKWaitInputCS.l_sUserInfo.szUser_Id, g_szUser_ID, MAX_ID_LENGTH );

						
						memcpy( &l_sPKWaitInputCS.l_sUserInfo.l_sUserInfo, &g_CGameUserList.m_GameUserInfo[0].m_sUserInfo, sizeof(g_CGameUserList.m_GameUserInfo[0].m_sUserInfo) );

						l_sPKWaitInputCS.l_sOption.bInvite = g_sClientEnviro.bOption[0];
						l_sPKWaitInputCS.l_sOption.bEarTalk = g_sClientEnviro.bOption[1];
						l_sPKWaitInputCS.l_sOption.bMemoRecv = g_sClientEnviro.bOption[3];


						if(!pClientSocket->WaitInput(&l_sPKWaitInputCS))
						{
							g_pCMainFrame->m_Message->PutMessage( "�������� �������� ����", "Code - 110" );

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
						//������ ä���� ���⿡�� �־�����Ѵ�.
						l_sLogIn.l_sUserList.l_sUserInfo.nChannel = g_szGameServerChannel;

						//���ӳ��� ��������Ʈ�� �ִ´�. g_CGameUserList�� ù��°�� �׻� �ڱ��ڽ��� ����.
						memcpy( l_sLogIn.l_sUserList.l_sUserInfo.szPreAvatar, g_szAvaUrl, sizeof(g_szAvaUrl) );
						g_CGameUserList.AddUser( &l_sLogIn.l_sUserList.l_sUserInfo, 1 ); 
						*/

						pClientSocket->Destroy();
						g_cServerDivi = '2';
						
						//Counter Server�� ����.
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
						/*������ ��Ŷ
						nUserNo : ���������ѹ�
						nRoomCnt : 1������ ���
						sRoomInfo : ������	( ����� �ι������� �𸥴�. )
						nWaitUserCnt : ���� �����
						sUserInfo : ���� ��������
						*/
						//////////////////////////////////////////////////////////////////////////
						
						nUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += sizeof(int);

						nRoomCnt = 	MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						//���� ��������Ʈ�� �ʱ�ȭ�Ѵ�.
						memset( &g_CUserList.m_WaitUserInfo , 0x00 , sizeof(g_CUserList.m_WaitUserInfo));

						//���� �渮��Ʈ�� �ʱ�ȭ�Ѵ�.
						g_CWaitRoomList.Init();

						g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.bInvite = g_sClientEnviro.bOption[0];

						//�ڱ��ڽ��� ������ �ִ´�.
						g_CUserList.AddUser( &g_CGameUserList.m_GameUserInfo[0].m_sUserInfo );

						//������.				
						for ( i = 0; i < nRoomCnt; i++ ) {
							g_CWaitRoomList.AddRoom( (sRoomInfo *)&szRealBufpos[nIndex] );
							nIndex += sizeof(sRoomInfo);
						}

                        // ���� �ε��� ����
                        g_pCMainFrame->WaitRoomIndexRefresh();

						nWaitUserCnt = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						//���� �����
						g_pCMainFrame->m_nWaitUserCurCnt = nWaitUserCnt + 1 ; // + 1 �� ����(�ڱ��ڽ�)����

						memset( &l_sUserInfo, 0x00, sizeof(l_sUserInfo) );

						//���� ������ ��������Ʈ�� �ִ´�.
						for ( i = 0;  i < nWaitUserCnt; i++ ) {
							memcpy( &l_sUserInfo, &szRealBufpos[nIndex], sizeof(sUserInfo) );
							nIndex += sizeof(sUserInfo);

							if ( g_CUserList.AddUser( &l_sUserInfo ) < 0 ) {
								g_pCMainFrame->m_Message->PutMessage( "������ �ʹ� �����ϴ�.", "Code - 112" );
                                
                                _endthreadex( 0 ); // ham 2005.11.29
								return 0;
							}
						}
						
						nIndex += sizeof(sUserInfo) * nWaitUserCnt;

						// ù��° ������ ������ �� ������ ����Ʈ �س��´�.
                        if ( g_pCMainFrame->m_nUserListTitleNum == 0 ) {
						    if( g_CUserList.m_WaitUserInfo[0].m_bUser == TRUE ) {
							    g_pCMainFrame->m_nViewUserIdx = 0;
						    }
                        }
                        
                        // ��ũ�� ���� ��ü �׸� �� �ʱ�ȭ
                        g_pCMainFrame->m_SCB_WaitUser->SetElem( nWaitUserCnt + 1 ); // +1�� ���� �ڽ�

						//�濡�� ���͵� ���Ⱑ �߻��ϱ� ������....���ӿ� ���κ��� ��� �ʱ�ȭ ��Ų��.
						SendMessage( g_hWnd, WM_WAITMODESET, 0, 0 );

						//������ ������ �����.
						g_CGameUserList.UserAllClear();

                        {
                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);

                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_WAITINPUT - CreateEvent() ���� �߻� : �����ڵ� %ld", g_szUser_ID, dwErr );
                            }

                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);

                            g_pCMainFrame->m_bGameInit = TRUE;
                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );
		                    
                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] ���� �ʱ�ȭ ����", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "���� �ʱ�ȭ �����Ͽ����ϴ�.", "Code - 1000", true );    
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

						//���� �渮��Ʈ�� �ʱ�ȭ�Ѵ�.
						g_CWaitRoomList.Init();
						
						for ( i = 0; i < nRoomCnt; i++ ) {
							
							g_CWaitRoomList.AddRoom( &l_spWaitRoomInfo[i] );

						}

						if ( g_nViewPageNo != 0 ) {
							
							if ( nRoomCnt == 0 ) {
								
								//�ι����� ���� ������ ó������ �ѱ��.
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
						sprintf( ptemp, "%s���� ��: ", szSendNickName );					

						// ���� ��� �г��Ӱ� �� �г����� ������
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
						//             'A' : join    'R' : exit, roomdel�� Client���� ó��), 
						//			   'X' : RoomInCloseSoket(�濡������Close)
						switch(cWaitInfoDivi) {
							case 'C':
								memcpy( &l_sOnCreateRoom, &szRealBufpos[nIndex], sizeof(l_sOnCreateRoom) );
								nIndex += sizeof(l_sOnCreateRoom);
								
								//���� ������ �����Ѵ�. ���� ������ ���� ��������״�.
								nUniqueUserNo = l_sOnCreateRoom.nUserNo; // 1. ����ũ �ѹ��� �޴´�.
								g_CUserList.DeleteUser(nUniqueUserNo);
								
								//���� �븮��Ʈ�� �ִ´�.
								g_CWaitRoomList.AddRoom( &l_sOnCreateRoom.l_sRoomInfo );
								
								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();
                                g_pCMainFrame->WaitRoomIndexRefresh();

								break;

							case 'A':

								memcpy( &l_sRoomInfoToWaitUserAX, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserAX) );
								nIndex += sizeof(l_sRoomInfoToWaitUserAX);
								
								//���� ������ �����Ѵ�. ���� ������ ���� ��������״�.
								nUniqueUserNo = l_sRoomInfoToWaitUserAX.nUserNo; // 1. ����ũ �ѹ��� �޴´�.
								g_CUserList.DeleteUser( nUniqueUserNo );
								
								//���� �渮��Ʈ�� �ο��� �����Ѵ�.
								g_CWaitRoomList.AddUser( l_sRoomInfoToWaitUserAX.nRoomNo, l_sRoomInfoToWaitUserAX.nRoomCnt );
								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();

                                //���ο��� �� á������ ���÷��� ���ָ� �ȴ�.
                                if ( l_sRoomInfoToWaitUserAX.nRoomCnt >= MAX_ROOM_IN ) {
                                    g_pCMainFrame->WaitRoomIndexRefresh();
                                }

                                // ���� �����ִ� ���� ���� ����
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserAX.nRoomNo );
								break;

							case 'X':

								memcpy( &l_sRoomInfoToWaitUserAX, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserAX) );
								nIndex += sizeof(l_sRoomInfoToWaitUserAX);

								if( l_sRoomInfoToWaitUserAX.nRoomCnt > 0 )	//�濡�� ������ �������� �Ѹ��̻��� ����������. �ش���� �ο��� �����Ѵ�.
								{	
									g_CWaitRoomList.DeleteUser( l_sRoomInfoToWaitUserAX.nRoomNo, l_sRoomInfoToWaitUserAX.nRoomCnt );
								}
								else //���ӹ濡�� ������ �ٷ� �������� ���� �ٷ� �������..  
								{
									g_CWaitRoomList.DeleteRoom( l_sRoomInfoToWaitUserAX.nRoomNo );
                                   
								}

                                g_pCMainFrame->WaitRoomIndexRefresh();

                                // ���� �����ִ� ���� ���� ����
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserAX.nRoomNo );
								break;

							case 'R':
								memcpy( &l_sRoomInfoToWaitUserR, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserR) );
								nIndex += sizeof(l_sRoomInfoToWaitUserR);

								if ( l_sRoomInfoToWaitUserR.nRoomCnt > 0 ) { //�濡 �Ѹ��̻��� ����������.
									//�濡�� �Ѹ��� ����
									g_CWaitRoomList.DeleteUser( l_sRoomInfoToWaitUserR.nRoomNo, l_sRoomInfoToWaitUserR.nRoomCnt );
								}
								else {
									//�� ����.
									g_CWaitRoomList.DeleteRoom( l_sRoomInfoToWaitUserR.nRoomNo );

								}

								//���ǿ� ������ �ְ�.
								g_CUserList.AddUser( &l_sRoomInfoToWaitUserR.l_sUserInfo );

								g_pCMainFrame->m_nWaitUserCurCnt = UserSort();
								g_pCMainFrame->DeleteUserForScroll();
                                g_pCMainFrame->WaitRoomIndexRefresh();

                                // ���� �����ִ� ���� ���� ����
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserR.nRoomNo );

								break;
								

						}
						break;

					case NGSC_CREATEROOM:
						memcpy( &l_sMyRoomInfo, &szRealBufpos[nIndex], sizeof(l_sMyRoomInfo) );
						nIndex += sizeof(l_sMyRoomInfo);

						memcpy( &g_sRoomInfo, &l_sMyRoomInfo, sizeof(l_sMyRoomInfo) );


						//���� ���� ��������Ƿ� ���� �����̰� ���̴�.
						g_CGameUserList.BangJangDel();
						g_CGameUserList.m_GameUserInfo[0].m_bBBangJjang = TRUE;

						g_CGameUserList.SunDel();
						g_CGameUserList.m_GameUserInfo[0].m_bSun = TRUE;
	
						//�濡�� ���͵� ���Ⱑ �߻��ϱ� ������....���ӿ� ���κ��� ��� �ʱ�ȭ ��Ų��.
						SendMessage( g_hWnd, WM_GAMEMODESET, 0, 0 );
                        g_pCMainFrame->m_pGameChat->Init();
						break;

					case NGSC_ROOMINFO:
                        {
						    memset( g_ClickRoomUserInfo, 0x00, sizeof(g_ClickRoomUserInfo) );
						    memcpy( g_ClickRoomUserInfo, &szRealBufpos[nIndex], sizeof(g_ClickRoomUserInfo) );
						    nIndex += sizeof(g_ClickRoomUserInfo);

                            bool bExist = false;    // ���� ���� ������ ������ �ִ��� �˻�

                            // �� ������ ���ŵǾ ���õǾ� �ִ� ������ ���� ������ ���õ� ���� ������ �״�� �����ȴ�.
                            if ( g_pCMainFrame->m_nUserListTitleNum == 1 && g_pCMainFrame->m_nViewUserNo != -1 )  {
                                for ( i = 0; i < MAX_ROOMCNT; i++ ) {
                                    if ( g_ClickRoomUserInfo[i].szNick_Name[0] == NULL) continue;

                                    if ( g_ClickRoomUserInfo[i].nUserNo == g_pCMainFrame->m_nViewUserNo ) {
                                        bExist = true;
                                    }
                                }
                            }

                            // ó������ ���õ� ������ ���ų�, ���õ� ������ ���� ������ ������ �������� ���� ��쿡��
                            // ���õ� ���� ������ ���ٰ� ǥ���Ѵ�.
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
						//�����߿� ���� �������� ���̽��� TRUE�� �ؾ� �ٸ����ī�带 ������ �ִ�.
						//�������� 0 : �����, 1 : ��ŸƮ��ư�����, 2 : ������, 3 : ������
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

						//�������� �ִ´�. �ڽ��� ���� ��̰� 0���̱⶧���� i��°�� ��� �ش� ��ġ�� �ȴ�.
						for ( i = 1; i < MAX_ROOM_IN; i++ ) {

							//�ڽ��� ��ȣ ���� ���� ������ �ȴ�.
							nUserArrayNo = ( nRoomInUserNo + i ) % MAX_ROOM_IN;
							
							if ( l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.nUserNo > 0 ) {

								DTRACE("NGSC_ROOMIN - AddUser  UserNo = %d, UserNick = %s",
									l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.nUserNo, l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.szNick_Name );
								
								nSlotNo = g_CGameUserList.AddUser( &l_sRoomInUserSend[nUserArrayNo].l_sUserInfo, i );

								//������ �־��ش�.
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
									//���� ī�带 ���ش�.
									for ( j = 0; j < l_sRoomInUserSend[nUserArrayNo].nCardCnt; j++ ) {
#ifdef SHOWUSERCARD
                                        g_pCMainFrame->UserCardAdd( l_sRoomInUserSend[nUserArrayNo].nUserCard[j], nSlotNo, FALSE );
#else
										//������ ���̽��� ������ 1,2��ī��� �޸� 5-6���� �ո�, 7���� �޸��� �����ְ�
										//���̽��� ���� �ʾ����� ��� �޸��� �����ش�.
										if ( g_CGameUserList.m_GameUserInfo[nSlotNo].m_bChoioce &&
											 l_sRoomInUserSend[nUserArrayNo].nGameJoin == 1 )	//�߰��� ���³��� �ٸ� ����� �׾��ִ� ���(�����)�̸� ī�� �޸��� ������� �Ѵ�.
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

						//�濡�� ���͵� ���Ⱑ �߻��ϱ� ������....���ӿ� ���κ��� ��� �ʱ�ȭ ��Ų��.
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

						//sprintf( ptemp, "�� %s���� �����ϼ̽��ϴ�.", l_sUserInfo.szNick_Name );	
						sprintf( ptemp, "�� �Ҹ������� �����ϼ̽��ϴ�.");	
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 255 ));
						break;
						
					case NGSC_IPCHECK:
						g_bConnectingSpr = false;
						g_pCMainFrame->m_Message->PutMessage( "���� �����Ƿ� ���� �濡��", "������ �Ҽ� �����ϴ�." );
						break;

					case NGSC_ROOMOUT_OTHERSEND:
						memcpy( &l_sRoomOutOtherSend, &szRealBufpos[nIndex], sizeof(l_sRoomOutOtherSend) );
						nIndex += sizeof(l_sRoomOutOtherSend);


						//������ �ٲ������ 
						if ( l_sRoomOutOtherSend.nNewBbangJjnag != 0 ) {
							g_CGameUserList.NewBangJangSet( l_sRoomOutOtherSend.nNewBbangJjnag );
						}

						//���� ����� ���� ��������.
						if ( l_sRoomOutOtherSend.nNewRoomMaster != 0 ) {
							g_CGameUserList.NewSunSet( l_sRoomOutOtherSend.nNewRoomMaster );
						}


						g_sRoomInfo.nCurCnt = l_sRoomOutOtherSend.nRoomCurCnt;

						g_pCMainFrame->PlaySound(g_Sound[GS_ROOMEXIT]);


						nSlotNo = g_CGameUserList.FindUser( l_sRoomOutOtherSend.nUserNo );
						//sprintf( ptemp, "�� %s���� �����̽��ϴ�.", g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.szNick_Name );					
						sprintf( ptemp, "�� �Ҹ������� �����̽��ϴ�." );					
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
						
						//���ƿ� ������ ������ ã�´�.
						nSlotNo = g_CGameUserList.FindUser( nUniqueUserNo );

						g_CGameUserList.m_GameUserInfo[nSlotNo].m_bChoioce = TRUE;

						//������ �������ִ� ī�带 ��������.
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

						//���ƿ� ������ ������ ã�´�.
						nSlotNo = g_CGameUserList.FindUser( nUniqueUserNo );
						g_CGameUserList.m_GameUserInfo[nSlotNo].m_nCardCnt--;
						break;

					case NGSC_GETONECARD:
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_GETONECARD \n");
						memcpy( &l_sGetOneCard, &szRealBufpos[nIndex], sizeof(l_sGetOneCard) );
						nIndex += sizeof(l_sGetOneCard);

						//ó�� ī�� ���̽��� �ƴϰ� 4��°���� �����̸�.
						if ( l_sGetOneCard.cUserSelect != -1 ) {

							//�������� ��, �� ���� ǥ��
							nSlotNo = g_CGameUserList.FindUser(  l_sGetOneCard.nCallUserUniNo );

                            g_pCMainFrame->SetPanMoney( nSlotNo, l_sGetOneCard.biRoomCurMoney );

							g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sGetOneCard.biCallUserMoney;

							g_pCMainFrame->UserSelectView( nSlotNo, l_sGetOneCard.cUserSelect, TRUE );						

#ifndef SHOWUSERCARD
							//���� ������ Die�� �Ͽ����� Die�� ������ ī�带 ���´�.
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

						//���ο� ���� �Ӹ�.
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

									//�ڽ��� �ٸ� ��Ŷ���� �´�. ���ڽ��� ��� ������ �����ϱ⶧��.
                                    if ( nSlotNo != 0 ) {
                                        memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szJokboCard, l_sGetOneCard.szJokboCard[i], sizeof(l_sGetOneCard.szJokboCard[i]) );
                                    }
								}
								else { 
									//�����̸�..ī�带 �����ʿ��ٰ� �־������.
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
                        // �� ������ ������ ���� �ٲ� ī�� ������ ��� �������� �ѷ��ش�.
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
                        // ��ħ, ����, ������ �Ǿ����� �˸��� ��Ŷ.
                        // ��ħ/����/������ ���۵� ������ �� ���� ���޵ȴ�.
                        //-------------------------------------------------
						g_pCMainFrame->m_ChonCurMousePointYOnHidden = -1;
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CHANGEOKALL \n");
						memset( &l_sChangeOkAll, 0x00, sizeof(l_sChangeOkAll) );
						memcpy( &l_sChangeOkAll, &szRealBufpos[nIndex], sizeof(l_sChangeOkAll) );
						nIndex += sizeof(l_sChangeOkAll);


						//�������� ��, �� ���� ǥ��
						nSlotNo = g_CGameUserList.FindUser( l_sChangeOkAll.nBeforeGameUserUniNo );
						g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sChangeOkAll.biBeforeGameUserMoney;

						g_pCMainFrame->UserSelectView( nSlotNo, l_sChangeOkAll.cBeforeUserSelect, TRUE );
						g_pCMainFrame->SetPanMoney( nSlotNo, l_sChangeOkAll.biRoomCurMoney);
						

						//////////////////////////////////////////////////////////////////////////
						//l_sChangeOkAll.nRound;  0: ��ħ, 1 : ����, 2 : ����
   						//////////////////////////////////////////////////////////////////////////
                        // ��ħ, ����, ���� ���� ����

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
                        // ī�� �ٲ� ���� ���� ����
                        //-------------------------
                       	TRACE("m_sChgCardInfo[i].l_sUserCardChange.nChangeCardCnt ==>  NGSC_CHANGEOKUSER \n");

						//ī�带 �ٲ����� ����
						nUniqueUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
						MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
						nIndex += 4;

						//ī�带 �ٲ����� ���� ����.
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
						//���Ӱ����ʱ�ȭ.
						//////////////////////////////////////////////////////////////////////////
                        {
                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);

                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_STARTGAME - CreateEvent() ���� �߻� : �����ڵ� %ld", g_szUser_ID, dwErr );
                            }

                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);        // �̺�Ʈ�� ���ȣ���·� �����.

							DTRACE2("[%s]NGSC_STARTGAME - �̺�Ʈ �����Ѵ�.", g_szUser_ID);

                            g_pCMainFrame->m_bGameInit = TRUE;

                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );

                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] ���� �ʱ�ȭ ����", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "���� �ʱ�ȭ �����Ͽ����ϴ�.", "Code - 1001", true );    
		                    }
                        }

						nRoomMasterArrayNo = g_CGameUserList.FindUser( l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo );

						//�����̳��̳��� �ʱ�ȭ�� �ϱ⶧����..�Ӹ����ش�.
						//���ο� ���� �Ӹ�.
						g_CGameUserList.NewSunSet( l_sGameStart.l_sStartUserCard.nRoomMasterUniqNo );

						//���ο� ������ �Ӹ�.
						g_CGameUserList.NewBangJangSet( l_sGameStart.l_sStartUserCard.nBbanJjangUniqNo );

						//ī�� �ʱ�ȭ�� ��� ��Ų��.
						for ( i = 0; i < MAX_ROOM_IN; i++  ) {
							//�ǵ� ����. �б�����.
                            if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo <= 0 ) {
                                continue; 
                            }

							g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.biUserMoney -= g_sRoomInfo.biRoomMoney;
						}

						for ( i = 0; i < nInitCardCnt; i++ ) {
							for ( j = 0; j < MAX_ROOM_IN; j++ ) {
								//���������� ������ �ȴ�.
								nUserArrayNo = ( nRoomMasterArrayNo + j + 1) % MAX_ROOM_IN;

                                if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_sUserInfo.nUserNo <= 0 ) {
                                    continue;
                                }
#ifdef SHOWUSERCARD
                                g_pCMainFrame->UserCardAdd( l_sGameStart.l_sStartUserCard.byCardNo[i], nUserArrayNo );
#else
								//�ڱ� �ڽ��� �ƴϸ�.
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

						strcpy( ptemp, "�� ���ӽ��� ��" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						strcpy( ptemp, "�� ����Ƚ�� : 1-2-2-3��" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						nSlotNo = g_CGameUserList.FindUser( l_sGameStart.l_sSetGameCurUser.nCurGameUserUniNo );

                        // �ʱ� ī�尡 �� �̵��� ������ ������ ����ǵ��� ���� ������ �ӽ� ��ҿ� �����صΰ�,
                        // �÷��׸� �����Ѵ�.

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
                                DTRACE2("[%s] NGSC_STARTGAME - CreateEvent() ���� �߻� : �����ڵ� %ld", g_szUser_ID, dwErr );
                            }

                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);

							DTRACE2("[%s]NGSC_STARTGAME - �̺�Ʈ �����Ѵ�.", g_szUser_ID);

                            g_pCMainFrame->m_bGameInit = TRUE;

                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );

                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] ���� �ʱ�ȭ ����", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "���� �ʱ�ȭ �����Ͽ����ϴ�.", "Code - 1001", true );    
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
								
								//��Ŷ�� ����� �ִ� ������� �־��ָ� �ȴ�.
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

						strcpy( ptemp, "�� ���ӽ��� ��" );
                        g_pCMainFrame->SetGameChatText( ptemp, RGB( 255, 255, 51 ));

						strcpy( ptemp, "�� ����Ƚ�� : 1-2-2-3��" );
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

						//�������� ��, �� ���� ǥ��
						nSlotNo = g_CGameUserList.FindUser( l_sSetGameCurUser.nBeforeGameUserUniNo );

                        // ����ȭ�� ���ؼ� ���� ������ ������ �ӽ� ����
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


						//�������� ��, �� ���� ǥ��
						nSlotNo = g_CGameUserList.FindUser( l_sEndGame.nCallUserUniNo ); 


						g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sEndGame.biCallGameUserMoney;


						g_pCMainFrame->UserSelectView( nSlotNo, l_sEndGame.cUserSelect, TRUE);

#ifndef SHOWUSERCARD
						//���� ������ Die�� �Ͽ����� Die�� ������ ī�带 ���´�.
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

								//������ ���а���
                                if ( g_nWinnerSlotNo == nSlotNo ) { //�����̸�.
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nWin_Num++;
                                }
                                else {
                                    g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nLose_Num++;
                                }

								//�ڽ��� �ٸ� ��Ŷ���� �´�.
								//�ӽÿ� �����س��� ī�带 �����ϸ� �����ֱ����ؼ�.
								memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szTempJokboCard, l_sEndGame.szJokboCard[i], sizeof(l_sEndGame.szJokboCard[i]) );

								//������ �ش��ϴ� ī�带 ��Ƴ��´�.
								memcpy( g_CGameUserList.m_GameUserInfo[nSlotNo].m_szFixJokboCard, l_sEndGame.szFixJokboCard[i], sizeof(l_sEndGame.szFixJokboCard[i]) );
							}
						}

                        // ��� �����̸� ��� �������� ��ȣ�� �����ϰ�, �÷��׸� �����Ѵ�.

                        // ��� ��� ����� �� �ʱ�ȭ
                        memset( g_pCMainFrame->m_nDrawWinner, 0x00, sizeof( g_pCMainFrame->m_nDrawWinner ));

                        if ( l_sEndGame.bDrawGame == TRUE ) {
                            for ( i = 0; i < MAX_ROOM_IN; i++ ) {
                                if ( l_sEndGame.nDrawWinnerUniq[i] != -1 ) {
                                    nSlotNo = g_CGameUserList.FindUser( l_sEndGame.nDrawWinnerUniq[i] );
                                    g_pCMainFrame->m_nDrawWinner[nSlotNo] = 1;  // ��� ����� ����
                                }
                            }

                            g_pCMainFrame->m_bDrawGame = TRUE;
                        }

						memset( g_szResultCmt, 0x00, sizeof(g_szResultCmt) );
                        g_pCMainFrame->SetGameChatText( "��---���Ӱ��----��", RGB( 255, 255, 51) );
						g_pCMainFrame->m_bEndding = TRUE;
                        g_pCMainFrame->ClearTimeMode();
						break;


					case NGSC_MYWINGAME:
						memcpy( &l_sMyWinGame, &szRealBufpos[nIndex], sizeof(l_sMyWinGame) );


						nIndex += sizeof(l_sMyWinGame);

						//�������� ��, �� ���� ǥ��
						nSlotNo = g_CGameUserList.FindUser( l_sMyWinGame.nCallUserUniNo ); 

						g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.biUserMoney = l_sMyWinGame.biCallGameUserMoney;

						g_pCMainFrame->UserSelectView( nSlotNo, l_sMyWinGame.cUserSelect, TRUE );

#ifndef SHOWUSERCARD
						//���� ������ Die�� �Ͽ����� Die�� ������ ī�带 ���´�.
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
						strcpy( g_szResultCmt, "��ǽ�" );
						
						//������ �� �� ����.����.
						for ( i = 0; i < MAX_ROOM_IN; i++ ) {

							if ( l_sMyWinGame.l_nUserUniqNo[i] > 0 ) {

								nSlotNo = g_CGameUserList.FindUser( l_sMyWinGame.l_nUserUniqNo[i] );

								g_CGameUserList.m_GameUserInfo[nSlotNo].m_biRealUserMoney = l_sMyWinGame.l_biUserMoney[i];
                                g_CGameUserList.m_GameUserInfo[nSlotNo].m_biPureUserWinLoseMoney = l_sMyWinGame.l_biPureUserWinLoseMoney[i];

								//������ ���а���
                                if ( g_nWinnerSlotNo == nSlotNo ) { //�����̸�.
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nWin_Num++;
                                }
                                else {
                                    g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.nLose_Num++;
                                }

							}

						}


                        g_pCMainFrame->SetGameChatText( "��----��ǽ�-----��", RGB( 180, 180, 180 ) );

						g_bMyWin = TRUE;
						g_pCMainFrame->m_bEnddingShow = TRUE;
                        g_pCMainFrame->m_bMoveHiddenCard = FALSE;
						g_pCMainFrame->m_ChonHiddenMode = FALSE;
                        g_pCMainFrame->ClearTimeMode();
						break;


					case NGSC_NAGARIGAME:
						
						memset( g_szResultCmt, 0x00, sizeof(g_szResultCmt) );
						strcpy( g_szResultCmt, "����������" );
						g_pCMainFrame->m_bEnddingShow = TRUE;
                        g_pCMainFrame->m_bMoveHiddenCard = FALSE;
						g_pCMainFrame->m_ChonHiddenMode = FALSE;
                        g_pCMainFrame->ClearTimeMode();
						break;
						

					case NGSC_STARTBTN_ONOFF:

						//�����߿� �θ��� �ְ� �Ѹ��� �����ؼ� ���â�� ����߿� �Ѹ��� �����»�Ȳ
						//���â ����߿� �̱���� ������ ������ �����ϸ� ������ �濡 ����ڸ� �ִ°����� �����Ѵ�.
						//�ֳ��ϸ� ���� �ѳ��� ������̱⶧���̴�.
						//�׷��� ���â�� ���ִ� ���� ��ŸƮ�� ���ƿ´�. �̶��� ����ؼ� �׳� ������ �ʱ�ȭ ����.
						//g_pCMainFrame->GameInit();
                        {

                            g_pCMainFrame->m_hGameInitEvent	= CreateEvent(NULL, false, true, NULL);


                            if ( g_pCMainFrame->m_hGameInitEvent == NULL ) {
                                DWORD dwErr = GetLastError();
                                DTRACE2("[%s] NGSC_STARTBTN_ONOFF - CreateEvent() ���� �߻� : �����ڵ� %ld", g_szUser_ID, dwErr );
                            }


                            ResetEvent(g_pCMainFrame->m_hGameInitEvent);

							DTRACE2("[%s]NGSC_STARTBTN_ONOFF - �̺�Ʈ �����Ѵ�.", g_szUser_ID);

                            g_pCMainFrame->m_bGameInit = TRUE;

                            DWORD dwRe = WaitForSingleObject( g_pCMainFrame->m_hGameInitEvent, 20000 );
		                    
                            if ( dwRe == WAIT_TIMEOUT ) {
                                DTRACE2("[%s] ���� �ʱ�ȭ ����", g_szUser_ID );
			                    g_pCMainFrame->m_Message->PutMessage( "���� �ʱ�ȭ �����Ͽ����ϴ�.", "Code - 1002", true );    
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

                        //üũ�� ���� FALSE �� �����. ( NEXT �� PREV �� ������ ��츦 ��� )
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

                        // �� ���� ��Ŷ�� ���޵Ǹ� �ӽ� ���� ��ҿ� ������ �����ϰ�, �÷��׸� �Ҵ�.
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

							//��Ŷ�� ����� �ִ� ������� �־��ָ� �ȴ�.
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



//��������Ʈ�� �̻��� ���������Ƿ� �����Ѵ�.
int UserSort( void )
{
	int nWaitUserNo = 0 ;       // ����� ��
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

    g_pCMainFrame->m_SCB_WaitUser->SetElem( nWaitUserNo );  // ����� ����Ʈ ��ũ�� �� ����(��ü ����� ��)

	return nWaitUserNo;
}


