#ifndef _SERVERCLIENTCOMMON_H
#define _SERVERCLIENTCOMMON_H

#include "NetWorkDefine.h"


BOOL AvailableRoomIn( BigInt biRoomMoney, BigInt biUserMoney, BOOL bMaxCheck = TRUE )
{
	BOOL bRe = FALSE;
	switch( biRoomMoney ) {
		case GAMEROOM_MONEY1:
			if ( GAMEROOM_MIN1 <= biUserMoney )
				bRe = TRUE;
			break;

		case GAMEROOM_MONEY2:
			if ( GAMEROOM_MIN2 <= biUserMoney )
				bRe = TRUE;
			break;

		case GAMEROOM_MONEY3:
			if ( GAMEROOM_MIN3 <= biUserMoney )
				bRe = TRUE;
			break;

		case GAMEROOM_MONEY4:
			if ( GAMEROOM_MIN4 <= biUserMoney )
				bRe = TRUE;
			break;

		case GAMEROOM_MONEY5:
			if ( GAMEROOM_MIN5 <= biUserMoney )
				bRe = TRUE;
			break;

		case GAMEROOM_MONEY6:
			if ( GAMEROOM_MIN6 <= biUserMoney )
				bRe = TRUE;
			break;

        case GAMEROOM_MONEY7:
			if ( GAMEROOM_MIN7 <= biUserMoney )
				bRe = TRUE;
			break;

		case GAMEROOM_MONEY8:
			if ( GAMEROOM_MIN8 <= biUserMoney )
				bRe = TRUE;
			break;
	}

	return bRe;
}








#endif _SERVERCLIENTCOMMON_H