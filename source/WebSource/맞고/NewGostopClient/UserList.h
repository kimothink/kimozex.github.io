#ifndef __USERLIST_H_
#define __USERLIST_H_


#include "UserInfo.h"


//========================================================================
// 대기실 유저 리스트
//========================================================================

class CUserList {

public:
	CUserList();
	virtual ~CUserList();
	
	CUserInfo m_pUserInfo[MAX_CHANNELPER];

	int     AddUser( sUserInfo *pUserInfo );
	void    DeleteUser( int nUserNo );
	int     FindUser( int nUniUserNo );

	int m_nCurMax;		//배열중 가장 큰.
};



//========================================================================
// 대기실 방 리스트
//========================================================================
class CGameWaitRoomList {
	
public:
	CGameWaitRoomList();
	virtual ~CGameWaitRoomList();
	sRoomStateInfo m_sRoomStateInfo[MAX_ROOMCNT];
};




//////////////////////////////////////////////////////////////////////////
// 방에서의 유저들
//////////////////////////////////////////////////////////////////////////
//게임할때  유저
class CGameUserInfo {

public:
	CGameUserInfo();
	virtual ~CGameUserInfo();

	bool m_bUser;
	bool m_bBBangJjang;						//방장
	bool m_bSun;	                        //선
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
	int AddRoomInOtherUser( sUserInfo *pUserInfo, int nVoice, int nNextNo );	//nNextNo : 방장다음 몇번째인다.
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
