#ifndef __NGFRAME_H
#define __NGFRAME_H

#include "UserDescriptor.h"


int NGFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode );

// 카운터 서버에 날릴때 헤더.
int NGCSFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode );

//처음 접속시에 서버에서 헤더를 보내준다.
int NGFrameFirst( char *pBuffer, CUserDescriptor *d );

//에러를 날린다.
int NGFrameFail( char *pBuffer, CUserDescriptor *d, char cCmd, char cErrorCode  );

//처음접속이 대기실의 모든정보를 날린다.
int NGFrameWaitInput( char *pBuffer, CUserDescriptor *d );

//대기실에 유저가 추가될때.( 게임방하고 상관없이 순수 유저리스만 ) counterServer -> GameServer로 들어올때.
int NGFrameUserAdd( char *pBuffer, CUserDescriptor *d );

//대기실에 유저가 삭제될때.( 게임방하고 상관없이 순수 유저리스만 ) GameServer -> counterServer로 들어올때.
int NGFrameUserDel( char *pBuffer, int nDelUserNo );

//방생성.
int NGFrameCreateGame( char *pBuffer, int nRoomNo, int nUserChannel );

//cCmdDivi   ( 'C' : RoomCreate(Userlist), 'M' : RoomModify(방정보 변걍)
//             'A' : join    'R' : exit, roomdel은 Client에서 처리), 
//			   'X' : RoomInCloseSoket(방에서접속Close),
//			   'G' : Game시작 'W' : 게임대기 	 )
int NGFrameWaitInfoChange( char *pBuffer, CUserDescriptor *d, int nRoomNo, int nRoomInUserNo, int nUserNo, char cCmdDivi );

//방에서 사람이 나가면 방의 다른사람들에게 알려준다.
int NGFrameRoomOutOtherSend( char *pBuffer, sRoomOutOtherSend *asRoomOutOtherSend );

//방에 사람이 들어오면 들어온사람에게 방의 다른사람들의 정보를 보낸다.
int NGFrameRoomIn( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo );

//방에 사람이 들어오면 방의 다른사람들에게 알려준다. nNextNo : 들어온놈이 방장 다음 몇번째인가.
int NGFrameRoomInOtherSend( char *pBuffer, CUserDescriptor *d, int nBangJangUniqNo, int nNextNo, int nRoomCurCnt ); 

int NGFrameWaitChat( char *pBuffer, int nUserNo, char *pNickName, int nChatMsgLen, char *pChatMsg );

//ViewPage
int NGFrameViewPage( char *pBuffer, int nUserChannel, int nUserReqPage );

//KeepAlive정보.
int NGFrameKeepAlive( char *pBuffer );

int NGFrmaeResultUpdateEnd( char *pBuffer ); 

int NGFrameGameViewWait( char *pBuffer, int nUserChannel, int nUserReqPage );

int NGFrameUserInvite( char *pBuffer, CUserDescriptor *d, int nInviteMsgLen, char *pInviteMsg );

int NGFrameGameingExiUser( char *pBuffer, CUserDescriptor *d );

int NGFrameMemo( char *pBuffer, char *pSendNickName, int nMsgLen, char *pMsg );

int NGFrameGameStart( char *pBuffer );

int NGFrameGetAvatarUrl( char *pBuffer, CUserDescriptor *d );

int NGFrameClientAllCmt( char *pBuffer, char *pAllSendCmt, CUserDescriptor *d );

int NGFrameIpCheck( char *pBuffer );

int NGFrameGameSunSet( char *pBuffer, BYTE *pSunPack );

int NGFrameChioceCardNo( char *pBuffer, int nChoiceNo, int nUniqUserNo );

int NGFrameSunDecide( char *pBuffer, int nSunUniqNo );

int NGFrameReChioceCard( char *pBuffer );

int NGFrameGetUserInfo( char *pBuffer, CUserDescriptor *d );

int NGFrameGetRoomInfo( char *pBuffer, CUserDescriptor *d, int nRoomNo );

int NGFrameSetVoice( char *pBuffer, CUserDescriptor *d );

//int NGFrameComStartGame( char *pBuffer, CUserDescriptor *d );

int NGFrameRealStartGame( char *pBuffer, CUserDescriptor *d );
//********************************************************
int NGDeFrameHeader( char *pBuffer, sCSHeaderPkt *asCSHeaderPkt );
int NGDeFrmaeChat( char *pBuffer, char *pNickName, int *nUserNo,  int *pChatMsgLen, char *pMsg );
int NGDeFrameUserInvite(  char *pBuffer, int *nUserInviteNo,  int *pInviteMsgLen, char *pInviteMsg );
int NGDeFrmaeMemo( char *pBuffer, char *pSendNickName, char *pRecvNickName, int *pChatMsgLen, char *pMsg );

#endif
