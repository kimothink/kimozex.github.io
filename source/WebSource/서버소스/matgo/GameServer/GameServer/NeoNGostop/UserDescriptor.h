// DescriptorData.h: interface for the CDescriptorData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __USER_DESCRIPTOR_H__
#define __USER_DESCRIPTOR_H__

#include "common.h"
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
	ush_int m_nPrizeNo;
	ush_int m_nSucceedCode;				// 0 �̸� ����, 1�̸� ����	

	int		m_nCurPtr;
	char	m_szBufferQ[MAX_BUFFER_SIZE];
	UINT	m_nReveillsize; //�޾ƾ��� ����Ÿ ũ��.
	int		m_nToriZisu;
	BigInt	m_biGameRoomMoney;
	BigInt	m_biSystemMoney;

	char m_szOtherNick_Name[MAX_ROOM_IN-1][MAX_NICK_LENGTH];	//������ ģ ����г���.


	sUserDBUpdateInfo m_sUserDBUpdateInfo;
};

#endif // __USER_DESCRIPTOR_H__
