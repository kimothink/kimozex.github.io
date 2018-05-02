#ifndef __USERLIST_H_
#define __USERLIST_H_

#include "ClientDefine.h"
#include "..\BadukiCommonDefine\BadukiCommonDefine.h"
//#include "PokerEvalution.h"

//대기실 유저
class CUserInfo {

public:
	CUserInfo();
	virtual ~CUserInfo();

	bool m_bUser;
	sUserInfo m_sUserInfo;


	Init();
	

};

//대기실 유저리스트
class CUserList {

public:
	CUserList();
	virtual ~CUserList();


	int m_nCurMax;		//배열중 가장 큰.
	

	CUserInfo m_WaitUserInfo[MAX_CHANNELPER];


	int AddUser( sUserInfo *pUserInfo );
	void DeleteUser( int nUniqueUserNo );
	int FindUser( int nUniqueUserNo );


};



//게임할때  유저
class CGameUserInfo {

public:
	CGameUserInfo();
	virtual ~CGameUserInfo();

	bool    m_bDie;	                        //유저가 다이했는지.
	bool    m_bUser;
	bool    m_bBBangJjang;	                //방장
	bool    m_bSun;	                        //선
	bool    m_bChoioce;	                    //내가 선택을 했는지의 여부.
	bool    m_bEnd;		                    //마지막에 족보를 보여줘도 되는지.
	bool    m_bAllin;		
	bool    m_bExitReserve;	                //나가기 예약

	bool    m_bUserSelectShow;	            //다이 등 유저 선택한거 표시하기.
	char    m_cUserSelct;                   // '0':따당, '1':체크, '2':쿼터, '3':삥, '4':하프, '5':콜, '6':풀, '7':다이
	int     m_nUserSelectCnt;	            //유저 선택한거 크게 표시하는 시간.
	
  //int         m_nGameJoin;	            //0 : 게임자인데 중간에 들어옴, 1 : 게임현재 참가자, 2 : 관전자
	int         m_nCardCnt;
	BigInt      m_biRealUserMoney;	        //게임결과가 나올때 마지막 돈을 세팅해줘야 하기 때문.마지막 결과올때 돈을 여기에 임시로 넣어놓는다.
	BigInt      m_biPureUserWinLoseMoney;	//그 판에서 순수 따거나 잃은 머니
	sUserInfo   m_sUserInfo;
	sUserCardInfo m_sUserCardInfo[MAX_CARD];
	BYTE        m_byUserRealCardNo[MAX_CARD];

    sUserNewCardPacket m_sUserNewCardPacket;
    sUserNewCard    m_sNewCard;             // 바꿀 새 카드 정보 저장



    

	//CPokerEvalution m_CPokerEvalution;
	BYTE m_szTempJokboCard[8];	            //맨첫자리를 족보. 그다음은 족보에 해당하는 카드
	BYTE m_szJokboCard[8];	                //맨첫자리를 족보. 그다음은 족보에 해당하는 카드
	BYTE m_szFixJokboCard[8];

	Init();
	InitCard();
    void InitOneCard( int nIdx );
    void InitOneCard( sUserCardInfo *pCard );
	UserInfoInit();
};


//게임할때 유저 리스트
class CGameUserList {
	
public:
	CGameUserList();
	virtual ~CGameUserList();

	

	int AddUser( sUserInfo *pUserInfo, int nSlotNo = -1  );	//0 : 게임자인데 중간에 들어옴, 1 : 게임현재 참가자, 2 : 관전자 
	int AddRoomInOtherUser( sUserInfo *pUserInfo, int nNextNo );	//nNextNo : 방장다음 몇번째인다.
	void DeleteUser( int nUniqueUserNo );
	int FindUser( int nUniqueUserNo );
	void NewBangJangSet( int nUniqueUserNo );
	void NewSunSet( int nUniqueUserNo );
	
	void BangJangDel();
	void SunDel();
	void UserCardDump( int nUserArrayNo, int nCardArrayNo );	//카드 한장 버리기.
	void UserCardChoice( int nUserArrayNo, int nCardArrayNo );	
	void UserAllClear();
	void UserCardPosRefresh( int nUserArrayNo );		//처음 유저가 카드를 선택하면 딱 붙여주는 .
	void UserCardShrink( int nUserArrayNo );

	CGameUserInfo m_GameUserInfo[MAX_ROOM_IN];

};


//대기실 방리스트.
class CWaitRoomList {
	
public:
	CWaitRoomList();
	virtual ~CWaitRoomList();

	

	void Init();
	int AddRoom( sRoomInfo *psRoomInfo );
	void DeleteRoom( int nRoomNo );

	void AddUser( int nRoomNo, int nRoomCnt );
	void DeleteUser( int nRoomNo, int nRoomCnt );


	BOOL m_bRoom[MAX_ROOMCNT];
	sRoomInfo m_sWaitRoomList[MAX_ROOMCNT];

};


#endif