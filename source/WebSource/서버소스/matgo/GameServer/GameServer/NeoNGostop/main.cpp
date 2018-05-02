//#pragma warning (disable : 4305)



#include "Common.h"
#include "Manager.h"


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
	ServerSrandom(time(0));

	Log("*********NewMATGO*********");
	LogFile("NewMATGO");


	if (!g_pCManager->MakeThread())	return 1;

	if (!g_pCManager->MakeCounterThread())	return 1;


	if (!g_pCManager->InitGame()) return 1;

	strcpy (pTemp, "*********NGN Server Start.*********");
	puts (pTemp);

	sprintf(pTemp, "*NGN Server Compile Time* %s (%s)\n",__DATE__,__TIME__);
	puts (pTemp);

	atexit (at_exit_exec);

	g_pCManager->Run();
	g_pCManager->Close();

	LogFile("End!");
	if (!g_pCManager->m_Reboot) {

		FILE *fp = fopen (".shutdown", "w");
		fclose (fp);

	}
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











































