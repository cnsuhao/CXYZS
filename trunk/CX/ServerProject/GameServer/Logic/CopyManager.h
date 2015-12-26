#ifndef __COPYMANAGER_H__
#define __COPYMANAGER_H__
#include "GameDef.h"
#include "Player.h"
#include "MsgInfo.h"
class CopyManager
{
public:
	void OnApplyCopy(MsgBase* msg);
	void updateCopyProgress(Player* player,int monsterId,int itemId = 0);
};
extern CopyManager* g_CopyManager;
#endif