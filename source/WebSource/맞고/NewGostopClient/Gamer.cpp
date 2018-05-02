#include "stdafx.h"
#include "Gamer.h"
#include "floor.h"
#include "dtrace.h"

#include "GoStopEngine.h"
#include "MainFrm.h"
#include <math.h>
#include "UserList.h"

extern CGameUserList g_CGameUserList;
extern CGoStopEngine *g_pGoStop;
extern int g_CurUser;

extern CMainFrame *g_pCMainFrame;
extern sMissionCardInfo g_sMissionCardInfo;



int GetPeeVal(BYTE nCard)
{
	if (g_pGoStopValue[nCard] == H_3)
		return 3;
	else if (g_pGoStopValue[nCard] == H_2)
		return 2;
	else if ( nCard == 33 )
		return 2;

	return 1;
}


CGamer::CGamer()
{
	m_bComputer = false;
	m_nGamerUserNo = -1;
	m_biCurWinLoseMoney = 0;

	m_nUserNo = -1;

	m_ExitReserv = false;

	memset( m_szUserImage, 0x00, sizeof(m_szUserImage) );
	memset( m_szPreAvatar, 0x00, sizeof(m_szPreAvatar) );

	Init();
}



CGamer::~CGamer()
{
}



void CGamer::RcvCard(const BYTE nCard)
{
	m_bCard[m_nCardCnt] = nCard;
	m_nCardCnt++;
}



void CGamer::Init()
{
	for (int i = 0 ; i < MAX_USERCARD_CNT ; ++i)
		m_bCard[i] = 0xff;

	m_nCardCnt = 0;
	memset( m_nEatCnt, 0x00, sizeof(m_nEatCnt) );
	memset( m_nEatCard, 0xff, sizeof(m_nEatCard) );

	m_nChoiceUserCardPos = 0xff;
	m_nChoiceFloorCardPos = 0x10;
	m_nEvalution = 0;
	m_nResultScore = 0;

	for ( i = 0; i < 5; i++ )
		m_arrCard[i] = 0xff;

	m_nUserThreeState = NOTICE_NONE;
	m_nShakeCnt = 0;
	m_nOldShakeCnt = 0;
	m_nMulti = 1;
	m_nBbukCnt = 0;
	m_nGoCnt = 0;
	m_bUserEvent = false;
	m_bNineCard = false;
	m_nMinWinVal = GOSTOP_ENDPOINT;
	m_nBombFlipCnt = 0; 
	m_GetOtherUserCardCnt = 0;
	m_nLoseEval = 0;
	m_biWinMoney = 0;
	m_biLoseMoney = 0;
	m_nMissionMultiply = 0;	
	
	m_bGo		 = FALSE;
	m_bShake   = FALSE;
	m_bMmong   = FALSE;
	m_bMission = FALSE;

	m_bPbak     = FALSE; // �ǹ�
	m_bKwangbak = FALSE; // ����
	m_bDokbak   = FALSE; // ����
	
	m_bGodori = false;
	m_b3kwang = false;
	m_b4kwang = false;
	m_b5kwang = false;
	m_bHongdan = false;
	m_bChongdan = false;
	m_bGusa = false;
	m_bMissionEffect = false;
	m_bAlly = false;

	memset( m_nBbukCardNo, -1, sizeof(m_nBbukCardNo) );

}

void CGamer::RoomInInit()	//������ �濡 ��������..�ʱ�ȭ �Ұ���...���嵵 ��������.
{
	m_biCurWinLoseMoney = 0;
}

void CGamer::SortUserCard()
{

	
	int pos;
	BYTE tmp;
	for (int i = 0 ; i < m_nCardCnt ; ++i)
	{
		if (m_bCard[i] == 0xff)
		{
			m_bCard[i] = m_bCard[m_nCardCnt-1];
			m_bCard[m_nCardCnt-1] = 0xff;
			m_nCardCnt--;
			break;
		}
	}

	for (i = 0 ; i < m_nCardCnt - 1 ; ++i)
	{
		pos = i;
		for (int j = i+1 ; j < m_nCardCnt ; ++j)
		{
			if (m_bCard[pos] > m_bCard[j])
				pos = j;
		}
		if (pos != i)
		{
			tmp = m_bCard[pos];
			m_bCard[pos] = m_bCard[i];
			m_bCard[i] = tmp;
		}
	}

}

void CGamer::EatCard(const BYTE nCard)
{
    //-------------------------------------------
    // �̼��� �߻��ϱ� ���� ���� ī�带 üũ�Ѵ�.
    //-------------------------------------------
    for ( int i = 0; i < g_sMissionCardInfo.nCardCnt; i++ ) {

        if ( g_sMissionCardInfo.bEat[i] ) continue;

        if ( g_sMissionCardInfo.nCardNo[i] == nCard ) {
            g_sMissionCardInfo.nUserNo[i] = m_nUserNo;
            g_sMissionCardInfo.nCardNo[i] = g_sMissionCardInfo.nCardNo[i] + 52;     // ��ο� ī��� ����
            g_sMissionCardInfo.bEat[i] = TRUE;
            break;
        }
    }

	int nCardKind = 3;
	if (g_pGoStopValue[nCard] & H_20)
		nCardKind = 0;
	else if (g_pGoStopValue[nCard] & H_10)
	{
		if ( nCard == 33 )
			m_bNineCard = true;

		nCardKind = 1;
	}
	else if (g_pGoStopValue[nCard] & H_5)
		nCardKind = 2;
	else
	{
		nCardKind = 3;
		m_nEatCard[3][m_nEatCnt[3]] = nCard;
		m_nEatCnt[3]++;
		m_nEatCnt[4] += ::GetPeeVal(nCard);

		return;
	}

	m_nEatCard[nCardKind][m_nEatCnt[nCardKind]] = nCard;
	m_nEatCnt[nCardKind]++;
}



int CGamer::IsPregident()
{
	int nSum[13];
	memset(nSum, 0, sizeof(nSum));
	
	for (int i = 0 ; i < m_nCardCnt ; ++i)
	{
		if (m_bCard[i] == 0xff)
			continue;
		
		nSum[m_bCard[i] / 4] ++;
	}

	for (i= 0 ; i < 12 ; ++i)
	{
		if (nSum[i] == 4)
		{
			return i * 4;
		}
	}
	
	return -1;
}

void CGamer::AutoSelect()	// ī�� ��ġ �ڵ� ����
{
	int nUserCardPos = 0xff;
	int nFloorCardPos = 0xff;
	
	int cnt = 0;					// �ڽ��� ������ �ִ� ī�尡 3���� �ִ��� üũ�Ѵ�.
	BYTE arrCard[5];				// �� 3���� ������ ���� ��.



	for ( int i = m_nCardCnt - 1 ; i >= 0 ; --i )
	{
		if ( m_bCard[i] == 0xff )
			continue;

		for ( int j = 0 ; j < m_pfloor->m_nCardCnt ; ++j )
		{
			if ( m_bCard[i] / 4 == m_pfloor->m_strCard[j].nCardNo / 4 )			//���� �� ������..
			{
				nUserCardPos = i;
				nFloorCardPos = m_pfloor->m_strCard[j].nPosNo;
				break;
			}
		}
		if ( nUserCardPos != 0xff )
			break;
	}
	
	if ( nUserCardPos == 0xff )													//���� �� ������..
	{
		nUserCardPos = m_nCardCnt - 1;											//�� ������ ī�带 �����ϰ�.
		
		for ( i = m_nCardCnt - 1 ; i >= 0 ; --i )
		{
			if ( m_bCard[i] == 0xff )
				continue;

			if ( m_bCard[i] >= 48 && m_bCard[i] < BOMB_CARDNO )								//���� ī�尡 ������ �̸� �����Ѵ�.
			{
				nUserCardPos = i;
				break;
			}

			if ( m_bCard[nUserCardPos] == BOMB_CARDNO && m_nCardCnt>= 3  ) 
				nUserCardPos = i;

		}
	}

	m_nChoiceUserCardPos = nUserCardPos;
	m_nChoiceFloorCardPos = nFloorCardPos;


	for ( i = 0 ; i < m_nCardCnt ; ++i ) {
		if ( m_bCard[i] / 4 == m_bCard[m_nChoiceUserCardPos] / 4)
			arrCard[cnt++] = m_bCard[i];
	}

	if ( cnt >= 3 && m_bCard[m_nChoiceUserCardPos] < 48 ) {

		if ( nFloorCardPos == 0xff )				//������ ������ ����̴�.
			m_nUserThreeState = NOTICE_THREE;
		else 
			m_nUserThreeState = NOTICE_BOOM;

	}
}

void CGamer::SetFloor( Cfloor *pfloor )
{
	m_pfloor = pfloor;	
}

void CGamer::UserMoneyVal( BigInt biMoney, bool bPM )
{
	CString strMent;
	
//	AfxMessageBox("UserMoneyVal");
    strMent.Format("Game Result : %d \n",biMoney);
	TRACE("%s\n",strMent);

	BigInt biUserMoney = 0;

	
	if ( m_nGamerUserNo >= 0 && m_nGamerUserNo < MAX_ROOM_IN )
		biUserMoney = g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.biUserMoney;
	else 
		return;

	if ( bPM == false ) {
		strMent.Format("Lose The Game : %d - %d\n",m_nGamerUserNo,biMoney);
		TRACE("%s\n",strMent);
		
		biUserMoney -= biMoney;

		g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_biLoseMoney += biMoney;

        if ( biUserMoney <= 0 ) 
			biUserMoney = 0;
	}
	else if ( bPM == true ) {
		strMent.Format("Lose The Game : %d - %d\n",m_nGamerUserNo,biMoney);
		TRACE("%s\n",strMent);

		g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_biObtainMoney += biMoney;
		biUserMoney += biMoney;
	}
	
	g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.biUserMoney = biUserMoney;
	strMent.Format("Lose The Game : %d - %d\n",m_nGamerUserNo,biUserMoney);

	TRACE("%s\n",strMent);
}

int CGamer::GetOnePee(int nGetOtherUserCardCnt)
{
	if (m_nEatCnt[3] < 1)	// �Ǹ������� �ϳ��� ������..
		return 0xff;

	int selPos = -1;

	if ( nGetOtherUserCardCnt == 1) { // ���Ǻ��� ã�´�. �״��� ����, ������....

		for ( int i = m_nEatCnt[3] - 1 ; i >= 0 && selPos == -1 ; --i )
		{
			if ( g_pGoStopValue[m_nEatCard[3][i]] == H_1 )	{//����(���ǹݴ�)�� ������
				selPos = i;
				break;
			}
		}

		if ( selPos == -1 ) {		//���ǰ� ������ ������ ��. �����ǿ� ���ǰ� ���� ������ �𸣱⶧����..���Ǹ� ����.
			for ( int i = m_nEatCnt[3] - 1 ; i >= 0 && selPos == -1 ; --i )
			{
				if ( g_pGoStopValue[m_nEatCard[3][i]] == H_2 ) {	//���ǰ� ������
					selPos = i;
					break;
				}
			}
		}
	}
	else if ( nGetOtherUserCardCnt == 2) { //���Ǻ��� ã�´�...�״��� ���ǰ� ������ ���ǰ� �����̸� ����. ���ǰ� �����̸� ������ 

		for ( int i = m_nEatCnt[3] - 1 ; i >= 0 && selPos == -1 ; --i )
		{
			if ( g_pGoStopValue[m_nEatCard[3][i]] == H_2 ) {	//���ǰ� ������
				selPos = i;
				break;
			}
		}

		if ( selPos == -1 ) { // ���ǰ� �����ΰ� ã�´�.
		
			int nH_1Cnt = 0;
			int nTempPos = -1;

			for ( int i = m_nEatCnt[3] - 1 ; i >= 0 && selPos == -1 ; --i )
			{
				if ( g_pGoStopValue[m_nEatCard[3][i]] == H_1 )	//����(���ǹݴ�)�� ������
					nH_1Cnt++;
				
				if ( g_pGoStopValue[m_nEatCard[3][i]] == H_3 )
					nTempPos = i;

				if ( nH_1Cnt >=2 ) { //���ǰ� �����̸�
					selPos = i;
					break;					
				}
			}

			if ( nH_1Cnt < 2 ) // ���ǰ� 2���̻��� �ƴϸ� ������ ������ �����Ǹ� ��´�.
				selPos = nTempPos;
		}
	}
	else if ( nGetOtherUserCardCnt == 3 ) { //�����ǰ� ������ ã�´�....�״��� ���Ǹ� ã�´�...�״���..���Ǹ� ã�´�..

		for ( int i = m_nEatCnt[3] - 1 ; i >= 0 && selPos == -1 ; --i )
		{
			if ( g_pGoStopValue[m_nEatCard[3][i]] == H_3 ) {	//�����ǰ� ������
				selPos = i;
				break;
			}
		}		

		if ( selPos == -1 ) { // ���� ã�´�.

			for ( int i = m_nEatCnt[3] - 1 ; i >= 0 && selPos == -1 ; --i )
			{
				if ( g_pGoStopValue[m_nEatCard[3][i]] == H_2 ) {	//���ǰ� ������
					selPos = i;
					break;
				}
			}	
		}
	}


	if ( selPos == -1 )	//�׷��� ������ 
		selPos = m_nEatCnt[3] - 1;

	int nCardNo = m_nEatCard[3][selPos];	

	for ( ; selPos < m_nEatCnt[3] - 1; ++selPos )		//���� ī�带 �޿� �ٽ� �����Ѵ�.
		m_nEatCard[3][selPos] = m_nEatCard[3][selPos+1];

	m_nEatCard[3][selPos] = 0xff;		//���� for������ selpos���� �߱� ������. �������κ��� oxff
	m_nEatCnt[3]--;						//������.����

	if (g_pGoStopValue[nCardNo] == H_3 )
		m_nEatCnt[4] -= 3;
	else if (g_pGoStopValue[nCardNo] == H_2 )
		m_nEatCnt[4] -= 2;
	else
		m_nEatCnt[4] -= 1;


	return nCardNo;

}


bool CGamer::Evalutaion()
{
	int nSlotNo;
	
	if ( !g_pGoStop->m_pFloor.RealGame() )
		nSlotNo = g_CurUser;
	else
		nSlotNo = m_nGamerUserNo;

	int nTempGoGob = 0;
	int tmpMul = 1;
	int i = 0;

	m_nEvalution = 0;
	m_nMulti	 = 1;

	m_kwang = 0;
 
	//�ǰ� 8���� �Ǹ� ���Ǹ� �Ƿ� ����Ұ������� �����Ѵ�.
	if ( m_bNineCard && m_nEatCnt[4] >= 8 ) {
		m_bNineCard = false;
		return true;
	}

	//��
	if ( m_nEatCnt[0] == 5 ) {
		if ( !m_b5kwang ) {
			g_pGoStop->EventSync( nSlotNo, 22, GS_5KWNAG );
			m_b5kwang = true;
		}
		m_kwang += 15;
	}
	else if ( m_nEatCnt[0] >= 3 ) //�ﱤ
	{
		m_kwang += m_nEatCnt[0];
		 
		if ( m_nEatCnt[0] == 3 )	//��ﱤ üũ
		{
			if (!m_b3kwang ) {
				g_pGoStop->EventSync( nSlotNo, 25, GS_3KWNAG );

				m_b3kwang = true;
			}

			for ( i = 0 ; i < m_nEatCnt[0] ; ++i )
				if ( m_nEatCard[0][i] == 44 ) {	//��.
					--m_kwang; 
					break;	
				}
		}
		else if ( m_nEatCnt[0] == 4 )  { //�籤�Ҹ�.

			if (!m_b4kwang ) {

				g_pGoStop->EventSync( nSlotNo, 26, GS_4KWNAG );

				m_b4kwang = true;
			}
		}
	}

	if (m_kwang > 0)
	{
		m_nEvalution += m_kwang;
	}

	if ( CheckCardIn(1, 5, 13, 29) )	//10��¥���߿� ������ �ִ���.
	{
		m_nEvalution += 5;

		if ( !m_bGodori ) {
			
			g_pGoStop->EventSync( nSlotNo, 13, GS_GODORI );

			m_bGodori = true;

			if ( m_nGamerUserNo < 0 )
			{
				g_pCMainFrame->PutTextInGame( "Com1" , "����" , 
					0 , RGB( 255 , 255 , 165 ) , FALSE );			
			}
			else
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.szNick_Name , "����" , 0 , RGB( 255 , 255 , 165 ) , FALSE );

		}


	}

	if (m_nEatCnt[1] >= 5)	//10��
	{
		m_nEvalution += (m_nEatCnt[1] - 4);
	}

	if (CheckCardIn(2, 1, 4, 9))	//ȫ��
	{
		m_nEvalution += 3;

		if ( !m_bHongdan ) {
			
			g_pGoStop->EventSync( nSlotNo, 12, GS_HONGDAN );
			m_bHongdan = true;


			if ( m_nGamerUserNo < 0 )
			{		
				g_pCMainFrame->PutTextInGame( "Com1" , "ȫ��" , 
				0 , RGB( 255 , 255 , 165 ) , FALSE );			
			}
			else
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.szNick_Name , "ȫ��" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
		}
	}

	if (CheckCardIn(2, 20, 32, 36))
	{
		m_nEvalution += 3;
		if ( !m_bChongdan ) {
			
			g_pGoStop->EventSync( nSlotNo, 11, GS_CHUNGDAN );
			m_bChongdan = true;

			if ( m_nGamerUserNo < 0 )
			{		
				g_pCMainFrame->PutTextInGame( "Com1" , "û��" , 
				0 , RGB( 255 , 255 , 165 ) , FALSE );		
			}
			else
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.szNick_Name , "û��" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
		}


	}
	if (CheckCardIn(2, 12, 16, 24))
	{

		m_nEvalution += 3;

		if ( !m_bGusa ) {
			
			g_pGoStop->EventSync( nSlotNo, 10, GS_CHODAN );
			m_bGusa = true;

			if ( m_nGamerUserNo < 0 )
			{		
				g_pCMainFrame->PutTextInGame( "Com1" , "�ʴ�" , 
					0 , RGB( 255 , 255 , 165 ) , FALSE );		
			}
			else
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.szNick_Name , "�ʴ�" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
		}

		
	}

	if ( !m_bMission ) {
		m_bMission = CheckMission( g_pGoStop->m_nMissionType );
		m_bMissionEffect = false;
	}


	if ( m_bMission ) {
		if ( !m_bMissionEffect ) {
			g_pCMainFrame->PlaySound(g_Sound[GS_MISSION_COMPLETE], false);
			m_bMissionEffect = true;
		}
	}

	//�α׸� ���� ��Ƴ��´�.
	m_nMissionMultiply = g_pGoStop->m_nMissionMultiply;

	if (m_nEatCnt[2] >= 5)
	{
		m_nEvalution += (m_nEatCnt[2] - 4);
	}

	if (m_nEatCnt[4] >= 10)
	{
		m_nEvalution += (m_nEatCnt[4] - 9);
	}

	if ( m_nGoCnt > 0 )
	{
		m_bGo = TRUE;

		m_nEvalution += m_nGoCnt;

	}

	if ( m_nEatCnt[1] >= 7 )
	{
		m_bMmong = TRUE;

		m_nMulti *= 2;
	}

	m_nMulti *= (int)pow(2, m_nShakeCnt);

	if( m_nShakeCnt >= 1 )
	{
		m_bShake = TRUE;
		
		if ( m_nShakeCnt != m_nOldShakeCnt ) { 
			if ( m_nGamerUserNo < 0 )
			{	
				g_pCMainFrame->PutTextInGame( "Com1" , "���" , 
				   0 , RGB( 255 , 255 , 165 ) , FALSE );	
			}
			else
				g_pCMainFrame->PutTextInGame( g_CGameUserList.m_GameUserInfo[m_nGamerUserNo].m_sUserInfo.szNick_Name , "���" , 0 , RGB( 255 , 255 , 165 ) , FALSE );
		}

		m_nOldShakeCnt = m_nShakeCnt;
	}


	if (m_nGoCnt >= 3)
	{
		int tmpMul = 1;
		tmpMul *= (int)pow(2, m_nGoCnt - 2);
		m_nMulti *= tmpMul;
	}


	m_nResultScore = m_nEvalution;


	return false;
}


bool CGamer::SendPeeEvalutaion()
{
	int nSlotNo;
	
	if ( !g_pGoStop->m_pFloor.RealGame() )
		nSlotNo = g_CurUser;
	else
		nSlotNo = m_nGamerUserNo;
	
	int nTempGoGob = 0;
	int tmpMul = 1;
	int i = 0;

	m_nEvalution = 0;
	m_nMulti	 = 1;

	m_kwang = 0;

	//��
	if ( m_nEatCnt[0] == 5 ) {
		m_kwang += 15;
	}
	else if ( m_nEatCnt[0] >= 3 ) //�ﱤ
	{
		m_kwang += m_nEatCnt[0];
		
		if ( m_nEatCnt[0] == 3 )	//��ﱤ üũ
		{
			for ( i = 0 ; i < m_nEatCnt[0] ; ++i )
				if ( m_nEatCard[0][i] == 44 ) {	//��.
					--m_kwang; 
					break;	
				}
		}
	}

	if (m_kwang > 0)
	{
		m_nEvalution += m_kwang;
	}

	if ( CheckCardIn(1, 5, 13, 29) )	//10��¥���߿� ������ �ִ���.
	{
		m_nEvalution += 5;
	}

	if (m_nEatCnt[1] >= 5)	//10��
	{
		m_nEvalution += (m_nEatCnt[1] - 4);
	}

	if (CheckCardIn(2, 1, 4, 9))	//ȫ��
	{
		m_nEvalution += 3;
	}


	if (CheckCardIn(2, 20, 32, 36)) //û��
	{
		m_nEvalution += 3;
	}

	if (CheckCardIn(2, 12, 16, 24)) //����
	{
		m_nEvalution += 3;
	}

	if (m_nEatCnt[2] >= 5)
	{
		m_nEvalution += (m_nEatCnt[2] - 4);
	}

	if (m_nEatCnt[4] >= 10)
	{
		m_nEvalution += (m_nEatCnt[4] - 9);
	}

	if ( m_nGoCnt > 0 )
	{
		m_nEvalution += m_nGoCnt;
	}

	m_nResultScore = m_nEvalution;

	return false;
}


bool CGamer::CheckCardIn(int slot, int c1, int c2, int c3, int c4, int c5 )
{

	bool bRet = false;


	if ( slot >= 0 ) {
		for (int i = 0 ; i < m_nEatCnt[slot] && !bRet ; ++i)
			if (m_nEatCard[slot][i] == c1)
				bRet = true;

		if ( !bRet ) return bRet;
		
		bRet = false;
		
		for (i = 0 ; i < m_nEatCnt[slot] && !bRet ; ++i)
			if (m_nEatCard[slot][i] == c2)
				bRet = true;

		if (!bRet) return bRet;

		bRet = false;

		for (i = 0 ; i < m_nEatCnt[slot] && !bRet ; ++i)
			if (m_nEatCard[slot][i] == c3)
				bRet = true;



		if(c4 == -1) return bRet;
		
		if (!bRet) return bRet;

		bRet = false;

		for (i = 0 ; i < m_nEatCnt[slot] && !bRet ; ++i)
		if (m_nEatCard[slot][i] == c4)
			bRet = true;

		if(c5 == -1) return bRet;

		if (!bRet) return bRet;
		bRet = false;

		for (i = 0 ; i < m_nEatCnt[slot] && !bRet ; ++i)
		if (m_nEatCard[slot][i] == c5)
			bRet = true;


		return bRet;
	}
	else
	{
	
		int i = 0;
		for ( int nCardSlot = 0; nCardSlot < 4; nCardSlot++ ) {

			for ( i = 0 ; i < m_nEatCnt[nCardSlot] && !bRet ; ++i) {
				if (m_nEatCard[nCardSlot][i] == c1) {
					bRet = true;
					break;
				}
			}

			if ( bRet ) break;
		}

		if ( !bRet ) return bRet;
	
		bRet = false;
		for ( nCardSlot = 0; nCardSlot < 4; nCardSlot++ ) {			
			for ( i = 0 ; i < m_nEatCnt[nCardSlot] && !bRet ; ++i) {
				if (m_nEatCard[nCardSlot][i] == c2) {
					bRet = true;
					break;
				}
			}

			if ( bRet ) break;
		}

		if (!bRet) return bRet;

		bRet = false;
		for ( nCardSlot = 0; nCardSlot < 4; nCardSlot++ ) {
			for (i = 0 ; i < m_nEatCnt[nCardSlot] && !bRet ; ++i) {
				if (m_nEatCard[nCardSlot][i] == c3){
					bRet = true;
					break;
				}
			}

			if ( bRet ) break;
		}


		if(c4 == -1) return bRet;
		
		if (!bRet) return bRet;

		bRet = false;

		for ( nCardSlot = 0; nCardSlot < 4; nCardSlot++ ) {

			for (i = 0 ; i < m_nEatCnt[nCardSlot] && !bRet ; ++i) {
				if (m_nEatCard[nCardSlot][i] == c4){
						bRet = true;
						break;
				}
			}
			if ( bRet ) break;
		}

		if(c5 == -1) return bRet;

		if (!bRet) return bRet;
		bRet = false;
		
		for ( nCardSlot = 0; nCardSlot < 4; nCardSlot++ ) {
			for (i = 0 ; i < m_nEatCnt[nCardSlot] && !bRet ; ++i) {
				if (m_nEatCard[nCardSlot][i] == c5){
						bRet = true;
						break;
				}
			}

			if ( bRet ) break;
		}

		return bRet;

	}
}

void CGamer::MoveNineCard( bool bLast )
{
	for (int i = 0 ; i < m_nEatCnt[1] - 1 ; ++i)
		if (m_nEatCard[1][i] == 33)
			break;

	for ( ; i < m_nEatCnt[1] - 1 ; ++i)
		m_nEatCard[1][i] = m_nEatCard[1][i+1];

	m_nEatCard[1][i] = 0xff;
	--m_nEatCnt[1];
	
	m_nEatCard[3][m_nEatCnt[3]++] = 33;
	m_nEatCnt[4] += 2;

	if ( !bLast )	//���ǿ� ����ī�带 �ű涧�� �����صεȴ�.
		Evalutaion();
}

void CGamer::UserSelect( int nChoiceUserCardPos )
{
	int nFloorCardPos = 0xff;
	
	int cnt = 0;					// �ڽ��� ������ �ִ� ī�尡 3���� �ִ��� üũ�Ѵ�.
	int i = 0;
	BYTE arrCard[5];				// �� 3���� ������ ���� ��.


	for ( int j = 0 ; j < m_pfloor->m_nCardCnt ; ++j )
	{
		if ( m_bCard[nChoiceUserCardPos] / 4 == m_pfloor->m_strCard[j].nCardNo / 4 )			//���� �� ������..
		{
			nFloorCardPos = m_pfloor->m_strCard[j].nPosNo;
			break;
		}
	}

	m_nChoiceUserCardPos = nChoiceUserCardPos;
	m_nChoiceFloorCardPos = nFloorCardPos;


	for ( i = 0 ; i < m_nCardCnt ; ++i ) {
		if ( m_bCard[i] / 4 == m_bCard[m_nChoiceUserCardPos] / 4)
			arrCard[cnt++] = m_bCard[i];
	}

	if ( cnt >= 3 && m_bCard[m_nChoiceUserCardPos] < 48 ) {

		if ( nFloorCardPos == 0xff )				//������ ������ ����̴�.
			m_nUserThreeState = NOTICE_THREE;
		else 
			m_nUserThreeState = NOTICE_BOOM;

	}
}

bool CGamer::UserBomb( int nChoiceUserCardPos )
{
	int nFloorCardPos = 0xff;
	
	int cnt = 0;					// �ڽ��� ������ �ִ� ī�尡 3���� �ִ��� üũ�Ѵ�.
	int i = 0;
	BYTE arrCard[5];				// �� 3���� ������ ���� ��.


	for ( int j = 0 ; j < m_pfloor->m_nCardCnt ; ++j )
	{
		if ( m_bCard[nChoiceUserCardPos] / 4 == m_pfloor->m_strCard[j].nCardNo / 4 )			//���� �� ������..
		{
			nFloorCardPos = m_pfloor->m_strCard[j].nPosNo;
			break;
		}
	}

	for ( i = 0 ; i < m_nCardCnt ; ++i ) {
		if ( m_bCard[i] / 4 == m_bCard[nChoiceUserCardPos] / 4)
			arrCard[cnt++] = m_bCard[i];
	}

	if ( cnt >= 3 && m_bCard[nChoiceUserCardPos] < 48 ) {

		if ( nFloorCardPos != 0xff )				//������ ������ ����̴�.
			return true;

	}

	return false;
}

bool CGamer::CheckEvalutaion()
{

	return (m_nEvalution >= m_nMinWinVal);
}

int CGamer::AddGo()
{
	++m_nGoCnt;
	m_nMinWinVal = m_nEvalution + 2;	//�������Ҽ� �ִ� ������ �÷��ش�. �׷��� �̻��ϳ�...�� 2���� ������..1���� ���ϸ�Ǵµ�. �ƴ��� ����������..
	SendPeeEvalutaion();
	
	return m_nGoCnt;
}

int CGamer::GetPPokCardPos()
{
	for ( int j = 0 ; j < m_nCardCnt ; ++j )
	{
		if ( m_bCard[j]  == BOMB_CARDNO )		
		{
			return j;
		}
	}

	return 0xff;
}

bool CGamer::CheckMission( MISSION type )
{
	//nSlotNo = -1 �̸� ��� ���� ī�� üũ.

	int nSlotNo = -1;
	switch(type)
	{
		case JAN:
			return CheckCardIn( nSlotNo, 0, 1, 2, 3);
			break;

		case FEB:
			return CheckCardIn( nSlotNo, 4, 5, 6, 7);
			break;

		case MAR:
			return CheckCardIn( nSlotNo, 8, 9, 10, 11);
			break;

		case APR:
			return CheckCardIn( nSlotNo, 12, 13, 14, 15);
			break;

		case MAY:
			return CheckCardIn( nSlotNo, 16, 17, 18, 19);
			break;

		case JUN:
			return CheckCardIn(nSlotNo, 20, 21, 22, 23);
			break;

		case JUL:
			return CheckCardIn(nSlotNo, 24, 25, 26, 27);
			break;

		case AUG:
			return CheckCardIn(nSlotNo, 28, 29, 30, 31);
			break;

		case SEP:
			return CheckCardIn(nSlotNo, 32, 33, 34, 35);
			break;

		case OCT:
			return CheckCardIn(nSlotNo, 36, 37, 38, 39);
			break;

		case NOV:
			return CheckCardIn(nSlotNo, 40, 41, 42, 43);
			break;

		case DEC:
			return CheckCardIn(nSlotNo, 44, 45, 46, 47);
			break;

		case OKWANG:
			return CheckCardIn(nSlotNo, 0, 8, 28, 40, 44);
			break;

		case HONGDAN:
			return CheckCardIn(nSlotNo, 1, 4, 9);
			break;

		case CHONGDAN:
			return CheckCardIn(nSlotNo, 20, 32, 36);
			break;

		case CHODAN:
			return CheckCardIn(nSlotNo, 12, 16, 24);
			break;
			
		case GODORI:
			return CheckCardIn(nSlotNo, 5, 13, 29);
			break;


	}

	return false;
}
