/********************************************************************
created:	2014/7/15
author:		LL
desc:		����Э��ţ���Э��������͡��� �ͻ��˷��������ô��ļ�
//��Ϸ�ͻ��ˣ�Client������½��������Login�������ط�������Gate������Ϸ��������Game�������ݿ��������DB��
���ط�����ֻ������Ϸ�������ļ�������Ϣ������ֻ����ת����
*********************************************************************/

#ifndef __MSGDEF_H__
#define __MSGDEF_H__

#pragma warning(disable:4996)

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
	ID_GATE2GAME,							//���ط�����->��Ϸ������
	ID_MAX
};

//ͨ����Ϣ����
enum MsgType
{
	Msg_GameServer_Register,		//��Ϸ�����������ط�����ע��	
	Msg_Notify,						//��Ϣ֪ͨ��ӦNotifyTypeö��
    Msg_GetGlobalGuid,              //��ȡȫ��guid
	
//��½
	Msg_Version_Check = 10,			//�汾��֤
	Msg_Regiser_Account,			//�˺�ע��
	Msg_Temp_Login,					//�ο͵�¼
	Msg_Account_Login,				//��¼�˺�
	Msg_ServerList,					//�������б�   
	Msg_SelServer,					//ѡ�������
//��Ϸ ����
	Msg_Random_RoleName = 30,		//��ɫ�������ȡ
	Msg_Player_Enter_GameServer,	//��ҽ�����Ϸ������	
	Msg_Player_Leave_GameServer,	//����뿪��Ϸ������
	Msg_Player_Remove_GameAndDBServer,//��������߳�������
	Msg_CreateRole,					//������ɫ
	Msg_Role_BaseInfo,				//��ɫ������Ϣ
    Msg_Role_DetailInfo,            //��ɫ��ϸ��Ϣ
    Msg_Role_EnterMap,              //��ɫ�����ͼ
	Msg_Role_DetailAttribute,		//��ɫ��Ϣ����
    Msg_OtherRole_BaseInfo,         //��ͼ������ɫ������Ϣ
    Msg_OtherRole_DetailInfo,       //������ɫ��ϸ��Ϣ
    Msg_OtherRole_DetailInfoQuery,  //������ɫ��ϸ��Ϣ��ѯ
    Msg_OtherRole_Leave,            //������ɫ�뿪��Ұ
	Msg_Role_BaseInfo_Update,		//�޸������Ϣ
//�
	Msg_Chest = 60,					//������
//
//��ɫ���
	Msg_Object_Move = 80,		    //����ƶ�
    Msg_Object_Move_Stop,           //���ֹͣ�ƶ�
    Msg_Object_Move_kick,           //����ƶ��߻�
	Msg_Player_state,				//������ͬ�����״̬(��)
	Msg_Player_UpdateLevel,			//������ͬ���������

    Msg_Object_SyncAttribute,		//>���������
    Msg_Object_AddHp,               //>�����Ѫ
    Msg_Object_DropHp,              //>�����Ѫ
    Msg_Object_BuffList,            //>����buff����
    Msg_Object_AddBuff,				//>�������buffЧ��
    Msg_Object_DelBuff,				//>��ҽ���buffЧ��

//ս�����
	Msg_Player_UseSkill = 100,		//��һ����ʹ�ü���
	Msg_Player_ReliveData,			//������֪ͨ�ͻ���ԭ�ظ�������Ҫ����Ϣ���������ʱ���������ԭ�ظ����Э�飬����ֱ�ӷ�Msg_Player_Relive
	Msg_Player_Relive,				//��Ҹ���
	Msg_Player_BuffHurt,			//����buff�����ĵ�Ѫ

//�������
	Msg_Monster_Baseinfo = 140,		//�㲥������Ϣ�������������Ұʱ�㲥�����������һ��
	Msg_Monster_Move,				//�㲥�����ƶ�
	Msg_Monster_Del,				//�㲥�Ƴ�������ڹ����������Ƴ���ʱЧ�ԵĹ����
	Msg_Monster_Goto,				//�㲥���͹�����ڳ���׷�ٷ�Χ�Ĺ��

//��Ʒ���
    Msg_Item_BagList = 200,         //�����嵥
    Msg_Item_Add_Set,               //�����Ʒ����
    Msg_Item_Del_Set,               //ɾ����Ʒ����
    Msg_Item_Updata_Set,            //������Ʒ����
    Msg_Item_ShopBuy,               //�̵깺��
    Msg_Item_ShopSell,              //�̵�����
    Msg_Item_Compose,               //��Ʒ�ϳ�
    Msg_Item_UpStar,                //װ������
    Msg_Item_Inherit,               //װ������
    Msg_Item_Use,                   //��Ʒʹ��
    Msg_Equip_List,                 //װ�������嵥
    Msg_Equip_Update,               //װ����������
    Msg_Equip_ShowUpdate,           //װ���仯��ʾ����

//�������
    Msg_Skill_List = 300,       //�������嵥
    Msg_Skill_Add,              //��Ӽ�����
    Msg_SKill_Update,           //���¼�����
    Msg_Skill_Activate,         //������������
    Msg_Skill_ChoseSkill,       //ѡ��uiʹ�ü���

//�������
	Msg_Task_Take = 340,			//��������
	Msg_Task_Make,
	Msg_Task_Submit,    			//�������
	Msg_Task_GetList,				//��������б�
	Msg_Task_Notify,				//�����֪ͨ��Ϣ
//���ѹ���
	Msg_Add_Friends = 400,			//��Ӻ���/������
	Msg_Del_Friends,				//ɾ������/������
	Msg_Get_FriendsList,			//��ú���/�������б�
	Msg_Get_PlayInfo,				//��ú���/��������Ϣ
	Msg_Fri_Notify,					//���ѵ�֪ͨ��Ϣ

//�������
	Msg_Team_Apply = 450,			//�������
	Msg_Team_ReplyApply,			//�ظ�����
    Msg_Team_Created,               //����ɹ�����
	Msg_Team_NewMember,				//�����³�Ա
	Msg_Team_MemberExit,		    //��Ա�뿪
	Msg_Team_ChangeLeader,			//�ӳ�����
	Msg_Team_KickMemberReq,			//�������
	Msg_Team_Notify,				//�����֪ͨ��Ϣ

//�������
	Msg_Chat_POINT = 500,			//��Ե�����
	Msg_Chat_CHANNEL,				//Ƶ����Ϣ
	Msg_Chat_Notify,				//�����֪ͨ��Ϣ

//���ɹ���
	Msg_Create_Gang = 520,			//��������
	Msg_Apply_Gang,					//����������
	Msg_Reply_ApplyGang,			//�ظ��������
	Msg_Get_GangApplyList,			//��������б�
	Msg_Leave_Gang,					//�뿪����
	Msg_Put_Gang,					//�������
	Msg_Transfer_GangHeader,		//ת�ð���
	Msg_Get_GangList,				//��ð����б�
	Msg_Get_GangAnnounce,
	Msg_Edit_GangAnnounce,
	Msg_Send_GangEmail,
	Msg_Add_GangContribution,
	Msg_Get_GangContribution,
	Msg_Gang_Appoint,
	Msg_Gang_Transfer,
	Msg_Gang_UpGrade,
	Msg_Get_GangLevel,
	Msg_Get_GangInfo,
	Msg_Get_ApplyInfo,

	Msg_Gang_Notify,				//���ɵ�֪ͨ��Ϣ

//�������
	Msg_Apply_Copy = 600,			// ������븱��
	Msg_Copy_Finish,				// �������
	Msg_Player_AttackType,			//ս��״̬ ��ʱ����

	Msg_Task_Collect,				//�ɼ�����

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
	//��ɫ
	Notify_S2C_CreateRole,			//������Ҫ��ͻ��˴�����ɫ
	Notify_S2C_CreateRole_NameRepeat,//��ɫ���Ѵ���
	Notify_S2C_CreateRole_OK,		//��ɫ�����ɹ�
    Notify_S2C_RoleExist,           //�Ѿ���������ɫ��
    Notify_S2C_PlayerOffline,       //��Ҳ�����
	//����
	Notify_S2C_TakeTask_OK = 30,			//�����ȡ�ɹ� 
	Notify_S2C_TakeTask_LevelError,	//�ȼ��ﲻ����ȡ��������
	Notify_S2C_TakeTask_RealmError, //��ȡ���񾳽����
	Notify_S2C_TakeTask_TaskExists, //�����Ѿ��ӹ�
	Notify_S2C_TakeTask_ReqTaskError,//��ȡ����ǰ����������
	Notify_S2C_TaskFinish,			//�����Ѿ���� 
	Notify_S2C_TakeTask_NoTask,  	//���񲻴���
	Notify_S2C_TakeTask_ItemError,	//��Ʒ����
	NotiFy_S2C_Task_BagFull,		//��������
	Notify_S2C_MakeTask_ItemError,	//��Ʒ����
	NotiFy_S2C_MakeTask_DisNpcError,
	Notify_S2C_SubmitTaskError,		//�ύ����ʧ��
	Notify_S2C_SubmitTaskFinish,	//�ύ����ɹ�
	Notify_S2C_GetTaskList_OK,		//��ȡ�����б�ɹ�

	//����
	Notify_S2C_ApplyCopy_TimeError = 50,//δ����������ʱ��
	Notify_S2C_ApplyCopy_LevelError,//�ȼ�δ�ﵽ����Ҫ��
	Notify_S2C_ApplyCopy_EnterNumError,//�����������
	Notify_S2C_ApplyCopy_PlayerNumError,//����δ�ﵽ��������
	Notify_S2C_ApplyCopy_ItemError,	//δ���ֽ��븱������Ҫ�ĵ���
	Notify_S2C_ApplyCopy_OK,		//���븱���ɹ�
	Notify_S2C_Copy_FinishANDNext,	//���� ������һ��
	Notify_S2C_Copy_Finish,			//�������

	//������ʾ
	Notify_S2C_No_Money,
	Notify_S2C_No_Gold,

    //��Ʒ
    Notify_S2C_EquipmentUpStar_Failture = 80,//��Ʒ����ʧ��.
    Notify_S2C_ItemUse_RepeatedAddSkillTree,//�������ظ����.
    Notify_S2C_ItemUse_SkillTreeExpFull,    //��������������
    Notify_S2C_ItemUse_CD_Cooling,          //��Ʒʹ��CD��ȴ��         

	//���ѹ���
	Notify_S2C_ADDFRI_OK = 100,			//������ӳɹ�
	Notify_S2C_ADDEDFRI_OK,				//���ѱ���ӳɹ�
	Notify_S2C_ADDBLACK_OK,				//��������ӳɹ�
	Notify_S2C_ADDFRI_FAILURE,			//�������ʧ��
	Notify_S2C_ADDBLACK_FAILURE,		//���������ʧ��
	Notify_S2C_FRINULL,					//����ID����
	Notify_S2C_DELFRI_OK,				//ɾ�����ѳɹ�
	Notify_S2C_DELBLACK_OK,				//ɾ���������ɹ�
	Notify_S2C_DELFRI_FAILURE,			//ɾ������ʧ��
	Notify_S2C_DELBLACK_FAILURE,		//ɾ��������ʧ��
	Notify_S2C_GETFRILIST_OK,			//��ȡ�����б�ɹ�
	Notify_S2C_GETBLACKLIST_OK,			//��ȡ�������ɹ�
	Notify_S2C_GETFRI_OK,				//��ȡ������Ϣ�ɹ�

	//�������
	Notify_S2C_CRATE_OK = 150,			//��������ɹ�
	Notify_S2C_APPLY_OK,				//�������ɹ�
	Notify_S2C_APPLY_HEADER,			//��ʾ�ӳ�����������Ӷ�
	Notify_S2C_APPLY_FAILURE,			//�������ʧ�ܣ��Է����Ƕӳ���
	Notify_S2C_INVITE_OK,				//������ҳɹ�
	Notify_S2C_INVITE_HEADER,			//��ʾ��ң���������Ӷ�
	Notify_S2C_INVITE_FAILURE,			//����ʧ�ܣ��Է����Ƕӳ���
	Notify_S2C_TeamFull,				//������������
	Notify_S2C_ADDTEAM_OK_HEADER,		//�Ӷӳɹ�_�ӳ�
	Notify_S2C_ADDTEAM_OK,				//�Ӷӳɹ�_��Ա
	Notify_S2C_ADDTEAM_FAILURE_HEADER,	//�Ӷ�ʧ��_�ӳ�
	Notify_S2C_ADDTEAM_FAILURE,			//�Ӷ�ʧ��_��Ա
	Notify_S2C_LEAVETEAM_HEADER,		//�뿪���飨�ӳ���
	Notify_S2C_LEAVETEAM,				//�뿪���飨��Ա��
	Notify_S2C_DISSOLVE,				//��ɢ����
	Notify_S2C_PUTTEAM_HEADER,			//������飨�ӳ���
	Notify_S2C_PUTTEAM,					//������飨��Ա��
	Notify_S2C_GET_TEAMLIST,			//��ö����б�

	//�������
	Notify_S2C_Recive_Point = 200,		//���յ���Ϣ
	Notify_S2C_Recive_Channel,			//����Ƶ����Ϣ

	//���ɹ���
	Notity_S2C_Create_OK = 210,			//�����ɹ�
	Notity_S2C_Create_Fail_InGang,		//����ʧ�ܣ��Ѵ��ڰ���֮��
	Notity_S2C_Create_Fail_RepeatName,	//����ʧ�ܣ���������
	Notity_S2C_Get_GangInfo,			//��ð����Ϣ�ɹ�
	Notity_S2C_Apply_OK,				//����ɹ�
	Notity_S2C_Apply_Fail_InGang,		//����ʧ��
	Notity_S2C_Apply_Fail_NoGang,		//���ɲ�����
	Notity_S2C_Apply_Fail_Repeat,		//�����ظ�
	Notity_S2C_Reply_OK,				//����ɹ�
	Notity_S2C_Reply_Refused,			//����ܾ�
	Notity_S2C_Reply_OK_INGang,			//����ɹ�,���Է��Ѿ��а���
	Notity_S2C_GETAPPLYLIST,			//��ð��������б�
	Notity_S2C_LEAVE_GANG,				//�뿪����
	Notity_S2C_LEAVE_GANG_HEADER,		//�����뿪����
	Notity_S2C_PUT_GANG,				//���������
	Notity_S2C_GET_GANGLIST,			//��ð����б�
	Notity_S2C_GET_Announce,
	Notity_S2C_EDIT_Announce_OK,
	Notity_S2C_TransferHeader_OK,		//ת�ð����ɹ�

	Notify_S2C_MakeTask_OK,

	Notify_MAX
};

//��Ʒ�Ĳ�������(����ɾ����)
enum ItemOperateType
{
    ItemOperate_BagLoading,			//��¼ʱ����
    ItemOperate_EquipLoading,       //����װ������
    ItemOperate_BagReset,			//������
    ItemOperate_ItemUsed,			//����Ʒʹ��
    ItemOperate_WearEquipment,      //����װ��
    ItemOperate_EquipGetOff,		//����װ��
    ItemOperate_EquipUpLevel,		//װ��ǿ��
    ItemOperate_EquipUpUid,         //װ������
    ItemOperate_EquipStone,         //װ����Ƕ
    ItemOperate_EquipStoneRemove,	//���װ����Ƕ��ʯ
    ItemOperate_ItemMerge,			//��Ʒ(��ʯ)�ϳ�
    ItemOperate_TaskReward,         //������
	ItemOpreate_TakeTaskItem,		//��ȡ����ʱ��������Ʒ
	ItemOperate_DoTakeItem,			//����������Ʒ
    ItemOperate_BuyMallItem,		//�����̳��������Ʒ
    ItemOperate_SellBagItem,		//���۱����������Ʒ
    ItemOperate_BuyShopItem,		//npc�̵깺��
    ItemOperate_EquipInherite,      //װ������
	ItemOperate_ItemDrop,			//ս������
	ItemOperate_BuyChest,			//���������
	ItemOperate_Chest,				//��������

    ItemOperate_Max
};
//װ����������
enum EquipmentAttributeType
{
	Attribute_Equipment_HpMaxValue = 1,		//װ�����ӵ�HP����ֵ
	Attribute_Equipment_HpMaxPercent,		//װ�����ӵ�HP���ްٷֱ�
	Attribute_Equipment_MpMaxValue,			//װ�����ӵģͣ�����ֵ
	Attribute_Equipment_MpMaxPercent,		//װ�����ӵ�MP���ްٷֱ�
	Attribute_Equipment_AttackValue,		//װ�����ӵĹ�����
	Attribute_Equipment_AttackPercent,		//װ�����ӵĹ������ٷֱ�
	Attribute_Equipment_DefenseValue,		//װ�����ӵķ�����
	Attribute_Equipment_DefensePercent,		//װ�����ӵķ������ٷֱ�
	Attribute_Equipment_HitValue,			//װ�����ӵ�����ֵ
	Attribute_Equipment_HitPercent,			//װ�����ӵ����аٷֱ�
	Attribute_Equipment_DodgeValue,			//װ�����ӵ�����ֵ
	Attribute_Equipment_DodgePercent,		//װ�����ӵ����ܰٷֱ�
	Attribute_Equipment_CritValue,			//װ�����ӵı���ֵ
	Attribute_Equipment_CritPercent,		//װ�����ӵı����ٷֱ�
	Attribute_Equipment_TenaCityValue,		//װ�����ӵ�����ֵ
	Attribute_Equipment_TenaCityPercent,	//װ�����ӵ����԰ٷֱ�
	Attribute_Equipment_HitRatio,			//װ�����ӵ�������
	Attribute_Equipment_DodgeRatio,			//װ�����ӵ�������
	Attribute_Equipment_CritRatio,			//װ�����ӵı�����
	Attribute_Equipment_TenaCityRatio,		//װ�����ӵ�������
	Attribute_Equipment_MoveSpeedOdds,		//װ�����ӵ��ƶ�ϵ��
	Attribute_Equipment_HurtStateOdds,		//װ�����ӵ��˺�״̬ϵ��
	Attribute_Equipment_AttackStateOdds,	//װ�����ӵĹ���״̬ϵ��
	Attribute_Equipment_TreatStateOdds,		//װ�����ӵ�����״̬ϵ��
	Attribute_Equipment_DoubleCritOdds		//װ�����ӵı�������ϵ��

};

#endif	//__MSGDEF_H__
