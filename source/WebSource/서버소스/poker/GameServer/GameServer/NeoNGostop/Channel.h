#ifndef __CHANNEL_H
#define __CHANNEL_H




#include "RoomList.h"

class CChannelUserList	 //�ش�ä���� ���� ����.
{
public:
	CChannelUserList();
	virtual ~CChannelUserList();

	CUserDescriptor *m_User;
	CChannelUserList *m_NextList;
};

class CViewWaitUserList  //�ش�ä�ο��� ��ȿ� �����鼭 ���� ����ȭ���� ���� �ִ� ���.
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

	//�ش�ä���� ���� ����.
	CChannelUserList *m_pWaitChannelUserList;
	void AddWaitChannelUser( CUserDescriptor *d );
	void DeleteWaitChannelUser( CUserDescriptor *d );


	//�ش�ä�ο��� ��ȿ� �����鼭 ���� ����ȭ���� ���� �ִ� ���.
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