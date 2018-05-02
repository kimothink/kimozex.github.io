#ifndef __DEFINE_H
#define __DEFINE_H

#define STR_BUFFER				128
#define TEMP_BUFFER				1024

#define OPT_USEC	1111
#define PASSES_PER_SEC	(1000000 / OPT_USEC)
#define RL_SEC		* PASSES_PER_SEC



// RL_SEC = 10
#define PULSE_SAVE_ALL			( 20 RL_SEC)	


#define PULSE_CONNECT_TIMEOUT	120	
#define LOSE_CONNECT_TIMEOUT	20



#define PULSE_COUNTER_TIMEOUT	300000 

#define PULSE_MOBILE_CONNECT_TIMEOUT	( 300 RL_SEC ) 


#define PULSE_DB_TIMEOUT		( 20 RL_SEC )	

#define PULSE_FRIST_TIMEOUT		( 30 RL_SEC )	


#define DAY_LOOPING				864000			


#define MAX_BUFFER_SIZE			2048
//*******************RECEIVE SOCKET BUFFER*******************************//
//Ŭ���̾�Ʈ�� �޾Ƽ� �и����� ť�� �ִ� ���۱���.
#define MAX_INPUT_LENGTH		MAX_BUFFER_SIZE					//Max length per *line* of input
//�� cmd�� ���̰� �̰��� ������ �ȵȴ�. �������� ������
#define MAX_RAW_INPUT_LENGTH	MAX_BUFFER_SIZE					/* Max size of *raw* input */
//BufferSize  // ClientSocket���� ���� �ϳ��� ��Ŷ�̴�.
#define INPUT_Q_BUFSIZE			MAX_BUFFER_SIZE					/* Static output buffer size   */
//***********************************************************************//
//*******************SEND(LISTEN)SOCKET BUFFER***************************//
#define MAX_SOCK_BUF            (12 * 1024)		/* Size of kernel's sock buf   */
//***********************************************************************//



#define MAX_PROMPT_LENGTH       96				/* Max length of prompt        */
#define GARBAGE_SPACE			32				/* Space for **OVERFLOW** etc  */
#define SMALL_BUFSIZE			4096			/* Static output buffer size   */



/* Max amount of output that can be buffered */
#define LARGE_BUFSIZE	   (MAX_SOCK_BUF - GARBAGE_SPACE - MAX_PROMPT_LENGTH)

//#define MAX_BUFFER				8192
#define NORMAL_SEND_BUFFER		2048
#define MID_SEND_BUFFER			2048
#define LARGE_SEND_BUFFER		5000

#define MAX_STRING_LENGTH		4096			// temp Buffer

#define HOST_LENGTH				 40		/* Used in char_file_u *DO*NOT*CHANGE* */




#define STATE(d)			((d)->m_nUserMode) //UserMode
#define STATEDBMODE(d)      ((d)->m_nDBMode)

//��� ��ٷ��� �ϴ���. ( ��� ��ٸ��µ��ȿ� ��Ŷ�� ������� �ȵǴϱ�.
#define WAITTING_DB(d)	((d)->m_bWaitDb)

//��� ������Ʈ�� �ϴ���. ( ������Ʈ�� �ʹ� �����ɸ��� ©����ϴϱ�. )
#define UPDATE_DB(d) ((d)->m_bUpdateDb)

//��� ȣ�����Ŀ� �����ϸ� 
#define ACCESSED_DB(d)	((d)->m_bDbAccess)

//������ ���ӹ����� �������� �˱�����.
#define USERPOSITION(d)	((d)->m_nGameMode)


#define ISNEWL(ch) ((ch) == '~') 
#define ISNEWLP(ch) ((ch) == '|') 

#define CLOSE_SOCKET(sock)	closesocket(sock)
#define SEND_TO_Q(messg, desc, length)  (desc)->WriteToOutput(messg, length)


#define INIT_USER_NO			1			//ó�� ���� �ѹ� ����.


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


#define MAX_BURDEN_WEIGHT_PER		10	  
#define MAX_BURDEN_WEIGHT			20    




//**********************************
//USERSTATE
#define CMD_WAIT				0		

#define NONE					1
#define GAME_RESULT_UPDATE		2	
//#define GET_AVATAR				3
#define GAMING_EXITUPDATE		4		
//#define DBUP					5
#define GETUSER					6
//#define GAME_RESULT_UPDATEEND	2	


#define CLIENT_WILL_DISCONNECT	101		
#define CLIENT_DISCONNECTED		102		
/**********************************/

//*********************************
//GameMode
#define USERNONE				0		
#define USERWAIT				1	
#define USERGAME				2	



#define REALTIME_REFRESH




#define DBCONNECT
#define PACKETANALY


// Tick�� ��� ����(�� ����)
#define TICS_GAP_SEC             1    // �⺻ tics�� 1��
#define REFRESH_TICS             86400 // 24�ð����� tics�� ���� ����


#endif
