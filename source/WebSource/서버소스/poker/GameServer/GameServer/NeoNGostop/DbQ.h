#ifndef _DBQ_H_
#define _DBQ_H_


#include "Ado.h"

#include "Define.h"
#include "UserDescriptor.h"
#include "JackPotQ.h"


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

	//�������� ����Ÿ�� �ִ� class
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

	//���� ť
	CJackPotQ m_CJackPotQ;
	bool JackPotUpdate( sJackPotQ *asJackPotQ );


	//ADO ���� Function
	BOOL DBConn();
	BOOL DBClose();
	bool DBConnCheck();
	void dump_com_error(_com_error &e);

	HRESULT m_hr;


	//char szStringUserMoney[MAX_MONEY_STRING];
	
	CString m_csUserId;
	CString m_csDbServerName;
	CString m_tmpString;
	CString m_csOtherNick1;
	CString m_csOtherNick2;
	CString m_csOtherNick3;
	CString m_csOtherNick4;
	CString m_csStringUserMoney;
	CString m_csStringRoomMoney;
	CString m_csJackPotMoney;
	CString m_csJackPotRoomMoney;
	CString m_csSystemMoney;
	CString m_csUserJackPotMoney;


    float g_ChonSystemMoneyRate;
    float g_ChonSystemJackPotRate;
	
	_variant_t m_v_id, m_v_DbServerName, m_v_GameChannel, m_v_GoMoney, m_v_WinLose, m_v_CutDivi, m_v_RoomMoney;
	_variant_t m_v_OtherNick1, m_v_OtherNick2, m_v_OtherNick3, m_v_OtherNick4;
	_variant_t m_v_JackPotMoney, m_v_JackPotRoomMoney;
	_variant_t m_v_JackPotDivi, m_v_SystemMoney, m_v_UserJackPotMoney;
	_variant_t m_v_PrizeNo, m_v_SucceedCode;

	_variant_t m_vData;
	


	bool SetDbInit();							//�ڽ��� ���� �̸����ε� ��� �������� islogin�� 0���� �Ѵ�.
	bool SetServerIsLogin();					
	bool GetJackPotMoney();						//��� ����� ������ ������ ���۵ɶ� �����´�.
	bool UserUpdate( CUserDescriptor *d );		//���Ӱ���� ������Ʈ �Ѵ�.
	bool CSDBOut( CUserDescriptor *d );			//�ش� ������ islogin �� 0���� �Ѵ�. 
	bool GetUserInfo( CUserDescriptor *d );		//������ ������ �����´�.
	bool GetSysRateInfo();

};


void DbQThread(LPVOID lParam);
void ThreadDbFunc(CDbQ *pQ);



#endif
















