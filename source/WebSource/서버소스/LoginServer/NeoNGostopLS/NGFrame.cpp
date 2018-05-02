#include "NGFrame.h"

int NGFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode )
{
	int nIndex = 0;

	sSCHeaderPkt l_sSCHeaderPkt;

	l_sSCHeaderPkt.szHeader[0] = 'N';
	l_sSCHeaderPkt.szHeader[1] = 'G';
	l_sSCHeaderPkt.nTPacketSize = nPayLoadLength;
	l_sSCHeaderPkt.cCmd = bCmd;
	l_sSCHeaderPkt.cErrorCode = bErrorCode;

	memcpy( pBuffer, &l_sSCHeaderPkt, sizeof(l_sSCHeaderPkt) );
	nIndex += sizeof(l_sSCHeaderPkt);


	return nIndex;	
}

int NGFrameFail( char *pBuffer, CUserDescriptor *d, char cCmd, char cErrorCode  )
{
	int nIndex = 0;
	int nPayLoadLength = SC_HEADER_SIZE;

	nIndex = NGFrameHeader( pBuffer, nPayLoadLength, cCmd, cErrorCode );

	return nIndex;
}

int NGFrameFirst( char *pBuffer, CUserDescriptor *d )
{
	sPKFirst l_sPKFirst;
	int nIndex = 0;
	
	memset( &l_sPKFirst, 0, sizeof(l_sPKFirst));

	nIndex += SC_HEADER_SIZE;

	memcpy( &l_sPKFirst, d->m_szPureHeader, sizeof(d->m_szPureHeader) );
	l_sPKFirst.nUserNo = d->m_MyInfo.l_sUserInfo.nUserNo;

	memcpy( &pBuffer[nIndex], &l_sPKFirst, sizeof(l_sPKFirst) );
	nIndex += sizeof(l_sPKFirst);
	


	NGFrameHeader( pBuffer, nIndex, NGSC_HEADERSEND, ERROR_NOT );

	return nIndex;
}


int NGFrameKeepAlive( char *pBuffer )
{
	int nIndex = SC_HEADER_SIZE;

	NGFrameHeader( pBuffer, nIndex, NGSC_KEEPALIVE, ERROR_NOT );
	
	return nIndex;
}

int NGFrameIsLogIn( char *pBuffer, CUserDescriptor *d )
{
	int nIndex = SC_HEADER_SIZE;


	sIsLogIn l_sIsLogIn;

	char pTemp[256];
//	DTRACE("################  GameJong : %d \n",d->m_nGameJong);
	if(d->m_nGameJong == 0)
		strcpy (pTemp, "################  Login From  MatGo \n");
	if(d->m_nGameJong == 1)
		strcpy (pTemp, "################  Login From  Poker \n");
	if(d->m_nGameJong == 2)
		strcpy (pTemp, "################  Login From Baduki \n");



	puts (pTemp);


	
	memset( &l_sIsLogIn, 0x00, sizeof(l_sIsLogIn) );
	
	memcpy( l_sIsLogIn.szGameServerIp, g_sGameServerInfo[d->m_nGameJong].szGameServerIp, MAX_IP_LEN );
	l_sIsLogIn.nGameServerPort = g_sGameServerInfo[d->m_nGameJong].nGameServerPort;	
	
	memcpy( &pBuffer[nIndex], &l_sIsLogIn, sizeof(l_sIsLogIn) );
	nIndex += sizeof(l_sIsLogIn);
	

	memcpy( &pBuffer[nIndex], &g_sNotice, sizeof(g_sNotice) );
	nIndex += sizeof(g_sNotice);

	NGFrameHeader( pBuffer, nIndex, NGSC_LOGIN, ERROR_NOT );
	
	return nIndex;
}

//***************************************************
int NGDeFrameHeader( char *pBuffer, sCSHeaderPkt *asCSHeaderPkt )
{

	int index = 0;
	memset( asCSHeaderPkt, 0x00, sizeof(sCSHeaderPkt) );

	memcpy( asCSHeaderPkt, pBuffer, sizeof(sCSHeaderPkt) );

	return sizeof(sCSHeaderPkt);

}
