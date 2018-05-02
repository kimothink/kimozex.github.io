// World.h: interface for the World class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLD_H__20F1A2A3_0432_44F1_8B96_7E87E1C3D2C3__INCLUDED_)
#define AFX_WORLD_H__20F1A2A3_0432_44F1_8B96_7E87E1C3D2C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Channel.h"


class World  
{
public:
	World();
	virtual ~World();

	
public:
	CChannel m_Channel[MAX_CHANNEL];

	
	//int NextUserSlot
	int FindRoomMasterUniNo( int nChannel, int nRoomNo );
	int FindRoomMasterSlotNo( int nChannel, int nRoomNo );
	int FindNextUserSltNo( int nChannel, int nRoomNo, int nMySlotNo );
	int FindNextUserUniqNo( int nChannel, int nRoomNo, int nMySlotNo );
	int FindSlot( int nChannel, int nRoomNo, int nUserUniQNo );


};

#endif // !defined(AFX_WORLD_H__20F1A2A3_0432_44F1_8B96_7E87E1C3D2C3__INCLUDED_)
