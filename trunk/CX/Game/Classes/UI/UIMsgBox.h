/********************************************************************
created:	2014/12/5
author:		LL
desc:		��Ϣ��ʾ��UI����MessageBox�Ļ����������˰�ť�ص���ͨ��MsgBoxEventType������ͬ�Ļص���Ӧ
*********************************************************************/
#ifndef __UI_MSGBOX_H__
#define __UI_MSGBOX_H__

#include "UIBase.h"


//��ť��ʽ
enum MsgBoxStyle
{	
	MSGBOX_STYLE_NULL,				//û��ť��������������رգ���Ҫ�ֶ�����Release
	MSGBOX_STYLE_CONFIRM,			//��ȷ����ť,��ȷ����ť��ִ����Ӧ�¼����Զ��ͷ�
	MSGBOX_STYLE_CONFIRMANDCANCEL,	//��ȷ����ȡ����ť,��ȷ����ȡ����ť��ִ����Ӧ�¼����Զ��ͷ�
};

//�¼�����
enum MsgBoxEventType
{
	MSGBOX_EVENT_NORMAL,	//û�лص��¼�
	MSGBOX_EVENT_LINK,		//�������ӷ�����
	MSGBOX_EVENT_EXIT,		//�˳���Ϸȷ��
	MSGBOX_EVENT_DELMAIL,	//ɾ���ʼ�ȷ��
	MSGBOX_EVENT_RETURNLOGIN,	//�������Ͽ����ӣ��践�ص�½����
	MSGBOX_EVENT_CREATEROLEOK,	//������ɫ�ɹ�ȷ�ϣ���ȷ��ֱ�ӽ���Ϸ
	MSGBOX_EVENT_ADDFRIEND,		//�ӵ������Ϊ����ʱ���Ƿ���Ӻ���
	MSGBOX_EVENT_RELIVE,		//ԭ�ظ���
    MSGBOX_EVENT_TEAM_APPLAY,   //��������
};

class UIMsgBox :public UIBase
{
public:
	UIMsgBox(); 
	~UIMsgBox();
	
	//����:�¼����ͣ������ı��������ı�,��ť��ʽ,�¼���Ӧ��ֵ
	static UIMsgBox * create(const char* msg, bool isutf8 = true, MsgBoxEventType eventtype = MSGBOX_EVENT_NORMAL, MsgBoxStyle style = MSGBOX_STYLE_CONFIRM, long long value = -1);	
    void Release();	//�ͷ�UIMsgBox����

	bool init();
private:
	void ConfirmCallback(cc2d::Ref *pSender, cc2dui::TouchEventType type);	//ȷ����ť�ص�
	void CancelCallback(cc2d::Ref *pSender, cc2dui::TouchEventType type);	//ȡ����ť�ص�	
	
	cc2dui::Text *msg_text;			//msg�����ı�
	//Button *confirm_button;	//ȷ����ť
	//Button *cancel_button;	//ȡ����ť

	MsgBoxStyle box_style;		//��ť��ʽ
	MsgBoxEventType event_type;	//�¼�����
	long long m_Value;	//MsgBoxEventType�¼���Ӧ��ֵ��Ĭ��Ϊ-1
};

#endif	//__UI_MSGBOX_H__