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
	void GameCreate( int nDouble, int nTriple, BigInt biMoney );   //������ ó���� �����Ѵ�.  ī���� ����.


	CHwatu  m_Hwatu;	
	Cfloor m_pFloor;

	BOOL m_bPregident; // �������� �ƴ��� ����
	bool m_bOtherWait;												//�ٸ������ ī�带 �޾Ƶ� �Ǵ����� ����.
	int m_nRound;
	int m_nUserno;
	int m_nSelectCard;												//ī�尡 �ΰ��� �϶� ����.
	int m_nGoStop;													//��� �����̳� ��(1), ����(0)
	int	m_nOtherUserCard;											//�ٸ�������Լ� ���� ī���ȣ

	BYTE *m_pRecvCardPack;											//�濡���ͼ� ����ī����.

	bool m_bMyKetInput;												//������ Űüũ.
	int m_nNineCard;												//������ ���� ����.
	int m_nShake;													//��� ī��..
	bool m_bGameStop;												//������ �������ε� �����������϶�¸޼����� ������ ������ �����.

	int m_nBaseMulti;												//���⸮�ϰ� ���� ���ο����� ������ floor�� basemulti�� 1�� �Ǳ� ������ �̸� ��� ���´�.
	BOOL m_bGameEndWill;											//������ ������ Ȯ���� ��ٸ������̴�..�̶� ������ �������� ó���ϱ����� �̸� �д�.
	
	int m_nPrizeNo;
	bool m_bCardPackService;										// �ѿ��� ��¤�� ī�尡 �������϶� �����и� ���� ������ ���ؼ�..�Ǹ�ó��..
	int m_nLastGo;

	bool Start();													//ī������ ������ Ŭ���̾�Ʈ�鿡�� �����ϰ� �и������ְ� ������ ����

	//*****START()*********
	bool InitCardPack(BYTE *pCard);									//������ ī������ �����´�.
	bool StartDeal();												//�и� ������.
	bool Round();													//������ �����Ѵ�.
	void EndingCheck();

	//**********************
	void AddCardFloor(int nCardNo);									//�ٴڿ� ī�带 �߰��Ѵ�.
	void AddCardUser(int nUserNo, int nCardNo);						//������ ī�带 �߰��Ѵ�.
	void SortAndRePaint( int nSortUserNo );							//�����и� ��Ʈ��Ų��.  nSortUserNo = 4�� ���, �ƴ� �ش�������ȣ
	int EatCardUser(const int nUserNo, const int nCardPos, int &nRecvCardNo);
	// nUserNo : �ش������� ��ȣ, nCardPos : ������ ���� �ٴ����� ��ġ, nCardNo : ������ ���� �ش����� ��(1 - ��, 2 - �����.������ );
	int SelectCard(const int nUserNo, int &anFloorPos, bool &abEat);

	MISSION m_nMissionType;
	int m_nMissionMultiply;
	void InitMission();
	
	//nUserNo : -1�̸� ī���ѿ��� ��¤�°Ŵ�.
	void UpCard(const int nUserNo, const int nPosNo);				//�ش� ī��( ������ �ٴڿ� ����ī��) �� ������ �����´�.
	void ThrowCard(const int nUserNo);								//�ش� ī�带 floor�� ������. ���õ� ī���� ��ġ�� floor�� ��ġ�� CGamer�� �ִ�.
	void ThrowFloorCard( const int nUserNo, const int nCardNo );	//�ش� ī�带 ī���ѿ��� �ٴ����� ������. ���� ī�带 ��¤������..
	
	//�ѿ��� ī�带 ��¤� ������ �ִ��� üũ�Ѵ�. 
	// nCardNo : ��¤�� ī��, 	nUserEatFloorPos : ����ڰ� ���� ī���� ��ġ(������ ������ ������ nUserEatFloorPos : -1 ). 
	// nPackEatFloorPos : �ѿ��� ������ ī�尡 ������ġ.
	bool ThrowFloor(int nSlotNo, const int nCardNo, const int nUserEatFloorPos, int nRoundNo, int &nPackEatFloorPos );

	// �÷ξ�ī�� ����� ���°�� ���õǾ������� ����. 
	int SelectFloorTwoPee(int nSlotNo, int nCardNo, int nRoundNo, int nUserEatFloorPos);

	int IsTwoPos(const int nCardNo, BYTE *retPos);


	void EatMyCardUser(const int nUserNo, const int nCardPos);		//�ڻ��� ī���� ���񽺸� ������.
	bool SortFloor(int &nPregiCardNo);												//�ٴڿ� ���� ������ ī�带 �����Ѵ�.
	void GetPeeEachGamer( int nSlotNo );	//�ٸ�������Լ� �Ǹ� �����´�.
    void GetOnePee( int nSlotNo );          // �ٸ� ������Լ� �Ǹ� �� �� �����´�. // ham 2006.02.22 [*PEE]

	bool SelectNineCardPee(const int nUserNo);						//9���Ǹ� ���Ѵ�.
	void SetActionEvent();											//������� Action�� ��ٸ���.
	void SetOtherActionEvent();										//�ٸ�������� Action�� ��ٸ���.
	int WaitGoStop(const int nUserNo);								//��� ������ ��ٸ���.	0: ����

	void OnUserExitReserv( int nUniQNo, bool bExit );			//�ٸ������ �����ٰ� �����Ѵ�. //true����, false���� ���,


	HANDLE	m_hDrawEvent;							//�̺�Ʈ�� ��ٸ���.
	void EventSync( int nSlotNo, int nEventDivi, int nSoundDivi, int nDelayTime = 50 );
	void DrawEndEvent();
	
	void AddSlowCardFloor(int nCardNo);
	void AddSlowCardUser(int nUserNo, int nCardNo);
	void UpSlowCard(const int nUserNo, const int nPosNo);
	void ThrowSlowCard( const int nUserNo );
	
	//Event
	HANDLE	m_hPaintEvent;										//Draw�� �ٵɶ����� ��ٸ���.
	HANDLE  m_hActionEvent;										//������� �ൿ�� ��ٸ���.
	HANDLE  m_hOtherUserActionEvent;							//�ٸ� ������� �ൿ�� ��ٸ���.

	void	SetPaintEvent();									//�׸��� �� �׷����� �̺�Ʈ�� ����.
	void	Stop();
	void	RoomClear();										//����ڰ� �濡 �尡�ų� ��������.
	void	UserStopInfoUpdate();								//�Ѱ����� ġ�� ���������� ����Ѵ�.
	void DivImage( int nUserSlotNo , char *p_szImage , BOOL bPrevImage ); // �̹��� ������ �޾Ƽ� �и��Ѵ�.
	void InitImageData( int nUserSlotNo ); // g_sImageData �� �ʱ�ȭ�ϴ� �Լ�.

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

	int TempY; // �ʱⰪ ����

	int Count; // ���� ��ȭ��ȣ

	CDXTalk Talk[MAX_CHAT_COUNT];

	CDXChat( int Y = 541 );
	
	virtual ~CDXChat();

	void DrawTalks( int X , int MaxY ); // ����� �׷��ֱ�
	void MoveTalks( void ); // ��ȭ���� �̵�

	void SetText( char *szText, COLORREF rgb = RGB( 0, 0, 0) ); // ��ȭ������ ����ֱ�

	void ClearAll( void ); // ��� ���� �����
};

DWORD WINAPI GostopEngineThread(LPVOID lpParameter);











#endif
