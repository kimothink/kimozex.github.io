#ifndef __USERLIST_H_
#define __USERLIST_H_

#include "ClientDefine.h"
#include "..\BadukiCommonDefine\BadukiCommonDefine.h"
//#include "PokerEvalution.h"

//���� ����
class CUserInfo {

public:
	CUserInfo();
	virtual ~CUserInfo();

	bool m_bUser;
	sUserInfo m_sUserInfo;


	Init();
	

};

//���� ��������Ʈ
class CUserList {

public:
	CUserList();
	virtual ~CUserList();


	int m_nCurMax;		//�迭�� ���� ū.
	

	CUserInfo m_WaitUserInfo[MAX_CHANNELPER];


	int AddUser( sUserInfo *pUserInfo );
	void DeleteUser( int nUniqueUserNo );
	int FindUser( int nUniqueUserNo );


};



//�����Ҷ�  ����
class CGameUserInfo {

public:
	CGameUserInfo();
	virtual ~CGameUserInfo();

	bool    m_bDie;	                        //������ �����ߴ���.
	bool    m_bUser;
	bool    m_bBBangJjang;	                //����
	bool    m_bSun;	                        //��
	bool    m_bChoioce;	                    //���� ������ �ߴ����� ����.
	bool    m_bEnd;		                    //�������� ������ �����൵ �Ǵ���.
	bool    m_bAllin;		
	bool    m_bExitReserve;	                //������ ����

	bool    m_bUserSelectShow;	            //���� �� ���� �����Ѱ� ǥ���ϱ�.
	char    m_cUserSelct;                   // '0':����, '1':üũ, '2':����, '3':��, '4':����, '5':��, '6':Ǯ, '7':����
	int     m_nUserSelectCnt;	            //���� �����Ѱ� ũ�� ǥ���ϴ� �ð�.
	
  //int         m_nGameJoin;	            //0 : �������ε� �߰��� ����, 1 : �������� ������, 2 : ������
	int         m_nCardCnt;
	BigInt      m_biRealUserMoney;	        //���Ӱ���� ���ö� ������ ���� ��������� �ϱ� ����.������ ����ö� ���� ���⿡ �ӽ÷� �־���´�.
	BigInt      m_biPureUserWinLoseMoney;	//�� �ǿ��� ���� ���ų� ���� �Ӵ�
	sUserInfo   m_sUserInfo;
	sUserCardInfo m_sUserCardInfo[MAX_CARD];
	BYTE        m_byUserRealCardNo[MAX_CARD];

    sUserNewCardPacket m_sUserNewCardPacket;
    sUserNewCard    m_sNewCard;             // �ٲ� �� ī�� ���� ����



    

	//CPokerEvalution m_CPokerEvalution;
	BYTE m_szTempJokboCard[8];	            //��ù�ڸ��� ����. �״����� ������ �ش��ϴ� ī��
	BYTE m_szJokboCard[8];	                //��ù�ڸ��� ����. �״����� ������ �ش��ϴ� ī��
	BYTE m_szFixJokboCard[8];

	Init();
	InitCard();
    void InitOneCard( int nIdx );
    void InitOneCard( sUserCardInfo *pCard );
	UserInfoInit();
};


//�����Ҷ� ���� ����Ʈ
class CGameUserList {
	
public:
	CGameUserList();
	virtual ~CGameUserList();

	

	int AddUser( sUserInfo *pUserInfo, int nSlotNo = -1  );	//0 : �������ε� �߰��� ����, 1 : �������� ������, 2 : ������ 
	int AddRoomInOtherUser( sUserInfo *pUserInfo, int nNextNo );	//nNextNo : ������� ���°�δ�.
	void DeleteUser( int nUniqueUserNo );
	int FindUser( int nUniqueUserNo );
	void NewBangJangSet( int nUniqueUserNo );
	void NewSunSet( int nUniqueUserNo );
	
	void BangJangDel();
	void SunDel();
	void UserCardDump( int nUserArrayNo, int nCardArrayNo );	//ī�� ���� ������.
	void UserCardChoice( int nUserArrayNo, int nCardArrayNo );	
	void UserAllClear();
	void UserCardPosRefresh( int nUserArrayNo );		//ó�� ������ ī�带 �����ϸ� �� �ٿ��ִ� .
	void UserCardShrink( int nUserArrayNo );

	CGameUserInfo m_GameUserInfo[MAX_ROOM_IN];

};


//���� �渮��Ʈ.
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