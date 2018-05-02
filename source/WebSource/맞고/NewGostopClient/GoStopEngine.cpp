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
extern sIMAGEDATA g_sImageData[MAX_ROOM_IN];//각 유저가 갖고있는 이미지
extern int g_CurUser;
extern int g_MyUser;
extern int g_nCardNoArr[5];
extern CNetworkSock *g_pClient;
extern bool g_bDeal;
extern char g_szUserImage[MAX_ROOM_IN][MAX_IMAGE_LEN]; // 유저가 가지고 있는 그림 ( 세명 )( 8 바이트 )
extern int g_nViewPageNo; // 현재 보고 있는 페이지 넘버
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
		if ( !bRe ) {	// 게임 이용자들의 카드 초기화

			if ( g_lpGE->m_bGameStop == TRUE ) {  //시작하자마자 동시에 유저가 나가면
				g_lpGE->m_bGameStop = FALSE;
				return 0;
			}
			else {	
				g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 144", true );
				g_pClient->Destroy();
				return 0;
			}
		}


		if ( g_lpGE->m_pFloor.m_nWinerNo < 0 && !g_lpGE->StartDeal()  )		//게이머들에게 패를 돌린다.
		{
			TRACE("\n .........-3");
			throw -2;
		}

		if ( g_lpGE->m_pFloor.m_nWinerNo < 0 && !g_lpGE->Round() ) 			// 게임 진행
		{
			 TRACE("\n .........-3");
			throw -3;		
		}

		if ( g_lpGE->m_pFloor.m_nWinerNo >= 0 )		// winer  존재
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
		if ( !g_lpGE->m_pFloor.RealGame() && g_lpGE->m_pFloor.m_pGamer[0].m_ExitReserv ) {	//컴터랑 칠때...갑자기 나가게 하기 위해서
			g_lpGE->Stop();
			//DTRACE("catch (...)  - g_pClient->RoomOut()" );
			g_pClient->RoomOut();
			return 0;
		}

		int nGamerUserNo = -1;
		//나가리시 비김을...계산.
		if (  g_lpGE->m_bGameStop == false ) { //g_lpGE->m_bGameStop true이면 컴이랑 칠때 유저가 들어온거기 때문에..여긴 필요없다.
			if ( g_lpGE->m_pFloor.RealGame() ) { 
				for( int i = 0; i < MAX_ROOM_IN; i++ )
				{
					nGamerUserNo = g_lpGE->m_pFloor.m_pGamer[i].m_nGamerUserNo;
					g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nDraw_Num++;
				}
			}

			g_lpGE->m_pFloor.m_nBaseMulti = 2;
			//DTRACE("다음판은 두배로 진행합니다.");

			g_pCMainFrame->PlaySound(g_Sound[GS_NAGARI + (g_CGameUserList.m_GameUserInfo[MASTER_POS].m_nVoice * 50)], false);
			//DTRACE("나가리");

			memset( g_pCMainFrame->l_nPVal, 0x00, sizeof(g_pCMainFrame->l_nPVal) );

			

		}
		g_pCMainFrame->GameBreak();
	}
	

	ResetEvent(g_lpGE->m_hActionEvent);
	g_lpGE->m_bGameEndWill = true;
	DWORD dwRe = WaitForSingleObject( g_lpGE->m_hActionEvent, WAIT_90TIME );	


	if ( dwRe == WAIT_TIMEOUT ) {
		g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 145", true );
		g_pClient->Destroy();
	}

	//DTRACE("Thread 끝.");

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

    

    // 각 유저가 선택한 종류의 음성으로 플레이해준다.
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
	//여기세어 서버의 메세지를 기다린다.
	if ( m_pFloor.RealGame()  ) {
		//DTRACE("다른놈들의 준비를 기다린다.");
		

		//서버로 준비 패킷을 보낸다..
		if ( m_bMasterMode != TRUE ) {
			//DTRACE("서버로 준비 패킷을 보낸다..");
			g_pClient->GameReady();
		}

		
		//혹시 이쯤에 사람이 나갔을수도 있다.
		if ( m_pFloor.RoomUserCntOK() == false ) {
			m_bGameStop = TRUE;
			return false;
		}


		//DTRACE("InitCardPack - 다른사람 액션기다림");
		ResetEvent(m_hOtherUserActionEvent);
		m_bOtherWait = true;
		DWORD dwRe = WaitForSingleObject(m_hOtherUserActionEvent, WAIT_STARTTIME);	//상대방에게서 메세지가 올때까지 기다린다.
		if ( dwRe == WAIT_TIMEOUT || m_bGameStop == TRUE ) {
			return false;
		}
	}
	
	g_pCMainFrame->PlaySound(g_Sound[GS_CARDPACKSTART], false); // bsw (이 함수에서 true는 반복 재생 여부)

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
	bool bMade;			//대통령 판다.
	int nRecvCardNo;

	Sleep(300);
	
	for ( int cnt = 0 ; cnt < 2 && m_bPlaying ; cnt++ )				//패를 두번에 걸쳐서 나줘서 준다...3장깔고 4장씩주고 다음에 3장깔고 3장을 준다..그래서 여기가 총 두번이다.
	{
		g_nGameSpeed = 8;
		
		g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOSHARE], false); // bsw (이 함수에서 true는 반복 재생 여부)

		for (i = 0 ; i < 4 && m_bPlaying ; i++ )						//바닥에 패를 깐다...4장씩 깐다.
		{
			if ( ( nCardNo = m_Hwatu.GetCard() )  < 0 )
				return false;

			AddSlowCardFloor(nCardNo);
		}

		Sleep(300);

		for (int u = 0 ; u < MAX_ROOM_IN && m_bPlaying ; u++)					// 2명에게 패를 나눠주기 떄문에 2번돈다.
		{
			g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOSHARE], false); // bsw (이 함수에서 true는 반복 재생 여부)

			for (i = 0 ; i < 5 ; i++)					// 한명당 5장을 주고
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

	SortAndRePaint(4);	//모두 소트

	while((nCardPos = m_pFloor.CheckNephew()) >= 0 && nCardPos != 0xff && m_bPlaying)			//처음에 패를 나눠주고 바닦에 서비스패가 있으면.
	{
		EatCardUser(0, nCardPos, nRecvCardNo);

		if ((nCardNo = m_Hwatu.GetCard()) == 0xff || nCardNo < 0)								//먹은 카드 이상유무 체크
			return false;
		g_pCMainFrame->SortUserEatCard( (m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN );

		AddCardFloor( nCardNo );
	}

	int nPregidentCardNo = -1;
	bMade = SortFloor(nPregidentCardNo);													//바닥에 깔린 패들을 같은 것은 묶어서 그린다.  이값이 true이면 대통령이다.

	if (bMade)					// 바닥에 대통령이면.
	{
		m_pFloor.m_nWinerNo = m_pFloor.m_nMasterSlotNo;
		m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEvalution = PREGIDENT_POINT;
		
		m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nResultScore = PREGIDENT_POINT;
		m_pFloor.m_nPregidentNo = 0;		//선이 대통령이 때문에..

		g_pCMainFrame->PlaySound(g_Sound[GS_CHONGTONG + (g_CGameUserList.m_GameUserInfo[m_pFloor.m_nWinerNo].m_nVoice * 50)], false);

		//여기선 다이얼로그가 3초후에 죽고 이걸푼다.
		//하지만 결과창에서 다시 걸어버린다. 여긴 타이머는 필요없다.
		//나중에 혹시 모르니깐 10초정도로 해도된다. 결과창에서 또 걸기 때문에..
		ResetEvent(m_hActionEvent);
		//DTRACE("(3) - CB_PREGIDENT 를 보내고 바닥에 대통령..");
		//m_nSelect X

		//대통령 메세지를 뿌려준다.
		g_pCMainFrame->PregiDent( nPregidentCardNo );


		DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12초 후에..
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 146", true );
			g_pClient->Destroy();
		}
		

		return m_bPlaying;

	}

	// 선부터 대통령인 사람이 승!!

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



			//여기선 다이얼로그가 3초후에 죽고 이걸푼다.
			//하지만 결과창에서 다시 걸어버린다. 여긴 타이머는 필요없다.
			//나중에 혹시 모르니깐 10초정도로 해도된다. 결과창에서 또 걸기 때문에..
			ResetEvent(m_hActionEvent);

			g_pCMainFrame->PregiDent( nCardNo );

			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12초 후에..
			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 147", true );
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


	m_nMissionType = MISSION( GetRandom( 1, 18 ) ); //그래서 1부터 이다.

	switch(m_nMissionType)
	{
		case KKWANG: // 꽝일경우 1배
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
		case DEC: // 1월 부터 12월 모으기는 3배
			m_nMissionMultiply = 3;
			break;

		case HONGDAN:
		case CHONGDAN:
		case CHODAN:
		case GODORI: // 홍단, 청단, 초단, 고도리 모으기는 4배
			m_nMissionMultiply = 4;
			break;

		case OKWANG: // 5광 모으기는 5배
			m_nMissionMultiply = 5;
			break;

		case DAEBAK: // 대박은 5 ~ 10배
			m_nMissionMultiply = GetRandom( 5, 10 );
			break;
	}
}



void CGoStopEngine::SoundEffectDivi( int nGoCnt, int nSoundDivi )
{
	// 0 : 화투 패치는 소리(맞았을때)
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
			InitMission(); // 미션 초기화

            //선물랜덤.
			if ( m_nMissionType == KKWANG )
				m_nPrizeNo = PIRZECNT - 1;		//다음기회에.
			else
				m_nPrizeNo = GetRandom( 0, PIRZECNT - 2 );	//다음기회에는 랜덤에서 제외되기때문에.

			g_pClient->MissionSet( m_nMissionType, m_nMissionMultiply, m_nPrizeNo );
			
			// 두번째 턴에서 미션 보여주기
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

			nOtherFloorPos = -1;	//뒤집은 패를 유저가 먹은 화투가 없을떄 놓을 위치.									
			do	// 한장을 깠는데..서비스패가 나올경우.
			{
				if (( nCardNo = m_Hwatu.GetCard() ) == 0xff || nCardNo < 0)
					return false;

				if ( nCardNo >= 48 && nCardNo != BOMB_CARDNO  )		//폭탄카드는 제외.
				{
					UpCard( -1, nCardNo );		// 카드를 뒤짚고 ( -1이면 카드팩에서 뒤집는거다. )
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
						if ( nOtherFloorPos < 0)	//처음에 서비스패를 놓을 위치.
							nOtherFloorPos = MAX_CARD_POS - 1;
						
						m_pFloor.m_pGamer[nSlotNo].m_nChoiceFloorCardPos = nOtherFloorPos;
						g_pCMainFrame->PlaySound(g_Sound[GS_MISS], false);
						ThrowFloorCard( m_nUserno, nCardNo );
					}
				}
			} while( nCardNo >= 48 && nCardNo != BOMB_CARDNO );


	
			// 뒤집은 패를 놓을 위치를 찾자. nCenterCardNo : 팩에서의 카드.
			nCenterCardNo = nCardNo;
			
			UpCard( -1, nCenterCardNo );   	// 카드를 뒤짚고 ( -1이면 카드팩에서 뒤집는거다. )

			m_bCardPackService = false;

			bCenterEat = ThrowFloor( nSlotNo, nCenterCardNo,					
										(bUserEat) ? nUserFloorPos : -1, m_nRound, nPackEatFloorPos );//nCenterFloorPos );

			m_pFloor.m_pGamer[nSlotNo].m_nChoiceFloorCardPos = nPackEatFloorPos;
			ThrowFloorCard( m_nUserno, nCenterCardNo );
			
			Sleep(250);

			if (m_nRound == 9 && m_nUserno == 1)		// 막판에 남은카드가 있으면 다 뿌린다. ( 서비스카드 )
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

			//먹었는데 쌀떄.
			if ( bUserEat && nUserFloorPos == nPackEatFloorPos ) { //유저가 바닥패를 먹었는데 뒤짚은팩도 같은것일때.

				EventSync( nSlotNo, 16, GS_SSAM );

				int nBbukCnt = ++m_pFloor.m_pGamer[nSlotNo].m_nBbukCnt;

				//유저가 뻑한 카드를 담아둔다..
				for ( int nTempBbuk = 0; nTempBbuk < 10; nTempBbuk++ ) {
					if ( m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] == -1 ) {
						m_pFloor.m_pGamer[nSlotNo].m_nBbukCardNo[nTempBbuk] = nCenterCardNo / 4;
						break;
					}
					
				}

				// 처음뻑 체크, 다음뻑 체크.(연뻑)
				if ( nBbukCnt <= 3 && nBbukCnt == m_nRound + 1 ) { 					
	
					BigInt bival = (BigInt)pow( 2, ( nBbukCnt - 1 ) ) * BBUK_X * m_biMoney;
					
					BigInt bimoney = 0;
					int nNextUserSlotNo = 0;

					for ( int i = 1 ; i < MAX_ROOM_IN ; ++i )		//다른 사용자들에게 뻑한 돈을 뺏는다.
					{
						//씨발....이것두.......에러가 두개씩이네..ㅠㅠ
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
							g_pCMainFrame->PutTextInGame( "Com1" , "첫뻑" , 
							bival , RGB( 255 , 255 , 165 ) );						
						}
						else
						{							
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
							
							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "첫뻑" , 
								bival , RGB( 255 , 255 , 165 ) );
						}
					}
					else if( nBbukCnt == 2 )
					{
						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "연뻑" , 
												bival , RGB( 255 , 255 , 165 ) );						
						}
						else
						{
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "연뻑" , 
													bival , RGB( 255 , 255 , 165 ) );
						}
					}
					else if ( nBbukCnt == 3 )
					{
						m_pFloor.m_nWinerNo = nSlotNo;

						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "쓰리연뻑" , 
													bival ,  RGB( 255 , 255 , 165 ) );						
						}
						else
						{
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "쓰리연뻑" , 
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
						g_pCMainFrame->PutTextInGame( "Com1" , "쓰리뻑" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );					
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "쓰리뻑" , 
													0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
					return true;
				}
				else {

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{	
							g_pCMainFrame->PutTextInGame( "Com1" , "뻑" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );					
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "뻑" , 
													0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
				}
			}
			else
			{
				//먹었는데...따닥.
				if ( bUserEat && 
				    nUserFloorPos != nPackEatFloorPos &&
				    bCenterEat && 
				    nUserCardNo / 4 == nCenterCardNo / 4 && m_nRound != 9 ) {	//따닥.

					EventSync( nSlotNo, 17, GS_DADDAK );
					//첫따닥...첫 따닥은 돈준다.
					if ( m_nRound == 0  ) {//첫 판.

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
							g_pCMainFrame->PutTextInGame( "Com1" , "첫 따닥" , 
							bival , RGB( 255 , 255 , 165 ) );
						}
						else
						{							
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
							
							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "첫 따닥" , 
								bival , RGB( 255 , 255 , 165 ) );
						}
					}
					else
					{
						if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
						{
							g_pCMainFrame->PutTextInGame( "Com1" , "따닥" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );
						}
						else
						{							
							nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
							
							g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "따닥" , 
								0, RGB( 255 , 255 , 165 ) , FALSE );
						}
					}

					
					//DTRACE(" %d 유저 따닥 ", m_nUserno );

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

				}
				

				if ( !bUserEat && m_nSelectCard != BOMB_CARDNO && // 내가 폭탄을 한후 팩에서 바로 깠을때..바닥에 맞으면 쪽이되어서 BOMB_CARDNO체크를 넣었다.
					 nUserFloorPos == nPackEatFloorPos && m_nRound != 9 ) {	// 쪽일때..막판 쪽은 패를 가져오지 않는다.

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					EventSync( nSlotNo, 15, GS_JJOK );

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{
						g_pCMainFrame->PutTextInGame( "Com1" , "쪽" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;
						
						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "쪽" , 
							0, RGB( 255 , 255 , 165 ) , FALSE );
					}
				}

				// 먹은 화투 수거
				int cnt1 = 0, cnt2 = 0;
				int nCardNo1, nCardNo2;

				if ( bUserEat )		//자신이 먹은패..가져오기
					cnt1 = EatCardUser( m_nUserno, nUserFloorPos, nCardNo1 );
				
				if ( nOtherFloorPos >= 0 ) //서비스 패를 뒤짚었으면 가져와야하므로..
					EatCardUser( m_nUserno, nOtherFloorPos, nCardNo );
				
				if ( bCenterEat )	//뒤짚은 패가 먹은거 가져오기.
					cnt2 = EatCardUser( m_nUserno, nPackEatFloorPos, nCardNo2 );

				// 폭탄 or 싼거 먹음
				if ( cnt1 == 4 && m_nRound != 9 )  {   //자신이 먹은패가 4장을 가져오면 한장씩 

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					//유저가 뻑한 카드를 담아둔다..
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

				if ( cnt2 == 4 && m_nRound != 9 )  {	  //팩에서 뒤짚은패가 4장을 가져오면 한장씩.

					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					//유저가 뻑한 카드를 담아둔다..
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

				if (m_pFloor.m_nCardCnt == 0 && m_nRound != 9)	// 쓸하면 한장씩 가져온다.,
				{
					EventSync( nSlotNo, 20, GS_PANSL, 100 );
					
					for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
						m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt++;
					}

					if ( m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo < 0 )
					{						
						g_pCMainFrame->PutTextInGame( "Com1" , "판쓸" , 
							0 , RGB( 255 , 255 , 165 ) , FALSE );					
					}
					else
					{
						nGameUserNo = m_pFloor.m_pGamer[nSlotNo].m_nGamerUserNo;

						g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "판쓸" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
					}
				}

				GetPeeEachGamer( nSlotNo ); 	

				for ( int f = 1; f < MAX_ROOM_IN; f++ ) {
					m_pFloor.m_pGamer[( nSlotNo+f ) % MAX_ROOM_IN].m_GetOtherUserCardCnt = 0;
				}

			}		


			if ( m_pFloor.m_pGamer[nSlotNo].Evalutaion() )	//점수계산.
			{
				if ( SelectNineCardPee(m_nUserno) )	//선택을 기다린다.
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
		
			if (m_pFloor.m_pGamer[nSlotNo].CheckEvalutaion())		//고를 할수 있는지 체크.
			{
				

				if ( m_nRound == 9 || WaitGoStop(m_nUserno) == 0 )
				{
					m_pFloor.m_nWinerNo = nSlotNo;

					EventSync( nSlotNo, 9, GS_STOP );

					return true;
				}

				int ngoCnt = m_pFloor.m_pGamer[nSlotNo].AddGo();
				
				m_nLastGo = nSlotNo;

				g_pCMainFrame->PlaySound(g_Sound[(GS_GO+ngoCnt-1)  +  (g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50)], false); // bsw (이 함수에서 true는 반복 재생 여부)				
				

				//===========BGM SOUND=====================
				//원고 한후..
				if ( ngoCnt >= g_nGoCnt )
					g_nGoCnt = ngoCnt;


				EventSync( nSlotNo, ngoCnt - 1, -1 );


				char szJunk[255];
				memset( &szJunk , 0x00 , sizeof( szJunk ));

				sprintf( szJunk , "%d고" , ngoCnt );

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
	g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOFRPUT], false); // bsw (이 함수에서 true는 반복 재생 여부)

	m_pFloor.OnlyAdd(nCardNo);
}

void CGoStopEngine::AddCardUser(int nUserNo, int nCardNo)
{
	g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOSHARE], false); // bsw (이 함수에서 true는 반복 재생 여부)

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
	//1초를 기다린후 시간으로 끝이나면 쓰레드를 완전 주긴다.
	DWORD dwRe = WaitForSingleObject( m_hGEThread, WAIT_1TIME ); 	
	if ( dwRe == WAIT_TIMEOUT ) {
		
		SetPaintEvent();
		SetActionEvent();
		SetOtherActionEvent();

		m_bGameStop = true;	
	}

	//한번더 체크를 한다.
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

	
	//게임 판수를 무조건 올려준다. 초기화는 대기실로 오면 무조건.
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


	// 게임이 시작되면..
    g_pCMainFrame->AddGameChatText( "☞게임 시작" , RGB( 189 , 255 , 206 ));
	

	m_bPlaying = true;
	m_bGameStop = false;
	m_bOtherWait = false;
	m_bGameEndWill = false;

	
	g_bDeal = true;
	g_SelectCardNo = -1;
	g_nGoCnt = 0;
	g_pCMainFrame->l_nTempCardNo = -1;
	g_pCMainFrame->l_nCardFlipFrameCnt = 0;

    //씨발..이것땜에 오픈늦어졌다..저때따~~~~~~~ㅜㅜㅜ..영훈씨만 욕머거따....ㅠㅠ..그래도 테스트는 해라..
    m_nLastGo = -1;


	//카드의 기본 포지션 세팅.
	g_pCMainFrame->InitCardPostion();



	//사람이 들어 왔을때만...세명이 다들어와야 한다.
	if ( m_bMasterMode == TRUE ) {

		//게임 Thread가 살아있는지 체크를 한다.
		GameThreadCK();

		m_Hwatu.Shuffle();
		nRoomSeed = GetRandom(1, 100000);
		srand(nRoomSeed);

		int nCardCnt = m_Hwatu.GetCardPack(m_pCardPack);

		if ( m_pFloor.RealGame() ) {	//방에 자신혼자 있거나 3명이 다 차야 Start()여기에 들어오므로 여기에선 인원을 체크해야한다.

			//여기는 사람이 들어왔을때 타는것이므로...
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

		//게임 Thread가 살아있는지 체크를 한다.
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

int CGoStopEngine::EatCardUser(const int nUserNo, const int nCardPos, int &nRecvCardNo)		// 카드를 먹을때 같은 카드를 먹을수도 있다.
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

			g_pCMainFrame->PlaySound(g_Sound[GS_HWATOOMOVE], false); // bsw (이 함수에서 true는 반복 재생 여부)

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

			//DTRACE("(2)자기 액션기다림");
			ResetEvent(m_hActionEvent);

			g_pCMainFrame->UserCardSetTime(slotNo);

			m_bMyKetInput = true;
			
			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//60초 후에..

			g_bUserCardRecv = true;
			
			if ( m_bGameStop ) return -1;
		}
		else {

			//DTRACE("(2)다른사람 액션기다림");
			ResetEvent(m_hOtherUserActionEvent);

			g_pCMainFrame->UserCardSetTime(slotNo);

			m_bOtherWait = true; // SelectCard함수가 끝이나도 true를 해주지만. 서비스를 먹으면 이리 바로 올수도 있다.
			DWORD dwRe = WaitForSingleObject( m_hOtherUserActionEvent, WAIT_90TIME );	//12초 후에..

			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 149", true );
				g_pClient->Destroy();
			}


			g_bUserCardRecv = true;
		}
		
		if ( m_pFloor.m_pGamer[slotNo].m_bUserEvent ) {	// 12초안에 사용자의 메세지가 들어오면 선택된 카드로 바꾼다.
			
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
			m_pFloor.m_pGamer[slotNo].AutoSelect();			//카드를 먹을수있는걸 선택. 먹을게 없으면 마지막 번호.

			nChoiceUserCardPos = m_pFloor.m_pGamer[slotNo].m_nChoiceUserCardPos;
			anFloorPos = m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos;

		}
	}
	else {

		Sleep(300);	//컴터가 칠때만 조금 기다린다.

		m_pFloor.m_pGamer[slotNo].AutoSelect();			//카드를 먹을수있는걸 선택. 먹을게 없으면 마지막 번호.

		nChoiceUserCardPos = m_pFloor.m_pGamer[slotNo].m_nChoiceUserCardPos;
		anFloorPos = m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos;
	}

    if ( nChoiceUserCardPos == 0xff )
		return -1;

	nCardNo = m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos];

	g_SelectCardNo = nCardNo;

	if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState == NOTICE_THREE ) {

		if ( m_pFloor.m_pGamer[slotNo].m_bComputer == false &&
			 g_MyUser == g_CurUser ) {	//가지고 있는게 같은게 3장이면 , 자기자신이 아니면 할필요 없다.
			
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
			//DTRACE("ShakeQ() - 기다린다");

			g_pCMainFrame->ShakeQ();

			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//60초 후에..
            
			//DTRACE("풀렸다.");
			if ( dwRe == WAIT_TIMEOUT ) {
				m_nShake = 1;
				g_pCMainFrame->l_bShowDialog = FALSE;	
			}

			if ( m_nShake == 1 ) {
				g_pClient->SendShake( nChoiceUserCardPos, g_nCardNoArr );	//흔듬과 선택된 카드를 클라이언트에게 알려준다.

				m_pFloor.m_pGamer[slotNo].m_nUserThreeState = NOTICE_SHAKE;

				//DTRACE("흔들고난후 1");
				
				g_pCMainFrame->PlaySound(g_Sound[GS_SHAKE + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

				//DTRACE("흔들고난후 2");

				ResetEvent(m_hActionEvent);

				//DTRACE("흔들고난후 3");
				///*************************************
				//여기에서 흔들었습니다 그림을 보여준다.
				//*************************************	
				g_pCMainFrame->Shake();

				//DTRACE("흔들고난후 4");
				DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12초 후에..

				//DTRACE("흔들고난후 5");
				if ( dwRe == WAIT_TIMEOUT ) {
					g_pCMainFrame->l_bShowDialog = FALSE;
					//DTRACE("흔들고 여기로?");						
				}

				memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );
				//DTRACE("*************1***********");

			}
		}
		else if ( m_pFloor.m_pGamer[slotNo].m_bComputer == true ) {			//가지고 있는게 같은게 3장이면


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
			//여기에서 흔들었습니다 그림을 보여준다.
			//*************************************	
			g_pCMainFrame->Shake();	


			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12초 후에..
			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_bShowDialog = FALSE;	
			}
		}
		else if ( g_nCardNoArr[0] >= 0 ) {

			m_pFloor.m_pGamer[slotNo].m_nUserThreeState = NOTICE_SHAKE;

			
			g_pCMainFrame->PlaySound(g_Sound[GS_SHAKE + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

			ResetEvent(m_hActionEvent);

			//*************************************
			//여기에서 흔들었습니다 그림을 보여준다.
			//*************************************	
			g_pCMainFrame->Shake();


			DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_90TIME );	//12초 후에..
			if ( dwRe == WAIT_TIMEOUT ) {
				g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 152", true );
				g_pClient->Destroy();
			}
		}
	}
	memset( g_nCardNoArr, -1, sizeof(g_nCardNoArr) );

	if ( g_CurUser == g_MyUser ) { // 자신이면 다른사람에게 카드선택을 알린다. 흔듬은 위에서 보낸다.
		if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState != NOTICE_SHAKE ) {
			//DTRACE("사용자 카드 번호 보냄 : %d", m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos]);
			g_pClient->SendUserCard( nChoiceUserCardPos );
		}
	}

	int nTempCurNo = SelectFloorTwoPee( slotNo, nCardNo, m_nRound, -1 );
	
	

	if ( nTempCurNo != 0xff ) {
		anFloorPos = m_pFloor.m_strCard[nTempCurNo].nPosNo;
		m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos = anFloorPos;
	}

	//바닥에 패가 두개 있는지 체크
	if ( nCardNo < 48 && nCardNo >= 0 && m_pFloor.m_pGamer[slotNo].m_nUserThreeState != NOTICE_BOOM ) { //폭탄일때 아래서 업카드를 해준다.
		UpCard( nUserNo, nChoiceUserCardPos );
	}
	
	if ( m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos] >= 48 ) {
		if ( nCardNo != BOMB_CARDNO )			//서비스패가 선택되었을때...
		{
			EatMyCardUser( nUserNo, nChoiceUserCardPos ); // "유후~"


            // 서비스 패를 사용하면 상대방의 패를 한 장 가져온다.
            GetOnePee( slotNo ); // ham 2006.02.22 [*PEE]


			// 받은 카드에 의해 대통령이 될수 있음.
			int nPregCardNo = m_pFloor.m_pGamer[slotNo].IsPregident();

			if ( nPregCardNo >= 0 )		// 0보다 크면 대통령이다.
			{
				m_pFloor.m_nWinerNo = slotNo;
				m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEvalution = PREGIDENT_POINT;

				m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nResultScore = PREGIDENT_POINT;
				
				//대통령일때 처리를 해야함....
				m_pFloor.m_nPregidentNo = nUserNo;

				g_pCMainFrame->PlaySound(g_Sound[GS_CHONGTONG + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false);

				//여기선 다이얼로그가 3초후에 죽고 이걸푼다.
				//하지만 결과창에서 다시 걸어버린다. 여긴 타이머는 필요없다.
				//나중에 혹시 모르니깐 10초정도로 해도된다. 결과창에서 또 걸기 때문에..
				ResetEvent(m_hActionEvent);
				//DTRACE("(3) - 2 - CB_PREGIDENT 를 보내고 바닥에 대통령..");


				g_pCMainFrame->PregiDent( nPregCardNo );


				DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );	//12초 후에..
				if ( dwRe == WAIT_TIMEOUT ) {
					g_pCMainFrame->l_bShowDialog = FALSE;
					//DTRACE("g_pCMainFrame->l_bShowDialog = FALSE - 4");
				}
		

				return -1;
			}

			//DTRACE("서비스를 자기 패에서 바꾸면");

			Sleep(300);

			return SelectCard( nUserNo, anFloorPos, abEat );
		}
		else {	//폭탄카드이면..ThrowCard하지 말고..나머지는 그대로..
			
			m_pFloor.m_pGamer[slotNo].m_bCard[nChoiceUserCardPos] = 0xff;

			m_pFloor.SortUserCard(nUserNo);
			SortAndRePaint( nUserNo );
			return BOMB_CARDNO;
		}

	}



	if ( anFloorPos == 0xff )							//먹을게 없을때..
	{

		if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState == NOTICE_SHAKE ) {		//먹을게 없지만 흔들었을떄.
			m_pFloor.m_pGamer[slotNo].m_nShakeCnt++;
		}

		g_pCMainFrame->PlaySound(g_Sound[GS_MISS], false); // bsw (이 함수에서 true는 반복 재생 여부)
			
		abEat = false;
		anFloorPos = m_pFloor.GetEmptyPos();
		m_pFloor.m_pGamer[slotNo].m_nChoiceFloorCardPos = anFloorPos;
		ThrowCard( nUserNo );

	}
	else												//먹었을때.
	{

		if ( m_pFloor.m_pGamer[slotNo].m_nUserThreeState == NOTICE_BOOM ) {			//먹을게 있는데 폭탄일떄.
			
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
				g_pCMainFrame->PutTextInGame( "Com1" , "폭탄" , 
					0 , RGB( 255 , 255 , 165 ) , FALSE );				
			}
			else
			{
				nGameUserNo = m_pFloor.m_pGamer[slotNo].m_nGamerUserNo;
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[nGameUserNo].m_sUserInfo.szNick_Name , "폭탄" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
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


	//폭탄카드 초기화 
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

	g_pCMainFrame->PlaySound(g_Sound[GS_CARDUP], false); // bsw (이 함수에서 true는 반복 재생 여부)


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

	g_pCMainFrame->PlaySound(g_Sound[GS_CARDUP], false); // bsw (이 함수에서 true는 반복 재생 여부)

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
	g_pCMainFrame->PlaySound(g_Sound[GS_BONUS + (g_CGameUserList.m_GameUserInfo[slotNo].m_nVoice * 50)], false); // bsw (이 함수에서 true는 반복 재생 여부)
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
		
		nSum[m_pFloor.m_strCard[i].nCardNo / 4] ++; // nSum은 같은 종류의 카드 수 합계( 1월~12월 )
	}

	for ( int nCardNo = 0 ; nCardNo < 12 ; ++nCardNo )
	{
		if ( nSum[nCardNo] == 4 ) // 같은 종류의 패가 4장 이면
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

				if ( nCardNo == m_pFloor.m_strCard[i].nCardNo / 4 ) // 같은 종류의 패일 경우
				{
					if (bFirst) // 첫번째 장이면
					{
						bFirst = false;
						nPos = m_pFloor.m_strCard[i].nPosNo; // nPos는 현재 패의 위치
					}
					else {
						m_pFloor.m_nPosCnt[m_pFloor.m_strCard[i].nPosNo]--; // 현재 Pos의 카드수 -1
						m_pFloor.m_strCard[i].nPosNo = nPos; // 현재 카드의 Pos는 nPos
						m_pFloor.m_nPosCnt[m_pFloor.m_strCard[i].nPosNo]++; // 현재 카드의 pos에 위치한 카드수 +1

					}
				}
			}			
		}
	}

	if ( i >= 12 )			// 대통령이 아니거나 같은 종류의 패가 없으면...
		return false;

	return false;
}

// 이용자 위치, 뒤짚은 카드, 이용자가 먹은 카드 위치(내부), 뒤짚은 카드 놓을 위치
bool CGoStopEngine::ThrowFloor(int nSlotNo, const int nCardNo, const int nUserEatFloorPos, int nRoundNo, int &nPackEatFloorPos)
{
	bool bEat = false;
	int nFloorCardCurNo;	//플로어카드 어레이중...몇번째냐...

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


// 이용자 위치, 뒤짚은 카드, 이용자가 먹은 카드 위치(화면위치)
int CGoStopEngine::SelectFloorTwoPee(int nSlotNo, int nCardNo, int nRoundNo, int nUserEatFloorPos)
{								  //(int nSlotNo, int nCardNo, int roundNo, int nExceptNo)

	BYTE arrPos[5];	// 뒤짚어진 패와 바닥에 패가 같으면 여기에 그 바닥패의 위치가 순서대로 들어간다.
	int cnt = IsTwoPos( nCardNo, arrPos ); // 같은 그림의 바닥 위치 결정


	// cnt가 1이면 먹을게 한장있는것이고 nRoundNo == 9이면 막판이고.
	// m_pFloor.m_nPosCnt[arrPos[0]] > 1이면 그위치에 2장이상이 깔려있다는 것이고.  
	// nUserEatFloorPos == arrPos[0] 먹은것과 같은 카드일때. 마지막에 싸는 경우 이다.	
	// 그러나 막판 뻑은 없기때문에. 맞은게 없는걸로 처리한다.

	if ( cnt == 1 && nRoundNo == 9 &&	
		m_pFloor.m_nPosCnt[arrPos[0]] > 1 && nUserEatFloorPos == arrPos[0])	
		return 0xff;
	
	if (cnt == 1 || cnt == 3)
		return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);	// 1장 또는 3장이 있을때..바닥의 위치.
	
	if ( cnt <= 0 && cnt != 2)
		return 0xff;

	if ( nUserEatFloorPos != -1 )		//못먹었으면 -1을 보내기때문에.
	{
		if (arrPos[0] == nUserEatFloorPos)	//첫번째걸 먹었으면..
			return m_pFloor.GetFIrstPosFromFloorPos(arrPos[1]);

		if (arrPos[1] == nUserEatFloorPos)  //두번째걸 먹었으면...
			return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
	}

	int nFloorCardCurNo1 = m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
	int nFloorCardCurNo2 = m_pFloor.GetFIrstPosFromFloorPos(arrPos[1]);

	// 두개의 카드가 동급이면 첫번째카드를 먹는다.
	if ( g_pGoStopValue[m_pFloor.m_strCard[nFloorCardCurNo1].nCardNo] ==
		 g_pGoStopValue[m_pFloor.m_strCard[nFloorCardCurNo2].nCardNo] )
		 return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);

	m_nSelectCard = -1;

	if ( m_pFloor.m_pGamer[nSlotNo].m_bComputer )	// Computer이면 첫번째카드를 선택한다.
	{
		return m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
	}
	else if ( g_MyUser == g_CurUser ) {			
		//*******************************************
		//사용자의 선택을 기다린다. 
		//일정시간을 주고 시간으로 빠져나왔는지 이벤트로 나왔는지 체크해서 적절히해준다.
		//*******************************************
		//엑션 초기화.
		//DTRACE("CB_SELECT_TWO_CARD 액션을 건다");
		ResetEvent(m_hActionEvent);

		g_pCMainFrame->SelectTwoCard( g_CurUser, nCardNo );


		DWORD dwRe = WaitForSingleObject(m_hActionEvent, WAIT_60TIME);
		if ( dwRe == WAIT_TIMEOUT ) {

			m_nSelectCard = m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]);
			g_pCMainFrame->l_bShowDialog = FALSE;

			//DTRACE("g_pCMainFrame->l_bShowDialog = FALSE - 5");
		}

	
		//카드를 선택했으면 다른사용자에게 카드 번호를 보내준다..
		//DTRACE("CB_SELECT_TWO_CARD 카드를 보낸다.");
		g_pClient->SendUserCard( m_nSelectCard );

		return m_nSelectCard;
	}
	else {	//다른사용자가 두장을 선택중이면 
		//DTRACE("카드를 선택중입니다");	//다이얼로그를 띠운다.
		//DTRACE("(2)다른사람 액션기다림");

		ResetEvent(m_hOtherUserActionEvent);

		g_pCMainFrame->WaitCardChoice();		

		m_nOtherUserCard = m_pFloor.GetFIrstPosFromFloorPos(arrPos[0]); //여기에서 갑자기 짤리면 이걸 선택하게 한다.
		
		m_bOtherWait = true;
		DWORD dwRe = WaitForSingleObject( m_hOtherUserActionEvent, WAIT_90TIME );	//12초 후에..
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pClient->Destroy();
			g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 155", true );
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
			retPos[cnt++] = i;		// 여기에 뒤짚은 카드와 동일한 카드의 바닥위치가 순서대로 들어간다.
		}
	}

	return cnt;
}

void CGoStopEngine::GetPeeEachGamer( int nSlotNo )
{

	//yuno 2005.10.30
	//소리 및 효과는 한번만 나야한다.
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

	//소리 및 효과는 한번만 나야한다.
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
    // "한장씩" 사운드가 나오지 않은 경우에만 "유후~" 사운드 출력
    if ( !bGetOnePee ) {
        g_pCMainFrame->PlaySound(g_Sound[GS_BONUS + (g_CGameUserList.m_GameUserInfo[nSlotNo].m_nVoice * 50)], false); // bsw (이 함수에서 true는 반복 재생 여부)
    }
    ////
}
////

bool CGoStopEngine::SelectNineCardPee(const int nUserNo)
{
	int slotNo = (nUserNo + m_pFloor.m_nMasterSlotNo) % MAX_ROOM_IN;

	m_nNineCard = -1;

	//엑션 초기화.
	ResetEvent(m_hActionEvent);
	
	if ( m_pFloor.m_pGamer[slotNo].m_bComputer )	// Computer
	{
		Sleep(200);		// 무조건 쌍피로 선택
		return true;
	}
	else if ( g_MyUser == g_CurUser ) { 

		g_pCMainFrame->SelectNine();

		//사용자 엑션 기다림. // 다이얼로그에서 하므로 일정시간만 준다.
		DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );
		if ( dwRe == WAIT_TIMEOUT ) {
			m_nNineCard = 1;
		}


		//쌍피를 어떻게 했는지를 보낸다..
		//DTRACE("사용자의 국진선택하고 보냄.. : %d", m_nNineCard );
		g_pClient->SendNineCard( m_nNineCard );


		return (m_nNineCard) ? true : false;

	}
	else {
		
		//쌍피 선택중...
		//DTRACE( "쌍피를 선택중입니다. 다른사용자의 선택을기다림...");


		ResetEvent(m_hOtherUserActionEvent);		
		g_pCMainFrame->WaitCardChoice();

		m_nOtherUserCard = 1; //여기에서 갑자기 짤리면 이걸 선택하게 한다.

		m_bOtherWait = true;		
		DWORD dwRe = WaitForSingleObject( m_hOtherUserActionEvent, WAIT_90TIME );	//12초 후에..
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pClient->Destroy();			
			g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 157", true );
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


		//다이얼로그에서 체크하므로 일정 시간만 준다.
		DWORD dwRe = WaitForSingleObject( m_hActionEvent, WAIT_60TIME );
		if ( dwRe == WAIT_TIMEOUT ) {
			m_nGoStop = 0;
		}


		g_pClient->SendGoStop( m_nGoStop );

		nRe = m_nGoStop;
	}
	else  {	 //다른사람이 고스톱 기다리는중이라고 바로 여기서 뿌려버린다.
		
		ResetEvent(m_hOtherUserActionEvent);

		g_pCMainFrame->WaitGoInfo();


		m_nOtherUserCard = 0; //여기에서 갑자기 짤리면 이걸 선택하게 한다.
		m_bOtherWait = true;
		DWORD dwRe = WaitForSingleObject(m_hOtherUserActionEvent, WAIT_90TIME);	//상대방에게서 메세지가 올때까지 기다린다.
		if ( dwRe == WAIT_TIMEOUT ) {
			g_pClient->Destroy();
			g_pCMainFrame->l_Message->PutMessage( "네트워크가 불안정합니다.", "Code - 159", true );
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

		if (m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEatCnt[4] >= 10)	// 피로 점수 남
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
				 m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nEatCnt[4] < 8 )	// 피박
			{
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_bPbak = TRUE;

				mul *= 2;
			}
		}

		if (m_pFloor.m_pGamer[m_pFloor.m_nWinerNo].m_nEatCnt[0] >= 3)	// 광으로 점수 남
		{
			if (m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nEatCnt[0] < 1)	// 광박
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
		
		if ( nDokBakSlot < 0 )	// 보통
		{
			BigInt bis = ( m_pFloor.m_nWinerNo+i ) % MAX_ROOM_IN;		//다른유저들 돈차감.
			int chonCheck = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nLoseEval;

		//	bival = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nLoseEval * m_biMoney * m_pFloor.m_nBaseMulti * nMissionMultiply;
			bival = chonCheck * m_biMoney * m_pFloor.m_nBaseMulti * nMissionMultiply;
			
			TRACE("bis:%d   chonCheck:%d  m_biMoney:%d  m_pFloor.m_nBaseMulti:%d nMissionMultiply:%d bival:%d \n",bis,chonCheck,m_biMoney,m_pFloor.m_nBaseMulti,nMissionMultiply,bival);

			//패자정보갱신.
			if ( m_pFloor.RealGame() ) {
				
				nGamerUserNo = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nGamerUserNo;
				//유저가 잃은 돈보다 더 적게 가지고 있으면 그만큼만 차감.
				
				if ( bival >= g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biUserMoney ) 
					bival = g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biUserMoney;

				int chaGam = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biCurWinLoseMoney;

				TRACE(" 차감전:%d    Loser 실제 차감금액:%d\n",chaGam,bival);

				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biCurWinLoseMoney -= bival;
				m_pFloor.UserMoneyVal( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN, bival, false );
				g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.nLose_Num++;	

			}

			biwinmoney2 += bival;
			m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biLoseMoney = bival;
//			TRACE("m_nWinerNo %d %d\n",m_nWinerNo, bival);
		}
		else	// 독박이 있는경우
		{
			
			if ( nDokBakSlot == (m_pFloor.m_nWinerNo+i) % MAX_ROOM_IN ) {

				bival = biwinMoney * m_biMoney * m_pFloor.m_nBaseMulti * nMissionMultiply;
				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_biLoseMoney = bival;
			

				m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_bDokbak = TRUE;

				EventSync( (m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN, 19, GS_DOKBAK, 100 );

				//패자정보갱신.
				if ( m_pFloor.RealGame() ) {


					nGamerUserNo = m_pFloor.m_pGamer[(m_pFloor.m_nWinerNo+i)% MAX_ROOM_IN].m_nGamerUserNo;

					//유저가 잃은 돈보다 더 적게 가지고 있으면 그만큼만 차감.
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
		//승자정보갱신.
		biwinmoney2 = GameSystemMoney(biwinmoney2);
		
	
		//승자정보갱신.
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

	//마스터를 변경한다.
	//유저 넘버를 변경.
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

	//floor 초기화.
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
		sprintf( szTempStr, "%s님", g_CGameUserList.m_GameUserInfo[nGameUserCnt].m_sUserInfo.szNick_Name );
		strcpy( szTempStr2, "나가기예약" );
	}
	else {
        g_pCMainFrame->m_bShowExitReservIcon[nSlotNo] = FALSE;
		sprintf( szTempStr, "%s님", g_CGameUserList.m_GameUserInfo[nGameUserCnt].m_sUserInfo.szNick_Name );
		strcpy( szTempStr2, "나가기취소" );
	}
	
	m_pFloor.m_pGamer[nSlotNo].m_ExitReserv = bExit;
    g_pCMainFrame->AddGameChatText( szTempStr, RGB(255, 255, 0));
    g_pCMainFrame->AddGameChatText( szTempStr2, RGB(255, 255, 100));
}

void CGoStopEngine::UserStopInfoUpdate()
{
	//방안의 사람 정보 모두 갱신
	int nGamerUserNo  = -1;
	float fTemp = 0;
	int nIntTemp = 0;

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		nGamerUserNo = m_pFloor.m_pGamer[i].m_nGamerUserNo;

		if ( m_pFloor.m_pGamer[i].m_biWinMoney > g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biMaxMoney ) {
			 g_CGameUserList.m_GameUserInfo[nGamerUserNo].m_sUserInfo.biMaxMoney = m_pFloor.m_pGamer[i].m_biWinMoney ;
		}

		//총전적( 이긴수 + 진수 + 비긴수 )
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
	g_sImageData[nUserSlotNo].nFile = 0; //이미지 파일명
	memset( g_sImageData[nUserSlotNo].szCurImageFileName , 0x00 , sizeof( g_sImageData[nUserSlotNo].szCurImageFileName ));
	g_sImageData[nUserSlotNo].nLastDec = 0;	    //이미지 최종단계
	g_sImageData[nUserSlotNo].nCurDec  = 0;		//이미지 현재단계
	g_sImageData[nUserSlotNo].nUserImageNo = 0;   //유저 이미지 번호( 현재 자신의 아바타 몇번째 )  무료일때는 0
	memset( g_sImageData[nUserSlotNo].szUserImage , 0x00 , sizeof( g_sImageData[nUserSlotNo].szUserImage )); // 유저가 가지고 있는 그림( 7 바이트 )
	
	g_sImageData[nUserSlotNo].bFreeGuest = TRUE; // 무료 회원이면 true 유료회원이면 FALSE	

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
    g_pCMainFrame->m_pWaitChat->Init();                // 대기실 채팅창 텍스트 초기화
    g_pCMainFrame->m_SCB_WaitChat->SetElem( 0 );
    g_pCMainFrame->m_SCB_WaitChat->StartPage();
    g_pCMainFrame->m_SCB_WaitRoom->StartPage();
    g_pCMainFrame->m_SCB_WaitUser->StartPage();
    g_pCMainFrame->m_IME_WaitChat->InputEnd();     // 대기실 채팅창 IME도 초기화 한다.
    g_pCMainFrame->m_IME_WaitChat->ClearBuffer();
    g_pCMainFrame->m_IME_WaitChat->InputBegin(true);
	

	//게임방룸내의 아바타를 지운다.
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
    g_pCMainFrame->m_nCardBlinkCnt = 0;     // 선 정할 때, 화투 주위를 깜빡이게 하는데 사용되는 카운터
    g_pCMainFrame->m_nCardBlinkFrm = 0;     // 선 정할 때, 화투 주위를 깜빡이게 하는데 사용되는 프레임 변수

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
    g_pCMainFrame->l_MaxBTN->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr",  22, 21 );   // ham 2005.09.22 이미지 크기 재설정
	g_pCMainFrame->l_MaxBTN2->ReGetButtonImage(".\\image\\game\\btn_bar_fullmode.spr", 22, 21 );
    g_pCMainFrame->l_ExitBTN->ReGetButtonImage(".\\image\\game\\btn_bar_close.spr",    22, 21 );

    g_pCMainFrame->l_Exit2BTN->ReGetButtonImage(".\\image\\game\\btn_board_close.spr",      50, 30 );
    g_pCMainFrame->l_CaptureBTN->ReGetButtonImage(".\\image\\game\\btn_capture_game.spr",   50, 30 );
    g_pCMainFrame->l_ConfigBTN->ReGetButtonImage(".\\image\\game\\btn_board_option.spr",    49, 21 );

	g_pCMainFrame->l_nStartBtnCnt = 0;
    g_pCMainFrame->m_pGameChat->Init();
    g_pCMainFrame->m_IME_GameChat->InputEnd();     // 게임화면 채팅창 IME 초기화
    g_pCMainFrame->m_IME_GameChat->ClearBuffer();
    g_pCMainFrame->m_SCB_GameChat->SetElem( 0 );
    g_pCMainFrame->m_SCB_GameChat->StartPage();
    //[alpha]
    memset( &(g_pCMainFrame->m_sPrevDlgInfo), 0x00, sizeof( g_pCMainFrame->m_sPrevDlgInfo ));

	//게임방룸내의 아바타를 지운다.
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