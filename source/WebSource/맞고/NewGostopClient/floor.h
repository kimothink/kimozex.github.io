#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "Gamer.h"



class Cfloor
{
public:
	Cfloor();
	virtual ~Cfloor();



	SFLOORCARD m_strCard[MAX_FLOOR_CARD];				//�ٴ��� ī��
	int	m_nPosCnt[MAX_CARD_POS];					//�ٴ��� ī�尡 ����ġ�� ������ ��ȳ�.  ���� ī���� pos�� ��ġ�� ī���
	POINT	m_pos[MAX_CARD_POS];					//�ٴ��� ��ġ ����.

	int		m_nCardCnt;								//�ٴڿ� �Ѹ��� ��������. �������� �����ϰ���??
	int		m_nMasterSlotNo;						//������(�̱��)����.��...�ڽ��� 0�� ����...�������� 1, ������ 2, ���Թ�ȣ��� �迭�� �����Ѵ�.  
	int		m_nBbangJjangSlotNo;					//������ ����.
													//�����ѹ��� �����Ͱ� 0��. ���������� 1��, 2��.
	int		m_nWinerNo;								//������ ������ȣ.
	CGamer	m_pGamer[MAX_ROOM_IN];				
	int		m_nPregidentNo;							//������̵� ������ ��ȣ.
	int		m_nGetEmptyPos;							//����� ������...
	int		m_nBaseMulti;							//������ �� ������ �ι�.
	bool	m_RealMode;								//��¥ ��������, ���̶� ġ����.

	void Init();
	void OnlyAdd(const BYTE nCard);					//�ٴ��� ī�带 �����ش�.
	int GetEmptyPos();								//�ٴ��� ��ġ�� ����� ã�ƺ���.  16������ ������ ���ѷ����� �� ���ɼ��� �ִ�. �̺κ��� �ٽû����ؼ� �Ѵ�.
	int GetCurPosCnt(int nCardCnt);					//�ٴ��� ���� ��ġ�� �ѷ��� ī�尡 �ִ��� üũ
	int AddCardUser(int nUserNo, int nCardNo);		//ó���� �������� ī�带 ������ �ٶ� ���.
	void SortUserCard(int nUserNo);					//ó�� �򸮰� ���� �и� ��Ʈ�Ѵ�.
	bool IsFloorHas(int nCardNo);					//�ٴڿ� ���� �а� �ִ��� ����.
	BYTE CheckNephew();								//���� ī�尡 ���п� �ִ��� �����Ѵ�.
	int FloorEat(int nCardCnt, int nSlotNo);		//�ش� ī�带 �ش� ������ ����ڰ� �Դ´�,
	POINT SetPoint(LONG x, LONG y);
	int AppendAdd(int nPosNo, const BYTE nCard);	//�ٴڿ� �и� ������ ��ġ�� �ٴ��� �п� �����ش�.
	int	GetFIrstPosFromFloorPos(int nFloorPos);		//

	int GetOneEachGamer( int nSlotNo, int nOtherSlotNo, int nGetOtherUserCardCnt ); //�ٸ��������Լ� �Ǹ� �����´�.

	//���� ó�� �����ϸ� �̰� �����ش�.
	void AddMyGamer( int nGamerUserNo, char *pPreImage );		
	void AddRoomInGamer( int nSlot, char *pPreImage );
	int AddOtherGamer( int nSlot, int nUserNo, char *pPreImage );

	//�����϶��� �游�鶧, �ٸ������ ���ö�..���������ְ�...������ ������ ������ 
	//Ȯ���� ���������� ���ο��� ��Ŷ�� ���ư��� ������ ����� ������ �ƴѻ���� ���� �̸� ������Ű��
	//��� ���ΰ����� �����Ҷ� ������ �ƴѻ���� ����� ��� Ȯ���� �������� üũ�Ѵ�.
	//������ ���۵Ǹ� �����̴� �ƴϴ� ��� �̸� 0�� �����Ѵ�.
	//void RoomInOutCnt( int nDivi );					//0 : ������� 0, 1 : �ϳ� ����, 2 : �ϳ�����.

	bool RealGame();								//������ ������ RealGame�� �ȴ�.
	bool RoomUserCntOK();							//������ ������ �� �������� true�̴�.

	//���̶� ġ�� ������ ������ ���͵� ���̸��� ���� ���� ��Ű�� �ȵǱ� ������..üũ�Ѵ�.
	void UserMoneyVal( int nSlotNo,  BigInt biMoney, bool bPM );	//�������� �� +-
	
	void DeleteUser( int nSlotNo );					//�濡�� ����.
	int FindUser( int nGamerUserNo );
	void UserRefresh( int nMasterSlot ); 


	int nTemp;


};













#endif

