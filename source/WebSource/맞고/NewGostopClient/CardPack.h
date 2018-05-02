#ifndef __CARD_PACK_H__
#define __CARD_PACK_H__

class CCardPack
{

public:
	int	m_nReadNo;	//ī���ѿ��� ���� �а� �ִºκ�,

	int m_nCardCnt;  //ī�带 ������ ������ΰ�
	BYTE *m_pCardPack;  //ī����

	CCardPack(BYTE cardCnt);
	virtual ~CCardPack();

	void Shuffle();			//ī�带 ���´�.
	int GetCard();			//���� m_nReadNo����Ű�� �ִ� �κ��� ī�带 �����´�.
	BYTE *GetCardPack();	//������ ī������ �����´�.
	bool SetCardPack(BYTE *cardPack, int len);		//ī������ �ִ´�.
	void SetCardNo(int pos, int val);		//ī������ Ư���� ��ġ���ٰ� ī�带 �ִ´�.
	int	 RemainCardCnt();				//���� ī���� ���� �����´�.

	void Init();
};

#endif
