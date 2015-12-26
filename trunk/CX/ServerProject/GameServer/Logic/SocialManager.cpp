#include "SocialManager.h"
#include "MapManager.h"
#include "GameServer.h"
#include "StackStream.h"
#include <time.h>
#include "Team.h"
#include "Map.h"

SocailManager *g_SocailManager = NULL;


SocailManager::SocailManager()
{
	m_teamCapacity = 0;
}

SocailManager::~SocailManager()
{
    Team* pTeam = NULL;
    auto it = m_teams.begin();
    for (auto end = m_teams.end(); it != end; ++it)
    {
        pTeam = *it;
        if (pTeam)
        {
            delete pTeam;
        }
    }

    while (m_freeTeams.size() > 0)
    {
        pTeam = m_freeTeams.front();
        delete pTeam;
        m_freeTeams.pop();
    }

}

//--------------------------------------------------------------------------------------------���ѹ���
//��Ӻ���
bool SocailManager::AddFriends(MsgBase* msg)
{
	MsgFriends* temp = (MsgFriends*)msg;

	Player *pplay = g_MapManager->GetPlayerByID(temp->iRequestRoleID);
	Player *pTarget = g_MapManager->GetPlayerByID(temp->iAddRoleID);
	if (pplay && pTarget)
	{
		//�ж�
		if (temp->iType == Relation_Friends)
		{
			bool bIsRepeat = false;
			vector<FriendsInfo>::iterator result;
			for (result =  pplay->m_vecFriendsList.begin(); result != pplay->m_vecFriendsList.end(); ++result)
			{
				if (result->roleId == pTarget->m_GUID)
				{
					bIsRepeat = true;
					break;
				}
			}
			if ( !bIsRepeat )
			{
				FriendsInfo tempF;
				tempF.roleId = pTarget->m_GUID;
				tempF.bIsLine = 1;
				tempF.iLevel = pTarget->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
				tempF.iFight = pTarget->m_BattlePower;
				strncpy(tempF.szName,pTarget->m_Name, MAX_NAME_SIZE);
				pplay->m_vecFriendsList.push_back(tempF);		//��Ӻ���
				//ת�����ݿ������
				strncpy(temp->szName,pTarget->m_Name, MAX_NAME_SIZE);
				msg->SysID = ID_G2D;
				g_GameServer->SendToDB(msg);

				//֪ͨ������
				MsgFriNotify ttt(ID_G2C, Notify_S2C_ADDFRI_OK,pTarget->m_GUID,0);
				ttt.OBJID = temp->OBJID;
				g_GameServer->SendToClient(&ttt);

				//֪ͨ��������
				bIsRepeat = false;
				vector<FriendsInfo>::iterator resultex;
				for (resultex =  pTarget->m_vecFriendsList.begin(); resultex != pTarget->m_vecFriendsList.end(); ++resultex)
				{
					if (resultex->roleId == pplay->m_GUID)
					{
						bIsRepeat = true;
						break;
					}
				}

				if (!bIsRepeat)
				{
					MsgFriNotify tttex(ID_G2C, Notify_S2C_ADDEDFRI_OK,pplay->m_GUID,0);
					tttex.OBJID = temp->iAddRoleID;
					StackStream<1024> friendListStream;
					friendListStream.Wirte(&tttex,sizeof(MsgFriNotify));
					friendListStream.Wirte(&pTarget->m_Name,MAX_NAME_SIZE);
					MsgFriNotify* pAddMsg = (MsgFriNotify*)friendListStream.Get();
					pAddMsg->Len = friendListStream.GetSize();
					g_GameServer->SendToClient(pAddMsg);
				}
				
			}
			else
			{
				//֪ͨ�ͻ���
				MsgFriNotify ttt(ID_G2C, Notify_S2C_ADDFRI_FAILURE,pplay->m_GUID,0);
				ttt.OBJID = temp->OBJID;
				g_GameServer->SendToClient(&ttt);
			}
		}
		else if (temp->iType == Relation_BlackList)
		{
			bool bIsRepeat = false;
			vector<FriendsInfo>::iterator result;
			for (result =  pplay->m_vecBlackList.begin(); result != pplay->m_vecBlackList.end(); ++result)
			{
				if (result->roleId == pTarget->m_GUID)
				{
					bIsRepeat = true;
					break;
				}
			}

			if ( !bIsRepeat )
			{
				FriendsInfo tempF;
				tempF.roleId = pTarget->m_GUID;
				tempF.bIsLine = 1;
				tempF.iLevel = pTarget->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
				tempF.iFight = pTarget->m_BattlePower;
				sprintf(tempF.szName,pTarget->m_Name);
				pplay->m_vecBlackList.push_back(tempF);		//��Ӻ�����
				//ת�����ݿ������
				msg->SysID = ID_G2D;
				g_GameServer->SendToDB(msg);

				//֪ͨ��ӳɹ�
				MsgFriNotify ttt(ID_G2C, Notify_S2C_ADDBLACK_OK,pTarget->m_GUID,0);
				ttt.OBJID = temp->OBJID;
				g_GameServer->SendToClient(&ttt);
			}
			else
			{
				//֪ͨ�ͻ���
				MsgFriNotify ttt(ID_G2C, Notify_S2C_ADDBLACK_FAILURE,pplay->m_GUID,0);
				ttt.OBJID = temp->OBJID;
				g_GameServer->SendToClient(&ttt);
			}
		}
	}
	else
	{
		//֪ͨ�ͻ���
		MsgFriNotify ttt(ID_G2C, Notify_S2C_FRINULL,0,NULL);
		ttt.OBJID = temp->OBJID;
		g_GameServer->SendToClient(&ttt);
	}
	
	return true;
}

//ɾ������
bool SocailManager::DelFriends(MsgBase* msg)		
{
	MsgFriends* temp = (MsgFriends*)msg;

	Player *pplay = g_MapManager->GetPlayerByID(temp->iRequestRoleID);
	if (pplay)
	{
		//�ж�
		if (temp->iType == Relation_Friends)
		{
			vector<FriendsInfo>::iterator result;
			for (result =  pplay->m_vecFriendsList.begin(); result != pplay->m_vecFriendsList.end(); ++result)
			{
				if (result->roleId == temp->iAddRoleID)
				{
					pplay->m_vecFriendsList.erase(result);

					//ת�����ݿ������
					msg->SysID = ID_G2D;
					g_GameServer->SendToDB(msg);

					//֪ͨ�ͻ���
					MsgFriNotify ttt(ID_G2C, Notify_S2C_DELFRI_OK,temp->iAddRoleID,0);
					ttt.OBJID = temp->OBJID;
					g_GameServer->SendToClient(&ttt);

					break;
				}
			}
		}
		else if(temp->iType == Relation_BlackList)
		{
			vector<FriendsInfo>::iterator result;
			for (result =  pplay->m_vecBlackList.begin(); result != pplay->m_vecBlackList.end(); ++result)
			{
				if (result->roleId == temp->iAddRoleID)
				{
					pplay->m_vecBlackList.erase(result);

					//ת�����ݿ������
					msg->SysID = ID_G2D;
					g_GameServer->SendToDB(msg);

					//֪ͨ�ͻ���
					MsgFriNotify ttt(ID_G2C, Notify_S2C_DELBLACK_OK,temp->iAddRoleID,0);
					ttt.OBJID = temp->OBJID;
					g_GameServer->SendToClient(&ttt);

					break;
				}
			}
		}
	}
	else
	{
		//֪ͨ�ͻ���
		MsgFriNotify ttt(ID_G2C, Notify_S2C_FRINULL,0,NULL);
		ttt.OBJID = temp->OBJID;
		g_GameServer->SendToClient(&ttt);
	}
	return true;
}

//�����б�
bool SocailManager::GetFriendsList(MsgBase* msg)
{
	MsgFriends* temp = (MsgFriends*)msg;
	Player *pplay = g_MapManager->GetPlayerByID(temp->iRequestRoleID);

	if (temp->iType == Relation_Friends)
	{
		//֪ͨ�ͻ���
		MsgFriNotify ttt(ID_G2C, Notify_S2C_GETFRILIST_OK,pplay->m_GUID,pplay->m_vecFriendsList.size());
		ttt.OBJID = temp->OBJID;

		StackStream<4096> friendListStream;
		friendListStream.Wirte(&ttt,sizeof(MsgFriNotify));

		FriendsInfo pFriend;
		vector<FriendsInfo>::iterator iVec;
		for ( iVec = pplay->m_vecFriendsList.begin(); iVec != pplay->m_vecFriendsList.end();++iVec)
		{
			pFriend = *iVec;
			//�����Ƿ�����
			Player *tempPlay = g_MapManager->GetPlayerByID(iVec->roleId);
			if (tempPlay)
			{
				pFriend.bIsLine = 1;
				friendListStream.Wirte(&tempPlay->m_GUID,sizeof(uint64));
				friendListStream.Wirte(&tempPlay->m_Name,MAX_NAME_SIZE);
				friendListStream.Wirte(&pFriend.bIsLine,sizeof(bool));
				friendListStream.Wirte(&tempPlay->m_BattlePower,sizeof(int));
				int tempValue = tempPlay->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
				friendListStream.Wirte(&tempValue,sizeof(int));
			}
			else
			{
				pFriend.bIsLine = 0;
				friendListStream.Wirte(&pFriend.roleId,sizeof(uint64));
				friendListStream.Wirte(&pFriend.szName,MAX_NAME_SIZE);
				friendListStream.Wirte(&pFriend.bIsLine,sizeof(bool));
				friendListStream.Wirte(&pFriend.iFight,sizeof(int));
				friendListStream.Wirte(&pFriend.iLevel,sizeof(int));
			}
		}

		MsgFriNotify* pAddMsg = (MsgFriNotify*)friendListStream.Get();
		pAddMsg->Len = friendListStream.GetSize();
		//>���͵��ͻ���
		g_GameServer->SendToClient(pAddMsg);
		
	}
	else if (temp->iType == Relation_BlackList)
	{
		//֪ͨ�ͻ���
		MsgFriNotify ttt(ID_G2C, Notify_S2C_GETBLACKLIST_OK,pplay->m_GUID,pplay->m_vecBlackList.size());
		ttt.OBJID = temp->OBJID;

		StackStream<4096> friendListStream;
		friendListStream.Wirte(&ttt,sizeof(MsgFriNotify));

		FriendsInfo pFriend;
		vector<FriendsInfo>::iterator iVec;
		for ( iVec = pplay->m_vecBlackList.begin(); iVec != pplay->m_vecBlackList.end();++iVec)
		{
			pFriend = *iVec;

			//�����Ƿ�����
			Player *tempPlay = g_MapManager->GetPlayerByID(iVec->roleId);
			if (tempPlay)
			{
				pFriend.bIsLine = 1;
				friendListStream.Wirte(&tempPlay->m_GUID,sizeof(uint64));
				friendListStream.Wirte(&tempPlay->m_Name,MAX_NAME_SIZE);
				friendListStream.Wirte(&pFriend.bIsLine,sizeof(bool));
				friendListStream.Wirte(&tempPlay->m_BattlePower,sizeof(int));
				friendListStream.Wirte(&tempPlay->m_Attribute[ATTRIBUTE_LEVEL],sizeof(int));
			}
			else
			{
				pFriend.bIsLine = 0;
				friendListStream.Wirte(&pFriend.roleId,sizeof(uint64));
				friendListStream.Wirte(&pFriend.szName,MAX_NAME_SIZE);
				friendListStream.Wirte(&pFriend.bIsLine,sizeof(bool));
				friendListStream.Wirte(&pFriend.iFight,sizeof(int));
				friendListStream.Wirte(&pFriend.iLevel,sizeof(int));
			}
		}

		MsgFriNotify* pAddMsg = (MsgFriNotify*)friendListStream.Get();
		pAddMsg->Len = friendListStream.GetSize();
		//>���͵��ͻ���
		g_GameServer->SendToClient(pAddMsg);
	}
	
	return true;
}

//�鿴������ϸ��Ϣ
bool SocailManager::GetFriendsInof(MsgBase* msg)
{
	MsgFriends* temp = (MsgFriends*)msg;

	Player *pTarget = g_MapManager->GetPlayerByName(temp->szName);
	if (pTarget)
	{
		//FriendsInfo tempFriend;
		
		//֪ͨ�ͻ���
		MsgFriNotify ttt(ID_G2C, Notify_S2C_GETFRI_OK,pTarget->m_GUID,1);
		ttt.OBJID = temp->OBJID;

		StackStream<1024> friendInfoStream;
		friendInfoStream.Wirte(&ttt,sizeof(MsgFriNotify));

		friendInfoStream.Wirte(&pTarget->m_GUID,sizeof(uint64));
		friendInfoStream.Wirte(&pTarget->m_Name,MAX_NAME_SIZE);
		friendInfoStream.Wirte(&pTarget->m_Attribute[ATTRIBUTE_LEVEL],sizeof(int));
		friendInfoStream.Wirte(&pTarget->m_BattlePower,sizeof(int));

		MsgFriNotify* pAddMsg = (MsgFriNotify*)friendInfoStream.Get();
		pAddMsg->Len = friendInfoStream.GetSize();
		//>���͵��ͻ���
		g_GameServer->SendToClient(pAddMsg);
	}
	else
	{
		//֪ͨ�ͻ���
		MsgFriNotify ttt(ID_G2C, Notify_S2C_FRINULL,0,NULL);
		ttt.OBJID = temp->OBJID;
		g_GameServer->SendToClient(&ttt);
	}

	return true;
}

//--------------------------------------------------------------------------------------------���칦��
//Ƶ����Ϣ
bool SocailManager::ChannelMessage(MsgBase* msg)
{
	MsgChat* pChatMsg = (MsgChat*)msg;
    Chat_Channel channel = (Chat_Channel)pChatMsg->iChannel;
    switch (channel)
    {
    case Channel_World:
        {
            stdext::hash_map<uint64, Player*>& playerList = g_MapManager->m_PlayerList;
            stdext::hash_map<uint64, Player*>::iterator it = playerList.begin();
            for (stdext::hash_map<uint64, Player*>::iterator end = playerList.end(); 
                 it != end; ++it)
            {
                Player* player = it->second;
                pChatMsg->OBJID = player->m_GUID;
                g_GameServer->SendToClient(pChatMsg);
            }
        }break;
    case Channel_SingleChat:
        {
        }
        break;
    case Channel_Near:
        {
            Player* player = g_MapManager->GetPlayerByID(pChatMsg->OBJID);
            if (player)
            {
                g_GameServer->SendToClient(pChatMsg);

                std::vector<Object*> players;
                player->m_Map->GetTargetRangeSet(20, player, player, players, Select_Player);
                if (!players.empty())
                {
                    auto it = players.begin();
                    for (auto end = players.end(); it != end; ++it)
                    {
                        pChatMsg->OBJID = (*it)->m_GUID;
                        g_GameServer->SendToClient(pChatMsg);
                    }
                }                
            }
        }break;
    case Channel_Guild:
        {
            Player* player = g_MapManager->GetPlayerByID(pChatMsg->OBJID);
            if (player)
            {
                if (player->m_SocietyID)
                {
                    Gang* pGang = g_GangManager->GetGangByID(player->m_SocietyID);
                    if (pGang)
                    {
                        auto it = pGang->m_mapMembers.begin();
                        for (auto end = pGang->m_mapMembers.end(); it != end; ++it)
                        {
                            pChatMsg->OBJID = it->first;
                            g_GameServer->SendToClient(pChatMsg);
                        }
                    }
                }
            }
        }break;
    case Channel_System:
        break;
    case Channel_Team:
        {
            auto it = g_MapManager->m_PlayerList.find(pChatMsg->OBJID);
            if (it != g_MapManager->m_PlayerList.end())
            {
                Player* player = it->second;
                Team* pTeam = g_SocailManager->GetTeam(player->m_teamId);
                if (pTeam)
                {
                    auto members = pTeam->GetMembers();
                    for (size_t i = 0; i < members.size(); ++i)
                    {
                        pChatMsg->OBJID = members[i].playerId;
                        g_GameServer->SendToClient(pChatMsg);
                    }
                }
            }
        }break;
    default:
        break;
    }

    
	return true;
}

//������Ϣ
bool SocailManager::PointMessage(MsgBase* msg)
{
	MsgChat* temp = (MsgChat*)msg;

	//MsgChatNotify tttex(ID_G2C, Notify_S2C_Recive_Point,temp->iSendRoleID,0,temp->szPar);
	//tttex.OBJID = temp->iType;
	//g_GameServer->SendToClient(&tttex);

	return true;
}

//--------------------------------------------------------------------------------------------���Ṧ��
//�������
bool SocailManager::CreateGang(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	Player *pTarget = g_MapManager->GetPlayerByID(temp->iPar[0]);

	bool bIsRepeat = false;
	string GangName(temp->szPar);

	if (g_GangManager->m_mapGang.size() > 0)
	{
		std::map<int64, Gang>::iterator iter = g_GangManager->m_mapGang.begin();
		for( ; iter != g_GangManager->m_mapGang.end(); ++iter)
		{
			if (!strcmp(GangName.c_str(), iter->second.m_szName))
			{
				bIsRepeat = true;
				break;
			}
		}
	}

	if (!bIsRepeat)
	{
		if(pTarget->m_SocietyID <= 0)
		{
			Gang tempGang;
			tempGang.m_id = g_GangManager->GetGangidForGangMap();
			strncpy(tempGang.m_szName, temp->szPar, MAX_NAME_SIZE);
			tempGang.m_iHeaderid = pTarget->m_GUID;
			strncpy(tempGang.m_szHeaderName,pTarget->m_Name, MAX_NAME_SIZE);
			GangMember tempGangMember;
			tempGangMember.id = pTarget->m_GUID;
			tempGangMember.iLevel = pTarget->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
			strncpy(tempGangMember.szName,pTarget->m_Name, MAX_NAME_SIZE);
			tempGang.m_mapMembers.insert(pair<int64,GangMember>(pTarget->m_GUID,tempGangMember));
			g_GangManager->m_mapGang.insert(pair<int64,Gang>(tempGang.m_id,tempGang));
			//ת�����ݿ������
			temp->iPar[1] = tempGang.m_id;
			msg->SysID = ID_G2D;
			g_GameServer->SendToDB(msg);

			MsgGangNotify tttex(ID_G2C, Notity_S2C_Create_OK,tempGang.m_id,0,0,"");
			tttex.OBJID = temp->OBJID;
			g_GameServer->SendToClient(&tttex);
		}
		else
		{
			MsgGangNotify tttex(ID_G2C, Notity_S2C_Create_Fail_InGang,temp->iPar[0],0,0,"");
			tttex.OBJID = temp->OBJID;
			g_GameServer->SendToClient(&tttex);
		}
	}
	else
	{
		MsgGangNotify tttex(ID_G2C, Notity_S2C_Create_Fail_RepeatName,temp->iPar[0],0,0,"");
		tttex.OBJID = temp->OBJID;
		g_GameServer->SendToClient(&tttex);
	}
	
	return true;
}

//��ð�����Ϣ
bool SocailManager::GetGangInfo(MsgBase* msg)
{
	MsgGang *temp = (MsgGang*)msg;
	Player *pTarget = g_MapManager->GetPlayerByID(temp->iPar[0]);
	Gang *tempGang = g_GangManager->GetGangByID(temp->iPar[1]);

	MsgGangNotify tttex(ID_G2C,Notity_S2C_Get_GangInfo,tempGang->m_mapMembers.size(),tempGang->m_mapApplys.size(),tempGang->m_vecGangLog.size(),"");
	tttex.OBJID = temp->iPar[0];
	StackStream<4096> gangInfoStream;

	gangInfoStream.Wirte(&tttex,sizeof(MsgGangNotify));
	gangInfoStream.Wirte(&tempGang->m_id,sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_szName,32);
	gangInfoStream.Wirte(&tempGang->m_iLevel,sizeof(int32));
	gangInfoStream.Wirte(&tempGang->m_iShopLevel,sizeof(int32));
	gangInfoStream.Wirte(&tempGang->m_iHeaderid,sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_szHeaderName,32);
	gangInfoStream.Wirte(&tempGang->m_iAct,sizeof(int32));
	gangInfoStream.Wirte(&tempGang->m_szAccounce,255);
	gangInfoStream.Wirte(&tempGang->m_iViveHeaderid[0],sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_iViveHeaderid[1],sizeof(int64));

	gangInfoStream.Wirte(&tempGang->m_iStarFlec[0],sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_iStarFlec[1],sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_iStarFlec[2],sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_iStarFlec[3],sizeof(int64));
	gangInfoStream.Wirte(&tempGang->m_iStarFlec[4],sizeof(int64));

	gangInfoStream.Wirte(&tempGang->m_szCreateTime,50);
	gangInfoStream.Wirte(&tempGang->m_iCopyid,sizeof(int32));
	gangInfoStream.Wirte(&tempGang->m_szStartTime,50);

	map<int64,GangMember>::iterator itt = tempGang->m_mapMembers.begin();
	for (; itt != tempGang->m_mapMembers.end() ; ++itt)
	{
		gangInfoStream.Wirte(&itt->second.id,sizeof(int64));
		gangInfoStream.Wirte(&itt->second.szName,32);
		gangInfoStream.Wirte(&itt->second.iLevel,sizeof(int32));
		gangInfoStream.Wirte(&itt->second.szApplyTime,50);
		gangInfoStream.Wirte(&itt->second.szPosition,32);
		gangInfoStream.Wirte(&itt->second.iContribution,sizeof(int32));
		gangInfoStream.Wirte(&itt->second.szLeaveTime,50);
	}
	gangInfoStream.Wirte(&tempGang->m_szBackupInfo,sizeof(255));
	itt = tempGang->m_mapApplys.begin();
	for (; itt != tempGang->m_mapApplys.end() ; ++itt)
	{
		gangInfoStream.Wirte(&itt->second.id,sizeof(int64));
		gangInfoStream.Wirte(&itt->second.szName,32);
		gangInfoStream.Wirte(&itt->second.iLevel,sizeof(int32));
		gangInfoStream.Wirte(&itt->second.szApplyTime,50);
		gangInfoStream.Wirte(&itt->second.szPosition,32);
		gangInfoStream.Wirte(&itt->second.iContribution,sizeof(int32));
		gangInfoStream.Wirte(&itt->second.szLeaveTime,50);
	}

	vector<GangLogInfo>::iterator it = tempGang->m_vecGangLog.begin();
	for (; it != tempGang->m_vecGangLog.end(); ++ it)
	{
		gangInfoStream.Wirte(&it->recordTime,30);
		gangInfoStream.Wirte(&it->recordContent,255);
	}

	MsgGangNotify* pAddMsg = (MsgGangNotify*)gangInfoStream.Get();
	pAddMsg->Len = gangInfoStream.GetSize();
	g_GameServer->SendToClient(pAddMsg);

	return true;
}

//����Ӱ�
bool SocailManager::ApplyAddGang(MsgBase* msg)
{
	MsgGang* temp = (MsgGang*)msg;
	Player *pTarget = g_MapManager->GetPlayerByID(temp->iPar[0]);
	Gang *tempGang = g_GangManager->GetGangByID(temp->iPar[1]);
	if (tempGang)
	{
		if (pTarget->m_SocietyID == 0)
		{
			GangMember gangMember;
			gangMember.id = pTarget->m_GUID;
			gangMember.iLevel = pTarget->m_Attribute[ATTRIBUTE_LEVEL].GetValueI();
			sprintf(gangMember.szName,"%s",pTarget->m_Name);
			time_t t = time(0);
			strftime(gangMember.szApplyTime, sizeof(gangMember.szApplyTime), "%Y%m%d%H%M%S",localtime(&t) ); 

			pair<map<int64,GangMember>::iterator,bool> InsertPair;
			InsertPair = tempGang->m_mapApplys.insert(pair<int64,GangMember>(pTarget->m_GUID,gangMember));

			if (InsertPair.second)
			{
				MsgGangNotify tttex(ID_G2C, Notity_S2C_Apply_OK,0,0,0,"");
				tttex.OBJID = temp->OBJID;
				g_GameServer->SendToClient(&tttex);
			}
			else
			{
				MsgGangNotify tttex(ID_G2C, Notity_S2C_Apply_Fail_Repeat,0,0,0,"");
				tttex.OBJID = temp->OBJID;
				g_GameServer->SendToClient(&tttex);
			}
		}
		else
		{
			MsgGangNotify tttex(ID_G2C, Notity_S2C_Apply_Fail_InGang,temp->iPar[0],0,0,"");
			tttex.OBJID = temp->OBJID;
			g_GameServer->SendToClient(&tttex);
		}
	}
	else
	{
		MsgGangNotify tttex(ID_G2C, Notity_S2C_Apply_Fail_NoGang,temp->iPar[0],0,0,"");
		tttex.OBJID = temp->OBJID;
		g_GameServer->SendToClient(&tttex);
	}
	

	return true;
}

//�ظ��Ӱ�����
bool SocailManager::ReplyAddGang(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Player	*pHeader = g_MapManager->GetPlayerByID(temp->OBJID);
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[0]);
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[1]);
	if (temp->iPar[2])
	{
		if (pTarget->m_SocietyID <= 0)
		{
			map<int64,GangMember>::iterator itt = pGang->m_mapApplys.find(pTarget->m_GUID);
			pGang->m_mapMembers.insert(pair<int64,GangMember>(pTarget->m_GUID,itt->second));
			pTarget->m_SocietyID = pGang->m_id;
			//�������߷���Ϣ
			MsgGangNotify tttex(ID_G2C, Notity_S2C_Reply_OK,pGang->m_id,pTarget->m_GUID,0,pGang->m_szName);
			tttex.OBJID = pTarget->m_GUID;
			g_GameServer->SendToClient(&tttex);

			GangLogInfo tempLog;
			sprintf(tempLog.recordContent,"%s���뵽����֮�У�",pTarget->m_Name);
			time_t t = time(0);
			strftime(tempLog.recordTime, sizeof(tempLog.recordTime), "%Y-%m-%d",localtime(&t)); 
			pGang->m_vecGangLog.push_back(tempLog);

			//���͸����ݿ�
			temp->SysID = ID_G2D;
			g_GameServer->SendToDB(temp);
		}
		else
		{
			//���������ʾ
			MsgGangNotify tttex(ID_G2C, Notity_S2C_Reply_OK_INGang,temp->iPar[0],0,0,pGang->m_szName);
			tttex.OBJID = temp->OBJID;
			g_GameServer->SendToClient(&tttex);
		}
	}
	else
	{
		MsgGangNotify tttex(ID_G2C, Notity_S2C_Reply_Refused,temp->iPar[0],0,0,pGang->m_szName);
		tttex.OBJID = pTarget->m_GUID;
		g_GameServer->SendToClient(&tttex);
	}

	//����ͬ�⻹�Ǿܾ������������ɾ��
	map<int64,GangMember>::iterator itt = pGang->m_mapApplys.find(pTarget->m_GUID);
	if(itt != pGang->m_mapApplys.end())
	{
		pGang->m_mapApplys.erase(itt);
	}

	return true;
}

//��ð��������б�
bool SocailManager::GetGangApplyList(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[0]);
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[1]);
	if (pTarget->m_SocietyID == pGang->m_id)
	{
		MsgGangNotify tttex(ID_G2C, Notity_S2C_GETAPPLYLIST,pGang->m_id,0,0,(char *)&pGang->m_mapApplys);
		tttex.OBJID = temp->OBJID;
		g_GameServer->SendToClient(&tttex);
	}  

	return true;
}

//�뿪���
bool SocailManager::LeaveGang(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[0]);
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[1]);

	if (pTarget->m_SocietyID == pGang->m_id)
	{
		if (pTarget->m_GUID != pGang->m_iHeaderid)
		{
			//��ԱҪ�뿪����
			//���Ұ������Ƿ��д���
			std::map<int64, GangMember>::iterator it = pGang->m_mapMembers.find(pTarget->m_GUID);
			if (it != pGang->m_mapMembers.end())
			{
				pGang->m_mapMembers.erase(it);

				//֪ͨ��Ա
				MsgGangNotify ttt(ID_G2C, Notity_S2C_LEAVE_GANG,pTarget->m_GUID,0,0,"");
				ttt.OBJID = pTarget->m_GUID;
				g_GameServer->SendToClient(&ttt);

				temp->SysID = ID_G2D;
				g_GameServer->SendToDB(temp);

				GangLogInfo tempLog;
				sprintf(tempLog.recordContent,"%s�뿪�˰�ᣡ",pTarget->m_Name);
				time_t t = time(0);
				strftime(tempLog.recordTime, sizeof(tempLog.recordTime), "%Y-%m-%d",localtime(&t)); 
				pGang->m_vecGangLog.push_back(tempLog);
			}
		}
		else
		{
			//֪ͨ��Ա
			MsgGangNotify ttt(ID_G2C, Notity_S2C_LEAVE_GANG_HEADER,pTarget->m_GUID,0,0,"");
			ttt.OBJID = pTarget->m_GUID;
			g_GameServer->SendToClient(&ttt);
		}
	}
	return true;
}

//�߳����
bool SocailManager::PutGang(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[0]);
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[1]);
	Player	*pHeader = g_MapManager->GetPlayerByID(temp->iPar[2]);
	
	if (pTarget->m_SocietyID == pGang->m_id)
	{
		if (pHeader->m_GUID == pGang->m_iHeaderid)
		{
			if (pHeader->m_GUID != pTarget->m_GUID)
			{
				//���Ұ������Ƿ��д���
				std::map<int64, GangMember>::iterator it = pGang->m_mapMembers.find(pTarget->m_GUID);
				if (it != pGang->m_mapMembers.end())
				{
					pTarget->m_SocietyID = 0;
					pGang->m_mapMembers.erase(it);
					
					//��ʾ��ɫ���߳�����
					MsgGangNotify ttt(ID_G2C, Notity_S2C_PUT_GANG,pGang->m_id,0,0,"");
					ttt.OBJID = pTarget->m_GUID;
					g_GameServer->SendToClient(&ttt);

					temp->SysID = ID_G2D;
					temp->MsgID = Msg_Leave_Gang;
					g_GameServer->SendToDB(temp);

					GangLogInfo tempLog;
					sprintf(tempLog.recordContent,"%s��%s�߳��˰�ᣡ",pTarget->m_Name,pHeader->m_Name);
					time_t t = time(0);
					strftime(tempLog.recordTime, sizeof(tempLog.recordTime), "%Y-%m-%d",localtime(&t)); 
					pGang->m_vecGangLog.push_back(tempLog);
				}
			}
		}
	}

	return true;
}

//��ð����б�
bool SocailManager::GetGangList(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;

	MsgGangNotify ttt(ID_G2C, Notity_S2C_GET_GANGLIST,g_GangManager->m_mapGang.size(),0,0,"");
	ttt.OBJID = temp->OBJID;
	StackStream<4096> gangInfoStream;
	
	gangInfoStream.Wirte(&ttt,sizeof(MsgGangNotify));
	map<int64,Gang>::iterator it = g_GangManager->m_mapGang.begin();
	for (; it != g_GangManager->m_mapGang.end() ;++it)
	{
		int32 GangNum = it->second.m_mapMembers.size();
		gangInfoStream.Wirte(&it->second.m_id,sizeof(int64));
		gangInfoStream.Wirte(&it->second.m_szName,32);
		gangInfoStream.Wirte(&it->second.m_iLevel,sizeof(int32));
		gangInfoStream.Wirte(&it->second.m_szHeaderName,32);
		gangInfoStream.Wirte(&GangNum,sizeof(int32));
		gangInfoStream.Wirte(&GangNum,sizeof(int32));
	}

	MsgGangNotify* pAddMsg = (MsgGangNotify*)gangInfoStream.Get();
	pAddMsg->Len = gangInfoStream.GetSize();
	g_GameServer->SendToClient(pAddMsg);

	return true;
}

//���ɹ���
bool SocailManager::GangAnnouncement(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[0]);
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[1]);
	if (pTarget->m_SocietyID == pGang->m_id)
	{
		if (pTarget->m_GUID != pGang->m_iHeaderid)
		{
			MsgGangNotify ttt(ID_G2C, Notity_S2C_GET_Announce,pGang->m_id,0,0,(char *)&pGang->m_szAccounce);
			ttt.OBJID = pTarget->m_GUID;
			g_GameServer->SendToClient(&ttt);
		}
	}
	return true;
}

//�༭���ṫ��
bool SocailManager::EditAnnouncement(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[0]);
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[1]);
	
	if (pTarget->m_SocietyID == pGang->m_id)
	{
		if (pTarget->m_GUID == pGang->m_iHeaderid)
		{
			sprintf(pGang->m_szAccounce,"%s",temp->szPar);
			MsgGangNotify ttt(ID_G2C, Notity_S2C_EDIT_Announce_OK,0,0,0,temp->szPar);
			ttt.OBJID = pTarget->m_GUID;
			g_GameServer->SendToClient(&ttt);

			//���͸����ݿ�
			temp->SysID = ID_G2D;
			g_GameServer->SendToDB(temp);

			GangLogInfo tempLog;
			strncpy(tempLog.recordContent,"�����޸��˰��ɹ���", 255);
			time_t t = time(0);
			strftime(tempLog.recordTime, sizeof(tempLog.recordTime), "%Y-%m-%d",localtime(&t)); 
			pGang->m_vecGangLog.push_back(tempLog);
		}
	}
	return true;
}

//���Ͱ����ʼ�
bool SocailManager::SendEmail(MsgBase* msg)
{
	return true;
}


//��Ӱ��ɹ���
bool SocailManager::AddGangContribution(MsgBase* msg)
{

	return true;
}


//�鿴���ɹ���
bool SocailManager::GetGangContribution(MsgBase* msg)
{
	return true;
}


//����
bool SocailManager::GangAppoint(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Player	*pHeader = g_MapManager->GetPlayerByID(temp->iPar[0]);
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[1]);
	Gang	*pGang = g_GangManager->GetGangByID(pHeader->m_GUID);

	//�ж��Ƿ�Ϊ�������ж�Ŀ�����Ƿ�Ϊ����
	if (pGang->m_iHeaderid == pHeader->m_GUID && pHeader->m_SocietyID == pTarget->m_SocietyID)
	{
		/*vector<int64>::iterator result = find( pGang->m_vecViveHeaderid.begin( ), pGang->m_vecViveHeaderid.end( ), pTarget->m_GUID );
		if ( result == pGang->m_vecViveHeaderid.end())
		{
		MsgGangNotify ttt(ID_G2C, Notity_S2C_EDIT_Announce_OK,0,0,NULL);
		ttt.OBJID = pTarget->m_GUID;
		g_GameServer->SendToClient(&ttt);
		}*/
	}

	return true;
}


//ת��
bool SocailManager::GangTransfer(MsgBase* msg)
{
	MsgGang	*temp = (MsgGang*)msg;
	Gang	*pGang = g_GangManager->GetGangByID(temp->iPar[0]);
	Player	*pTarget = g_MapManager->GetPlayerByID(temp->iPar[1]);
	Player	*pHeader = g_MapManager->GetPlayerByID(temp->iPar[2]);

	if (pTarget->m_SocietyID == pGang->m_id)
	{
		if (pHeader->m_GUID == pGang->m_iHeaderid)
		{
			if (pHeader->m_GUID != pTarget->m_GUID)
			{
				pGang->m_iHeaderid = pTarget->m_GUID;
				strncpy(pGang->m_szHeaderName,pTarget->m_Name, MAX_NAME_SIZE);

				//��ʾ��ɫ��Ϊ����
				MsgGangNotify ttt(ID_G2C, Notity_S2C_TransferHeader_OK,pGang->m_id,0,0,pGang->m_szName);
				ttt.OBJID = pTarget->m_GUID;
				g_GameServer->SendToClient(&ttt);

				temp->SysID = ID_G2D;
				g_GameServer->SendToDB(temp);

				GangLogInfo tempLog;
				sprintf(tempLog.recordContent,"%s�ѻ᳤ְλת�ø�%s��",pHeader->m_Name,pTarget->m_Name);
				time_t t = time(0);
				strftime(tempLog.recordTime, sizeof(tempLog.recordTime), "%Y-%m-%d",localtime(&t));
				pGang->m_vecGangLog.push_back(tempLog);

			}
		}
	}
	return true;
}


//��������
bool SocailManager::GangUpGrade(MsgBase* msg)
{
	return true;
}


//��ð��ɵȼ�
bool SocailManager::GetGangLevel(MsgBase* msg)
{
	return true;
}


//�����̵깺��
bool SocailManager::GangShopBuy(MsgBase* msg)
{
	return true;
}


//��������
bool SocailManager::StartCopy(MsgBase* msg)
{
	return true;
}


//��������
bool SocailManager::CopyReward(MsgBase* msg)
{
	return true;
}

Team* SocailManager::GetTeam(uint32 teamId)
{
    if (teamId > 0)
    {
        int teamIdx = teamId - 1;
        if (teamIdx < (int)m_teams.size())
        {
            return m_teams[teamIdx];
        }
    }
    return NULL;
}

Team* SocailManager::AllocTeam()
{
    if (m_freeTeams.empty())
    {
        AllocTeamCapacity();
    }

    if (!m_freeTeams.empty())
    {
        Team* pTeam = m_freeTeams.front();
        int teamIdx = pTeam->GetId() - 1;
        m_teams[teamIdx] = pTeam;
        m_freeTeams.pop();
        return pTeam;
    }
    return NULL;
}

void SocailManager::FreeTeam(Team* pTeam)
{
    if (pTeam)
    {
        pTeam->Clear();
        m_freeTeams.push(pTeam);
        int teamIdx = pTeam->GetId() - 1;
        m_teams[teamIdx] = NULL;
    }
}

void SocailManager::AllocTeamCapacity()
{
    uint32 oldCapcity = m_teamCapacity;
    uint32 newCapcity = m_teamCapacity * 2;
    if (oldCapcity == 0)
    {
        newCapcity = 1;
        m_teams.resize(newCapcity);
    }

    //std::vector<Team*> tempTeam(m_teams.size());
    //std::copy(m_teams.begin(), m_teams.end(), tempTeam.begin());
    m_teams.resize(newCapcity);
    //std::copy(tempTeam.begin(), tempTeam.end(), m_teams.begin());

    Team* pTeam = NULL;
    for (uint32 i = oldCapcity; i < newCapcity; ++i)
    {
        pTeam = new Team(i+1);
        m_freeTeams.push(pTeam);
    }

    m_teamCapacity = newCapcity;
    
}

GangManager *g_GangManager = nullptr;

GangManager::GangManager()
{
	//�������ݿ�����
	m_mapGang.clear();
}

GangManager::~GangManager()
{
}

void GangManager::Init()
{
	//�������ݿ�����
}

Gang* GangManager::GetGangByID(int64 id)
{
	std::map<int64, Gang>::iterator it = m_mapGang.find(id);
	if (it != m_mapGang.end())
	{
		return &it->second;
	}

	return NULL;
}

//����½�����ID
int64 GangManager::GetGangidForGangMap()
{
	if(m_mapGang.size() == 0)
		return 1;

	int64 iValue = 0;
	std::map<int64, Gang>::iterator iter = m_mapGang.begin();
	for( ; iter != m_mapGang.end(); ++iter)
	{
		if(iter->first > iValue)
		{
			iValue = iter->first;
		}
	}

	return iValue + 1;
}