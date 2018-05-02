#ifndef __POKERENGINE_H__
#define __POKERENGINE_H__


#include "CardPack.h"






class CPokerEngine {

public:

	CPokerEngine();
	virtual ~CPokerEngine();



	CCardPack *m_pCardPack;
	CChngeCardPack m_pChangeCardPack;

	void CreateGame( int nCardCnt, int nPackCnt );
	void StartDeal( int nChannel, int nRoomNo );
	void UserInit( int nChannel, int nRoomNo, int nRoomInUserNo = -1, BOOL bSpecifyUser = FALSE );
	int ResetMaster( int nChannel, int nRoomNo );
	void UserCardDeal( int nChannel, int nRoomNo );
	void PokerCalcuRaceMoney( int nChannel, int nRoomNo, int nSetUserArrayNo );
	void PokerBtnReset( CUserDescriptor *d );			//포커 버튼 Disable
	void CardChange( CUserDescriptor *d, sUserCardChange *psUserCardChange, sUserNewCard *psUserNewCard );

	void NewRound( int nChannel, int nRoomNo );

	BOOL UserMade( CUserDescriptor *d );
	BOOL PokerBtnSet( int nChannel, int nRoomNo, int nSetUserArrayNo );
	void LowRaceCalCu( BettingHistory aBettingHistory, CUserDescriptor *d );


	int GameVerify( int nChannel, int nRoomNo );
	int GameRanking( int nChannel, int nRoomNo );

};










#endif