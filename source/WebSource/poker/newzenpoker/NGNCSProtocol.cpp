#include "stdafx.h"

#include "NGNCSProtocol.h"
#include "ClientDefine.h"

//yun 2004.10.19
int NGFramePacket( char *pBuffer, char cHeader1, char cHeader2, int nPayLoadLength, char cMode, char bCmd, char bErrorCode, char *pSendPacket )
{
	//                   √—πŸ¿Ã∆Æ 2(nPayLoadLength)         CMD       errorcode
	int nIndex = 0;

	sCSHeaderPkt l_sCSHeaderPkt;

	l_sCSHeaderPkt.szHeader[0] = cHeader1;
	l_sCSHeaderPkt.szHeader[1] = cHeader2;
	l_sCSHeaderPkt.nTPacketSize = nPayLoadLength + CS_HEADER_SIZE;
	l_sCSHeaderPkt.cMode = cMode;
	l_sCSHeaderPkt.cCmd = bCmd;
	l_sCSHeaderPkt.cErrorCode = bErrorCode;

	memcpy( pBuffer, &l_sCSHeaderPkt, sizeof(l_sCSHeaderPkt) );
	nIndex += sizeof(l_sCSHeaderPkt);

	memcpy( &pBuffer[nIndex], pSendPacket, nPayLoadLength );
	nIndex += nPayLoadLength;

	return nIndex;	
}

int NGTURNFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode )
{
	//                   √—πŸ¿Ã∆Æ 2(nPayLoadLength)         CMD       errorcode
	int nIndex = 0;

	sSCHeaderPkt l_sSCHeaderPkt;

	l_sSCHeaderPkt.szHeader[0] = 'N';
	l_sSCHeaderPkt.szHeader[1] = 'G';
	l_sSCHeaderPkt.nTPacketSize = nPayLoadLength;
	l_sSCHeaderPkt.cCmd = bCmd;
	l_sSCHeaderPkt.cErrorCode = bErrorCode;

	memcpy( &pBuffer[CCPAKETLEN], &l_sSCHeaderPkt, sizeof(l_sSCHeaderPkt) );
	nIndex += sizeof(l_sSCHeaderPkt);


	return nIndex;	
}


//************************************************************************


int NGNCSDeframeHeader( const char *pBuffer, sSCHeaderPkt *asSCHeaderPkt )
{

	int index = 0;

	memcpy( asSCHeaderPkt, pBuffer, sizeof(sSCHeaderPkt) );
	
	return sizeof(sSCHeaderPkt);

}