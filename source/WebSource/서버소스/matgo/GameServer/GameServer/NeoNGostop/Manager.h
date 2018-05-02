#ifndef __MANAGER_H
#define __MANAGER_H

#include "common.h"
#include "UserDescriptor.h"
#include "DbQ.h"
#include "CSServerCon.h"
#include "CounterThread.h"





class CManager
{
public:
	
	CManager();
	virtual ~CManager();


	int m_MaxPlayers;

	bool m_ServerShutdown;
	int  m_ShutdownRemain;

	SOCKET m_ListenSocket;

	int m_PlayerCount;

	CUserDescriptor *m_UserDescriptorList;		
	unsigned long m_hDbQThread;				
	unsigned long m_hRoomMaThread;			
	unsigned long m_hCSConThread;			
	unsigned long m_hCounterThread;			


	bool m_bKeepAliveSend;
	
	//fd_set input_set, output_set, exc_set;
	fd_set input_set, exc_set;

	int m_pulse;
	
	int m_nSelectCnt;

	//서버를 즉시 닫을 것인가.
	bool m_bServerNowOff;

	//서버를 몇초후에 닫을것인가..
	int m_nShutdown_time;

	//파일로 남긴다.
	bool m_Reboot;
	bool m_NameserverIsSlow;

	int m_nKeepAliveSendTime;
	int m_nKeepAliveCheckTime;

	bool GetPath();
	bool InitGame();

	SOCKET InitSocket(ush_int port);
	int SetSendbuf(SOCKET s);

	struct in_addr *GetBindAddr();

	void Nonblock(SOCKET s);

	void Run();
	
	void Close();

	void ServerSleep(struct timeval *timeout);

	void CloseSocket(CUserDescriptor *d);

	int NewDescriptor(int s);

	bool MakeThread ();
	bool MakeCSConThread();
	bool MakeCounterThread();

	CDbQ *m_pDbQ;
	CSServerCon *m_CSServerCon;

	void SendOutput(CUserDescriptor *d);

	void ServerOff();
	void KeepAliveSend();
	void KeepAliveCheck();
	void SendToAllPlaying();

	//*****************************
	bool LimitConnectCheck();
	bool DouLogin( CUserDescriptor *d );
	void CreateGame( BYTE bErrorCode, CUserDescriptor *d, char *pReceBuffer );

	void do_game_server_tics(int tics);



};


int WriteToDescriptor(SOCKET desc, const char *txt, int total);
int PerformSocketRead(SOCKET desc, char *read_point, size_t space_left);






#endif