#ifndef __NGFRAME_H
#define __NGFRAME_H

#include "UserDescriptor.h"


int NGFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode );

// ī���� ������ ������ ���.
int NGCSFrameHeader( char *pBuffer, int nPayLoadLength, char bCmd, char bErrorCode );

//ó�� ���ӽÿ� �������� ����� �����ش�.
int NGFrameFirst( char *pBuffer, CUserDescriptor *d );

//������ ������.
int NGFrameFail( char *pBuffer, CUserDescriptor *d, char cCmd, char cErrorCode  );

//ó�������� ������ ��������� ������.
int NGFrameWaitInput( char *pBuffer, CUserDescriptor *d );

//���ǿ� ������ �߰��ɶ�.( ���ӹ��ϰ� ������� ���� ���������� ) counterServer -> GameServer�� ���ö�.
int NGFrameUserAdd( char *pBuffer, CUserDescriptor *d );

//���ǿ� ������ �����ɶ�.( ���ӹ��ϰ� ������� ���� ���������� ) GameServer -> counterServer�� ���ö�.
int NGFrameUserDel( char *pBuffer, int nDelUserNo );

//�����.
int NGFrameCreateGame( char *pBuffer, int nRoomNo, int nUserChannel );

//cCmdDivi   ( 'C' : RoomCreate(Userlist), 'M' : RoomModify(������ ����)
//             'A' : join    'R' : exit, roomdel�� Client���� ó��), 
//			   'X' : RoomInCloseSoket(�濡������Close),
//			   'G' : Game���� 'W' : ���Ӵ�� 	 )
int NGFrameWaitInfoChange( char *pBuffer, CUserDescriptor *d, int nRoomNo, int nRoomInUserNo, int nUserNo, char cCmdDivi );

//�濡�� ����� ������ ���� �ٸ�����鿡�� �˷��ش�.
int NGFrameRoomOutOtherSend( char *pBuffer, sRoomOutOtherSend *asRoomOutOtherSend );

//�濡 ����� ������ ���»������ ���� �ٸ�������� ������ ������.
int NGFrameRoomIn( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo );

//�濡 ����� ������ ���� �ٸ�����鿡�� �˷��ش�. nNextNo : ���³��� ���� ���� ���°�ΰ�.
int NGFrameRoomInOtherSend( char *pBuffer, CUserDescriptor *d, int nBangJangUniqNo, int nNextNo, int nRoomCurCnt ); 

int NGFrameWaitChat( char *pBuffer, int nUserNo, char *pNickName, int nChatMsgLen, char *pChatMsg );

//ViewPage
int NGFrameViewPage( char *pBuffer, int nUserChannel, int nUserReqPage );

//KeepAlive����.
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
