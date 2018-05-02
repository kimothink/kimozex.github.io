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
	CTxtQ input;				/* q of unprocessed input		*/ 	//Client�κ��� ���� �޼���.
	CTxtQ m_DbOutput;			//DB���� ���� ����̴�.�� ��û�� ���̴�.
	bool m_DBQRunning;			//��û���ť�� �ְ� ������ m_UserDescriptorList���� ����� �ȵǱ� ������
   								//CloseSocket���� List�� ��� üũ���ش�.
	char m_Host[HOST_LENGTH];	/* client ip					*/
	char m_HostCClass[HOST_LENGTH]; //C Class���� ip��Ƴ��´�.
	int	 m_IdleTics;			/* tics idle has no input		*/
	int	 m_nUserMode;
	int	 m_TimeStamp;			/* player: total playing time,  mob: last attacked pulse */
	bool m_bWaitDb;				//��� ��û�ϰ� ��ٷ��� �Ҷ��̴�.
	bool m_bUpdateDb;			//��� update��û�ϰ� �������� �ʰ� ��� �����ð��Ŀ��� update�� �Ϸ���� ������ �� �ش� Client �� ©�������.
	bool m_bDbAccess;			//��� ��û�� ����.
	int  m_Buflength;			//Client�� ���� Receive���� ���� ����Ÿ�κ��̴�. ť�� ����ǰ� �����κ��� ����Ų��.
	bool m_QuitSave;			/* when all data saved then set true */
								//Client�� ������ �������� DB ť�� �ִ� ��� �κ��� ����� �ִµ� DB Thread�� Ÿ�� �ȵȴ�.
	int	m_ConnectReq;			// connect request count   ó���� connect���ϰ� �ƹ������� �ȳ��ƿ����� üũ�ϱ����ؼ�.

	BOOL m_bDataInput;
	BOOL m_bKeepAliveSend;

	//User Personnal Info
	sUserList m_MyInfo;

	//RoomInfo
	int m_nRoomNo;
	int m_nRoomInUserNo;
	bool m_bBbangJjang;			//���ӿ��� ����.
	bool m_bRoomMaster;			//���ӿ��� 1��.
	int m_nUserPage;			//User�� ���� ���� �ִ� Page

	//User General
	char m_szPureHeader[3];					//Server���� Client���� �ο��ϴ� Header
	bool m_bPlay;							//Game�� �ѹ��̶� �߾���......�ѹ��� �������� DB�� ������Ʈ�� ���ص� �ǹǷ�.
	int m_nGameMode;
	int	 m_nDBMode;

	int m_nIsLogIn;

	sOption m_sOption;
	

	//Event
	char szSiteCode[5];


	//yun 2004.10.19
	int		m_nCurPtr;
	char	m_szBufferQ[MAX_BUFFER_SIZE];
	UINT	m_nReveillsize; //�޾ƾ��� ����Ÿ ũ��.
	int		m_nToriZisu;
	BigInt	m_biGameRoomMoney;


	//////////////////////////////////////////////////////////////////////////
	///Poker���ӿ��� �ʿ��� ����
	//////////////////////////////////////////////////////////////////////////
	int m_nGameJoin;	//0 : �����, 1 : ������
	int m_nCardCnt;
	int m_nBettingCnt;
	int m_nCardChange;	//ī�� �ٲٴ� ��.
	

	BigInt m_biJackPotMoney;		//���� ��÷�ȸӴ�
	BigInt m_biRoundMoney;			//�� ���忡 �� �ѸӴ�.
	BigInt m_biCallMoney;			//�ڱ����� �ݸӴ�.
	BigInt m_biSideMoney;			//���νÿ� ���� �ɱ� �� ������ ���� �Ӵ�.
	BigInt m_biTempRealRaceMoney;	//�ӽ÷� ���â���� ���� �� ���� �� �Ҿ����� �������ؼ�.
									//�������� ��� ���ڸ��� 0���� �����ϱ� ������...���⵵ �ִ´�.

	BOOL m_bChoice;
	BOOL m_bMyOrder;
	BOOL m_bCheck;					//���� ý�� �߾�����..ī�带 ������ FALSE�� �Ѵ�.
	BOOL m_bCardChange;				//ī�带 ��� ü���� �ߴ��� �˱� ����.
	BOOL m_bChangeState;			//ī�带 �ٲٴ� ��������( �̶� �������� �����ϱ� ���� )
	

	BYTE m_nUserCard[MAX_USER_CARD];
	

	sGameCurUser m_sGameCurUser;
	//sUserSideMoney m_sUserSideMoney;
	CPokerEvalution m_CPokerEvalution;

	void SetBettingBtn( BOOL bCall, BOOL bHalf, BOOL bBing, BOOL bDdaDang, BOOL bCheck, BOOL bDie, BOOL bFull, BOOL bQuater );
	void UserCardChoice( int nCardArrayNo );
	void UserCardDump( int nCardArrayNo );
	void Evalution();
	void DBUpdate( int nWinLose, int nCutTime );  /* nWinLose 0 : �й�, 1:�¸�*/  /* nCutTime 0 : ����, 1: ©������*/
	bool operator>(const CUserDescriptor &rValue)
	{	return (m_CPokerEvalution > rValue.m_CPokerEvalution);	}
	bool operator<(const CUserDescriptor &rValue)
	{	return (m_CPokerEvalution > rValue.m_CPokerEvalution);	}

	//DB Update
	sUserDBUpdateInfo m_sUserDBUpdateInfo;


};

#endif // __USER_DESCRIPTOR_H__
