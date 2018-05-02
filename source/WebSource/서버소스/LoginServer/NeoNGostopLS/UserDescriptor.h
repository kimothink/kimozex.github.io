// DescriptorData.h: interface for the CDescriptorData class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __USER_DESCRIPTOR_H__
#define __USER_DESCRIPTOR_H__

#include "common.h"

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
	int	 m_IdleTics;			/* tics idle has no input		*/
	int	 m_nUserMode;
	int	 m_TimeStamp;			/* player: total playing time,  mob: last attacked pulse */
	bool m_bWaitDb;				
	bool m_bUpdateDb;			
	bool m_bDbAccess;			
	int  m_Buflength;			
	bool m_QuitSave;			/* when all data saved then set true */
								
	//int	m_DescNum;				/* unique num assigned to desc		*/
	int	m_ConnectReq;			
	int m_bKeepAliveSend;		


	//User Personnal Info
	sUserList m_MyInfo;
	int m_nIsLogin;

	//User General
	char m_szPureHeader[3];		//Server에서 Client에게 부여하는 Header



	//yun 2004.10.19
	int		m_nCurPtr;
	char	m_szBufferQ[MAX_BUFFER_SIZE];
	UINT	m_nReveillsize; 
	int		m_nGameJong;	



};

#endif // __USER_DESCRIPTOR_H__
