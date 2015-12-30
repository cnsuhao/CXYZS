/********************************************************************
created:	2014/12/21
author:		LL
desc:		用于定义游戏中的基础类型及数值限定。
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

#define PLAYER_ANIMATION_FRAME_TIME 0.07f	//角色动作每帧时间

//最大传送点数量
#define CONVEY_NUM_MAX 5

//游戏场景类型
enum GameScene
{	
	SCENE_UPDATE,	//更新版本
	SCENE_LOGIN,	//登陆
	SCENE_LOAD,		//加载,用于加载资源或者作为其他场景之间的过渡
	SCENE_MAIN,		//主场景
	SCENE_COPY,		//副本
};

//游戏大图层定义，可根据需要插入图层、修改层级关系
enum GameLayer
{
	LAYER_LISTENER,			//监听层
	LAYER_BACKGROUND,		//背景层
	LAYER_MAP,				//地图层
	LAYER_MAPOBJ,			//地图装饰层
	LAYER_OBJECT,			//人物层
	LAYER_HP,				//掉血
	LAYER_EFFECT,			//战斗特效
	LAYER_UI,				//UI层
	LAYER_MSGTIP,			//消息提示层
	LAYER_MSGBOX,			//消息对话层
	LAYER_SCENE_EFFECT,		//屏幕特效层
	LAYER_GUIDE,			//引导层
};

//动作类型
enum ActionType
{
	ACTION_IDLE,			//待机
	ACTION_HORSE_IDLE,		//骑马待机
	ACTION_DRIVE_IDLE,		//御剑待机
	ACTION_MOVE,			//移动	
	ACTION_HORSE_MOVE,		//骑马移动
	ACTION_DRIVE_MOVE,		//御剑移动
	ACTION_BIRTH,			//出生
	ACTION_DEAD,			//死亡	
	ACTION_ATTACK,			//攻击0
	ACTION_ATTACK1,			//攻击1
	ACTION_ATTACK2,			//攻击2
	ACTION_ATTACK3,			//攻击3
	ACTION_MAX
};
//方向类型
enum DirectionType
{	
	//0上、1右上、2右、3右下、4下、5左下、6左、7左上
	DIRECTION_UP,			//上
	DIRECTION_RIGHT_UP,		//右上
	DIRECTION_RIGHT,		//右
	DIRECTION_RIGHT_DOWN,	//右下
	DIRECTION_DOWN,			//下
	DIRECTION_LEFT_DOWN,	//左下
	DIRECTION_LEFT,			//左
	DIRECTION_LEFT_UP,		//左上
	DIRECTION_MAX
};

#endif	//__CLIENTDEF_H__