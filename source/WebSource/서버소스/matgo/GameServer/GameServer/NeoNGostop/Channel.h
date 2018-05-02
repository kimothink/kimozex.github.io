#ifndef __CHANNEL_H
#define __CHANNEL_H




#include "RoomList.h"

class CChannelUserList	 
{
public:
	CChannelUserList();
	virtual ~CChannelUserList();

	CUserDescriptor *m_User;
	CChannelUserList *m_NextList;
};

class CViewWaitUserList  
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




	char m_szChannelName[MAX_CHANNEL_NAME];
	ush_int m_nCurChannelCnt;

	int CreateRoom( CUserDescriptor *d, sCreateRoom *pCreateGame, ush_int nReqRoomNo );
	int DeleteRoom( int nRoomNo );
	int RoomIn( CUserDescriptor *d, sRoomIn *psRoomIn );
	int RoomView( CUserDescriptor *d, sRoomIn *psRoomIn );

	void RoomOutUser( int nRoomNo, int nRoomInUserNo, CUserDescriptor *d );	
	int QuickJoinRoomNo( BigInt biUserMoney );


};


























#endif