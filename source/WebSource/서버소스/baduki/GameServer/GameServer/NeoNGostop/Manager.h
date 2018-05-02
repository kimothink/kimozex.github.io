#ifndef __MANAGER_H
#define __MANAGER_H

#include "common.h"
#include "UserDescriptor.h"
#include "DbQ.h"





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
	unsigned long m_hDbQThread;				//��� ������

	bool m_bKeepAliveSend;
	
	//fd_set input_set, output_set, exc_set;
	fd_set input_set, exc_set;

	int m_pulse;
	
	int m_nSelectCnt;

	//������ ��� ���� ���ΰ�.
	bool m_bServerNowOff;

	//������ �����Ŀ� �������ΰ�..
	int m_nShutdown_time;

	//���Ϸ� �����.
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

	CDbQ *m_pDbQ;

	void SendOutput(CUserDescriptor *d);

	void ServerOff();
	void KeepAliveSend();
	void KeepAliveCheck();
	void SendToAllPlaying();

	void do_game_server_tics(int tics);



};


int WriteToDescriptor(SOCKET desc, const char *txt, int total);
int PerformSocketRead(SOCKET desc, char *read_point, size_t space_left);






#endif