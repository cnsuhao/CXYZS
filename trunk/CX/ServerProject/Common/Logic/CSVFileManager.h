/********************************************************************
created:	2014/12/20
author:		LL
desc:		csv文件管理：加载所有csv文件并将数据保存在内存。客户端服务器共用此文件
*********************************************************************/
#ifndef __CSVFILEMANAGER_H__
#define __CSVFILEMANAGER_H__
 
#include "GameDef.h"
#include "CSVParser.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

#ifdef GAME_SERVER
#include "Vec2.h"
// 从cocos2d从拷贝而来
class Size
{
public:
    float width;
    float height;
public:
    operator Vec2() const
    {
        return Vec2(width, height);
    }

public:
    Size():width(0), height(0){}
    Size(float width, float height):width(width),height(height){}
    Size(const Size& other):width(other.width), height(other.height){}
    explicit Size(const Vec2& point):width(point.x), height(point.y){}
    Size& operator= (const Size& other){ setSize(other.width, other.height); return *this; }
    Size& operator= (const Vec2& point){ setSize(point.x, point.y); return *this; }
    Size operator+(const Size& right) const { return Size(this->width + right.width, this->height + right.height); }
    Size operator-(const Size& right) const { return Size(this->width - right.width, this->height - right.height); }
    Size operator*(float a) const { return Size(this->width * a, this->height * a); }
    Size operator/(float a) const { return Size(this->width / a, this->height / a); }
    void setSize(float width, float height) { this->width = width; this->height = height; }
    bool equals(const Size& target) const { return (fabs(this->width  - target.width)  < FLT_EPSILON) && (fabs(this->height - target.height) < FLT_EPSILON); }

    static const Size ZERO;
};

class Rect
{
public:
    Vec2 origin;
    Size  size;

public:
    Rect() { setRect(0.0f, 0.0f, 0.0f, 0.0f); }
    Rect(float x, float y, float width, float height) { setRect(x, y, width, height); }
    Rect(const Rect& other) { setRect(other.origin.x, other.origin.y, other.size.width, other.size.height); }
    Rect& operator= (const Rect& other) { setRect(other.origin.x, other.origin.y, other.size.width, other.size.height); return *this; }
    void setRect(float x, float y, float width, float height) { origin.x = x; origin.y = y; size.width = width; size.height = height; }
    float getMinX() const { return origin.y + size.height; } /// return the leftmost x-value of current rect
    float getMidX() const { return origin.x + size.width / 2.0f; } /// return the midpoint x-value of current rect
    float getMaxX() const { return origin.x + size.width; } /// return the rightmost x-value of current rect
    float getMinY() const { return origin.y; } /// return the bottommost y-value of current rect
    float getMidY() const { return origin.y + size.height / 2.0f; } /// return the midpoint y-value of current rect
    float getMaxY() const { return origin.y + size.height; } /// return the topmost y-value of current rect
    //bool equals(const Rect& rect) const { return (origin.equals(rect.origin) && size.equals(rect.size)); }
    bool containsPoint(const Vec2& point) const { 
        bool bRet = false; 
        if (point.x >= getMinX() && point.x <= getMaxX() && point.y >= getMinY() && point.y <= getMaxY())
            bRet = true;
        
        return bRet;
    }
    bool intersectsRect(const Rect& rect) const{
        return !(     getMaxX() < rect.getMinX() ||
                 rect.getMaxX() <      getMinX() ||
                      getMaxY() < rect.getMinY() ||
                 rect.getMaxY() <      getMinY());
    }
    //Rect unionWithRect(const Rect & rect) const;
    //void merge(const Rect& rect);
    
    static const Rect ZERO;
};

typedef Vec2 Point;

#endif // GAME_SERVER

#ifndef GAME_SERVER
    #include "ccTypes.h"
    #include "ClientDef.h"
    using namespace cocos2d;
#endif // !GAME_SERVER

struct tagRect
{
    Point	locationPos;
    Size	locationSize;
};

//Buff
struct BuffInfo
{
	int32		id;				//>Buffid
	string		strName;		//>Buff名
	int32		type;		    //>类型(有益,有害,控制)
	int32		funcType;		//>功能类型
    bool        qushan;         //>驱散
    int32       overlappedN;    //>最多添加的buff个数
	string		strIconPath;	//Buff图标,路径
	int32		iTargetEffect;	//目标特效1ID

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
        type = paser.GetInt();
        funcType = paser.GetInt();
        qushan = (paser.GetInt() != 0) ? true : false;
        overlappedN = paser.GetInt();
        strIconPath = paser.GetStr();
        iTargetEffect = paser.GetInt();
	}
};

//购买次数价格表
struct BuyNumPrice
{
	int32		iBuyNum;			//购买次数
	int32		iCopyNumPrice;		//副本次数价格
	int32		iMagicWeaponPrice;	//法宝祝福价格

	void SetData(CSVParser& paser)
	{
		iBuyNum = paser.GetInt();
		iCopyNumPrice = paser.GetInt();
		iMagicWeaponPrice = paser.GetInt();
	}
};

//宝箱
struct ChestInfo
{
	int32		id;					//宝箱id
	int32		iLevelSection;		//等级段
	string		strChestName;		//宝箱名称
	int32		iCurrencyType;		//货币种类
	int32		iCurrencyNum;		//货币数量
	int32		iBuyItemsid;		//购买物品ID
	int32		iExtractingNum;		//抽取次数
	int32		iDropid;			//掉落id
	int32		iSpecialDropid;		//特殊掉落id,优先掉落此掉落

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		iLevelSection = paser.GetInt();
		strChestName = paser.GetStr();
		iCurrencyType = paser.GetInt();
		iCurrencyNum = paser.GetInt();
		iBuyItemsid = paser.GetInt();
		iExtractingNum = paser.GetInt();
		iDropid = paser.GetInt();
		iSpecialDropid = paser.GetInt();
	}
};

//副本
struct CopyInfo
{
	int32			id;					//副本id
	string			strCopyName;		//副本名称
	int32			iMapid;				//对应地图ID
	int32			iCopyType;			//副本类型，0.1人副本1.5人副本2.公会副本3.爬塔id从1开始每层加1
	int32			iConsumeItemsid;	//消耗道具
	int32			iLevellimit;		//等级限制
	int32			iNumlimit;			//次数限制
	vector<string>	vecStartTime;	//开始时间，时间以XX:XX来表示,当一天内有多个时间点刷新时以"|"隔开
	int32			iKeepTime;			//持续时间，秒
	int32			iDifficulty;		//难度，1.简单2.普通3.困难
	int32			iConditions;		//完成条件，1.击杀所有怪物2.击杀指定怪物3.获得指定物品
	int32			iConditionsValue;	//条件参数
	int32			iConditionsNum;		//条件数量
	string			strDesc;			//副本描述
	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strCopyName = paser.GetStr();
		iMapid = paser.GetInt();
		iCopyType = paser.GetInt();
		iConsumeItemsid = paser.GetInt();
		iLevellimit = paser.GetInt();
		iNumlimit = paser.GetInt();
		string strTemp,strPart;
		vecStartTime.clear();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			while (strTemp.find_first_of('|') != -1)
			{
				strPart = strTemp.substr(0,strTemp.find_first_of('|'));
				strTemp = strTemp.substr(strTemp.find_first_of('|') + 1);
				vecStartTime.push_back(strPart);
			}
			vecStartTime.push_back(strTemp);
		}
		iKeepTime = paser.GetInt();
		iDifficulty = paser.GetInt();
		iConditions = paser.GetInt();
		iConditionsValue = paser.GetInt();
		iConditionsNum = paser.GetInt();
		strDesc = paser.GetStr();
	}
};

//掉落表
struct Drop
{
	int32		iDropid;		//掉落ID
	int32		iDroptype;		//掉落类型，1.所有物品随机2.每件物品随机一次
	int32		iGroup;			//分组，当类型=2,按照分组计算权重.
	int32		iItemid;		//道具ID
    bool        bIsbind;        //是否绑定
	int32		iItemNum;		//道具数量
	int32		iDropProbability;//掉落概率，当掉落类型为1时,该处填写的为权重权重.当掉落类型为2时，该处填写的为掉落概率.十万分之几.

	void SetData(CSVParser& paser)
	{
		iDropid = paser.GetInt();
		iDroptype = paser.GetInt();
		iGroup = paser.GetInt();
		iItemid = paser.GetInt();
        bIsbind = paser.GetInt() != 0 ? true : false;
		iItemNum = paser.GetInt();
		iDropProbability = paser.GetInt();
	}
};

//装备前缀
struct EquipmentPrefix
{
	int32		id;					//前缀ID
	int32		iEquipmentLevel;	//装备等级,每10级用不同的前缀属性.1到10级取10级
	int32		iEquipmentQuality;	//装备品质
	string		strPrefixName;		//前缀名称
	int32		iDrop;			    //掉落权重,每个等级段为一个分组
	int32		iAddAttribute;		//增加属性,属性字典
	float		iAddValue;			//增加数值

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		iEquipmentLevel = paser.GetInt();
		iEquipmentQuality = paser.GetInt();
		strPrefixName = paser.GetStr();
		iDrop = paser.GetInt();
		iAddAttribute = paser.GetInt();
		iAddValue = paser.GetFloat();
	}
};

//装备升级
struct EquipmentUpgrade
{
	int32 iEquipmentid;	//装备星级
	float fChance;		//升级几率,浮点
	int32 iMaterialid;	//材料ID
	int32 iMaterialNum;	//材料数量
	int32 iCurrency;	//货币

	void SetData(CSVParser& paser)
	{
		iEquipmentid = paser.GetInt();
		fChance = paser.GetFloat();
		iMaterialid = paser.GetInt();
		iMaterialNum = paser.GetInt();
		iCurrency = paser.GetInt();
	}
};

//属性分
struct ForceForAttribute
{
	int32		iAttid;				//序号,属性字典
	string		strAttName;			//属性名
	int32		iEffect;			//战斗力

	void SetData(CSVParser& paser)
	{
		iAttid = paser.GetInt();
		strAttName = paser.GetStr();
		iEffect = paser.GetInt();
	}
};
#ifndef GAME_SERVER

//图像
struct ImageInfo
{
	int32	id;			//图像id
	string	strDesc;	//描述
	bool	bIsList;	//图像类型，0单图 1Plist序列帧
	string	strPath;	//图像路径
	int32	iFrameCount;//帧数
	float	fPlayTime;	//播放一次的时间s
	int32	iOffsetX;	//X偏移
	int32	iOffsetY;	//Y偏移
	int32	iSoundID;	//特效配音id，只在特效创建时播放
    bool    bLoop;      //是否循环

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strDesc = paser.GetStr();
		bIsList = paser.GetBool();
		strPath = paser.GetStr();
		iFrameCount = paser.GetInt();
		fPlayTime = paser.GetFloat();
		iOffsetX = paser.GetInt();
		iOffsetY = paser.GetInt();
		iSoundID = paser.GetInt();
        bLoop = paser.GetInt() != 0;
	}
};

#endif //! GAME_SERVER

//道具
struct ItemsInfo
{
	uint32		id;					//>模板Id
	string		strName;            //>名字
	int32		iPicre;             //>出售价格
	int32		iNumLimit;			//>堆叠数量
	int32		iQuality;           //>道具品质，0=白色1=绿色2=蓝色3=紫色4=橙色
	int32		iValidTime;			//>有效时间(单位:秒)，持续状态:0表示永久存在>1则表示道具从获取到消失之间的时间(单位:秒)
	int32		iLevelLimit;		//等级限制
	bool		bIsUse;             //>可否使用
	bool		bIsBatchUse;        //>可否批量使用
	int32		iCompositeID;		//>合成新模板Id
	int32		iBigClass;			//物品大类，物品字典
	int32		iSamllClass;		//物品小类，物品字典
	float		fParams[3];			//3个参数，物品字典
	int32		iBaseAtt[4];		//>基础属性
	float		iBaseAttParams[4];	//>基础属性参数
	int32		iSuitID;			//>套装Id
	int32		iFaceID;			//外观ID
	string		strDropIcon;		//掉落图标
	string		strUseTip;			//使用后提示
	string		strIcon;			//图标
	string		strDes;				//物品描述
	string		strBackup;			//备注

	void SetData(CSVParser& paser)
	{
		id = (uint32)paser.GetInt();
		strName = paser.GetStr();
		iPicre = paser.GetInt();
		iNumLimit = paser.GetInt();
		iQuality = paser.GetInt();
		iValidTime = paser.GetInt();
		iLevelLimit = paser.GetInt();
		bIsUse = paser.GetBool();
		bIsBatchUse = paser.GetBool();
		iCompositeID = paser.GetInt();
		iBigClass = paser.GetInt();
		iSamllClass = paser.GetInt();
		fParams[0] = paser.GetFloat();
		fParams[1] = paser.GetFloat();
		fParams[2] = paser.GetFloat();
		iBaseAtt[0] = paser.GetInt();
		iBaseAttParams[0] = paser.GetFloat();
		iBaseAtt[1] = paser.GetInt();
		iBaseAttParams[1] = paser.GetFloat();
		iBaseAtt[2] = paser.GetInt();
		iBaseAttParams[2] = paser.GetFloat();
		iBaseAtt[3] = paser.GetInt();
		iBaseAttParams[3] = paser.GetFloat();
		iSuitID = paser.GetInt();
		iFaceID = paser.GetInt();
		strDropIcon = paser.GetStr();
		strUseTip = paser.GetStr();
		strIcon = paser.GetStr();
		strDes = paser.GetStr();
		strBackup = paser.GetStr();
	}
};

//职业
struct JobInfo
{
	int32		id;					//职业id
	string		strJobName;			//职业名
	int32		iSkillTree[2];		//初始功法
	int32		iMapid;				//初始地图
	int32		iMaleFace;			//男外观
	int32		iFemaleFace;		//女外观
	string		strMaleHead;		//男头像
	string		strFemaleHead;		//女头像
	string		strJobDes;			//职业介绍

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strJobName = paser.GetStr();
		iSkillTree[0] = paser.GetInt();
		iSkillTree[1] = paser.GetInt();
		iMapid = paser.GetInt();
		iMaleFace = paser.GetInt();
		iFemaleFace = paser.GetInt();
		strMaleHead = paser.GetStr();
		strFemaleHead = paser.GetStr();
		strJobDes =  paser.GetStr();
	}
};

//等级信息，随等级变化的数据信息
struct LevelInfo
{
	int32	id;					//等级
	int32	iExpValue;			//升级经验
	int32	iAtt[11];			//属性
	int32	iValue[11];			//数值
	int32	iMagicWeaponExp;	//法宝经验

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		iExpValue = paser.GetInt();
		iAtt[0] = paser.GetInt();
		iValue[0] = paser.GetInt();
		iAtt[1] = paser.GetInt();
		iValue[1] = paser.GetInt();
		iAtt[2] = paser.GetInt();
		iValue[2] = paser.GetInt();
		iAtt[3] = paser.GetInt();
		iValue[3] = paser.GetInt();
		iAtt[4] = paser.GetInt();
		iValue[4] = paser.GetInt();
		iAtt[5] = paser.GetInt();
		iValue[5] = paser.GetInt();
		iAtt[6] = paser.GetInt();
		iValue[6] = paser.GetInt();
		iAtt[7] = paser.GetInt();
		iValue[7] = paser.GetInt();
		iAtt[8] = paser.GetInt();
		iValue[8] = paser.GetInt();
		iAtt[9] = paser.GetInt();
		iValue[9] = paser.GetInt();
		iAtt[10] = paser.GetInt();
		iValue[10] = paser.GetInt();
		iMagicWeaponExp = paser.GetInt();
	}
};

//法宝
struct MagicWeapon
{
	int32		id;				//法宝id
	string		strName;		//法宝名称
	int32		iFace;			//法宝成色
	int32		iGrowthMin;		//法宝成长系数最小值
	int32		iGrowthMax;		//法宝成长系数最大值
	int32		iAttribute[5];	//属性1
	int32		iValue[5];		//属性1数值
	int32		iXpSkillID;		//XP技能ID
	int32		iMountid;		//初始坐骑ID
	string		strIco;			//图标
	string		strFace;		//外观

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iFace = paser.GetInt();
		iGrowthMin = paser.GetInt();
		iGrowthMax = paser.GetInt();
		iAttribute[0] = paser.GetInt();
		iValue[0] = paser.GetInt();
		iAttribute[1] = paser.GetInt();
		iValue[1] = paser.GetInt();
		iAttribute[2] = paser.GetInt();
		iValue[2] = paser.GetInt();
		iAttribute[3] = paser.GetInt();
		iValue[3] = paser.GetInt();
		iAttribute[4] = paser.GetInt();
		iValue[4] = paser.GetInt();
		iXpSkillID = paser.GetInt();
		iMountid = paser.GetInt();
		strIco = paser.GetStr();
		strFace = paser.GetStr();
	}
};

//地图
struct MapInfo
{
	int32			id;						//地图id
	string			strName;				//地图名
	int32			iMapType;				//地图类型,1.场景地图2.世界boos地图3.副本地图4.竞技场地图5.战场地图.当类型为:1,2,3,4时,只读位置1.当类型为：5时,分甲方乙方,甲方为位置1,乙方为位置2位置包含:出生位置,活动位置,安全区位置
	int32			iMapPKModel;			//地图PK模式,1.安全模式2.队伍模式3.工会模式
	bool			bClientRefresh;			//是否客户端刷怪
	Size			sMapSize;				//地图尺寸
	Point			pBornPos[2];			//出生地
	int32			iResMapid;				//复活地图id,0为原地图复活.具体ID表示具体地图ID
	vector<Point>	vecResPos[2];		//复活位置1	
	vector<tagRect> vecSafeArea[2];			//安全区
	string			strBackImage;			//背景图
	string			strMapPath;				//地图路径
	int32			iMusicId;				//地图背景音乐id
	string			strDesc;				//地图描述

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iMapType = paser.GetInt();
		iMapPKModel = paser.GetInt();
		bClientRefresh = paser.GetBool();
		//地图尺寸
		string strTemp,strTest,strPart;
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			sMapSize.width = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			sMapSize.height = (float)atoi(strTest.c_str());
		}
		else{ sMapSize.width = 0;	sMapSize.height = 0;}
		//出生地1
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pBornPos[0].x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pBornPos[0].y = (float)atoi(strTest.c_str());
		}
		else{ pBornPos[0].x = 0;	pBornPos[0].y = 0;}
		//出生地2
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pBornPos[1].x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pBornPos[1].y = (float)atoi(strTest.c_str());
		}
		else{ pBornPos[1].x = 0;	pBornPos[1].y = 0;}
		//复活地图
		iResMapid = paser.GetInt();

		Point pointTemp;
		//复活位置1
		vecResPos[0].clear();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			while (strTemp.find_first_of('|') != -1)
			{
				strPart = strTemp.substr(0,strTemp.find_first_of('|'));
				strTemp = strTemp.substr(strTemp.find_first_of('|') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				pointTemp.x = (float)atoi(strTest.c_str());
				strTest = strPart.substr(strPart.find_first_of('#') + 1);
				pointTemp.y = (float)atoi(strTest.c_str());
				vecResPos[0].push_back(pointTemp);
			}
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pointTemp.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pointTemp.y = (float)atoi(strTest.c_str());
			vecResPos[0].push_back(pointTemp);
		}
		//复活位置2
		vecResPos[1].clear();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			while (strTemp.find_first_of('|') != -1)
			{
				strPart = strTemp.substr(0,strTemp.find_first_of('|'));
				strTemp = strTemp.substr(strTemp.find_first_of('|') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				pointTemp.x = (float)atoi(strTest.c_str());
				strTest = strPart.substr(strPart.find_first_of('#') + 1);
				pointTemp.y = (float)atoi(strTest.c_str());
				vecResPos[1].push_back(pointTemp);
			}
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pointTemp.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pointTemp.y = (float)atoi(strTest.c_str());
			vecResPos[1].push_back(pointTemp);
		}
		tagRect tempRect;
		//安全区1
		vecSafeArea[0].clear();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			while (strTemp.find_first_of('|') != -1)
			{
				strPart = strTemp.substr(0,strTemp.find_first_of('|'));
				strTemp = strTemp.substr(strTemp.find_first_of('|') + 1);

				strTest = strPart.substr(0,strPart.find_first_of('#'));
				tempRect.locationPos.x = (float)atoi(strTest.c_str());
				strPart = strPart.substr(strPart.find_first_of('#') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				tempRect.locationPos.y = (float)atoi(strTest.c_str());
				strPart = strPart.substr(strPart.find_first_of('#') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				tempRect.locationSize.width = (float)atoi(strTest.c_str());
				strTest = strPart.substr(strPart.find_first_of('#') + 1);
				tempRect.locationSize.height = (float)atoi(strTest.c_str());
				vecSafeArea[0].push_back(tempRect);
			}
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			tempRect.locationPos.x = (float)atoi(strTest.c_str());
			strTemp = strTemp.substr(strTemp.find_first_of('#') + 1);
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			tempRect.locationPos.y = (float)atoi(strTest.c_str());
			strTemp = strTemp.substr(strTemp.find_first_of('#') + 1);
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			tempRect.locationSize.width = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			tempRect.locationSize.height = (float)atoi(strTest.c_str());
			vecSafeArea[0].push_back(tempRect);
		}
		//安全区2
		vecSafeArea[1].clear();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			while (strTemp.find_first_of('|') != -1)
			{
				strPart = strTemp.substr(0,strTemp.find_first_of('|'));
				strTemp = strTemp.substr(strTemp.find_first_of('|') + 1);

				strTest = strPart.substr(0,strPart.find_first_of('#'));
				tempRect.locationPos.x = (float)atoi(strTest.c_str());
				strPart = strPart.substr(strPart.find_first_of('#') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				tempRect.locationPos.y = (float)atoi(strTest.c_str());
				strPart = strPart.substr(strPart.find_first_of('#') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				tempRect.locationSize.width = (float)atoi(strTest.c_str());
				strTest = strPart.substr(strPart.find_first_of('#') + 1);
				tempRect.locationSize.height = (float)atoi(strTest.c_str());
				vecSafeArea[1].push_back(tempRect);
			}
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			tempRect.locationPos.x = (float)atoi(strTest.c_str());
			strTemp = strTemp.substr(strTemp.find_first_of('#') + 1);
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			tempRect.locationPos.y = (float)atoi(strTest.c_str());
			strTemp = strTemp.substr(strTemp.find_first_of('#') + 1);
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			tempRect.locationSize.width = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			tempRect.locationSize.height = (float)atoi(strTest.c_str());
			vecSafeArea[1].push_back(tempRect);
		}

		strBackImage = paser.GetStr();
		strMapPath = paser.GetStr();
		iMusicId = paser.GetInt();
		strDesc = paser.GetStr();
	}
};

//传送信息
struct ConveyInfo
{
	int32		id;				//传送点ID
	int32		iMapid;			//地图ID
	Point		pConvey;		//传送点位置
	int32		iTargetMapid;	//目标地图ID
	Point		pTarget;		//目标位置

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		iMapid = paser.GetInt();
		string strTemp,strTest;
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pConvey.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pConvey.y = (float)atoi(strTest.c_str());
		}
		else{ pConvey.x = 0;	pConvey.y = 0;}
		iTargetMapid = paser.GetInt();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pTarget.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pTarget.y = (float)atoi(strTest.c_str());
		}
		else{ pTarget.x = 0;	pTarget.y = 0;}
	}
};

struct MapEffect
{
	int32	id;			//地图编号
	Point	pPos;		//位置
	int32	iLayer;		//层级
	int32	iEffectId;	//效果路径

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		pPos.x = (float)paser.GetInt();
		pPos.y = (float)paser.GetInt();
		iLayer = paser.GetInt();
		iEffectId = paser.GetInt();
	}
};

//怪物
struct MonsterInfo
{
    int32			id;					//ID
    string			strName;			//名字
    int32			iType;				//类型,1=普通2=精英3=BOSS
    bool			bIsAttack;			//可否攻击
    int32			iAlertArea;			//警戒范围
    int32			iPatrolArea;		//巡逻范围
    int32			iPatrolTime;		//巡逻间隔
    int32			iTrackingArea;		//追踪范围
    int32			iSkill[3];			//技能ID
    int32			iLevel;				//等级

    int32			iAtt[11];			//属性（与怪物属性数量关联）
	int32			iValue[11];			//数值
	int32			iAttEx;				//拓展属性
	float			fValueEx;			//拓展数值

    int32			iAllocationRules;	//分配规则, 1.伤害最高的获得掉落2.最后一击获得掉落
    int32			iExp;				//人物经验
    int32			iSkillTreeExp;		//法功经验
    int32			iDropID;			//掉落ID
    int32			iDropMoneyType;		//掉落货币类型
    int32			iDropNum;			//掉落数量
	int32			iMaxPatrolNum;		//最大巡逻怪物数量
	int32			iDealRefreshTime;	//死亡后刷新时间，0.不刷新具体时间为死亡后刷新时间(单位以秒为单位)
	int32			iMapid;				//地图ID
	vector<Point>	vecPos;				//位置
    string			strModleid;			//模型id
    string			strHeadIcon;		//头像路径
    int32			iNameHeight;		//名字高度
    int32			iDieSound;			//死亡声音id
    float			fScale;				//缩放
    float           fAnchorX;           //锚点x
    float           fAnchorY;           //锚点y
    string			strDesc;			//描述

	int GetAttByType(int type)
	{
		for (int i = 0; i< 11; i++)
		{
			if (iAtt[i] == type)
			{
				return iValue[i];
			}
		}
		return 0;
	}
    void SetData(CSVParser& paser)
    {
        id = paser.GetInt();
        strName = paser.GetStr();
        iType = paser.GetInt();
        bIsAttack = paser.GetBool();
        iAlertArea = paser.GetInt();
        iPatrolArea = paser.GetInt();
        iPatrolTime = paser.GetInt();
        iTrackingArea = paser.GetInt();
        iSkill[0] = paser.GetInt();
		iSkill[1] = paser.GetInt();
		iSkill[2] = paser.GetInt();
        iLevel = paser.GetInt();

		iAtt[0] = paser.GetInt();
		iValue[0] = paser.GetInt();
		iAtt[1] = paser.GetInt();
		iValue[1] = paser.GetInt();
		iAtt[2] = paser.GetInt();
		iValue[2] = paser.GetInt();
		iAtt[3] = paser.GetInt();
		iValue[3] = paser.GetInt();
		iAtt[4] = paser.GetInt();
		iValue[4] = paser.GetInt();
		iAtt[5] = paser.GetInt();
		iValue[5] = paser.GetInt();
		iAtt[6] = paser.GetInt();
		iValue[6] = paser.GetInt();
		iAtt[7] = paser.GetInt();
		iValue[7] = paser.GetInt();
		iAtt[8] = paser.GetInt();
		iValue[8] = paser.GetInt();
		iAtt[9] = paser.GetInt();
		iValue[9] = paser.GetInt();
		iAtt[10] = paser.GetInt();
		iValue[10] = paser.GetInt();
        
		iAttEx = paser.GetInt();
		fValueEx = paser.GetFloat();

        iAllocationRules = paser.GetInt();
        iExp = paser.GetInt();
        iSkillTreeExp = paser.GetInt();
        iDropID = paser.GetInt();
        iDropMoneyType = paser.GetInt();
        iDropNum = paser.GetInt();
		iMaxPatrolNum = paser.GetInt();
		iDealRefreshTime = paser.GetInt();
		iMapid = paser.GetInt();

		string strTemp,strPart,strTest;
		Point pointTemp;
		vecPos.clear();
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			while (strTemp.find_first_of('|') != -1)
			{
				strPart = strTemp.substr(0,strTemp.find_first_of('|'));
				strTemp = strTemp.substr(strTemp.find_first_of('|') + 1);
				strTest = strPart.substr(0,strPart.find_first_of('#'));
				pointTemp.x = (float)atoi(strTest.c_str());
				strTest = strPart.substr(strPart.find_first_of('#') + 1);
				pointTemp.y = (float)atoi(strTest.c_str());
				vecPos.push_back(pointTemp);
			}
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pointTemp.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pointTemp.y = (float)atoi(strTest.c_str());
			vecPos.push_back(pointTemp);
		}

        strModleid = paser.GetStr();
        fAnchorX = paser.GetFloat();
        fAnchorY = paser.GetFloat();
        strHeadIcon = paser.GetStr();
        iNameHeight = paser.GetInt();
        iDieSound= paser.GetInt();
        fScale = paser.GetFloat();
        strDesc = paser.GetStr();
    }
};

//名称颜色表
struct NameColor
{
	int32		id;			//id
	string		strName;	//名称
	int32		iR;			//R
	int32		iG;			//G
	int32		iB;			//B
	string		strDes;		//备注

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iR = paser.GetInt();
		iG = paser.GetInt();
		iB = paser.GetInt();
		strDes = paser.GetStr();
	}
};

//npc信息
struct NPCInfo
{
	int32		id;				//npcid
	string		strName;		//npc名称
	int32		iMapid;			//地图ID
	Point		pPos;			//位置
	string		strTalk[2];		//NPC默认对话,2选1显示
	string		strModleid;		//模型
	string		strHeadPath;	//头像路径
	string		strTalkHead;	//对话头像
	string		strTaskHead;	//任务头像
	int32		iNpcDirecion;	//NPC面向,0：左 1：右
	int32		iNameHeight;	//名称高度
	float		fScale;			//缩放
    float       fAnchorX;       //Npc锚点x
    float       fAnchorY;       //Npc锚点y

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iMapid = paser.GetInt();
		string strTemp,strTest;
		strTemp = paser.GetStr();
		if (strTemp != "NULL")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pPos.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pPos.y = (float)atoi(strTest.c_str());
		}
		else{ pPos.x = 0;	pPos.y = 0;}
		strTalk[0] = paser.GetStr();
		strTalk[1] = paser.GetStr();
		strModleid = paser.GetStr();
        fAnchorX = paser.GetFloat();
        fAnchorY = paser.GetFloat();
        strHeadPath = paser.GetStr();
		strTalkHead = paser.GetStr();
		strTaskHead = paser.GetStr();
		iNpcDirecion = paser.GetInt();
		iNameHeight = paser.GetInt();
		fScale = paser.GetFloat();
	}
};

//境界
struct Realm
{
	int32		id;				//境界ID
	string		strName;		//境界名
	int32		iRequireFix;	//所需修为
	int32		iRequireItems;	//所需道具
	int32		iRequireNum;	//所需数量
	int32		iSucChances;	//成功概率（万分比）
	int32		iConsumeAtt;	//消耗属性
	int32		iConsumeNum;	//消耗数量
	int32		iAtt[11];
	int32		iValue[11];


	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iRequireFix = paser.GetInt();
		iRequireItems = paser.GetInt();
		iRequireNum = paser.GetInt();
		iSucChances = paser.GetInt();
		iConsumeAtt = paser.GetInt();
		iConsumeNum = paser.GetInt();
		iAtt[0] = paser.GetInt();
		iValue[0] = paser.GetInt();
		iAtt[1] = paser.GetInt();
		iValue[1] = paser.GetInt();
		iAtt[2] = paser.GetInt();
		iValue[2] = paser.GetInt();
		iAtt[3] = paser.GetInt();
		iValue[3] = paser.GetInt();
		iAtt[4] = paser.GetInt();
		iValue[4] = paser.GetInt();
		iAtt[5] = paser.GetInt();
		iValue[5] = paser.GetInt();
		iAtt[6] = paser.GetInt();
		iValue[6] = paser.GetInt();
		iAtt[7] = paser.GetInt();
		iValue[7] = paser.GetInt();
		iAtt[8] = paser.GetInt();
		iValue[8] = paser.GetInt();
		iAtt[9] = paser.GetInt();
		iValue[9] = paser.GetInt();
		iAtt[10] = paser.GetInt();
		iValue[10] = paser.GetInt();
	}
};

//采集
struct ResInfo
{
	int32		id;				//id
	string		strName;		//名称
	int32		iTime;			//采集读条时间
	int32		iMapid;			//地图ID
	Point		pPos;			//位置
	int32		iModelid;		//模型ID	
	int32		iEffectid;		//特效ID
	int32		iItemsid;		//道具ID
	int32		iNameHeight;	//名称高度

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iTime = paser.GetInt();
		iMapid = paser.GetInt();
		string strTemp,strTest;
		strTemp = paser.GetStr();
		if (strTemp != "NULL")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pPos.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pPos.y = (float)atoi(strTest.c_str());
		}
		else{ pPos.x = 0;	pPos.y = 0;}
		iModelid = paser.GetInt();
		iEffectid = paser.GetInt();
		iItemsid = paser.GetInt();
		iNameHeight = paser.GetInt();
	}
};

//商店
struct Shop
{
	int32		id;				//标签ID
	string		strShopType;	//商店类型，字典
	string		strName;		//标签名
	int32		iCurrencyType;	//货币类型，字典
	bool		bIsVip;			//是否享受VIP，1是，0否
	string		strStartTime;	//开启时间，1900/1/1  0:00:00
    bool        bIsBind;        //出售的物品是否绑定
	int32		iContinueTime;	//持续时间，秒
	string		strBackup;		//备注

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strShopType = paser.GetStr();
		strName = paser.GetStr();
		iCurrencyType = paser.GetInt();
		bIsVip = paser.GetBool();
		strStartTime = paser.GetStr();
        bIsBind = paser.GetInt() != 0 ? true : false;
		iContinueTime = paser.GetInt();
		strBackup = paser.GetStr();
	}
};

//商店物品
struct ShopItem
{
	int32		id;					//标签id
	int32		iGoodsid;			//商品id
    int32       iItemId;            //物品id
	int32		iPrice;				//价格, 最后总价格= iPrice * iDiscount * iNumber.
    int32       iNumber;            //数量
	int32		iPersonMaxNum;		//每人限购数量，0为不限
	int32		iServerMaxNum;		//全服限购数量，0为不限
	int32		iDiscount;			//折扣,百分比
	int32		iVipLevelLimit;		//需求VIP等级,等级达到才显示, 在总价格计算完成后再打折.
	int32		iGangLevelLimit;	//需求公会等级,等级达到才显示

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		iGoodsid = paser.GetInt();
        iItemId = paser.GetInt();
		iPrice = paser.GetInt();
        iNumber = paser.GetInt();
		iPersonMaxNum = paser.GetInt();
		iServerMaxNum = paser.GetInt();
		iDiscount = paser.GetInt();
		iVipLevelLimit = paser.GetInt();
		iGangLevelLimit = paser.GetInt();
	}
};

//技能
struct SkillInfo
{
	int32		id;				//>技能id，ID后3位代表技能等级
	string		strName;		//>技能名
	int32		kind;			//>技能类型(主动技能,被动技能) 
	int32       triggerType;    //>触发类型(普通攻击触发,受击触发,受控制buff触发)
    float       triggerData[2]; //>触发参数
    int32		funcType;		//>功能类型(伤害,治疗,Buff,驱散有益,驱散有害,召唤,溅射,反弹)
    float       funcData[3];    //>功能参数
    int32       targetType;     //>目标类型
    float       rang;           //>技能范围
    float       time;           //>技能时间
    float       delay;          //>功能延迟
    float       factor;         //>影响系数
    int32       minValue;       //>最小值
    int32       maxValue;       //>最大值
    int32       mpCost;        //>Mp消耗
    float       CD;             //>cd

    int         showDamageCount;//>伤害显示次数,客户端用

    struct AddBuffData
    {
        int buffId;
        int targetType; //>buff目标类型
        float range;    //>buff范围
        float time;     //>buff时间
        float data[3];  //>buff参数
    };
    AddBuffData buffDatas[3]; //>buff参数

    int animId;             //>动作编号
    int attackEffectId;     //>攻击特效
    int beHurtEffectId;     //>受击特效
    int targetRangeEffectId;    //>目标影响的范围特效

    int soundId;        //>技能音效
    float soundDelay;   //>技能音效延迟

    string   iconPath;
    int   upgradeMoneyCost;     //>金币消耗
    int   upgradeItemId;        //>物品id
    int   upgradeItemNumberCost;//>物品数量

    string  triggerDesc;        //>触发描述
    string  desc;               //>描述

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		kind = paser.GetInt();

		triggerType = paser.GetInt();
		for (int i = 0; i < 2; ++i)
		{
            triggerData[i] = paser.GetFloat();
		}

        CD = paser.GetFloat();
        mpCost = paser.GetInt();

        targetType = paser.GetInt();
        rang = paser.GetFloat();

        showDamageCount = paser.GetInt();

        funcType = paser.GetInt();
        for (int i = 0; i < 3; ++i)
        {
            funcData[i] = paser.GetFloat();
        }

        factor = paser.GetFloat();
        minValue = paser.GetInt();
        maxValue = paser.GetInt();
        
        for (int i = 0; i < 3; ++i)
        {
            buffDatas[i].buffId = paser.GetInt();
            buffDatas[i].targetType = paser.GetInt();
            buffDatas[i].range = paser.GetFloat();
            buffDatas[i].time = paser.GetFloat();
            buffDatas[i].data[0] = paser.GetFloat();
            buffDatas[i].data[1] = paser.GetFloat();
            buffDatas[i].data[2] = paser.GetFloat();
        }

        animId = paser.GetInt();
        attackEffectId = paser.GetInt();
        delay = paser.GetFloat();
        time = paser.GetFloat();
        beHurtEffectId = paser.GetInt();
        targetRangeEffectId = paser.GetInt();
        
        soundId = paser.GetInt();
        soundDelay = paser.GetFloat();

        iconPath = paser.GetStr();
        upgradeMoneyCost = paser.GetInt();
        upgradeItemId = paser.GetInt();
        upgradeItemNumberCost = paser.GetInt();
        triggerDesc = paser.GetStr();
        desc = paser.GetStr();
	}
};

//功法
struct SkillTreeInfo
{
    int32		id;				//技能树id
    string		strName;		//技能树名
    int32		iLevelExp;		//升级经验
    string		strDesc;		//技能树描述

    void SetData(CSVParser& paser)
    {
        id = paser.GetInt();
        strName = paser.GetStr();
        iLevelExp = paser.GetInt();
        strDesc = paser.GetStr();
    }
};

//功法激活
struct SkillActivate
{
    int32       skllTreeTypeId;     //技能树类型id
    std::string skllTreeIcon;       //技能树图标
    int32       requestLevel[4];    //需求等级
    int32       activateSkillId[4]; //激活技能id

    void SetData(CSVParser& paser)
    {
        skllTreeTypeId = paser.GetInt();
        skllTreeIcon = paser.GetStr();
        for (int i = 0; i < 4; ++i)
        {
            requestLevel[i]     = paser.GetInt();
            activateSkillId[i]  = paser.GetInt();
        }
    }
};


//声音
struct SoundInfo
{
	int32	id;			//声音id
	string	strName;	//声音名
	int32	iType;		//声音类型，0音乐 1音效
	string	strPath;	//文件路径
	int32	iPlayCount;	//播放次数

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iType = paser.GetInt();
		strPath = paser.GetStr();
		iPlayCount = paser.GetInt();
	}
};

//套装
struct SuitInfo
{
	int32		id;					//套装属性ID
	string		strName;			//套装名称
	int32		iSuitAttribute[4];	//2-8件套装属性
	float		fSuitValue[4];		//2-8件套装数值
	int32		iSuitParts[8];		//套装装备
	int32		iSuitEffceid;		//套装特效ID，1、野外副本 2、战场 3、竞技场 4、世界BOSS

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iSuitAttribute[0] = paser.GetInt();
		fSuitValue[0] = paser.GetFloat();
		iSuitAttribute[1] = paser.GetInt();
		fSuitValue[1] = paser.GetFloat();
		iSuitAttribute[2] = paser.GetInt();
		fSuitValue[2] = paser.GetFloat();
		iSuitAttribute[3] = paser.GetInt();
		fSuitValue[3] = paser.GetFloat();
		iSuitParts[0] = paser.GetInt();
		iSuitParts[1] = paser.GetInt();
		iSuitParts[2] = paser.GetInt();
		iSuitParts[3] = paser.GetInt();
		iSuitParts[4] = paser.GetInt();
		iSuitParts[5] = paser.GetInt();
		iSuitParts[6] = paser.GetInt();
		iSuitParts[7] = paser.GetInt();
		iSuitEffceid = paser.GetInt();
	}
};

//对话
struct TalkInfo
{
	int32	id;			//对话id
	int32	iObjID[8];	//说话者id,为0时表示主角，否则为npcid
	string	strDesc[8];	//说话内容

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		iObjID[0] = paser.GetInt();
		strDesc[0] = paser.GetStr();
		iObjID[1] = paser.GetInt();
		strDesc[1] = paser.GetStr();
		iObjID[2] = paser.GetInt();
		strDesc[2] = paser.GetStr();
		iObjID[3] = paser.GetInt();
		strDesc[3] = paser.GetStr();
		iObjID[4] = paser.GetInt();
		strDesc[4] = paser.GetStr();
		iObjID[5] = paser.GetInt();
		strDesc[5] = paser.GetStr();
		iObjID[6] = paser.GetInt();
		strDesc[6] = paser.GetStr();
		iObjID[7] = paser.GetInt();
		strDesc[7] = paser.GetStr();
	}
};

//任务
struct TaskInfo
{
	int32		id;					//任务id
	string		strName;			//任务名	
	int32		iType;				//任务类型,0、主线任务 1、支线任务 2、日常任务 3、公会任务
	int32		iLevel;				//需求等级
	int32		iRealmID;			//境界限制
	int32		iReqTaskID;			//前置任务id
	int32		iTaskMapID;			//接取地图id
	int32		iTaskNpcID;			//接取npcid
	int32		iReceiveDropID;		//接取奖励，对应Drop.csv的id	
	int32		iReferMapID;		//交付地图
	int32		iReferNpcID;		//交付npc
	int32		iReferDropID;		//交付奖励，对应Drop.csv的id	
	int32		iPerserExp;			//人物经验奖励
	int32		iRealmExp;			//境界经验奖励
	int32		iRewardType;		//奖励类型
	int32		iRewardNum;			//奖励数量
	int32		iTermsType;			//完成条件类型,任务条件字典
	int32		iTermsParam[2];		//条件参数 副本ＩＤ 交付物品ID,任务条件字典
	int32		iTargetMapid;		//任务目标地图ID
	Point		pTargetPoint;		//任务目标点
	int32		iRepeatNum;			//每日可做次数
	int32		iReceivableTalkID;	//可接任务对话
	int32		iAcceptTalkID;		//接取任务对话
	int32		iTaskMapid;			//接取取任对话后传送地图ID
	Point		pTaskPos;			//传送地图位置
	int32		iAchieveTalkID;		//可交任务对话
	int32		iReferTalkID;		//交付任务对话
	string		strTargetDesc;		//任务目标描述
	string		strTaskDesc;		//任务描述

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iType = paser.GetInt();
		iLevel = paser.GetInt();
		iRealmID = paser.GetInt();
		iReqTaskID = paser.GetInt();	
		iTaskMapID = paser.GetInt();
		iTaskNpcID = paser.GetInt();
		iReceiveDropID = paser.GetInt();
		iReferMapID = paser.GetInt();
		iReferNpcID = paser.GetInt();
		iReferDropID = paser.GetInt();
		iPerserExp = paser.GetInt();
		iRealmExp = paser.GetInt();
		iRewardType = paser.GetInt();
		iRewardNum = paser.GetInt();
		iTermsType = paser.GetInt();
		iTermsParam[0] = paser.GetInt();
		iTermsParam[1] = paser.GetInt();
		iTargetMapid = paser.GetInt();

		string strTemp,strTest;
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pTargetPoint.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pTargetPoint.y = (float)atoi(strTest.c_str());
		}
		else{ pTargetPoint.x = 0;	pTargetPoint.y = 0;}

		iRepeatNum = paser.GetInt();
		iReceivableTalkID = paser.GetInt();
		iAcceptTalkID = paser.GetInt();
		iTaskMapid = paser.GetInt();
		
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pTaskPos.x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pTaskPos.y = (float)atoi(strTest.c_str());
		}
		else{ pTaskPos.x = 0;	pTaskPos.y = 0;}

		iAchieveTalkID = paser.GetInt();
		iReferTalkID = paser.GetInt();
		strTargetDesc = paser.GetStr();
		strTaskDesc = paser.GetStr();
	}
};

//文本
struct TextInfo
{
	int32		id;				//文本ID,id只增加不可删改
	string		strContent;		//文本内容覆盖
	string		strDesc;		//文本描述

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strContent = paser.GetStr();
		strDesc = paser.GetStr();
	}
};

//翅膀
struct Wings
{
	int32		id;						//孔位id
	//int32		iLevel;					//翅膀等级
	int32		iAtt[2];				//属性，属性字典
	int32		iValue[2];				//数值
	int32		iConsumptionItemsid;	//消耗物品ID
	int32		iItemsNum;				//物品数量
	int32		iCurrencyid;			//属性ID
	int32		iCurrencyNum;			//货币数量
	string		strName;				//孔位名称

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		//iLevel = paser.GetInt();
		iAtt[0] = paser.GetInt();
		iAtt[1] = paser.GetInt();
		iValue[0] = paser.GetInt();
		iValue[1] = paser.GetInt();
		iConsumptionItemsid = paser.GetInt();
		iItemsNum = paser.GetInt();
		iCurrencyid = paser.GetInt();
		iCurrencyNum = paser.GetInt();
		strName = paser.GetStr();
	}
};

//////////////////////////////////////////////////////////////////////////
class CSVFileManager
{
public:
    CSVFileManager();
    ~CSVFileManager();

    void Init();	//初始化
    void Release();	//释放
    bool SaveUserData();	//保存用户数据
    bool LoadAllCSVFile();	//加载所有csv文件

	const BuffInfo* GetBuffByID(int32 id);						//根据buffid取得buff信息
	const BuyNumPrice* GetBuyNumPriceByID(int32 id);				//根据文本id取得购买次数价格表信息
	const ChestInfo* GetChestByID(int32 id);						//根据文本id取得宝箱信息
	const ChestInfo* GetChestInfo(float level, AttributeType MoneyType, int num);//取得宝箱信息
	const CopyInfo* GetCopyByID(int32 id);						//根据副本id取得副本信息
	const std::vector<Drop>* GetDropByID(int32 id);							//根据文本id取得掉落信息
	const EquipmentPrefix* GetEquipmentPrefixByID(int32 id);		//根据文本id取得装备前缀信息
    const EquipmentPrefix* GenerateEquipmentPrefix(int32 equipmentLevel, int32 equipmentQuality);
	const EquipmentUpgrade* GetEquipmentUpgradeByID(int32 id);	//根据文本id取得装备升级信息
	const ForceForAttribute* GetAttributeByID(int32 id);			//根据文本id取得属性分信息
#ifndef GAME_SERVER
	const ImageInfo* GetImageByID(int32 id);						//根据图像id取得图像信息
#endif //!GAME_SERVER	
    const ItemsInfo* GetItemsByID(uint32 id);						//根据文本id取得道具信息
	const JobInfo* GetJobByID(int32 id);							//根据文本id取得职业信息
	const LevelInfo* GetLevelByID(int32 id);						//根据文本id取得等级信息
	const MagicWeapon* GetMagicWeaponByID(int32 id);				//根据文本id取得法宝信息
	const MapInfo* GetMapByID(int32 id);							//根据地图id取得地图信息
	const ConveyInfo* GetConveyByID(int32 id);					//根据id取得地图传送点信息
	int32 GetMapEffects(int32 mapid, std::vector<const MapEffect*>& effects); //获得地图所有特效
    const MonsterInfo* GetMonsterByID(uint32 id);					//根据id取得怪物表信息	
	const NameColor* GetNameColorByID(int32 id);					//根据文本id取得名称颜色信息
    const NPCInfo* GetNPCInfoByID(int32 id);						//根据文本id取得地图NPC刷新信息	
	const Realm* GetRealmByID(int32 id);							//根据文本id取得境界信息
	const ResInfo* GetResByID(int32 id);							//根据文本id取得采集表信息
	const Shop* GetShopByID(int32 id);							//根据文本id取得商店信息
	const std::map<int32,ShopItem>* GetShopItemByID(int32 id);	//根据文本id取得商店物品信息
	const SkillInfo* GetSkillByID(int32 id);						//根据技能id取得技能信息
    const SkillTreeInfo* GetSkillTreeByID(int32 id);				//根据技能树id取得技能数信息
    const std::vector<const SkillInfo*>* GetSkillTreeSkills(int32 skillTreeId); //根据技能树id取得所有技能信息.
    const SkillActivate* GetSkillActivateByType(int32 skillTreeType);//根据技能树id取得激活技能信息
    const SoundInfo* GetSoundByID(int32 id);						//根据声音id取得声音信息
	const SuitInfo* GetSuitByID(int32 id);						//根据文本id取得套装信息
    const TalkInfo* GetTalkByID(int32 id);						//根据对话id取得对话信息
    const TaskInfo* GetTaskByID(int32 id);						//根据任务id取得任务信息
    const TextInfo* GetTextByID(int32 id);						//根据文本id取得文本信息
	const Wings* GetWingsByID(int32 id);						//根据文本id取得翅膀信息
    const std::vector<int32>& GetSkillJueXueOrder();            //>获得绝学编号
#ifndef GAME_SERVER  
    const string& GetStrByItemType(ItemType type);
    const string& GetStrByEquipType(EquipType type);
#endif // !GAME_SERVER
    void InitSkillTreeHasSkill();

//////////////////////////////////////////////////////////////////////////
// 公式
    bool            IsPercentAttribute(AttributeType type);             //>根据属性类型判断数据是否为百分比
    int             CalcUpStarEffectBaseAttribute(AttributeType type, int baseAttributeVaule, int upStarLevel);  //>根据基础属性类型出计算升星影响值.
    const string&   GetAttributeName(AttributeType type);               //>根据属性类获得属性名称

public:
    bool m_StateMutex[State_Max][Oper_Max];					//状态互斥表信息
    int32 m_StateSet[State_Max][Oper_Max];					//状态设置表信息

	float m_RatioInfo[ATTRIBUTE_RATIO_MAX];					//系数表信息
#ifndef GAME_SERVER
    Color3B m_QualityColor[QUALITY_MAX];					//品质对应颜色值
    string m_QualityDesc[QUALITY_MAX];						//品质对应描述文字
    std::map<string, Color3B> m_ColorNameMap;				//颜色名对应颜色值
    std::map<string, string> m_ColorDescMap;				//颜色名对应描述文字
    //LevelInfo m_LevelInfo[41];							//等级信息列表	
    int32 m_PartLayer[DIRECTION_MAX][EQUIPTYPE_HORSE+1];	//部件层次
#endif // !GAME_SERVER

protected:

public:
	std::map<int32,BuffInfo>			m_BuffInfoMap;				//buff的信息map
	std::map<int32,BuyNumPrice>			m_BuyNumPriceMap;			//购买次数价格表map
	std::map<int32,ChestInfo>			m_ChestInfoMap;				//宝箱信息表map
	std::map<int32,CopyInfo>			m_CopyInfoMap;				//副本的信息map	
	std::map<int32,std::vector<Drop>>	m_DropMap;					//掉落表map
	std::map<int32,EquipmentPrefix>		m_EquipmentPrefixMap;		//装备前缀map
    std::map<int32, std::vector<std::vector<EquipmentPrefix*> > >   m_EquipPrefixHelperMap;   //装备前缀加速查询结构.
	std::map<int32,EquipmentUpgrade>	m_EquipmentUpgradeMap;		//装备升级map
	std::map<int32,ForceForAttribute>	m_ForceForAttributeMap;		//属性信息map
#ifndef GAME_SERVER
	std::map<int32,ImageInfo>			m_ImageInfoMap;				//图像的信息map
#endif //!GAME_SERVER
	std::map<uint32,ItemsInfo>			m_ItemsInfoMap;				//道具信息map
	std::map<int32,JobInfo>				m_JobInfoMap;				//职业的信息map
	std::map<int32,LevelInfo>			m_LevelInfoMap;				//等级的信息map
	std::map<int32,MagicWeapon>			m_MagicWeaponMap;			//法宝的信息map
	std::map<int32,MapInfo>				m_MapInfoMap;				//地图的信息map	
	std::map<int32,ConveyInfo>			m_ConveyMap;				//地图传送点map
	std::multimap<int32,MapEffect>		m_MapEffects;				//地图特效
    std::map<uint32,MonsterInfo>		m_MonsterInfoMap;			//怪物的信息map
    std::map<int32,NameColor>			m_NameColorMap;				//名称颜色表map
    std::map<int32,NPCInfo>				m_NPCInfoMap;				//地图Npc刷新信息map
    std::map<int32,Realm>				m_RealmInfoMap;				//境界信息map
    std::map<int32,ResInfo>				m_ResInfoMap;				//采集表map
    std::map<int32,Shop>				m_ShopInfoMap;				//商店信息map
	std::map<int32,std::map<int32,ShopItem> >		m_ShopItemInfoMap;			//商店物品信息map
	std::map<int32,SkillInfo>			m_SkillInfoMap;				//技能的信息map
	std::map<int32,SkillTreeInfo>		m_SkillTreeInfoMap;			//技能的信息map
    std::map<int32,SkillActivate>       m_SkillActivateMap;         //技能激活信息map
    std::map<int32, std::vector<const SkillInfo*> > m_TreeHasSkills;//技能树下所有技能
    std::map<int32,SoundInfo>			m_SoundInfoMap;				//声音的信息map
    std::map<int32,SuitInfo>			m_SuitInfoMap;				//套装信息map
    std::map<int32,TalkInfo>			m_TalkInfoMap;				//对话信息map
    std::map<int32,TaskInfo>			m_TaskInfoMap;				//任务信息map
    std::map<int32,TextInfo>			m_TextMap;					//文本Map,用于游戏内的所有文本提示，便于国际化。
	std::map<int32,Wings>				m_WingsInfoMap;				//翅膀信息map
    std::vector<int32>                  m_SkillXueJueOrder;         //>技能绝学编号

#ifdef GAME_SERVER
    std::string                         m_TablePath;
#endif //GAME_SERVER
};
extern CSVFileManager* g_CSVFileManager;
#endif	//__CSVFILEMANAGER_H__
