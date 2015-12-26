/********************************************************************
created:	2015/7/27
author:		LL
desc:		��ͼ����,�����ͼ�Ĵ������ͷţ�������ҡ������ͼ������
*********************************************************************/

#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__
#include "GameDef.h"
#include "Vec2.h"
#include <hash_map>
class Player;
class Monster;
class Map;
struct MsgRole_BaseInfo;

class MapManager
{
public:
	MapManager();
	~MapManager();
	
	void Init();								//��ʼ��
	void OpenMap(int32 mapid);					//�򿪵�ͼ,���ݲ�ͬ��ͼ���͵��ò�ͬ�߼�������ͼ
	uint64 NewCopyMapGuid(int32 mapid);			//����һ��������ͼ��ȫ��id,������ͼֱ����ģ��id
	uint64 NewMonsterGuid();					//����һ������ȫ��id
    void Update(float deltaTime);               //����

	void CreatePlayer(MsgRole_BaseInfo *info);	//����һ�����
    void PlayerEnterMap(Player* player, int32 mapid, const Vec2& position);  //��ҽ����ͼ
    void PlayerLeaveMap(Player* player);                    //�뿪��ͼ
    void PlayerChangeMap(Player* player, int32 mapid, const Vec2& position);	//����л���ͼ


	//void CopyPlayerAttribute(BaseInfoIt baseInfo,Player* player);
	void DelPlayer(uint64 accountid);			//ɾ�����
	Player* GetPlayerByID(uint64 accountid);	//��ȡ���,����idȡ
	Player* GetPlayerByName(char* name);		//��ȡ���,��������ȡ
	Monster* GetMonsterByID(uint64 guid);		//��ȡ���,����idȡ
    inline Map* GetMapByID(uint64 mapId);       //���map

    
    void PlayerOffline(Player* player);         //�������

	stdext::hash_map<uint64, Map*> m_MapList;
	stdext::hash_map<uint64, Player*> m_PlayerList;
	stdext::hash_map<uint64, Monster*> m_MonsterList;
	stdext::hash_map<std::string, Player*> m_PlayerListByName;
};

inline Map* MapManager::GetMapByID(uint64 mapId)
{
    stdext::hash_map<uint64, Map*>::iterator it = m_MapList.find(mapId);
    if (it != m_MapList.end())
    {
        return it->second;
    }
    return NULL;
}

extern MapManager* g_MapManager;
#endif	//__MAPMANAGER_H__
