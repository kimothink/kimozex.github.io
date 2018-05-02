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

	int m_nGetOutCnt;		//찬성해야하는수.
	int m_nGetOutRepl;		//찬성한수.
	int	m_nOutUserNo;		//강퇴자 번호.
	int m_nRound;
	int m_nPrizeDecide;		//0이면 상품 결정
	int m_nPrizeNo;			//상품번호.
	BigInt m_biRoomCurMoney;

	//Call과 따당을 위해서 앞사람이 한 머니를 무조건 넣어둔다.
	BigInt m_biBeforeMoney;
	//BigInt m_biSideMoney;
	
	void RoomInit();

	sRoomList l_sRoomList;
	CUserDescriptor *m_User[MAX_ROOM_IN];


	CPokerEngine m_CPokerEngine;
	


};


#endif // !defined(AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_)
