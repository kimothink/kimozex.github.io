#include "StdAfx.h"

#include "MainFrm.h"
#include "NGClientControl.h"
#include "GoStopEngine.h"
#include "UserList.h"
#include "dtrace.h"
#include "AlarmMsg.h"


int g_nUserNo = 0;
int g_nMyUserNo = 0;
int nWaitUserCnt = 0;

CUserList g_CUserList;

sPKWaitInputCS g_sPKWaitInputCS;
sIMAGEDATA g_sImageData[MAX_ROOM_IN];//�� ������ �����ִ� �̹���
BYTE g_szRecvCardPack[MAX_RECVCARD_CNT];


extern bool g_bPaketThredOn;
extern bool g_bUserCardRecvThreadOn;
extern char g_cServerDivi;
extern sLogIn g_sLogIn;
extern CGoStopEngine *g_pGoStop;
extern int g_CurUser;
extern int g_MyUser;
extern int g_nCardNoArr[5];
extern bool g_bUserCardRecv;
extern bool g_ServerSend;
extern char g_szCSServerIp[MAX_IP_LEN];
extern int g_CSPort;
extern sClientEnviro g_sClientEnviro;
extern CGameWaitRoomList g_CGameWaitRoomList;
extern sGamingExitUser g_sGamingExitUser[MAX_ROOM_IN];
extern int g_nViewPageNo; // ���� ���� �ִ� ������ �ѹ�
extern char g_szUser_ID[MAX_ID_LENGTH];
extern CGameUserList g_CGameUserList;
extern BigInt g_biRoomMoney;
extern BigInt g_biSystemMoney;
extern int g_nGameCnt;
extern int g_nDoubleCardCnt;
extern int g_nThreeCardCnt;
extern sServrMashineList g_sServrMashineList[MASHINE_CNT];
extern char g_szRoomName[MAX_ROOM_NAME];
extern int g_nRoomNo;
extern sGameWaitInfo g_sGameWaitInfo[MAX_GAMEVIEWWAIT];
extern char g_szUserImage[MAX_ROOM_IN][MAX_IMAGE_LEN];
extern bool g_bGameUserListSafeMode;
extern bool g_bConnectingSpr;
extern char g_szClassName[20];
extern CMainFrame *g_pCMainFrame;
extern char g_szAvaUrl[MAX_AVAURLLEN];
extern char g_szGameGubun[5];
extern sPaintAvatar g_sPaintAvatar;
extern sAvatarPlayStr g_sAvatarPlayStr;
extern ush_int g_szGameJong;
extern ush_int g_szGameServerChannel;
extern sUserInfo g_ClickRoomUserInfo[MAX_ROOM_IN];
extern CNetworkSock *g_pClient;
extern int g_nRoomCurCnt;
extern ush_int g_nServiceDiv;

int UserSort( void )
{
	int nWaitUserNo = 0 ; // ��ٸ��� �����

	sUserInfo l_sTempUserInfo;

	memset( &l_sTempUserInfo , 0x00 , sizeof(sUserInfo));

	for( int i = 0 ; i < MAX_CHANNELPER; i ++ )
	{
		if( g_CUserList.m_pUserInfo[i].m_bUser == FALSE )
		{
			if( g_CUserList.m_pUserInfo[ i + 1 ].m_bUser == TRUE )
			{
				l_sTempUserInfo = g_CUserList.m_pUserInfo[i + 1].m_sUser;
				g_CUserList.DeleteUser( i + 1 );
				g_CUserList.AddUser( &l_sTempUserInfo );
			}			
		}
		
		if( g_CUserList.m_pUserInfo[i].m_bUser == TRUE )
		{
			nWaitUserNo++;
		}
	}
	return nWaitUserNo;
}




void SunSetInit()
{
	g_pCMainFrame->m_bSunSet = TRUE;

	for ( int i = 0; i < MAX_SUNCARD_CNT; i++ ) {
		g_pCMainFrame->m_szSunPackPos[i].NewPos = g_pCMainFrame->m_szSunPackPoint[i];
		g_pCMainFrame->m_szSunPackPos[i].StartPos = g_pCMainFrame->l_CardPackPos;
		g_pCMainFrame->m_szSunPackPos[i].Pos = g_pCMainFrame->l_CardPackPos;
		g_pCMainFrame->m_szSunPackPos[i].bSetAction = FALSE;
		g_pCMainFrame->m_szSunPackPos[i].nAniCount = 0;
		g_pCMainFrame->m_szSunPackPos[i].nAniMaxCount = -1;
		g_pCMainFrame->m_szSunPackPos[i].type = FLOOR;
		g_pCMainFrame->m_szSunPackPos[i].bGetOneCardMode = FALSE;
	}
}



int g_nRecvShakeCardArr[5];

void GetErrorMessage(BYTE nErrorCode)
{
	switch(nErrorCode) {
		case ERROR_ACCESS_DBFAIL: 
			g_pCMainFrame->l_Message->PutMessage( "����Ÿ���̽� ������ �����Ͽ����ϴ�.", "Code - 100", true );
			break;

		case ERROR_PK:
			break;

		case ERROR_LIMIT_USER:
			g_pCMainFrame->l_Message->PutMessage( "������ ���� �ʹ� �����ϴ�.", "Code - 102" );
			break;

		case ERROR_NOCREATEROOM:
			g_pCMainFrame->l_Message->PutMessage( "���̻� ���� �����Ҽ� �����ϴ�.", "Code - 103" );
			break;

		case ERROR_NOROOM:
			g_pCMainFrame->l_Message->PutMessage( "�������� �ʴ� ���Դϴ�.", "Code - 104" );
			break;
		
		case ERROR_ROOMFULL:
			g_pCMainFrame->l_Message->PutMessage( "�濡 �ο��� ��� á���ϴ�.", "Code - 105" );
			break;
		
		case ERROR_NOTPWD:
			g_pCMainFrame->l_Message->PutMessage( "��й�ȣ�� Ʋ�Ƚ��ϴ�.", "Code - 106" );
			break;

		case ERROR_NOQUICK:
			//�ٷ� CreateGame�� ������.
			PostMessage( g_hWnd, WM_OUICKFAILCREATEROOM, 0, 0 );
			break;

		case ERROR_NOTVER:
			g_pCMainFrame->l_Message->PutMessage( "������ �ٸ��ϴ�. ����� ���μ�ġ�ϼ���!!!", "Code - 139", TRUE );
			break;

		case ERROR_MULTILOGIN:
			g_pCMainFrame->l_Message->PutMessage( "�̹� ������� ���̵��Դϴ�.!!!", "Code - 140", TRUE );
			break;

		case ERROR_NOTOPTION:
			g_pCMainFrame->l_Message->PutMessage( "����,�ʴ�ź� �Ǵ� ���ӻ����Դϴ�.", "Code - 137" );
			break;
	}

}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	int nInput_Q_BufSize;
	int nRead = 0;
	int nIndex = 0;
	int nnWaitUserCnt = 0;
    int nUserNo = 0;
	int nRoomCnt = 0;
	int i = 0;
    int j = 0;
	int nWaitUserCnt = 0;
	int nRoomNo = 0;
    int nDeleteUserNo;
    int nMemoLen;
	int nRe = 0;
	int nEarTalkUserNo;
	int nChatLen;
    int nUserListNo = 0;
	int nCardCnt = 0;
	int nRecvUserCard = -1;
	int nUserSlot = -1;
	int nGoStop = -1;
	int nNineCard = -1;
	int nShakeCard = -1;
	int nRoomSeed = 0;
	int nGamerUserNo = -1;
	int nUniQNo = -1;
	int nMissionType = 0;
	int nMissionMultiply = 0;
	int nChannelTotCnt = 0;
	int nPrizeNo = 0;
	int nExitUserSlot = 0;
	int nViewNo = -1;
	int l_nNoRoom = 0; // ���� �ϳ��� ������ �߰� ��Ų��. 2 �� �������� 0 �� ����鼭 ���� �� ������ ������.
	int nChoiceNo = 0;
	int nMasterSlotNo = 0;
	int nRoomInUserNo = 0;
	int nUserArrayNo = 0;
	int nSlotNo = 0;
	int nSenNo = 0;
	int nNextNo = 0;
	int nBbangJjangUniqNo = 0;
	int nTempNo = 0;
    int UserArrayNo;
	int nUserVoice = 0;

    char szRecvBuf[MAX_BUFFER_SIZE];
	char *szRealBufpos = NULL;
    char cUserAddMius;
    char cWaitInfoDivi;
    char ptemp[255];
	char szChatMsg[MAX_CHATMSG];
	char szInviteMsg[MAX_CHATMSG];
	char szSendNickName[MAX_NICK_LENGTH];
	char szMemoMsg[MAX_CHATMSG];
	char szAvaUrl[MAX_AVAURLLEN];
	char szGameGubun[5];
    char szAllCmt[1000];
    char cSex;

	BOOL bIsImage = FALSE;
    bool bExit;

    BigInt biRoomMoney = 0;

	sUserInfo szWaitUserInfo[MAX_CHANNELPER];
    sSCHeaderPkt l_sSCHeaderPkt;
    sRoomInfoClient *l_sRoomInfoClient;
	sWaitUserInfo l_szAvaWaitUserInfo[MAX_CHANNELPER];
	sUserInfo l_sAddUserInfo;
	sOnCreateRoom l_sOnCreateRoom;
	sRoomInfoToWaitUserAX l_sRoomInfoToWaitUserAX;
	sRoomInfoToWaitUserR l_sRoomInfoToWaitUserR;
	sRoomOutOtherSend l_sRoomOutOtherSend;
	sRoomInUserSend l_sRoomInUserSend[MAX_ROOM_IN];
	sRoomInOtherSend l_sRoomInOtherSend;
	sRoomInfo l_sMyRoomInfo;
	sGamingExitUser l_sGamingExitUser;
	sBroadServerInfo l_sBroadServerInfo;
	sGameWaitInfo l_sGameWaitInfo[MAX_GAMEVIEWWAIT];
	sIsLogIn l_sIsLogIn;
	sUserList l_sUserList;
	sUserInvite l_sUserInvite;
	sNotice l_sNotice;
	sRoomInAdd l_sRoomInAdd;
	sRealGameStart l_sRealGameStart;
	sUserChange l_sUserChange;
    
    BYTE bCmd;
    BYTE nCardNo = 0;
	BYTE bErrorCode = l_sSCHeaderPkt.cErrorCode;

	CNetworkSock *pClientSocket = (CNetworkSock *)lpParameter;

    memset( &l_sAddUserInfo, 0x00, sizeof(l_sAddUserInfo) );
	memset( g_sGamingExitUser, 0x00, sizeof(g_sGamingExitUser) );

	
	

	//gloval �ʱ�ȭ.

	while( g_bPaketThredOn )	
	{
		if( pClientSocket->m_input.GetFromQ(szRecvBuf, &nInput_Q_BufSize) )
		{
			g_ServerSend = true;	//�������� ������ �޾����� �ٽ� ������ �ִ»��·� �����.

			nIndex = 0;
			nRead = NGNCSDeframeHeader(szRecvBuf, &l_sSCHeaderPkt);
			
			bCmd = (BYTE)l_sSCHeaderPkt.cCmd;
			bErrorCode = l_sSCHeaderPkt.cErrorCode;

			szRealBufpos = szRecvBuf + nRead;
			
			if ( bErrorCode != ERROR_NOT ) {
				GetErrorMessage(bErrorCode);
				nRead = sizeof(sSCHeaderPkt);
			}
			else {
				switch(bCmd)
				{
					case NGSC_HEADERSEND:
						//DTRACE("Packet Recv - NGSC_HEADERSEND");
						sPKFirst l_sPKFirst;
						memcpy( &l_sPKFirst, szRealBufpos, sizeof(l_sPKFirst) );

						g_szHeader[0] = l_sPKFirst.szHeader[0];
						g_szHeader[1] = l_sPKFirst.szHeader[1];
						g_nUserNo = l_sPKFirst.nUserNo;

						nIndex += sizeof(sPKFirst);

						//DTRACE("NGSC_HEADERSEND - g_szHeader[0] :%d, g_szHeader[1] : %d", l_sPKFirst.szHeader[0], l_sPKFirst.szHeader[1]  );					

						switch(g_cServerDivi)
						{
							case '0':

								if ( !pClientSocket->LogInServer( g_szUser_ID ) ) {
									g_pCMainFrame->l_Message->PutMessage( "�α��� ����", "Code - 109", true );
								}
								break;

							case '1':

								//DTRACE("ó��");
								break;

							case '2':

								pClientSocket->GetUserInfo( g_szUser_ID, g_szGameServerChannel );
								break;
						}
						break;	

					case NGSC_GETUSERINFO:

						memcpy( &l_sUserList, &szRealBufpos[nIndex], sizeof(l_sUserList) );
						nIndex += sizeof(l_sUserList);

						memcpy( &g_sLogIn.l_sUserList, &l_sUserList, sizeof(l_sUserList) );
						memset( &g_sPKWaitInputCS, 0x00, sizeof(g_sPKWaitInputCS) );
						memcpy( &g_sPKWaitInputCS.l_sUserInfo, &g_sLogIn.l_sUserList, sizeof(g_sLogIn.l_sUserList) );

						//�α��μ������� ���������� �ƴ϶� ���Ӽ������� ���� �����ѹ��� �����ϱ� ����...
						g_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.nUserNo = g_nUserNo;

						g_sPKWaitInputCS.l_sOption.bInvite = g_sClientEnviro.bOption[0];
						g_sPKWaitInputCS.l_sOption.bEarTalk = g_sClientEnviro.bOption[1];
						g_sPKWaitInputCS.l_sOption.bMemoRecv = g_sClientEnviro.bOption[3];
						g_sPKWaitInputCS.l_sOption.nVoice = g_sClientEnviro.nVoice;

						strcpy( g_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.szPreAvatar, g_szAvaUrl );
						strcpy( g_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.szGameGubun, g_szGameGubun );

						if(!pClientSocket->WaitInput(&g_sPKWaitInputCS))
						{
							g_pCMainFrame->l_Message->PutMessage( "�������� �������� ����", "Code - 110" );

							//������ ���� �ٽ� ä�μ����� ����.
						}

						//���� �濡�� ����Ҷ��� ������ ��Ƴ��´�...
						g_CGameUserList.AddGameUser( &g_sPKWaitInputCS.l_sUserInfo.l_sUserInfo, g_sClientEnviro.nVoice );


						//�ڽ��� ������ ��������Ʈ�� �ִ´�.
						nUserListNo = -1;
						nUserListNo = g_CUserList.AddUser( &g_sPKWaitInputCS.l_sUserInfo.l_sUserInfo );
						if ( nUserListNo >= 0 ) {
							//�����ֱ�.
							
							g_pGoStop->m_pFloor.AddMyGamer( 0, g_sPKWaitInputCS.l_sUserInfo.l_sUserInfo.szPreAvatar );
						}
						else {
							g_pCMainFrame->l_Message->PutMessage( "������ �ʹ� �����ϴ�.", "Code - 111" );
							//������ ���� �ٽ� ä�μ����� ����.
							return 0;
						}


						break;


					case NGSC_LOGIN:

						//DTRACE("Packet Recv - NGSC_LOGIN");

						memset( &l_sIsLogIn, 0x00, sizeof(l_sIsLogIn) );
						memcpy( &l_sIsLogIn, &szRealBufpos[nIndex], sizeof(l_sIsLogIn) );

						nIndex += sizeof(l_sIsLogIn);

						memset( &l_sNotice, 0x00, sizeof(l_sNotice) );
						memcpy( &l_sNotice, &szRealBufpos[nIndex], sizeof(l_sNotice) );
						nIndex += sizeof(l_sNotice);


                        {
                            // ��ũ�� �������� ����
                            if ( l_sNotice.m_Notice1[0] != NULL )
                                g_pCMainFrame->m_pScrollNotice = new CScrollText( l_sNotice.m_Notice1, RGB(255, 255, 255), 705, 118, 995, 133 );
                        }


						pClientSocket->Destroy();
						g_cServerDivi = '2';
						
						//Counter Server�� ����.
						pClientSocket->InitSocket( g_hWnd, l_sIsLogIn.szGameServerIp, l_sIsLogIn.nGameServerPort );
						g_ServerSend = false;
						break;

						break;

					case NGSC_GETSERVERLIST:
						
						memcpy( g_sServrMashineList, &szRealBufpos[nIndex], sizeof(g_sServrMashineList) );
						nIndex += sizeof(g_sServrMashineList);

						memset( &g_sAvatarPlayStr, 0x00, sizeof(g_sAvatarPlayStr) );
						g_sAvatarPlayStr.nSlot = -1;
						strcpy( g_sAvatarPlayStr.szAvaUrl, g_szAvaUrl );
						strcpy( g_sAvatarPlayStr.szGameGubun, g_szGameGubun );
						g_sAvatarPlayStr.cSex = g_sLogIn.l_sUserList.l_sUserInfo.cSex;

						PostMessage( g_hWnd, WM_AVATARPLAY, 0, 0  );

						g_pGoStop->ChannelModeSet();

						g_bConnectingSpr = false;

						break;
						
					case NGCSC_BROADSERVERINFO:
						memset( &l_sBroadServerInfo, 0x00, sizeof(l_sBroadServerInfo) );
						memcpy( &l_sBroadServerInfo, &szRealBufpos[nIndex], sizeof(l_sBroadServerInfo) );

						g_sServrMashineList[l_sBroadServerInfo.nGSNo].nCurCnt[l_sBroadServerInfo.nChannel] = l_sBroadServerInfo.nCurCnt;

						break;

					case NGSC_WAITINPUT:

						//�濡�� �������͵� ���� Ÿ�� ������....������ 0�̴�.
						g_nGameCnt = 0;

						//DTRACE("Packet Recv - NGSC_WAITINPUT");

						nUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += sizeof(int);

						nRoomCnt = 	MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						//�� ������ �ʱ�ȭ ��Ų��.
						memset( &g_CGameWaitRoomList.m_sRoomStateInfo, 0x00, sizeof(g_CGameWaitRoomList.m_sRoomStateInfo) );
						memset( &g_CUserList.m_pUserInfo , 0x00 , sizeof(g_CUserList.m_pUserInfo));

						l_sRoomInfoClient = new sRoomInfoClient[nRoomCnt];
						memset( l_sRoomInfoClient, 0x00, sizeof(sRoomInfoClient) * nRoomCnt );

						g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.bInvite = g_sClientEnviro.bOption[0];

						g_CUserList.AddUser( &g_CGameUserList.m_GameUserInfo[0].m_sUserInfo );

						//������.				
						for ( i = 0; i < nRoomCnt; i++ ) {
							memcpy( &l_sRoomInfoClient[i].l_sRoomInfo, &szRealBufpos[nIndex], sizeof(sRoomInfo) );
							nIndex += sizeof(sRoomInfo);

							//�� ������ �޴´�.
                            nViewNo = l_sRoomInfoClient[i].l_sRoomInfo.nRoomNo;
							memcpy( &g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo, &l_sRoomInfoClient[i].l_sRoomInfo , sizeof(l_sRoomInfoClient[i].l_sRoomInfo));
						}

                        
						nWaitUserCnt = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						memset( l_szAvaWaitUserInfo, 0x00, sizeof(l_szAvaWaitUserInfo) );
						memcpy( l_szAvaWaitUserInfo, &szRealBufpos[nIndex],  sizeof(sWaitUserInfo) * nWaitUserCnt );

						nIndex += sizeof(sWaitUserInfo) * nWaitUserCnt;

						g_pCMainFrame->l_nWaitUserCurCnt = nWaitUserCnt + 1;    // + 1 �� ����(�ڱ��ڽ�)����

                        g_pCMainFrame->WaitUserScrollBarRefresh( nWaitUserCnt + 1 );           // +1�� ���� �ڽ�

						//���� ������ ����� ������ ��������Ʈ�� �ִ´�.
						for ( i = 0;  i < nWaitUserCnt; i++ ) {

							szWaitUserInfo[i].cSex = l_szAvaWaitUserInfo[i].cSex;
							szWaitUserInfo[i].fWinRate = l_szAvaWaitUserInfo[i].fWinRate;
							szWaitUserInfo[i].biMaxMoney = l_szAvaWaitUserInfo[i].biMaxMoney;
							szWaitUserInfo[i].biUserMoney = l_szAvaWaitUserInfo[i].biUserMoney;
							szWaitUserInfo[i].nChannel = l_szAvaWaitUserInfo[i].nChannel;
							szWaitUserInfo[i].nClass = l_szAvaWaitUserInfo[i].nClass;
							szWaitUserInfo[i].nCut_Time = l_szAvaWaitUserInfo[i].nCut_Time;
							szWaitUserInfo[i].nDraw_Num = l_szAvaWaitUserInfo[i].nDraw_Num;
							szWaitUserInfo[i].nLose_Num = l_szAvaWaitUserInfo[i].nLose_Num;
							szWaitUserInfo[i].nUserNo = l_szAvaWaitUserInfo[i].nUserNo;
							szWaitUserInfo[i].nWin_Num = l_szAvaWaitUserInfo[i].nWin_Num;
							szWaitUserInfo[i].bInvite = l_szAvaWaitUserInfo[i].bInvite;
							strcpy( szWaitUserInfo[i].szNick_Name, l_szAvaWaitUserInfo[i].szNick_Name );
							memset( szWaitUserInfo[i].szPreAvatar, 0x00, MAX_AVAURLLEN );

							if ( g_CUserList.AddUser( &szWaitUserInfo[i] ) < 0 ) {
								g_pCMainFrame->l_Message->PutMessage( "������ �ʹ� �����ϴ�.", "Code - 112" );
								//������ ���� �ٽ� ä�μ����� ����.

								return 0;
							}
						}

						// ù��° ���� ������ �� ���� ����Ʈ �س��´�.
						if( g_CGameWaitRoomList.m_sRoomStateInfo[0].l_sRoomInfo.nCurCnt >= 1 )
						{
							g_pCMainFrame->l_nViewRoomMessage = 0;
						}

						// ù��° ������ ������ �� ������ ����Ʈ �س��´�.
						if( g_CUserList.m_pUserInfo[0].m_bUser == TRUE )
						{
							g_pCMainFrame->l_bShowUserState = TRUE;
							g_pCMainFrame->l_nViewWaitUser = 0;
						}						

						//�濡�� ���͵� ���Ⱑ �߻��ϱ� ������....���ӿ� ���κ��� ��� �ʱ�ȭ ��Ų��.
                        SendMessage( g_hWnd, WM_WAITMODESET, 0, 0 );
		

						g_pCMainFrame->l_bShowResult = false;
						g_pCMainFrame->l_bShowDialog = false;
						g_pCMainFrame->l_nDlgMode = 0;
						g_pGoStop->m_bMyKetInput = true;
						g_pGoStop->m_bGameStop = false;
						g_pCMainFrame->l_nStartBtnCnt = 0;
						g_pCMainFrame->l_bShowStartBTN = false;
						g_pGoStop->RoomClear();

						//���ӷ��� ���� ������ �����.
						g_CGameUserList.UserAllClear();

						g_bConnectingSpr = false;

						//�濡�� ���Ȱ� ���ÿ� �������ϼ��� �ֱ⶧����...
						g_pGoStop->m_bGameStop = TRUE;
						g_pGoStop->m_bPlaying = FALSE;

						//�̺�Ʈ�� �ɷȴٸ� Ǯ���ְ� �ɸ��� �ʾҴٸ� ���� ����. 
						if ( g_pGoStop->m_bOtherWait )
							g_pGoStop->SetOtherActionEvent();

                        // ���� �ε��� ����
                        g_pCMainFrame->WaitRoomIndexRefresh();
						break;


					case NGSC_USERINFOCHANGE:
						//DTRACE("Packet Recv - NGSC_USERINFOCHANGE");
						
						cUserAddMius = szRealBufpos[nIndex++];
                        
						if ( cUserAddMius == 'A' ) {
							memcpy( &l_sAddUserInfo, &szRealBufpos[nIndex], sizeof(sUserInfo) );
							nIndex += sizeof(sUserInfo);

							g_CUserList.AddUser( &l_sAddUserInfo );

							g_pCMainFrame->l_nWaitUserCurCnt = UserSort();
                            g_pCMainFrame->WaitUserScrollBarRefresh( g_pCMainFrame->l_nWaitUserCurCnt );
						}
						else {
							nDeleteUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
							nIndex += sizeof(int);

							//����ũ �ѹ��� �´� ������ ����Ʈ �ȿ��� ã�´�.
							UserArrayNo = g_CUserList.FindUser( nDeleteUserNo );
							
							if( UserArrayNo >= 0 ) {
								g_CUserList.DeleteUser( UserArrayNo );
								g_pCMainFrame->l_nWaitUserCurCnt = UserSort();
                                g_pCMainFrame->WaitUserScrollBarRefresh( g_pCMainFrame->l_nWaitUserCurCnt );

                                // ������ ����� �ڽ��� �ƴϰ�, �ٸ� ����ڰ� ���õǾ� �ִ� ���
                                if ( UserArrayNo != 0 && g_pCMainFrame->l_nViewWaitUser != 0 ) {
                                    g_pCMainFrame->m_nHighlightBarNum = -1;
                                    g_pCMainFrame->l_nViewRoomUser = -1;
                                    g_pCMainFrame->l_nViewWaitUser = 0;
                                    g_pCMainFrame->AvatarDelete( -1 );
		                            g_pClient->GetAvatarUrl( g_CUserList.m_pUserInfo[ g_pCMainFrame->l_nViewWaitUser ].m_sUser.nUserNo );
                                }
							}
						}

						break;

					case NGSC_WAITINFOCHANGE:
						//DTRACE("Packet Recv - NGSC_WAITINFOCHANGE");

						cWaitInfoDivi = szRealBufpos[nIndex++];
					
						//cCmdDivi   ( 'C' : RoomCreate(Userlist), 'M' : RoomModify(������ ����)
						//             'A' : join    'R' : exit, roomdel�� Client���� ó��), 
						//			   'X' : RoomInCloseSoket(�濡������Close),
						//			   'G' : Game���� 'W' : ���Ӵ�� 	 )
						switch(cWaitInfoDivi) {
							case 'C':
								memcpy( &l_sOnCreateRoom, &szRealBufpos[nIndex], sizeof(l_sOnCreateRoom) );
								nIndex += sizeof(l_sOnCreateRoom);
								
								nUserNo = l_sOnCreateRoom.nUserNo; // 1. ����ũ �ѹ��� �޴´�.								
								
								// 3. ����ũ �ѹ��� �´� ������ ����Ʈ �ȿ��� ã�´�.
								UserArrayNo = g_CUserList.FindUser( nUserNo );

                                nViewNo = l_sOnCreateRoom.l_sRoomInfo.nRoomNo;

								if( UserArrayNo >= 0 )
								{
									// 2. �������� �ִ´�.									
									memcpy( &g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo , &l_sOnCreateRoom.l_sRoomInfo , sizeof( l_sOnCreateRoom.l_sRoomInfo ));

									// 4. ��������Ʈ�� ���� ������ �� �ȿ� �ִ´�.
									g_CUserList.DeleteUser( UserArrayNo );
									g_pCMainFrame->l_nWaitUserCurCnt = UserSort();
                                    g_pCMainFrame->WaitUserScrollBarRefresh( g_pCMainFrame->l_nWaitUserCurCnt );

									//���� ���� �ο��� �˷��ش�.
									g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo.nCurCnt = l_sOnCreateRoom.l_sRoomInfo.nCurCnt;
								}											

								// 1. ����ũ �ѹ��� �޴´�.
								// 2. �������� �ִ´�.
								// 3. ����ũ �ѹ��� �´� ������ ����Ʈ �ȿ��� ã�´�.
								// 4. ��������Ʈ�� ���� ������ �� �ȿ� �ִ´�.
								// 5. ��������Ʈ�� ���� ������ �����.

                                g_pCMainFrame->WaitRoomIndexRefresh();
								break;

							case 'A':
								memcpy( &l_sRoomInfoToWaitUserAX, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserAX) );
								nIndex += sizeof(l_sRoomInfoToWaitUserAX);

								// 1. ����ũ �ѹ��� �޴´�.
								nUserNo = l_sRoomInfoToWaitUserAX.nUserNo;

								// 3. ����ũ �ѹ��� �´� ������ ����Ʈ �ȿ��� ã�´�.
								UserArrayNo = g_CUserList.FindUser( nUserNo );								
                                nViewNo = l_sRoomInfoToWaitUserAX.nRoomNo;
								g_CUserList.DeleteUser( UserArrayNo );								
								
								//���� ���� �ο��� �˷��ش�.
								g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo.nCurCnt = l_sRoomInfoToWaitUserAX.nRoomCnt;
								g_pCMainFrame->l_nWaitUserCurCnt = UserSort();
                                g_pCMainFrame->WaitUserScrollBarRefresh( g_pCMainFrame->l_nWaitUserCurCnt );

                                // ������ ���� ���� �� ���� ��쿡�� �� ���°� ���ϹǷ�
                                // �� á�� ���� refresh ���ش�.

                                if ( l_sRoomInfoToWaitUserAX.nRoomCnt >= MAX_ROOM_IN ) {
								    g_pCMainFrame->WaitRoomIndexRefresh();
                                }

                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserAX.nRoomNo );
								break;

                            
							case 'X':   // ������ ������ ���ͼ� ���� �������� ��
								memcpy( &l_sRoomInfoToWaitUserAX, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserAX) );
								nIndex += sizeof(l_sRoomInfoToWaitUserAX);

                                nViewNo = l_sRoomInfoToWaitUserAX.nRoomNo;
								
								if( l_sRoomInfoToWaitUserAX.nRoomCnt > 0 ) {    //Join

									//���� ���� �ο��� �˷��ش�.
									g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo.nCurCnt = l_sRoomInfoToWaitUserAX.nRoomCnt;

								}
								else {  //���ӹ濡�� ������ �ٷ� �������� ���� �ٷ� �������..
									memset( &g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo] , 0 , sizeof(g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo]));
								}

                                g_pCMainFrame->WaitRoomIndexRefresh();
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserAX.nRoomNo );
								break;

							case 'R':   // ���� �Ѹ��� �濡�� ������ ��
								memcpy( &l_sRoomInfoToWaitUserR, &szRealBufpos[nIndex], sizeof(l_sRoomInfoToWaitUserR) );
								nIndex += sizeof(l_sRoomInfoToWaitUserR);

                                // ��� �濡�� ���� ������ ����� ����Ʈ�� �߰��Ѵ�.
                                g_CUserList.AddUser( &l_sRoomInfoToWaitUserR.l_sUserInfo );

                                nViewNo = l_sRoomInfoToWaitUserR.nRoomNo;

								if( l_sRoomInfoToWaitUserR.nRoomCnt <= 0 ) {
									memset( &g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo , 0x00 , sizeof(sRoomInfo));
								}

								//���� ���� �ο��� �˷��ش�.
								g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo.nCurCnt = l_sRoomInfoToWaitUserR.nRoomCnt;


                                if ( g_CUserList.m_pUserInfo[ g_pCMainFrame->l_nViewWaitUser ].m_sUser.nUserNo > 0 ) {
                                    if ( g_pCMainFrame->l_nViewWaitUser != 0 ) {
                                        g_pCMainFrame->l_nViewWaitUser = 0;
                                        g_pCMainFrame->AvatarDelete( -1 );
		                                 g_pClient->GetAvatarUrl( g_CUserList.m_pUserInfo[ g_pCMainFrame->l_nViewWaitUser ].m_sUser.nUserNo );		
                                    }
                                }

						
								g_pCMainFrame->l_nWaitUserCurCnt = UserSort();
                                g_pCMainFrame->WaitUserScrollBarRefresh( g_pCMainFrame->l_nWaitUserCurCnt );
                                g_pCMainFrame->WaitRoomIndexRefresh();
                                g_pCMainFrame->ViewRoomInfoRefresh( l_sRoomInfoToWaitUserR.nRoomNo );
								break;
						}
						break;

					case NGSC_CREATEROOM:

						//DTRACE("Packet Recv - NGSC_CREATEROOM");

						nRoomNo = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						g_nRoomNo = nRoomNo;
						g_nRoomCurCnt = 0;


                        SendMessage( g_hWnd, WM_GAMEMODESET, 0, 0 );

						g_pGoStop->RoomClear(); //���⿡�� �ο��� 0���� �ϱ⶧����..

						//���� ���� ��������Ƿ� ���� �����̰� ���̴�.
						g_CGameUserList.BangJangDel();
						g_CGameUserList.m_GameUserInfo[0].m_bBBangJjang = TRUE;

						g_CGameUserList.SunDel();
						g_CGameUserList.m_GameUserInfo[0].m_bSun = TRUE;

						g_MyUser = 0;

						g_pGoStop->GameCreate(g_nDoubleCardCnt, g_nThreeCardCnt, g_biRoomMoney);
						g_pCMainFrame->l_bShowStartBTN = TRUE;

						memset( g_pCMainFrame->l_nPVal, 0x00, sizeof(g_pCMainFrame->l_nPVal) );

						g_sPaintAvatar.bPrint[1] = FALSE;
						g_sPaintAvatar.bPrint[2] = FALSE;

						g_pCMainFrame->AvatarDelete(1);
						g_pCMainFrame->AvatarDelete(2);

						g_sPaintAvatar.bPrint[1] = TRUE;
						g_sPaintAvatar.bPrint[2] = TRUE;
                        g_pCMainFrame->m_pGameChat->Init();

						break;

                    // �ٸ� ����� ������ ��
					case NGSC_ROOMOUT_OTHERSEND:

                        //ó���ѹ��� �̺�Ʈ�� �����ֱ����켭,
						g_pCMainFrame->m_bSunSet = FALSE;

						g_pCMainFrame->m_nSunSetRender = 0;
						g_pCMainFrame->m_bSunSetRender = FALSE;

						g_pCMainFrame->m_nSunRenderDecided = 0;
						g_pCMainFrame->m_bRenderSun = FALSE;

						if ( g_pGoStop->m_bPlaying == false ) {
							g_pCMainFrame->l_nStartBtnCnt = 0;
							g_pCMainFrame->l_bShowStartBTN = false;
						}
								
						memcpy( &l_sRoomOutOtherSend, &szRealBufpos[nIndex], sizeof(l_sRoomOutOtherSend) );
						nIndex += sizeof(l_sRoomOutOtherSend);

						//�������ѹ��� g_CurGamerUserInfo�� ��̹�ȣ.


						
						g_nRoomCurCnt = l_sRoomOutOtherSend.nRoomCurCnt;
						nGamerUserNo = g_CGameUserList.FindUser( l_sRoomOutOtherSend.nUserNo );

						if ( nGamerUserNo >= 0 ) {
							//���������.						
							nUserSlot = g_pGoStop->m_pFloor.FindUser( nGamerUserNo );
								
							if ( nUserSlot >= 0 ) {

								g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.szNick_Name , "�����߽��ϴ�" , 0 , RGB( 255 , 255 , 165 ) , FALSE );						
								g_pGoStop->m_pFloor.DeleteUser( nUserSlot );
								g_CGameUserList.DeleteUser( l_sRoomOutOtherSend.nUserNo );
							}
							else {
								g_pCMainFrame->l_Message->PutMessage( "�濡 ���� �����Դϴ�.", "Code - 112" );
							}

						}
						else {
							g_pCMainFrame->l_Message->PutMessage( "��������Ʈ�� ���� �����Դϴ�.", "Code - 113" );
						}

						//������ �ٲ������ 
						if ( l_sRoomOutOtherSend.nNewBbangJjnag != 0 ) {
							g_CGameUserList.NewBangJangSet( l_sRoomOutOtherSend.nNewBbangJjnag );
						}

						//���� ����� ���� ��������.
						if ( l_sRoomOutOtherSend.nNewHostNo != 0 ) {
							g_CGameUserList.NewSunSet( l_sRoomOutOtherSend.nNewHostNo );
						}


						//Ȥ�� �÷��� ���̸� ������ ���߰�..
						if ( g_pGoStop->m_pFloor.RealGame() && g_pGoStop->m_bPlaying ) {
						
							g_pGoStop->m_bGameStop = TRUE;
							g_pGoStop->m_bPlaying = FALSE;

						}
						else {
							g_pGoStop->m_bGameStop = FALSE;
						}


						//�̺�Ʈ�� �ɷȴٸ� Ǯ���ְ� �ɸ��� �ʾҴٸ� ���� ����. 
						if ( g_pGoStop->m_bOtherWait )
							g_pGoStop->SetOtherActionEvent();

						if ( g_nRoomCurCnt == 1 ) {
							//start

							g_pGoStop->m_pFloor.m_RealMode = false;
							g_pGoStop->m_bMasterMode = true;

							//���� ������ ���� ������ ���̵ȴ�.
							g_CGameUserList.BangJangDel();
							g_CGameUserList.m_GameUserInfo[0].m_bBBangJjang = TRUE;

							g_CGameUserList.SunDel();
							g_CGameUserList.m_GameUserInfo[0].m_bSun = TRUE;

							if ( g_pGoStop->m_bPlaying == false ) {
								g_pCMainFrame->l_nStartBtnCnt = 0;
								g_pGoStop->GameCreate(g_nDoubleCardCnt, g_nThreeCardCnt, g_biRoomMoney);
								g_pCMainFrame->l_bShowStartBTN = TRUE;
							}
						}
						else if ( g_nRoomCurCnt > MAX_ROOM_IN || g_nRoomCurCnt <= 0	) {
								g_pCMainFrame->l_Message->PutMessage( "Floor�� �̻��մϴ�.", "Code - 118" );							
						}

						g_pGoStop->m_pFloor.m_pGamer[0].m_nUserNo = 0;
						g_MyUser = g_pGoStop->m_pFloor.m_pGamer[0].m_nUserNo;

                        // �ٸ� ����� ������ �� ��ǻ�� �̹����� �ѷ��ش�.
						for ( i = 1; i < MAX_ROOM_IN; i++ ) {
							g_sPaintAvatar.bPrint[i] = FALSE;
							g_pCMainFrame->AvatarDelete(i);
							sprintf( ptemp, ".\\image\\game\\comno%d.spr", i );
                            g_pCMainFrame->ReGetSprite( &g_pCMainFrame->l_SPR_GifAvatar[i], ptemp, 125, 190, 1 );
							g_sPaintAvatar.bPrint[i] = TRUE;
						}

                        g_pCMainFrame->PlaySound( g_Sound[GS_ROOMEXIT] );
						break;

					case NGSC_ROOMIN:

						memset( &l_sMyRoomInfo, 0x00, sizeof(l_sMyRoomInfo) );
						memcpy( &l_sMyRoomInfo, &szRealBufpos[nIndex], sizeof(l_sMyRoomInfo) );
						nIndex += sizeof(l_sMyRoomInfo);

						g_biRoomMoney = l_sMyRoomInfo.biRoomMoney;
						g_nRoomCurCnt = l_sMyRoomInfo.nCurCnt;

						memset( &l_sRoomInUserSend, 0x00, sizeof(l_sRoomInUserSend) ); 
						memcpy( &l_sRoomInUserSend, &szRealBufpos[nIndex], sizeof(l_sRoomInUserSend) );
						nIndex += sizeof(l_sRoomInUserSend);

						g_nRoomNo = l_sMyRoomInfo.nRoomNo;
						memcpy( g_szRoomName, l_sMyRoomInfo.szRoomName, MAX_ROOM_NAME );

						memset( &l_sRoomInAdd, 0x00, sizeof(l_sRoomInAdd) ); 
						memcpy( &l_sRoomInAdd, &szRealBufpos[nIndex], sizeof(l_sRoomInAdd) );
						nIndex += sizeof(l_sRoomInAdd);

						nRoomInUserNo = l_sRoomInAdd.nRoomInUserNo;

						//�������� �ִ´�. �ڽ��� ���� ��̰� 0���̱⶧���� i��°�� ��� �ش� ��ġ�� �ȴ�.
						for ( i = 1; i < MAX_ROOM_IN; i++ ) {

							//�ڽ��� ��ȣ ���� ���� ������ �ȴ�.
							nUserArrayNo = ( nRoomInUserNo + i ) % MAX_ROOM_IN;
							
							if ( l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.nUserNo > 0 ) {

								nSlotNo = g_CGameUserList.AddGameUser( &l_sRoomInUserSend[nUserArrayNo].l_sUserInfo, l_sRoomInUserSend[nUserArrayNo].nVoice, i );

								g_pGoStop->m_pFloor.AddRoomInGamer( nSlotNo, l_sRoomInUserSend[nUserArrayNo].l_sUserInfo.szPreAvatar );

								//������ �־��ش�.
								if ( l_sRoomInUserSend[nUserArrayNo].bBbangJjang == TRUE ) {
									g_CGameUserList.BangJangDel();
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_bBBangJjang = TRUE;
								}

								if ( l_sRoomInUserSend[nUserArrayNo].bRoomMaster == TRUE ) {
									g_CGameUserList.SunDel();
									g_CGameUserList.m_GameUserInfo[nSlotNo].m_bSun = TRUE;

									nSenNo = nSlotNo;
								}
							}
						}

						//floor�� ���Ӽ����� �������ش�.
						for ( i = 0; i < MAX_ROOM_IN; i++ ) {
							//������� ���Ӽ����� �����Ѵ�.
							nUserArrayNo = ( nSenNo + i ) % MAX_ROOM_IN;

							if ( g_CGameUserList.m_GameUserInfo[nUserArrayNo].m_bUser ) {
								g_pGoStop->m_pFloor.m_pGamer[nUserArrayNo].m_nUserNo = i;
							}
						}

						g_pGoStop->m_biMoney = l_sMyRoomInfo.biRoomMoney;
						g_pGoStop->m_Hwatu.JoinCreate();			

						g_pGoStop->m_bMasterMode = false;

						g_pGoStop->m_pFloor.m_RealMode = true;

						g_MyUser = g_pGoStop->m_pFloor.m_pGamer[0].m_nUserNo;

						for( i = 0 ; i < MAX_ROOM_IN ; i ++ )
						{
							if ( g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo < 0 ) {
								
								g_sPaintAvatar.bPrint[i] = FALSE;
								g_pCMainFrame->AvatarDelete(i);
								g_sPaintAvatar.bPrint[i] = TRUE;

								continue;
							}

							g_sPaintAvatar.bPrint[i] = FALSE;
							memset( &g_sAvatarPlayStr, 0x00, sizeof(g_sAvatarPlayStr) );

							g_sAvatarPlayStr.nSlot = i;
							strcpy( g_sAvatarPlayStr.szAvaUrl, g_pGoStop->m_pFloor.m_pGamer[i].m_szPreAvatar );
							strcpy( g_sAvatarPlayStr.szGameGubun, g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo].m_sUserInfo.szGameGubun );
							g_sAvatarPlayStr.cSex = g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo].m_sUserInfo.cSex;

							SendMessage( g_hWnd, WM_AVATARPLAY, 0, 0  );
							g_sPaintAvatar.bPrint[i] = TRUE;

						}
	
                        SendMessage( g_hWnd, WM_GAMEMODESET, 0, 0 );

						memset( g_pCMainFrame->l_nPVal, 0x00, sizeof(g_pCMainFrame->l_nPVal) );
						

						g_pCMainFrame->PlaySound( g_Sound[GS_ROOMJOIN] );
                        g_pCMainFrame->m_pGameChat->Init();
						break;

					case NGSC_ROOMIN_OTHERSEND:
                        
						if ( g_pCMainFrame->l_bShowStartBTN == TRUE )
							g_pCMainFrame->l_bShowStartBTN = FALSE;

						memset( &l_sRoomInOtherSend, 0x00, sizeof(l_sRoomInOtherSend) ); 
						memcpy( &l_sRoomInOtherSend, &szRealBufpos[nIndex], sizeof(l_sRoomInOtherSend) );
						nIndex += sizeof(l_sRoomInOtherSend);

						nUserVoice = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						g_nRoomCurCnt = l_sRoomInOtherSend.nRoomCurCnt;
						

						nNextNo = l_sRoomInOtherSend.nNextNo;
						nBbangJjangUniqNo = l_sRoomInOtherSend.nBbangJjangUniqNo;

						g_CGameUserList.NewBangJangSet( nBbangJjangUniqNo );
						
						nSlotNo = g_CGameUserList.AddRoomInOtherUser( &l_sRoomInOtherSend.l_sUserInfo, nUserVoice, nNextNo );
						g_pGoStop->m_pFloor.AddOtherGamer( nSlotNo, nNextNo, l_sRoomInOtherSend.l_sUserInfo.szPreAvatar );
						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nSlotNo].m_sUserInfo.szNick_Name , "�����߽��ϴ�" , 0 ,RGB( 255 , 255 , 165 ) , FALSE );

						SunSetInit();

						for( i = 1 ; i < MAX_ROOM_IN ; i ++ )
						{
							if ( g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo < 0 ) {
								
								g_sPaintAvatar.bPrint[i] = FALSE;
								g_pCMainFrame->AvatarDelete(i);
								g_sPaintAvatar.bPrint[i] = TRUE;

								continue;
							}

							g_sPaintAvatar.bPrint[i] = FALSE;
							memset( &g_sAvatarPlayStr, 0x00, sizeof(g_sAvatarPlayStr) );

							g_sAvatarPlayStr.nSlot = i;
							strcpy( g_sAvatarPlayStr.szAvaUrl, g_pGoStop->m_pFloor.m_pGamer[i].m_szPreAvatar );
							strcpy( g_sAvatarPlayStr.szGameGubun, g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo].m_sUserInfo.szGameGubun );
							g_sAvatarPlayStr.cSex = g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_pGamer[i].m_nGamerUserNo].m_sUserInfo.cSex;

							SendMessage( g_hWnd, WM_AVATARPLAY, 0, 0  );
							g_sPaintAvatar.bPrint[i] = TRUE;

						}
						g_pCMainFrame->PlaySound( g_Sound[GS_ROOMJOIN] );

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
                            g_pCMainFrame->AddWaitChatText( ptemp, RGB( 73, 73, 73 ));
						}
						else
						{
                            g_pCMainFrame->AddWaitChatText( ptemp, RGB( 38, 86, 131 ));
						}						
						break;	
						
					case NGSC_VIEWPAGE:
						
						nRoomCnt = 	MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						l_sRoomInfoClient = new sRoomInfoClient[nRoomCnt];
						memset( l_sRoomInfoClient, 0x00, sizeof(sRoomInfoClient) * nRoomCnt );

						//�� ������ �ʱ�ȭ ��Ų��.
						memset( &g_CGameWaitRoomList.m_sRoomStateInfo, 0x00, sizeof(g_CGameWaitRoomList.m_sRoomStateInfo) );						

						g_CUserList.AddUser( &g_CGameUserList.m_GameUserInfo[0].m_sUserInfo );
						
						
						//������.				
						for ( i = 0; i < nRoomCnt; i++ ) {
							memcpy( &l_sRoomInfoClient[i].l_sRoomInfo, &szRealBufpos[nIndex], sizeof(sRoomInfo) );
							nIndex += sizeof(sRoomInfo);

							//�� ������ �޴´�.
                            nViewNo = l_sRoomInfoClient[i].l_sRoomInfo.nRoomNo;
							memcpy( &g_CGameWaitRoomList.m_sRoomStateInfo[nViewNo].l_sRoomInfo, &l_sRoomInfoClient[i].l_sRoomInfo , sizeof(l_sRoomInfoClient[i].l_sRoomInfo));													
						}

						if ( g_nViewPageNo != 0 ) {
							
							if ( nRoomCnt == 0 ) {
								
								l_nNoRoom++;

								if ( l_nNoRoom >= 2 ) {
									
									pClientSocket->ViewPage(0);
									g_nViewPageNo = 0;
									g_pCMainFrame->l_nPageStart = 0;
									l_nNoRoom = 0;

								}
								break;
							}

							l_nNoRoom = 0;
						}
						
						break;

					case NGSC_KEEPALIVE:

						pClientSocket->KeepAlive();
						break;

					case NGCC_GAMECHAT:

						memcpy( szSendNickName, &szRealBufpos[nIndex], MAX_NICK_LENGTH );
						nIndex += MAX_NICK_LENGTH;
						
						nEarTalkUserNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						nChatLen = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;	

						memcpy( szChatMsg, &szRealBufpos[nIndex], nChatLen );
						nIndex += nChatLen;

                        g_pCMainFrame->AddGameChatText( szChatMsg, RGB(132, 255, 200));
						break;	
						
					case NGSC_GAMEEND:
						//���� �����ϱⰡ ������ �ȴ�.
						break;

                    //-------------------------------------------------------
                    // �������� ���� ������ �ʴ��� �� ����� ���� �����ش�.
                    //-------------------------------------------------------

					case NGSC_VIEWWAIT:

                        nWaitUserCnt = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

                        memset( l_sGameWaitInfo, 0x00, sizeof(l_sGameWaitInfo) );
                        memcpy( l_sGameWaitInfo, &szRealBufpos[nIndex],  sizeof(sGameWaitInfo) * nWaitUserCnt );

						nIndex += sizeof(sGameWaitInfo) * nWaitUserCnt;

                        memset( g_sGameWaitInfo, 0x00, sizeof(g_sGameWaitInfo) );
						memcpy( g_sGameWaitInfo, l_sGameWaitInfo, sizeof(l_sGameWaitInfo) );

						//üũ�� ���� FALSE �� �����. ( NEXT �� PREV �� ������ ��츦 ��� )
						nTempNo	= g_pCMainFrame->GetInviteCheck();
                        if ( nTempNo != -1 ) {
                            g_pCMainFrame->sInviteCheck[nTempNo].bCheck = FALSE;
                        }

						g_pCMainFrame->l_bShowInviteBox = TRUE;
                        g_pCMainFrame->m_SCB_Invite->SetElem( nWaitUserCnt );
                        g_pCMainFrame->m_SCB_Invite->StartPage();

						break;

					case NGSC_USERINVITE:

						memset( &l_sUserInvite, 0x00, sizeof(l_sUserInvite) );
						memcpy( &l_sUserInvite, &szRealBufpos[nIndex],  sizeof(l_sUserInvite) );
						nIndex += sizeof(l_sUserInvite);
						
						memcpy( szInviteMsg, &szRealBufpos[nIndex], l_sUserInvite.nInviteLen );
						nIndex += l_sUserInvite.nInviteLen;

						g_pCMainFrame->RecieveInvite( l_sUserInvite.nRoomNo , l_sUserInvite.biRoomMoney, 
							l_sUserInvite.szRoomPass , szInviteMsg );
						
						break;

					case NGCC_STARTGAME:

						nRoomSeed = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
						nIndex += 4;

						nCardCnt = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						memset( g_szRecvCardPack, 0x00, sizeof(BYTE) * MAX_RECVCARD_CNT );
						memcpy( g_szRecvCardPack, &szRealBufpos[nIndex], sizeof(BYTE) * MAX_RECVCARD_CNT );
						nIndex += nCardCnt;

						srand(nRoomSeed);

						g_pGoStop->m_pRecvCardPack = g_szRecvCardPack;
						g_pGoStop->Start();
						
						break;

					case NGCC_SENDUSERCARD:

						nRecvUserCard = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;
						
						for ( i = 0; i < MAX_ROOM_IN; i++ )
						{
							if ( g_CurUser == g_pGoStop->m_pFloor.m_pGamer[i].m_nUserNo )
								nUserSlot = i;
						}

						//Q�� �ִ´�.
						pClientSocket->m_RecvCardInputQ.WriteToQ( nRecvUserCard );
						break;

					case NGCC_RESULTOK:
						
						nUniQNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						g_pCMainFrame->OnResultOk( nUniQNo, 0  );


						break;
					case NGCC_CARDSHAKE:

						nShakeCard = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						memset( g_nRecvShakeCardArr, -1, sizeof(g_nRecvShakeCardArr) );
						memcpy( g_nRecvShakeCardArr, &szRealBufpos[nIndex], sizeof(g_nRecvShakeCardArr) );

						nIndex += sizeof(g_nRecvShakeCardArr);

						pClientSocket->m_RecvCardInputQ.WriteToQ( nShakeCard, 1 );

						break;

					case NGCC_GOSTOP:

						nGoStop = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						pClientSocket->m_RecvCardInputQ.WriteToQ( nGoStop );

						break;	
						
					case NGCC_SENDNINECARD:

						nNineCard = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						//Q�� �ִ´�.
						pClientSocket->m_RecvCardInputQ.WriteToQ( nNineCard );

						break;	

					case NGCC_USEREXITRESERV:

						nUniQNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;
						
						memcpy( &bExit, &szRealBufpos[nIndex], sizeof(bExit) );
						nIndex += sizeof(bExit);

						g_pGoStop->OnUserExitReserv( nUniQNo, bExit );
						break;
						
					case NGSC_GAMINGEXITUSER:

						memset( &l_sGamingExitUser, 0x00, sizeof(l_sGamingExitUser) );
						memcpy( &l_sGamingExitUser, &szRealBufpos[nIndex], sizeof(l_sGamingExitUser) );
						nIndex += sizeof(l_sGamingExitUser);

						nGamerUserNo = g_CGameUserList.FindUser( l_sGamingExitUser.nUserNo );
						l_sGamingExitUser.nGameUserNo = nGamerUserNo;

						nUserSlot = g_pGoStop->m_pFloor.FindUser( nGamerUserNo );
						nExitUserSlot = nUserSlot;
						g_pGoStop->m_pFloor.m_pGamer[nUserSlot].m_ExitReserv = true;
						g_pGoStop->m_pFloor.m_pGamer[nUserSlot].m_bComputer = true;
						g_pGoStop->m_pFloor.m_pGamer[nUserSlot].m_bAlly = true;

						//������ �̹����� �ٲ��ش�.
						g_sPaintAvatar.bPrint[nUserSlot] = FALSE;

						g_pCMainFrame->AvatarDelete(nUserSlot);
                        g_pCMainFrame->ReGetSprite( &(g_pCMainFrame->l_SPR_GifAvatar[nUserSlot]), ".\\image\\game\\disconnect.spr", 125, 190, 1 );
						g_sPaintAvatar.bPrint[nUserSlot] = TRUE;	

					
						//Ȥ�� �̺�Ʈ�� �ɷ������� �𸣹Ƿ� Ǯ���ش�.
						if ( g_pGoStop->m_pFloor.m_pGamer[nUserSlot].m_nUserNo == g_CurUser ) {
							
							Sleep(1000);	//�̺�Ʈ�� �ɸ����� �ణ ��ٷ��ش�. 1�ʸ� ��ٷ��� �׷��� �ڽ��̰� �̺�Ʈ�� �ɷ�������..
							if ( g_pGoStop->m_pFloor.m_pGamer[nUserSlot].m_nUserNo == g_CurUser ) {
								if ( g_pGoStop->m_bOtherWait )
									g_pGoStop->SetOtherActionEvent();
							}
							
							g_pCMainFrame->l_bShowDialog = FALSE;
							g_pCMainFrame->l_nDlgMode = 0;
						}

						//������ �ٲ������ üũ
						if ( l_sGamingExitUser.nNewBbangJjnag != 0 ) {
							
							//�������ѹ��� g_CurGamerUserInfo�� ��̹�ȣ.
							nGamerUserNo = g_CGameUserList.FindUser( l_sGamingExitUser.nNewBbangJjnag );

							if ( nGamerUserNo >= 0 ) {
								//���彽��ã��.
								nUserSlot = g_pGoStop->m_pFloor.FindUser( nGamerUserNo );
									
								if ( nUserSlot >= 0 ) {
									g_pGoStop->m_pFloor.m_nBbangJjangSlotNo = nUserSlot;
								}
								else {
									g_pCMainFrame->l_Message->PutMessage( "�濡 ���� �����Դϴ�", "Code - 128" );
								} 
							}
							else {
								g_pCMainFrame->l_Message->PutMessage( "��������Ʈ�� ���� �����Դϴ�.", "Code - 129" );
							}
						}

						for ( i = 0; i < MAX_ROOM_IN; i++ ) {
							if ( g_sGamingExitUser[i].nUserNo <= 0 ) {
								memcpy( &g_sGamingExitUser[i], &l_sGamingExitUser, sizeof(l_sGamingExitUser) );
								break;
							}
						}
												
						//������ ������ Ȯ���� ������ ���� ������ Ȯ���� ������ �ʰ� ������ �������� OnRegames�� �ҷ��ش�.
						if ( g_pGoStop->m_bGameEndWill ) {
							g_pCMainFrame->OnReGames( g_pGoStop->m_bMasterMode, nExitUserSlot );
						}



						break;

					case NGCC_MISSIONSET:

						nMissionType = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						nMissionMultiply = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						nPrizeNo = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						g_pCMainFrame->PlaySound( g_Sound[GS_MISSION_START] );
						g_pGoStop->m_nMissionType = (MISSION)nMissionType;
						g_pGoStop->m_nMissionMultiply = nMissionMultiply;
						g_pGoStop->m_nPrizeNo = nPrizeNo;

						g_pCMainFrame->ShowMission();

						break;

					case NGSC_MEMO:

						memcpy( szSendNickName, &szRealBufpos[nIndex], MAX_NICK_LENGTH );
						nIndex += MAX_NICK_LENGTH;

						nMemoLen = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						memcpy( szMemoMsg, &szRealBufpos[nIndex], nMemoLen );
						nIndex += nMemoLen;

						g_pCMainFrame->RecieveMemo( szSendNickName , szMemoMsg );
						break;

					case NGSC_STARTGAME:

						//Q�� �ִ´�.
						pClientSocket->m_RecvCardInputQ.WriteToQ( 0, 2 );
						break;
						
					case NGSC_GETAVAURL:

						memset( szAvaUrl, 0x00, MAX_AVAURLLEN );

						cSex = szRealBufpos[nIndex];
						nIndex++;

						memcpy( szAvaUrl, &szRealBufpos[nIndex], MAX_AVAURLLEN );
						nIndex += MAX_AVAURLLEN;

						memcpy( szGameGubun, &szRealBufpos[nIndex], 5 );
						nIndex += 5;	

						memset( &g_sAvatarPlayStr, 0x00, sizeof(g_sAvatarPlayStr) );

						g_sAvatarPlayStr.nSlot = -1;
						strcpy( g_sAvatarPlayStr.szAvaUrl, szAvaUrl );
						strcpy( g_sAvatarPlayStr.szGameGubun, szGameGubun );
						g_sAvatarPlayStr.cSex = cSex;

						SendMessage( g_hWnd, WM_AVATARPLAY, 0, 0  );

						break;

					case NGSC_ALLCMT:

						memset( szAllCmt, 0x00, 1000 );

						memcpy( szAllCmt, &szRealBufpos[nIndex], 1000 );
						nIndex += 1000;

                        if ( g_pCMainFrame->l_nCurrentMode == WAITROOM_MODE ) {
                            g_pCMainFrame->AddWaitChatText( szAllCmt, RGB( 73, 73, 73 ));
                        }
                        else if ( g_pCMainFrame->l_nCurrentMode == GAME_MODE ) {
                            g_pCMainFrame->AddGameChatText( szAllCmt, RGB(132, 255, 200));
                        }
						break;

					case NGSC_IPCHECK:
						g_pCMainFrame->l_Message->PutMessage( "���� �����Ƿ� ���� �濡��", "������ �Ҽ� �����ϴ�." );
						break;

					case NGSC_GAMESUNSET:
                        g_pCMainFrame->AddGameChatText( "[ī�带 ����ּ���!!!]", RGB(132, 255, 200));

						memset( g_pCMainFrame->m_szSunPack, 0x00, sizeof(g_pCMainFrame->m_szSunPack) );
						memcpy( g_pCMainFrame->m_szSunPack, &szRealBufpos[nIndex], sizeof(g_pCMainFrame->m_szSunPack) );
						nIndex += sizeof(g_pCMainFrame->m_szSunPack);

						SunSetInit();
						g_pCMainFrame->m_bSunSetChoice = FALSE;

						g_pCMainFrame->m_nSunSetRender = 0;
						g_pCMainFrame->m_nMySunChioce = -1;
						g_pCMainFrame->m_nOtherSunChioce = -1;
						
						for ( i = 0; i < MAX_SUNCARD_CNT; i++ ) {
							g_pCMainFrame->l_AniStack.AddStack( &g_pCMainFrame->m_szSunPackPos[i] );
						}

						g_pCMainFrame->m_bSunSetRender = TRUE;
						
						break;

					case NGSC_CHOICECARDNO:
						nChoiceNo = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						nUniQNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						if ( nUniQNo == g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.nUserNo )
							g_pCMainFrame->m_nMySunChioce = nChoiceNo;
						else 
							g_pCMainFrame->m_nOtherSunChioce = nChoiceNo;

						break;

					case NGSC_SUNDECIDE:
						nUniQNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );

						nIndex += 4;

						g_CGameUserList.NewSunSet(nUniQNo);

						if ( nUniQNo == g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.nUserNo ) {
							g_pCMainFrame->m_bSunSet = FALSE;
							g_pGoStop->GameCreate(g_nDoubleCardCnt, g_nThreeCardCnt, g_biRoomMoney);
							nMasterSlotNo = MASTER_POS;
						}
						else {
							g_pGoStop->m_Hwatu.JoinCreate();
							nMasterSlotNo = 1;
						}

						//���� ��� ����.
						g_pGoStop->m_pFloor.m_nMasterSlotNo = nMasterSlotNo;
						if ( g_pGoStop->m_pFloor.m_nMasterSlotNo == MASTER_POS )
							g_pGoStop->m_bMasterMode = true;
						else
							g_pGoStop->m_bMasterMode = false;

						g_pGoStop->m_pFloor.UserRefresh( g_pGoStop->m_pFloor.m_nMasterSlotNo );

						g_MyUser = g_pGoStop->m_pFloor.m_pGamer[0].m_nUserNo;


						g_pCMainFrame->m_nSunRenderDecided = 0;
						g_pCMainFrame->m_bRenderSun = TRUE;

						sprintf( ptemp , "%s�� ��", g_CGameUserList.m_GameUserInfo[g_pGoStop->m_pFloor.m_nMasterSlotNo].m_sUserInfo.szNick_Name );
                        g_pCMainFrame->AddGameChatText( ptemp, RGB(132, 255, 200));

						break;

					case NGSC_RECHOICE:
						g_pCMainFrame->m_bSunSetChoice = FALSE;
                        g_pCMainFrame->AddGameChatText( "[������ �� ī�� �Դϴ�! �ٽü����Ͽ� �ּ���]", RGB(132, 255, 200));

						break;

					case NGSC_ROOMINFO:
                        {
						    memset( g_ClickRoomUserInfo, 0x00, sizeof(g_ClickRoomUserInfo) );
						    memcpy( g_ClickRoomUserInfo, &szRealBufpos[nIndex], sizeof(g_ClickRoomUserInfo) );
						    nIndex += sizeof(g_ClickRoomUserInfo);

                            bool bExist = false;    // ���� ���� ������ ������ �ִ��� �˻�

                            // �� ������ ���ŵǾ ���õǾ� �ִ� ������ ���� ������ ���õ� ���� ������ �״�� �����ȴ�.
                            if ( g_pCMainFrame->m_nUserListTitleNum == 1 && 
                                 g_pCMainFrame->l_nViewRoomMessage != -1 &&
                                 g_pCMainFrame->l_nViewRoomUser != -1 )
                            {
                                for ( i = 0; i < MAX_ROOM_IN; i++ ) {
                                    if ( g_ClickRoomUserInfo[i].szNick_Name[0] == NULL) continue;

                                    if ( g_ClickRoomUserInfo[i].nUserNo == g_CGameWaitRoomList.m_sRoomStateInfo[g_pCMainFrame->l_nViewRoomMessage].l_sUserInfo[g_pCMainFrame->l_nWaitUserNo].nUserNo ) {
                                        bExist = true;
                                    }
                                }
                            }

							g_pCMainFrame->l_nViewRoomUser = -1;
                        }

						break;

					case NGSC_GAMEREALSTARTBTN:

						memset( &l_sRealGameStart, 0x00, sizeof(l_sRealGameStart) );
						memcpy( &l_sRealGameStart, &szRealBufpos[nIndex], sizeof(l_sRealGameStart) );
						nIndex += sizeof(l_sRealGameStart);

						g_CGameUserList.NewBangJangSet( l_sRealGameStart.nBbangJjangUniqNo );
						g_CGameUserList.NewSunSet( l_sRealGameStart.nRoomMasterUniqNo );

						if ( g_CGameUserList.m_GameUserInfo[0].m_bSun ) {
							if ( !g_pGoStop->m_bPlaying ) {
								
								//start
								g_pGoStop->m_pFloor.m_RealMode = true;
								g_pCMainFrame->l_nStartBtnCnt = 0;
								g_pGoStop->GameCreate(g_nDoubleCardCnt, g_nThreeCardCnt, g_biRoomMoney);

								g_pCMainFrame->l_bShowStartBTN = TRUE;
			
							}
							else {
								g_pGoStop->m_bGameStop = true;
							}
						}
						else {
							g_pGoStop->m_pFloor.m_RealMode = true;
						}

						break;

					case NGSC_SETVOICE:

						nUniQNo = MAKELONG( MAKEWORD( szRealBufpos[nIndex + 3], szRealBufpos[nIndex + 2] ) ,
									 MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] ) );
						nIndex += 4;

						nUserVoice = MAKEWORD( szRealBufpos[nIndex + 1], szRealBufpos[nIndex + 0] );
						nIndex += 2;

						g_CGameUserList.UserVoiceSet( nUniQNo, nUserVoice );

						break;

                    // �ٸ� ������ ī�带 �ٲپ����Ƿ� �� ������ ī�� ������ �ٲ��ش�.
					case NGCC_CARDCHANGE:
						memset( &l_sUserChange, 0x00, sizeof(l_sUserChange) );
						memcpy( &l_sUserChange, &szRealBufpos[nIndex], sizeof(l_sUserChange) );
						nIndex += sizeof(l_sUserChange);			

                        {
                            int nUserCardIdx = 0;
                            int nPackCardIdx = 0;
                            long nTempX = 0;
                            long nTempY = 0;
                            bool bExistPackCard = false;
                            bool bExistUserCard = false;

                            //ī���Ѱ� ������ ī�带 �ٲ۴�.
						    for ( i = g_pGoStop->m_Hwatu.m_pCardPack->m_nReadNo; i < g_pGoStop->m_Hwatu.m_pCardPack->m_nCardCnt; i++ ) {
							    if ( g_pGoStop->m_Hwatu.m_pCardPack->m_pCardPack[i] == l_sUserChange.bChangeCardNo ) {
                                    bExistPackCard = true;
                                    nPackCardIdx = i;
								    break;
							    }
						    }

						    for ( i = 0; i < g_pGoStop->m_pFloor.m_pGamer[1].m_nCardCnt; i++ ) {
							    if ( g_pGoStop->m_pFloor.m_pGamer[1].m_bCard[i] == l_sUserChange.bUerCardNo ) {
                                    bExistUserCard = true;
                                    nUserCardIdx = i;
								    break;
							    }
						    }

                            if ( bExistPackCard && bExistUserCard ) {
                                // ī�� ��ȣ ��ȯ
                                g_pGoStop->m_Hwatu.m_pCardPack->m_pCardPack[nPackCardIdx] = l_sUserChange.bUerCardNo;
                                g_pGoStop->m_pFloor.m_pGamer[1].m_bCard[nUserCardIdx] = l_sUserChange.bChangeCardNo;

                                // ī�� ��ġ ��ȯ
                                nTempX = g_pCMainFrame->l_CardPos[l_sUserChange.bUerCardNo].Pos.x;
                                nTempY = g_pCMainFrame->l_CardPos[l_sUserChange.bUerCardNo].Pos.y;
                                g_pCMainFrame->l_CardPos[l_sUserChange.bUerCardNo].Pos.x = g_pCMainFrame->l_CardPos[l_sUserChange.bChangeCardNo].Pos.x;
                                g_pCMainFrame->l_CardPos[l_sUserChange.bUerCardNo].Pos.y = g_pCMainFrame->l_CardPos[l_sUserChange.bChangeCardNo].Pos.y;
                                g_pCMainFrame->l_CardPos[l_sUserChange.bChangeCardNo].Pos.x = nTempX;
                                g_pCMainFrame->l_CardPos[l_sUserChange.bChangeCardNo].Pos.y = nTempY;
                            }
                        }
						

						break;
				}
			}
		}
		Sleep(50);
	}
	return 0;
}

                        

DWORD WINAPI UserCardRecvThreadProc(LPVOID lpParameter)
{
	int nRecvUserCard;
	int nDivi;
	int nUserSlot;
	CNetworkSock *pClientSocket = (CNetworkSock *)lpParameter;

	while( g_bUserCardRecvThreadOn )	
	{
		if ( g_pGoStop->m_bOtherWait ) {

			if( pClientSocket->m_RecvCardInputQ.GetFromQ( &nRecvUserCard, &nDivi ) )
			{
				if ( nDivi == 2 ) {	//StartGame�̸� 
					//DTRACE("�׳� Ǯ���ش�." );
					g_pGoStop->SetOtherActionEvent();
				}
				else  {

					if ( nDivi == 1 ) { //��� ī���̸�. //����� ������ �̸� �־��ش�.
						memcpy( g_nCardNoArr, g_nRecvShakeCardArr, sizeof(g_nRecvShakeCardArr) );
					}

					
					nUserSlot = ( g_CurUser + g_pGoStop->m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;
					g_pGoStop->m_pFloor.m_pGamer[nUserSlot].m_bUserEvent = true;

					g_pGoStop->m_nOtherUserCard = nRecvUserCard;

					g_pCMainFrame->l_bShowDialog = FALSE;
					g_pCMainFrame->l_nDlgMode = 0;
					g_pCMainFrame->l_nDialogWaitFrameCnt = 0;
					g_pGoStop->m_bOtherWait = false;
					
					//DTRACE("OnRecvUserCard - ����� �׼� Ǯ�� ī���ȣ : %d", nRecvUserCard );
					g_pGoStop->SetOtherActionEvent();
				}
			}

		}

		Sleep(50);
	}
	return 0;
}
