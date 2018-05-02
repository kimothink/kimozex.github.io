#ifndef __DEFINE_H
#define __DEFINE_H

#define STR_BUFFER				128
#define TEMP_BUFFER				1024

#define OPT_USEC	2000
#define PASSES_PER_SEC	(1000000 / OPT_USEC)	
#define RL_SEC		* PASSES_PER_SEC


// RL_SEC = 10
#define PULSE_SAVE_ALL			( 20 RL_SEC)	
#define PULSE_CONNECT_TIMEOUT	( 300 RL_SEC)	
#define LOSE_CONNECT_TIMEOUT	( 10 RL_SEC )	


#define	PULSE_GAME_TIMEOUT		( 15 RL_SEC )	
#define PULSE_MOBILE_CONNECT_TIMEOUT	( 300 RL_SEC ) 


#define PULSE_DB_TIMEOUT		( 20 RL_SEC )	
#define PULSE_FRIST_TIMEOUT		( 30 RL_SEC )	

#define DAY_LOOPING				864000			

#define MAX_BUFFER_SIZE			2048
//*******************RECEIVE SOCKET BUFFER*******************************//
//클라이언트로 받아서 분리한후 큐에 넣는 버퍼길이.
#define MAX_INPUT_LENGTH		MAX_BUFFER_SIZE					//Max length per *line* of input
//한 cmd의 길이가 이것을 넘으면 안된다. 실제들어온 사이즈
#define MAX_RAW_INPUT_LENGTH	MAX_BUFFER_SIZE					/* Max size of *raw* input */
//BufferSize  // ClientSocket에서 들어온 하나이 패킷이다.
#define INPUT_Q_BUFSIZE			MAX_BUFFER_SIZE					/* Static output buffer size   */
//***********************************************************************//
//*******************SEND(LISTEN)SOCKET BUFFER***************************//
#define MAX_SOCK_BUF            (12 * 1024)		/* Size of kernel's sock buf   */
//***********************************************************************//



#define MAX_PROMPT_LENGTH       96				/* Max length of prompt        */
#define GARBAGE_SPACE			32				/* Space for **OVERFLOW** etc  */
#define SMALL_BUFSIZE			1024			/* Static output buffer size   */



/* Max amount of output that can be buffered */
#define LARGE_BUFSIZE	   (MAX_SOCK_BUF - GARBAGE_SPACE - MAX_PROMPT_LENGTH)

//#define MAX_BUFFER				8192
#define NORMAL_SEND_BUFFER		1024
#define MID_SEND_BUFFER			2048
#define LARGE_SEND_BUFFER		5000

#define MAX_STRING_LENGTH		4096			// temp Buffer

#define HOST_LENGTH				 40		/* Used in char_file_u *DO*NOT*CHANGE* */

#define CHANNEL_PER_MASHINE		10



#define STATE(d)			((d)->m_nUserMode) //UserMode

//디비를 기다려야 하는지. ( 디비를 기다리는동안에 패킷이 날라오면 안되니깐.
#define WAITTING_DB(d)	((d)->m_bWaitDb)

//디비를 업데이트만 하는지. ( 업데이트가 너무 오래걸리면 짤라야하니깐. )
#define UPDATE_DB(d) ((d)->m_bUpdateDb)

//디비를 호출한후에 실해하면 
#define ACCESSED_DB(d)	((d)->m_bDbAccess)

//유저가 게임방인지 대기실인지 알기위해.
#define USERPOSITION(d)	((d)->m_nGameMode)


#define ISNEWL(ch) ((ch) == '~') 
#define ISNEWLP(ch) ((ch) == '|') 

#define CLOSE_SOCKET(sock)	closesocket(sock)
#define SEND_TO_Q(messg, desc, length)  (desc)->WriteToOutput(messg, length)


#define INIT_USER_NO			10			//처음 유저 넘버 시작.


#define MAX_Q_NUM  100000

#define SAFE_DELETE(pointer)				if ( (pointer) != NULL ) { delete (pointer);	(pointer) = NULL; }
#define SAFE_DELETE_ARRAY(pointer)			if ( (pointer) != NULL ) { delete [] (pointer);	(pointer) = NULL; }

#define is_space(c)		(c == 0x20 || (c >= 0x09 && c <= 0x0D))



#define REMOVE_FROM_LIST(item, head, next)	\
			if ((item) == (head))		\
				head = (item)->next;		\
			else {				\
				temp = head;			\
				while (temp && (temp->next != (item))) \
					temp = temp->next;		\
				if (temp)				\
					temp->next = (item)->next;	\
			}					\
			

#define REMOVE_CHANNEL_LIST( DeleteUser, List, m_User, m_NextList ) \
			if ( List != NULL ) {							\
				if ( List->m_User == DeleteUser ) {			\
					pTemp = List;							\
					List = List->m_NextList;				\
					SAFE_DELETE(pTemp);						\
				}											\
				else {										\
					pTemp = List;							\
					while ( pTemp && ( pTemp->m_NextList->m_User != DeleteUser ) )  \
						pTemp = pTemp->m_NextList;									\
					if (pTemp) {													\
						pTempNext = pTemp->m_NextList;								\
						pTemp->m_NextList = pTemp->m_NextList->m_NextList;			\
						SAFE_DELETE(pTempNext);										\
					}																\
				}																	\
			}																	

  
//REMOVE_FROM_LIST(d, m_UserDescriptorList, m_pNext);

#define CREATE(result, type, number)  do {\
	if ((number) <= 0)	\
		LogFile("SYSERR: Zero bytes or less requested at");	\
	if (!((result) = new type [(number)]))	\
		{ LogFile ("SYSERR: new failure"); exit(0); } } while(0)

#define CLOSE_SOCKET(sock)	closesocket(sock)

#define LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))



//RAMDOM
#define	m  (unsigned long)2147483647
#define	q  (unsigned long)127773

#define	a (unsigned int)16807
#define	r (unsigned int)2836


//**********************************
//USERSTATE
#define CMD_WAIT				0		
#define DBSEARCH_ISLOGIN		1





#define NOTICE_READ_TIME		3000   
#define NOTICE_BYTE				100

#define DBCONNECT


#endif



