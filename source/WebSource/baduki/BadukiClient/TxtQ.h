// TxtQ.h: interface for the CTxtQ class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GAME_TxtQ_H__
#define __GAME_TxtQ_H__

//yun �⺻���ۿ� ����Ÿ�� ���� ���� �� ����Ÿ�� ��Ƴ��´�.
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

	//yun ���� Q�� �ִ� ���� �����´�.
	int GetFromQ(char *dest, int *nQSize = NULL );
	CTxtBlock *m_pHead;
	CTxtBlock *m_pTail;
};



class CRecvCardQBlock {

public:
	int m_nRecvCard;
	int m_nDivi;			//0�̸� �Ϲ� ī�� ����(���Ǽ�������)...1�̸� ��� ī�� ����.
	CRecvCardQBlock *m_pNext;
};

class CRecvCardQ {

public:
	CRecvCardQ();
	virtual ~CRecvCardQ();

	void WriteToQ( const int nRecvCard, int nDivi = 0);

	//yun ���� Q�� �ִ� ���� �����´�.
	int GetFromQ( int *nRecvCard, int *nDivi );
	void FlushQueues();

	CRecvCardQBlock *m_pHead;
	CRecvCardQBlock *m_pTail;


};



#endif
