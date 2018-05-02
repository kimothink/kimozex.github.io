#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H


#include "UserDescriptor.h"
#include "World.h"


void CmdProcess( CUserDescriptor *d, char *pReceBuffer );
void WaitInput( CUserDescriptor *d, char *pReceBuffer );  
void CreateGame( CUserDescriptor *d, char *pReceBuffer ); //���� �����.
void ExitGame( CUserDescriptor *d, char *pReceBuffer ); //���ǿ��� ������ ������.
void RoomOut( CUserDescriptor *d, char *pReceBuffer ); //�濡�� ������.
void RoomIn( CUserDescriptor *d, char *pReceBuffer ); //�濡 ����.
void RoomView( CUserDescriptor *d, char *pReceBuffer ); //����



void QuickJoin( CUserDescriptor *d, char *pReceBuffer ); //�濡 ����.
void WaitChat( CUserDescriptor *d, char *pReceBuffer );// ���ǿ��� ä���� �Ѵ�.
void ViewPage( CUserDescriptor *d, char *pReceBuffer );
void KeepAlive( CUserDescriptor *d, char *pReceBuffer );
void TurnMsg( CUserDescriptor *d, char *pReceBuffer ); //Client���� �� Packet�� ����� ��� Ŭ���̾�Ʈ���� �����ش�.
void GameEnd( CUserDescriptor *d, char *pReceBuffer ); //�Ѱ����� ������.
void GameViewWait( CUserDescriptor *d, char *pReceBuffer ); //���ӹ濡�� ���� ������ ����.
void UserInvite( CUserDescriptor *d, char *pReceBuffer ); //�����ʴ�.
void GamingExitUser( CUserDescriptor *d, char *pReceBuffer ); //������ ������ ��������...������ ������Ʈ ���ְ�. �濡�� ���� ó���� ���ش�.
void MemoSend( CUserDescriptor *d, char *pReceBuffer );
void SetClientOption( CUserDescriptor *d, char *pReceBuffer );
void GameReady( CUserDescriptor *d, char *pReceBuffer );
void GetAvatarUrl( CUserDescriptor *d, char *pReceBuffer );
void AllCmt( CUserDescriptor *d, char *pReceBuffer );
void GameSunSet( CUserDescriptor *d, char *pReceBuffer );
void SunChoice( CUserDescriptor *d, char *pReceBuffer );
void AutoChoice( CUserDescriptor *d, char *pReceBuffer );
void GetUserInfo( CUserDescriptor *d, char *pReceBuffer );
void GetRoomInfo( CUserDescriptor *d, char *pReceBuffer );

void UserWaitRefresh( CUserDescriptor *d, int nUserChannel, char cUserAddMius, int nDelUserNo, bool bViewUserPageEx = false  );  //��������Ʈ�� ��ȭ�� �ǽð� �����ش�.
bool UserCheck( CUserDescriptor *d, char cWorR );  // true : �´� ��Ŷ, false : Ʋ����Ŷ.     cWorR : None(N) ����(W) ���  �� ���(R) 
bool LimitUserCheck( int nChannel );					// true : �߰� ����. false : �߰� �Ҵ�.
bool DataCheck( int nData, int nFrom, int nTo );
bool UserIpCheck( CUserDescriptor *d, int nRoomNo );	// ���� �濡�� Ipüũ
bool UserIpCheckAdmin( CUserDescriptor *d, int nRoomNo );	//������ IP üũ

void BroadCastChannel( char *pSendBuff, int nChannel, int nSendPacketLen, bool bViewUserPageEx, CUserDescriptor *d );	//���� ���
void BroadCastUserPage( char *pSendBuff, int nChannel, int nUserPage, int nSendPacketLen ); //������ �� ������ �ش�.
void BroadCastRoomInUser( char *pReceBuffer, CUserDescriptor *d, int nChannel, int nRoomNo , int nRoomInUserNo, int nSendLength, bool bSelfEx = false ); //��ȿ���� ..�⺻ : ��ȸ������.
void BroadCastGameViewWait( char *pSendBuff, int nChannel, int nSendPacketLen );	//���ӹ濡�� ���� �����ִ»����.


CUserDescriptor *FindUser( int nUserNo, int nChannel );

//*************************************************
//protcol function pointer ����.
// ���������� command ������ ���������� ������ ���� command list structure
typedef struct tagCmd {
	BYTE bCmd;						// CmdProcess
	void (*Cmd_pt) ( CUserDescriptor *d, char *pReceBuffer );
} sCmd;


// �������� �Ǵ� ������ ��ɾ� ó�� list, function pointer�� �̵�

//���� ���
const sCmd cmdMode_list[][20] = {

	{
		{NGCS_WAITINPUT, WaitInput},
		{NGCS_CREATEROOM, CreateGame},
		{NGCS_ROOMIN, RoomIn},
		{NGCS_ROOMOUT, RoomOut},
		{NGCS_QUICKJOIN, QuickJoin},
		{NGCS_WAITCHAT, WaitChat},
		{NGCS_VIEWPAGE, ViewPage},
		{NGCS_EXITGAME, ExitGame},
		{NGCS_KEEPALIVE, KeepAlive},
		{NGCS_GETAVAURL, GetAvatarUrl},
		{NGCS_GETUSERINFO, GetUserInfo},
		{NGCS_ROOMINFO, GetRoomInfo},
		{NGCS_ADMINJUSTVIEW, RoomView},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL}
	},
	{
		//GameMode
		{NGCS_TURNMSG, TurnMsg},
		{NGCS_GAMEEND, GameEnd},
		{NULL, NULL},
		{NGCS_VIEWWAIT, GameViewWait},
		{NGCS_USERINVITE, UserInvite},
		{NGCS_GAMINGEXITUSER, GamingExitUser},
		{NGCS_STARTREADY, GameReady},
		{NGCS_GAMESUNSET, GameSunSet},
		{NGCS_SUNCHOICE, SunChoice},
		{NGCS_AUTOCHOICE, AutoChoice},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL}
	},
	{
		{NGCS_MEMOSEND, MemoSend},
		{NGCS_SETOPTION, SetClientOption},
		{NGCS_ALLCMT, AllCmt},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL}
	}
};















#endif