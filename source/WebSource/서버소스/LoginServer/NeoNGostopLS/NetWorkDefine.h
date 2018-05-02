#ifndef _NETWORK_DEFINE_H
#define _NETWORK_DEFINE_H
//7f �����ϸ� �Ѿ����~~~~

#pragma pack( push, 1)

typedef unsigned short int	ush_int;

typedef UINT	BigInt;		//21�����....

//****************************************************************************************
//Protocol ����.
//****************************************************************************************

#define MAX_NICK_LENGTH				21
#define MAX_AVAURLLEN				20									//�ƹ�Ÿ Url����.
#define MAX_ID_LENGTH				50
#define	MAX_IP_LEN					30


//S->C
typedef struct tagCSHeaderPkt
{
	char 	szHeader[2];			//'N''G'
	ush_int	nTPacketSize;
	char	cCmd;
	char	cErrorCode;
} sSCHeaderPkt;


//C -> S HEADER  
typedef struct tagSCHeaderPkt
{ 
	char	szHeader[2];
	ush_int nTPacketSize;
	char	cMode;				//0 : GameMode,   1 : PlayMode
	char	cCmd;
	char	cErrorCode;
} sCSHeaderPkt;


#define SC_HEADER_SIZE			sizeof(sSCHeaderPkt)   // NGHeader(2) + packetsize(2) + cmd(1) + errorcode(1)
#define SC_PKTLEN_POS			2



//Client -> Server
//��������� ������ ��¿�� ����.
#define NGCS_ISLOGIN				0		//Client�� LogInServer�� �����Ҷ� ���� Command
#define NGCS_KEEPALIVE				8		//KeepAlive

//LogInServer���� �������� Ư���� �����´�.
#define NGSC_HEADERSEND				0x30		//ó�� Client���ӽ� ������ Header�� �����ش�.
#define NGSC_ERROR					0x31		//Error
#define NGSC_LOGIN					0x32		//LogInServer���� ������.
#define ERROR_NOT					0x33			
#define ERROR_PK					0x34		//packeterror
#define ERROR_NOTVER				0x35		//������ �ٸ���.
#define ERROR_ACCESS_DBFAIL			0x36		//��� �������� ����.
#define ERROR_MULTILOGIN			0x37		//�ߺ��α���.
#define NGSC_KEEPALIVE				0x38		//KeepAlive�� �������� ���� ������.





typedef struct tagUserInfo	//Ŭ���̾�Ʈ���� �����ִ� ���� ����
{
	char szNick_Name[MAX_NICK_LENGTH];	//�����г���.
	BigInt	nbUserMoney;				//������ ��.
	ush_int nClass;						//���
	BigInt	nbMaxMoney;					//���ݲ� �ְ�� ���� ����.
	float	fWinRate;					//�·�.
	ush_int	nWin_Num;					//�̱�Ƚ��.
	ush_int	nLose_Num;					//����Ƚ��.
	ush_int	nDraw_Num;					//���º�Ƚ��.
	ush_int	nCut_Time;					//����Ƚ��.
	ush_int nChannel;					//������ ä��
	int nUserNo;						//������ �����ѹ�
	char cSex;							//'0' ���� '1'����
	char szPreAvatar[MAX_AVAURLLEN];	//��ǥ�ƹ�Ÿ.
	char szGameGubun[5];				// '0' : Neo, '1' : õ����.
} sUserInfo;


typedef struct tagUserList	//Server�� ������ �ִ� ��������
{
	char szUser_Id[MAX_ID_LENGTH];		//�������̵�
	sUserInfo l_sUserInfo;
} sUserList;


typedef struct tagPKFirst
{
	char szHeader[2];
	int nUserNo;
	char szTail[2];
} sPKFirst;


typedef struct tagLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;
	sUserList l_sUserList;

} sLogIn;

typedef struct tagIsLogIn
{

	char szGameServerIp[MAX_IP_LEN];
	ush_int nGameServerPort;

} sIsLogIn;


#pragma pack( pop, 1)


#endif
