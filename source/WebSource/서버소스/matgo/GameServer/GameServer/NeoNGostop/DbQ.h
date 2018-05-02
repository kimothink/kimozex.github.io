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
	
	VARIANT m_v_ObtainMoney;
	VARIANT m_v_LoseMoney;
	VARIANT m_v_DbServerName, m_v_GameChannel;
	VARIANT m_v_id, m_v_GoMoney, m_v_MaxMoney, m_v_WinNum, m_v_FailNum, m_v_DrawNum, m_v_CutNum;
	VARIANT m_v_WinRate, m_v_IsLogin, m_v_SiteCode, m_v_PrizeNo, m_v_SucceedCode;
	VARIANT m_v_WinNick;
	VARIANT m_v_OtherNickName1, m_v_OtherNickName2, m_v_ToriZisu, m_v_GameRoomMoney;
	_variant_t m_vData;
	VARIANT m_v_SystemMoney;
	VARIANT m_v_WinLose;
	VARIANT m_v_CutDivi;
	VARIANT m_v_RoomMoney;
	VARIANT m_v_OtherNick1;
	VARIANT m_v_OtherNick2;


	CString m_csDbServerName;
	CString m_csUserId, m_csSiteCode, m_csOtherNick1, m_csOtherNick2, m_csWinNick;
	CString m_tmpString;
	CString m_csStringObtainMoney;
	CString m_csStringLoseMoney;
	CString m_csMaxMoney;
	CString m_csStringRoomMoney;
	CString m_csSystemMoney;
	CString m_csStringUserMoney;



	bool SetDbInit();
	bool SetServerIsLogin();									//islogin 0으로 다 만든다..


	bool GameResultUpdate( CUserDescriptor *d );				//Game 결과를 DB에 업데이트한다.
	bool BugsyGameResultUpdate( CUserDescriptor *d );			//Game 결과를 DB에 업데이트한다.
	bool NewGameResultUpdate( CUserDescriptor *d );
	bool CSDBOut( CUserDescriptor *d );
	bool GetUserInfo( CUserDescriptor *d );						//유저의 정보를 가져온다.

};


void DbQThread(LPVOID lParam);
void ThreadDbFunc(CDbQ *pQ);






#endif
















