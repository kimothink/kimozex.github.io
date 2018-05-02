// DescriptorData.cpp: implementation of the CDescriptorData class.
//
//////////////////////////////////////////////////////////////////////
#include "UserDescriptor.h"
#include "Manager.h"

CUserDescriptor::CUserDescriptor()
{
	memset(m_Inbuf, 0x00, MAX_RAW_INPUT_LENGTH);
	memset(m_SmallOutbuf, 0x00, SMALL_BUFSIZE);
	memset(m_Host, 0x00, HOST_LENGTH);
	memset(m_HostCClass, 0x00, HOST_LENGTH);
	
	m_Buflength = 0;
	m_Output = NULL;
	m_Bufptr = 0;
	m_Bufspace = 0;
	m_LargeOutbuf = NULL;
	m_pNext = NULL;
	m_bClosed = false;
	m_nUserMode = 0;
	m_TimeStamp	= -1;
	m_bWaitDb = false;
	m_bUpdateDb = false;
	m_DBQRunning = false;
	m_bDbAccess = true;
	m_QuitSave = false;
	m_bDataInput = FALSE;
	m_bKeepAliveSend = FALSE;

	//User Personnal Info	
	memset(&m_MyInfo, 0x00, sizeof(sUserList));

	//User General
	memset( m_szPureHeader, 0x00, sizeof(m_szPureHeader) );

	m_nRoomNo = -1;
	m_nRoomInUserNo = -1;
	m_bBbangJjang = false;
	m_bRoomMaster = false;	
	m_nUserPage = -1;

	m_bPlay = false;
	m_nGameMode = USERNONE;

	m_nDBMode = NONE;
	m_nIsLogIn = 1;

	memset( &m_sOption, 0x00, sizeof(m_sOption) );
	memset( szSiteCode, 0x00, sizeof(szSiteCode) );
	memset( m_szOtherNick_Name, 0x00, MAX_NICK_LENGTH );

	m_nPrizeNo = 0;
	m_nSucceedCode = 0;

	//yun 2004.10.19
	m_nCurPtr = 0;
	m_nReveillsize = 0;
	m_nToriZisu = 0;
	m_biSystemMoney = 0;

	memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ) );
	memset( &m_sUserDBUpdateInfo, 0x00, sizeof(m_sUserDBUpdateInfo) );

}

CUserDescriptor::~CUserDescriptor()
{


}

void CUserDescriptor::CloseSocket()
{
	CLOSE_SOCKET(m_ClientSocket);
	FlushQueues();
}


void CUserDescriptor::FlushQueues()
{
	int nBlockSize = 0;
	if (m_LargeOutbuf) {
		m_LargeOutbuf->m_pNext = bufpool;
		bufpool = m_LargeOutbuf;
	}

	while (input.GetFromQ(a_Buf2, &nBlockSize));
}


void CUserDescriptor::WriteToOutput(const char *txt, int nSendBufLength)
{
	int size;
	char *data = (char *)txt;

	if (m_bClosed) return;
	/* if we're in the overflow state already, ignore this new output */
	if (m_Bufptr < 0)
		return;

	//size = strlen(txt);
	size = nSendBufLength;

	
	/* if we have enough space, just write to buffer and that's it! */
	
	if (m_Bufspace >= size) {
		//strcpy(m_Output + m_Bufptr, data);
		memcpy(m_Output + m_Bufptr, data, size);
		m_Bufspace -= size;
		m_Bufptr += size;
		return;
	}


	sprintf (a_Log, "BufferChange : UserId = %s, Size = %d", this->m_MyInfo.szUser_Id, size );
	IntCat (a_Log, m_Bufptr);
	LogFile(a_Log);

	/*
	* If the text is too big to fit into even a large buffer, chuck the
	* new text and switch to the overflow state.
	*/
	if (size + m_Bufptr > LARGE_BUFSIZE - 1) {

		sprintf (a_Log, "ERROR > Buffer Overflow : UserId = %s, Size = %d", this->m_MyInfo.szUser_Id, size );
		IntCat (a_Log, m_Bufptr);
		LogFile(a_Log);

		m_Bufptr = -1;
		buf_overflows++;
		return;
	}
	buf_switches++;

	/* if the pool has a buffer in it, grab it */
	if (bufpool != NULL) {
		m_LargeOutbuf = bufpool;
		bufpool = bufpool->m_pNext;
		
	} else {			// else create a new one 
		CREATE(m_LargeOutbuf, CTxtBlock, 1);
		CREATE(m_LargeOutbuf->m_Text, char, LARGE_BUFSIZE);
		buf_largecount++;

	}

	//strcpy(m_LargeOutbuf->m_Text, m_Output);	/* copy to big buffer */
	memcpy(m_LargeOutbuf->m_Text, m_Output, m_Bufptr);	/* copy to big buffer */

	m_Output = m_LargeOutbuf->m_Text;	/* make big buffer primary */
	
	//strcat(m_Output, data);	/* now add new text */
	memcpy( &m_LargeOutbuf->m_Text[m_Bufptr], data, size);

	/* set the pointer for the next write */
	//m_Bufptr = strlen(m_Output);
	m_Bufptr += size;

	/* calculate how much space is left in the buffer */
	m_Bufspace = LARGE_BUFSIZE - 1 - m_Bufptr;
}

int CUserDescriptor::ProcessOutput(bool bSecret/*CDescriptorData *t*/)
{
	//char i[MAX_SOCK_BUF + 1];
	int result;
	int total;

	if (m_Bufptr > 0) {
		total = m_Bufptr;
	}
	else {
		strcpy (a_Log, "(ProcessOutput)\t");
		StrCat (a_Log, "UserID : "); 
		//StrCat (a_Log, this->m_UserId); 
		StrCat (a_Log, "\t유저가 ProcessOutput을 했는데 m_Bufptr가 없었다.");
		LogFile (a_Log);
		return -1;
	}
	

	m_Output[total] = '\0';
	result = WriteToDescriptor(m_ClientSocket, m_Output, total);


	if (result < 0) {
		m_bClosed = true;
	}

	if (m_LargeOutbuf) {
		m_LargeOutbuf->m_pNext = bufpool;
		bufpool = m_LargeOutbuf;
		
		m_Output = m_SmallOutbuf;
	}
	/* reset total bufspace back to that of a small buffer */
	m_Bufspace = SMALL_BUFSIZE - 1;
	m_Bufptr = 0;
	*(m_Output) = '\0';

	return (result);
}

bool CUserDescriptor::GetIdName(char *arg)
{
	BYTE bCmd, bErrorCode;
	int nIndex = 0;


	return true;
}

int CUserDescriptor::ProcessInput(bool bSecret/*CDescriptorData *t*/)
{
	char	szRecvBuffer[MAX_BUFFER_SIZE];
	
	memset( szRecvBuffer, 0, MAX_BUFFER_SIZE );

	WSABUF  RecvBuf;
	RecvBuf.buf = szRecvBuffer;
	RecvBuf.len = MAX_BUFFER_SIZE;


	DWORD RecvBytes;
	DWORD tRecvBytes;
	int ret = 0;
	DWORD dwFlag = 0;
	

	ret = WSARecv( m_ClientSocket, &RecvBuf, 1, &RecvBytes, &dwFlag, NULL, NULL );


	if( ret == SOCKET_ERROR )
	{
		if( WSAGetLastError() == WSAEWOULDBLOCK )
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else {
		if ( RecvBytes == 0 ) return -1;
	}
	

	tRecvBytes = RecvBytes;
	
	if ( m_nCurPtr == 0 ) {
		m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );
		
		if ( m_nReveillsize <= tRecvBytes ) {
			if ( m_nReveillsize == tRecvBytes ) {
				m_nCurPtr = 0;
				input.WriteToQ( RecvBuf.buf, m_nReveillsize);
			}
			else  {	
				while(1) {
						m_nCurPtr = 0;
						input.WriteToQ( RecvBuf.buf, m_nReveillsize);
							
						tRecvBytes = tRecvBytes - m_nReveillsize;
						
						if ( tRecvBytes == 0 ) break;
						memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );

						m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );

						if ( m_nReveillsize  > tRecvBytes ) {
							m_nReveillsize = m_nReveillsize - tRecvBytes;
							memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
							m_nCurPtr += tRecvBytes;
							break;
						}
				}
			}
		}
		else {
				memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
				m_nReveillsize = m_nReveillsize - tRecvBytes;
				m_nCurPtr += tRecvBytes;
		}
	}
	else {
		if ( m_nReveillsize > tRecvBytes ) {
			memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, tRecvBytes );
			m_nReveillsize = m_nReveillsize - tRecvBytes;
			m_nCurPtr += tRecvBytes;
		}
		else
			if ( m_nReveillsize == tRecvBytes ) {
				memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, tRecvBytes );
				m_nCurPtr += tRecvBytes;
				input.WriteToQ( m_szBufferQ, m_nCurPtr);
				memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ));
				m_nCurPtr = 0;
			}
			else {
				memcpy( m_szBufferQ + m_nCurPtr, RecvBuf.buf, m_nReveillsize );	
				m_nCurPtr += m_nReveillsize;
				input.WriteToQ( m_szBufferQ, m_nCurPtr);
				memset( m_szBufferQ, 0x00, sizeof(m_szBufferQ));

				tRecvBytes = tRecvBytes - m_nReveillsize;
				memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );
				m_nCurPtr = 0;

				m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );
				
				if ( m_nReveillsize <= tRecvBytes ) {
					if ( m_nReveillsize == tRecvBytes ) {
						m_nCurPtr = 0;

						input.WriteToQ( RecvBuf.buf, m_nReveillsize);
					}
					else  {	
						while(1) {
							m_nCurPtr = 0;
							input.WriteToQ( RecvBuf.buf, m_nReveillsize);
										
							tRecvBytes = tRecvBytes - m_nReveillsize;
									
							memcpy( RecvBuf.buf, RecvBuf.buf + m_nReveillsize, tRecvBytes );

							m_nReveillsize = MAKEWORD( RecvBuf.buf[SC_PKTLEN_POS], RecvBuf.buf[SC_PKTLEN_POS+1] );

							if ( m_nReveillsize  > tRecvBytes ) {
								m_nReveillsize = m_nReveillsize - tRecvBytes;
								memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
								m_nCurPtr += tRecvBytes;
								break;
							}
									
						}

				}
			}
			else {
				memcpy( m_szBufferQ, RecvBuf.buf, tRecvBytes );
				m_nReveillsize = m_nReveillsize - tRecvBytes;
				m_nCurPtr += tRecvBytes;
			}
		}
	}
	return 1;
}