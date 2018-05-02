#ifndef __CHANNEL_H
#define __CHANNEL_H




#include "RoomList.h"

class CChannelUserList	 //해당채널의 대기실 유저.
{
public:
	CChannelUserList();
	virtual ~CChannelUserList();

	CUserDescriptor *m_User;
	CChannelUserList *m_NextList;
};

class CViewWaitUserList  //해당채널에서 방안에 있으면서 대기실 유저화면을 보고 있는 경우.
{
public:
	CViewWaitUserList();
	virtual ~CViewWaitUserList();

	CUserDescriptor *m_User;
	CViewWaitUserList *m_NextList;
};


class CChannel  
{
public:
	CChannel();
	virtual ~CChannel();



public:

	CRoomList m_RoomList[MAX_ROOMCNT];

	//해당채널의 대기실 유저.
	CChannelUserList *m_pWaitChannelUserList;
	void AddWaitChannelUser( CUserDescriptor *d );
	void DeleteWaitChannelUser( CUserDescriptor *d );


	//해당채널에서 방안에 있으면서 대기실 유저화면을 보고 있는 경우.
	CViewWaitUserList *m_pViewWaitUserList;
	void AddViewWaitUserList( CUserDescriptor *d );
	void DeleteViewWaitUserList( CUserDescriptor *d );

	ush_int m_nCurChannelCnt;

	int CreateRoom( CUserDescriptor *d, sCreateRoom *pCreateGame );
	int DeleteRoom( int nChannel, int nRoomNo );
	int RoomIn( CUserDescriptor *d, sRoomIn *psRoomIn );
	void RoomOutUser( int nRoomNo, int nRoomInUserNo, CUserDescriptor *d );	
	int QuickJoinRoomNo( BigInt biUserMoney );


};


























#endif