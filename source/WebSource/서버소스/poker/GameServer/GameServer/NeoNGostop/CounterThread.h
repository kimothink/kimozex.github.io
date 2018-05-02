// CounterThread.h: interface for the RoomMaThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COUNTERTHREAD_H_
#define _COUNTERTHREAD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void * LPVOID;

class CounterThread  
{
public:
	CounterThread();
	virtual ~CounterThread();


	unsigned long m_hThread;
	
	void SetThread(unsigned long hThread) { m_hThread = hThread; }

};

void CounterThreadProc(LPVOID lParam);

#endif // !defined(AFX_ROOMMATHREAD_H__32E8F908_A90D_426E_BD6F_BFC5C10B3CB7__INCLUDED_)
