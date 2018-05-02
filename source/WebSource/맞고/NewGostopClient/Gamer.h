#ifndef __GAMER_H__
#define __GAMER_H__

#include "ClientDefine.h"

extern HWND g_hWnd;


int GetPeeVal(BYTE nCard);


class Cfloor;

class CGamer
{
public:
	CGamer();
	virtual ~CGamer();


public:

	BYTE m_bCard[MAX_USERCARD_CNT];					//유저가 받은카드			
	int	 m_nCardCnt;								//유저의 받은카드수 또는 가지고 있는 카드수.
	BYTE m_nEatCnt[5];								// [0] : 광먹은수, [1] : 10긋먹은수 , [2] : 5긋먹은수    , 
													// [3] : 실제피먹은수(쌍피 1개 피1개면 2).  , 
													// [4] : 쌍피나 쓰리피 포함 먹은피수(쌍피 1개 피1개면 3) .  단지 숫자만 파악.

	BYTE m_nEatCard[4][30];							// [0] : 광,  [1] : 10긋,  [2] : 5긋 , [3] : 피				무슨카드를 먹었는지 
	bool m_bNineCard;								// 9자 쌍피를 먹었을때 나중에 피로 할것인지..아니 10긋으로 할것인지 결정.
	int  m_nEvalution;								//유저가 먹은 점수.
	Cfloor *m_pfloor;
	bool m_bComputer;								//컴퓨터인지 
	
	int m_nChoiceUserCardPos;						//유저가 선택한 카드
	int m_nChoiceFloorCardPos;						//유저가 먹을 바닥의 카드.
	int  m_nMinWinVal;								//유저가 고나 스톱을 할수 있는 상태..
	BYTE m_arrCard[5];								//폭탄등이 되었을때 그세장을 담을..
	int m_nUserThreeState;							//폭탄, 흔듬.
	int m_nShakeCnt;
	int m_nOldShakeCnt;
	int m_nBbukCnt;									//뻑 카운트
	int m_nMulti;									//유저가 나중에 *2 배 * 3배 표시.
	int m_nGoCnt;
	bool m_bUserEvent;								//유저이벤트가 들어오면 true
	int m_nBombFlipCnt;								// 폭탄했을때 패 그냥 넘기기 카운트 bsw 01.05
	int	m_GetOtherUserCardCnt;						//다른사람에게 몇장씩 가겨올건가...
	int m_nLoseEval;								//유저가 잃은점수.
	BigInt m_biCurWinLoseMoney;							//유저가 현재 방에서 잃고 딴돈.

	//int m_nSlotNo;
	int m_nUserNo;
	int m_nBbukCardNo[10];							//자신이 뻑한카드.

	int m_nResultScore;                             // m_nEvalution 을 받는다.
	//yuno 2005.10.28
	int m_nMissionMultiply;

	bool m_bAlly;									//얼라이..
	int m_nGamerUserNo;								//방에서의 자신번호.
	bool m_bGodori;
	bool m_b3kwang;
	bool m_b4kwang;
	bool m_b5kwang;
	bool m_bHongdan;
	bool m_bChongdan;
	bool m_bGusa;
	bool m_ExitReserv;
	bool m_bMissionEffect;


	BigInt m_biWinMoney;								//유저가 그 판에 딴돈.
	BigInt m_biLoseMoney;								//유저가 그 판에 잃은돈.



	void SetFloor( Cfloor *pfloor );				//바닥포인터를 넘긴다. AutoSelect를 위해.
	void RcvCard(const BYTE nCard);
	void SortUserCard();
	void EatCard(const BYTE nCard);
	int IsPregident();								// 자신의 패가 대통령인지.
	void AutoSelect();								// 카드 위치 자동 선택
	void UserSelect(int nChoiceUserCardPos );		// 유저가 자신의 패를 선택.
	void UserMoneyVal( BigInt biMoney, bool bPM );		// 해당유저의 돈을 차감시킨다. bPM : true +, false : -
	int GetOnePee(int nGetOtherUserCardCnt);		//
	bool Evalutaion();								// 점수계산.
	bool CheckCardIn(int slot, int c1, int c2, int c3, int c4 = -1, int c5 = -1);	//3가지의 카드가 유저가 가지고 있는지.
	void MoveNineCard( bool bLast = false);			//9 쌍피를 피로 선택하면 옮겨준다.
	bool CheckEvalutaion();							//유저가 고나 스톱을 할수 있는지....
	int AddGo();									//고카운트를 늘려준다.
	int GetPPokCardPos();								//폭탄의 피가 있는 위치를 알려준다.
	void Init();
	void RoomInInit();								//유저가 방에 들어왔을때..초기화 할값들...방장도 마찬가지.
	bool CheckMission( MISSION type );
	bool UserBomb( int nChoiceUserCardPos );		//선택된 카드가 폭판인지 선택했을당시에 알아야 한다.


	bool SendPeeEvalutaion();						//한피를 준후 점수계산을 바로한다.

	bool m_bGo;
	bool m_bMmong;
	bool m_bShake;
	bool m_bMission;
	bool m_bPbak; // 피박
	bool m_bKwangbak; // 광박
	bool m_bDokbak; // 독박
	int  m_kwang;

	char m_szUserImage[MAX_IMAGE_CNT][MAX_IMAGE_LEN];
	char m_szPreAvatar[MAX_AVAURLLEN];
};












#endif
