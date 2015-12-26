#include "UITalk.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCSGUIReader.h"
#include "CocoStudio.h"
#include "UIManager.h"
#include "BaseFunction.h"
#include "NetManager.h"


using namespace cocostudio;

UITalk::UITalk()
{

}

UITalk::~UITalk()
{
}

bool UITalk::init()
{
	bool bRet = false;
	do 
	{
		Size winSize = Director::getInstance()->getVisibleSize();

		CC_BREAK_IF(!UIBase::init());
		m_talkPanel = CSLoader::createNode("UI/Talk.csb");	
		addChild(m_talkPanel);

		m_txNPC = static_cast<Text*>(m_talkPanel->getChildByName("tx_npc"));
		m_imgNPC = static_cast<ImageView*>(m_talkPanel->getChildByName("img_npc"));
		m_txNPCname = static_cast<Text*>(m_talkPanel->getChildByName("tx_npcname"));
		m_txHERO = static_cast<Text*>(m_talkPanel->getChildByName("tx_hero"));
		m_imgHERO = static_cast<ImageView*>(m_talkPanel->getChildByName("img_hero"));
		m_txHEROname = static_cast<Text*>(m_talkPanel->getChildByName("tx_heroname"));
		m_txHERO->setVisible(false);
		m_imgHERO->setVisible(false);

		m_iCurrProgress = 0;
		m_iAllProgress = 0;

		m_iClickNpc = 0;
		m_iTaskid = 0;
		m_IsPre = true;
		m_bIsTalk = true;
		m_bIsReceiveTask = true;

		m_receive = static_cast<Layout*>(m_talkPanel->getChildByName("panel_reveice"));

		Button *btn = static_cast<Button*>(m_receive->getChildByName("Button_1"));
		if (btn)
		{
			btn->addClickEventListener([this](Ref*)
			{
				GetTaskClick();
			});
		}

		m_bg = static_cast<Layout*>(m_talkPanel->getChildByName("panel_temp"));
		if (m_bg)
		{
			m_bg->addClickEventListener([this](Ref* _sender)
			{
				PageClick();
			});
		}

		bRet = true;
	} while (0);

	return bRet;
}

void UITalk::InitProgress(int32 taskid,int64 npcid,bool bIsReceiveTask)
{
	m_bIsReceiveTask = bIsReceiveTask;
	m_iClickNpc = npcid;
	const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(m_iClickNpc);
	static_cast<ImageView*>(m_talkPanel->getChildByName("img_bg"))->setVisible(true);
	m_txNPC->setVisible(true);
	m_imgNPC->setVisible(true);
	m_txNPCname->setVisible(true);
	m_txHERO->setVisible(false);
	m_imgHERO->setVisible(false);
	m_txHEROname->setVisible(false);
	m_IsPre = true;
	m_receive->setVisible(false);

	if (taskid == 0)
	{
		m_bIsTalk = true;
		if (npcinfo)
		{
			//默认对话
			m_txNPCname->setString(BaseFunction::a2u(npcinfo->strName));
			m_txNPC->setString(BaseFunction::a2u(npcinfo->strTalk[0]));
			m_imgNPC->loadTexture(npcinfo->strTalkHead);
		}
	}
	else
	{
		//隐藏物品奖励框
		//边框
		static_cast<ImageView*>(m_receive->getChildByName("Image_14"))->setVisible(false);
		static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy"))->setVisible(false);
		static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy"))->setVisible(false);
		static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy_0"))->setVisible(false);
		//物品
		static_cast<ImageView*>(m_receive->getChildByName("Image_18"))->setVisible(false);
		static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy"))->setVisible(false);
		static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_0"))->setVisible(false);
		static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_1"))->setVisible(false);
		//数量
		static_cast<Text*>(m_receive->getChildByName("Text_22"))->setVisible(false);
		static_cast<Text*>(m_receive->getChildByName("Text_22_Copy"))->setVisible(false);
		static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_0"))->setVisible(false);
		static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_1"))->setVisible(false);

		m_iTaskid = taskid;
		const TaskInfo* m_taskTemp = g_CSVFileManager->GetTaskByID(m_iTaskid);

		m_bIsTalk = false;
		m_iCurrProgress = 0;
		m_iAllProgress = 0;
		if (m_bIsReceiveTask)
		{
			m_ProtalkInfo = g_CSVFileManager->GetTalkByID(m_taskTemp->iReceivableTalkID);
			m_talkInfo = g_CSVFileManager->GetTalkByID(m_taskTemp->iAcceptTalkID);

			char szTemp[20];
			int32 dropid = m_taskTemp->iReceiveDropID;
			if (dropid != 0)
			{
				int Num = 0;
				
				std::map<int32,std::vector<Drop>>::iterator it = g_CSVFileManager->m_DropMap.find(dropid);
				if(it != g_CSVFileManager->m_DropMap.end())
				{
					std::vector<Drop>::iterator itDrop;
					for(itDrop = it->second.begin();itDrop != it->second.end();itDrop++, Num++)
					{					
						switch (Num)
						{
						case 1:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22"))->setString(szTemp);
							}
							break;
						case 2:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy"))->setString(szTemp);
							}
							break;
						case 3:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_0"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_0"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_0"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_0"))->setString(szTemp);
							}
							break;
						case 4:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy_0"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_1"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_1"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_1"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_1"))->setString(szTemp);
							}
							break;
						default:
							break;
						}
					}
				}
			}
			static_cast<ImageView*>(m_receive->getChildByName("Image_12"))->loadTexture(npcinfo->strTalkHead);
			static_cast<Text*>(m_receive->getChildByName("Text_4_Copy"))->setString(BaseFunction::a2u(m_taskTemp->strTaskDesc));
			static_cast<Text*>(m_receive->getChildByName("Text_4"))->setString(BaseFunction::a2u(m_taskTemp->strName));
			static_cast<Text*>(m_receive->getChildByName("Text_3"))->setString(BaseFunction::a2u(npcinfo->strName));
			static_cast<Button*>(m_receive->getChildByName("Button_1"))->setTitleText(BaseFunction::a2u("领取任务"));
		}
		else
		{
			m_ProtalkInfo = g_CSVFileManager->GetTalkByID(m_taskTemp->iAchieveTalkID);
			m_talkInfo = g_CSVFileManager->GetTalkByID(m_taskTemp->iReferTalkID);

			static_cast<ImageView*>(m_receive->getChildByName("Image_12"))->loadTexture(npcinfo->strTalkHead);
			static_cast<Text*>(m_receive->getChildByName("Text_4_Copy"))->setString(BaseFunction::a2u(m_taskTemp->strTaskDesc));
			static_cast<Text*>(m_receive->getChildByName("Text_4"))->setString(BaseFunction::a2u(m_taskTemp->strName));
			static_cast<Text*>(m_receive->getChildByName("Text_3"))->setString(BaseFunction::a2u(npcinfo->strName));
			static_cast<Button*>(m_receive->getChildByName("Button_1"))->setTitleText(BaseFunction::a2u("交付任务"));

			char szTemp[20];
			int32 dropid = m_taskTemp->iReferDropID;
			if (dropid != 0)
			{
				int Num = 0;
				std::map<int32,std::vector<Drop>>::iterator it = g_CSVFileManager->m_DropMap.find(dropid);

				if(it != g_CSVFileManager->m_DropMap.end())
				{
					std::vector<Drop>::iterator itDrop;
					for(itDrop = it->second.begin();itDrop != it->second.end();itDrop++)
					{
						Num++;
						switch (Num)
						{
						case 1:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22"))->setString(szTemp);
							}
							break;
						case 2:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy"))->setString(szTemp);
							}
							break;
						case 3:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_0"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_0"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_0"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_0"))->setString(szTemp);
							}
							break;
						case 4:
							{
								const ItemsInfo *itemTemp = g_CSVFileManager->GetItemsByID(itDrop->iItemid);
								static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy_0"))->setVisible(true);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_1"))->setVisible(true);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_1"))->setVisible(true);
								sprintf(szTemp,"%d",itDrop->iItemNum);
								static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_1"))->loadTexture(itemTemp->strDropIcon);
								static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_1"))->setString(szTemp);
							}
							break;
						default:
							break;
						}
					}
				}
			}
		}
		

		//先判断前置对话有几段
		if (m_ProtalkInfo != nullptr)
		{
			for (int i = 0; i < 8 ; ++i)
			{
				if (m_ProtalkInfo->strDesc[i] == "NULL")
				{
					m_iAllProgress = i;
					break;
				}
			}

			const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(m_ProtalkInfo->iObjID[0]);
			m_imgNPC->loadTexture(npcinfo->strTalkHead);
			m_txNPCname->setString(BaseFunction::a2u(npcinfo->strName));
			m_txNPC->setString(BaseFunction::a2u(m_ProtalkInfo->strDesc[0]));
		}
		else
		{
			static_cast<ImageView*>(m_talkPanel->getChildByName("img_bg"))->setVisible(false);
			m_txNPC->setVisible(false);
			m_imgNPC->setVisible(false);
			m_txNPCname->setVisible(false);
			m_txHERO->setVisible(false);
			m_imgHERO->setVisible(false);
			m_txHEROname->setVisible(false);
			m_IsPre = false;
			m_receive->setVisible(true);
		}
	}
}

void UITalk::PageClick()
{
	if (m_bIsTalk)
	{
		g_UIManager->HideUI(UI_TALK);
		g_UIManager->OpenUI(UI_MAIN);
		return;
	}

	m_iCurrProgress ++ ;
	if (m_iCurrProgress < m_iAllProgress)
	{
		if (m_iCurrProgress/2 != 1)
		{
			static_cast<ImageView*>(m_talkPanel->getChildByName("img_bg"))->setVisible(true);
			m_txNPC->setVisible(false);
			m_imgNPC->setVisible(false);
			m_txNPCname->setVisible(false);
			m_txHERO->setVisible(true);
			m_imgHERO->setVisible(true);
			m_txHEROname->setVisible(true);
			if (m_IsPre)
			{
				m_txHERO->setString(BaseFunction::a2u(m_ProtalkInfo->strDesc[m_iCurrProgress]));
			}
			else
			{
				m_txHERO->setString(BaseFunction::a2u(m_talkInfo->strDesc[m_iCurrProgress]));
			}

		}
		else
		{
			static_cast<ImageView*>(m_talkPanel->getChildByName("img_bg"))->setVisible(true);
			m_txNPC->setVisible(true);
			m_imgNPC->setVisible(true);
			m_txNPCname->setVisible(true);
			m_txHERO->setVisible(false);
			m_imgHERO->setVisible(false);
			m_txHEROname->setVisible(false);
			if (m_IsPre)
			{
				const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(m_ProtalkInfo->iObjID[0]);
				m_imgNPC->loadTexture(npcinfo->strTalkHead);
				m_txNPCname->setString(BaseFunction::a2u(npcinfo->strName));
				m_txNPC->setString(BaseFunction::a2u(m_ProtalkInfo->strDesc[m_iCurrProgress]));
			}
			else
			{
				const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(m_talkInfo->iObjID[0]);
				m_imgNPC->loadTexture(npcinfo->strTalkHead);
				m_txNPCname->setString(BaseFunction::a2u(npcinfo->strName));
				m_txNPC->setString(BaseFunction::a2u(m_talkInfo->strDesc[m_iCurrProgress]));
			}
		}
	}
	else
	{
		if (m_IsPre)
		{
			m_IsPre = false;
			m_receive->setVisible(true);

			//隐藏物品奖励框
			//边框
			static_cast<ImageView*>(m_receive->getChildByName("Image_14"))->setVisible(false);
			static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy"))->setVisible(false);
			static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy"))->setVisible(false);
			static_cast<ImageView*>(m_receive->getChildByName("Image_14_Copy_Copy_0"))->setVisible(false);
			//物品
			static_cast<ImageView*>(m_receive->getChildByName("Image_18"))->setVisible(false);
			static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy"))->setVisible(false);
			static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_0"))->setVisible(false);
			static_cast<ImageView*>(m_receive->getChildByName("Image_18_Copy_1"))->setVisible(false);
			//数量
			static_cast<Text*>(m_receive->getChildByName("Text_22"))->setVisible(false);
			static_cast<Text*>(m_receive->getChildByName("Text_22_Copy"))->setVisible(false);
			static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_0"))->setVisible(false);
			static_cast<Text*>(m_receive->getChildByName("Text_22_Copy_1"))->setVisible(false);

			static_cast<ImageView*>(m_talkPanel->getChildByName("img_bg"))->setVisible(false);
			m_txNPC->setVisible(false);
			m_imgNPC->setVisible(false);
			m_txNPCname->setVisible(false);
			m_txHERO->setVisible(false);
			m_imgHERO->setVisible(false);
			m_txHEROname->setVisible(false);
		}
		else
		{
			g_UIManager->HideUI(UI_TALK);
			g_UIManager->OpenUI(UI_MAIN);
		}
	}
}

void UITalk::GetTaskClick()
{
	//界面显示
	static_cast<ImageView*>(m_talkPanel->getChildByName("img_bg"))->setVisible(true);
	m_txNPC->setVisible(true);
	m_imgNPC->setVisible(true);
	m_txNPCname->setVisible(true);
	m_txHERO->setVisible(false);
	m_imgHERO->setVisible(false);
	m_txHEROname->setVisible(false);
	m_receive->setVisible(false);
	m_iCurrProgress = 0;
	m_iAllProgress = 0;
	//先判断后置对话有几段
	if (m_talkInfo != nullptr)
	{
		for (int i = 0; i < 8 ; ++i)
		{
			if (m_talkInfo->strDesc[i] == "NULL")
			{
				m_iAllProgress = i;
				break;
			}
		}
		
		const NPCInfo* npcinfo = g_CSVFileManager->GetNPCInfoByID(m_talkInfo->iObjID[0]);
		m_imgNPC->loadTexture(npcinfo->strTalkHead);
		m_txNPCname->setString(BaseFunction::a2u(npcinfo->strName));

		m_txNPC->setString(BaseFunction::a2u(m_talkInfo->strDesc[0]));
	}
	else
	{
		g_UIManager->HideUI(UI_TALK);
		g_UIManager->OpenUI(UI_MAIN);
	}

	const TaskInfo* m_taskTemp = g_CSVFileManager->GetTaskByID(m_iTaskid);

	if (m_bIsReceiveTask)
	{
		//领取任务
		MsgTask_TakeTask msg;
		msg.taskId = m_taskTemp->id;
		msg.iReqTaskID = m_taskTemp->iReqTaskID;
		msg.itemId = 0;
		msg.MsgID = Msg_Task_Take;
		g_NetManager->SendMsg(msg);
	}
	else
	{
		//交接任务
		MsgTask_SubmitTask msg;
		msg.taskId = m_taskTemp->id;
		msg.MsgID = Msg_Task_Submit;
		g_NetManager->SendMsg(msg);
	}
}