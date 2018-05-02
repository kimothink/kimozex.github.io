//#pragma warning (disable : 4305)



#include "Common.h"
#include "Manager.h"



CManager *g_pCManager = NULL;
SERVERINFO g_ServerInfo;
SERVERINFO g_ChonServerInfo[3];


sGameServerInfo g_sGameServerInfo[MAX_GAME_JONG];
ADOCONN g_AdoConn;
sNotice g_sNotice;

FILE *game_log = NULL;
FILE *game_logFile = NULL;

void LoadSettings();
void LoadGameServerIp();


char	g_ClientSendBuf[NORMAL_SEND_BUFFER];
char	g_ClientSendMidBuf[MID_SEND_BUFFER];
char	g_ClientSendLargeBuf[LARGE_SEND_BUFFER];
char	a_Buf2[MAX_STRING_LENGTH];




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

	//응용프로그램오류 메세지 박스를 발생시키지 않는다.
	LPTOP_LEVEL_EXCEPTION_FILTER  pOldhandler =  SetUnhandledExceptionFilter(HUNUnhandledExceptionFilter); 

	g_pCManager = new CManager;
	g_pCManager->GetPath();

	LoadSettings();
	LoadGameServerIp();
	InitLogFile ();
	ServerSrandom(time(0));

	Log("*********Opening ZenterrGames LogIn Server*********");
	LogFile("Opening ZenterrGames LogIn Server");


	//DB 접속부분...
	if (!g_pCManager->MakeThread())	return 1;
	if (!g_pCManager->InitGame()) return 1;

	strcpy (pTemp, "*********LogIn Server Start.*********");
	puts (pTemp);

	sprintf(pTemp, "*LogIn Server Compile Time* %s (%s)\n",__DATE__,__TIME__);
	puts (pTemp);

	atexit (at_exit_exec);

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
	FILE *fp;

	sprintf(strTemp, "%s%s", g_ServerPath, "avaconfig.ini");

	if (!(fp = fopen(strTemp, "rb"))) {
		Log ("Setting Error!!");
		exit(0);
	}

	fscanf(fp, "%s %d", g_ServerInfo.szName, &g_ServerInfo.nListenPort );
	fscanf(fp, "%s %d %s", g_ServerInfo.szMgrServerName, &g_ServerInfo.nMgrPort, &g_ServerInfo.szMgrServerIp );

	




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


void LoadGameServerIp()
{
	char strTemp[256];
	FILE *fp;

	sprintf(strTemp, "%s%s", g_ServerPath, "gameserverip.ini");

	if (!(fp = fopen(strTemp, "rb"))) {
		Log ("Setting Error!!");
		exit(0);
	}

	for ( int i = 0; i < MAX_GAME_JONG; i++ ) {
		fscanf(fp, "%s %s %d", strTemp, g_sGameServerInfo[i].szGameServerIp, &g_sGameServerInfo[i].nGameServerPort );
	}

	fclose(fp);
}










