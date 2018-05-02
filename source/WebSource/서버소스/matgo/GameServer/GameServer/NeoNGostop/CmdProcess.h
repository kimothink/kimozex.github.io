#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H


#include "UserDescriptor.h"
#include "World.h"


void CmdProcess( CUserDescriptor *d, char *pReceBuffer );
void WaitInput( CUserDescriptor *d, char *pReceBuffer );  
void CreateGame( CUserDescriptor *d, char *pReceBuffer ); //방을 만든다.
void ExitGame( CUserDescriptor *d, char *pReceBuffer ); //대기실에서 게임을 나간다.
void RoomOut( CUserDescriptor *d, char *pReceBuffer ); //방에서 나간다.
void RoomIn( CUserDescriptor *d, char *pReceBuffer ); //방에 들어간다.
void RoomView( CUserDescriptor *d, char *pReceBuffer ); //관전



void QuickJoin( CUserDescriptor *d, char *pReceBuffer ); //방에 들어간다.
void WaitChat( CUserDescriptor *d, char *pReceBuffer );// 대기실에서 채팅을 한다.
void ViewPage( CUserDescriptor *d, char *pReceBuffer );
void KeepAlive( CUserDescriptor *d, char *pReceBuffer );
void TurnMsg( CUserDescriptor *d, char *pReceBuffer ); //Client에서 온 Packet를 방안의 모든 클라이언트에게 보내준다.
void GameEnd( CUserDescriptor *d, char *pReceBuffer ); //한게임이 끝나고.
void GameViewWait( CUserDescriptor *d, char *pReceBuffer ); //게임방에서 대기실 유저를 볼때.
void UserInvite( CUserDescriptor *d, char *pReceBuffer ); //유저초대.
void GamingExitUser( CUserDescriptor *d, char *pReceBuffer ); //게임중 유저가 나갔을때...유저를 업데이트 해주고. 방에서 나감 처리를 해준다.
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

void UserWaitRefresh( CUserDescriptor *d, int nUserChannel, char cUserAddMius, int nDelUserNo, bool bViewUserPageEx = false  );  //유저리스트의 변화를 실시간 보내준다.
bool UserCheck( CUserDescriptor *d, char cWorR );  // true : 맞는 패킷, false : 틀린패킷.     cWorR : None(N) 대기실(W) 모드  룸 모드(R) 
bool LimitUserCheck( int nChannel );					// true : 추가 가능. false : 추가 불능.
bool DataCheck( int nData, int nFrom, int nTo );
bool UserIpCheck( CUserDescriptor *d, int nRoomNo );	// 같은 방에서 Ip체크
bool UserIpCheckAdmin( CUserDescriptor *d, int nRoomNo );	//관리자 IP 체크

void BroadCastChannel( char *pSendBuff, int nChannel, int nSendPacketLen, bool bViewUserPageEx, CUserDescriptor *d );	//대기실 모두
void BroadCastUserPage( char *pSendBuff, int nChannel, int nUserPage, int nSendPacketLen ); //대기실중 그 페이지 해당.
void BroadCastRoomInUser( char *pReceBuffer, CUserDescriptor *d, int nChannel, int nRoomNo , int nRoomInUserNo, int nSendLength, bool bSelfEx = false ); //방안에모두 ..기본 : 방안모두포함.
void BroadCastGameViewWait( char *pSendBuff, int nChannel, int nSendPacketLen );	//게임방에서 대기실 보고있는사람들.


CUserDescriptor *FindUser( int nUserNo, int nChannel );

//*************************************************
//protcol function pointer 관련.
// 프로토콜의 command 정보를 순차적으로 가지고 있을 command list structure
typedef struct tagCmd {
	BYTE bCmd;						// CmdProcess
	void (*Cmd_pt) ( CUserDescriptor *d, char *pReceBuffer );
} sCmd;


// 서버끼리 또는 관리용 명령어 처리 list, function pointer로 이동

//대기실 모드
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