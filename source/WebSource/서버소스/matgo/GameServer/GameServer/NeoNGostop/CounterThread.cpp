#include "Common.h"
#include "CounterThread.h"
#include "Manager.h"
#include "NGFrame.h"

extern CManager *l_pCManager;


CounterThread::CounterThread()
{

}

CounterThread::~CounterThread()
{

}

void CounterThreadProc(LPVOID lParam)
{
	int nCounterTime = (int)PULSE_COUNTER_TIMEOUT;

	CUserDescriptor *d;
	int nSendPacketLen = 0;
	ush_int nClientCnt[CHANNEL_PER_MASHINE];



	while( true ) {

		Sleep(nCounterTime);
		
		memset( nClientCnt, 0x00, sizeof(nClientCnt) );
		
		for ( d = l_pCManager->m_UserDescriptorList; d; d = d->m_pNext ) {
			nClientCnt[d->m_MyInfo.l_sUserInfo.nChannel]++;
		}


	}

}

