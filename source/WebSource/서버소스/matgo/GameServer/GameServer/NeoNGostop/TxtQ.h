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

	int GetFromQ(char *dest, int *nQSize = NULL );
	CTxtBlock *m_pHead;
	CTxtBlock *m_pTail;

	CUserDescriptor *m_UserDescriptor;

};
#endif
