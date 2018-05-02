#include "CmdProcess.h"
#include "Manager.h"
#include "NGFrame.h"

extern CManager *l_pCManager;
extern CDbQ *g_pDbQ;

void CmdProcess( CUserDescriptor *d, char *pReceBuffer )
{
	int nIndex = 0;

	BYTE bCmd, bErrorCode, bMode;
	sCSHeaderPkt l_sCSHeaderPkt;
	int nSendPacketLen = 0;
	char *pPayLoad;

	nIndex = NGDeFrameHeader( pReceBuffer, &l_sCSHeaderPkt );

	bErrorCode = l_sCSHeaderPkt.cErrorCode;

	bMode = l_sCSHeaderPkt.cMode;
	bCmd = l_sCSHeaderPkt.cCmd;

	if (  ( ( l_sCSHeaderPkt.szHeader[0] != d->m_szPureHeader[0]) ||
		    ( l_sCSHeaderPkt.szHeader[1] != d->m_szPureHeader[1]) ) ||
		    ( bErrorCode != ERROR_NOT ) ) {

		sprintf( a_Log, "(FAIL)\t Packet Client %d실패", bCmd );
		LogFile (a_Log);

		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_PK );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);
		d->m_bClosed = true;
		l_pCManager->CloseSocket(d);

		return;
	}

	//Header를 제외한 곳의 포인터...
	pPayLoad = pReceBuffer + sizeof(sCSHeaderPkt);

	//command
	cmdMode_list[bCmd].Cmd_pt( d, pPayLoad );
}

void IsLogIn( CUserDescriptor *d, char *pReceBuffer )
{

	sprintf( a_Log, "(IsLogInReQ)\t m_Host : %s \t0\tLogIn요청이 왔다", d->m_Host );
	LogFile (a_Log);

	int nSendPacketLen = 0;
	int nIndex = 0;
	char szVer[10];

	memset( szVer, 0x00, sizeof(szVer) );
	memcpy( szVer, &pReceBuffer[nIndex], 10 );
	nIndex += 10;

	if ( strcmp( "VER1.1", szVer ) ) {
		memset( g_ClientSendBuf, 0x00, NORMAL_SEND_BUFFER);
		nSendPacketLen = NGFrameFail( g_ClientSendBuf, d, NGSC_ERROR, ERROR_NOTVER );
		SEND_TO_Q(g_ClientSendBuf, d, nSendPacketLen);
		l_pCManager->SendOutput (d);

		return;
	}

	memcpy( d->m_MyInfo.szUser_Id, &pReceBuffer[nIndex], sizeof(d->m_MyInfo.szUser_Id) );
	nIndex += MAX_ID_LENGTH;

	d->m_nGameJong = MAKEWORD( pReceBuffer[nIndex+1], pReceBuffer[nIndex] );
	nIndex += 2;

	STATE(d) = DBSEARCH_ISLOGIN;
	WAITTING_DB(d) = true;
	g_pDbQ->AddToQ(d);			

	sprintf( a_Log, "(IsLogInReP)\t UserId : %s \t0\t DB Access", d->m_MyInfo.szUser_Id );
	LogFile (a_Log);

	return;
}

void KeepAlive( CUserDescriptor *d, char *pReceBuffer )
{
	sprintf( a_Log, "(KeepAliveReQ)\t UserId : %s \t0\tKeepAlive요청이 왔다", d->m_MyInfo.szUser_Id );
	LogFile (a_Log);

	d->m_IdleTics = 0;
	d->m_bKeepAliveSend = false;
}
