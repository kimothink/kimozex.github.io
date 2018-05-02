#ifndef __POKERENGINE_H__
#define __POKERENGINE_H__


#include "CardPack.h"

class CPokerEngine {

public:

	CPokerEngine();
	virtual ~CPokerEngine();



	CCardPack *m_pCardPack;


	void CreateGame( int nCardCnt );
	void StartDeal( int nChannel, int nRoomNo );
	void UserInit( int nChannel, int nRoomNo, int nRoomInUserNo = -1, BOOL bSpecifyUser = FALSE );
	int ResetMaster( int nChannel, int nRoomNo );
	void UserCardDeal( int nChannel, int nRoomNo );
	void PokerCalcuRaceMoney( int nChannel, int nRoomNo, int nSetUserArrayNo );
	void PokerBtnReset( CUserDescriptor *d );			//��Ŀ ��ư Disable


	BOOL UserMade( CUserDescriptor *d );
	BOOL PokerBtnSet( int nChannel, int nRoomNo, int nSetUserArrayNo );
	void LowRaceCalCu( BettingHistory aBettingHistory, CUserDescriptor *d );
	//////////////////////////////////////////////////////////////////////////


	int GameVerify( int nChannel, int nRoomNo );		//�� �׾����� �ƴϸ� �Ѹ� ���Ҵ���.


};










#endif