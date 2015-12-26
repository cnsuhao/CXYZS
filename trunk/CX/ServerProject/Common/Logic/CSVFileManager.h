/********************************************************************
created:	2014/12/20
author:		LL
desc:		csv�ļ�������������csv�ļ��������ݱ������ڴ档�ͻ��˷��������ô��ļ�
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
// ��cocos2d�ӿ�������
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
	string		strName;		//>Buff��
	int32		type;		    //>����(����,�к�,����)
	int32		funcType;		//>��������
    bool        qushan;         //>��ɢ
    int32       overlappedN;    //>�����ӵ�buff����
	string		strIconPath;	//Buffͼ��,·��
	int32		iTargetEffect;	//Ŀ����Ч1ID

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

//��������۸��
struct BuyNumPrice
{
	int32		iBuyNum;			//�������
	int32		iCopyNumPrice;		//���������۸�
	int32		iMagicWeaponPrice;	//����ף���۸�

	void SetData(CSVParser& paser)
	{
		iBuyNum = paser.GetInt();
		iCopyNumPrice = paser.GetInt();
		iMagicWeaponPrice = paser.GetInt();
	}
};

//����
struct ChestInfo
{
	int32		id;					//����id
	int32		iLevelSection;		//�ȼ���
	string		strChestName;		//��������
	int32		iCurrencyType;		//��������
	int32		iCurrencyNum;		//��������
	int32		iBuyItemsid;		//������ƷID
	int32		iExtractingNum;		//��ȡ����
	int32		iDropid;			//����id
	int32		iSpecialDropid;		//�������id,���ȵ���˵���

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

//����
struct CopyInfo
{
	int32			id;					//����id
	string			strCopyName;		//��������
	int32			iMapid;				//��Ӧ��ͼID
	int32			iCopyType;			//�������ͣ�0.1�˸���1.5�˸���2.���ḱ��3.����id��1��ʼÿ���1
	int32			iConsumeItemsid;	//���ĵ���
	int32			iLevellimit;		//�ȼ�����
	int32			iNumlimit;			//��������
	vector<string>	vecStartTime;	//��ʼʱ�䣬ʱ����XX:XX����ʾ,��һ�����ж��ʱ���ˢ��ʱ��"|"����
	int32			iKeepTime;			//����ʱ�䣬��
	int32			iDifficulty;		//�Ѷȣ�1.��2.��ͨ3.����
	int32			iConditions;		//���������1.��ɱ���й���2.��ɱָ������3.���ָ����Ʒ
	int32			iConditionsValue;	//��������
	int32			iConditionsNum;		//��������
	string			strDesc;			//��������
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

//�����
struct Drop
{
	int32		iDropid;		//����ID
	int32		iDroptype;		//�������ͣ�1.������Ʒ���2.ÿ����Ʒ���һ��
	int32		iGroup;			//���飬������=2,���շ������Ȩ��.
	int32		iItemid;		//����ID
    bool        bIsbind;        //�Ƿ��
	int32		iItemNum;		//��������
	int32		iDropProbability;//������ʣ�����������Ϊ1ʱ,�ô���д��ΪȨ��Ȩ��.����������Ϊ2ʱ���ô���д��Ϊ�������.ʮ���֮��.

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

//װ��ǰ׺
struct EquipmentPrefix
{
	int32		id;					//ǰ׺ID
	int32		iEquipmentLevel;	//װ���ȼ�,ÿ10���ò�ͬ��ǰ׺����.1��10��ȡ10��
	int32		iEquipmentQuality;	//װ��Ʒ��
	string		strPrefixName;		//ǰ׺����
	int32		iDrop;			    //����Ȩ��,ÿ���ȼ���Ϊһ������
	int32		iAddAttribute;		//��������,�����ֵ�
	float		iAddValue;			//������ֵ

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

//װ������
struct EquipmentUpgrade
{
	int32 iEquipmentid;	//װ���Ǽ�
	float fChance;		//��������,����
	int32 iMaterialid;	//����ID
	int32 iMaterialNum;	//��������
	int32 iCurrency;	//����

	void SetData(CSVParser& paser)
	{
		iEquipmentid = paser.GetInt();
		fChance = paser.GetFloat();
		iMaterialid = paser.GetInt();
		iMaterialNum = paser.GetInt();
		iCurrency = paser.GetInt();
	}
};

//���Է�
struct ForceForAttribute
{
	int32		iAttid;				//���,�����ֵ�
	string		strAttName;			//������
	int32		iEffect;			//ս����

	void SetData(CSVParser& paser)
	{
		iAttid = paser.GetInt();
		strAttName = paser.GetStr();
		iEffect = paser.GetInt();
	}
};
#ifndef GAME_SERVER

//ͼ��
struct ImageInfo
{
	int32	id;			//ͼ��id
	string	strDesc;	//����
	bool	bIsList;	//ͼ�����ͣ�0��ͼ 1Plist����֡
	string	strPath;	//ͼ��·��
	int32	iFrameCount;//֡��
	float	fPlayTime;	//����һ�ε�ʱ��s
	int32	iOffsetX;	//Xƫ��
	int32	iOffsetY;	//Yƫ��
	int32	iSoundID;	//��Ч����id��ֻ����Ч����ʱ����
    bool    bLoop;      //�Ƿ�ѭ��

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

//����
struct ItemsInfo
{
	uint32		id;					//>ģ��Id
	string		strName;            //>����
	int32		iPicre;             //>���ۼ۸�
	int32		iNumLimit;			//>�ѵ�����
	int32		iQuality;           //>����Ʒ�ʣ�0=��ɫ1=��ɫ2=��ɫ3=��ɫ4=��ɫ
	int32		iValidTime;			//>��Чʱ��(��λ:��)������״̬:0��ʾ���ô���>1���ʾ���ߴӻ�ȡ����ʧ֮���ʱ��(��λ:��)
	int32		iLevelLimit;		//�ȼ�����
	bool		bIsUse;             //>�ɷ�ʹ��
	bool		bIsBatchUse;        //>�ɷ�����ʹ��
	int32		iCompositeID;		//>�ϳ���ģ��Id
	int32		iBigClass;			//��Ʒ���࣬��Ʒ�ֵ�
	int32		iSamllClass;		//��ƷС�࣬��Ʒ�ֵ�
	float		fParams[3];			//3����������Ʒ�ֵ�
	int32		iBaseAtt[4];		//>��������
	float		iBaseAttParams[4];	//>�������Բ���
	int32		iSuitID;			//>��װId
	int32		iFaceID;			//���ID
	string		strDropIcon;		//����ͼ��
	string		strUseTip;			//ʹ�ú���ʾ
	string		strIcon;			//ͼ��
	string		strDes;				//��Ʒ����
	string		strBackup;			//��ע

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

//ְҵ
struct JobInfo
{
	int32		id;					//ְҵid
	string		strJobName;			//ְҵ��
	int32		iSkillTree[2];		//��ʼ����
	int32		iMapid;				//��ʼ��ͼ
	int32		iMaleFace;			//�����
	int32		iFemaleFace;		//Ů���
	string		strMaleHead;		//��ͷ��
	string		strFemaleHead;		//Ůͷ��
	string		strJobDes;			//ְҵ����

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

//�ȼ���Ϣ����ȼ��仯��������Ϣ
struct LevelInfo
{
	int32	id;					//�ȼ�
	int32	iExpValue;			//��������
	int32	iAtt[11];			//����
	int32	iValue[11];			//��ֵ
	int32	iMagicWeaponExp;	//��������

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

//����
struct MagicWeapon
{
	int32		id;				//����id
	string		strName;		//��������
	int32		iFace;			//������ɫ
	int32		iGrowthMin;		//�����ɳ�ϵ����Сֵ
	int32		iGrowthMax;		//�����ɳ�ϵ�����ֵ
	int32		iAttribute[5];	//����1
	int32		iValue[5];		//����1��ֵ
	int32		iXpSkillID;		//XP����ID
	int32		iMountid;		//��ʼ����ID
	string		strIco;			//ͼ��
	string		strFace;		//���

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

//��ͼ
struct MapInfo
{
	int32			id;						//��ͼid
	string			strName;				//��ͼ��
	int32			iMapType;				//��ͼ����,1.������ͼ2.����boos��ͼ3.������ͼ4.��������ͼ5.ս����ͼ.������Ϊ:1,2,3,4ʱ,ֻ��λ��1.������Ϊ��5ʱ,�ּ׷��ҷ�,�׷�Ϊλ��1,�ҷ�Ϊλ��2λ�ð���:����λ��,�λ��,��ȫ��λ��
	int32			iMapPKModel;			//��ͼPKģʽ,1.��ȫģʽ2.����ģʽ3.����ģʽ
	bool			bClientRefresh;			//�Ƿ�ͻ���ˢ��
	Size			sMapSize;				//��ͼ�ߴ�
	Point			pBornPos[2];			//������
	int32			iResMapid;				//�����ͼid,0Ϊԭ��ͼ����.����ID��ʾ�����ͼID
	vector<Point>	vecResPos[2];		//����λ��1	
	vector<tagRect> vecSafeArea[2];			//��ȫ��
	string			strBackImage;			//����ͼ
	string			strMapPath;				//��ͼ·��
	int32			iMusicId;				//��ͼ��������id
	string			strDesc;				//��ͼ����

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iMapType = paser.GetInt();
		iMapPKModel = paser.GetInt();
		bClientRefresh = paser.GetBool();
		//��ͼ�ߴ�
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
		//������1
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pBornPos[0].x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pBornPos[0].y = (float)atoi(strTest.c_str());
		}
		else{ pBornPos[0].x = 0;	pBornPos[0].y = 0;}
		//������2
		strTemp = paser.GetStr();
		if (strTemp != "NULL" && strTemp != "0")
		{
			strTest = strTemp.substr(0,strTemp.find_first_of('#'));
			pBornPos[1].x = (float)atoi(strTest.c_str());
			strTest = strTemp.substr(strTemp.find_first_of('#') + 1);
			pBornPos[1].y = (float)atoi(strTest.c_str());
		}
		else{ pBornPos[1].x = 0;	pBornPos[1].y = 0;}
		//�����ͼ
		iResMapid = paser.GetInt();

		Point pointTemp;
		//����λ��1
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
		//����λ��2
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
		//��ȫ��1
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
		//��ȫ��2
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

//������Ϣ
struct ConveyInfo
{
	int32		id;				//���͵�ID
	int32		iMapid;			//��ͼID
	Point		pConvey;		//���͵�λ��
	int32		iTargetMapid;	//Ŀ���ͼID
	Point		pTarget;		//Ŀ��λ��

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
	int32	id;			//��ͼ���
	Point	pPos;		//λ��
	int32	iLayer;		//�㼶
	int32	iEffectId;	//Ч��·��

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		pPos.x = (float)paser.GetInt();
		pPos.y = (float)paser.GetInt();
		iLayer = paser.GetInt();
		iEffectId = paser.GetInt();
	}
};

//����
struct MonsterInfo
{
    int32			id;					//ID
    string			strName;			//����
    int32			iType;				//����,1=��ͨ2=��Ӣ3=BOSS
    bool			bIsAttack;			//�ɷ񹥻�
    int32			iAlertArea;			//���䷶Χ
    int32			iPatrolArea;		//Ѳ�߷�Χ
    int32			iPatrolTime;		//Ѳ�߼��
    int32			iTrackingArea;		//׷�ٷ�Χ
    int32			iSkill[3];			//����ID
    int32			iLevel;				//�ȼ�

    int32			iAtt[11];			//���ԣ��������������������
	int32			iValue[11];			//��ֵ
	int32			iAttEx;				//��չ����
	float			fValueEx;			//��չ��ֵ

    int32			iAllocationRules;	//�������, 1.�˺���ߵĻ�õ���2.���һ����õ���
    int32			iExp;				//���ﾭ��
    int32			iSkillTreeExp;		//��������
    int32			iDropID;			//����ID
    int32			iDropMoneyType;		//�����������
    int32			iDropNum;			//��������
	int32			iMaxPatrolNum;		//���Ѳ�߹�������
	int32			iDealRefreshTime;	//������ˢ��ʱ�䣬0.��ˢ�¾���ʱ��Ϊ������ˢ��ʱ��(��λ����Ϊ��λ)
	int32			iMapid;				//��ͼID
	vector<Point>	vecPos;				//λ��
    string			strModleid;			//ģ��id
    string			strHeadIcon;		//ͷ��·��
    int32			iNameHeight;		//���ָ߶�
    int32			iDieSound;			//��������id
    float			fScale;				//����
    float           fAnchorX;           //ê��x
    float           fAnchorY;           //ê��y
    string			strDesc;			//����

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

//������ɫ��
struct NameColor
{
	int32		id;			//id
	string		strName;	//����
	int32		iR;			//R
	int32		iG;			//G
	int32		iB;			//B
	string		strDes;		//��ע

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

//npc��Ϣ
struct NPCInfo
{
	int32		id;				//npcid
	string		strName;		//npc����
	int32		iMapid;			//��ͼID
	Point		pPos;			//λ��
	string		strTalk[2];		//NPCĬ�϶Ի�,2ѡ1��ʾ
	string		strModleid;		//ģ��
	string		strHeadPath;	//ͷ��·��
	string		strTalkHead;	//�Ի�ͷ��
	string		strTaskHead;	//����ͷ��
	int32		iNpcDirecion;	//NPC����,0���� 1����
	int32		iNameHeight;	//���Ƹ߶�
	float		fScale;			//����
    float       fAnchorX;       //Npcê��x
    float       fAnchorY;       //Npcê��y

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

//����
struct Realm
{
	int32		id;				//����ID
	string		strName;		//������
	int32		iRequireFix;	//������Ϊ
	int32		iRequireItems;	//�������
	int32		iRequireNum;	//��������
	int32		iSucChances;	//�ɹ����ʣ���ֱȣ�
	int32		iConsumeAtt;	//��������
	int32		iConsumeNum;	//��������
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

//�ɼ�
struct ResInfo
{
	int32		id;				//id
	string		strName;		//����
	int32		iTime;			//�ɼ�����ʱ��
	int32		iMapid;			//��ͼID
	Point		pPos;			//λ��
	int32		iModelid;		//ģ��ID	
	int32		iEffectid;		//��ЧID
	int32		iItemsid;		//����ID
	int32		iNameHeight;	//���Ƹ߶�

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

//�̵�
struct Shop
{
	int32		id;				//��ǩID
	string		strShopType;	//�̵����ͣ��ֵ�
	string		strName;		//��ǩ��
	int32		iCurrencyType;	//�������ͣ��ֵ�
	bool		bIsVip;			//�Ƿ�����VIP��1�ǣ�0��
	string		strStartTime;	//����ʱ�䣬1900/1/1  0:00:00
    bool        bIsBind;        //���۵���Ʒ�Ƿ��
	int32		iContinueTime;	//����ʱ�䣬��
	string		strBackup;		//��ע

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

//�̵���Ʒ
struct ShopItem
{
	int32		id;					//��ǩid
	int32		iGoodsid;			//��Ʒid
    int32       iItemId;            //��Ʒid
	int32		iPrice;				//�۸�, ����ܼ۸�= iPrice * iDiscount * iNumber.
    int32       iNumber;            //����
	int32		iPersonMaxNum;		//ÿ���޹�������0Ϊ����
	int32		iServerMaxNum;		//ȫ���޹�������0Ϊ����
	int32		iDiscount;			//�ۿ�,�ٷֱ�
	int32		iVipLevelLimit;		//����VIP�ȼ�,�ȼ��ﵽ����ʾ, ���ܼ۸������ɺ��ٴ���.
	int32		iGangLevelLimit;	//���󹫻�ȼ�,�ȼ��ﵽ����ʾ

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

//����
struct SkillInfo
{
	int32		id;				//>����id��ID��3λ�����ܵȼ�
	string		strName;		//>������
	int32		kind;			//>��������(��������,��������) 
	int32       triggerType;    //>��������(��ͨ��������,�ܻ�����,�ܿ���buff����)
    float       triggerData[2]; //>��������
    int32		funcType;		//>��������(�˺�,����,Buff,��ɢ����,��ɢ�к�,�ٻ�,����,����)
    float       funcData[3];    //>���ܲ���
    int32       targetType;     //>Ŀ������
    float       rang;           //>���ܷ�Χ
    float       time;           //>����ʱ��
    float       delay;          //>�����ӳ�
    float       factor;         //>Ӱ��ϵ��
    int32       minValue;       //>��Сֵ
    int32       maxValue;       //>���ֵ
    int32       mpCost;        //>Mp����
    float       CD;             //>cd

    int         showDamageCount;//>�˺���ʾ����,�ͻ�����

    struct AddBuffData
    {
        int buffId;
        int targetType; //>buffĿ������
        float range;    //>buff��Χ
        float time;     //>buffʱ��
        float data[3];  //>buff����
    };
    AddBuffData buffDatas[3]; //>buff����

    int animId;             //>�������
    int attackEffectId;     //>������Ч
    int beHurtEffectId;     //>�ܻ���Ч
    int targetRangeEffectId;    //>Ŀ��Ӱ��ķ�Χ��Ч

    int soundId;        //>������Ч
    float soundDelay;   //>������Ч�ӳ�

    string   iconPath;
    int   upgradeMoneyCost;     //>�������
    int   upgradeItemId;        //>��Ʒid
    int   upgradeItemNumberCost;//>��Ʒ����

    string  triggerDesc;        //>��������
    string  desc;               //>����

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

//����
struct SkillTreeInfo
{
    int32		id;				//������id
    string		strName;		//��������
    int32		iLevelExp;		//��������
    string		strDesc;		//����������

    void SetData(CSVParser& paser)
    {
        id = paser.GetInt();
        strName = paser.GetStr();
        iLevelExp = paser.GetInt();
        strDesc = paser.GetStr();
    }
};

//��������
struct SkillActivate
{
    int32       skllTreeTypeId;     //����������id
    std::string skllTreeIcon;       //������ͼ��
    int32       requestLevel[4];    //����ȼ�
    int32       activateSkillId[4]; //�����id

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


//����
struct SoundInfo
{
	int32	id;			//����id
	string	strName;	//������
	int32	iType;		//�������ͣ�0���� 1��Ч
	string	strPath;	//�ļ�·��
	int32	iPlayCount;	//���Ŵ���

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strName = paser.GetStr();
		iType = paser.GetInt();
		strPath = paser.GetStr();
		iPlayCount = paser.GetInt();
	}
};

//��װ
struct SuitInfo
{
	int32		id;					//��װ����ID
	string		strName;			//��װ����
	int32		iSuitAttribute[4];	//2-8����װ����
	float		fSuitValue[4];		//2-8����װ��ֵ
	int32		iSuitParts[8];		//��װװ��
	int32		iSuitEffceid;		//��װ��ЧID��1��Ұ�⸱�� 2��ս�� 3�������� 4������BOSS

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

//�Ի�
struct TalkInfo
{
	int32	id;			//�Ի�id
	int32	iObjID[8];	//˵����id,Ϊ0ʱ��ʾ���ǣ�����Ϊnpcid
	string	strDesc[8];	//˵������

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

//����
struct TaskInfo
{
	int32		id;					//����id
	string		strName;			//������	
	int32		iType;				//��������,0���������� 1��֧������ 2���ճ����� 3����������
	int32		iLevel;				//����ȼ�
	int32		iRealmID;			//��������
	int32		iReqTaskID;			//ǰ������id
	int32		iTaskMapID;			//��ȡ��ͼid
	int32		iTaskNpcID;			//��ȡnpcid
	int32		iReceiveDropID;		//��ȡ��������ӦDrop.csv��id	
	int32		iReferMapID;		//������ͼ
	int32		iReferNpcID;		//����npc
	int32		iReferDropID;		//������������ӦDrop.csv��id	
	int32		iPerserExp;			//���ﾭ�齱��
	int32		iRealmExp;			//���羭�齱��
	int32		iRewardType;		//��������
	int32		iRewardNum;			//��������
	int32		iTermsType;			//�����������,���������ֵ�
	int32		iTermsParam[2];		//�������� �����ɣ� ������ƷID,���������ֵ�
	int32		iTargetMapid;		//����Ŀ���ͼID
	Point		pTargetPoint;		//����Ŀ���
	int32		iRepeatNum;			//ÿ�տ�������
	int32		iReceivableTalkID;	//�ɽ�����Ի�
	int32		iAcceptTalkID;		//��ȡ����Ի�
	int32		iTaskMapid;			//��ȡȡ�ζԻ����͵�ͼID
	Point		pTaskPos;			//���͵�ͼλ��
	int32		iAchieveTalkID;		//�ɽ�����Ի�
	int32		iReferTalkID;		//��������Ի�
	string		strTargetDesc;		//����Ŀ������
	string		strTaskDesc;		//��������

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

//�ı�
struct TextInfo
{
	int32		id;				//�ı�ID,idֻ���Ӳ���ɾ��
	string		strContent;		//�ı����ݸ���
	string		strDesc;		//�ı�����

	void SetData(CSVParser& paser)
	{
		id = paser.GetInt();
		strContent = paser.GetStr();
		strDesc = paser.GetStr();
	}
};

//���
struct Wings
{
	int32		id;						//��λid
	//int32		iLevel;					//���ȼ�
	int32		iAtt[2];				//���ԣ������ֵ�
	int32		iValue[2];				//��ֵ
	int32		iConsumptionItemsid;	//������ƷID
	int32		iItemsNum;				//��Ʒ����
	int32		iCurrencyid;			//����ID
	int32		iCurrencyNum;			//��������
	string		strName;				//��λ����

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

    void Init();	//��ʼ��
    void Release();	//�ͷ�
    bool SaveUserData();	//�����û�����
    bool LoadAllCSVFile();	//��������csv�ļ�

	const BuffInfo* GetBuffByID(int32 id);						//����buffidȡ��buff��Ϣ
	const BuyNumPrice* GetBuyNumPriceByID(int32 id);				//�����ı�idȡ�ù�������۸����Ϣ
	const ChestInfo* GetChestByID(int32 id);						//�����ı�idȡ�ñ�����Ϣ
	const ChestInfo* GetChestInfo(float level, AttributeType MoneyType, int num);//ȡ�ñ�����Ϣ
	const CopyInfo* GetCopyByID(int32 id);						//���ݸ���idȡ�ø�����Ϣ
	const std::vector<Drop>* GetDropByID(int32 id);							//�����ı�idȡ�õ�����Ϣ
	const EquipmentPrefix* GetEquipmentPrefixByID(int32 id);		//�����ı�idȡ��װ��ǰ׺��Ϣ
    const EquipmentPrefix* GenerateEquipmentPrefix(int32 equipmentLevel, int32 equipmentQuality);
	const EquipmentUpgrade* GetEquipmentUpgradeByID(int32 id);	//�����ı�idȡ��װ��������Ϣ
	const ForceForAttribute* GetAttributeByID(int32 id);			//�����ı�idȡ�����Է���Ϣ
#ifndef GAME_SERVER
	const ImageInfo* GetImageByID(int32 id);						//����ͼ��idȡ��ͼ����Ϣ
#endif //!GAME_SERVER	
    const ItemsInfo* GetItemsByID(uint32 id);						//�����ı�idȡ�õ�����Ϣ
	const JobInfo* GetJobByID(int32 id);							//�����ı�idȡ��ְҵ��Ϣ
	const LevelInfo* GetLevelByID(int32 id);						//�����ı�idȡ�õȼ���Ϣ
	const MagicWeapon* GetMagicWeaponByID(int32 id);				//�����ı�idȡ�÷�����Ϣ
	const MapInfo* GetMapByID(int32 id);							//���ݵ�ͼidȡ�õ�ͼ��Ϣ
	const ConveyInfo* GetConveyByID(int32 id);					//����idȡ�õ�ͼ���͵���Ϣ
	int32 GetMapEffects(int32 mapid, std::vector<const MapEffect*>& effects); //��õ�ͼ������Ч
    const MonsterInfo* GetMonsterByID(uint32 id);					//����idȡ�ù������Ϣ	
	const NameColor* GetNameColorByID(int32 id);					//�����ı�idȡ��������ɫ��Ϣ
    const NPCInfo* GetNPCInfoByID(int32 id);						//�����ı�idȡ�õ�ͼNPCˢ����Ϣ	
	const Realm* GetRealmByID(int32 id);							//�����ı�idȡ�þ�����Ϣ
	const ResInfo* GetResByID(int32 id);							//�����ı�idȡ�òɼ�����Ϣ
	const Shop* GetShopByID(int32 id);							//�����ı�idȡ���̵���Ϣ
	const std::map<int32,ShopItem>* GetShopItemByID(int32 id);	//�����ı�idȡ���̵���Ʒ��Ϣ
	const SkillInfo* GetSkillByID(int32 id);						//���ݼ���idȡ�ü�����Ϣ
    const SkillTreeInfo* GetSkillTreeByID(int32 id);				//���ݼ�����idȡ�ü�������Ϣ
    const std::vector<const SkillInfo*>* GetSkillTreeSkills(int32 skillTreeId); //���ݼ�����idȡ�����м�����Ϣ.
    const SkillActivate* GetSkillActivateByType(int32 skillTreeType);//���ݼ�����idȡ�ü������Ϣ
    const SoundInfo* GetSoundByID(int32 id);						//��������idȡ��������Ϣ
	const SuitInfo* GetSuitByID(int32 id);						//�����ı�idȡ����װ��Ϣ
    const TalkInfo* GetTalkByID(int32 id);						//���ݶԻ�idȡ�öԻ���Ϣ
    const TaskInfo* GetTaskByID(int32 id);						//��������idȡ��������Ϣ
    const TextInfo* GetTextByID(int32 id);						//�����ı�idȡ���ı���Ϣ
	const Wings* GetWingsByID(int32 id);						//�����ı�idȡ�ó����Ϣ
    const std::vector<int32>& GetSkillJueXueOrder();            //>��þ�ѧ���
#ifndef GAME_SERVER  
    const string& GetStrByItemType(ItemType type);
    const string& GetStrByEquipType(EquipType type);
#endif // !GAME_SERVER
    void InitSkillTreeHasSkill();

//////////////////////////////////////////////////////////////////////////
// ��ʽ
    bool            IsPercentAttribute(AttributeType type);             //>�������������ж������Ƿ�Ϊ�ٷֱ�
    int             CalcUpStarEffectBaseAttribute(AttributeType type, int baseAttributeVaule, int upStarLevel);  //>���ݻ����������ͳ���������Ӱ��ֵ.
    const string&   GetAttributeName(AttributeType type);               //>��������������������

public:
    bool m_StateMutex[State_Max][Oper_Max];					//״̬�������Ϣ
    int32 m_StateSet[State_Max][Oper_Max];					//״̬���ñ���Ϣ

	float m_RatioInfo[ATTRIBUTE_RATIO_MAX];					//ϵ������Ϣ
#ifndef GAME_SERVER
    Color3B m_QualityColor[QUALITY_MAX];					//Ʒ�ʶ�Ӧ��ɫֵ
    string m_QualityDesc[QUALITY_MAX];						//Ʒ�ʶ�Ӧ��������
    std::map<string, Color3B> m_ColorNameMap;				//��ɫ����Ӧ��ɫֵ
    std::map<string, string> m_ColorDescMap;				//��ɫ����Ӧ��������
    //LevelInfo m_LevelInfo[41];							//�ȼ���Ϣ�б�	
    int32 m_PartLayer[DIRECTION_MAX][EQUIPTYPE_HORSE+1];	//�������
#endif // !GAME_SERVER

protected:

public:
	std::map<int32,BuffInfo>			m_BuffInfoMap;				//buff����Ϣmap
	std::map<int32,BuyNumPrice>			m_BuyNumPriceMap;			//��������۸��map
	std::map<int32,ChestInfo>			m_ChestInfoMap;				//������Ϣ��map
	std::map<int32,CopyInfo>			m_CopyInfoMap;				//��������Ϣmap	
	std::map<int32,std::vector<Drop>>	m_DropMap;					//�����map
	std::map<int32,EquipmentPrefix>		m_EquipmentPrefixMap;		//װ��ǰ׺map
    std::map<int32, std::vector<std::vector<EquipmentPrefix*> > >   m_EquipPrefixHelperMap;   //װ��ǰ׺���ٲ�ѯ�ṹ.
	std::map<int32,EquipmentUpgrade>	m_EquipmentUpgradeMap;		//װ������map
	std::map<int32,ForceForAttribute>	m_ForceForAttributeMap;		//������Ϣmap
#ifndef GAME_SERVER
	std::map<int32,ImageInfo>			m_ImageInfoMap;				//ͼ�����Ϣmap
#endif //!GAME_SERVER
	std::map<uint32,ItemsInfo>			m_ItemsInfoMap;				//������Ϣmap
	std::map<int32,JobInfo>				m_JobInfoMap;				//ְҵ����Ϣmap
	std::map<int32,LevelInfo>			m_LevelInfoMap;				//�ȼ�����Ϣmap
	std::map<int32,MagicWeapon>			m_MagicWeaponMap;			//��������Ϣmap
	std::map<int32,MapInfo>				m_MapInfoMap;				//��ͼ����Ϣmap	
	std::map<int32,ConveyInfo>			m_ConveyMap;				//��ͼ���͵�map
	std::multimap<int32,MapEffect>		m_MapEffects;				//��ͼ��Ч
    std::map<uint32,MonsterInfo>		m_MonsterInfoMap;			//�������Ϣmap
    std::map<int32,NameColor>			m_NameColorMap;				//������ɫ��map
    std::map<int32,NPCInfo>				m_NPCInfoMap;				//��ͼNpcˢ����Ϣmap
    std::map<int32,Realm>				m_RealmInfoMap;				//������Ϣmap
    std::map<int32,ResInfo>				m_ResInfoMap;				//�ɼ���map
    std::map<int32,Shop>				m_ShopInfoMap;				//�̵���Ϣmap
	std::map<int32,std::map<int32,ShopItem> >		m_ShopItemInfoMap;			//�̵���Ʒ��Ϣmap
	std::map<int32,SkillInfo>			m_SkillInfoMap;				//���ܵ���Ϣmap
	std::map<int32,SkillTreeInfo>		m_SkillTreeInfoMap;			//���ܵ���Ϣmap
    std::map<int32,SkillActivate>       m_SkillActivateMap;         //���ܼ�����Ϣmap
    std::map<int32, std::vector<const SkillInfo*> > m_TreeHasSkills;//�����������м���
    std::map<int32,SoundInfo>			m_SoundInfoMap;				//��������Ϣmap
    std::map<int32,SuitInfo>			m_SuitInfoMap;				//��װ��Ϣmap
    std::map<int32,TalkInfo>			m_TalkInfoMap;				//�Ի���Ϣmap
    std::map<int32,TaskInfo>			m_TaskInfoMap;				//������Ϣmap
    std::map<int32,TextInfo>			m_TextMap;					//�ı�Map,������Ϸ�ڵ������ı���ʾ�����ڹ��ʻ���
	std::map<int32,Wings>				m_WingsInfoMap;				//�����Ϣmap
    std::vector<int32>                  m_SkillXueJueOrder;         //>���ܾ�ѧ���

#ifdef GAME_SERVER
    std::string                         m_TablePath;
#endif //GAME_SERVER
};
extern CSVFileManager* g_CSVFileManager;
#endif	//__CSVFILEMANAGER_H__
