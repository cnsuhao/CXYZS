/********************************************************************
created:	2014/7/15
author:		LL
desc:		����Э��ţ���Э��������͡��� �ͻ��˷��������ô��ļ�
//��Ϸ�ͻ��ˣ�Client������½��������Login�������ط�������Gate������Ϸ��������Game�������ݿ��������DB��
���ط�����ֻ������Ϸ�������ļ�������Ϣ������ֻ����ת����
*********************************************************************/

#ifndef __MSGDEF_H__
#define __MSGDEF_H__


//ϵͳ��Ϣ������չ�����ڱ�ʶ��Ϣ���շ���
enum SysMsgType
{
//c/sͨѶ
	ID_C2L = ID_USER_PACKET_ENUM + 11,		//�ͻ���->��½������
	ID_L2C,									//��½������->�ͻ���
	ID_C2G,									//�ͻ���->��Ϸ������
	ID_G2C,									//��Ϸ������->�ͻ���	
	ID_D2C,									//���ݿ������->��Ϸ������->�ͻ���	

//�ڲ�������ͨѶ
	ID_G2L,									//��Ϸ������->��½������
	ID_L2G,									//��½������->��Ϸ������
	ID_G2D,									//��Ϸ������->���ݿ������
	ID_D2G,									//���ݿ������->��Ϸ������
	ID_GAME2GATE,							//��Ϸ������->���ط�����

	ID_MAX
};

//ͨ����Ϣ����
enum MsgType
{
	Msg_GameServer_Register,		//��Ϸ�����������ط�����ע��	
	Msg_Notify,						//��Ϣ֪ͨ��ӦNotifyTypeö��
//��½
	Msg_Version_Check,				//�汾��֤
	Msg_Regiser_Account,			//�˺�ע��
	Msg_Temp_Login,					//�ο͵�¼
	Msg_Account_Login,				//��¼�˺�
	Msg_ServerList,					//�������б�   
	Msg_SelServer,					//ѡ�������
//��Ϸ ����
	Msg_Random_RoleName,			//��ɫ�������ȡ
	Msg_Player_Enter_GameServer,	//��ҽ�����Ϸ������	
	Msg_Player_Leave_GameServer,	//����뿪��Ϸ������
	Msg_CreateRole,					//������ɫ

	Msg_MAX
};


//֪ͨ��Ϣ����
enum NotifyType
{
	//ע��
	Notify_S2C_Regiser_NameLen,		//ע���˺������Ȳ���
	Notify_S2C_Regiser_PasswordLen,	//ע���˺����볤�Ȳ���
	Notify_S2C_Regiser_NameShield,	//ע���˺�������������
	Notify_S2C_Regiser_NameRepeat,	//ע���˺��Ѵ���
	Notify_S2C_Regiser_OK,			//ע��ɹ�
	//��½
	Notify_S2C_Login_NamePassError,	//�˺Ż��������
	Notify_S2C_Login_OK,			//��½�ɹ�
	Notify_C2S_Login_ServerList,	//��ѯ�������б�
	Notify_S2C_Login_ServerNotOpen,	//������δ����
	Notify_S2C_Login_ReLogin,		//������Ҫ��ͻ������µ�½
	Notify_S2C_Login_HeLogin,		//���˺��ڱ𴦵�½
	//��ɫ
	Notify_S2C_CreateRole,			//������Ҫ��ͻ��˴�����ɫ
	Notify_S2C_CreateRole_NameRepeat,//��ɫ���Ѵ���
	Notify_S2C_CreateRole_OK,		//��ɫ�����ɹ�

	Notify_MAX
};

//��Ʒ�Ĳ�������(����ɾ����)
enum ItemOperateType
{
    ItemOperate_LoginLoad,			//��¼ʱ����
    ItemOperate_BagReset,			//������
    ItemOperate_ItemUsed,			//����Ʒʹ��
    ItemOperate_EquipGetOff,		//����װ��
    ItemOperate_EquipUpLevel,		//װ��ǿ��
    ItemOperate_EquipUpUid,         //װ������
    ItemOperate_EquipStone,         //װ����Ƕ
    ItemOperate_EquipStoneRemove,	//���װ����Ƕ��ʯ
    ItemOperate_ItemMerge,			//��Ʒ(��ʯ)�ϳ�
    ItemOperate_TaskReward,         //������
    ItemOperate_BuyMallItem,		//�����̳��������Ʒ
    ItemOperate_SellBagItem,		//���۱����������Ʒ
    ItemOperate_BuyShopItem,		//npc�̵깺��
    
    ItemOperate_Max
};

#endif	//__MSGDEF_H__
