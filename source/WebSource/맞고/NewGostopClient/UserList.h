#ifndef __USERLIST_H_
#define __USERLIST_H_


#include "UserInfo.h"


//========================================================================
// ���� ���� ����Ʈ
//========================================================================

class CUserList {

public:
	CUserList();
	virtual ~CUserList();
	
	CUserInfo m_pUserInfo[MAX_CHANNELPER];

	int     AddUser( sUserInfo *pUserInfo );
	void    DeleteUser( int nUserNo );
	int     FindUser( int nUniUserNo );

	int m_nCurMax;		//�迭�� ���� ū.
};



//========================================================================
// ���� �� ����Ʈ
//========================================================================
class CGameWaitRoomList {
	
public:
	CGameWaitRoomList();
	virtual ~CGameWaitRoomList();
	sRoomStateInfo m_sRoomStateInfo[MAX_ROOMCNT];
};




//////////////////////////////////////////////////////////////////////////
// �濡���� ������
//////////////////////////////////////////////////////////////////////////
//�����Ҷ�  ����
class CGameUserInfo {

public:
	CGameUserInfo();
	virtual ~CGameUserInfo();

	bool m_bUser;
	bool m_bBBangJjang;						//����
	bool m_bSun;	                        //��
	BigInt m_biObtainMoney;
	BigInt m_biLoseMoney;
	sUserInfo m_sUserInfo;
    sGameEndHistory l_sGameEndHistory;

	int m_nVoice;

	Init();
};


class CGameUserList {

public:
	CGameUserList();
	virtual ~CGameUserList();

	int AddGameUser( sUserInfo *pUserInfo, int nVoice, int nSlotNo = -1 );
	int AddRoomInOtherUser( sUserInfo *pUserInfo, int nVoice, int nNextNo );	//nNextNo : ������� ���°�δ�.
	void DeleteUser( int nUniqueUserNo );
	int FindUser( int nUniqueUserNo );
	void NewBangJangSet( int nUniqueUserNo );
	void NewSunSet( int nUniqueUserNo );
	
	void BangJangDel();
	void SunDel();
	void UserAllClear();
	void UserVoiceSet( int nUserUniqNo, int nUserVoice );


	CGameUserInfo m_GameUserInfo[MAX_ROOM_IN];

};





#endif
