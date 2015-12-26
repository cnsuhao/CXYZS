//////////////////////////////////////////////////////////////////////////
//    File Name:      UIDefaultServer.h
//    Description:    ѡ�������ҳ��
//    Creator:        zp
//    Version:        2015.04.21
//////////////////////////////////////////////////////////////////////////  


#include "UIBase.h"
#include "RequestServerList.h"


#ifndef __UIDEFAULTSERVER_H__
#define __UIDEFAULTSERVER_H__

class UIDefaultServer : public UIBase
{
public:
	UIDefaultServer();
	~UIDefaultServer();

	virtual bool init();
	CREATE_FUNC(UIDefaultServer);

protected:
	//Ĭ�Ϸ����������з���
	void OnBtEnterGame();
	void OnBtChangeServer();
	//ѡ������������з���
	void OnTxSelectServer();
	//ѡ���ɫ�����з���
	void OnBtSelectRole();
	//���ص�Ĭ��ѡ�����������
	void OnBtSelectServertoDefault();

	void ReplaceUI(cc2d::Node* oldUI, cc2d::Node* newUI);
	//��߷������б��������
	void AddAcountItem(cc2dui::ScrollView* sv, cc2dui::Widget* layout,  std::string itemName, int iPos);
	//�ұ߷������б��������
	void AddServerName(cc2dui::Layout* sv, cc2dui::Widget* layout, tagServer& serverLeft,tagServer& serverRight);
	//�ұ߷�������ɫ�������
	void AddServerRole(cc2dui::Layout* sv, cc2dui::Widget* layout);

	//����Ĭ�ϵ�¼����ķ�������
	void UpdateServerName();
	//����ѡ�����������ķ�������
	void UpdateSelectServerName();

private:
	//Ĭ�Ϸ�����UI��ѡ�������UI
    cc2d::Node* defaultServerUI;
	cc2d::Node* selectServerUI;
	
	//��ߵķ������б�ģ��
	cc2dui::ScrollView* accountSV;
	cc2dui::Layout* accountTemplate;

	//�ұߵķ������б�ģ��
	cc2dui::Layout* selectServer;
	cc2dui::Layout* selectServerTemplate;
	//�ұߵĽ�ɫ�б�ģ��
	cc2dui::Layout* selectRole;
	cc2dui::Layout* selectRoleTemplate;

	//������
	cc2d::Sprite* sp_canvas;
	//ѡ�������������ߵ�CHECKBOX
	map<int, cc2dui::CheckBox*> m_mapcheckboxs; 

	//��ǰѡ��ķ�����
	tagServer m_currServer;
	vecServerList m_vecServerList;

	//����������״̬��ʾ
	char m_szSerStateOld[15];			//�Ϸ�
	char m_szSerStateNew[15];			//�·�
	char m_szSerStateAnd[15];			//�Ϸ�
	char m_szSerStateClose[15];			//�ط�

	char m_szSerRec[15];				//�Ƽ�
	char m_szSer1to10[15];				//1-10��
	char m_szSer11to20[15];				//11-20��
};

#endif	//__UIDEFAULTSERVER_H__