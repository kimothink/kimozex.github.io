// DescriptorData.h: interface for the CDescriptorData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __USER_DESCRIPTOR_H__
#define __USER_DESCRIPTOR_H__

#include "common.h"
#include ".\\Card\\PokerEvalution.h"
//#include "NetWorkDefine.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CUserDescriptor
{
public:
	CUserDescriptor();
	virtual ~CUserDescriptor();

	void CloseSocket();
	void FlushQueues();
	int  ProcessOutput (bool bSecret = true);
	int  ProcessInput  (bool bSecret = true);
	void WriteToOutput (const char *txt, int nSendBufLength);
	bool GetIdName(char *arg);

	SOCKET	m_ClientSocket;	
	char m_Inbuf[MAX_RAW_INPUT_LENGTH];  /* buffer for raw input		*/
	char m_SmallOutbuf[SMALL_BUFSIZE];  /* standard output buffer		*/
	char *m_Output;			/* ptr to the current output buffer	*/
	int  m_Bufptr;			/* ptr to end of current output		*/
	int  m_Bufspace;		/* space left in the output buffer	*/
	CTxtBlock  *m_LargeOutbuf; /* ptr to large buffer, if we need it */
	CUserDescriptor *m_pNext; /* link to next descriptor		*/

	bool m_bClosed;
	CTxtQ input;				/* q of unprocessed input		*/ 	//Client로부터 받은 메세지.
	CTxtQ m_DbOutput;			//DB에서 나온 결과이다.로 요청할 값이다.
	bool m_DBQRunning;			//요청디비큐에 넣고 있을떄 m_UserDescriptorList에서 지우면 안되기 떄문에
   								//CloseSocket에서 List를 지울떄 체크해준다.
	char m_Host[HOST_LENGTH];	/* client ip					*/
	char m_HostCClass[HOST_LENGTH]; //C Class까지 ip담아놓는다.
	int	 m_IdleTics;			/* tics idle has no input		*/
	int	 m_nUserMode;
	int	 m_TimeStamp;			/* player: total playing time,  mob: last attacked pulse */
	bool m_bWaitDb;				//디비에 요청하고 기다려야 할때이다.
	bool m_bUpdateDb;			//디비에 update요청하고 나서리진 않고 대신 일정시간후에도 update가 완료되지 않으면 그 해당 Client 를 짤라버린다.
	bool m_bDbAccess;			//디비에 요청이 실패.
	int  m_Buflength;			//Client로 부터 Receive에서 받은 데이타부분이다. 큐로 복사되고 남은부분을 가르킨다.
	bool m_QuitSave;			/* when all data saved then set true */
								//Client가 접속이 끊어져서 DB 큐에 있는 모든 부분을 지우고 있는데 DB Thread가 타면 안된다.
	int	m_ConnectReq;			// connect request count   처음에 connect만하고 아무정보가 안날아오는지 체크하기위해서.

	BOOL m_bDataInput;
	BOOL m_bKeepAliveSend;

	//User Personnal Info
	sUserList m_MyInfo;

	//RoomInfo
	int m_nRoomNo;
	int m_nRoomInUserNo;
	bool m_bBbangJjang;			//게임에서 방장.
	bool m_bRoomMaster;			//게임에서 1등.
	int m_nUserPage;			//User가 현재 보고 있는 Page

	//User General
	char m_szPureHeader[3];					//Server에서 Client에게 부여하는 Header
	bool m_bPlay;							//Game을 한번이라도 했었나......한번도 안했으면 DB에 업데이트를 안해도 되므로.
	int m_nGameMode;
	int	 m_nDBMode;

	int m_nIsLogIn;

	sOption m_sOption;
	

	//Event
	char szSiteCode[5];


	//yun 2004.10.19
	int		m_nCurPtr;
	char	m_szBufferQ[MAX_BUFFER_SIZE];
	UINT	m_nReveillsize; //받아야할 데이타 크기.
	int		m_nToriZisu;
	BigInt	m_biGameRoomMoney;


	//////////////////////////////////////////////////////////////////////////
	///Poker게임에서 필요한 정보
	//////////////////////////////////////////////////////////////////////////
	int m_nGameJoin;	//0 : 대기자, 1 : 참가자
	int m_nCardCnt;
	int m_nBettingCnt;
	int m_nCardChange;	//카드 바꾸는 수.
	

	BigInt m_biJackPotMoney;		//잭팟 당첨된머니
	BigInt m_biRoundMoney;			//그 라운드에 건 총머니.
	BigInt m_biCallMoney;			//자기차례 콜머니.
	BigInt m_biSideMoney;			//올인시에 내가 걸구 더 못걸은 남은 머니.
	BigInt m_biTempRealRaceMoney;	//임시로 결과창에서 누가 얼마 따고 얼마 잃었는지 보기위해서.
									//기존값은 디비에 넣자마자 0으로 세팅하기 때문에...여기도 넣는다.

	BOOL m_bChoice;
	BOOL m_bMyOrder;
	BOOL m_bCheck;					//내가 첵을 했었는지..카드를 날릴때 FALSE로 한다.
	BOOL m_bCardChange;				//카드를 모두 체인지 했는지 알기 위해.
	BOOL m_bChangeState;			//카드를 바꾸는 상태인지( 이때 끊어짐을 방지하기 위해 )
	

	BYTE m_nUserCard[MAX_USER_CARD];
	

	sGameCurUser m_sGameCurUser;
	//sUserSideMoney m_sUserSideMoney;
	CPokerEvalution m_CPokerEvalution;

	void SetBettingBtn( BOOL bCall, BOOL bHalf, BOOL bBing, BOOL bDdaDang, BOOL bCheck, BOOL bDie, BOOL bFull, BOOL bQuater );
	void UserCardChoice( int nCardArrayNo );
	void UserCardDump( int nCardArrayNo );
	void Evalution();
	void DBUpdate( int nWinLose, int nCutTime );  /* nWinLose 0 : 패배, 1:승리*/  /* nCutTime 0 : 정상, 1: 짤렸을때*/
	bool operator>(const CUserDescriptor &rValue)
	{	return (m_CPokerEvalution > rValue.m_CPokerEvalution);	}
	bool operator<(const CUserDescriptor &rValue)
	{	return (m_CPokerEvalution > rValue.m_CPokerEvalution);	}

	//DB Update
	sUserDBUpdateInfo m_sUserDBUpdateInfo;


};

#endif // __USER_DESCRIPTOR_H__
