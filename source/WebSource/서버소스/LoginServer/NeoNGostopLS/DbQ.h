#ifndef _DBQ_H_
#define _DBQ_H_


#include "Ado.h"

#include "Define.h"
#include "UserDescriptor.h"

typedef void * LPVOID;

class CDbQ
{
public:
	CDbQ();
	virtual ~CDbQ();

	bool AddToQ(CUserDescriptor *d);
	CUserDescriptor* GetFromQ();
	void EraseQ(CUserDescriptor *d);

	bool IsFull();
	bool IsEmpty();

	void SetThread(unsigned long hThread) { m_hThread = hThread; }

	//유저들의 데이타가 있는 class
	CUserDescriptor **m_Desc;
	CUserDescriptor *m_MgrClient;

	int m_Head;
	int m_Tail;
	bool m_dbEnd;
	bool m_Semaphore;

	int	m_DescNum;		/* unique num assigned to desc		*/

	unsigned long m_hThread;

	CADODatabase	m_AdoDB;
	DWORD			m_dwLastError;


	//ADO 관련 Function
	BOOL DBConn();
	BOOL DBClose();
	bool DBConnCheck();
	void dump_com_error(_com_error &e);


	HRESULT m_hr;
	CString m_csUserId;
	VARIANT m_v_id;
	CString m_tmpString;
	_variant_t m_vData;
	
	bool SetDbInit();
	bool IsLoginDbUser( CUserDescriptor *d );
	bool NoticeRead();
	//bool PokerIsLogin( CUserDescriptor *d );
	
};


void DbQThread(LPVOID lParam);
void ThreadDbFunc(CDbQ *pQ);




#endif
















