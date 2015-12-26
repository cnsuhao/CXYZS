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


struct GangLogInfo
{
	char			recordTime[30];
	char			recordContent[255];
};

class Gang
{
public:
	Gang();
	~Gang();

	int64			m_id;							//����id
	char			m_szName[MAX_NAME_SIZE];		//��������
	int32			m_iLevel;						//���ɵȼ�
	int32			m_iShopLevel;					//�����̵�ȼ�
	int64			m_iHeaderid;					//�᳤ID
	char			m_szHeaderName[MAX_NAME_SIZE];	//�᳤����
	int32			m_iAct;							//��Ծ��
	char			m_szAccounce[255];				//���ɹ���
	int64			m_iViveHeaderid[2];				//���᳤
	int64			m_iStarFlec[5];					//����
	char			m_szCreateTime[50];				//����ʱ��
	int32			m_iCopyid;						//����ID
	char			m_szStartTime[50];				//����ʱ��
	map<int64,GangMember>	m_mapMembers;			//��Ա�б�
	char			m_szBackupInfo[255];			//��ע��Ϣ
	
	map<int64,GangMember>	m_mapApplys;			//��������б�

	vector<GangLogInfo>		m_vecGangLog;			//������־


private:

};
#endif	//__GANGMANAGER_H__