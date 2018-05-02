#include "stdafx.h"
#include "floor.h"
#include "dtrace.h"


extern int g_nRoomCurCnt;

Cfloor::Cfloor()
{
    //-----------------
    // 바닥패 위치 설정
    //-----------------

    m_pos[0]  = SetPoint( 251, 300 );
    m_pos[1]  = SetPoint( 454, 376 );
    m_pos[2]  = SetPoint( 464, 300 );
    m_pos[3]  = SetPoint( 261, 376 );
    m_pos[4]  = SetPoint( 320, 216 );
    m_pos[5]  = SetPoint( 395, 460 );
    m_pos[6]  = SetPoint( 395, 216 );
    m_pos[7]  = SetPoint( 320, 460 );
    m_pos[8]  = SetPoint( 245, 216 );
    m_pos[9]  = SetPoint( 470, 460 );
    m_pos[10] = SetPoint( 470, 216 );
    m_pos[11] = SetPoint( 245, 460 );
    m_pos[12] = SetPoint( 176, 338 );
    m_pos[13] = SetPoint( 539, 338 );
    m_pos[14] = SetPoint( 170, 241 );
    m_pos[15] = SetPoint( 545, 435 );
    m_pos[16] = SetPoint( 545, 241 );

	nTemp = 0;
	m_nBaseMulti = 1;
	m_nMasterSlotNo = MASTER_POS;
	m_nBbangJjangSlotNo = MASTER_POS;

	m_RealMode = false;

	Init();

}

// public
Cfloor::~Cfloor()
{
}

POINT Cfloor::SetPoint(LONG x, LONG y)
{
	POINT pt;

	pt.x = x;
	pt.y = y;

	return pt;
}


void Cfloor::Init()
{
	for (int i = 0 ; i < MAX_FLOOR_CARD ; i++ )
	{
		m_strCard[i].nCardNo = 0xff;
		m_strCard[i].nPosNo = 0xff;
	}

	for (i = 0 ; i < MAX_CARD_POS ; i++ )
		m_nPosCnt[i] = 0;	


	m_nCardCnt = 0;
	m_nWinerNo = -1;
	m_nPregidentNo = -1;
	m_nGetEmptyPos = 0;

	for ( i = 0; i < MAX_ROOM_IN; i++ ) 
		m_pGamer[i].SetFloor(this);


}

int Cfloor::GetEmptyPos()
{

	int nEmptyCnt = 0;
	int nEmptyPos = 0;
	int nEmptyTemp[MAX_CARD_POS];

	memset( nEmptyTemp, 0x00, sizeof(nEmptyTemp) );
	
	for ( int i = 0; i < MAX_CARD_POS - 1; i++ ) {
		if ( m_nPosCnt[i] <= 0 )
			return i;
	}

	return 0;

}

void Cfloor::OnlyAdd(const BYTE nCard)
{
	m_strCard[m_nCardCnt].nCardNo = nCard;			
	m_strCard[m_nCardCnt].nPosNo = GetEmptyPos();

	m_nPosCnt[m_strCard[m_nCardCnt].nPosNo]++;

	m_nCardCnt++;

}

int Cfloor::GetCurPosCnt(int nCardCnt)
{
	int nPos = m_strCard[nCardCnt].nPosNo;
	int cnt = 0;

	for (int i = 0 ; i <= nCardCnt ; ++i)
	{
		if (m_strCard[i].nPosNo == nPos)
			cnt++;
	}
	cnt--;
	return cnt;
}


int Cfloor::AddCardUser(int nUserNo, int nCardNo)
{
	int slotNo = (nUserNo + m_nMasterSlotNo) % MAX_ROOM_IN;

	m_pGamer[slotNo].RcvCard(nCardNo);

	return slotNo;
}

void Cfloor::SortUserCard(int nUserNo)
{
	int nSlot = (nUserNo + m_nMasterSlotNo) % MAX_ROOM_IN;
	m_pGamer[nSlot].SortUserCard();
}


bool Cfloor::IsFloorHas(int nCardNo)
{
	if ( nCardNo >= REAL_CARD_MAXNO )			// 48부터는 서비스 카드이기때문에..
		return true;
	for (int i = 0 ; i < m_nCardCnt ; ++i)
	{
		if (m_strCard[i].nCardNo == 0xff || m_strCard[i].nPosNo == 0xff)
			continue;
		
		if (m_strCard[i].nCardNo / 4 == nCardNo / 4)
			return true;
	}

	return false;
}


BYTE Cfloor::CheckNephew()
{
	for (int i = 0 ; i < m_nCardCnt ; ++i)
	{
		if (m_strCard[i].nCardNo < REAL_CARD_MAXNO)
			continue;

		return m_strCard[i].nPosNo;		
	}
	return 0xff;
}



int Cfloor::FloorEat(int nCardCnt, int nSlotNo)
{
	int nCardNo = m_strCard[nCardCnt].nCardNo;

	m_nPosCnt[m_strCard[nCardCnt].nPosNo]--;					//그위치에 카드수를 하나 줄여준다.

	m_pGamer[nSlotNo].EatCard(nCardNo);

	for (int i = nCardCnt ; i < m_nCardCnt -1 ; ++i)			//먹은 카드를 남은카드로 쭉 땡겨온다.
	{
		m_strCard[i].nCardNo = m_strCard[i+1].nCardNo;
		m_strCard[i].nPosNo = m_strCard[i+1].nPosNo;
	}

	m_nCardCnt--;

	m_strCard[m_nCardCnt].nCardNo = 0xff;
	m_strCard[m_nCardCnt].nPosNo = 0xff;
	
	return nCardNo;
}



int Cfloor::AppendAdd(int nPosNo, const BYTE nCard)
{
	m_strCard[m_nCardCnt].nCardNo = nCard;
	m_strCard[m_nCardCnt].nPosNo = nPosNo;
	++m_nPosCnt[m_strCard[m_nCardCnt].nPosNo];
	return m_strCard[m_nCardCnt++].nPosNo;
}

int	Cfloor::GetFIrstPosFromFloorPos(int nFloorPos)
{
	for (int i = 0 ; i < m_nCardCnt ; ++i)
	{
		if (m_strCard[i].nPosNo == nFloorPos)
			return i;
	}
	return 0xff;
}

int Cfloor::GetOneEachGamer( int nSlotNo, int nOtherSlotNo, int nGetOtherUserCardCnt )
{
	int nCardNo;

	nCardNo = m_pGamer[nOtherSlotNo].GetOnePee(nGetOtherUserCardCnt);
	
	if (nCardNo != 0xff && nCardNo >= 0)
		m_pGamer[nSlotNo].EatCard(nCardNo);

	return nCardNo;
}


void Cfloor::AddMyGamer( int nGamerUserNo, char *pPreImage )
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) { //방이 최초형성되므로 초기화해야한다. 혹시 몰라서.
		m_pGamer[i].m_bComputer = true;
		m_pGamer[i].m_nGamerUserNo = -1;
		m_pGamer[i].m_nUserNo = -1;

		memset( m_pGamer[i].m_szUserImage, 0x00, sizeof(m_pGamer[i].m_szUserImage) );
		memset( m_pGamer[i].m_szPreAvatar, 0x00, sizeof(m_pGamer[i].m_szPreAvatar) );

		m_pGamer[i].Init();
	}
			
	m_pGamer[0].m_nGamerUserNo = nGamerUserNo;
	m_pGamer[0].m_bComputer = false;
	m_pGamer[0].m_nUserNo = 0;

	memcpy( &m_pGamer[0].m_szPreAvatar, pPreImage, sizeof(m_pGamer[0].m_szPreAvatar) );

	m_nMasterSlotNo = 0;	
	m_nBbangJjangSlotNo = 0;

	m_pGamer[0].RoomInInit();


}

void Cfloor::AddRoomInGamer( int nSlot, char *pPreImage )
{
	//RoomInOutCnt(1);

	m_pGamer[nSlot].RoomInInit();
	m_pGamer[nSlot].Init();

	m_pGamer[nSlot].m_bComputer = false;
	m_pGamer[nSlot].m_nGamerUserNo = nSlot;

	memcpy( &m_pGamer[nSlot].m_szPreAvatar, pPreImage, sizeof(m_pGamer[nSlot].m_szPreAvatar) );


	return;
}



int Cfloor::AddOtherGamer( int nSlot,int nUserNo, char *pPreImage )
{
	int nRe = 0;

	memcpy( &m_pGamer[nSlot].m_szPreAvatar, pPreImage, sizeof(m_pGamer[nSlot].m_szPreAvatar) );
				
	m_pGamer[nSlot].m_nGamerUserNo = nSlot;
	m_pGamer[nSlot].m_nUserNo = nUserNo;
	m_pGamer[nSlot].RoomInInit();

	return nRe;


}


void Cfloor::UserRefresh( int nMasterSlot )
{
	int nTemp = 0;
	int nUserArrayNo = 0;
	
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {

		nUserArrayNo = ( nMasterSlot + i ) % MAX_ROOM_IN;
		m_pGamer[nUserArrayNo].m_nUserNo = nTemp;
		nTemp++;
	}
}



bool Cfloor::RealGame()
{
	return m_RealMode;
}

bool Cfloor::RoomUserCntOK()
{

	int nTemp = MAX_ROOM_IN;

	if( g_nRoomCurCnt >= nTemp  )
		return true;

	return false;

}

void Cfloor::UserMoneyVal( int nSlotNo,  BigInt biMoney, bool bPM )
{
	if ( !RealGame() ) return;

	m_pGamer[nSlotNo].UserMoneyVal( biMoney, bPM );
} 

void Cfloor::DeleteUser( int nSlotNo )
{
	//RoomInOutCnt(2); //방에 인원하나 감소.
	m_pGamer[nSlotNo].m_bComputer = true;
	m_pGamer[nSlotNo].m_nGamerUserNo = -1;
	m_pGamer[nSlotNo].m_biCurWinLoseMoney = 0;

	m_pGamer[nSlotNo].m_nUserNo = -1;
	memset( m_pGamer[nSlotNo].m_szUserImage, 0x00, sizeof(m_pGamer[nSlotNo].m_szUserImage) );
	
	m_pGamer[nSlotNo].m_bGodori = false;
	m_pGamer[nSlotNo].m_b3kwang = false;
	m_pGamer[nSlotNo].m_b4kwang = false;
	m_pGamer[nSlotNo].m_b5kwang = false;
	m_pGamer[nSlotNo].m_bHongdan = false;
	m_pGamer[nSlotNo].m_bChongdan = false;
	m_pGamer[nSlotNo].m_bGusa = false;
	m_pGamer[nSlotNo].m_bMissionEffect = false;

	m_pGamer[nSlotNo].m_ExitReserv = false;

	m_pGamer[nSlotNo].m_bGo		 = FALSE;
	m_pGamer[nSlotNo].m_bShake   = FALSE;
	m_pGamer[nSlotNo].m_bMmong   = FALSE;
	m_pGamer[nSlotNo].m_bMission = FALSE;
	m_pGamer[nSlotNo].m_bPbak     = FALSE; // 피박
	m_pGamer[nSlotNo].m_bKwangbak = FALSE; // 광박
	m_pGamer[nSlotNo].m_bDokbak   = FALSE; // 독박	
}

int Cfloor::FindUser( int nGamerUserNo )
{
	for ( int i = 0; i < MAX_ROOM_IN; i++ ) {
		if ( m_pGamer[i].m_nGamerUserNo == nGamerUserNo )
			return i;
	}
	return -1;
}
