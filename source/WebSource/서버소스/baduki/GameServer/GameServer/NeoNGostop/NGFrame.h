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
int NGFrameRoomInOtherSend( char *pBuffer, CUserDescriptor *d, int nNextNo ); 


int NGFrameWaitChat( char *pBuffer, int nUserNo, char *pNickName, int nChatMsgLen, char *pChatMsg );

//ViewPage
int NGFrameViewPage( char *pBuffer, int nUserChannel, int nUserReqPage );

//KeepAlive����.
int NGFrameKeepAlive( char *pBuffer );

int NGFrameClientAllCmt( char *pBuffer, char *pAllSendCmt, CUserDescriptor *d );

int NGFrameIpCheck( char *pBuffer );

int NGFrameGetRoomInfo( char *pBuffer, CUserDescriptor *d, int nRoomNo );

int NGFrameGameChat( char *pBuffer, int nUserNo, char *pNickName, int nChatMsgLen, char *pChatMsg );

//	int NGFrameStartGame( char *pBuffer, CUserDescriptor *d, int nRoomMasterUniqNo, int nBbangJjangUniqNo, int nPrizeDecide, int nPrizeNo );

int NGFrameBadukiStart( char *pBuffer, CUserDescriptor *d, int nRoomMasterUniqNo, int nBbangJjangUniqNo, int nPrizeDecide, int nPrizeNo, int nMasterSlot );

int NGFrameGetUserInfo( char *pBuffer, CUserDescriptor *d );

int NGFrameCardChoice( char *pBuffer, int nUserNo, BYTE nCardNo );

int NGFrameCardDump( char *pBuffer, int nUserNo );

int NGFrameChoiceCardView( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo );

int NGFrameGetOneCard( char *pBuffer, CUserDescriptor *d, int nChannel, int nRoomNo, int nBeforeRoomMasterRoomInUserNo, char cUserSelect = -1 );

int NGFrameEndGame( char *pBuffer, int nChannel, int nRoomNo, int nMasterSlotNo, int nBeforeRoomMasterRoomInUserNo, CUserDescriptor *d, 
				   char cUserSelect, ush_int nJackPot, BigInt biUserJackPotMoney, int *nDrawWinnerUniq, BOOL bDrawGame = FALSE );

int NGFrameSetGameCurUser( char *pBuffer, CUserDescriptor *d, char cBeforeUserSelect, int nCurUserSlotNo );

int NGFrameMyWinGame( char *pBuffer, int nChannel, int nRoomNo, int nMasterSlotNo, CUserDescriptor *d, char cUserSelect  );

int NGFrameNagariGame( char *pBuffer );

int NGFrameStartBtnOnOff( char *pBuffer, CUserDescriptor *d, char cBtnOnOff );	//cBtnOnOff - 0 : Off, 1 : On

int NGFrameGameViewWait( char *pBuffer, int nUserChannel, int nUserReqPage );

int NGFrameUserInvite( char *pBuffer, CUserDescriptor *d, int nInviteMsgLen, char *pInviteMsg );

int NGFrameMyUserJokBo( char *pBuffer, CUserDescriptor *d );

int NGFrameAllUserJokBo( char *pBuffer, int nChannel, int nRoomNo );

int NGFrameGetOutQues( char *pBuffer, CUserDescriptor *d, int nGetOutUserNo );

int NGFrameGetOut( char *pBuffer, CUserDescriptor *d, int nGetOutUserNo );

int NGFrameMemo( char *pBuffer, char *pSendNickName, int nMsgLen, char *pMsg );

int NGFrameCardChange( char *pBuffer, int nUserNo, sUserNewCard *psUserNewCard, BOOL bPass  );

// nRound 0 :��ħ, 1 : ����, 2 : ����
int NGFrameChangeOkAll( char *pBuffer, CUserDescriptor *d, int nRound, char cBeforeUserSelect );

int NGFrameChangeOkUser( char *pBuffer, int nMasterUniqUserNo );

int NGFrameUserChange( char *pBuffer, sUserChange *pUserChange );

//********************************************************
int NGDeFrameHeader( char *pBuffer, sCSHeaderPkt *asCSHeaderPkt );

int NGDeFrmaeChat( char *pBuffer, char *pNickName, int *nUserNo,  int *pChatMsgLen, char *pMsg );

int NGDeFrameUserInvite(  char *pBuffer, int *nUserInviteNo,  int *pInviteMsgLen, char *pInviteMsg );

int NGDeFrmaeMemo( char *pBuffer, char *pSendNickName, char *pRecvNickName, int *pChatMsgLen, char *pMsg );
#endif
