/********************************************************************
created:	2014/12/21
author:		LL
desc:		���ڶ�����Ϸ�еĻ������ͼ���ֵ�޶���
*********************************************************************/

#ifndef __CLIENTDEF_H__
#define __CLIENTDEF_H__
#include <string.h>
#include "MsgInfo.h"
#include "CCVector.h"
#include "CCNode.h"
#include "CCSprite.h"
#include "math/Vec2.h"

using namespace std;

#define PLAYER_ANIMATION_FRAME_TIME 0.07f	//��ɫ����ÿ֡ʱ��

//����͵�����
#define CONVEY_NUM_MAX 5

//��Ϸ��������
enum GameScene
{	
	SCENE_UPDATE,	//���°汾
	SCENE_LOGIN,	//��½
	SCENE_LOAD,		//����,���ڼ�����Դ������Ϊ��������֮��Ĺ���
	SCENE_MAIN,		//������
	SCENE_COPY,		//����
};

//��Ϸ��ͼ�㶨�壬�ɸ�����Ҫ����ͼ�㡢�޸Ĳ㼶��ϵ
enum GameLayer
{
	LAYER_LISTENER,			//������
	LAYER_BACKGROUND,		//������
	LAYER_MAP,				//��ͼ��
	LAYER_MAPOBJ,			//��ͼװ�β�
	LAYER_OBJECT,			//�����
	LAYER_HP,				//��Ѫ
	LAYER_EFFECT,			//ս����Ч
	LAYER_UI,				//UI��
	LAYER_MSGTIP,			//��Ϣ��ʾ��
	LAYER_MSGBOX,			//��Ϣ�Ի���
	LAYER_SCENE_EFFECT,		//��Ļ��Ч��
	LAYER_GUIDE,			//������
};

//��������
enum ActionType
{
	ACTION_IDLE,			//����
	ACTION_HORSE_IDLE,		//�������
	ACTION_DRIVE_IDLE,		//��������
	ACTION_MOVE,			//�ƶ�	
	ACTION_HORSE_MOVE,		//�����ƶ�
	ACTION_DRIVE_MOVE,		//�����ƶ�
	ACTION_BIRTH,			//����
	ACTION_DEAD,			//����	
	ACTION_ATTACK,			//����0
	ACTION_ATTACK1,			//����1
	ACTION_ATTACK2,			//����2
	ACTION_ATTACK3,			//����3
	ACTION_MAX
};
//��������
enum DirectionType
{	
	//0�ϡ�1���ϡ�2�ҡ�3���¡�4�¡�5���¡�6��7����
	DIRECTION_UP,			//��
	DIRECTION_RIGHT_UP,		//����
	DIRECTION_RIGHT,		//��
	DIRECTION_RIGHT_DOWN,	//����
	DIRECTION_DOWN,			//��
	DIRECTION_LEFT_DOWN,	//����
	DIRECTION_LEFT,			//��
	DIRECTION_LEFT_UP,		//����
	DIRECTION_MAX
};

#endif	//__CLIENTDEF_H__