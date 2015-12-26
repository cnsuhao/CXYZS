#include "CSVFileManager.h"

#ifndef GAME_SERVER
    #include "CCDirector.h"
    #include "CCFileUtils.h"
    #include "ClientDef.h"
    #include "BaseFunction.h"
#endif // !GAME_SERVER

#ifndef GAME_SERVER
    #include "ccMacros.h"
#else
    #include <cassert>
    #define CCASSERT(con, msg) assert(con)
#endif // !GAME_SERVER

class CSVParserEx : public CSVParser
{
public:
    bool OpenFile(const char* fileName)
    {
#ifdef GAME_SERVER
        if (!g_CSVFileManager->m_TablePath.empty())
        {
            std::string absPath;
            absPath.append(g_CSVFileManager->m_TablePath);
            absPath.append("/");
            absPath.append(fileName);
            return LoadFile(absPath.c_str());
        }
        else
        {
            return LoadFile(fileName);
        }
#else
        std::string filedata;
        filedata = FileUtils::getInstance()->getStringFromFile(fileName);
        if (filedata.size() >0)
        {
            LoadFileFromMemory(filedata.c_str(), filedata.size());
            return true;
        }
        return false;
#endif // !GAME_SERVER
    }
};


CSVFileManager *g_CSVFileManager = NULL;

CSVFileManager::CSVFileManager()
{  
}

CSVFileManager::~CSVFileManager()
{
	Release();
}

void CSVFileManager::Init()
{
	//cocos2d::FileUtils::getInstance()->addSearchPath("Data/");
	 LoadAllCSVFile();
}

void CSVFileManager::Release()
{	
}

bool CSVFileManager::LoadAllCSVFile()
{
	CSVParserEx  csv_parser;		//csv文件解析器	

	//加载buff表
	if (csv_parser.OpenFile("Data/Buff.csv"))
	{
		BuffInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();	
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_BuffInfoMap.insert(std::map<int32, BuffInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//购买次数价格表
	if (csv_parser.OpenFile("Data/BuyNumPrice.csv"))
	{
		BuyNumPrice TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();	
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_BuyNumPriceMap.insert(std::map<int32, BuyNumPrice>::value_type(TempInfo.iBuyNum, TempInfo));
		}
	}

	//宝箱
	if (csv_parser.OpenFile("Data/Chest.csv"))
	{
		ChestInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_ChestInfoMap.insert(std::map<int32, ChestInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//加载副本表
	if (csv_parser.OpenFile("Data/Copy.csv"))
	{
		CopyInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_CopyInfoMap.insert(std::map<int32, CopyInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//掉落信息
	if (csv_parser.OpenFile("Data/Drop.csv")) 
	{
		Drop TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			std::map<int32,std::vector<Drop>>::iterator it = m_DropMap.find(TempInfo.iDropid);
			if(it != m_DropMap.end())
			{
				it->second.push_back(TempInfo);
			}
			else
			{
				std::vector<Drop> vec;
				vec.push_back(TempInfo);
				m_DropMap.insert(std::map<int32, std::vector<Drop>>::value_type(TempInfo.iDropid, vec));
			}
			
		}
	}

	//装备前缀
	if (csv_parser.OpenFile("Data/EquipmentPrefix.csv"))
	{
		EquipmentPrefix TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_EquipmentPrefixMap.insert(std::map<int32, EquipmentPrefix>::value_type(TempInfo.id, TempInfo));
		}

        std::map<int32,EquipmentPrefix>::iterator it = m_EquipmentPrefixMap.begin();
        for (std::map<int32,EquipmentPrefix>::iterator end = m_EquipmentPrefixMap.end(); 
            it != end; ++it)
        {
            EquipmentPrefix& TempInfo = it->second;
            //>根据需求等级.
            std::map<int32, std::vector<std::vector<EquipmentPrefix*> > >::iterator it2 = m_EquipPrefixHelperMap.find(TempInfo.iEquipmentLevel); 
            if (it2 == m_EquipPrefixHelperMap.end())
            {
                std::vector<std::vector<EquipmentPrefix*> > emptyVector;
                emptyVector.resize(QUALITY_MAX);  //>五个品质等级
                m_EquipPrefixHelperMap.insert(std::pair<int32, std::vector<std::vector<EquipmentPrefix*> > >(TempInfo.iEquipmentLevel, emptyVector));
                it2 = m_EquipPrefixHelperMap.find(TempInfo.iEquipmentLevel);
            }

            if (it2 != m_EquipPrefixHelperMap.end())
            {
                std::vector<std::vector<EquipmentPrefix*> >& levelVector = it2->second;
                assert(TempInfo.iEquipmentQuality < QUALITY_MAX);
                levelVector[TempInfo.iEquipmentQuality].push_back(&TempInfo);
            }
        }

        std::map<int32, std::vector<std::vector<EquipmentPrefix*> > >::iterator h_it = m_EquipPrefixHelperMap.begin();
        for (std::map<int32, std::vector<std::vector<EquipmentPrefix*> > >::iterator end = m_EquipPrefixHelperMap.end(); 
                h_it != end; ++h_it)
        {
            std::vector<std::vector<EquipmentPrefix*> >& qualityVector = h_it->second;
            int level = QUALITY_MAX - 1;
            do
            {
                for (int i = level - 1; i >= 0; --i)
                {
                    qualityVector[level].insert(qualityVector[level].end(), qualityVector[i].begin(), qualityVector[i].end());
                }
            }
            while(--level > 0);
        }
	}

	//装备升级
	if (csv_parser.OpenFile("Data/EquipmentUpgrade.csv"))
	{
		EquipmentUpgrade TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_EquipmentUpgradeMap.insert(std::map<int32, EquipmentUpgrade>::value_type(TempInfo.iEquipmentid, TempInfo));
		}
	}

	//属性分
	if (csv_parser.OpenFile("Data/ForceForAttribute.csv"))
	{
		ForceForAttribute TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_ForceForAttributeMap.insert(std::map<int32, ForceForAttribute>::value_type(TempInfo.iAttid, TempInfo));
		}
	}
#ifndef GAME_SERVER
    //加载图像表
    if (csv_parser.OpenFile("Data/Image.csv"))
    {
        ImageInfo TempInfo;
        for (int i = 0 ; i < 3 ; i++)	//忽略前3行
            csv_parser.GetLine();
        while (csv_parser.GetLine())
        {
            TempInfo.SetData(csv_parser);
            m_ImageInfoMap.insert(std::map<int32, ImageInfo>::value_type(TempInfo.id, TempInfo));
        }
    }
#endif // !GAME_SERVER

	//道具信息
	if (csv_parser.OpenFile("Data/item.csv"))
	{
		ItemsInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_ItemsInfoMap.insert(std::map<int32, ItemsInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//职业
	if (csv_parser.OpenFile("Data/Job.csv"))
	{
		JobInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_JobInfoMap.insert(std::map<int32, JobInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//加载等级表
	if (csv_parser.OpenFile("Data/Level.csv"))
	{
		LevelInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_LevelInfoMap.insert(std::map<int32, LevelInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	////法宝
	//if (csv_parser.OpenFile("Data/MagicWeapon.csv"))
	//{
	//	MagicWeapon TempInfo;
	//	for (int i = 0 ; i < 3 ; i++)	//忽略前3行
	//		csv_parser.GetLine();
	//	while (csv_parser.GetLine())
	//	{
	//		TempInfo.SetData(csv_parser);
	//		m_MagicWeaponMap.insert(std::map<int32, MagicWeapon>::value_type(TempInfo.id, TempInfo));
	//	}
	//}

	//加载地图表
	if (csv_parser.OpenFile("Data/Map.csv"))
	{
		MapInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_MapInfoMap.insert(std::map<int32, MapInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//加载地图传送点表
	if (csv_parser.OpenFile("Data/MapConvey.csv"))
	{
		ConveyInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_ConveyMap.insert(std::map<int32, ConveyInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	// 加载地图特效
	if (csv_parser.OpenFile("Data/MapEffects.csv"))
	{
		MapEffect TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_MapEffects.insert(std::map<int32, MapEffect>::value_type(TempInfo.id, TempInfo));
		}
	}

	//加载怪物
	if(csv_parser.OpenFile("Data/Monster.csv"))
    {
		MonsterInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_MonsterInfoMap.insert(std::map<int32, MonsterInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//名称颜色
	if (csv_parser.OpenFile("Data/NameColor.csv")) 
	{
		NameColor TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_NameColorMap.insert(std::map<int32, NameColor>::value_type(TempInfo.id, TempInfo));
		}
	}

	//地图NPC信息
	if (csv_parser.OpenFile("Data/NPC.csv"))
	{
		NPCInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_NPCInfoMap.insert(std::map<int32, NPCInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

#ifndef GAME_SERVER
	//部件层次
	if (csv_parser.OpenFile("Data/PartLayer.csv")) 
	{
        for (int i = 0 ; i < 3 ; i++)	//忽略前3行
            csv_parser.GetLine();
		for (int32 i = 0, j = 0; i < DIRECTION_MAX; i++)
		{
			if (csv_parser.GetLine())
			{
				csv_parser.NextColumn();	//忽略第一列
                csv_parser.NextColumn();	//忽略第二列		
				for (j = 0; j < EQUIPTYPE_HORSE+1; j++)
				{
					m_PartLayer[i][j] = csv_parser.GetInt();
				}					
			}
		}
	}
#endif // !GAME_SERVER

	//人物状态IN
	if (csv_parser.OpenFile("Data/PlayerState_In.csv")) 
	{
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		for (int32 i = 0, j = 0; i < State_Max; i++)
		{
			if (csv_parser.GetLine())
			{
				csv_parser.GetInt();	//忽略第一列		
				for (j = 0; j < Oper_Max; j++)
				{
					m_StateMutex[i][j] = csv_parser.GetBool();
				}					
			}
		}
	}

	//人物状态SET
	if (csv_parser.OpenFile("Data/PlayerState_Set.csv")) 
	{
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		for (int32 i = 0, j = 0; i < State_Max; i++)
		{
			if (csv_parser.GetLine())
			{
				csv_parser.GetInt();	//忽略第一列		
				for (j = 0; j < Oper_Max; j++)
				{
					m_StateSet[i][j] = csv_parser.GetInt();
				}					
			}
		}
	}

	//加载品质颜色表
#ifndef GAME_SERVER
	if (csv_parser.OpenFile("Data/QualityColor.csv"))
	{		
        for (int i = 0 ; i < 3 ; i++)	//忽略前3行
            csv_parser.GetLine();
		string name;
		Color3B color;
		string desc;
		for (int32 i = 0; i < QUALITY_MAX; i++)
		{
			if (csv_parser.GetLine())
			{
				csv_parser.GetInt();	//忽略第一列			
				name = csv_parser.GetStr();
				color.r = csv_parser.GetInt();
                color.g = csv_parser.GetInt();
                color.b = csv_parser.GetInt();
				desc =  csv_parser.GetStr();
				m_ColorNameMap[name] = color;
				m_ColorDescMap[name] = desc;
			}
			m_QualityColor[i] = color;
			m_QualityDesc[i] = desc;			 
		}
	}
#endif // !GAME_SERVER

	//系数
	if (csv_parser.OpenFile("Data/Ratio.csv")) 
	{
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		
		for (int32 i = 1; i < ATTRIBUTE_RATIO_MAX; i++)
		{
			csv_parser.GetLine();
			csv_parser.GetInt();
			m_RatioInfo[i] = csv_parser.GetFloat();
		}
	}

	//境界信息
	if (csv_parser.OpenFile("Data/Realm.csv"))
	{
		Realm TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_RealmInfoMap.insert(std::map<int32, Realm>::value_type(TempInfo.id, TempInfo));
		}
	}

	//采集信息
	if (csv_parser.OpenFile("Data/Res.csv"))
	{
		ResInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_ResInfoMap.insert(std::map<int32, ResInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//商店信息
	if (csv_parser.OpenFile("Data/Shop.csv"))
	{
		Shop TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_ShopInfoMap.insert(std::map<int32, Shop>::value_type(TempInfo.id, TempInfo));
		}
	}

	//商店物品信息
	if (csv_parser.OpenFile("Data/ShopItem.csv"))
	{
		ShopItem TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);

			std::map<int32,std::map<int32,ShopItem> >::iterator it= m_ShopItemInfoMap.find(TempInfo.id);
			if(it == m_ShopItemInfoMap.end()) 
			{
				//没找到
				std::map<int32,ShopItem> mapTemp;
				mapTemp.insert(std::map<int32, ShopItem>::value_type(TempInfo.iGoodsid, TempInfo));
				m_ShopItemInfoMap.insert(std::map<int32, std::map<int32,ShopItem> >::value_type(TempInfo.id, mapTemp));
			}
			else 
			{
				//找到
				it->second.insert(std::map<int32, ShopItem>::value_type(TempInfo.iGoodsid, TempInfo));
			}
		}
	}

	//加载技能表
	if (csv_parser.OpenFile("Data/Skill.csv"))
	{
		SkillInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_SkillInfoMap.insert(std::map<int32, SkillInfo>::value_type(TempInfo.id, TempInfo));
		}
	}	
	
	//加载技能树表
	if (csv_parser.OpenFile("Data/SkillTree.csv"))
	{
		SkillTreeInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_SkillTreeInfoMap.insert(std::map<int32, SkillTreeInfo>::value_type(TempInfo.id, TempInfo));
		}
	}
    //加载技能激活表
    if (csv_parser.OpenFile("Data/SkillActive.csv"))
    {
        SkillActivate TempInfo;
        for (int i = 0 ; i < 3 ; i++)	//忽略前3行
            csv_parser.GetLine();
        while (csv_parser.GetLine())
        {
            TempInfo.SetData(csv_parser);
            m_SkillActivateMap.insert(std::map<int32, SkillActivate>::value_type(TempInfo.skllTreeTypeId, TempInfo));
        }
    }
	
	//加载声音表
	if (csv_parser.OpenFile("Data/Sound.csv"))
	{
		SoundInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_SoundInfoMap.insert(std::map<int32, SoundInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//套装信息
	if (csv_parser.OpenFile("Data/Suit.csv"))
	{
		SuitInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_SuitInfoMap.insert(std::map<int32, SuitInfo>::value_type(TempInfo.id, TempInfo));
		}
	}
	
	//加载对话表
	if (csv_parser.OpenFile("Data/Talk.csv"))
	{
		TalkInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_TalkInfoMap.insert(std::map<int32, TalkInfo>::value_type(TempInfo.id, TempInfo));
		}
	}
	//加载任务表
	if (csv_parser.OpenFile("Data/Task.csv"))
	{
		TaskInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_TaskInfoMap.insert(std::map<int32, TaskInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//加载文本表
	if (csv_parser.OpenFile("Data/Text.csv"))
	{
		TextInfo TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_TextMap.insert(std::map<int32, TextInfo>::value_type(TempInfo.id, TempInfo));
		}
	}

	//翅膀信息
	if (csv_parser.OpenFile("Data/Wings.csv"))
	{
		Wings TempInfo;
		for (int i = 0 ; i < 3 ; i++)	//忽略前3行
			csv_parser.GetLine();
		while (csv_parser.GetLine())
		{
			TempInfo.SetData(csv_parser);
			m_WingsInfoMap.insert(std::map<int32, Wings>::value_type(TempInfo.id, TempInfo));
		}
	}

    //>绝学
    if (csv_parser.OpenFile("Data/Skillunique.csv"))
    {
        for (int i = 0 ; i < 3 ; i++)	//忽略前3行
            csv_parser.GetLine();

        while (csv_parser.GetLine())
        {
            csv_parser.GetInt(); //id
            int skillType = csv_parser.GetInt(); //type
            m_SkillXueJueOrder.push_back(skillType);
        }
    }

    InitSkillTreeHasSkill();

	return true;
}

const MonsterInfo* CSVFileManager::GetMonsterByID(uint32 id)
{
	if (id > 0)
	{
		std::map<uint32, MonsterInfo>::iterator it = m_MonsterInfoMap.find(id);
		if (it != m_MonsterInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const BuffInfo* CSVFileManager::GetBuffByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, BuffInfo>::iterator it = m_BuffInfoMap.find(id);
		if (it != m_BuffInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const SkillTreeInfo* CSVFileManager::GetSkillTreeByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, SkillTreeInfo>::iterator it = m_SkillTreeInfoMap.find(id);
		if (it != m_SkillTreeInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const std::vector<const SkillInfo*>* CSVFileManager::GetSkillTreeSkills(int32 skillTreeId)
{
    std::map<int32, std::vector<const SkillInfo*> >::const_iterator it = m_TreeHasSkills.find(skillTreeId);
    if (it != m_TreeHasSkills.end())
    {
        return &(it->second);
    }
    return NULL;
}

const SkillActivate* CSVFileManager::GetSkillActivateByType(int32 skillTreeType)
{
    std::map<int32, SkillActivate>::iterator it = m_SkillActivateMap.find(skillTreeType);
    if (it != m_SkillActivateMap.end())
    {			
        return &(it->second);
    }
    return NULL;
}

const SkillInfo* CSVFileManager::GetSkillByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, SkillInfo>::iterator it = m_SkillInfoMap.find(id);
		if (it != m_SkillInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//地图传送点
const ConveyInfo* CSVFileManager::GetConveyByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, ConveyInfo>::iterator it = m_ConveyMap.find(id);
		if (it != m_ConveyMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const MapInfo* CSVFileManager::GetMapByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, MapInfo>::iterator it = m_MapInfoMap.find(id);
		if (it != m_MapInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const CopyInfo* CSVFileManager::GetCopyByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, CopyInfo>::iterator it = m_CopyInfoMap.find(id);
		if (it != m_CopyInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

int32 CSVFileManager::GetMapEffects(int32 mapid, std::vector<const MapEffect*>& effects)
{
    int32 count = m_MapEffects.count(mapid);
    if (count > 0)
    {
        std::multimap<int32, MapEffect>::iterator it = m_MapEffects.find(mapid);
        for (int32 i = 0; i < count; ++i, ++it)
        {
            effects.push_back(&it->second);
        }
    }
    return count;
}

const SoundInfo* CSVFileManager::GetSoundByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, SoundInfo>::iterator it = m_SoundInfoMap.find(id);
		if (it != m_SoundInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

#ifndef GAME_SERVER
const ImageInfo* CSVFileManager::GetImageByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, ImageInfo>::iterator it = m_ImageInfoMap.find(id);
		if (it != m_ImageInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}
#endif  //!GAME_SERVER

const TalkInfo* CSVFileManager::GetTalkByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, TalkInfo>::iterator it = m_TalkInfoMap.find(id);
		if (it != m_TalkInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//任务
const TaskInfo* CSVFileManager::GetTaskByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, TaskInfo>::iterator it = m_TaskInfoMap.find(id);
		if (it != m_TaskInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//文本
const TextInfo* CSVFileManager::GetTextByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, TextInfo>::iterator it = m_TextMap.find(id);
		if (it != m_TextMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//属性分
const ForceForAttribute* CSVFileManager::GetAttributeByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, ForceForAttribute>::iterator it = m_ForceForAttributeMap.find(id);
		if (it != m_ForceForAttributeMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//购买次数价格表
const BuyNumPrice* CSVFileManager::GetBuyNumPriceByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, BuyNumPrice>::iterator it = m_BuyNumPriceMap.find(id);
		if (it != m_BuyNumPriceMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//宝箱
const ChestInfo* CSVFileManager::GetChestByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, ChestInfo>::iterator it = m_ChestInfoMap.find(id);
		if (it != m_ChestInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const ChestInfo* CSVFileManager::GetChestInfo(float level, AttributeType MoneyType, int num)
{
	ChestInfo* info = NULL;
	std::map<int32, ChestInfo>::iterator it = m_ChestInfoMap.begin();
	for (; it != m_ChestInfoMap.end(); ++it)
	{			
		if (it->second.iCurrencyType == MoneyType && it->second.iExtractingNum == num)
		{			
			if (it->second.iLevelSection > level)
			{
				break;
			}
			info = &(it->second);
		}
	}
	return info;
}

//装备前缀
const EquipmentPrefix* CSVFileManager::GetEquipmentPrefixByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, EquipmentPrefix>::iterator it = m_EquipmentPrefixMap.find(id);
		if (it != m_EquipmentPrefixMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const EquipmentPrefix* CSVFileManager::GenerateEquipmentPrefix(int32 equipmentLevel, int32 equipmentQuality)
{
    int level = ((equipmentLevel + 9) / 10) * 10;
    assert(equipmentQuality < QUALITY_MAX);

    std::map<int32, std::vector<std::vector<EquipmentPrefix*> > >::iterator it = m_EquipPrefixHelperMap.find(level);
    if (it != m_EquipPrefixHelperMap.end())
    {
        std::vector<std::vector<EquipmentPrefix*> >& qualityTable = it->second;
        size_t count = qualityTable[equipmentQuality].size();
        int sum = 0;
        for (size_t i = 0; i < count; ++i)
        {
            sum += qualityTable[equipmentQuality][i]->iDrop;
        }
        int rate = BaseFunction::Random(0, sum);
        int current = 0;
        for (size_t i = 0; i < count; ++i)
        {
            current += qualityTable[equipmentQuality][i]->iDrop;
            if (rate <= current)
            {
                return qualityTable[equipmentQuality][i];  
            }
        }
    }

    return NULL;
}

//装备升级
const EquipmentUpgrade* CSVFileManager::GetEquipmentUpgradeByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, EquipmentUpgrade>::iterator it = m_EquipmentUpgradeMap.find(id);
		if (it != m_EquipmentUpgradeMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//职业
const JobInfo* CSVFileManager::GetJobByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, JobInfo>::iterator it = m_JobInfoMap.find(id);
		if (it != m_JobInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//等级
const LevelInfo* CSVFileManager::GetLevelByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, LevelInfo>::iterator it = m_LevelInfoMap.find(id);
		if (it != m_LevelInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//法宝
const MagicWeapon* CSVFileManager::GetMagicWeaponByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, MagicWeapon>::iterator it = m_MagicWeaponMap.find(id);
		if (it != m_MagicWeaponMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}



//地图NPC刷新信息
const NPCInfo* CSVFileManager::GetNPCInfoByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, NPCInfo>::iterator it = m_NPCInfoMap.find(id);
		if (it != m_NPCInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//道具信息
const ItemsInfo* CSVFileManager::GetItemsByID(uint32 id)
{
	if (id > 0)
	{
		std::map<uint32, ItemsInfo>::iterator it = m_ItemsInfoMap.find(id);
		if (it != m_ItemsInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}


//境界信息
const Realm* CSVFileManager::GetRealmByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, Realm>::iterator it = m_RealmInfoMap.find(id);
		if (it != m_RealmInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//商店信息
const Shop* CSVFileManager::GetShopByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, Shop>::iterator it = m_ShopInfoMap.find(id);
		if (it != m_ShopInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}
//根据文本id取得商店物品信息
const std::map<int32,ShopItem>* CSVFileManager::GetShopItemByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32,std::map<int32,ShopItem> >::iterator it = m_ShopItemInfoMap.find(id);
		if (it != m_ShopItemInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//套装信息
const SuitInfo* CSVFileManager::GetSuitByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, SuitInfo>::iterator it = m_SuitInfoMap.find(id);
		if (it != m_SuitInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//翅膀信息
const Wings* CSVFileManager::GetWingsByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, Wings>::iterator it = m_WingsInfoMap.find(id);
		if (it != m_WingsInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

const std::vector<int32>& CSVFileManager::GetSkillJueXueOrder()
{
    return m_SkillXueJueOrder;
}

#ifndef GAME_SERVER  

const string& CSVFileManager::GetStrByItemType(ItemType type)
{
    const TextInfo* textInfo = NULL;
    switch (type)
    {
    case ITEMTYPE_EQUIP:
        textInfo = GetTextByID(41);
        break;
    case ITEMTYPE_EXPEND:
        textInfo = GetTextByID(42);
        break;
    case ITEMTYPE_MATERIAL:
        textInfo = GetTextByID(43);
        break;
    case ITEMTYPE_WINGPIECE:
        textInfo = GetTextByID(44);
        break;
    case ITEMTYPE_TASK:
        textInfo = GetTextByID(45);
        break;
    default:
        break;
    }

    if (textInfo)
    {
        return textInfo->strContent;
    }

    static string unknownStr("unknown");
    return unknownStr;
}

const string& CSVFileManager::GetStrByEquipType(EquipType type)
{
    const TextInfo* textInfo = NULL;
    switch (type)
    {
    case EQUIPTYPE_HEAD:
        textInfo = GetTextByID(46);
        break;
    case EQUIPTYPE_BODY:
        textInfo = GetTextByID(47);
        break;
    case EQUIPTYPE_WEAPON:
        textInfo = GetTextByID(48);
        break;
    case EQUIPTYPE_WING:
        textInfo = GetTextByID(49);
        break;
    case EQUIPTYPE_HORSE:
        textInfo = GetTextByID(50);
        break;
    case EQUIPTYPE_BELT:
        textInfo = GetTextByID(51);
        break;
    case EQUIPTYPE_SHOES:
        textInfo = GetTextByID(52);
        break;
    case EQUIPTYPE_NECKLACE:
        textInfo = GetTextByID(53);
        break;
    case EQUIPTYPE_RING:
        textInfo = GetTextByID(54);
        break;
    case EQUIPTYPE_ORNAMENTS:
        textInfo = GetTextByID(55);
        break;
    case EQUIPTYPE_FASHION:
        textInfo = GetTextByID(56);
        break;
    case EQUIPTYPE_MAGICWEAPON:
        textInfo = GetTextByID(58);
        break;
    case EQUIPTYPE_MAX:
        break;
    default:
        break;
    }

    if (textInfo)
    {
        return textInfo->strContent;
    }

    static string unknownStr("unknown");
    return unknownStr;
}

#endif // !GAME_SERVER

void CSVFileManager::InitSkillTreeHasSkill()
{
    /*
    int32 currentLevel;
    std::map<int32,SkillTreeInfo>::const_iterator tIt;
    std::map<int32,SkillTreeInfo>::const_iterator tEnd = m_SkillTreeInfoMap.end();

    std::map<int32,SkillInfo>::const_iterator sIt;
    std::map<int32,SkillInfo>::const_iterator sEnd = m_SkillInfoMap.end();

    std::map<int32, std::vector<const SkillInfo*> >::iterator hsIt;
    std::vector<const SkillInfo*> emptySkillInfo;

    std::map<int32,SkillActivate>::const_iterator aIt = m_SkillActivateMap.begin();
    for (std::map<int32,SkillActivate>::const_iterator aEnd = m_SkillActivateMap.end(); 
        aIt != aEnd; ++aIt)
    {
        const SkillActivate& activate = aIt->second;
        for (int i = 0; i < 4; ++i)
        {
            int32 skillId =  activate.activateSkillId[i];
            sIt = m_SkillInfoMap.find(skillId);
            if (sIt != sEnd)
                currentLevel = activate.requestLevel[i];

            while (sIt != sEnd)
            {
                const SkillInfo& skill = sIt->second;
                for (int i = currentLevel; i < skill.iRequireLevel; ++i)
                {
                    int skillTreeId = aIt->first * 1000 + i;
                    tIt = m_SkillTreeInfoMap.find(skillTreeId);
                    CCASSERT(tIt != tEnd, "");

                    hsIt = m_TreeHasSkills.find(skillTreeId);
                    if (hsIt == m_TreeHasSkills.end())
                    {
                        emptySkillInfo.push_back(&skill);
                        m_TreeHasSkills.insert(std::pair<int32, std::vector<const SkillInfo*> >(skillTreeId, emptySkillInfo));
                        emptySkillInfo.clear();
                    }
                    else
                    {
                        hsIt->second.push_back(&skill);
                    }
                }
                currentLevel = skill.iRequireLevel;
                ++skillId;
                sIt = m_SkillInfoMap.find(skillId);
            }
        }
    }
    */
}

bool CSVFileManager::IsPercentAttribute(AttributeType type)
{
    switch (type)
    {
    case ATTRIBUTE_MONEY:
    case ATTRIBUTE_HPMAX:
    case ATTRIBUTE_MPMAX:
    case ATTRIBUTE_ATTACK:
    case ATTRIBUTE_DEFENSE:
    case ATTRIBUTE_HIT:
    case ATTRIBUTE_DODGE:
    case ATTRIBUTE_CRIT:
        return false;
    case ATTRIBUTE_HP:
    case ATTRIBUTE_MP:
    case ATTRIBUTE_GOLD:
    case ATTRIBUTE_BINDGOLD:
    case ATTRIBUTE_ARENAMONEY:
    case ATTRIBUTE_BATTLEFIELDMONEY:
        return false;
    case ATTRIBUTE_XPMAX:
    case ATTRIBUTE_XP:
    case ATTRIBUTE_LEVEL:
    case ATTRIBUTE_EXP:
    case ATTRIBUTE_REALMLEVEL:
    case ATTRIBUTE_REALMEXP:
        return false;
    case ATTRIBUTE_MOVESPEED_FACTOR:
    case ATTRIBUTE_HUR_FACTOR:
    case ATTRIBUTE_ATK_FACTOR:
    case ATTRIBUTE_CURE_FACTOR:
    case ATTRIBUTE_HIT_PROB:
    case ATTRIBUTE_CRIT_PROB:
        return true;
    case ATTRIBUTE_MAX:
        return false;
    default:
        return false;
    }
}

int CSVFileManager::CalcUpStarEffectBaseAttribute(AttributeType type, int baseAttributeVaule, int upStarLevel)
{
    if (ATTRIBUTE_HPMAX <= type && type <= ATTRIBUTE_CRIT && upStarLevel > 0)
    {
        static bool checkedTable = false;
        static float factorInit = 0;
        static float factor1 = 0;
        static float factor2 = 0;
        static float modify  = 0;
        if (checkedTable == false)
        {
            checkedTable = true;
        }
        float f_upStartLevel = (float)(upStarLevel);

        return int(baseAttributeVaule * (
                                            m_RatioInfo[ATTRIBUTE_RATIO_UPSTARUPINIT] + (f_upStartLevel - 1.0f) * m_RatioInfo[ATTRIBUTE_RATIO_UPSTARUPADJUST1] + 
                                            (upStarLevel - 1.0f)*(upStarLevel - 2.0f)*m_RatioInfo[ATTRIBUTE_RATIO_UPSTARUPADJUST2]*0.5f
                                         ) + 
                   upStarLevel * m_RatioInfo[ATTRIBUTE_RATIO_UPSTARUPDATE]);
    }
    return 0;
}

const string& CSVFileManager::GetAttributeName(AttributeType type)
{
    const TextInfo* info = GetTextByID(100 + (int)type);
    if (info)
    {
        return info->strContent;
    }

    static string unknownStr("unknown");
    return unknownStr;
}

//掉落信息
const  vector<Drop>* CSVFileManager::GetDropByID(int32 Dropid)
{
	if (Dropid > 0)
	{
		std::map<int32, vector<Drop>>::iterator it = m_DropMap.find(Dropid);
		if (it != m_DropMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//名称颜色
const NameColor* CSVFileManager::GetNameColorByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, NameColor>::iterator it = m_NameColorMap.find(id);
		if (it != m_NameColorMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}

//采集表
const ResInfo* CSVFileManager::GetResByID(int32 id)
{
	if (id > 0)
	{
		std::map<int32, ResInfo>::iterator it = m_ResInfoMap.find(id);
		if (it != m_ResInfoMap.end())
		{			
			return &(it->second);
		}
	}
	return NULL;
}