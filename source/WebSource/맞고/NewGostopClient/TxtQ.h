// TxtQ.h: interface for the CTxtQ class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GAME_TxtQ_H__
#define __GAME_TxtQ_H__

//yun 기본버퍼에 데이타가 들어가지 못할 떄 데이타를 담아놓는다.
class CTxtBlock {
public:
   char	*m_Text;
   CTxtBlock *m_pNext;
   int m_nBlockSize;
};

class CTxtQ {
public:
	CTxtQ();
	virtual ~CTxtQ();

	void WriteToQ(const char *txt, int ntxtSize );

	//yun 현재 Q에 있는 것을 가져온다.
	int GetFromQ(char *dest, int *nQSize = NULL );
	CTxtBlock *m_pHead;
	CTxtBlock *m_pTail;
};



class CRecvCardQBlock {

public:
	int m_nRecvCard;
	int m_nDivi;			//0이면 일반 카드 날림(쌍피선택포함)...1이면 흔듬 카드 날림.
	CRecvCardQBlock *m_pNext;
};

class CRecvCardQ {

public:
	CRecvCardQ();
	virtual ~CRecvCardQ();

	void WriteToQ( const int nRecvCard, int nDivi = 0);

	//yun 현재 Q에 있는 것을 가져온다.
	int GetFromQ( int *nRecvCard, int *nDivi );
	void FlushQueues();

	CRecvCardQBlock *m_pHead;
	CRecvCardQBlock *m_pTail;


};



#endif
