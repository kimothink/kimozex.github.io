
#include "StdAfx.h"
#include "UserInfo.h"





CUserInfo::CUserInfo()
{
	Init();
}

CUserInfo::~CUserInfo()
{
}

CUserInfo::Init()
{
	m_bUser = false;
	memset( &m_sUser, 0x00, sizeof(sUserInfo) );
}


