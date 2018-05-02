// RoomList.h: interface for the RoomList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_)
#define AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserDescriptor.h"

class CRoomList  
{
public:
	CRoomList();
	virtual ~CRoomList();

public:

	sRoomList l_sRoomList;
	CUserDescriptor *m_User[MAX_ROOM_IN];
	BYTE szSunPack[MAX_SUNCARD_CNT];
	BYTE szUserChoiceSunCard[MAX_ROOM_IN];		//유저가 처음 선을 뽑기위해 선택한 카드 번호.
	

	int m_nReadyCnt;
};


#endif // !defined(AFX_ROOMLIST_H__B2ABB67F_9EBF_4EDE_B1B9_459ECB8049ED__INCLUDED_)
