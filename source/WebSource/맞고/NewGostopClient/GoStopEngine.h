#ifndef __GOSTOP_ENGINE_H__
#define __GOSTOP_ENGINE_H__





#include "Hwatu.h"
#include "floor.h"



static unsigned long seed;

#define	m  (unsigned long)2147483647
#define	q  (unsigned long)127773
#define	a (unsigned int)16807
#define	r (unsigned int)2836


void ServerSrandom(unsigned long initial_seed);
unsigned long ServerRandom(void);
int GetRandom(int from, int to);



class CGoStopEngine
{

public:
	
	CGoStopEngine();
	virtual ~CGoStopEngine();
	int m_nDouble;
	int m_nTriple;
	BigInt m_biMoney;
	DWORD	m_dwGEThreadId;
	HANDLE  m_hGEThread;
	BOOL	m_bMasterMode;
	BYTE	m_pCardPack[MAX_CARDPACK_CNT];
	HWND	m_hMainWnd;
	BOOL	m_bPlaying;
	
	void SetWndHandle( HWND aHwnd );
	void GameCreate( int nDouble, int nTriple, BigInt biMoney );   //게임을 처음에 생성한다.  카드팩 생성.


	CHwatu  m_Hwatu;	
	Cfloor m_pFloor;

	BOOL m_bPregident; // 총통인지 아닌지 구별
	bool m_bOtherWait;												//다른사람의 카드를 받아도 되는지의 여부.
	int m_nRound;
	int m_nUserno;
	int m_nSelectCard;												//카드가 두개를 일때 선택.
	int m_nGoStop;													//고냐 스톱이냐 고(1), 스톱(0)
	int	m_nOtherUserCard;											//다른사람에게서 받은 카드번호

	BYTE *m_pRecvCardPack;											//방에들어와서 받은카드팩.

	bool m_bMyKetInput;												//유저의 키체크.
	int m_nNineCard;												//국진을 따로 뺀다.
	int m_nShake;													//흔든 카드..
	bool m_bGameStop;												//게임이 진행중인데 게임을시작하라는메세지가 들어오면 게임을 멈춘다.

	int m_nBaseMulti;												//나기리하고 나서 새로운판이 끝나면 floor에 basemulti가 1로 되기 떄문에 이를 담아 놓는다.
	BOOL m_bGameEndWill;											//게임이 끝나고 확인을 기다리는중이다..이때 소켓이 끊어지면 처리하기위해 이를 둔다.
	
	int m_nPrizeNo;
	bool m_bCardPackService;										// 팩에서 뒤짚은 카드가 서비스패일때 다음패를 빨리 뒤집기 위해서..피망처럼..
	int m_nLastGo;

	bool Start();													//카드팩을 각각의 클라이언트들에게 전달하고 패를나워주고 게임을 시작

	//*****START()*********
	bool InitCardPack(BYTE *pCard);									//생성돤 카드팩을 가져온더.
	bool StartDeal();												//패를 돌린다.
	bool Round();													//게임을 진행한다.
	void EndingCheck();

	//**********************
	void AddCardFloor(int nCardNo);									//바닥에 카드를 추가한다.
	void AddCardUser(int nUserNo, int nCardNo);						//유저의 카드를 추가한다.
	void SortAndRePaint( int nSortUserNo );							//돌린패를 소트시킨다.  nSortUserNo = 4면 모두, 아님 해당유저번호
	int EatCardUser(const int nUserNo, const int nCardPos, int &nRecvCardNo);
	// nUserNo : 해당유저의 번호, nCardPos : 유저가 먹을 바닥패의 위치, nCardNo : 유저가 먹은 해당패의 월(1 - 솔, 2 - 사쿠라.ㅋㅋㅋ );
	int SelectCard(const int nUserNo, int &anFloorPos, bool &abEat);

	MISSION m_nMissionType;
	int m_nMissionMultiply;
	void InitMission();
	
	//nUserNo : -1이면 카드팩에서 뒤짚는거다.
	void UpCard(const int nUserNo, const int nPosNo);				//해당 카드( 유저가 바닥에 던질카드) 의 유저를 뒤집는다.
	void ThrowCard(const int nUserNo);								//해당 카드를 floor로 보낸다. 선택된 카드의 위치와 floor의 위치는 CGamer에 있다.
	void ThrowFloorCard( const int nUserNo, const int nCardNo );	//해당 카드를 카드팩에서 바닥으로 보낸다. 서비스 카드를 뒤짚었을때..
	
	//팩에서 카드를 뒤짚어서 먹을게 있는지 체크한다. 
	// nCardNo : 뒤짚은 카드, 	nUserEatFloorPos : 사용자가 먹은 카드의 위치(유저가 먹은게 없으면 nUserEatFloorPos : -1 ). 
	// nPackEatFloorPos : 팩에서 뒤집은 카드가 놓일위치.
	bool ThrowFloor(int nSlotNo, const int nCardNo, const int nUserEatFloorPos, int nRoundNo, int &nPackEatFloorPos );

	// 플로어카드 어레이중 몇번째가 선택되었는지를 리턴. 
	int SelectFloorTwoPee(int nSlotNo, int nCardNo, int nRoundNo, int nUserEatFloorPos);

	int IsTwoPos(const int nCardNo, BYTE *retPos);


	void EatMyCardUser(const int nUserNo, const int nCardPos);		//자산의 카드중 서비스를 먹을때.
	bool SortFloor(int &nPregiCardNo);												//바닥에 같은 종류의 카드를 정리한다.
	void GetPeeEachGamer( int nSlotNo );	//다른사람에게서 피를 가져온다.
    void GetOnePee( int nSlotNo );          // 다른 사람에게서 피를 한 장 가져온다. // ham 2006.02.22 [*PEE]

	bool SelectNineCardPee(const int nUserNo);						//9쌍피를 택한다.
	void SetActionEvent();											//사용자의 Action을 기다린다.
	void SetOtherActionEvent();										//다른사용자의 Action을 기다린다.
	int WaitGoStop(const int nUserNo);								//고냐 스톱을 기다린다.	0: 스톱

	void OnUserExitReserv( int nUniQNo, bool bExit );			//다른사람이 나간다고 예약한다. //true나감, false나감 취소,


	HANDLE	m_hDrawEvent;							//이벤트를 기다린다.
	void EventSync( int nSlotNo, int nEventDivi, int nSoundDivi, int nDelayTime = 50 );
	void DrawEndEvent();
	
	void AddSlowCardFloor(int nCardNo);
	void AddSlowCardUser(int nUserNo, int nCardNo);
	void UpSlowCard(const int nUserNo, const int nPosNo);
	void ThrowSlowCard( const int nUserNo );
	
	//Event
	HANDLE	m_hPaintEvent;										//Draw가 다될때까지 기다린다.
	HANDLE  m_hActionEvent;										//사용자의 행동을 기다린다.
	HANDLE  m_hOtherUserActionEvent;							//다른 사용자의 행동을 기다린다.

	void	SetPaintEvent();									//그림을 다 그렸으면 이벤트를 해제.
	void	Stop();
	void	RoomClear();										//사용자가 방에 드가거나 나왔을때.
	void	UserStopInfoUpdate();								//한게임을 치고 유저정보를 계산한다.
	void DivImage( int nUserSlotNo , char *p_szImage , BOOL bPrevImage ); // 이미지 정보를 받아서 분리한다.
	void InitImageData( int nUserSlotNo ); // g_sImageData 를 초기화하는 함수.

	int CheckReGetSprite( int nUserSlotNo );

	void WaitRoomModeSet();
	void GameModeSet();
	void ChannelModeSet();
	void SoundEffectDivi( int nGoCnt, int nSoundDivi );
	BOOL GameThreadCK();
	BigInt GameSystemMoney( BigInt biUserMoney );

};


#define MAX_CHAT_COUNT 25

class CDXChat
{
private:

	class CDXTalk
	{
	private:
		int Y;
		char szChat[255];

		BOOL bShow;
		COLORREF rgb; 

	public:

		friend class CDXChat;
	};	
		
public:

	int TempY; // 초기값 저장

	int Count; // 현재 대화번호

	CDXTalk Talk[MAX_CHAT_COUNT];

	CDXChat( int Y = 541 );
	
	virtual ~CDXChat();

	void DrawTalks( int X , int MaxY ); // 내용들 그려주기
	void MoveTalks( void ); // 대화내용 이동

	void SetText( char *szText, COLORREF rgb = RGB( 0, 0, 0) ); // 대화내용을 집어넣기

	void ClearAll( void ); // 모든 내용 지우기
};

DWORD WINAPI GostopEngineThread(LPVOID lpParameter);











#endif
