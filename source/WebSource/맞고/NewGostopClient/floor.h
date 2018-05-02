#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "Gamer.h"



class Cfloor
{
public:
	Cfloor();
	virtual ~Cfloor();



	SFLOORCARD m_strCard[MAX_FLOOR_CARD];				//바닥의 카드
	int	m_nPosCnt[MAX_CARD_POS];					//바닥의 카드가 깔린위치에 몇장이 깔렸나.  현재 카드의 pos에 위치한 카드수
	POINT	m_pos[MAX_CARD_POS];					//바닥의 위치 정의.

	int		m_nCardCnt;								//바닥에 뿌릴때 더해진다. 가져가면 감소하겠지??
	int		m_nMasterSlotNo;						//마스터(이긴놈)슬롯.즉...자신이 0번 슬롯...오른쪽이 1, 왼쪽이 2, 슬롯번호대로 배열을 관리한다.  
	int		m_nBbangJjangSlotNo;					//방장의 슬롯.
													//유저넘버는 마스터가 0번. 오른쪽으로 1번, 2번.
	int		m_nWinerNo;								//승자의 유저번호.
	CGamer	m_pGamer[MAX_ROOM_IN];				
	int		m_nPregidentNo;							//대통령이된 유저의 번호.
	int		m_nGetEmptyPos;							//빈곳이 없을때...
	int		m_nBaseMulti;							//나가리 시 다음판 두배.
	bool	m_RealMode;								//진짜 게임인지, 컴이랑 치는지.

	void Init();
	void OnlyAdd(const BYTE nCard);					//바당의 카드를 더해준다.
	int GetEmptyPos();								//바닥의 위치중 빈곳을 찾아본다.  16군데가 다차면 무한루프를 돌 가능성이 있다. 이부분을 다시생각해서 한다.
	int GetCurPosCnt(int nCardCnt);					//바닥의 같은 위치에 뿌려질 카드가 있는지 체크
	int AddCardUser(int nUserNo, int nCardNo);		//처음에 유저에게 카드를 나누어 줄때 사용.
	void SortUserCard(int nUserNo);					//처음 깔리고 나서 패를 소트한다.
	bool IsFloorHas(int nCardNo);					//바닥에 먹을 패가 있는지 조사.
	BYTE CheckNephew();								//서비스 카드가 내패에 있는지 조사한다.
	int FloorEat(int nCardCnt, int nSlotNo);		//해당 카드를 해당 슬롯의 사용자가 먹는다,
	POINT SetPoint(LONG x, LONG y);
	int AppendAdd(int nPosNo, const BYTE nCard);	//바닥에 패를 던질때 위치등 바닥의 패에 더해준다.
	int	GetFIrstPosFromFloorPos(int nFloorPos);		//

	int GetOneEachGamer( int nSlotNo, int nOtherSlotNo, int nGetOtherUserCardCnt ); //다른유저에게서 피를 가져온다.

	//내가 처음 접속하면 이걸 콜해준다.
	void AddMyGamer( int nGamerUserNo, char *pPreImage );		
	void AddRoomInGamer( int nSlot, char *pPreImage );
	int AddOtherGamer( int nSlot, int nUserNo, char *pPreImage );

	//방장일때만 방만들때, 다른사람이 들어올때..증가시켜주고...마지막 게임이 끝나고 
	//확인을 눌렀을때는 서로에게 패킷이 날아가기 때문에 방장과 방장이 아닌사람도 같이 이를 증가시키고
	//대신 새로게임을 시작할때 방장이 아닌사람도 사람이 모두 확인을 눌렀는지 체크한다.
	//게임이 시작되면 방장이던 아니던 모두 이를 0로 세팅한다.
	//void RoomInOutCnt( int nDivi );					//0 : 방을모두 0, 1 : 하나 증가, 2 : 하나감소.

	bool RealGame();								//유저가 들어오면 RealGame이 된다.
	bool RoomUserCntOK();							//유저가 세명이 다 들어왔으면 true이다.

	//컴이랑 치고 있을때 유저가 들어와도 게이머의 돈을 감소 시키면 안되기 때문에..체크한다.
	void UserMoneyVal( int nSlotNo,  BigInt biMoney, bool bPM );	//유저들의 돈 +-
	
	void DeleteUser( int nSlotNo );					//방에서 나감.
	int FindUser( int nGamerUserNo );
	void UserRefresh( int nMasterSlot ); 


	int nTemp;


};













#endif

