#include "common.h"
#include "comfunc.h"

#include <mmsystem.h>

char a_Log[SMALL_BUFSIZE];
char i_buf[128];




char i_buf2[128];
void IntCat2 (char *dest, int i)
{
	strcat(dest, " ");
	_Itoa(i, i_buf2);
	strcat(dest, i_buf2);
}

void IntCat(char *dest, int i)
{
	memset(i_buf, 0x00, sizeof(char)*128);
	strcat(dest, " ");
	Itoa(i, i_buf);
	strcat(dest, i_buf);
}


void _Itoa(int value, char *string)
{
	int idx = 0;
	int plus = 1;
	if (value < 0) {
		plus = -1;
		value = -value;
	}

	while (value != 0) {
		string[idx++] = (value % 10) + '0';
		value /= 10;
	}

	if (idx > 0) {
		if (plus < 0)
			string[idx++] = '-';
		for (int i = 0; i < idx/2; i++) {
			char temp = string[i];
			string[i] = string[idx-i-1];
			string[idx-i-1] = temp;
		}
		string[idx] = '\0';
	} else {
		*string++ = '0';
		*string = '\0';
	}
}


void SkipSpaces(char **string)
{
	for (; **string && is_space(**string); (*string)++);
}

char *StrDup(const char *source)
{
  char *new_z;

  CREATE(new_z, char, strlen(source) + 1);
  return (strcpy(new_z, source));
}

void StrCat(char *dest, const char *src)
{
	strcat(dest, " ");
	strcat(dest, src);
}

void LongCat(char *dest, long l)
{
	memset(i_buf, 0x00, sizeof(char)*128);
	strcat(dest, " ");
	sprintf (i_buf, "%d", l);
	//ltoa(l, i_buf, 128);
	strcat(dest, i_buf);
}

//로그기록 fuction
void Log(const char *message)
{
	void Itoa (int value, char *string);	//정수를 문자열로 변환

	extern FILE * game_log;
	time_t ct = time(0);					//Current Time을 가져옴
	struct tm ti;							
	char te_buf[1024], tt_buf[16];
	static int day = -1;
	//static int sec = 0;

	memcpy (&ti, localtime (&ct), sizeof(struct tm));
	Itoa   ((ti.tm_year + 1900), tt_buf);
	strcpy (te_buf, tt_buf);
	strcat (te_buf, "/");
	te_buf[5] = '0' + (ti.tm_mon + 1) / 10; //?
	te_buf[6] = '0' + (ti.tm_mon + 1) % 10;
	te_buf[7] = '/';
	te_buf[8] = '0' + ti.tm_mday / 10;
	te_buf[9] = '0' + ti.tm_mday % 10;
	te_buf[10] = ' ';
	te_buf[11] = '0' + ti.tm_hour / 10;
	te_buf[12] = '0' + ti.tm_hour % 10;
	te_buf[13] = ':';
	te_buf[14] = '0' + ti.tm_min / 10;
	te_buf[15] = '0' + ti.tm_min % 10;
	te_buf[16] = ':';
	te_buf[17] = '0' + ti.tm_sec / 10;
	te_buf[18] = '0' + ti.tm_sec % 10;
	te_buf[19] = '\0';
	strcat (te_buf, " :: ");
	strcat (te_buf, message);
	strcat (te_buf, "\n");

	if (day == -1) day = ti.tm_mday;

	//if (day != ti.tm_mday)
	//	InitLogFile ();

	fputs  (te_buf, game_log);
	fflush (game_log);
}

//로그기록 fuction
void LogFile(const char *message)
{
	//Log(message);
	//return;

	void Itoa (int value, char *string);	//정수를 문자열로 변환

	extern FILE * game_logFile;
	time_t ct = time(0);					//Current Time을 가져옴
	struct tm ti;							
	char te_buf[1024], tt_buf[16];
	static int day = -1;
	//static int sec = 0;

	memcpy (&ti, localtime (&ct), sizeof(struct tm));
	Itoa   ((ti.tm_year + 1900), tt_buf);
	strcpy (te_buf, tt_buf);
	strcat (te_buf, "/");
	te_buf[5] = '0' + (ti.tm_mon + 1) / 10; //
	te_buf[6] = '0' + (ti.tm_mon + 1) % 10;
	te_buf[7] = '/';
	te_buf[8] = '0' + ti.tm_mday / 10;
	te_buf[9] = '0' + ti.tm_mday % 10;
	te_buf[10] = ' ';
	te_buf[11] = '0' + ti.tm_hour / 10;
	te_buf[12] = '0' + ti.tm_hour % 10;
	te_buf[13] = ':';
	te_buf[14] = '0' + ti.tm_min / 10;
	te_buf[15] = '0' + ti.tm_min % 10;
	te_buf[16] = ':';
	te_buf[17] = '0' + ti.tm_sec / 10;
	te_buf[18] = '0' + ti.tm_sec % 10;
	te_buf[19] = '\0';
	strcat (te_buf, " :: ");
	strcat (te_buf, message);
	strcat (te_buf, "\n");

	if (day == -1) day = ti.tm_mday;

	if (day != ti.tm_mday) {
		day = ti.tm_mday;
		InitLogFile ();
	}
	
	fputs  (te_buf, game_logFile);
	fflush (game_logFile);

}


void Itoa(int value, char *string)
{
	int idx = 0;
	int plus = 1;
	if (value < 0) {
		plus = -1;
		value = -value;
	}

	while (value != 0) {
		string[idx++] = (value % 10) + '0';
		value /= 10;
	}

	if (idx > 0) {
		if (plus < 0)
			string[idx++] = '-';
		for (int i = 0; i < idx/2; i++) {
			char temp = string[i];
			string[i] = string[idx-i-1];
			string[idx-i-1] = temp;
		}
		string[idx] = '\0';
	} else {
		*string++ = '0';
		*string = '\0';
	}
}

//로그파일 초기화
void InitLogFile ()
{
	//yun 현재 타임을 가져온다.
	time_t ct = time(0);
	//yun 가져온 현재 타임을 시간, 분, 초등으로 나눈다.
	struct tm * ti = localtime (&ct);
	
	char logstr[128];
	extern FILE * game_log;

	extern FILE *game_logFile;

	//로그파일이 이미 열려 있으면 로그파일 닫기
	if (game_log != NULL && game_log != stdout)
		fclose (game_log);

	game_log = stdout;



	//로그파일이 이미 열려 있으면 로그파일 닫기
	if (game_logFile != NULL && game_logFile != stdout)
		fclose (game_logFile);

	sprintf (logstr, "./LogFiles/Log%02d%02d.txt", (ti->tm_mon+1), ti->tm_mday);

	if ((game_logFile = fopen(logstr, "a+")) == NULL)
		game_logFile = stdout;


}


void GetTimeofDay(struct timeval *t, struct timezone *dummy)
{
	unsigned long millisec = timeGetTime();
	t->tv_sec = (int) (millisec / 1000);

	//yun 나머지 *1000 하면 마이크로 sec를 구할수 있다.
	t->tv_usec = (millisec % 1000) * 1000;
}

long time_gap_return_second(struct timeval *t1, struct timeval *t2)
{
	struct timeval rtn;

	rtn.tv_sec = 0;
	rtn.tv_usec = 0;

	if( t1->tv_sec == t2->tv_sec )
	{
		if( t1->tv_usec < t2->tv_usec ) rtn.tv_usec = t2->tv_usec - t1->tv_usec;
	}
	else if( t1->tv_sec < t2->tv_sec )
	{
		rtn.tv_sec = t2->tv_sec - t1->tv_sec;
		if( t1->tv_usec > t2->tv_usec )
		{
			rtn.tv_usec = t2->tv_usec + 1000000 - t1->tv_usec;
			rtn.tv_sec--;
		}
		else rtn.tv_usec = t2->tv_usec - t1->tv_usec;
	}

	// 소수점 이하 버림... usec은 버림...
	return (long)(rtn.tv_sec);
}


void TimeDiff(struct timeval *rslt, struct timeval *arg1, struct timeval *arg2 )
{
	if (arg1->tv_sec < arg2->tv_sec)
		*rslt = a_NullTime;
	else if (arg1->tv_sec == arg2->tv_sec) {
		if (arg1->tv_usec < arg2->tv_usec)
			*rslt = a_NullTime;
		else {
			rslt->tv_sec = 0;
			rslt->tv_usec = arg1->tv_usec - arg2->tv_usec;
		}
	} else {			// a->tv_sec > b->tv_sec 
		rslt->tv_sec = arg1->tv_sec - arg2->tv_sec;
		if (arg1->tv_usec < arg2->tv_usec) {
			rslt->tv_usec = arg1->tv_usec + 1000000 - arg2->tv_usec;
			rslt->tv_sec--;
		} else
			rslt->tv_usec = arg1->tv_usec - arg2->tv_usec;
	}
}

void TimeAdd(struct timeval *rslt, struct timeval *arg1, struct timeval *arg2)
{
	rslt->tv_sec = arg1->tv_sec + arg2->tv_sec;
	rslt->tv_usec = arg1->tv_usec + arg2->tv_usec;

	while (rslt->tv_usec >= 1000000) {
		rslt->tv_usec -= 1000000;
		rslt->tv_sec++;
	}
}




void ServerSrandom(unsigned long initial_seed)
{
	seed = initial_seed; 
}


unsigned long ServerRandom(void)
{
	int lo, hi, test;

	hi   = seed/q;
	lo   = seed%q;

	test = a*lo - r*hi;

	if (test > 0)
		seed = test;
	else
		seed = test+ m;

	return (seed);
}

int GetRandom(int from, int to)
{
	if (from > to) {
		int tmp = from;
		from = to;
		to = tmp;
	}

	return ((ServerRandom() % (to - from + 1)) + from);
}


void PlusSort( int *nData, int nLength )
{
	int nImsi;

	for( int i = 0; i < nLength - 1; i ++ )
	{
		for( int j = i + 1; j < nLength; j ++ )
		{

			if(  nData[i] > nData[j] )
			{
				nImsi = nData[i];
				nData[i] = nData[j];
				nData[j] = nImsi;
			}
		}
	}
}


void MinusSort( int *nData, int nLength )
{
	int nImsi;

	for( int i = 0; i < nLength - 1; i ++ )
	{
		for( int j = i + 1; j < nLength; j ++ )
		{

			if(  nData[i] < nData[j] )
			{
				nImsi = nData[i];
				nData[i] = nData[j];
				nData[j] = nImsi;
			}
		}
	}
}

void MinusSort( float *nData, int nLength )
{
	float nImsi;

	for( int i = 0; i < nLength - 1; i ++ )
	{
		for( int j = i + 1; j < nLength; j ++ )
		{

			if(  nData[i] < nData[j] )
			{
				nImsi = nData[i];
				nData[i] = nData[j];
				nData[j] = nImsi;
			}
		}
	}
}

void StaticRandom( int *nResultData, int nDefaultSize, int nResultDataSize )
{
	int  i, j;
	int nGetRandom = 0;
	int nCurPoint = 0;
	int nTo;
	int *nDefaultData = NULL;

	memset( nResultData, 0x00, sizeof(int) * nResultDataSize );

	
	nDefaultData = new int[nDefaultSize];
	// 1,2,3,4,5,6...Setting
	for ( i = 0; i < nDefaultSize; i++ ) {
		nDefaultData[i] = i + 1;
	}

		
	nTo = nDefaultSize;


	for ( i = 0; i < nResultDataSize; i++ ) {
		nGetRandom = GetRandom( 0, nTo - 1 );
		nResultData[i] = nDefaultData[nGetRandom];

		for ( j = 0; j < nTo; j ++ ){
			if ( nDefaultData[j] != nDefaultData[nGetRandom] ) {
				nDefaultData[nCurPoint]	= nDefaultData[j];
				nCurPoint++;
			}
		}
		nTo--;
		nCurPoint = 0;
	}

	
	SAFE_DELETE_ARRAY(nDefaultData);
}
