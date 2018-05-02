#ifndef __COMMON_H
#define __COMMON_H

#pragma once

//#undef  FD_SETSIZE	/* MSVC 6 is reported to have 64. */
#define MAX_PLAYING				500			//�ִ� �÷��̾��
#define FD_SETSIZE				MAX_PLAYING
//# endif


#include <afxdisp.h>
#include <winsock2.h>

#include <process.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "..\..\..\BadukiCommonDefine\BadukiCommonDefine.h"
#include "Define.h"
#include "Comfunc.h"
#include "TxtQ.h"

#include "Ado.h"

typedef struct ServerInfo {
	char szDbServerName[20];
	int szServerChannel;
	int nListenPort;
	char szMgrServerName[MAX_SERVERNAME];
	int nMgrPort;
	char szMgrServerIp[MAX_IP_LEN];
	char szIpCheck[20];
} SERVERINFO;


typedef struct strAdoConn
{
	char			m_AdoId[STR_BUFFER];
	char			m_AdoPwd[STR_BUFFER];
	char			m_AdoProvider[STR_BUFFER];
	char			m_AdoCatalog[STR_BUFFER];
	char			m_AdoDataSource[STR_BUFFER];
	char			m_MemberTableName[STR_BUFFER];
	char			m_AdoConStr[TEMP_BUFFER];
} ADOCONN;

typedef struct tagJackPotQ
{
	BigInt biRoomMoney;
	BigInt biJackPotMoney;
} sJackPotQ;

//���� ����.
typedef struct tagJackPotInfo
{
	BigInt biJackPotRoomMoney;		//���ӹ� �⺻�Ӵ�
	BigInt biJackPotMoney;			//���ӹ溰 ���� �Ӵ�.
} sJackPotInfo;


//World g_World;

extern SERVERINFO g_ServerInfo;
extern char *g_ServerPath;
extern char	a_Log[SMALL_BUFSIZE];
extern CTxtBlock *bufpool;	/* pool of large output buffers */
extern char	a_Buf2[MAX_STRING_LENGTH];
extern int buf_overflows;		/* # of overflows of output */
extern int buf_switches;		/* # of switches from small to large buf */
extern int buf_largecount;		/* # of large buffers which exist */
extern struct timeval a_NullTime;	/* zero-valued time structure */
extern char	g_ClientSendBuf[NORMAL_SEND_BUFFER];
extern char	g_ClientSendMidBuf[MID_SEND_BUFFER];
extern char	g_ClientSendLargeBuf[LARGE_SEND_BUFFER];
extern char	g_ClientAllSendBuf[NORMAL_SEND_BUFFER];
extern int	g_ClientAllSendLen;

extern ADOCONN g_AdoConn;
extern char cFinalError;
extern bool g_DbEnd;

#endif
