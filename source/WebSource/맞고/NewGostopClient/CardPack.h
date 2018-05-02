#ifndef __CARD_PACK_H__
#define __CARD_PACK_H__

class CCardPack
{

public:
	int	m_nReadNo;	//카드팩에서 현재 읽고 있는부분,

	int m_nCardCnt;  //카드를 몇장을 만들것인가
	BYTE *m_pCardPack;  //카드팩

	CCardPack(BYTE cardCnt);
	virtual ~CCardPack();

	void Shuffle();			//카드를 섞는다.
	int GetCard();			//현재 m_nReadNo가르키고 있는 부분의 카드를 가져온다.
	BYTE *GetCardPack();	//생성된 카드팩을 가져온다.
	bool SetCardPack(BYTE *cardPack, int len);		//카드팩을 넣는다.
	void SetCardNo(int pos, int val);		//카드팩의 특별한 위치에다가 카드를 넣는다.
	int	 RemainCardCnt();				//남은 카드의 수를 가져온다.

	void Init();
};

#endif
