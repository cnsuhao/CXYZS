/********************************************************************
created:	2014/7/15
author:		LL
desc:		���ڶ���������ڲ����ݡ�
*********************************************************************/
#ifndef _SERVERDEF__H_
#define _SERVERDEF__H_

#include "MsgInfo.h"


#define FRAME_GET_MSG_MAX 100		//ÿ֡���������Ϣ����
#define LINK_TIMEOUT_TIME 100000	//���ӳ�ʱʱ��ms

#define CUR_CLIENT_VERSION_MAIN		1	//�汾��
#define CUR_CLIENT_VERSION_SUB		1	//�汾��


#define SERVER_CONNECTION_PASSWORD	 "~Game@44445~"	//����������������������

#define GATESERVER_CONNECTIONS_MAX	50000	//���ط����������������
#define LOGINSERVER_CONNECTIONS_MAX	500		//��½�����������������
#define GAMESERVER_CONNECTIONS_MAX	4		//��Ϸ�����������������
#define DBSERVER_CONNECTIONS_MAX	2		//���ݿ�����������������



#endif	//_SERVERDEF__H_