#ifndef __ALARMMSG_H__
#define __ALARMMSG_H__

//#include "DX2DEngine.h"//ham




//char l_szRoomName[255] , l_szTempRoom[80]; // Ÿ��Ʋ �̸�

class GAMEMESSAGE
{
private:

	char szText[255] , szText2[255]; // �� �޼���
	BOOL bShowCheck;
	
public:

	GAMEMESSAGE(); // ������	
	
	void PutMessage( char *szString , char *szString2, bool bCloseGame = false );

	BOOL GetbShowCheck( void );
	void SetbShowCheck( BOOL Check );

	void Draw();

	bool m_bCloseGame;
};

//GAMEMESSAGE *l_Message = NULL; // �˶� �޽���








#endif