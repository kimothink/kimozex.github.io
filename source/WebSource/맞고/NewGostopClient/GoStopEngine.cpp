#include "stdafx.h"
#include "MainFrm.h"
#include "GoStopEngine.h"
#include "dtrace.h"
#include "AlarmMsg.h"
#include <math.h>
#include "UserList.h"


CGoStopEngine *g_lpGE;


extern CGameUserList g_CGameUserList;
extern bool g_bUserCardRecv;
extern sIMAGEDATA g_sImageData[MAX_ROOM_IN];//�� ������ �����ִ� �̹���
extern int g_CurUser;
extern int g_MyUser;
extern int g_nCardNoArr[5];
extern CNetworkSock *g_pClient;
extern bool g_bDeal;
extern char g_szUserImage[MAX_ROOM_IN][MAX_IMAGE_LEN]; // ������ ������ �ִ� �׸� ( ���� )( 8 ����Ʈ )
extern int g_nViewPageNo; // ���� ���� �ִ� ������ �ѹ�
extern int g_nGameSpeed;
extern int g_nBombCardNo[3];
extern bool g_bBomb;
extern int g_nGoCnt;
extern CMainFrame *g_pCMainFrame;
extern int g_SelectCardNo;
extern int g_nCardPackNo;
extern sClientEnviro g_sClientEnviro;
extern int g_nGameCnt;
extern BigInt g_biSystemMoney;
extern sUserInfo g_ClickRoomUserInfo[MAX_ROOM_IN];

extern char g_szAvaUrl[MAX_AVAURLLEN];
extern char g_szGameGubun[5];

extern sAvatarPlayStr g_sAvatarPlayStr;


void ServerSrandom(unsigned long initial_seed)
{
	seed = initial_seed; 
}


unsigned long ServerRandom(void)
{
	int lo, hi, test;

	hi   = seed/q;
	lo   = seed%q;

	test = a*lo - r*hi;

	if (test > 0)
		seed = test;
	else
		seed = test+ m;

	return (seed);
}

int GetRandom(int from, int to)
{
	if (from > to) {
		int tmp = from;
		from = to;
		to = tmp;
	}

	return ((ServerRandom() % (to - from + 1)) + from);
}


DWORD WINAPI GostopEngineThread(LPVOID lpParameter)
{
	BYTE *pCard = g_lpGE->m_pRecvCardPack;
	BOOL bRe;

	try
	{
		bRe = g_lpGE->InitCardPack(pCard);
		if ( !bRe ) {	// ���� �̿��ڵ��� ī�� �ʱ�ȭ

			if ( g_lpGE->m_bGameStop == TRUE ) {  //�������ڸ��� ���ÿ� ������ ������
				g_lpGE->m_bGameStop = FALSE;
				return 0;
			}
			else {	
				g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 144", true );
				g_pClient->Destroy();
				return 0;
			}
		}


		if ( g_lpGE->m_pFloor.m_nWinerNo < 0 && !g_lpGE->StartDeal()  )		//���̸ӵ鿡�� �и� ������.
		{
			TRACE("\n .........-3");
			throw -2;
		}

		if ( g_lpGE->m_pFloor.m_nWinerNo < 0 && !g_lpGE->Round() ) 			// ���� ����
		{
			 TRACE("\n .........-3");
			throw -3;		
		}

		if ( g_lpGE->m_pFloor.m_nWinerNo >= 0 )		// winer  ����
			g_lpGE->EndingCheck();
		else 
			throw -4;

		g_lpGE->m_pFloor.m_nBaseMulti = 1;

		if ( g_lpGE->m_pFloor.m_nWinerNo == MASTER_POS )
			g_pCMainFrame->PlaySound(g_Sound[GS_WIN], false);
		else
			g_pCMainFrame->PlaySound(g_Sound[GS_LOST], false);

		g_pCMainFrame->GameBreak();

	}
	catch (...) 
	{
		//DTRACE("catch (...) catch (...) ");
		if ( !g_lpGE->m_pFloor.RealGame() && g_lpGE->m_pFloor.m_pGamer[0].m_ExitReserv ) {	//���Ͷ� ĥ��...���ڱ� ������ �ϱ� ���ؼ�
			g_lpGE->Stop();
			//DTRACE("catch (...)  - g_pClient->RoomOut()" );
			g_pClient->RoomOut();
			return 0;
		}

		int nGamerUserNo = -1;
		//�������� �����...���.
		if (  g_lpGE->m_bGameStop == false ) { //g_lpGE->m_bGameStop true�̸� ���̶� ĥ�� ������ ���°ű� ������..���� �ʿ����.
			if ( g_lpGE->m_pFloor.RealGame() ) { 
				for( int i = 0; i < MAX_ROOM_IN; i++ )
				{
					nGamerUserNo = g_lpGE->m_pFloor.m_pGamer[i].m_nGamerUserNo;
					g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nDraw_Num++;
				}
			}

			g_lpGE->m_pFloor.m_nBaseMulti = 2;
			//DTRACE("�������� �ι�� �����մϴ�.");

			g_pCMainFrame->PlaySound(g_Sound[GS_NAGARI + (g_CGameUserList.m_GameUserInfo[MASTER_POS].m_nVoice * 50)], false);
			//DTRACE("������");

			memset( g_pCMainFrame->l_nPVal, 0x00, sizeof(g_pCMainFrame->l_nPVal) );

			

		}
		g_pCMainFrame->GameBreak();
	}
	

	ResetEvent(g_lpGE->m_hActionEvent);
	g_lpGE->m_bGameEndWill = true;
	DWORD dwRe = WaitForSingleObject( g_lpGE->m_hActionEvent, WAIT_90TIME );	


	if ( dwRe == WAIT_TIMEOUT ) {
		g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 145", true );
		g_pClient->Destroy();
	}

	//DTRACE("Thread ��.");

	return 0;
}

CGoStopEngine::CGoStopEngine()
{
	ServerSrandom(time(0));

	g_lpGE = this;
	m_hMainWnd = NULL;
	m_nDouble = 0;
	m_nTriple = 0;
	m_biMoney = 0;
	m_bPlaying = false;
	m_bGameEndWill = false;


	m_dwGEThreadId = 0;
	m_hGEThread = NULL;
	m_bMasterMode = false;
	m_nSelectCard = -1;
	m_nNineCard = -1;
	m_nShake = -1;
	m_nOtherUserCard = -1;

	
	m_nRound = 0;
	m_nUserno = 0;
	m_nGoStop = 0;
	m_nPrizeNo = 0;

	m_pRecvCardPack = NULL;
	m_bMyKetInput = false;
	m_bOtherWait = false;
	m_bGameStop = false;
	m_bCardPackService = false;


	m_nMissionType = KKWANG;
	m_nMissionMultiply = 1;

	m_nBaseMulti = 1;
	
    m_hPaintEvent	= CreateEvent(NULL, false, true, NULL);
	m_hActionEvent	= CreateEvent(NULL, false, true, NULL);
	m_hOtherUserActionEvent	= CreateEvent(NULL, false, true, NULL);
	m_hDrawEvent	= CreateEvent(NULL, false, true, NULL);
}

CGoStopEngine::~CGoStopEngine()
{
    if (m_hPaintEvent) 
        CloseHandle(m_hPaintEvent);

    if (m_hActionEvent) 
        CloseHandle(m_hActionEvent);

    if (m_hOtherUserActionEvent) 
        CloseHandle(m_hOtherUserActionEvent);

    if (m_hDrawEvent) 
        CloseHandle(m_hDrawEvent);
}

void CGoStopEngine::EventSync( int nSlotNo, int nEventDivi, int nSoundDivi, int nDelayTime )
{
	ResetEvent(m_hDrawEvent);

    

    // �� ������ ������ ������ �������� �÷������ش�.
    if ( nSoundDivi >= 50 && nSoundDivi <= 82 ) {
        nSoundDivi = nSoundDivi + g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50;
    }
    


	if ( nSoundDivi > 0 )
		g_pCMainFrame->PlaySound( g_Sound[nSoundDivi], false );

	g_pCMainFrame->Effect( nSlotNo, nEventDivi, nDelayTime );

	WaitForSingleObject( m_hDrawEvent, WAIT_PAINT_TIME );
}

void CGoStopEngine::DrawEndEvent()
{
	SetEvent(m_hDrawEvent);
}	

void CGoStopEngine::SetWndHandle( HWND aHwnd )
{

	m_hMainWnd = aHwnd;

}

void CGoStopEngine::SetPaintEvent()
{
	SetEvent(m_hPaintEvent);
}

bool CGoStopEngine::InitCardPack(BYTE *pCard)
{
	//���⼼�� ������ �޼����� ��ٸ���.
	if ( m_pFloor.RealGame()  ) {
		//DTRACE("�ٸ������ �غ� ��ٸ���.");
		

		//������ �غ� ��Ŷ�� ������..
		if ( m_bMasterMode != TRUE ) {
			//DTRACE("������ �غ� ��Ŷ�� ������..");
			g_pClient->GameReady();
		}

		
		//Ȥ�� ���뿡 ����� ���������� �ִ�.
		if ( m_pFloor.RoomUserCntOK() == false ) {
			m_bGameStop = TRUE;
			return false;
		}


		//DTRACE("InitCardPack - �ٸ���� �׼Ǳ�ٸ�");
		ResetEvent(m_hOtherUserActionEvent);
		m_bOtherWait = true;
		DWORD dwRe = WaitForSingleObject(m_hOtherUserActionEvent, WAIT_STARTTIME);	//���濡�Լ� �޼����� �ö����� ��ٸ���.
		if ( dwRe == WAIT_TIMEOUT || m_bGameStop == TRUE ) {
			return false;
		}
	}
	
	g_pCMainFrame->PlaySound(g_Sound[GS_CARDPACKSTART], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

	if ( m_bMasterMode ) {
	}
	else {
		m_Hwatu.SetCardPack( pCard );
	}

	return true;
}


bool CGoStopEngine::StartDeal()
{
	int i;
	int nCardNo;
	int nCardPos;
	bool bMade;			//����� �Ǵ�.
	int nRecvCardNo;

	Sleep(300);
	
	for ( int cnt = 0 ; cnt < 2 && m_bPlaying ; cnt++ )				//�и� �ι��� ���ļ� ���༭ �ش�...3���� 4�徿�ְ� ������ 3���� 3���� �ش�..�׷��� ���Ⱑ �� �ι��̴�.
	{
		g_nGameSpeed = 8;
		
		g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOSHARE], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

		for (i = 0 ; i < 4 && m_bPlaying ; i++ )						//�ٴڿ� �и� ���...4�徿 ���.
		{
			if ( ( nCardNo = m_Hwatu.GetCard() )  < 0 )
				return false;

			AddSlowCardFloor(nCardNo);
		}

		Sleep(300);

		for (int u = 0 ; u < MAX_ROOM_IN && m_bPlaying ; u++)					// 2���� �и� �����ֱ� ������ 2������.
		{
			g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOSHARE], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

			for (i = 0 ; i < 5 ; i++)					// �Ѹ�� 5���� �ְ�
			{
				if ( ( nCardNo = m_Hwatu.GetCard() )  < 0 )
					return false;

				AddSlowCardUser(u+1, nCardNo);

			}

			Sleep(300);

		}
	}

	g_bDeal = false;
	g_nGameSpeed = 4;

	SortAndRePaint(4);	//��� ��Ʈ

	while((nCardPos = m_pFloor.CheckNephew()) >= 0 && nCardPos != 0xff && m_bPlaying)			//ó���� �и� �����ְ� �ٴۿ� �����а� ������.
	{
		EatCardUser(0, nCardPos, nRecvCardNo);

		if ((nCardNo = m_Hwatu.GetCard()) == 0xff || nCardNo < 0)								//���� ī�� �̻����� üũ
			return false;
		g_pCMainFrame->SortUserEatCard( (m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN );

		AddCardFloor( nCardNo );
	}

	int nPregidentCardNo = -1;
	bMade = SortFloor(nPregidentCardNo);													//�ٴڿ� �� �е��� ���� ���� ��� �׸���.  �̰��� true�̸� ������̴�.

	if (bMade)					// �ٴڿ� ������̸�.
	{
		m_pFloor.m_nWinerNo = m_pFloor.m_nMasterSlotNo;
		m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEvalution = PREGIDENT_POINT;
		
		m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nResultScore = PREGIDENT_POINT;
		m_pFloor.m_nPregidentNo = 0;		//���� ������� ������..

		g_pCMainFrame->PlaySound(g_Sound[GS_CHONGTONG + (g_CGameUserList.m_GameUserInfo[m_pFloor.m_nWinerNo].m_nVoice * 50)], false);

		//���⼱ ���̾�αװ� 3���Ŀ� �װ� �̰�Ǭ��.
		//������ ���â���� �ٽ� �ɾ������. ���� Ÿ�̸Ӵ� �ʿ����.
		//���߿� Ȥ�� �𸣴ϱ� 10�������� �ص��ȴ�. ���â���� �� �ɱ� ������..
		ResetEvent(m_hActionEvent);
		//DTRACE("(3) - CB_PREGIDENT �� ������ �ٴڿ� �����..");
		//m_nSelect X

		//����� �޼����� �ѷ��ش�.
		g_pCMainFrame->PregiDent( nPregidentCardNo );


		DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12�� �Ŀ�..
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 146", true );
			g_pClient->Destroy();
		}
		

		return m_bPlaying;

	}

	// ������ ������� ����� ��!!

	for (i = 0 ; i < MAX_ROOM_IN ; ++i)
	{
		int s = ( i + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;
		nCardNo = m_pFloor.m_pGamer[s].IsPregident();
		
		if (nCardNo >= 0)
		{
			m_pFloor.m_nWinerNo = s;
			m_pFloor.m_nPregidentNo = s;
			m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEvalution = PREGIDENT_POINT;

			m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nResultScore = PREGIDENT_POINT;
			
			g_pCMainFrame->PlaySound(g_Sound[GS_CHONGTONG + (g_CGameUserList.m_GameUserInfo[s].m_nVoice * 50)], false);



			//���⼱ ���̾�αװ� 3���Ŀ� �װ� �̰�Ǭ��.
			//������ ���â���� �ٽ� �ɾ������. ���� Ÿ�̸Ӵ� �ʿ����.
			//���߿� Ȥ�� �𸣴ϱ� 10�������� �ص��ȴ�. ���â���� �� �ɱ� ������..
			ResetEvent(m_hActionEvent);

			g_pCMainFrame->PregiDent( nCardNo );

			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12�� �Ŀ�..
			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 147", true );
				g_pClient->Destroy();
			}

			return m_bPlaying;
		}
	}
	
	return m_bPlaying;
}



void CGoStopEngine::InitMission()
{
	g_pCMainFrame->PlaySound(g_Sound[GS_MISSION_START], false);


	m_nMissionType = MISSION( GetRandom( 1, 18 ) ); //�׷��� 1���� �̴�.

	switch(m_nMissionType)
	{
		case KKWANG: // ���ϰ�� 1��
			m_nMissionMultiply = 1;
			break;

		case JAN:
		case FEB:
		case MAR:
		case APR:
		case MAY:
		case JUN:
		case JUL:
		case AUG:
		case SEP:
		case OCT:
		case NOV:
		case DEC: // 1�� ���� 12�� ������� 3��
			m_nMissionMultiply = 3;
			break;

		case HONGDAN:
		case CHONGDAN:
		case CHODAN:
		case GODORI: // ȫ��, û��, �ʴ�, ���� ������� 4��
			m_nMissionMultiply = 4;
			break;

		case OKWANG: // 5�� ������� 5��
			m_nMissionMultiply = 5;
			break;

		case DAEBAK: // ����� 5 ~ 10��
			m_nMissionMultiply = GetRandom( 5, 10 );
			break;
	}
}



void CGoStopEngine::SoundEffectDivi( int nGoCnt, int nSoundDivi )
{
	// 0 : ȭ�� ��ġ�� �Ҹ�(�¾�����)
	switch(nSoundDivi) {
		case 0:
			switch(nGoCnt) {
				case 0:
					g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMATCH], false);
					break;

				case 1:
					g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMATCH_1GO], false);
					break;

				case 2:
					g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMATCH_2GO], false);
					break;

				default:
					g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMATCH_3GO], false);
					break;


			}

			break;
	}

}

bool CGoStopEngine::Round()
{
	int nUserFloorPos, nPackEatFloorPos, nOtherFloorPos;
	bool bUserEat, bCenterEat;
	int nUserCardNo, nCenterCardNo, nCardNo;
	int nSlotNo;
	int nGameUserNo;

	for (m_nRound = 0 ; m_nRound < 10 && m_bPlaying ; ++m_nRound)
	{
		if( m_nRound == 1 && m_bMasterMode )
		{
			InitMission(); // �̼� �ʱ�ȭ

            //��������.
			if ( m_nMissionType == KKWANG )
				m_nPrizeNo = PIRZECNT - 1;		//������ȸ��.
			else
				m_nPrizeNo = GetRandom( 0, PIRZECNT - 2 );	//������ȸ���� �������� ���ܵǱ⶧����.

			g_pClient->MissionSet( m_nMissionType, m_nMissionMultiply, m_nPrizeNo );
			
			// �ι�° �Ͽ��� �̼� �����ֱ�
			g_pCMainFrame->ShowMission();
		}

		for (m_nUserno = 0 ; m_nUserno < MAX_ROOM_IN && m_bPlaying ; m_nUserno++)
		{

			g_CurUser = m_nUserno;
			
			if ( m_bGameStop ) 
				return 0;

			g_pCMainFrame->PlaySound(g_Sound[GS_MYORDER], false);
			
			nSlotNo = ( m_nUserno + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;

			if ( (nUserCardNo = SelectCard(m_nUserno, nUserFloorPos, bUserEat) ) < 0 )
			{
				if ( m_pFloor.m_nWinerNo < 0 )
					return false;
				
				return true;
			}

			nOtherFloorPos = -1;	//������ �и� ������ ���� ȭ���� ������ ���� ��ġ.									
			do	// ������ ���µ�..�����а� ���ð��.
			{
				if (( nCardNo = m_Hwatu.GetCard() ) == 0xff || nCardNo < 0)
					return false;

				if ( nCardNo >= 48 && nCardNo != BOMB_CARDNO  )		//��źī��� ����.
				{
					UpCard( -1, nCardNo );		// ī�带 ��¤�� ( -1�̸� ī���ѿ��� �����°Ŵ�. )
					GetOnePee(nSlotNo); // Edited by shkim

					m_bCardPackService = true;
					if ( bUserEat )
					{
						//============SOUND==============
						SoundEffectDivi( g_nGoCnt, 0 );

						ThrowFloorCard( m_nUserno, nCardNo );
					}
					else
					{
						if ( nOtherFloorPos < 0)	//ó���� �����и� ���� ��ġ.
							nOtherFloorPos = MAX_CARD_POS - 1;
						
						m_pFloor.m_pGamer[nSlotNo].m_nChoiceFloorCardPos = nOtherFloorPos;
						g_pCMainFrame->PlaySound(g_Sound[GS_MISS], false);
						ThrowFloorCard( m_nUserno, nCardNo );
					}
				}
			} while( nCardNo >= 48 && nCardNo != BOMB_CARDNO );


	
			// ������ �и� ���� ��ġ�� ã��. nCenterCardNo : �ѿ����� ī��.
			nCenterCardNo = nCardNo;
			
			UpCard( -1, nCenterCardNo );   	// ī�带 ��¤�� ( -1�̸� ī���ѿ��� �����°Ŵ�. )

			m_bCardPackService = false;

			bCenterEat = ThrowFloor( nSlotNo, nCenterCardNo,					
										(bUserEat) ? nUserFloorPos : -1, m_nRound, nPackEatFloorPos );//nCenterFloorPos );

			m_pFloor.m_pGamer[nSlotNo].m_nChoiceFloorCardPos = nPackEatFloorPos;
			ThrowFloorCard( m_nUserno, nCenterCardNo );
			
			Sleep(250);

			if (m_nRound == 9 && m_nUserno == 1)		// ���ǿ� ����ī�尡 ������ �� �Ѹ���. ( ����ī�� )
			{
				while((nCardNo = m_Hwatu.GetCard()) != 0xff && nCardNo >= 0)
				{
					UpCard( -1, nCardNo );
					if (nCardNo < 48)
						return false;

					SoundEffectDivi( g_nGoCnt, 0 );

					ThrowFloorCard( m_nUserno, nCardNo );
					Sleep(250);
				}
			}

			//�Ծ��µ� �ҋ�.
			if ( bUserEat && nUserFloorPos == nPackEatFloorPos ) { //������ �ٴ��и� �Ծ��µ� ��¤���ѵ� �������϶�.

				EventSync( nSlotNo, 16, GS_SSAM );

				int nBbukCnt = ++m_pFloor.m_pGamer[nSlotNo].m_nBbukCnt;

				//������ ���� ī�带 ��Ƶд�..
				for ( int nTempBbuk = 0; nTempBbuk < 10; nTempBbuk++ ) {
					if ( m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] == -1 ) {
						m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] = nCenterCardNo / 4;
						break;
					}
					
				}

				// ó���� üũ, ������ üũ.(����)
				if ( nBbukCnt <= 3 && nBbukCnt == m_nRound + 1 ) { 					
	
					BigInt bival = (BigInt)pow( 2, ( nBbukCnt - 1 ) ) * BBUK_X * m_biMoney;
					
					BigInt bimoney = 0;
					int nNextUserSlotNo = 0;

					for ( int i = 1 ; i < MAX_ROOM_IN ; ++i )		//�ٸ� ����ڵ鿡�� ���� ���� ���´�.
					{
						//����....�̰͵�.......������ �ΰ����̳�..�Ф�
						nNextUserSlotNo = ( i + nSlotNo ) % MAX_ROOM_IN;

						if ( m_pFloor.RealGame()  ) {
							m_pFloor.UserMoneyVal( nNextUserSlotNo, bival, false );
							m_pFloor.m_pGamer[nNextUserSlotNo].m_biCurWinLoseMoney -= bival;
							TRACE("%d\n",bival);
						}

						bimoney += bival;
					}


					if ( nBbukCnt == 3 ) {
						bimoney = GameSystemMoney( bimoney );
					}
					
					if ( m_pFloor.RealGame()  ) {
						if (  bimoney > 0 ) {
							m_pFloor.UserMoneyVal( nSlotNo, bimoney, true );
							m_pFloor.m_pGamer[nSlotNo].m_biCurWinLoseMoney += bimoney;
								TRACE("%d\n",bival);
						}
					}				
					
					if ( nBbukCnt == 1 ) {
						
						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{						
							g_pCMainFrame->PutTextInGame( "Com1" , "ù��" , 
							bival , RGB( 255 , 255 , 165 ) );						
						}
						else
						{							
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
							
							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "ù��" , 
								bival , RGB( 255 , 255 , 165 ) );
						}
					}
					else if( nBbukCnt == 2 )
					{
						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "����" , 
												bival , RGB( 255 , 255 , 165 ) );						
						}
						else
						{
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "����" , 
													bival , RGB( 255 , 255 , 165 ) );
						}
					}
					else if ( nBbukCnt == 3 )
					{
						m_pFloor.m_nWinerNo = nSlotNo;

						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "��������" , 
													bival ,  RGB( 255 , 255 , 165 ) );						
						}
						else
						{
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "��������" , 
													bival , RGB( 255 , 255 , 165 ) );
						}
						m_pFloor.m_pGamer[nSlotNo].m_biWinMoney = bimoney;
						return true;
					}

				}
				else if ( nBbukCnt == 3 ) { 
					m_pFloor.m_nWinerNo = nSlotNo;
					m_pFloor.m_pGamer[nSlotNo].m_nEvalution = THREEBBUCK_POINT;

					m_pFloor.m_pGamer[nSlotNo].m_nResultScore = THREEBBUCK_POINT;

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{
						g_pCMainFrame->PutTextInGame( "Com1" , "������" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );					
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "������" , 
													0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
					return true;
				}
				else {

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{	
							g_pCMainFrame->PutTextInGame( "Com1" , "��" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );					
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "��" , 
													0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
				}
			}
			else
			{
				//�Ծ��µ�...����.
				if ( bUserEat && 
				    nUserFloorPos != nPackEatFloorPos &&
				    bCenterEat && 
				    nUserCardNo / 4 == nCenterCardNo / 4 && m_nRound != 9 ) {	//����.

					EventSync( nSlotNo, 17, GS_DADDAK );
					//ù����...ù ������ ���ش�.
					if ( m_nRound == 0  ) {//ù ��.

						BigInt bival = BBUK_X * m_biMoney;
						BigInt bimoney = 0;
						int nNextUserSlotNo = 0;

						if ( m_pFloor.RealGame() ) {
							for ( int i = 1 ; i < MAX_ROOM_IN ; ++i )		
							{
								nNextUserSlotNo = ( i + nSlotNo ) % MAX_ROOM_IN;
								m_pFloor.UserMoneyVal( nNextUserSlotNo, bival, false );
								bimoney += bival;

								m_pFloor.m_pGamer[nNextUserSlotNo].m_biCurWinLoseMoney -= bival;
							}

							if ( bimoney > 0 ) {
								m_pFloor.UserMoneyVal( nSlotNo, bimoney, true );

								m_pFloor.m_pGamer[nSlotNo].m_biCurWinLoseMoney += bimoney;
							}						
						}

						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "ù ����" , 
							bival , RGB( 255 , 255 , 165 ) );
						}
						else
						{							
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
							
							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "ù ����" , 
								bival , RGB( 255 , 255 , 165 ) );
						}
					}
					else
					{
						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "����" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );
						}
						else
						{							
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
							
							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "����" , 
								0, RGB( 255 , 255 , 165 ) , FALSE );
						}
					}

					
					//DTRACE(" %d ���� ���� ", m_nUserno );

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

				}
				

				if ( !bUserEat && m_nSelectCard != BOMB_CARDNO && // ���� ��ź�� ���� �ѿ��� �ٷ� ������..�ٴڿ� ������ ���̵Ǿ BOMB_CARDNOüũ�� �־���.
					 nUserFloorPos == nPackEatFloorPos && m_nRound != 9 ) {	// ���϶�..���� ���� �и� �������� �ʴ´�.

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					EventSync( nSlotNo, 15, GS_JJOK );

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{
						g_pCMainFrame->PutTextInGame( "Com1" , "��" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
						
						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "��" , 
							0, RGB( 255 , 255 , 165 ) , FALSE );
					}
				}

				// ���� ȭ�� ����
				int cnt1 = 0, cnt2 = 0;
				int nCardNo1, nCardNo2;

				if ( bUserEat )		//�ڽ��� ������..��������
					cnt1 = EatCardUser( m_nUserno, nUserFloorPos, nCardNo1 );
				
				if ( nOtherFloorPos >= 0 ) //���� �и� ��¤������ �����;��ϹǷ�..
					EatCardUser( m_nUserno, nOtherFloorPos, nCardNo );
				
				if ( bCenterEat )	//��¤�� �а� ������ ��������.
					cnt2 = EatCardUser( m_nUserno, nPackEatFloorPos, nCardNo2 );

				// ��ź or �Ѱ� ����
				if ( cnt1 == 4 && m_nRound != 9 )  {   //�ڽ��� �����а� 4���� �������� ���徿 

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					//������ ���� ī�带 ��Ƶд�..
					for ( int nTempBbuk = 0; nTempBbuk < 10; nTempBbuk++ ) {
						if ( m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] == nCardNo1  ) {

							for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
								m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
							}

							m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] = -1;

							break;

						}
					}
				}

				if ( cnt2 == 4 && m_nRound != 9 )  {	  //�ѿ��� ��¤���а� 4���� �������� ���徿.

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					//������ ���� ī�带 ��Ƶд�..
					for ( int nTempBbuk = 0; nTempBbuk < 10; nTempBbuk++ ) {
						if ( m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] == nCardNo2  ) {

							for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
								m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
							}

							m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] = -1;

							break;
						}
					}
				}

				if (m_pFloor.m_nCardCnt == 0 && m_nRound != 9)	// ���ϸ� ���徿 �����´�.,
				{
					EventSync( nSlotNo, 20, GS_PANSL, 100 );
					
					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{						
						g_pCMainFrame->PutTextInGame( "Com1" , "�Ǿ�" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );					
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "�Ǿ�" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
				}

				GetPeeEachGamer( nSlotNo ); 	

				for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
					m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt = 0;
				}

			}		


			if ( m_pFloor.m_pGamer[nSlotNo].Evalutaion() )	//�������.
			{
				if ( SelectNineCardPee(m_nUserno) )	//������ ��ٸ���.
				{
					ResetEvent(m_hPaintEvent);
					m_pFloor.m_pGamer[nSlotNo].MoveNineCard();

					g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMOVE], false);
					
					g_pCMainFrame->OnNineCardMove( nSlotNo, 0 );

					WaitForSingleObject(m_hPaintEvent, WAIT_PAINT_TIME);

					g_pCMainFrame->SortUserEatCard( nSlotNo );

				}
				else
					m_pFloor.m_pGamer[nSlotNo].Evalutaion();

			}
		
			if (m_pFloor.m_pGamer[nSlotNo].CheckEvalutaion())		//�� �Ҽ� �ִ��� üũ.
			{
				

				if ( m_nRound == 9 || WaitGoStop(m_nUserno) == 0 )
				{
					m_pFloor.m_nWinerNo = nSlotNo;

					EventSync( nSlotNo, 9, GS_STOP );

					return true;
				}

				int ngoCnt = m_pFloor.m_pGamer[nSlotNo].AddGo();
				
				m_nLastGo = nSlotNo;

				g_pCMainFrame->PlaySound(g_Sound[(GS_GO+ngoCnt-1)  +  (g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50)], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)				
				

				//===========BGM SOUND=====================
				//���� ����..
				if ( ngoCnt >= g_nGoCnt )
					g_nGoCnt = ngoCnt;


				EventSync( nSlotNo, ngoCnt - 1, -1 );


				char szJunk[255];
				memset( &szJunk , 0x00 , sizeof( szJunk ));

				sprintf( szJunk , "%d��" , ngoCnt );

				if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
				{					
					g_pCMainFrame->PutTextInGame( "Com1" , szJunk , 
						0 , RGB( 255 , 255 , 165 ) , FALSE );				
				}
				else
				{
					nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

					g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , szJunk , 0 , RGB( 255 , 255 , 165 ) , FALSE );
				}					
			}

			m_nGoStop = 0;
  
		}
	}

	return m_bPlaying;
}

void CGoStopEngine::AddCardFloor(int nCardNo)
{
	g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOFRPUT], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

	m_pFloor.OnlyAdd(nCardNo);
}

void CGoStopEngine::AddCardUser(int nUserNo, int nCardNo)
{
	g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOSHARE], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

	int slotNo = m_pFloor.AddCardUser(nUserNo, nCardNo);
	
	ResetEvent(m_hPaintEvent);

	g_pCMainFrame->OnUserCardPaint( nCardNo, slotNo );

    WaitForSingleObject(m_hPaintEvent, WAIT_PAINT_TIME);
}

void CGoStopEngine::AddSlowCardFloor(int nCardNo)
{

	m_pFloor.OnlyAdd(nCardNo);
}

void CGoStopEngine::AddSlowCardUser(int nUserNo, int nCardNo)
{


	int slotNo = m_pFloor.AddCardUser(nUserNo, nCardNo);
	g_pCMainFrame->OnUserCardPaint( nCardNo, slotNo );
}


void CGoStopEngine::GameCreate( int nDouble, int nTriple, BigInt biMoney )
{
	m_biMoney = biMoney;
	m_nDouble = nDouble;
	m_nTriple = nTriple;

	m_Hwatu.ReCreate( nDouble, nTriple );
}

BOOL CGoStopEngine::GameThreadCK()
{
	//1�ʸ� ��ٸ��� �ð����� ���̳��� �����带 ���� �ֱ��.
	DWORD dwRe = WaitForSingleObject( m_hGEThread, WAIT_1TIME ); 	
	if ( dwRe == WAIT_TIMEOUT ) {
		
		SetPaintEvent();
		SetActionEvent();
		SetOtherActionEvent();

		m_bGameStop = true;	
	}

	//�ѹ��� üũ�� �Ѵ�.
	dwRe = WaitForSingleObject( m_hGEThread, WAIT_5TIME ); 
	if ( dwRe == WAIT_TIMEOUT ) {
		g_pCMainFrame->KillThread(m_hGEThread);
	}


	m_bGameStop = false;

	return true;

}

bool CGoStopEngine::Start()
{	
	if ( m_biMoney <= 0 )
		return false;

	int nRoomSeed = 0;

	
	//���� �Ǽ��� ������ �÷��ش�. �ʱ�ȭ�� ���Ƿ� ���� ������.
	g_nGameCnt++;

	m_bPregident = FALSE;

	g_pCMainFrame->m_nDojangShow = 0;
	g_pCMainFrame->m_nDojangCount = 0;
	g_pCMainFrame->m_bDojangSound = FALSE;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		g_CGameUserList.m_GameUserInfo[i].m_biLoseMoney = 0;
		g_CGameUserList.m_GameUserInfo[i].m_biObtainMoney = 0;
	}

	g_pCMainFrame->m_bSunSet = FALSE;
	g_pCMainFrame->m_nSunSetRender = 0;
	g_pCMainFrame->m_bSunSetRender = FALSE;

	g_pCMainFrame->m_nSunRenderDecided = 0;
	g_pCMainFrame->m_bRenderSun = FALSE;
	
	m_nMissionType = KKWANG;


	// ������ ���۵Ǹ�..
    g_pCMainFrame->AddGameChatText( "�Ѱ��� ����" , RGB( 189 , 255 , 206 ));
	

	m_bPlaying = true;
	m_bGameStop = false;
	m_bOtherWait = false;
	m_bGameEndWill = false;

	
	g_bDeal = true;
	g_SelectCardNo = -1;
	g_nGoCnt = 0;
	g_pCMainFrame->l_nTempCardNo = -1;
	g_pCMainFrame->l_nCardFlipFrameCnt = 0;

    //����..�̰Ͷ��� ���´ʾ�����..������~~~~~~~�̤̤�..���ƾ��� ��Ӱŵ�....�Ф�..�׷��� �׽�Ʈ�� �ض�..
    m_nLastGo = -1;


	//ī���� �⺻ ������ ����.
	g_pCMainFrame->InitCardPostion();



	//����� ��� ��������...������ �ٵ��;� �Ѵ�.
	if ( m_bMasterMode == TRUE ) {

		//���� Thread�� ����ִ��� üũ�� �Ѵ�.
		GameThreadCK();

		m_Hwatu.Shuffle();
		nRoomSeed = GetRandom(1, 100000);
		srand(nRoomSeed);

		int nCardCnt = m_Hwatu.GetCardPack(m_pCardPack);

		if ( m_pFloor.RealGame() ) {	//�濡 �ڽ�ȥ�� �ְų� 3���� �� ���� Start()���⿡ �����Ƿ� ���⿡�� �ο��� üũ�ؾ��Ѵ�.

			//����� ����� �������� Ÿ�°��̹Ƿ�...
			if ( !m_pFloor.RoomUserCntOK() ) return false;	//

			for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
				m_pFloor.m_pGamer[i].m_bComputer = false;
				m_pFloor.m_pGamer[i].m_ExitReserv = false;
			}

			
			g_pClient->StartGame( nRoomSeed, nCardCnt, m_pCardPack );
		}
		else  {
			//DTRACE("********************5");
		}

		m_hGEThread = CreateThread( NULL, 0, GostopEngineThread, NULL, 0, &m_dwGEThreadId );
	}
	else {

		//���� Thread�� ����ִ��� üũ�� �Ѵ�.
		GameThreadCK();


		for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
			m_pFloor.m_pGamer[i].m_bComputer = false;
			m_pFloor.m_pGamer[i].m_ExitReserv = false;
		}

		m_hGEThread = CreateThread( NULL, 0, GostopEngineThread, NULL, 0, &m_dwGEThreadId );
	}

	return true;
}


void CGoStopEngine::SortAndRePaint( int nSortUserNo )
{
	if ( nSortUserNo > MAX_ROOM_IN ) {
		for(int i = 0 ; i < MAX_ROOM_IN ; ++i)
		{
			m_pFloor.SortUserCard(i);

			g_pCMainFrame->RepaintUserCard( i );

		}
	}
	else {
		m_pFloor.SortUserCard(nSortUserNo);
		g_pCMainFrame->RepaintUserCard( nSortUserNo );
	}

}

int CGoStopEngine::EatCardUser(const int nUserNo, const int nCardPos, int &nRecvCardNo)		// ī�带 ������ ���� ī�带 �������� �ִ�.
{

	int slotNo = (nUserNo + m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;
	
	int cnt = 0;
	nRecvCardNo = 0xff;

	for ( int i = m_pFloor.m_nCardCnt - 1 ; i >= 0 ; --i )
	{
		if ( m_pFloor.m_strCard[i].nPosNo == nCardPos )
		{
			if ( m_pFloor.m_strCard[i].nCardNo < REAL_CARD_MAXNO ) {
				
				cnt++;
				nRecvCardNo = m_pFloor.m_strCard[i].nCardNo / 4;
			}

			g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMOVE], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

			ResetEvent(m_hPaintEvent);

			g_pCMainFrame->OnFloorEat( i, slotNo);

		    WaitForSingleObject(m_hPaintEvent, WAIT_PAINT_TIME);
		}
	}

	g_pCMainFrame->SortUserEatCard( ( nUserNo + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN);
		
	m_pFloor.SortUserCard(nUserNo);

	SortAndRePaint( nUserNo );
	return cnt;

}

int CGoStopEngine::SelectCard(const int nUserNo, int &anFloorPos, bool &abEat)
{
	int nCardNo;
	int nChoiceUserCardPos = 0;

	abEat = false;
	int slotNo = (nUserNo + m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;
	int nGameUserNo;

	anFloorPos = 0xff;

	if ( !m_pFloor.m_pGamer[slotNo].m_bComputer  ) {

		if ( g_CurUser == g_MyUser ) {
			g_pCMainFrame->l_bTimeEvent = true;

			//DTRACE("(2)�ڱ� �׼Ǳ�ٸ�");
			ResetEvent(m_hActionEvent);

			g_pCMainFrame->UserCardSetTime(slotNo);

			m_bMyKetInput = true;
			
			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//60�� �Ŀ�..

			g_bUserCardRecv = true;
			
			if ( m_bGameStop ) return -1;
		}
		else {

			//DTRACE("(2)�ٸ���� �׼Ǳ�ٸ�");
			ResetEvent(m_hOtherUserActionEvent);

			g_pCMainFrame->UserCardSetTime(slotNo);

			m_bOtherWait = true; // SelectCard�Լ��� ���̳��� true�� ��������. ���񽺸� ������ �̸� �ٷ� �ü��� �ִ�.
			DWORD dwRe = WaitForSingleObject( m_hOtherUserActionEvent, WAIT_90TIME );	//12�� �Ŀ�..

			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 149", true );
				g_pClient->Destroy();
			}


			g_bUserCardRecv = true;
		}
		
		if ( m_pFloor.m_pGamer[slotNo].m_bUserEvent ) {	// 12�ʾȿ� ������� �޼����� ������ ���õ� ī��� �ٲ۴�.
			
			m_pFloor.m_pGamer[slotNo].m_bUserEvent = false;
			
			if ( g_CurUser == g_MyUser ) 
				nChoiceUserCardPos = m_nSelectCard;
			else
				nChoiceUserCardPos = m_nOtherUserCard;


			if ( nChoiceUserCardPos != BOMB_CARDNO ) {

				m_pFloor.m_pGamer[slotNo].UserSelect(nChoiceUserCardPos);
				anFloorPos = m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos;

			}
			else 				
				nChoiceUserCardPos = m_pFloor.m_pGamer[slotNo].GetPPokCardPos(); 

		}
		else {
			m_pFloor.m_pGamer[slotNo].AutoSelect();			//ī�带 �������ִ°� ����. ������ ������ ������ ��ȣ.

			nChoiceUserCardPos = m_pFloor.m_pGamer[slotNo].m_nChoiceUserCardPos;
			anFloorPos = m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos;

		}
	}
	else {

		Sleep(300);	//���Ͱ� ĥ���� ���� ��ٸ���.

		m_pFloor.m_pGamer[slotNo].AutoSelect();			//ī�带 �������ִ°� ����. ������ ������ ������ ��ȣ.

		nChoiceUserCardPos = m_pFloor.m_pGamer[slotNo].m_nChoiceUserCardPos;
		anFloorPos = m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos;
	}

    if ( nChoiceUserCardPos == 0xff )
		return -1;

	nCardNo = m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos];

	g_SelectCardNo = nCardNo;

	if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState == NOTICE_THREE ) {

		if ( m_pFloor.m_pGamer[slotNo].m_bComputer == false &&
			 g_MyUser == g_CurUser ) {	//������ �ִ°� ������ 3���̸� , �ڱ��ڽ��� �ƴϸ� ���ʿ� ����.
			
			int nTempCnt = 0;
			memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );
			for (int i = 0 ; i < m_pFloor.m_pGamer[slotNo].m_nCardCnt ; ++i)
			{
				if (m_pFloor.m_pGamer[slotNo].m_bCard[i] / 4 == nCardNo / 4)
				{
					g_nCardNoArr[nTempCnt] = m_pFloor.m_pGamer[slotNo].m_bCard[i];
					nTempCnt++;
				}
			}
				
			ResetEvent(m_hActionEvent);
			//DTRACE("ShakeQ() - ��ٸ���");

			g_pCMainFrame->ShakeQ();

			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//60�� �Ŀ�..
            
			//DTRACE("Ǯ�ȴ�.");
			if ( dwRe == WAIT_TIMEOUT ) {
				m_nShake = 1;
				g_pCMainFrame->l_bShowDialog = FALSE;	
			}

			if ( m_nShake == 1 ) {
				g_pClient->SendShake( nChoiceUserCardPos, g_nCardNoArr );	//���� ���õ� ī�带 Ŭ���̾�Ʈ���� �˷��ش�.

				m_pFloor.m_pGamer[slotNo].m_nUserThreeState = NOTICE_SHAKE;

				//DTRACE("������ 1");
				
				g_pCMainFrame->PlaySound(g_Sound[GS_SHAKE + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

				//DTRACE("������ 2");

				ResetEvent(m_hActionEvent);

				//DTRACE("������ 3");
				///*************************************
				//���⿡�� �������ϴ� �׸��� �����ش�.
				//*************************************	
				g_pCMainFrame->Shake();

				//DTRACE("������ 4");
				DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12�� �Ŀ�..

				//DTRACE("������ 5");
				if ( dwRe == WAIT_TIMEOUT ) {
					g_pCMainFrame->l_bShowDialog = FALSE;
					//DTRACE("���� �����?");						
				}

				memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );
				//DTRACE("*************1***********");

			}
		}
		else if ( m_pFloor.m_pGamer[slotNo].m_bComputer == true ) {			//������ �ִ°� ������ 3���̸�


			int nTempCnt = 0;
			memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );
			for (int i = 0 ; i < m_pFloor.m_pGamer[slotNo].m_nCardCnt ; ++i)
			{
				if (m_pFloor.m_pGamer[slotNo].m_bCard[i] / 4 == nCardNo / 4)
				{
					g_nCardNoArr[nTempCnt] = m_pFloor.m_pGamer[slotNo].m_bCard[i];
					nTempCnt++;
				}
			}


			m_pFloor.m_pGamer[slotNo].m_nUserThreeState = NOTICE_SHAKE;
			
			
			g_pCMainFrame->PlaySound(g_Sound[GS_SHAKE + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

			ResetEvent(m_hActionEvent);

			//*************************************
			//���⿡�� �������ϴ� �׸��� �����ش�.
			//*************************************	
			g_pCMainFrame->Shake();	


			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12�� �Ŀ�..
			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_bShowDialog = FALSE;	
			}
		}
		else if ( g_nCardNoArr[0] >= 0 ) {

			m_pFloor.m_pGamer[slotNo].m_nUserThreeState = NOTICE_SHAKE;

			
			g_pCMainFrame->PlaySound(g_Sound[GS_SHAKE + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

			ResetEvent(m_hActionEvent);

			//*************************************
			//���⿡�� �������ϴ� �׸��� �����ش�.
			//*************************************	
			g_pCMainFrame->Shake();


			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_90TIME );	//12�� �Ŀ�..
			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 152", true );
				g_pClient->Destroy();
			}
		}
	}
	memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );

	if ( g_CurUser == g_MyUser ) { // �ڽ��̸� �ٸ�������� ī�弱���� �˸���. ����� ������ ������.
		if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState != NOTICE_SHAKE ) {
			//DTRACE("����� ī�� ��ȣ ���� : %d", m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos]);
			g_pClient->SendUserCard( nChoiceUserCardPos );
		}
	}

	int nTempCurNo = SelectFloorTwoPee( slotNo, nCardNo, m_nRound, -1 );
	
	

	if ( nTempCurNo != 0xff ) {
		anFloorPos = m_pFloor.m_strCard[nTempCurNo].nPosNo;
		m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos = anFloorPos;
	}

	//�ٴڿ� �а� �ΰ� �ִ��� üũ
	if ( nCardNo < 48 && nCardNo >= 0 && m_pFloor.m_pGamer[slotNo].m_nUserThreeState != NOTICE_BOOM ) { //��ź�϶� �Ʒ��� ��ī�带 ���ش�.
		UpCard( nUserNo, nChoiceUserCardPos );
	}
	
	if ( m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos] >= 48 ) {
		if ( nCardNo != BOMB_CARDNO )			//�����а� ���õǾ�����...
		{
			EatMyCardUser( nUserNo, nChoiceUserCardPos ); // "����~"


            // ���� �и� ����ϸ� ������ �и� �� �� �����´�.
            GetOnePee( slotNo ); // ham 2006.02.22 [*PEE]


			// ���� ī�忡 ���� ������� �ɼ� ����.
			int nPregCardNo = m_pFloor.m_pGamer[slotNo].IsPregident();

			if ( nPregCardNo >= 0 )		// 0���� ũ�� ������̴�.
			{
				m_pFloor.m_nWinerNo = slotNo;
				m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEvalution = PREGIDENT_POINT;

				m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nResultScore = PREGIDENT_POINT;
				
				//������϶� ó���� �ؾ���....
				m_pFloor.m_nPregidentNo = nUserNo;

				g_pCMainFrame->PlaySound(g_Sound[GS_CHONGTONG + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

				//���⼱ ���̾�αװ� 3���Ŀ� �װ� �̰�Ǭ��.
				//������ ���â���� �ٽ� �ɾ������. ���� Ÿ�̸Ӵ� �ʿ����.
				//���߿� Ȥ�� �𸣴ϱ� 10�������� �ص��ȴ�. ���â���� �� �ɱ� ������..
				ResetEvent(m_hActionEvent);
				//DTRACE("(3) - 2 - CB_PREGIDENT �� ������ �ٴڿ� �����..");


				g_pCMainFrame->PregiDent( nPregCardNo );


				DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12�� �Ŀ�..
				if ( dwRe == WAIT_TIMEOUT ) {
					g_pCMainFrame->l_bShowDialog = FALSE;
					//DTRACE("g_pCMainFrame->l_bShowDialog = FALSE - 4");
				}
		

				return -1;
			}

			//DTRACE("���񽺸� �ڱ� �п��� �ٲٸ�");

			Sleep(300);

			return SelectCard( nUserNo, anFloorPos, abEat );
		}
		else {	//��źī���̸�..ThrowCard���� ����..�������� �״��..
			
			m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos] = 0xff;

			m_pFloor.SortUserCard(nUserNo);
			SortAndRePaint( nUserNo );
			return BOMB_CARDNO;
		}

	}



	if ( anFloorPos == 0xff )							//������ ������..
	{

		if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState == NOTICE_SHAKE ) {		//������ ������ ��������.
			m_pFloor.m_pGamer[slotNo].m_nShakeCnt++;
		}

		g_pCMainFrame->PlaySound(g_Sound[GS_MISS], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)
			
		abEat = false;
		anFloorPos = m_pFloor.GetEmptyPos();
		m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos = anFloorPos;
		ThrowCard( nUserNo );

	}
	else												//�Ծ�����.
	{

		if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState == NOTICE_BOOM ) {			//������ �ִµ� ��ź�ϋ�.
			
			m_pFloor.m_pGamer[slotNo].m_nShakeCnt++;

			g_pCMainFrame->Effect( slotNo, 21 );

			int nTempCnt = 0;
			for (int i = 0 ; i < m_pFloor.m_pGamer[slotNo].m_nCardCnt ; ++i)
			{
				if ( m_pFloor.m_pGamer[slotNo].m_bCard[i] / 4 == nCardNo / 4 )
				{
					UpSlowCard( nUserNo, i );

					g_nBombCardNo[nTempCnt] = m_pFloor.m_pGamer[slotNo].m_bCard[i];
					nTempCnt++;					

					Sleep(10);
				}
			}
			Sleep(200);

			for ( i = 0 ; i < m_pFloor.m_pGamer[slotNo].m_nCardCnt ; ++i)
			{
				if ( m_pFloor.m_pGamer[slotNo].m_bCard[i] / 4 == nCardNo / 4 )
				{
					m_pFloor.m_pGamer[slotNo].m_nChoiceUserCardPos = i;

					Sleep(20);
					ThrowCard( nUserNo );
					g_pCMainFrame->PlaySound( g_Sound[GS_BOOM], false );
	
				}
			}
			

			
			m_pFloor.m_pGamer[slotNo].RcvCard(BOMB_CARDNO);
			m_pFloor.m_pGamer[slotNo].RcvCard(BOMB_CARDNO);
			m_pFloor.m_pGamer[slotNo].m_nBombFlipCnt += 2; // bsw 01.05

			if ( m_pFloor.m_pGamer[slotNo].m_nGamerUserNo < 0 )
			{
				g_pCMainFrame->PutTextInGame( "Com1" , "��ź" , 
					0 , RGB( 255 , 255 , 165 ) , FALSE );				
			}
			else
			{
				nGameUserNo = m_pFloor.m_pGamer[slotNo].m_nGamerUserNo;
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "��ź" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
			}

		}
		else {
			SoundEffectDivi( g_nGoCnt, 0 );

			ThrowCard( nUserNo );
		}

		abEat = true;
	}

	m_pFloor.m_pGamer[slotNo].m_nUserThreeState = NOTICE_NONE;

	m_pFloor.SortUserCard(nUserNo);
	SortAndRePaint( nUserNo );


	//��źī�� �ʱ�ȭ 
	memset ( g_nBombCardNo, -1, sizeof(g_nBombCardNo) );
	g_bBomb = false;

	return nCardNo;
}

void CGoStopEngine::UpCard(const int nUserNo, const int nPosNo)
{
	int nSlotNo;
	if ( nUserNo < 0  ) {

		if ( m_bCardPackService  )
			Sleep(20);
		else
			Sleep(200);		

		g_nCardPackNo = nPosNo;
		nSlotNo = nUserNo;
	}
	else {
		nSlotNo = ( nUserNo + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;
	}

	if ( m_bCardPackService  )
		Sleep(200);
	else
		Sleep(300);		

	g_pCMainFrame->PlaySound(g_Sound[GS_CARDUP], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)


	g_nCardPackNo = -1;
}

void CGoStopEngine::ThrowCard( const int nUserNo )
{
	int nSlotNo = ( nUserNo + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;

	ResetEvent( m_hPaintEvent );

	g_pCMainFrame->OnCardThrow( nSlotNo, 0 );

    WaitForSingleObject( m_hPaintEvent, WAIT_PAINT_TIME );
}

void CGoStopEngine::UpSlowCard(const int nUserNo, const int nPosNo)
{

	Sleep(100);

	int nSlotNo;
	if ( nUserNo < 0  )
		nSlotNo = nUserNo;
	else
		nSlotNo = ( nUserNo + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;

	g_pCMainFrame->PlaySound(g_Sound[GS_CARDUP], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)

	g_pCMainFrame->OnCardUp( nSlotNo, nPosNo );

}

void CGoStopEngine::ThrowSlowCard( const int nUserNo )
{
	int nSlotNo = ( nUserNo + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;


	g_pCMainFrame->OnCardThrow( nSlotNo, 0 );

}

void CGoStopEngine::ThrowFloorCard( const int nUserNo, const int nCardNo )
{
	ResetEvent( m_hPaintEvent );

	g_pCMainFrame->OnCardThrowFloor( nUserNo, nCardNo );

    WaitForSingleObject( m_hPaintEvent, WAIT_PAINT_TIME );
}

void CGoStopEngine::EatMyCardUser(const int nUserNo, const int nCardPos)
{	

	int slotNo = ( nUserNo + m_pFloor.m_nMasterSlotNo ) % MAX_ROOM_IN;
	m_pFloor.m_pGamer[slotNo].EatCard( m_pFloor.m_pGamer[slotNo].m_bCard[nCardPos] );
	m_pFloor.m_pGamer[slotNo].m_bCard[nCardPos] = 0xff;

	g_pCMainFrame->SortUserEatCard( slotNo );


	m_pFloor.SortUserCard(nUserNo);	
	SortAndRePaint( nUserNo );

    /* ham 2006.02.27 [*YOOHOO]
	g_pCMainFrame->PlaySound(g_Sound[GS_BONUS + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)
    */


	int nCardNo = m_Hwatu.GetCard();

	AddCardUser( nUserNo, nCardNo );

	m_pFloor.SortUserCard(nUserNo);	
	SortAndRePaint( nUserNo );
}


bool CGoStopEngine::SortFloor(int &nPregiCardNo)
{
	int nSum[13];
	int nPos;
	bool bFirst;


	memset( nSum, 0, sizeof(nSum) );

	for (int i = 0 ; i < m_pFloor.m_nCardCnt ; ++i)
	{
		//DTRACE("SortFloor - nCard = %d", m_pFloor.m_strCard[i].nCardNo );
		if ( m_pFloor.m_strCard[i].nCardNo == 0xff || m_pFloor.m_strCard[i].nPosNo == 0xff)
			continue;
		
		nSum[m_pFloor.m_strCard[i].nCardNo / 4] ++; // nSum�� ���� ������ ī�� �� �հ�( 1��~12�� )
	}

	for ( int nCardNo = 0 ; nCardNo < 12 ; ++nCardNo )
	{
		if ( nSum[nCardNo] == 4 ) // ���� ������ �а� 4�� �̸�
		{
			nPregiCardNo = nCardNo * 4;
			return true;
		}
		
		if ( nSum[nCardNo] == 3 ) {

			bFirst = true;

			for ( i = 0 ; i < m_pFloor.m_nCardCnt ; ++i )
			{
				if ( m_pFloor.m_strCard[i].nCardNo == 0xff || m_pFloor.m_strCard[i].nPosNo == 0xff)
					continue;

				if ( nCardNo == m_pFloor.m_strCard[i].nCardNo / 4 ) // ���� ������ ���� ���
				{
					if (bFirst) // ù��° ���̸�
					{
						bFirst = false;
						nPos = m_pFloor.m_strCard[i].nPosNo; // nPos�� ���� ���� ��ġ
					}
					else {
						m_pFloor.m_nPosCnt[m_pFloor.m_strCard[i].nPosNo]--; // ���� Pos�� ī��� -1
						m_pFloor.m_strCard[i].nPosNo = nPos; // ���� ī���� Pos�� nPos
						m_pFloor.m_nPosCnt[m_pFloor.m_strCard[i].nPosNo]++; // ���� ī���� pos�� ��ġ�� ī��� +1

					}
				}
			}			
		}
	}

	if ( i >= 12 )			// ������� �ƴϰų� ���� ������ �а� ������...
		return false;

	return false;
}

// �̿��� ��ġ, ��¤�� ī��, �̿��ڰ� ���� ī�� ��ġ(����), ��¤�� ī�� ���� ��ġ
bool CGoStopEngine::ThrowFloor(int nSlotNo, const int nCardNo, const int nUserEatFloorPos, int nRoundNo, int &nPackEatFloorPos)
{
	bool bEat = false;
	int nFloorCardCurNo;	//�÷ξ�ī�� �����...���°��...

	nFloorCardCurNo = SelectFloorTwoPee(nSlotNo, nCardNo, nRoundNo, nUserEatFloorPos);

	if ( nFloorCardCurNo != 0xff )
	{
		nPackEatFloorPos = m_pFloor.m_strCard[nFloorCardCurNo].nPosNo;

		if  ( nPackEatFloorPos == nUserEatFloorPos ) {

			bEat = false;
		}
		else {
			SoundEffectDivi( g_nGoCnt, 0 );

			bEat = true;
		}

	}
	else
	{
		g_pCMainFrame->PlaySound(g_Sound[GS_MISS], false);

		nPackEatFloorPos = m_pFloor.GetEmptyPos();
	}

	return bEat;
}


// �̿��� ��ġ, ��¤�� ī��, �̿��ڰ� ���� ī�� ��ġ(ȭ����ġ)
int CGoStopEngine::SelectFloorTwoPee(int nSlotNo, int nCardNo, int nRoundNo, int nUserEatFloorPos)
{								  //(int nSlotNo, int nCardNo, int roundNo, int nExceptNo)

	BYTE arrPos[5];	// ��¤���� �п� �ٴڿ� �а� ������ ���⿡ �� �ٴ����� ��ġ�� ������� ����.
	int cnt = IsTwoPos( nCardNo, arrPos ); // ���� �׸��� �ٴ� ��ġ ����


	// cnt�� 1�̸� ������ �����ִ°��̰� nRoundNo == 9�̸� �����̰�.
	// m_pFloor.m_nPosCnt[arrPos[0]] > 1�̸� ����ġ�� 2���̻��� ����ִٴ� ���̰�.  
	// nUserEatFloorPos == arrPos[0] �����Ͱ� ���� ī���϶�. �������� �δ� ��� �̴�.	
	// �׷��� ���� ���� ���⶧����. ������ ���°ɷ� ó���Ѵ�.

	if ( cnt == 1 && nRoundNo == 9 &&	
		m_pFloor.m_nPosCnt[arrPos[0]] > 1 && nUserEatFloorPos == arrPos[0])	
		return 0xff;
	
	if (cnt == 1 || cnt == 3)
		return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);	// 1�� �Ǵ� 3���� ������..�ٴ��� ��ġ.
	
	if ( cnt <= 0 && cnt != 2)
		return 0xff;

	if ( nUserEatFloorPos != -1 )		//���Ծ����� -1�� �����⶧����.
	{
		if (arrPos[0] == nUserEatFloorPos)	//ù��°�� �Ծ�����..
			return m_pFloor.GetFIrstPosFromFloorPos(arrPos[1]);

		if (arrPos[1] == nUserEatFloorPos)  //�ι�°�� �Ծ�����...
			return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
	}

	int nFloorCardCurNo1 = m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
	int nFloorCardCurNo2 = m_pFloor.GetFIrstPosFromFloorPos(arrPos[1]);

	// �ΰ��� ī�尡 �����̸� ù��°ī�带 �Դ´�.
	if ( g_pGoStopValue[m_pFloor.m_strCard[nFloorCardCurNo1].nCardNo] ==
		 g_pGoStopValue[m_pFloor.m_strCard[nFloorCardCurNo2].nCardNo] )
		 return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);

	m_nSelectCard = -1;

	if ( m_pFloor.m_pGamer[nSlotNo].m_bComputer )	// Computer�̸� ù��°ī�带 �����Ѵ�.
	{
		return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
	}
	else if ( g_MyUser == g_CurUser ) {			
		//*******************************************
		//������� ������ ��ٸ���. 
		//�����ð��� �ְ� �ð����� �������Դ��� �̺�Ʈ�� ���Դ��� üũ�ؼ� ���������ش�.
		//*******************************************
		//���� �ʱ�ȭ.
		//DTRACE("CB_SELECT_TWO_CARD �׼��� �Ǵ�");
		ResetEvent(m_hActionEvent);

		g_pCMainFrame->SelectTwoCard( g_CurUser, nCardNo );


		DWORD dwRe = WaitForSingleObject(m_hActionEvent, WAIT_60TIME);
		if ( dwRe == WAIT_TIMEOUT ) {

			m_nSelectCard = m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
			g_pCMainFrame->l_bShowDialog = FALSE;

			//DTRACE("g_pCMainFrame->l_bShowDialog = FALSE - 5");
		}

	
		//ī�带 ���������� �ٸ�����ڿ��� ī�� ��ȣ�� �����ش�..
		//DTRACE("CB_SELECT_TWO_CARD ī�带 ������.");
		g_pClient->SendUserCard( m_nSelectCard );

		return m_nSelectCard;
	}
	else {	//�ٸ�����ڰ� ������ �������̸� 
		//DTRACE("ī�带 �������Դϴ�");	//���̾�α׸� ����.
		//DTRACE("(2)�ٸ���� �׼Ǳ�ٸ�");

		ResetEvent(m_hOtherUserActionEvent);

		g_pCMainFrame->WaitCardChoice();		

		m_nOtherUserCard = m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]); //���⿡�� ���ڱ� ©���� �̰� �����ϰ� �Ѵ�.
		
		m_bOtherWait = true;
		DWORD dwRe = WaitForSingleObject( m_hOtherUserActionEvent, WAIT_90TIME );	//12�� �Ŀ�..
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pClient->Destroy();
			g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 155", true );
		}
		return m_nOtherUserCard;
	}

	return m_nSelectCard;
}


int CGoStopEngine::IsTwoPos( const int nCardNo, BYTE *retPos )	
{
	int cnt = 0;
	BYTE arrPos[MAX_CARD_POS];	
	memset(arrPos, 0, MAX_CARD_POS);

	for ( int i = 0 ; i < m_pFloor.m_nCardCnt ; ++i )
	{
		if (m_pFloor.m_strCard[i].nCardNo / 4 == nCardNo / 4)
			arrPos[m_pFloor.m_strCard[i].nPosNo]++;
	}

	for (i = 0 ; i < MAX_CARD_POS ; ++i)
	{
		if ( arrPos[i] > 0 )
		{
			retPos[cnt++] = i;		// ���⿡ ��¤�� ī��� ������ ī���� �ٴ���ġ�� ������� ����.
		}
	}

	return cnt;
}

void CGoStopEngine::GetPeeEachGamer( int nSlotNo )
{

	//yuno 2005.10.30
	//�Ҹ� �� ȿ���� �ѹ��� �����Ѵ�.
	bool bGetOnePee = false;
	bool bFirstEffect = false;
	bool bFirstSound = false;

	for ( int i = 1; i < MAX_ROOM_IN; i++ ) {

		if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_nEatCnt[3] == 0  )
			 continue;

		bGetOnePee = false;

		if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt == 1 && !bFirstEffect ) {

			EventSync( nSlotNo, 18, GS_GETONEPEE );
			bFirstEffect = true;
		}
		else if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt == 2 && !bFirstEffect ) {

			EventSync( nSlotNo, 14, GS_GETTWOPEE );
			bFirstEffect = true;
		}
		else if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt == 3 && !bFirstEffect ) {

			EventSync( nSlotNo, 23, GS_GETTHREEPEE );
			bFirstEffect = true;
		}
		else if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt == 4 && !bFirstEffect ) {

			EventSync( nSlotNo, 24, GS_GETFOURPEE );
			bFirstEffect = true;
		}
		else if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt == 5 && !bFirstEffect ) {

			EventSync( nSlotNo, 27, GS_GETFIVEPEE );
			bFirstEffect = true;
		}


		while ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt > 0 ) {
			ResetEvent(m_hPaintEvent);

			g_pCMainFrame->OnGetOneCard( nSlotNo, ( nSlotNo+i ) % MAX_ROOM_IN );

			WaitForSingleObject(m_hPaintEvent, WAIT_PAINT_TIME);

			g_pCMainFrame->SortUserEatCard( ( nSlotNo+i ) % MAX_ROOM_IN );

			g_pCMainFrame->SortUserEatCard( nSlotNo );


			bGetOnePee = true;

			m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].SendPeeEvalutaion();
		}

		if ( bGetOnePee && !bFirstSound ) {
			
			bFirstSound = true;
			g_pCMainFrame->PlaySound(g_Sound[GS_AFTERJJOK + (g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50)], false);
			Sleep(500);
		}
	}
}

// ham 2006.02.22 [*PEE]
void CGoStopEngine::GetOnePee( int nSlotNo )
{
    bool bGetOnePee = false;    // ham 2006.02.27 [*YOOHOO]

	//�Ҹ� �� ȿ���� �ѹ��� �����Ѵ�.
	for ( int i = 1; i < MAX_ROOM_IN; i++ ) {

		if ( m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_nEatCnt[3] == 0  )
			 continue;


        bGetOnePee = true; // ham 2006.02.27 [*YOOHOO]

        m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt = 1;

		EventSync( nSlotNo, 18, GS_GETONEPEE );

		ResetEvent(m_hPaintEvent);

		g_pCMainFrame->OnGetOneCard( nSlotNo, ( nSlotNo+i ) % MAX_ROOM_IN );

		WaitForSingleObject(m_hPaintEvent, WAIT_PAINT_TIME);

		g_pCMainFrame->SortUserEatCard( ( nSlotNo+i ) % MAX_ROOM_IN );

		g_pCMainFrame->SortUserEatCard( nSlotNo );


		m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].SendPeeEvalutaion();

		g_pCMainFrame->PlaySound(g_Sound[GS_AFTERJJOK + (g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50)], false);
		Sleep(500);

        m_pFloor.m_pGamer[( nSlotNo+i ) % MAX_ROOM_IN].m_GetOtherUserCardCnt = 0;
	}

    // ham 2006.02.27 [*YOOHOO]
    // "���徿" ���尡 ������ ���� ��쿡�� "����~" ���� ���
    if ( !bGetOnePee ) {
        g_pCMainFrame->PlaySound(g_Sound[GS_BONUS + (g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50)], false); // bsw (�� �Լ����� true�� �ݺ� ��� ����)
    }
    ////
}
////

bool CGoStopEngine::SelectNineCardPee(const int nUserNo)
{
	int slotNo = (nUserNo + m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;

	m_nNineCard = -1;

	//���� �ʱ�ȭ.
	ResetEvent(m_hActionEvent);
	
	if ( m_pFloor.m_pGamer[slotNo].m_bComputer )	// Computer
	{
		Sleep(200);		// ������ ���Ƿ� ����
		return true;
	}
	else if ( g_MyUser == g_CurUser ) { 

		g_pCMainFrame->SelectNine();

		//����� ���� ��ٸ�. // ���̾�α׿��� �ϹǷ� �����ð��� �ش�.
		DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );
		if ( dwRe == WAIT_TIMEOUT ) {
			m_nNineCard = 1;
		}


		//���Ǹ� ��� �ߴ����� ������..
		//DTRACE("������� ���������ϰ� ����.. : %d", m_nNineCard );
		g_pClient->SendNineCard( m_nNineCard );


		return (m_nNineCard) ? true : false;

	}
	else {
		
		//���� ������...
		//DTRACE( "���Ǹ� �������Դϴ�. �ٸ�������� ��������ٸ�...");


		ResetEvent(m_hOtherUserActionEvent);		
		g_pCMainFrame->WaitCardChoice();

		m_nOtherUserCard = 1; //���⿡�� ���ڱ� ©���� �̰� �����ϰ� �Ѵ�.

		m_bOtherWait = true;		
		DWORD dwRe = WaitForSingleObject( m_hOtherUserActionEvent, WAIT_90TIME );	//12�� �Ŀ�..
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pClient->Destroy();			
			g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 157", true );
		}

		return (m_nOtherUserCard) ? true : false;
	}

	return true;

}

void CGoStopEngine::SetActionEvent()//int nValue)
{
	SetEvent(m_hActionEvent);
}

void CGoStopEngine::SetOtherActionEvent()//int nValue)
{
	SetEvent(m_hOtherUserActionEvent);
}

int CGoStopEngine::WaitGoStop(const int nUserNo)
{
	int slotNo = (nUserNo + m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;
	int nRe = 0;


	if ( m_pFloor.m_pGamer[slotNo].m_bComputer )	// Computer
	{
		Sleep(300);
		m_nSelectCard = 1;
		for (int i = 0 ; i < MAX_ROOM_IN ; ++i)
		{
			if (i == slotNo)
				continue;
			if (m_pFloor.m_pGamer[i].m_nEvalution > 0)
			{
				m_nSelectCard = 0;
				break;
			}
		}
		nRe = m_nSelectCard;
	}
	else if ( g_MyUser == g_CurUser ) { 
		
		ResetEvent(m_hActionEvent);

		g_pCMainFrame->SelectGoStop();


		//���̾�α׿��� üũ�ϹǷ� ���� �ð��� �ش�.
		DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );
		if ( dwRe == WAIT_TIMEOUT ) {
			m_nGoStop = 0;
		}


		g_pClient->SendGoStop( m_nGoStop );

		nRe = m_nGoStop;
	}
	else  {	 //�ٸ������ ���� ��ٸ������̶�� �ٷ� ���⼭ �ѷ�������.
		
		ResetEvent(m_hOtherUserActionEvent);

		g_pCMainFrame->WaitGoInfo();


		m_nOtherUserCard = 0; //���⿡�� ���ڱ� ©���� �̰� �����ϰ� �Ѵ�.
		m_bOtherWait = true;
		DWORD dwRe = WaitForSingleObject(m_hOtherUserActionEvent, WAIT_90TIME);	//���濡�Լ� �޼����� �ö����� ��ٸ���.
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pClient->Destroy();
			g_pCMainFrame->l_Message->PutMessage( "��Ʈ��ũ�� �Ҿ����մϴ�.", "Code - 159", true );
		}


		nRe = m_nOtherUserCard;
	}
    

	return nRe;
}

void CGoStopEngine::EndingCheck()
{
	int nGamerUserNo = -1;

	bool bIsImage = false;
	int nDokBakSlot = -1;
	int i = 0;
	int winVal = m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEvalution;
	int winMul = m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nMulti;
	BigInt biwinMoney = 0;
	int nMissionMultiply = 1;

	nMissionMultiply = m_nMissionMultiply;

	m_nBaseMulti = m_pFloor.m_nBaseMulti;

	if ( m_nMissionType == DAEBAK ) {
		m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_bMission = true;
	}
	else if ( !m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_bMission )
		nMissionMultiply = 1;


	for ( i = 1 ; i < MAX_ROOM_IN ; ++i)
	{
		int mul = winMul;

		if (m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEatCnt[4] >= 10)	// �Ƿ� ���� ��
		{
			if ( m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nEatCnt[4] > 0 &&
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_bNineCard ) {

				ResetEvent(m_hPaintEvent);
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].MoveNineCard(false);

				g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMOVE], false);

				g_pCMainFrame->OnNineCardMove( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN, 0);


				WaitForSingleObject(m_hPaintEvent, WAIT_PAINT_TIME);

				g_pCMainFrame->SortUserEatCard( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN );

				
			}
				
			if ( m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nEatCnt[4] > 0 && 
				 m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nEatCnt[4] < 8 )	// �ǹ�
			{
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_bPbak = TRUE;

				mul *= 2;
			}
		}

		if (m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEatCnt[0] >= 3)	// ������ ���� ��
		{
			if (m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nEatCnt[0] < 1)	// ����
			{
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_bKwangbak = TRUE;

				mul *= 2;
			}
		}

		m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nLoseEval = winVal * mul;

		biwinMoney += m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nLoseEval;
	}
	
	BigInt biwinmoney2 = 0;

	for (i = 1 ; i < MAX_ROOM_IN ; ++i)
	{
		BigInt bival;
		
		if ( nDokBakSlot < 0 )	// ����
		{
			BigInt bis = ( m_pFloor.m_nWinerNo+i ) % MAX_ROOM_IN;		//�ٸ������� ������.
			int chonCheck = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nLoseEval;

		//	bival = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nLoseEval * m_biMoney * m_pFloor.m_nBaseMulti * nMissionMultiply;
			bival = chonCheck * m_biMoney * m_pFloor.m_nBaseMulti * nMissionMultiply;
			
			TRACE("bis:%d   chonCheck:%d  m_biMoney:%d  m_pFloor.m_nBaseMulti:%d nMissionMultiply:%d bival:%d \n",bis,chonCheck,m_biMoney,m_pFloor.m_nBaseMulti,nMissionMultiply,bival);

			//������������.
			if ( m_pFloor.RealGame() ) {
				
				nGamerUserNo = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nGamerUserNo;
				//������ ���� ������ �� ���� ������ ������ �׸�ŭ�� ����.
				
				if ( bival >= g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biUserMoney ) 
					bival = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biUserMoney;

				int chaGam = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biCurWinLoseMoney;

				TRACE(" ������:%d    Loser ���� �����ݾ�:%d\n",chaGam,bival);

				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biCurWinLoseMoney -= bival;
				m_pFloor.UserMoneyVal( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN, bival, false );
				g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nLose_Num++;	

			}

			biwinmoney2 += bival;
			m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biLoseMoney = bival;
//			TRACE("m_nWinerNo %d %d\n",m_nWinerNo, bival);
		}
		else	// ������ �ִ°��
		{
			
			if ( nDokBakSlot == (m_pFloor.m_nWinerNo+i) % MAX_ROOM_IN ) {

				bival = biwinMoney * m_biMoney * m_pFloor.m_nBaseMulti * nMissionMultiply;
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biLoseMoney = bival;
			

				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_bDokbak = TRUE;

				EventSync( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN, 19, GS_DOKBAK, 100 );

				//������������.
				if ( m_pFloor.RealGame() ) {


					nGamerUserNo = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nGamerUserNo;

					//������ ���� ������ �� ���� ������ ������ �׸�ŭ�� ����.
					if ( bival >= g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biUserMoney ) 
						bival = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biUserMoney;

					m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biCurWinLoseMoney -= bival;
					
					m_pFloor.UserMoneyVal( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN, bival, false );

					g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nLose_Num++;	
					
				}

				biwinmoney2 = bival;
				break;
			}
		}
	}
	
	if ( biwinmoney2 > 0)
	{
		//������������.
		biwinmoney2 = GameSystemMoney(biwinmoney2);
		
	
		//������������.
		if ( m_pFloor.RealGame() ) {
			m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_biCurWinLoseMoney += biwinmoney2;
			m_pFloor.UserMoneyVal( m_pFloor.m_nWinerNo, biwinmoney2, true );
			nGamerUserNo = m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nGamerUserNo;
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nWin_Num++;

		}

		m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_biWinMoney = biwinmoney2;
	}


	if ( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.biUserMoney < GAMEROOM_MIN1 ) {
		m_pFloor.m_pGamer[0].m_ExitReserv = true;
		g_pClient->UserExitReserv( g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.nUserNo, m_pFloor.m_pGamer[0].m_ExitReserv );
	}

	g_pCMainFrame->PutResultText();
}



void CGoStopEngine::Stop()
{	
	if ( m_pFloor.RealGame() ) {
		UserStopInfoUpdate();
	}

	g_pCMainFrame->l_bShowMission = false;
    g_pCMainFrame->m_nHwatuSelectCnt = 0;
    g_pCMainFrame->m_nHwatuSelectFrm = 0;
    //[alpha]
    memset( &(g_pCMainFrame->m_sPrevDlgInfo), 0x00, sizeof( g_pCMainFrame->m_sPrevDlgInfo ));

	g_CurUser = -1;
	m_bPlaying = false;
	m_nSelectCard = -1;
	m_nNineCard = -1;
	m_nShake = -1;
	m_nOtherUserCard = -1;
	m_bGameStop = false;
	m_nMissionMultiply = 1;
	m_nLastGo = -1;

	m_nRound = 0;
	m_nUserno = 0;
	m_nGoStop = 0;	

	//�����͸� �����Ѵ�.
	//���� �ѹ��� ����.
	if ( m_pFloor.RealGame() ) {
		if ( m_pFloor.m_nWinerNo >= 0 ) {
			m_pFloor.m_nMasterSlotNo = m_pFloor.m_nWinerNo;
		}

		if ( m_pFloor.m_nMasterSlotNo == MASTER_POS )
			m_bMasterMode = true;
		else
			m_bMasterMode = false;

		m_pFloor.UserRefresh( m_pFloor.m_nMasterSlotNo );
	}
	g_MyUser = m_pFloor.m_pGamer[0].m_nUserNo;

	m_pFloor.Init();
	
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_pFloor.m_pGamer[i].Init();
	}
}

void CGoStopEngine::RoomClear()
{
	m_nDouble = 0;
	m_nTriple = 0;
	m_biMoney = 0;
	m_bPlaying = false;

	m_bMasterMode = true;
	m_nSelectCard = -1;
	m_nNineCard = -1;
	m_nShake = -1;
	m_nOtherUserCard = -1;
	m_nMissionMultiply = 1;

	m_nRound = 0;
	m_nUserno = 0;
	m_nGoStop = 0;

	m_pRecvCardPack = NULL;
	m_bMyKetInput = false;
	m_bOtherWait = false;

	g_CurUser = -1;
	g_MyUser = -1;
	g_nViewPageNo = 0;

	//floor �ʱ�ȭ.
	m_pFloor.m_nBaseMulti = 1;
	m_pFloor.m_nMasterSlotNo = MASTER_POS;
	m_pFloor.m_nBbangJjangSlotNo = MASTER_POS;

	m_pFloor.m_RealMode = false;

	for ( int i = 1; i < MAX_ROOM_IN; i++ ) {
		m_pFloor.m_pGamer[i].RoomInInit();
		m_pFloor.m_pGamer[i].Init();
		m_pFloor.m_pGamer[i].m_ExitReserv = false;
		m_pFloor.m_pGamer[i].m_biCurWinLoseMoney = 0;
		m_pFloor.m_pGamer[i].m_bComputer = true;
		m_pFloor.m_pGamer[i].m_nGamerUserNo = -1;
		m_pFloor.m_pGamer[i].m_nUserNo = -1;
		memset( m_pFloor.m_pGamer[i].m_szUserImage, 0x00, sizeof(m_pFloor.m_pGamer[i].m_szUserImage) );
		memset( m_pFloor.m_pGamer[i].m_szPreAvatar, 0x00, sizeof(m_pFloor.m_pGamer[i].m_szPreAvatar) );
	}

	m_pFloor.m_pGamer[0].m_nUserNo = 0;
	m_pFloor.m_pGamer[0].m_ExitReserv = false;
	m_pFloor.m_pGamer[0].m_biCurWinLoseMoney = 0;
	m_pFloor.Init();
}

void CGoStopEngine::OnUserExitReserv( int nUniQNo, bool bExit )
{
	char szTempStr[255];
	char szTempStr2[255];
	int nGameUserCnt = -1;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( g_CGameUserList.m_GameUserInfo[i].m_sUserInfo.nUserNo == nUniQNo) {
			nGameUserCnt = i;
		}
	}
	//
	int nSlotNo =  m_pFloor.FindUser( nGameUserCnt  );

	memset( szTempStr, 0x00, 255 );
	memset( szTempStr2, 0x00, 255 );

	if ( bExit ) {
        g_pCMainFrame->m_bShowExitReservIcon[nSlotNo] = TRUE;
		sprintf( szTempStr, "%s��", g_CGameUserList.m_GameUserInfo[nGameUserCnt].m_sUserInfo.szNick_Name );
		strcpy( szTempStr2, "�����⿹��" );
	}
	else {
        g_pCMainFrame->m_bShowExitReservIcon[nSlotNo] = FALSE;
		sprintf( szTempStr, "%s��", g_CGameUserList.m_GameUserInfo[nGameUserCnt].m_sUserInfo.szNick_Name );
		strcpy( szTempStr2, "���������" );
	}
	
	m_pFloor.m_pGamer[nSlotNo].m_ExitReserv = bExit;
    g_pCMainFrame->AddGameChatText( szTempStr, RGB(255, 255, 0));
    g_pCMainFrame->AddGameChatText( szTempStr2, RGB(255, 255, 100));
}

void CGoStopEngine::UserStopInfoUpdate()
{
	//����� ��� ���� ��� ����
	int nGamerUserNo  = -1;
	float fTemp = 0;
	int nIntTemp = 0;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		nGamerUserNo = m_pFloor.m_pGamer[i].m_nGamerUserNo;

		if ( m_pFloor.m_pGamer[i].m_biWinMoney > g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biMaxMoney ) {
			 g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biMaxMoney = m_pFloor.m_pGamer[i].m_biWinMoney ;
		}

		//������( �̱�� + ���� + ���� )
		int nTotNum = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nWin_Num 
			+ g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nLose_Num 
			+ g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nDraw_Num;

		if ( nTotNum > 0 ) {
			g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.fWinRate = 
				( (float)g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nWin_Num / (float)nTotNum ) * 100;
		}

		nIntTemp = (int)(g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.fWinRate * 100);
		fTemp = ((float)nIntTemp/100);

		g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.fWinRate = fTemp;
	}
}

void CGoStopEngine::DivImage( int nUserSlotNo , char *p_szImage , BOOL bPrevImage )
{	
}

void CGoStopEngine::InitImageData( int nUserSlotNo )
{
	g_sImageData[nUserSlotNo].nFile = 0; //�̹��� ���ϸ�
	memset( g_sImageData[nUserSlotNo].szCurImageFileName , 0x00 , sizeof( g_sImageData[nUserSlotNo].szCurImageFileName ));
	g_sImageData[nUserSlotNo].nLastDec = 0;	    //�̹��� �����ܰ�
	g_sImageData[nUserSlotNo].nCurDec  = 0;		//�̹��� ����ܰ�
	g_sImageData[nUserSlotNo].nUserImageNo = 0;   //���� �̹��� ��ȣ( ���� �ڽ��� �ƹ�Ÿ ���° )  �����϶��� 0
	memset( g_sImageData[nUserSlotNo].szUserImage , 0x00 , sizeof( g_sImageData[nUserSlotNo].szUserImage )); // ������ ������ �ִ� �׸�( 7 ����Ʈ )
	
	g_sImageData[nUserSlotNo].bFreeGuest = TRUE; // ���� ȸ���̸� true ����ȸ���̸� FALSE	

	g_sImageData[nUserSlotNo].l_bPrint = FALSE;
}

int CGoStopEngine::CheckReGetSprite( int nUserSlotNo ) // 
{
    g_pCMainFrame->ReGetSprite( &(g_sImageData[nUserSlotNo].l_SPR_Avatar), g_sImageData[nUserSlotNo].szCurImageFileName, 125, 190, 1 );
	return 0;
}

void CGoStopEngine::WaitRoomModeSet()
{
    g_pCMainFrame->l_MinBTN->SetPos( 962, 5 );
    g_pCMainFrame->l_MaxBTN->SetPos( 981, 5 );
	g_pCMainFrame->l_MaxBTN2->SetPos( 981, 5 );
    g_pCMainFrame->l_ExitBTN->SetPos( 1000, 5 );

    g_pCMainFrame->l_Exit2BTN->SetPos( 29, 555 );
    g_pCMainFrame->l_ConfigBTN->SetPos( 881, 4 );
    g_pCMainFrame->l_CaptureBTN->SetPos( 804, 4 );

    g_pCMainFrame->l_MinBTN->ReGetButtonImage(".\\image\\lobby\\min_btn.spr",   16, 16 );
    g_pCMainFrame->l_MaxBTN->ReGetButtonImage(".\\image\\lobby\\max_btn.spr",   16, 16 );
	g_pCMainFrame->l_MaxBTN2->ReGetButtonImage(".\\image\\lobby\\max_btn.spr",  16, 16 );
    g_pCMainFrame->l_ExitBTN->ReGetButtonImage(".\\image\\lobby\\exit_btn.spr", 16, 16 );

    g_pCMainFrame->l_Exit2BTN->ReGetButtonImage(".\\image\\lobby\\exit2_btn.spr",     107, 30 );
    g_pCMainFrame->l_ConfigBTN->ReGetButtonImage(".\\image\\lobby\\config_btn.spr",    71, 19 );
    g_pCMainFrame->l_CaptureBTN->ReGetButtonImage(".\\image\\lobby\\btn_capture.spr",  71, 19 );

    memset( g_pCMainFrame->m_bShowExitReservIcon, 0x00, sizeof( g_pCMainFrame->m_bShowExitReservIcon ));
    memset( g_ClickRoomUserInfo, 0x00, sizeof( g_ClickRoomUserInfo ));

    g_pCMainFrame->l_bShowMakeRoomDlg = FALSE;
    g_pCMainFrame->l_nViewRoomMessage = -1;
    g_pCMainFrame->m_nSelectedRoomTitleList = -1;
    g_pCMainFrame->m_nHighlightBarNum = -1;
    g_pCMainFrame->m_nUserListTitleNum = 0;
    g_pCMainFrame->m_pWaitChat->Init();                // ���� ä��â �ؽ�Ʈ �ʱ�ȭ
    g_pCMainFrame->m_SCB_WaitChat->SetElem( 0 );
    g_pCMainFrame->m_SCB_WaitChat->StartPage();
    g_pCMainFrame->m_SCB_WaitRoom->StartPage();
    g_pCMainFrame->m_SCB_WaitUser->StartPage();
    g_pCMainFrame->m_IME_WaitChat->InputEnd();     // ���� ä��â IME�� �ʱ�ȭ �Ѵ�.
    g_pCMainFrame->m_IME_WaitChat->ClearBuffer();
    g_pCMainFrame->m_IME_WaitChat->InputBegin(true);
	

	//���ӹ�볻�� �ƹ�Ÿ�� �����.
	PostMessage( g_hWnd, WM_AVATARDEL, 0, 0  );
	PostMessage( g_hWnd, WM_AVATARDEL, 1, 0  );


	memset( &g_sAvatarPlayStr, 0x00, sizeof(g_sAvatarPlayStr) );

	g_sAvatarPlayStr.nSlot = -1;
	strcpy( g_sAvatarPlayStr.szAvaUrl, g_szAvaUrl );
	strcpy( g_sAvatarPlayStr.szGameGubun, g_szGameGubun );
	g_sAvatarPlayStr.cSex = g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.cSex;

	PostMessage( g_hWnd, WM_AVATARPLAY, 0, 0  );

    g_pCMainFrame->m_bShowRoomTitleList = FALSE;
	g_pCMainFrame->l_nCurrentMode = WAITROOM_MODE;
	g_pCMainFrame->m_bSunSet = FALSE;
	g_pCMainFrame->m_nSunSetRender = 0;
	g_pCMainFrame->m_bSunSetRender = FALSE;
	g_pCMainFrame->m_nSunRenderDecided = 0;
	g_pCMainFrame->m_bRenderSun = FALSE;
    //g_pCMainFrame->ShowBanner();
}



void CGoStopEngine::GameModeSet()
{
	g_pCMainFrame->m_FrameSkip.Clear();

    memset( g_pCMainFrame->m_bShowExitReservIcon, 0x00, sizeof( g_pCMainFrame->m_bShowExitReservIcon ));

    g_pCMainFrame->m_nHwatuSelectCnt = 0;
    g_pCMainFrame->m_nHwatuSelectFrm = 0;
    g_pCMainFrame->m_nCardBlinkCnt = 0;     // �� ���� ��, ȭ�� ������ �����̰� �ϴµ� ���Ǵ� ī����
    g_pCMainFrame->m_nCardBlinkFrm = 0;     // �� ���� ��, ȭ�� ������ �����̰� �ϴµ� ���Ǵ� ������ ����

	g_pCMainFrame->l_nMBoxXPos = MEMO_DLG_POS_X;
	g_pCMainFrame->l_nMBoxYPos = MEMO_DLG_POS_Y;

	g_pCMainFrame->l_SPR_MessageBox->SetPos( MEMO_DLG_POS_X, MEMO_DLG_POS_Y );

    g_pCMainFrame->l_MinBTN->SetPos( 941, 6 );
    g_pCMainFrame->l_MaxBTN->SetPos( 967, 6 );
	g_pCMainFrame->l_MaxBTN2->SetPos( 967, 6 );
    g_pCMainFrame->l_ExitBTN->SetPos( 993, 6 );

	g_pCMainFrame->l_Exit2BTN->SetPos( 967, 568 );
    g_pCMainFrame->l_CaptureBTN->SetPos( 849, 568 );
    g_pCMainFrame->l_ConfigBTN->SetPos( 852, 6 );

    g_pCMainFrame->l_MinBTN->ReGetButtonImage(".\\image\\game\\btn_bar_minimize.spr",  22, 21 );
    g_pCMainFrame->l_MaxBTN->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr",  22, 21 );   // ham 2005.09.22 �̹��� ũ�� �缳��
	g_pCMainFrame->l_MaxBTN2->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr", 22, 21 );
    g_pCMainFrame->l_ExitBTN->ReGetButtonImage(".\\image\\game\\btn_bar_close.spr",    22, 21 );

    g_pCMainFrame->l_Exit2BTN->ReGetButtonImage(".\\image\\game\\btn_board_close.spr",      50, 30 );
    g_pCMainFrame->l_CaptureBTN->ReGetButtonImage(".\\image\\game\\btn_capture_game.spr",   50, 30 );
    g_pCMainFrame->l_ConfigBTN->ReGetButtonImage(".\\image\\game\\btn_board_option.spr",    49, 21 );

	g_pCMainFrame->l_nStartBtnCnt = 0;
    g_pCMainFrame->m_pGameChat->Init();
    g_pCMainFrame->m_IME_GameChat->InputEnd();     // ����ȭ�� ä��â IME �ʱ�ȭ
    g_pCMainFrame->m_IME_GameChat->ClearBuffer();
    g_pCMainFrame->m_SCB_GameChat->SetElem( 0 );
    g_pCMainFrame->m_SCB_GameChat->StartPage();
    //[alpha]
    memset( &(g_pCMainFrame->m_sPrevDlgInfo), 0x00, sizeof( g_pCMainFrame->m_sPrevDlgInfo ));

	//���ӹ�볻�� �ƹ�Ÿ�� �����.
	PostMessage( g_hWnd, WM_AVATARDEL, -1, 0  );

	memset( &g_sAvatarPlayStr, 0x00, sizeof(g_sAvatarPlayStr) );

	g_sAvatarPlayStr.nSlot = 0;
	strcpy( g_sAvatarPlayStr.szAvaUrl, g_szAvaUrl );
	strcpy( g_sAvatarPlayStr.szGameGubun, g_szGameGubun );
	g_sAvatarPlayStr.cSex = g_CGameUserList.m_GameUserInfo[0].m_sUserInfo.cSex;

	PostMessage( g_hWnd, WM_AVATARPLAY, 0, 0  );

	g_pCMainFrame->l_nCurrentMode = GAME_MODE;
    g_pCMainFrame->SetFocus();
    //g_pCMainFrame->CloseBanner();
}


void CGoStopEngine::ChannelModeSet()
{
	g_pCMainFrame->l_nCurMashine = 0;
	g_pCMainFrame->l_nCurChannelNo = 0;
	g_pCMainFrame->l_nCurrentMode = CHANNEL_MODE;
}



BigInt CGoStopEngine::GameSystemMoney( BigInt biUserMoney )
{
//	BigInt biTemp = (float)( biUserMoney / 100) * SYSTEMMONEY_RATE ;
	BigInt biTemp = (float)( biUserMoney / 100) * g_nSystemMoneyRate ;

	g_biSystemMoney = biTemp;

	return biUserMoney - biTemp;
}