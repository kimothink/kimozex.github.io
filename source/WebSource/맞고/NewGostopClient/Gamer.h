#ifndef __GAMER_H__
#define __GAMER_H__

#include "ClientDefine.h"

extern HWND g_hWnd;


int GetPeeVal(BYTE nCard);


class Cfloor;

class CGamer
{
public:
	CGamer();
	virtual ~CGamer();


public:

	BYTE m_bCard[MAX_USERCARD_CNT];					//������ ����ī��			
	int	 m_nCardCnt;								//������ ����ī��� �Ǵ� ������ �ִ� ī���.
	BYTE m_nEatCnt[5];								// [0] : ��������, [1] : 10�߸����� , [2] : 5�߸�����    , 
													// [3] : �����Ǹ�����(���� 1�� ��1���� 2).  , 
													// [4] : ���ǳ� ������ ���� �����Ǽ�(���� 1�� ��1���� 3) .  ���� ���ڸ� �ľ�.

	BYTE m_nEatCard[4][30];							// [0] : ��,  [1] : 10��,  [2] : 5�� , [3] : ��				����ī�带 �Ծ����� 
	bool m_bNineCard;								// 9�� ���Ǹ� �Ծ����� ���߿� �Ƿ� �Ұ�����..�ƴ� 10������ �Ұ����� ����.
	int  m_nEvalution;								//������ ���� ����.
	Cfloor *m_pfloor;
	bool m_bComputer;								//��ǻ������ 
	
	int m_nChoiceUserCardPos;						//������ ������ ī��
	int m_nChoiceFloorCardPos;						//������ ���� �ٴ��� ī��.
	int  m_nMinWinVal;								//������ �� ������ �Ҽ� �ִ� ����..
	BYTE m_arrCard[5];								//��ź���� �Ǿ����� �׼����� ����..
	int m_nUserThreeState;							//��ź, ���.
	int m_nShakeCnt;
	int m_nOldShakeCnt;
	int m_nBbukCnt;									//�� ī��Ʈ
	int m_nMulti;									//������ ���߿� *2 �� * 3�� ǥ��.
	int m_nGoCnt;
	bool m_bUserEvent;								//�����̺�Ʈ�� ������ true
	int m_nBombFlipCnt;								// ��ź������ �� �׳� �ѱ�� ī��Ʈ bsw 01.05
	int	m_GetOtherUserCardCnt;						//�ٸ�������� ���徿 ���ܿðǰ�...
	int m_nLoseEval;								//������ ��������.
	BigInt m_biCurWinLoseMoney;							//������ ���� �濡�� �Ұ� ����.

	//int m_nSlotNo;
	int m_nUserNo;
	int m_nBbukCardNo[10];							//�ڽ��� ����ī��.

	int m_nResultScore;                             // m_nEvalution �� �޴´�.
	//yuno 2005.10.28
	int m_nMissionMultiply;

	bool m_bAlly;									//�����..
	int m_nGamerUserNo;								//�濡���� �ڽŹ�ȣ.
	bool m_bGodori;
	bool m_b3kwang;
	bool m_b4kwang;
	bool m_b5kwang;
	bool m_bHongdan;
	bool m_bChongdan;
	bool m_bGusa;
	bool m_ExitReserv;
	bool m_bMissionEffect;


	BigInt m_biWinMoney;								//������ �� �ǿ� ����.
	BigInt m_biLoseMoney;								//������ �� �ǿ� ������.



	void SetFloor( Cfloor *pfloor );				//�ٴ������͸� �ѱ��. AutoSelect�� ����.
	void RcvCard(const BYTE nCard);
	void SortUserCard();
	void EatCard(const BYTE nCard);
	int IsPregident();								// �ڽ��� �а� ���������.
	void AutoSelect();								// ī�� ��ġ �ڵ� ����
	void UserSelect(int nChoiceUserCardPos );		// ������ �ڽ��� �и� ����.
	void UserMoneyVal( BigInt biMoney, bool bPM );		// �ش������� ���� ������Ų��. bPM : true +, false : -
	int GetOnePee(int nGetOtherUserCardCnt);		//
	bool Evalutaion();								// �������.
	bool CheckCardIn(int slot, int c1, int c2, int c3, int c4 = -1, int c5 = -1);	//3������ ī�尡 ������ ������ �ִ���.
	void MoveNineCard( bool bLast = false);			//9 ���Ǹ� �Ƿ� �����ϸ� �Ű��ش�.
	bool CheckEvalutaion();							//������ �� ������ �Ҽ� �ִ���....
	int AddGo();									//��ī��Ʈ�� �÷��ش�.
	int GetPPokCardPos();								//��ź�� �ǰ� �ִ� ��ġ�� �˷��ش�.
	void Init();
	void RoomInInit();								//������ �濡 ��������..�ʱ�ȭ �Ұ���...���嵵 ��������.
	bool CheckMission( MISSION type );
	bool UserBomb( int nChoiceUserCardPos );		//���õ� ī�尡 �������� ����������ÿ� �˾ƾ� �Ѵ�.


	bool SendPeeEvalutaion();						//���Ǹ� ���� ��������� �ٷ��Ѵ�.

	bool m_bGo;
	bool m_bMmong;
	bool m_bShake;
	bool m_bMission;
	bool m_bPbak; // �ǹ�
	bool m_bKwangbak; // ����
	bool m_bDokbak; // ����
	int  m_kwang;

	char m_szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];
	char m_szPreAvatar[MAX_AVAURLLEN];
};












#endif
