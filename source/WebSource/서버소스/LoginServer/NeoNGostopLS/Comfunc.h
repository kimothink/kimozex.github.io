#ifndef __COMFUNC_H
#define __COMFUNC_H


void Log(const char *message);
void LogFile(const char *message);

char *StrDup(const char *source);
void StrCat(char *dest, const char *src);
void Itoa(int value, char *string);
void IntCat2 (char *dest, int i);
void InitLogFile ();
void IntCat(char *dest, int i);
void _Itoa(int value, char *string);
void LongCat(char *dest, long l);
void SkipSpaces(char **string);


void GetTimeofDay(struct timeval *t, struct timezone *dummy);
void TimeDiff(struct timeval *rslt, struct timeval *arg1, struct timeval *arg2 );
void TimeAdd(struct timeval *rslt, struct timeval *arg1, struct timeval *arg2);


static unsigned long seed;

void ServerSrandom(unsigned long initial_seed);
unsigned long ServerRandom(void);
int GetRandom(int from, int to);

void PlusSort( int *nData, int nLength );
void MinusSort( int *nData, int nLength );
void MinusSort( float *nData, int nLength );

//한번 나온 숫자는 나오지 않게...
void StaticRandom( int *nResultData, int nDefaultSize, int nResultDataSize );


#endif


