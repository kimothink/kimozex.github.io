#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H


#include "UserDescriptor.h"
#include "World.h"


void CmdProcess( CUserDescriptor *d, char *pReceBuffer );
void WaitInput( CUserDescriptor *d, char *pReceBuffer );  //처음접속후에 대기실정보를 모두클라이언트에게 Send
void CreateGame( CUserDescriptor *d, char *pReceBuffer ); //방을 만든다.
void RoomOut( CUserDescriptor *d, char *pReceBuffer ); //방에서 나간다.
void RoomIn( CUserDescriptor *d, char *pReceBuffer ); //방에 들어간다.
void QuickJoin( CUserDescriptor *d, char *pReceBuffer ); //방에 들어간다.
void WaitChat( CUserDescriptor *d, char *pReceBuffer );// 대기실에서 채팅을 한다.
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
void UserInvite( CUserDescriptor *d, char *pReceBuffer ); //유저초대.
void GetOutQuesRepl( CUserDescriptor *d, char *pReceBuffer );
void GetOut( CUserDescriptor *d, char *pReceBuffer );
void MemoSend( CUserDescriptor *d, char *pReceBuffer );
void CardChange( CUserDescriptor *d, char *pReceBuffer );
void CardChangeEnd( CUserDescriptor *d, char *pReceBuffer );
void UserChange( CUserDescriptor *d, char *pReceBuffer );


//정상 종료와 기권승 카바.
/* nEndDivi 0 : 정상종료, 1 : 기권승, 2 : 나가리. */
void EndGameProc( CUserDescriptor *d, char cUserSelect, int nEndDivi );	
void EndDrawGame( CUserDescriptor *d, char cUserSelect, int nChannel, int nRoomNo );	
bool UserIpCheck( CUserDescriptor *d, int nRoomNo );
BigInt JackPotCalcu( CRoomList *l_CRoomList, int nMasterSlotNo );
void MyUserJokBoSend( int nChannel, int nRoomNo, CUserDescriptor *d, BOOL bAll = TRUE );
//void JokboSend( int nChannel, int nRoomNo);


// bViewUserPageEx 방에 변화가 있을 때 그 페이지만을 보고 있는 사람들에게 메세지를 날리는데
// 이렇게되면 그페이지를 보고 있지 않은 사람은 유저리스트가 갱신되지 않는다...
// bViewUserPageEx = false이면 그 채널 대기실모두에게 true이면 그 페이지를 보고 있는 사람을 제외한 대기실 모두에게( 게임 페이지를 보고 있는 사람은 유저리스트가 보내지기 때문에. )
void UserWaitRefresh( CUserDescriptor *d, int nUserChannel, char cUserAddMius, int nDelUserNo, bool bViewUserPageEx = false  );  //유저리스트의 변화를 실시간 보내준다.
bool UserCheck( CUserDescriptor *d, char cWorR );  // true : 맞는 패킷, false : 틀린패킷.     cWorR : None(N) 대기실(W) 모드  룸 모드(R) 
bool LimitUserCheck( int nChannel );					// true : 추가 가능. false : 추가 불능.
bool UserIpCheck( CUserDescriptor *d, int nRoomNo );	// 같은 방에서 Ip체크
void BroadCastChannel( char *pSendBuff, int nChannel, int nSendPacketLen, bool bViewUserPageEx, CUserDescriptor *d );	//대기실 모두
void BroadCastUserPage( char *pSendBuff, int nChannel, int nUserPage, int nSendPacketLen ); //대기실중 그 페이지 해당.
void BroadCastRoomInUser( char *pReceBuffer, CUserDescriptor *d, int nChannel, int nRoomNo , int nRoomInUserNo, int nSendLength, bool bSelfEx = false ); //방안에모두 ..기본 : 방안모두포함.


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
		//0x 이기때문에..더이상 명령어를 추가하면 안된다...그럴려면 구조를 좀 바꿔야 한다.
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