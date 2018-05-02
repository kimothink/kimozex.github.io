#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H


#include "UserDescriptor.h"


void CmdProcess( CUserDescriptor *d, char *pReceBuffer );
void IsLogIn( CUserDescriptor *d, char *pReceBuffer );
void KeepAlive( CUserDescriptor *d, char *pReceBuffer );

//*************************************************
//protcol function pointer ����.
// ���������� command ������ ���������� ������ ���� command list structure
typedef struct tagCmd {
	BYTE bCmd;						// CmdProcess
	void (*Cmd_pt) ( CUserDescriptor *d, char *pReceBuffer );
} sCmd;


// �������� �Ǵ� ������ ��ɾ� ó�� list, function pointer�� �̵�

//���� ���
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