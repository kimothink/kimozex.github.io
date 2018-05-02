// RoomList.h: interface for the RoomList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_)
#define AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserDescriptor.h"
#include ".\\Card\\PokerEngine.h"

class CRoomList  
{
public:
	CRoomList();
	virtual ~CRoomList();

public:

	int m_nGetOutCnt;		//�����ؾ��ϴ¼�.
	int m_nGetOutRepl;		//�����Ѽ�.
	int	m_nOutUserNo;		//������ ��ȣ.
	int m_nRound;
	int m_nPrizeDecide;		//0�̸� ��ǰ ����
	int m_nPrizeNo;			//��ǰ��ȣ.
	BigInt m_biRoomCurMoney;

	//Call�� ������ ���ؼ� �ջ���� �� �Ӵϸ� ������ �־�д�.
	BigInt m_biBeforeMoney;
	//BigInt m_biSideMoney;
	
	void RoomInit();

	sRoomList l_sRoomList;
	CUserDescriptor *m_User[MAX_ROOM_IN];


	CPokerEngine m_CPokerEngine;
	


};


#endif // !defined(AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_)
