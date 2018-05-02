#ifndef __NGNCSPROTOCOL_H
#define __NGNCSPROTOCOL_H


#include "..\BadukiCommonDefine\BadukiCommonDefine.h"


int NGFramePacket( char *pBuffer, char cHeader1, char cHeader2, int nPayLoadLength, char cMode, char bCmd, char bErrorCode, char *pSendPacket );



int NGTURNFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode );











//****************DEFRAME******************************
// identifier(2), payloadlength(4), cmd(1)
int NGNCSDeframeHeader( const char *pBuffer, sSCHeaderPkt *asSCHeaderPkt );












#endif __NGNCSPROTOCOL_H