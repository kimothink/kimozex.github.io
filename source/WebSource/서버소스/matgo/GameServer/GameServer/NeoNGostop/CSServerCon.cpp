#include "CSServerCon.h"




CSServerCon::CSServerCon()
{
	m_CSClient = NULL;

}


CSServerCon::~CSServerCon()
{

}





void CSServerConThread(LPVOID lParam)
{
	CSServerCon *pCSServerCon = (CSServerCon *) lParam;

	CSServerConDbFunc(pCSServerCon);
}

bool ConnCSServer (CSServerCon *pCSServerCon)
{
	return true;
}

void CSServerConDbFunc(CSServerCon *pCSServerCon)
{
	while (true) {	
		Sleep (3000);

		if (ConnCSServer (pCSServerCon))
			continue;
		if (pCSServerCon->m_CSClient)
			pCSServerCon->m_CSClient->m_bClosed = true;

	}		
}

