/********************************************************************
//created:	2015/8/19
//author:		zp
//desc:		���ɹ���
//*********************************************************************/

#ifndef __GANGMANAGER_H__
#define __GANGMANAGER_H__

#include "GameDef.h"
#include <vector>
#include <map>
using namespace std;

class Gang
{
public:
	Gang();
	~Gang();

	int64			m_id;							//����id
	char			m_szName[MAX_NAME_SIZE];		//��������
	int8			m_iLevel;						//���ɵȼ�
	int8			m_iShopLevel;					//�����̵�ȼ�
	int64			m_iHeaderid;					//�᳤ID
	char			m_szHeaderName[MAX_NAME_SIZE];	//�᳤����
	int32			m_iAct;							//��Ծ��
	int8			m_iNum;							//��������
	char			m_szAccounce[255];				//���ɹ���
	vector<int64>	m_vecViveHeaderid;				//���᳤
	char			m_szCreateTime[50];				//����ʱ��
	int32			m_iCopyid;						//����ID
	char			m_szStartTime[50];				//����ʱ��

	vector<int64>	m_vecMembersid;					//��Ա����
	vector<int64>	m_vecApplyid;					//�����������
private:

};



class GangManager
{
public:
	GangManager();
	~GangManager();

	void Init();

	map<int64,Gang>	m_mapGang;

	Gang* GetGangByID(int64 id);
};
extern GangManager* g_GangManager;
#endif	//__GANGMANAGER_H__