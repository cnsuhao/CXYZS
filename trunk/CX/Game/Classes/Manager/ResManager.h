/********************************************************************
created:	2014/12/20
author:		LL
desc:		资源管理：管理图片、声音、粒子的加载和释放
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
	void Init();	//初始化
	void Release();	//释放	
	//释放精灵,一般都作为回调函数自动调用
	void ReleaseSprite(cc2d::Node *sender, long data);	
	
	//根据ImageID指定一个父节点来显示精灵,父节点若为空则加入到当前图层，根据Image表的时间显示，显示完自动释放。
	void ShowSprite(int ImageID, cc2d::Node* Parent = NULL, int layer = LAYER_OBJECT, cc2d::Point pos = cc2d::Point(0,0), float DelayTime = 0.0f, float scale = 1.0f);
	//在当前场景根据ImageID和动作来显示一个精灵，动作播放完成自动释放。
	void ShowSpriteByAction(int ImageID, cc2d::ActionInterval* action, cc2d::Node* Parent = NULL, int layer = LAYER_OBJECT, cc2d::Point pos = cc2d::Point(0,0), float scale = 1.0f);
	//根据ImageID来创建一个精灵，精灵需要手动释放。
	cc2d::Sprite* CreateSprite(int ImageID, cc2d::Point pos = cc2d::Point(0,0));
    //>根据ImageID来创建一个精灵，精灵需要手动释放。
    cc2d::Sprite* CreateBuffSprite(int ImageID, bool& isLoop);
	//每次创建怪物 角色和换装时调用(异步加载)
	cc2d::Sprite* CreatePlayerPartAsync(const string &path);
    //每次创建怪物 角色和换装时调用
    cc2d::Sprite* CreatePlayerPart(const string &path);
	//每次创建npc时调用
	cc2d::Sprite* CreateNPCPart(const string &path);

	//声音 音效 播放
	void PreloadAllSound();   //预加载全部声音
	void PreloadSound(int SoundId);  //预加载声音
	int PlaySound(int SoundId, float gain = 1.0f);  //播放声音
	//设置音量  音效  背景音乐 和 所有 
	void SetBgMusicVolume(float volume);  //0~1.0f 之间
	void SetEffectVolume(float volume);  //0~1.0f 之间
	void SetSoundVolume(float volume);  //0~1.0f 之间
	//停止 
	void StopBgMusic();
	void StopAllEffect();
	void StopAllSound();
	//暂停 和恢复
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
	std::map<string, cc2d::Vector<cc2d::SpriteFrame*>> m_AnimFramesList;	//动画帧队列

    std::function<void(const std::string& path)> m_CreateCallback;
};
extern ResManager* g_ResManager;
#endif  // __RESMANAGER_H__