#include "CopyManager.h"
#include <time.h>
#include "GameServer.h"
#include "MapManager.h"
#include "StackStream.h"
#include "map.h"
#include "TaskManager.h"
#include "Team.h"
#include "SocialManager.h"
#include "CSVFileManager.h"

CopyManager* g_CopyManager = NULL;

void CopyManager::OnApplyCopy(MsgBase* msg)
{
	Player* player = g_MapManager->GetPlayerByID(msg->OBJID);
	MsgCopy_Apply* copyMsg = (MsgCopy_Apply*)msg;
	const CopyInfo* copyInfo = g_CSVFileManager->GetCopyByID(copyMsg->copyId);

	if(player->m_Attribute[ATTRIBUTE_LEVEL].GetValueI() < copyInfo->iLevellimit)
	{
		MsgCopyNotify ttt(ID_G2C,Notify_S2C_ApplyCopy_LevelError,copyMsg->copyId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	if(copyInfo->iCopyType == 1 || copyInfo->iCopyType == 2)
	{
        Team* pTeam = g_SocailManager->GetTeam(player->m_teamId);
        if(pTeam || pTeam && pTeam->GetMembers().size() <= 1)
        {
            MsgCopyNotify ttt(ID_G2C,Notify_S2C_ApplyCopy_PlayerNumError,copyMsg->copyId,0);
            ttt.OBJID = msg->OBJID;
            g_GameServer->SendToClient(&ttt);
            return;
        }
	}
	if(copyInfo->iConsumeItemsid != 0)
	{
		bool rs = g_ItemManager->RemoveItem(player,ItemOperate_DoTakeItem,(uint32)copyInfo->iConsumeItemsid,1);
		if(!rs)
		{
			MsgCopyNotify ttt(ID_G2C,Notify_S2C_ApplyCopy_ItemError,copyMsg->copyId,0);
			ttt.OBJID = msg->OBJID;
			g_GameServer->SendToClient(&ttt);
			return;
		}
	}



	std::vector<CopyInfoIt>::iterator it1;
	for(it1 = player->m_CopyList.begin();it1 != player->m_CopyList.end();it1++)
	{
		if(it1->id == copyMsg->copyId)
		{
			if(it1->enterNum >= copyInfo->iNumlimit)
			{
				MsgCopyNotify ttt(ID_G2C,Notify_S2C_ApplyCopy_EnterNumError,copyMsg->copyId,0);
				ttt.OBJID = msg->OBJID;
				g_GameServer->SendToClient(&ttt);
				return;
			}
		}
	}
	int32 keepTime = copyInfo->iKeepTime;
	int32 keepHour = keepTime / 3600;
	int32 keepMin  = (keepTime % 3600) / 60;
	time_t now_time = time(NULL);
	tm* t = localtime(&now_time);
	int32 now_hour = t->tm_hour;
	int32 now_min = t->tm_min;
	bool isStart = false;
	std::vector<string> vecStartTime = copyInfo->vecStartTime;
	std::vector<string>::iterator it;
	int32 openTime[2];
	for(it = vecStartTime.begin(); it != vecStartTime.end();it++)
	{
		if (*it != "NULL" && *it != "0")
		{
			while ((*it).find_first_of(':') != -1)
			{
				openTime[0] = atoi((*it).substr(0,(*it).find_first_of(':')).c_str());
				openTime[1] = atoi((*it).substr((*it).find_first_of(':') + 1).c_str());
			}
			if(now_hour >= openTime[0] && now_min >= openTime[1] && now_hour <= openTime[0]+keepHour && now_min <= openTime[1]+keepMin)
			{
				isStart = true;
				break;
			}
		}
	}
	if(!isStart)
	{
		MsgCopyNotify ttt(ID_G2C,Notify_S2C_ApplyCopy_TimeError,copyMsg->copyId,0);
		ttt.OBJID = msg->OBJID;
		g_GameServer->SendToClient(&ttt);
		return;
	}
	bool isFindCopy = false;
	CopyInfoIt copyInfoIt;
	for(it1 = player->m_CopyList.begin();it1 != player->m_CopyList.end();it1++)
	{
		if(it1->id == copyMsg->copyId)
		{
			it1->enterNum += 1;
			copyInfoIt = *it1;
			isFindCopy = true;
			break;
		}
	}
	if(!isFindCopy)
	{
		copyInfoIt.id = copyMsg->copyId;
		copyInfoIt.enterNum = 1;
		copyInfoIt.layer = 1;
	}


	g_MapManager->OpenMap(copyInfo->iMapid);

	//uint64 mapGuid =  player->m_map->GetMapID();

	//通知客户端进入副本地图成功
	MsgCopyNotify ttt(ID_G2C,Notify_S2C_ApplyCopy_OK,copyMsg->copyId,0);
	ttt.OBJID = msg->OBJID;
	g_GameServer->SendToClient(&ttt);

	MsgCopy_Apply udpCopy= *copyMsg;
	StackStream<> udpCopyStream;
	udpCopy.SysID = ID_G2D;
	udpCopyStream.Wirte(&udpCopy, sizeof(udpCopy));

	udpCopyStream.Wirte(&copyInfoIt.id, sizeof(int));
	udpCopyStream.Wirte(&copyInfoIt.enterNum, sizeof(int));
	udpCopyStream.Wirte(&copyInfoIt.layer, sizeof(int));
	//通知服务器
	MsgCopy_Apply* pUdpTask = (MsgCopy_Apply*)udpCopyStream.Get();
	pUdpTask->Len = udpCopyStream.GetSize();
	g_GameServer->SendToDB(pUdpTask);
}

void CopyManager::updateCopyProgress(Player* player,int monsterId,int itemId /* = 0 */)
{
	bool isFinish = false;
	int mapId = player->m_Map->GetMapID();
	std::map<int32,CopyInfo>::iterator itCopy;
	CopyInfo* copyInfo =NULL;
	for(itCopy = g_CSVFileManager->m_CopyInfoMap.begin();itCopy != g_CSVFileManager->m_CopyInfoMap.end();itCopy++)
	{
		copyInfo = &itCopy->second;
		if(copyInfo->iMapid ==  mapId)
		{
			break;
		}
	}

	int32 copyId = 0;
	if(monsterId != 0)
	{
		if(copyInfo->iConditionsValue == 0 && copyInfo->iConditions==1)
		{
			int n = g_MapManager->m_MonsterList.size();
			if(n == 0)
			{
				copyId = copyInfo->id;
				isFinish = true;
			}
		}
		else if(copyInfo->iConditions==2 && copyInfo->iConditionsValue == monsterId)
		{
			copyId = copyInfo->id;
			isFinish = true;
		}
	}
	else
	{
		if(copyInfo->iConditionsValue == itemId && copyInfo->iConditions==3)
		{
			int sumItem = g_ItemManager->GetTotalNumber(player,itemId);
			if(sumItem >= copyInfo->iConditionsNum)
			{
				copyId = copyInfo->id;
				isFinish = true;
			}
		}
	}

	std::vector<CopyInfoIt>::iterator it;
	if(isFinish)
	{
		if(copyInfo->iCopyType ==3)
		{
			for(it = player->m_CopyList.begin();it != player->m_CopyList.end();it++)
			{
				if(it->id == copyId)
				{
					if(it->layer < copyInfo->iConditionsNum)
					{
						it->layer += 1;
						MsgCopy_Finish copyFinish;
						copyFinish.copyInfoIt = *it;
						g_GameServer->SendToDB(&copyFinish);

						MsgCopyNotify ttt(ID_G2C,Notify_S2C_Copy_FinishANDNext,copyId,0);
						ttt.OBJID = player->m_GUID;
						g_GameServer->SendToClient(&ttt);
						break;
					}
					else
					{
						g_TaskManager->OnCopyPass(copyId,player);
						MsgCopyNotify ttt(ID_G2C,Notify_S2C_Copy_Finish,copyId,0);
						ttt.OBJID = player->m_GUID;
						g_GameServer->SendToClient(&ttt);
						break;
					}
					
				}
			}
		}
		else
		{
			g_TaskManager->OnCopyPass(copyId,player);
			MsgCopyNotify ttt(ID_G2C,Notify_S2C_Copy_Finish,copyId,0);
			ttt.OBJID = player->m_GUID;
			g_GameServer->SendToClient(&ttt);
		}
		
	}
	
}
