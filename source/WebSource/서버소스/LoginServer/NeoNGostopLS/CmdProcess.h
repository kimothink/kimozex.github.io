#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H


#include "UserDescriptor.h"


void CmdProcess( CUserDescriptor *d, char *pReceBuffer );
void IsLogIn( CUserDescriptor *d, char *pReceBuffer );
void KeepAlive( CUserDescriptor *d, char *pReceBuffer );

//*************************************************
//protcol function pointer 관련.
// 프로토콜의 command 정보를 순차적으로 가지고 있을 command list structure
typedef struct tagCmd {
	BYTE bCmd;						// CmdProcess
	void (*Cmd_pt) ( CUserDescriptor *d, char *pReceBuffer );
} sCmd;


// 서버끼리 또는 관리용 명령어 처리 list, function pointer로 이동

//대기실 모드
const sCmd cmdMode_list[10] = {	
		{NGCS_ISLOGIN, IsLogIn},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NGCS_KEEPALIVE, KeepAlive},
		{NULL, NULL}
};









#endif