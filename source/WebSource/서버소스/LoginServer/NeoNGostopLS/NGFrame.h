#ifndef __NGFRAME_H
#define __NGFRAME_H

#include "UserDescriptor.h"


int NGFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode );

int NGFrameFirst( char *pBuffer, CUserDescriptor *d );
//에러를 날린다.
int NGFrameFail( char *pBuffer, CUserDescriptor *d, char cCmd, char cErrorCode  );


//KeepAlive정보.
int NGFrameKeepAlive( char *pBuffer );

int NGFrameIsLogIn( char *pBuffer, CUserDescriptor *d );

//********************************************************
int NGDeFrameHeader( char *pBuffer, sCSHeaderPkt *asCSHeaderPkt );


#endif
