//#include <stdafx.h>
#include <windows.h>
#include <stdio.h>
//#include <stdio.h>
#include "Comfunc.h"

#include "dtrace.h"

HWND m_gTraceHandle = 0;

HRESULT _cdecl DTRACE(char * lpszFormat, ...)
{
	HRESULT hResult;

	int nBuf;
	char szBuffer[512];
	va_list args;

	va_start(args, lpszFormat);
	nBuf = vsprintf(szBuffer, lpszFormat, args);
	va_end(args);
	if (nBuf < 0) return -1;

	//LogFile (szBuffer);

	COPYDATASTRUCT cds;
	cds.cbData = nBuf;
	cds.lpData = (void *) szBuffer;

	if (m_gTraceHandle == 0)
		m_gTraceHandle = FindWindow("TfmMain", "Trace Server");

	hResult = (HRESULT)::SendMessage(m_gTraceHandle, WM_COPYDATA, NULL, (LPARAM)&cds);
	return hResult;
}

