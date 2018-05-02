#ifndef _CSSERVERCON_H_
#define _CSSERVERCON_H_


#include "UserDescriptor.h"


class CSServerCon
{
public:
	CSServerCon();
	virtual ~CSServerCon();

	unsigned long m_hThread;
	void SetThread(unsigned long hThread) { m_hThread = hThread; }	


	CUserDescriptor *m_CSClient;

};



void CSServerConThread( LPVOID lParam );
void CSServerConDbFunc( CSServerCon *pCSServerCon );



#endif