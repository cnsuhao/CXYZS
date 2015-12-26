/********************************************************************
created:	2014/12/20
author:		LL
desc:		��Դ��������ͼƬ�����������ӵļ��غ��ͷ�
*********************************************************************/
#ifndef __RESMANAGER_H__
#define __RESMANAGER_H__

#include "ClientDef.h"
#include "CCActionInterval.h"

class ResManager
{
public: 
	ResManager();
	~ResManager();
	void Init();	//��ʼ��
	void Release();	//�ͷ�	
	//�ͷž���,һ�㶼��Ϊ�ص������Զ�����
	void ReleaseSprite(cc2d::Node *sender, long data);	
	
	//����ImageIDָ��һ�����ڵ�����ʾ����,���ڵ���Ϊ������뵽��ǰͼ�㣬����Image���ʱ����ʾ����ʾ���Զ��ͷš�
	void ShowSprite(int ImageID, cc2d::Node* Parent = NULL, int layer = LAYER_OBJECT, cc2d::Point pos = cc2d::Point(0,0), float DelayTime = 0.0f, float scale = 1.0f);
	//�ڵ�ǰ��������ImageID�Ͷ�������ʾһ�����飬������������Զ��ͷš�
	void ShowSpriteByAction(int ImageID, cc2d::ActionInterval* action, cc2d::Node* Parent = NULL, int layer = LAYER_OBJECT, cc2d::Point pos = cc2d::Point(0,0), float scale = 1.0f);
	//����ImageID������һ�����飬������Ҫ�ֶ��ͷš�
	cc2d::Sprite* CreateSprite(int ImageID, cc2d::Point pos = cc2d::Point(0,0));
    //>����ImageID������һ�����飬������Ҫ�ֶ��ͷš�
    cc2d::Sprite* CreateBuffSprite(int ImageID, bool& isLoop);
	//ÿ�δ������� ��ɫ�ͻ�װʱ����(�첽����)
	cc2d::Sprite* CreatePlayerPartAsync(const string &path);
    //ÿ�δ������� ��ɫ�ͻ�װʱ����
    cc2d::Sprite* CreatePlayerPart(const string &path);
	//ÿ�δ���npcʱ����
	cc2d::Sprite* CreateNPCPart(const string &path);

	//���� ��Ч ����
	void PreloadAllSound();   //Ԥ����ȫ������
	void PreloadSound(int SoundId);  //Ԥ��������
	int PlaySound(int SoundId, float gain = 1.0f);  //��������
	//��������  ��Ч  �������� �� ���� 
	void SetBgMusicVolume(float volume);  //0~1.0f ֮��
	void SetEffectVolume(float volume);  //0~1.0f ֮��
	void SetSoundVolume(float volume);  //0~1.0f ֮��
	//ֹͣ 
	void StopBgMusic();
	void StopAllEffect();
	void StopAllSound();
	//��ͣ �ͻָ�
	void pauseBgMusic();
	void resumeBgMusic();
	void pauseAllEffect();
	void resumeAllEffect();
	void pauseAllSound();
	void resumeAllSound();
	
	cc2d::Vector<cc2d::SpriteFrame*> GetSpriteFrame(string &name)
	{
        static cc2d::Vector<cc2d::SpriteFrame*> empty;

        std::map<string, cc2d::Vector<cc2d::SpriteFrame*>>::iterator it = m_AnimFramesList.find(name);
        if (it != m_AnimFramesList.end())
            return it->second;
        
		return empty; ;
	}
	std::map<string, cc2d::Vector<cc2d::SpriteFrame*>> m_AnimFramesList;	//����֡����

    std::function<void(const std::string& path)> m_CreateCallback;
};
extern ResManager* g_ResManager;
#endif  // __RESMANAGER_H__