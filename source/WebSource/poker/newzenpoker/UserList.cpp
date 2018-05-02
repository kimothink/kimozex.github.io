
#include "StdAfx.h"
#include "UserList.h"
#include "Externvar.h"
#include "dtrace.h"


CUserInfo::CUserInfo()
{
	Init();
}

CUserInfo::~CUserInfo()
{
}

CUserInfo::Init()
{
	m_bUser = false;
	memset( &m_sUserInfo, 0x00, sizeof(m_sUserInfo) );
}









//////////////////////////////////////////////////////////////////////////


CUserList::CUserList()
{
	m_nCurMax = 0;
}

CUserList::~CUserList()
{

}


int CUserList::AddUser( sUserInfo *pUserInfo )
{
	int i;
	
	for ( i = 0; i < m_nCurMax; i++ ) {	//이미 들어가 있는 사람이 있으면.. 그 번호에 새로 복사한다. 왜냐하면 그사람의 정보가 바뀌었을수 있기 때문에...서버에서 받은정보가 항상 최신이다.
		if ( m_WaitUserInfo[i].m_sUserInfo.nUserNo == pUserInfo->nUserNo  )  {
			m_WaitUserInfo[i].m_bUser = true;
			memcpy( &m_WaitUserInfo[i].m_sUserInfo, pUserInfo, sizeof(sUserInfo) );
			return i;

		}
	}

	
	for ( i = 0; i < MAX_CHANNELPER; i++ ) {
		if ( m_WaitUserInfo[i].m_bUser == false ) {
			
			m_WaitUserInfo[i].m_bUser = true;
			memcpy( &m_WaitUserInfo[i].m_sUserInfo, pUserInfo, sizeof(sUserInfo) );

			if ( i == m_nCurMax )
				m_nCurMax++;
			
			return i;
		}
	}

	return -1;


}


void CUserList::DeleteUser( int nUniqueUserNo )
{
	int nDeleteUserNo = FindUser(nUniqueUserNo);

	if ( nDeleteUserNo < 0 ) return;

	m_WaitUserInfo[nDeleteUserNo].Init();

	if ( nUniqueUserNo == m_nCurMax )
		m_nCurMax--;	
}

int CUserList::FindUser( int nUniqueUserNo )
{
	for ( int i = 0; i < m_nCurMax; i++) {
		
		if ( m_WaitUserInfo[i].m_sUserInfo.nUserNo == nUniqueUserNo )
			return i;
	}

	return -1;

}







//////////////////////////////////////////////////////////////////////////
CGameUserInfo::CGameUserInfo()
{
	Init();
}

CGameUserInfo::~CGameUserInfo()
{
}

CGameUserInfo::Init()
{
	m_nUserSelectCnt = 0;
	m_cUserSelct = 0x00;
	m_nCardCnt = 0;
	m_biRealUserMoney = 0;
	m_biPureUserWinLoseMoney = 0;
	
	m_bUser = false;
	//m_bBBangJjang = false;	//방장
	m_bSun = false;	//선
	m_bChoioce = false;
	m_bEnd = false;
	m_bUserSelectShow = false;
	m_bDie = false;
	m_bAllin = false;
	m_bExitReserve = false;
	
	memset( m_szTempJokboCard, 0x00, sizeof(m_szTempJokboCard) );
	memset( m_szJokboCard,     0xFF, sizeof(m_szJokboCard) );
	memset( m_szFixJokboCard,  0x00, sizeof(m_szFixJokboCard) );


}

CGameUserInfo::UserInfoInit()
{
	memset( &m_sUserInfo, 0x00, sizeof(m_sUserInfo) );
}

CGameUserInfo::InitCard() 
{
	for(int i = 0; i < MAX_CARD; i++)
	{
		m_sUserCardInfo[i].NewPos = g_pCMainFrame->SetPoint(INIT_CARD_POS_X, INIT_CARD_POS_Y);
		m_sUserCardInfo[i].StartPos = g_pCMainFrame->SetPoint(INIT_CARD_POS_X, INIT_CARD_POS_Y);
		m_sUserCardInfo[i].Pos = g_pCMainFrame->SetPoint(INIT_CARD_POS_X, INIT_CARD_POS_Y);
		m_sUserCardInfo[i].nAniCount = 0;
		m_sUserCardInfo[i].nAniMaxCount = -1;
		m_sUserCardInfo[i].nCardNo = -1;
		m_sUserCardInfo[i].ctCardType = PACK;
		m_byUserRealCardNo[i] = -1;
	}
}





//////////////////////////////////////////////////////////////////////////


CGameUserList::CGameUserList()
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {	
		m_GameUserInfo[i].Init();
		m_GameUserInfo[i].InitCard();
		m_GameUserInfo[i].UserInfoInit();
	}

}

CGameUserList::~CGameUserList()
{

}


void CGameUserList::UserAllClear()
{
	//유저 정보들 초기화.
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( i != 0 )
			m_GameUserInfo[i].UserInfoInit();

		m_GameUserInfo[i].Init();
		m_GameUserInfo[i].InitCard();
	}
}

int CGameUserList::AddUser( sUserInfo *pUserInfo, int nSlotNo )
{
	int i;

	if ( nSlotNo < 0 ) {

		for ( i = 0; i < MAX_ROOM_IN; i++ ) {	
			if ( m_GameUserInfo[i].m_bUser == false ) {
				memcpy( &m_GameUserInfo[i].m_sUserInfo, pUserInfo, sizeof(sUserInfo)  );
				m_GameUserInfo[i].m_bUser = true;
				//m_GameUserInfo[i].m_nGameJoin = nGameJoin;
				return i;
			}
		}
	}
	else {
		memcpy( &m_GameUserInfo[nSlotNo].m_sUserInfo, pUserInfo, sizeof(sUserInfo)  );
		m_GameUserInfo[nSlotNo].m_bUser = true;
		//m_GameUserInfo[nSlotNo].m_nGameJoin = nGameJoin;
		return nSlotNo;
	}


	return -1;
}

int CGameUserList::AddRoomInOtherUser( sUserInfo *pUserInfo, int nNextNo )
{
	int nBangJangArrayNo = 0;
	int nSetArrayNo = 0;
	
	for ( int k = 0; k < MAX_ROOM_IN; k++ ) {
		DTRACE("슬롯 : %d, 닉넴 : %s", k, m_GameUserInfo[k].m_sUserInfo.szNick_Name );
	}



	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_GameUserInfo[i].m_bBBangJjang == TRUE ) {
			nBangJangArrayNo = i;
			DTRACE("방장 %s", m_GameUserInfo[i].m_sUserInfo.szNick_Name);
			break;
		}
	}

	//방장 다음을 찾는다.
	nSetArrayNo = ( nBangJangArrayNo + nNextNo ) % MAX_ROOM_IN;
	memcpy( &m_GameUserInfo[nSetArrayNo].m_sUserInfo, pUserInfo, sizeof(sUserInfo)  );
	m_GameUserInfo[nSetArrayNo].m_bUser = true;
	//m_GameUserInfo[nSetArrayNo].m_nGameJoin = nGameJoin;

	return nSetArrayNo;


}

void CGameUserList::DeleteUser( int nUniqueUserNo )
{
	int i;

	for ( i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_GameUserInfo[i].m_sUserInfo.nUserNo == nUniqueUserNo ) {
			m_GameUserInfo[i].Init();
			m_GameUserInfo[i].UserInfoInit();
			m_GameUserInfo[i].InitCard();

		}

	}
}

int CGameUserList::FindUser( int nUniqueUserNo )
{
	for ( int i = 0; i < MAX_ROOM_IN; i++) {
		if ( m_GameUserInfo[i].m_sUserInfo.nUserNo == nUniqueUserNo )
			return i;
	}
	return -1;
}

void CGameUserList::BangJangDel()
{
	DTRACE("방장이 지워졌다");
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_GameUserInfo[i].m_bBBangJjang = FALSE;
	}
}

void CGameUserList::SunDel()
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		m_GameUserInfo[i].m_bSun = FALSE;
	}
}


void CGameUserList::NewBangJangSet( int nUniqueUserNo )
{

	BangJangDel();

	int nUsetArrayNo = FindUser(nUniqueUserNo);
	DTRACE("NewBangJangSet - FindArray = %d, nUniqueUserNo = %d", nUsetArrayNo, nUniqueUserNo);
	m_GameUserInfo[nUsetArrayNo].m_bBBangJjang = TRUE;

}

void CGameUserList::NewSunSet( int nUniqueUserNo )
{

	SunDel();

	int nUsetArrayNo = FindUser(nUniqueUserNo);
	m_GameUserInfo[nUsetArrayNo].m_bSun = TRUE;

}

void CGameUserList::UserCardDump( int nUserArrayNo, int nCardArrayNo )
{
	for ( int i = nCardArrayNo; i < m_GameUserInfo[nUserArrayNo].m_nCardCnt; i++ ) {
		m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[i].nCardNo = m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[i + 1].nCardNo;
	}

	int nTemp = m_GameUserInfo[nUserArrayNo].m_nCardCnt - 1;

	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].NewPos = g_pCMainFrame->SetPoint(INIT_CARD_POS_X, INIT_CARD_POS_Y);
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].StartPos = g_pCMainFrame->SetPoint(INIT_CARD_POS_X, INIT_CARD_POS_Y);
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].Pos = g_pCMainFrame->SetPoint(INIT_CARD_POS_X, INIT_CARD_POS_Y);
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].nAniCount = 0;
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].nAniMaxCount = -1;
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].nCardNo = -1;
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nTemp].ctCardType = PACK;
	m_GameUserInfo[nUserArrayNo].m_byUserRealCardNo[nTemp] = -1;
		
	m_GameUserInfo[nUserArrayNo].m_nCardCnt--;
	
}

void CGameUserList::UserCardChoice( int nUserArrayNo, int nCardArrayNo )
{
	int nTempCard;

	//3번째카드 번호를 담아놓는다.
	nTempCard = m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[2].nCardNo;

	//3번째에다가 카드를 넣는다.
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[2].nCardNo = m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nCardArrayNo].nCardNo;

	//임시로 받은 3번째 카드를 선택한 위치로 넣는다.
	m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[nCardArrayNo].nCardNo = nTempCard;
}



void CGameUserList::UserCardPosRefresh( int nUserArrayNo )
{
	for ( int i = 0; i < m_GameUserInfo[nUserArrayNo].m_nCardCnt; i++ ) {
		m_GameUserInfo[nUserArrayNo].m_sUserCardInfo[i].Pos.x = g_pCMainFrame->m_sRoomInUserPosition[nUserArrayNo].pCardStartPos.x + i * CARD_DISTANCE_B; 
	}
}

//////////////////////////////////////////////////////////////////////////


CWaitRoomList::CWaitRoomList()
{
	Init();

}

CWaitRoomList::~CWaitRoomList()
{

}

void CWaitRoomList::Init()
{
	memset( m_bRoom, 0x00, sizeof(m_bRoom) );
	
	for ( int i = 0; i < MAX_ROOMCNT; i++ ) {
		m_bRoom[i] = false;
		m_sWaitRoomList[i].bSecret = false;
		m_sWaitRoomList[i].nGameDivi = 0;
		m_sWaitRoomList[i].nCurCnt = 0;
		m_sWaitRoomList[i].biRoomMoney = 0;
		m_sWaitRoomList[i].nRoomNo = -1;
		m_sWaitRoomList[i].nTotCnt = 0;
		memset( m_sWaitRoomList[i].szRoomName, 0x00, sizeof(m_sWaitRoomList[i].szRoomName) );
	}
}

int CWaitRoomList::AddRoom( sRoomInfo *psRoomInfo )
{
	//방번호가 10번이 넘어가면 배열과 맞지 않기때문에..
	int nArrayNo = psRoomInfo->nRoomNo;

	memcpy( &m_sWaitRoomList[nArrayNo], psRoomInfo, sizeof(sRoomInfo)  );
	m_bRoom[nArrayNo] = true;





	return nArrayNo;
}


void CWaitRoomList::DeleteRoom( int nRoomNo )
{
	//방번호가 10번이 넘어가면 배열과 맞지 않기때문에..
	int nArrayNo = nRoomNo;
	
	m_bRoom[nArrayNo] = false;
	m_sWaitRoomList[nArrayNo].bSecret = false;
	m_sWaitRoomList[nArrayNo].nGameDivi = 0;
	m_sWaitRoomList[nArrayNo].nCurCnt = 0;
	m_sWaitRoomList[nArrayNo].biRoomMoney = 0;
	m_sWaitRoomList[nArrayNo].nRoomNo = -1;
	m_sWaitRoomList[nArrayNo].nTotCnt = 0;
	memset( m_sWaitRoomList[nArrayNo].szRoomName, 0x00, sizeof(m_sWaitRoomList[nArrayNo].szRoomName) );



}

void CWaitRoomList::AddUser( int nRoomNo, int nRoomCnt )
{
	m_sWaitRoomList[nRoomNo].nCurCnt = nRoomCnt;
	/*
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( m_sWaitRoomList[i].nRoomNo == nRoomNo ) {

			m_sWaitRoomList[i].nCurCnt = nRoomCnt;
		}
	}
	*/
}

void CWaitRoomList::DeleteUser( int nRoomNo, int nRoomCnt )
{
	m_sWaitRoomList[nRoomNo].nCurCnt = nRoomCnt;
	/*
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		if ( m_sWaitRoomList[i].nRoomNo == nRoomNo ) {

			m_sWaitRoomList[i].nCurCnt = nRoomCnt;
		}
	}
	*/


}




