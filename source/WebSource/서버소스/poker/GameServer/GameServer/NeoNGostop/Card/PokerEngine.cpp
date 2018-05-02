#include "PokerEngine.h"
#include "..\World.h"

#include "..\dtrace.h"

extern World g_World;

CPokerEngine::CPokerEngine()
{
	m_pCardPack = NULL;
}

CPokerEngine::~CPokerEngine()
{

}


void CPokerEngine::CreateGame( int nCardCnt )
{

	SAFE_DELETE( m_pCardPack );
	m_pCardPack = new CCardPack(nCardCnt);

	m_pCardPack->Shuffle();
	m_pCardPack->Shuffle();
	m_pCardPack->Shuffle();
	
}

void CardLog( int nRoomNo, char *pNickName, BYTE nCardNo )
{
	char pTemp[255];
	memset( pTemp, 0x00, sizeof(pTemp) );

	switch( nCardNo / 13 ) {
		case 0:
			sprintf( pTemp, "(방%d) Nick : %s, 종류 : 크로버, %d", nRoomNo, pNickName, nCardNo % 13 + 2 );
			break;
		
		case 1:
			sprintf( pTemp, "(방%d)Nick : %s, 종류 : 하트, %d", nRoomNo, pNickName, nCardNo % 13 + 2 );
			break;
		
		case 2:
			sprintf( pTemp, "(방%d)Nick : %s, 종류 : 다이아, %d", nRoomNo, pNickName, nCardNo % 13 + 2 );
			break;

		case 3:
			sprintf( pTemp, "(방%d)Nick : %s, 종류 : 스페이드, %d", nRoomNo, pNickName, nCardNo % 13 + 2 );
			break;
	}
	DTRACE( "%s", pTemp );
}

void CPokerEngine::StartDeal( int nChannel, int nRoomNo )
{
	int nRoomMasterRoomInUserNo = 0;
	int nTemp;
	CRoomList *l_CRoomList;
	int i, j;
	int nRoomUserCnt = 0;
	int nRecordTemp = 0;
	int nRecordUserCnt = 0;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	l_CRoomList->RoomInit();
	UserInit( nChannel, nRoomNo );


	l_CRoomList->l_sRoomList.l_sRoomInfo.nGameDivi = 2;

	nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );

	DTRACE("(방:%d) 게임을 시작 판돈 : %I64u", nRoomNo, l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney );

	int nInitCardCnt = MAX_INIT_CARD;

	if ( l_CRoomList->l_sRoomList.l_sRoomInfo.bChoiceRoom )
		nInitCardCnt = 4;
	else
		nInitCardCnt = 3;

	for ( i = 0; i < nInitCardCnt; i++ ) {

		for ( j = 0; j < MAX_ROOM_IN; j++ ) {

			nTemp = ( nRoomMasterRoomInUserNo + j + 1 ) % MAX_ROOM_IN;
			
			if ( l_CRoomList->m_User[nTemp] == NULL || l_CRoomList->m_User[nTemp]->m_nGameJoin == 0 ) continue;
			
			if ( i == 2 ) {
				nRoomUserCnt++;
				l_CRoomList->m_nRound = 3;

				l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.biUserMoney -= l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney;	

				l_CRoomList->m_User[nTemp]->m_sUserDBUpdateInfo.m_biRaceMoney += l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney;


				nRecordUserCnt = 0;
				nRecordTemp = 0;
				for ( int k = 1; k < MAX_ROOM_IN; k++ ) {

					nRecordTemp = ( l_CRoomList->m_User[nTemp]->m_nRoomInUserNo + k ) % MAX_ROOM_IN;

					if ( l_CRoomList->m_User[nRecordTemp] == NULL ||
						 l_CRoomList->m_User[nRecordTemp]->m_nGameJoin == 0 ) continue;

					memcpy( l_CRoomList->m_User[nTemp]->m_sUserDBUpdateInfo.szOtherGameUser[nRecordUserCnt], 
						l_CRoomList->m_User[nRecordTemp]->m_MyInfo.l_sUserInfo.szNick_Name, MAX_NICK_LENGTH );
					nRecordUserCnt++;
				}

				DTRACE("(방:%d) %s유저가 %I64u베팅했다. 유저 남은 돈 : %I64u, 총베팅한 금액 : %I64u", 
					l_CRoomList->m_User[nTemp]->m_nRoomNo,
					l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.szNick_Name, 
					l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney,
					l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.biUserMoney, 
					l_CRoomList->m_User[nTemp]->m_sUserDBUpdateInfo.m_biRaceMoney );
						
			}

			if ( l_CRoomList->m_User[nTemp]->m_nGameJoin == 1  ) {
				l_CRoomList->m_User[nTemp]->m_nUserCard[i] = m_pCardPack->GetCard();
				l_CRoomList->m_User[nTemp]->m_nCardCnt++;

				//////////////////////////////////////////////////////////////////////////
				CardLog( l_CRoomList->m_User[nTemp]->m_nRoomNo, l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.szNick_Name,  l_CRoomList->m_User[nTemp]->m_nUserCard[i] );				
			}
		}
	}

	l_CRoomList->m_biRoomCurMoney = l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney * nRoomUserCnt;
}

void CPokerEngine::UserInit( int nChannel, int nRoomNo, int nRoomInUserNo, BOOL bSpecifyUser )
{
	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( bSpecifyUser )
			i = nRoomInUserNo;

		if ( l_CRoomList->m_User[i] == NULL ) {
			if ( bSpecifyUser )
				break;
			else
				continue;
		}

		l_CRoomList->m_User[i]->m_nCardCnt = 0;
		l_CRoomList->m_User[i]->m_nBettingCnt = 0;
		
		l_CRoomList->m_User[i]->m_bChoice = FALSE;
		l_CRoomList->m_User[i]->m_bCheck = FALSE;


		l_CRoomList->m_User[i]->m_sGameCurUser.bBing = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bCall = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bCheck = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bDdaDang = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bDie = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bFull = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bHalf = FALSE;
		l_CRoomList->m_User[i]->m_sGameCurUser.bQuater = FALSE;


		memset( l_CRoomList->m_User[i]->m_nUserCard, 0x00, sizeof(l_CRoomList->m_User[i]->m_nUserCard) );
		memset( &l_CRoomList->m_User[i]->m_sGameCurUser, 0x00, sizeof(l_CRoomList->m_User[i]->m_sGameCurUser) );
		memset( l_CRoomList->m_User[i]->m_CPokerEvalution.m_szJokboCard, 0xff, sizeof(l_CRoomList->m_User[i]->m_CPokerEvalution.m_szJokboCard) );
		l_CRoomList->m_User[i]->m_CPokerEvalution.m_nJokboCardCnt = 0;


		l_CRoomList->m_User[i]->m_biRoundMoney = 0;
		l_CRoomList->m_User[i]->m_biCallMoney = 0;
		l_CRoomList->m_User[i]->m_biSideMoney = 0;

		l_CRoomList->m_User[i]->m_CPokerEvalution.m_nJackPot = 0;
		l_CRoomList->m_User[i]->m_sUserDBUpdateInfo.biUserJackPotMoney = 0;

		l_CRoomList->m_User[i]->m_biTempRealRaceMoney = 0;

		if ( bSpecifyUser )
			break;
	}
}


int CPokerEngine::GameVerify( int nChannel, int nRoomNo )
{
	int nNodieCnt = 0;
	int nMasterSlot = 0;
	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
	
		if ( l_CRoomList->m_User[i] == NULL ) continue;

		if ( l_CRoomList->m_User[i]->m_nGameJoin == 1 ) //게임참가자이면.
			nNodieCnt++;
	}

	if ( nNodieCnt < 1 )
		return -1;

	if ( nNodieCnt == 1 )
		return -2;

	return 1;
}

int CPokerEngine::ResetMaster( int nChannel, int nRoomNo )
{
	int nMasterSlot = 0;

	
	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	for ( int i = 0 ; i < MAX_ROOM_IN ; i++ )
	{
		if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;

		nMasterSlot = i;
		break;
	}


	for ( i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( l_CRoomList->m_User[i] == NULL || 
			 l_CRoomList->m_User[i]->m_sGameCurUser.bDie ==  TRUE ||
			l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;	//널이거나 Die이거나 대기중일때.

		l_CRoomList->m_User[i]->m_bRoomMaster = FALSE;
		l_CRoomList->m_User[i]->Evalution();
	}

	for ( i = 0 ; i < MAX_ROOM_IN ; i++ )
	{
		if ( l_CRoomList->m_User[i] == NULL || l_CRoomList->m_User[i]->m_nGameJoin == 0 ) continue;

		if (*l_CRoomList->m_User[i] > *l_CRoomList->m_User[nMasterSlot])
			nMasterSlot = i;
	}

	if ( l_CRoomList->m_User[nMasterSlot] == NULL )
		return -1;

	l_CRoomList->m_User[nMasterSlot]->m_bRoomMaster = TRUE;
	
	return nMasterSlot;
}

void CPokerEngine::UserCardDeal( int nChannel, int nRoomNo )
{
	int nRoomMasterRoomInUserNo = 0;
	int nTemp;
	CRoomList *l_CRoomList;
	int i, j;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );


	for ( j = 0; j < MAX_ROOM_IN; j++ ) {

		nTemp = ( nRoomMasterRoomInUserNo + j ) % MAX_ROOM_IN;

		if ( l_CRoomList->m_User[nTemp] == NULL ) continue;
			 
		if ( l_CRoomList->m_User[nTemp]->m_nGameJoin == 1  ) {
			l_CRoomList->m_User[nTemp]->m_nUserCard[l_CRoomList->m_User[nTemp]->m_nCardCnt] = m_pCardPack->GetCard();

			//////////////////////////////////////////////////////////////////////////
			CardLog( l_CRoomList->m_User[nTemp]->m_nRoomNo, l_CRoomList->m_User[nTemp]->m_MyInfo.l_sUserInfo.szNick_Name,  l_CRoomList->m_User[nTemp]->m_nUserCard[l_CRoomList->m_User[nTemp]->m_nCardCnt] );

			
			memset( &l_CRoomList->m_User[nTemp]->m_sGameCurUser, 0x00, sizeof(l_CRoomList->m_User[nTemp]->m_sGameCurUser) );
			l_CRoomList->m_User[nTemp]->m_nBettingCnt = 0;
			l_CRoomList->m_User[nTemp]->m_nCardCnt++;
			l_CRoomList->m_User[nTemp]->m_biRoundMoney = 0;

			l_CRoomList->m_User[nTemp]->m_bCheck = FALSE;
		}
	}

	l_CRoomList->m_biBeforeMoney = 0;
	l_CRoomList->m_nRound++;
}

void CPokerEngine::PokerCalcuRaceMoney( int nChannel, int nRoomNo, int nSetUserArrayNo )
{
	CRoomList *l_CRoomList;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	BigInt biCallMoney = l_CRoomList->m_biBeforeMoney - l_CRoomList->m_User[nSetUserArrayNo]->m_biRoundMoney;

	l_CRoomList->m_User[nSetUserArrayNo]->m_biCallMoney = biCallMoney;

	BigInt biRecvMoney = l_CRoomList->m_biRoomCurMoney + biCallMoney;


	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biBing = l_CRoomList->l_sRoomList.l_sRoomInfo.biRoomMoney;
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biCall = biCallMoney;
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biCheck = 0;
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biDdaDang = biCallMoney * 2;
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biDie = 0;
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biFull = biCallMoney + biRecvMoney;
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biHalf = biCallMoney + ( biRecvMoney / 2 );
	l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biQuater = biCallMoney + ( biRecvMoney / 4 );

	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biBing > l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney ) {

		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biBing = l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	}

	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biCall > l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney ) {
	
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biCall = l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	}

	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biDdaDang > l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney ) {
		
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biDdaDang = l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	}

	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biFull > l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney ) {

		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biFull = l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	}

	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biHalf > l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney ) {
		
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biHalf = l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	}

	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biQuater > l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney ) {

		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biQuater = l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney;
	}


	if ( l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCheck ) {
		if ( l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney <= l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.biBing 
		 &&  l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney > 0 ) {
			PokerBtnReset( l_CRoomList->m_User[nSetUserArrayNo] );
			
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bBing = TRUE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCheck = TRUE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bDie = TRUE;		
		}
	}
	else {
		if ( ( l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney <= biCallMoney 
		 &&  l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney > 0 ) 
		 || biCallMoney < 0 ) 
		{ 
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bBing = FALSE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bDdaDang = FALSE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bFull = FALSE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bHalf = FALSE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bQuater = FALSE;				
		}
	}


}

void CPokerEngine::PokerBtnReset( CUserDescriptor *d )
{
	d->m_sGameCurUser.bBing = FALSE;
	d->m_sGameCurUser.bCall = FALSE;
	d->m_sGameCurUser.bCheck = FALSE;
	d->m_sGameCurUser.bDdaDang = FALSE;
	d->m_sGameCurUser.bDie = FALSE;
	d->m_sGameCurUser.bFull = FALSE;
	d->m_sGameCurUser.bHalf = FALSE;
	d->m_sGameCurUser.bQuater = FALSE;
}

BOOL CPokerEngine::UserMade( CUserDescriptor *d )
{
	d->m_CPokerEvalution.Calcul( d->m_nUserCard, d->m_nCardCnt );
	if ( d->m_CPokerEvalution.m_szJokboCard[0] >= POKER_STRAIGHT )
		return TRUE;

	return FALSE;
}

void CPokerEngine::LowRaceCalCu( BettingHistory aBettingHistory, CUserDescriptor *d )
{
	if ( aBettingHistory == FULL )
		d->SetBettingBtn( TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE );
	else if ( aBettingHistory == HALF )
		d->SetBettingBtn( TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE );
	else if ( aBettingHistory == QUATER || aBettingHistory == BBING || aBettingHistory == DDADANG )
		d->SetBettingBtn( TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE );
}

BOOL CPokerEngine::PokerBtnSet( int nChannel, int nRoomNo, int nSetUserArrayNo )
{
	int nRoomMasterRoomInUserNo = 0;
	CRoomList *l_CRoomList;
	BOOL bBetting = FALSE;
	BOOL bCheck = FALSE;
	int nViewUserArrayNo = 0;
	BOOL bMade = FALSE;
	BettingHistory l_BettingHistory;

	l_CRoomList = &g_World.m_Channel[nChannel].m_RoomList[nRoomNo];

	nRoomMasterRoomInUserNo = g_World.FindRoomMasterSlotNo( nChannel, nRoomNo );

	if ( nRoomMasterRoomInUserNo == nSetUserArrayNo && l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt == 0 ) {
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bBing = TRUE;
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCheck = TRUE;
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bQuater = TRUE;
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bDie = TRUE;
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bHalf = TRUE;
		l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bFull = TRUE;

		
		l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt++;



		if ( l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney <= 0 ) {
			PokerBtnReset(l_CRoomList->m_User[nSetUserArrayNo]);

			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCheck = TRUE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bDie = TRUE;
		}
		/*
		if ( l_CRoomList->m_nRound < 6 ) {
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bHalf = FALSE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bFull = FALSE;
		}
		*/

		return TRUE;
	}
	else {

		for ( int i = 1; i < MAX_ROOM_IN; i++ ) {

			nViewUserArrayNo = nSetUserArrayNo - i;

			if ( nViewUserArrayNo < 0 ) {
				nViewUserArrayNo = MAX_ROOM_IN + nViewUserArrayNo;
			}

			if ( l_CRoomList->m_User[nViewUserArrayNo] == NULL || 
				 l_CRoomList->m_User[nViewUserArrayNo]->m_nGameJoin == 0 ) continue;


			if ( l_CRoomList->m_User[nViewUserArrayNo]->m_sGameCurUser.bHalf ) {
				l_BettingHistory = HALF;
				bBetting = TRUE;
				break;
			}
			else if ( l_CRoomList->m_User[nViewUserArrayNo]->m_sGameCurUser.bBing ) {
				l_BettingHistory = BBING;
				bBetting = TRUE;
				break;
			}
			else if ( l_CRoomList->m_User[nViewUserArrayNo]->m_sGameCurUser.bFull ) {
				l_BettingHistory = FULL;
				bBetting = TRUE;
				break;
			}
			else if ( l_CRoomList->m_User[nViewUserArrayNo]->m_sGameCurUser.bQuater ) {
				l_BettingHistory = QUATER;
				bBetting = TRUE;
				break;
			}
			else if ( l_CRoomList->m_User[nViewUserArrayNo]->m_sGameCurUser.bCheck ) {
				l_BettingHistory = CHECK;
				bBetting = TRUE;
				bCheck = TRUE;
				break;
			}
			else if ( l_CRoomList->m_User[nViewUserArrayNo]->m_sGameCurUser.bDdaDang ) {
				
				l_BettingHistory = DDADANG;
				bBetting = TRUE;
			}
			
		}

		//앞사람이 베팅을 했을경우.
		if ( bBetting ) {

			if ( l_CRoomList->m_nRound <= 4 ) {

				if ( l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt <= 0  ) {
					if ( bCheck ) {
						l_CRoomList->m_User[nSetUserArrayNo]->SetBettingBtn( TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE );
					}
					else {
						LowRaceCalCu( l_BettingHistory, l_CRoomList->m_User[nSetUserArrayNo] );
					}
				}
				else {	
					l_CRoomList->m_User[nSetUserArrayNo]->SetBettingBtn( TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE );
				}
				l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt++;

			}
			else
			{
				int nBettingCnt = 0;

				nBettingCnt = 1;

				if ( l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt <= nBettingCnt  ) {
					if ( bCheck ) {
						l_CRoomList->m_User[nSetUserArrayNo]->SetBettingBtn( TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE );
					}
					else {
						if ( l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt == 1 &&
							 l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCall ) 
						{
							l_CRoomList->m_User[nSetUserArrayNo]->SetBettingBtn( TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE );		
						}
						else {
							LowRaceCalCu( l_BettingHistory, l_CRoomList->m_User[nSetUserArrayNo] );

						}
					}
				}
				else {	
					l_CRoomList->m_User[nSetUserArrayNo]->SetBettingBtn( TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE );
				}
				l_CRoomList->m_User[nSetUserArrayNo]->m_nBettingCnt++;
			}


			if ( l_CRoomList->m_User[nSetUserArrayNo]->m_MyInfo.l_sUserInfo.biUserMoney <= 0 ) {
				PokerBtnReset(l_CRoomList->m_User[nSetUserArrayNo]);

				l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCall = TRUE;
				l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bDie = TRUE;
			}		
/*
		if ( l_CRoomList->m_nRound < 6 ) {
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bHalf = FALSE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bFull = FALSE;			
		}
		*/


		if ( l_CRoomList->m_User[nSetUserArrayNo]->m_bCheck ) {
			PokerBtnReset(l_CRoomList->m_User[nSetUserArrayNo]);
			
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bCall = TRUE;
			l_CRoomList->m_User[nSetUserArrayNo]->m_sGameCurUser.bDie = TRUE;
		}



			return TRUE;
		}
		else {
			return FALSE;
		}

	}
}

