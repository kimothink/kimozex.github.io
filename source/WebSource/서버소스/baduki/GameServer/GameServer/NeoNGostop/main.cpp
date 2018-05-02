#include "Common.h"
#include "Manager.h"
#include "dtrace.h"

CManager *g_pCManager = NULL;
SERVERINFO g_ServerInfo;
ADOCONN g_AdoConn;

FILE *game_log = NULL;
FILE *game_logFile = NULL;

void LoadSettings();

char	g_ClientSendBuf[NORMAL_SEND_BUFFER];
char	g_ClientSendMidBuf[MID_SEND_BUFFER];
char	g_ClientSendLargeBuf[LARGE_SEND_BUFFER];
char	a_Buf2[MAX_STRING_LENGTH];

char	g_ClientAllSendBuf[NORMAL_SEND_BUFFER];
int		g_ClientAllSendLen = 0;


sJackPotInfo g_sJackPotInfo[JACKPOT_CNT] = {0, };


void at_exit_exec (void)
{
	if (game_log) fclose (game_log);
}

LONG __stdcall HUNUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) 
{
    return EXCEPTION_EXECUTE_HANDLER;
}



int main(int argc, char* argv[], char *envp[])
{
	int nRetCode = 1004;
	char pTemp[256];

	LPTOP_LEVEL_EXCEPTION_FILTER  pOldhandler =  SetUnhandledExceptionFilter(HUNUnhandledExceptionFilter); 

	g_pCManager = new CManager;
	g_pCManager->GetPath();

	LoadSettings();
	InitLogFile ();
	g_pCManager->m_pDbQ->GetSysRateInfo();

	ServerSrandom(time(0));

	Log("*********BADUKI*********");
	LogFile("BADUKI");


	//DB 접속부분...
	if (!g_pCManager->MakeThread())	return 1;

	if (!g_pCManager->InitGame()) return 1;

	strcpy (pTemp, "*********NGN Server Start.*********");
	puts (pTemp);

	sprintf(pTemp, "*NGN Server Compile Time* %s (%s)\n",__DATE__,__TIME__);
	puts (pTemp);

	atexit (at_exit_exec);


	CUserDescriptor *newd;
	CREATE(newd, CUserDescriptor, 1);
	

	/*
	A	2	3	4	5	6	7	8	9	10	J	Q	K

    0	1	2	3	4	5	6	7	8	9	10	11	12		♣
	13	14	15	16	17	18	19	20	21	22	23	24	25		♥
	26	27	28	29	30	31	32	33	34	35	36	37	38		◆
	39	40	41	42	43	44	45	46	47	48	49	50	51		♠
	*/

	//♣J, ◆J, ♣10, ◆10 = 10, 36, 9, 35
	//◆K, ♣J, ♣9, ◆J = 38, 10, 8, 36
	//◆A, ♣J, ♣9, ◆J = 26, 10, 8, 36	
	//◆5, ♣2, ♣K, ◆2 = 30, 1, 12, 27
	//◆3, ♣J, ♣9, ◆J = 28, 10, 8, 36
	//♣A, ◆2, ◆A, ♠2 = 0, 27, 26, 40
	//♣9, ♣A, ♠A, ♠Q = 8, 0, 39, 50
	//♠2, ♠K, ♥J, ◆J = 40, 51, 23, 36
	//♣5, ♠5, ♥7, ♥9 = 4, 43, 19, 21
	//♠4, ♣6, ♣4, ♠7 = 42, 5, 3, 45
	//♠2, ♠8, ◆5, ◆2 = 40, 46, 30, 27
	//♥A, ♣K, ♥7, ◆K = 13, 12, 19, 38
	//◆8, ♠8, ♠9, ◆10 = 33, 46, 47, 35
	//♥2, ♣5, ♣2, ◆5 = 14, 4, 1, 30

	//♠A, ◇4, ♡8, ♡Q = 39, 29, 20, 24
	//♠4, ◇4, ♡8, ♣Q = 42, 29, 20, 11
	//♡3, ◇4, ♠4, ♠K = 15, 29, 42, 51
	//♠A, ♡4, ♡8, ♡Q = 39, 16, 20, 24
	//♡4, ◇4, ♠4, ♠K = 16, 29, 42, 51
	//◇4, ◇5, ♠4, ♠K = 29, 30, 42, 51

	/*
	newd->m_CPokerEvalution.m_nHaveCnt = 4;

	newd->m_nUserCard[0] = 29;
	newd->m_nUserCard[1] = 33;
	newd->m_nUserCard[2] = 31;
	newd->m_nUserCard[3] = 0;


	newd->m_CPokerEvalution.Calcul( newd->m_nUserCard, 4 );
	*/




	g_pCManager->Run();
	g_pCManager->Close();

	LogFile("End!");
	if (!g_pCManager->m_Reboot) {

		FILE *fp = fopen (".shutdown", "w");
		fclose (fp);

	}
	//dll loading delete
	WSACleanup();
	delete g_pCManager;
	return nRetCode;
}

void LoadSettings()
{
	char strTemp[256];
	char strTemp2[256];
	FILE *fp;

	sprintf(strTemp, "%s%s", g_ServerPath, "avaconfig.ini");

	if (!(fp = fopen(strTemp, "rb"))) {
		Log ("Setting Error!!");
		exit(0);
	}

	fscanf(fp, "%s %s", strTemp2, g_ServerInfo.szDbServerName);
	fscanf(fp, "%s %d", strTemp2, &g_ServerInfo.szServerChannel);		
	fscanf(fp, "%s %d", strTemp2, &g_ServerInfo.nListenPort);		
	fscanf(fp, "%s %d %s", g_ServerInfo.szMgrServerName, &g_ServerInfo.nMgrPort, g_ServerInfo.szMgrServerIp);		
	fscanf(fp, "%s %s", strTemp2, g_ServerInfo.szIpCheck);
		
	fscanf(fp, "%s %s", strTemp, g_AdoConn.m_AdoProvider );
	fscanf(fp, "%s %s", strTemp, g_AdoConn.m_AdoCatalog);
	fscanf(fp, "%s %s", strTemp, g_AdoConn.m_AdoDataSource);
	fscanf(fp, "%s %s", strTemp, g_AdoConn.m_AdoId );
	fscanf(fp, "%s %s", strTemp, g_AdoConn.m_AdoPwd);

	sprintf(g_AdoConn.m_AdoConStr, "Provider=%s;Initial Catalog=%s;Data Source=%s;User ID=%s;Password=%s", 
		g_AdoConn.m_AdoProvider, g_AdoConn.m_AdoCatalog, g_AdoConn.m_AdoDataSource, 
		g_AdoConn.m_AdoId, g_AdoConn.m_AdoPwd);

	fclose(fp);
}











































