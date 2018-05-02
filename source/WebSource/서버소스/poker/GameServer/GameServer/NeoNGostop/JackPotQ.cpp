#include "JackPotQ.h"


CJackPotQ::CJackPotQ()
{

	memset( m_sJackPotQ, 0x00, sizeof(m_sJackPotQ) );

	if (!m_sJackPotQ) {
		perror ("SYSERR: Cannot create thread queue");
		exit (0);
	}

	m_Head = 0;
	m_Tail = 0;
	m_Semaphore = false;
}


CJackPotQ::~CJackPotQ()
{
}


bool CJackPotQ::IsEmpty()
{
	return (m_Head == m_Tail);
}

bool CJackPotQ::AddToQ( BigInt biRoomMoney, BigInt biJackPotMoney )
{
	m_Semaphore = true;
	if (IsFull()) {

		m_Semaphore = false;
		return false; // SYSERR

	}

	m_sJackPotQ[m_Tail].biRoomMoney = biRoomMoney;
	m_sJackPotQ[m_Tail].biJackPotMoney = biJackPotMoney;

	int tail = m_Tail+1;
	if (tail >= MAX_JACKPOTQ_NUM) {
		tail = 0;
	}

	m_Tail = tail;

	m_Semaphore = false;

	return true;
}

bool CJackPotQ::GetFromQ( sJackPotQ *asJackPotQ )
{
	if (m_Semaphore) {

		sprintf( a_Log, "(CJackPotQ::GetFromQ) m_Semaphore : %d 이거때문에 큐에서 가져오는거 실패했다. ", m_Semaphore );
		LogFile (a_Log);
	
		return NULL;
	}

	if (m_Head == m_Tail) {
		return NULL;
	}

	memcpy( asJackPotQ, &m_sJackPotQ[m_Head], sizeof(m_sJackPotQ[m_Head]) );
	memset( &m_sJackPotQ[m_Head], 0x00, sizeof(m_sJackPotQ[m_Head]) );
	
	int head = m_Head+1;
	if (head >= MAX_JACKPOTQ_NUM)
		head = 0;
	
	m_Head = head;

	return true;
}

bool CJackPotQ::IsFull()
{
	int tail = m_Tail+1;

	if (tail >= MAX_JACKPOTQ_NUM)
		tail = 0;
	if (tail == m_Head)
		return true;

	return false;
}