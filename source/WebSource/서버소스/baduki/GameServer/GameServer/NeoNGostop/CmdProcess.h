#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H


#include "UserDescriptor.h"
#include "World.h"


void CmdProcess( CUserDescriptor *d, char *pReceBuffer );
void WaitInput( CUserDescriptor *d, char *pReceBuffer );  //ó�������Ŀ� ���������� ���Ŭ���̾�Ʈ���� Send
void CreateGame( CUserDescriptor *d, char *pReceBuffer ); //���� �����.
void RoomOut( CUserDescriptor *d, char *pReceBuffer ); //�濡�� ������.
void RoomIn( CUserDescriptor *d, char *pReceBuffer ); //�濡 ����.
void QuickJoin( CUserDescriptor *d, char *pReceBuffer ); //�濡 ����.
void WaitChat( CUserDescriptor *d, char *pReceBuffer );// ���ǿ��� ä���� �Ѵ�.
void ViewPage( CUserDescriptor *d, char *pReceBuffer );
void KeepAlive( CUserDescriptor *d, char *pReceBuffer );
void SetClientOption( CUserDescriptor *d, char *pReceBuffer );
void GetRoomInfo( CUserDescriptor *d, char *pReceBuffer );
void AllCmt( CUserDescriptor *d, char *pReceBuffer );
void GameChat( CUserDescriptor *d, char *pReceBuffer );
void GameStart( CUserDescriptor *d, char *pReceBuffer );
void GetUserInfo( CUserDescriptor *d, char *pReceBuffer );
void CardChoice( CUserDescriptor *d, char *pReceBuffer );
void CardDump( CUserDescriptor *d, char *pReceBuffer );
void UserSelect( CUserDescriptor *d, char *pReceBuffer );
void GameReStart( CUserDescriptor *d, char *pReceBuffer );
void GameViewWait( CUserDescriptor *d, char *pReceBuffer );
void UserInvite( CUserDescriptor *d, char *pReceBuffer ); //�����ʴ�.
void GetOutQuesRepl( CUserDescriptor *d, char *pReceBuffer );
void GetOut( CUserDescriptor *d, char *pReceBuffer );
void MemoSend( CUserDescriptor *d, char *pReceBuffer );
void CardChange( CUserDescriptor *d, char *pReceBuffer );
void CardChangeEnd( CUserDescriptor *d, char *pReceBuffer );
void UserChange( CUserDescriptor *d, char *pReceBuffer );


//���� ����� ��ǽ� ī��.
/* nEndDivi 0 : ��������, 1 : ��ǽ�, 2 : ������. */
void EndGameProc( CUserDescriptor *d, char cUserSelect, int nEndDivi );	
void EndDrawGame( CUserDescriptor *d, char cUserSelect, int nChannel, int nRoomNo );	
bool UserIpCheck( CUserDescriptor *d, int nRoomNo );
BigInt JackPotCalcu( CRoomList *l_CRoomList, int nMasterSlotNo );
void MyUserJokBoSend( int nChannel, int nRoomNo, CUserDescriptor *d, BOOL bAll = TRUE );
//void JokboSend( int nChannel, int nRoomNo);


// bViewUserPageEx �濡 ��ȭ�� ���� �� �� ���������� ���� �ִ� ����鿡�� �޼����� �����µ�
// �̷��ԵǸ� ���������� ���� ���� ���� ����� ��������Ʈ�� ���ŵ��� �ʴ´�...
// bViewUserPageEx = false�̸� �� ä�� ���Ǹ�ο��� true�̸� �� �������� ���� �ִ� ����� ������ ���� ��ο���( ���� �������� ���� �ִ� ����� ��������Ʈ�� �������� ������. )
void UserWaitRefresh( CUserDescriptor *d, int nUserChannel, char cUserAddMius, int nDelUserNo, bool bViewUserPageEx = false  );  //��������Ʈ�� ��ȭ�� �ǽð� �����ش�.
bool UserCheck( CUserDescriptor *d, char cWorR );  // true : �´� ��Ŷ, false : Ʋ����Ŷ.     cWorR : None(N) ����(W) ���  �� ���(R) 
bool LimitUserCheck( int nChannel );					// true : �߰� ����. false : �߰� �Ҵ�.
bool UserIpCheck( CUserDescriptor *d, int nRoomNo );	// ���� �濡�� Ipüũ
void BroadCastChannel( char *pSendBuff, int nChannel, int nSendPacketLen, bool bViewUserPageEx, CUserDescriptor *d );	//���� ���
void BroadCastUserPage( char *pSendBuff, int nChannel, int nUserPage, int nSendPacketLen ); //������ �� ������ �ش�.
void BroadCastRoomInUser( char *pReceBuffer, CUserDescriptor *d, int nChannel, int nRoomNo , int nRoomInUserNo, int nSendLength, bool bSelfEx = false ); //��ȿ���� ..�⺻ : ��ȸ������.


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
		//0x �̱⶧����..���̻� ��ɾ �߰��ϸ� �ȵȴ�...�׷����� ������ �� �ٲ�� �Ѵ�.
		//WaitMode
		{NGCS_WAITINPUT, WaitInput},
		{NGCS_CREATEROOM, CreateGame},
		{NGCS_ROOMIN, RoomIn},
		{NGCS_ROOMOUT, RoomOut},
		{NGCS_QUICKJOIN, QuickJoin},
		{NGCS_WAITCHAT, WaitChat},
		{NGCS_VIEWPAGE, ViewPage},
		{NGCS_ROOMINFO, GetRoomInfo},
		{NGCS_KEEPALIVE, KeepAlive},
		{NGCS_GETUSERINFO, GetUserInfo},
		{NGCS_MEMOSEND, MemoSend},
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
		//GameMode
		{NGCS_GAMECHAT, GameChat},
		{NGCS_GAMESTART, GameStart},
		{NGCS_CARDCHIOCE, CardChoice},
		{NGCS_USERSELECT, UserSelect},
		{NGCS_RESTART, GameReStart},
		{NGCS_VIEWWAIT, GameViewWait},
		{NGCS_USERINVITE, UserInvite},
		{NGCS_CARDDUMP, CardDump},
		{NGCS_GETOUT, GetOut},
		{NGCS_GETOUTQUESREPL, GetOutQuesRepl},
		{NGCS_CARDCHANGE, CardChange},
		{NGCS_CARDCHANGEEND, CardChangeEnd},
		{NGCS_USERCHANGE, UserChange},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL},
		{NULL, NULL}

	},
	{
		{NGCS_SETOPTION, SetClientOption},
		{NULL, NULL},
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