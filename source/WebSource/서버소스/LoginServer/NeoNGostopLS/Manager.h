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
	unsigned long m_hDbQThread;				//디비 쓰레드
	unsigned long m_hRoomMaThread;			//룸관련 


	fd_set input_set, output_set, exc_set;

	int m_pulse;

	bool m_bServerNowOff;
	int m_nShutdown_time;
	bool m_Reboot;

	bool GetPath();
	bool InitGame();

	bool m_NameserverIsSlow;

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

};


int WriteToDescriptor(SOCKET desc, const char *txt, int total);
int PerformSocketRead(SOCKET desc, char *read_point, size_t space_left);

#endif
