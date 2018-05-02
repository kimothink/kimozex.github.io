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

class CUserDescriptor;

class CTxtQ {
public:
	CTxtQ();
	virtual ~CTxtQ();

	void WriteToQ(const char *txt, int ntxtSize );

	//yun 현재 Q에 있는 것을 가져온다.
	int GetFromQ(char *dest, int *nQSize = NULL );
	CTxtBlock *m_pHead;
	CTxtBlock *m_pTail;

	CUserDescriptor *m_UserDescriptor;

};
#endif
