#include "ResManager.h"
#include "ClientDef.h"
#include "CSVFileManager.h"
#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "CCArmatureDataManager.h"
#include "CCSpriteFrameCache.h"
#include "CCFileUtils.h"
#include "CCDirector.h"
#include "CCTextureCache.h"
#include "CCActionInstant.h"

using namespace CocosDenshion;

ResManager *g_ResManager = NULL;

static const std::string ResObjectDir("Object/");

ResManager::ResManager()
{	
    FileUtils::getInstance()->setPopupNotify(false);
}

ResManager::~ResManager()
{
}

void ResManager::Init()
{	
    m_CreateCallback = ([this](const std::string& path)
    {
        char str[64] = {0};
        char png[64] = {0};
        int i = 0, j = 0, k = 1;
        for (i = 0; i < ACTION_MAX; i++)
        {
            for (j = 0; j < DIRECTION_MAX; j++)
            {
                k = 1;
                Vector<SpriteFrame*> animFrames;
                sprintf(str, "%s_%d_%d", path.c_str(), i, j);

                //>避免重复加载
                std::map<string, Vector<SpriteFrame*>>::iterator it = m_AnimFramesList.find(str);
                if (it != m_AnimFramesList.end() && (false == it->second.empty()))
                    continue;

                while(1)
                {
                    sprintf(png, "%s_%d.png", str, k);
                    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( png );
                    if (!frame)
                    {					
                        break;
                    }
                    animFrames.pushBack(frame);	
                    k++;
                }
                if (animFrames.size() > 0)
                {
                    m_AnimFramesList.insert(std::map<string, Vector<SpriteFrame*>>::value_type(str, animFrames));
                }
            }
        }
    });
}

void ResManager::Release()
{	
}

class SpriteFrameCacheAsync 
{
public:
    static SpriteFrameCacheAsync* getInstance()
    {
        static SpriteFrameCacheAsync* _cacheAsync = NULL;
        if (_cacheAsync == NULL)
        {
            _cacheAsync = new SpriteFrameCacheAsync();
        }
        return _cacheAsync;
    }


    void addSpriteFramesWithFile(const std::string& plist, const std::string& textureFileName, const std::string& path, std::function<void(const std::string& path)>& callback)
    {
        CCASSERT(textureFileName.size()>0, "texture name should not be null");

        if (!FileUtils::getInstance()->isFileExist(plist))
            return;

        std::string fullpath = FileUtils::getInstance()->fullPathForFilename(textureFileName);

        if (_callbacs.find(fullpath) != _callbacs.end())
            return;

        LoadData data;
        data.path = path;
        data.plist = plist;
        data.func = callback;
        _callbacs.insert(std::make_pair(fullpath ,data));

        Director::getInstance()->getTextureCache()->addImageAsync(textureFileName, [this](const std::string& path, cocos2d::Texture2D* pTexture)
        {
            std::map<std::string, LoadData>::iterator it = _callbacs.find(path);
            if (it != _callbacs.end())
            {
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(it->second.plist, pTexture);
                it->second.func(it->second.path);
            }
        });
    }


    struct LoadData
    {
        std::function<void(const std::string& path)> func;
        std::string plist;
        std::string path;
    };

    std::map<std::string, LoadData> _callbacs;

};


void ResManager::ReleaseSprite(Node *sender, long data)
{
	sender->removeFromParentAndCleanup(true);
}

void ResManager::ShowSprite(int ImageID, Node* Parent, int layer, Point pos, float DelayTime, float scale)
{
	char path1[64];
	const ImageInfo *image = g_CSVFileManager->GetImageByID(ImageID);
	if (image)
	{
		if (image->iSoundID > 0)
		{
			PlaySound(image->iSoundID);
		}
		Sprite* sprite = NULL;
		switch ((int)image->bIsList)
		{
		case 0:
			{
				sprintf(path1, "Image/%s", image->strPath.c_str());
				sprite = Sprite::create(path1);
				if (!sprite)
				{
					CCLOG("Load Err：%s", image->strPath.c_str());
					return;
				}
				sprite->runAction(Sequence::create(Hide::create(), DelayTime::create(DelayTime), Show::create(), DelayTime::create(image->fPlayTime), CallFunc::create(std::bind(&ResManager::ReleaseSprite,this,sprite,0)), NULL));	
			}
			break;
		case 1:
			{
				sprite = Sprite::create();
				char path2[64];
				sprintf(path1, "Image/%s.plist", image->strPath.c_str());
				sprintf(path2, "Image/%s.png", image->strPath.c_str());
				SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                sprintf(path1, "Image/%s~1.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~1.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~2.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~2.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~3.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~3.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                
				Vector<SpriteFrame*> animFrames(image->iFrameCount);
				char str[64] = {0};
				for(int i = 0; i < image->iFrameCount; i++) 
				{
					sprintf(str, "%s_%d.png", image->strPath.c_str(), i);
					auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( str );
                    if (frame != NULL)
                    {
                        animFrames.pushBack(frame);
                    }else
                    {
                        log("can't find animation key=%s when loading plist=%s ", str, image->strPath.c_str());
                    }
				}
				Animation* animation = Animation::createWithSpriteFrames(animFrames, image->fPlayTime/image->iFrameCount);
				sprite->runAction(Sequence::create(DelayTime::create(DelayTime), Animate::create(animation), CallFunc::create(std::bind(&ResManager::ReleaseSprite,this,sprite,0)), NULL));	
			}
			break;
		default:
			return;
		}
		sprite->setScale(scale);
		sprite->setAnchorPoint(Point(0.5f, 0.5f));
		sprite->setPosition(Point(pos + Point(image->iOffsetX, image->iOffsetY)));				
		if (Parent == NULL)
		{
			g_SceneManager->m_CurrLayer->addChild(sprite, layer);
		} 
		else
		{
			Parent->addChild(sprite, layer);
		}
	}
}

void ResManager::ShowSpriteByAction(int ImageID, ActionInterval* action, Node* Parent, int layer, Point pos, float scale)
{
	char path1[64];
	const ImageInfo *image = g_CSVFileManager->GetImageByID(ImageID);
	if (image)
	{
		if (image->iSoundID > 0)
		{
			PlaySound(image->iSoundID);
		}
		Sprite* sprite = NULL;
		switch ((int)image->bIsList)
		{
		case 0:
			{
				sprintf(path1, "Image/%s", image->strPath.c_str());
				sprite = Sprite::create(path1);
				if (!sprite)
				{
					CCLOG("Load Err：%s", image->strPath.c_str());
					return;
				}
			}
			break;
		case 1:
			{
				sprite = Sprite::create();				
				char path2[64];
				sprintf(path1, "Image/%s.plist", image->strPath.c_str());
				sprintf(path2, "Image/%s.png", image->strPath.c_str());
				SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                sprintf(path1, "Image/%s~1.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~1.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~2.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~2.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~3.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~3.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }

				Vector<SpriteFrame*> animFrames(image->iFrameCount);
				char str[64] = {0};
				for(int i = 0; i < image->iFrameCount; i++) 
				{
					sprintf(str, "%s_%d.png", image->strPath.c_str(), i);
					auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( str );
                    if (frame != NULL)
                    {
                        animFrames.pushBack(frame);
                    }else
                    {
                        log("can't find animation key=%s when loading plist=%s ", str, image->strPath.c_str());
                    }		
				}
				Animation* animation = Animation::createWithSpriteFrames(animFrames, image->fPlayTime/image->iFrameCount);
				sprite->runAction(Animate::create(animation));
			}
			break;
		default:
			return;
		}
		sprite->setScale(scale);
		sprite->setAnchorPoint(Point(0.5f, 0.5f));
		sprite->setPosition(Point(pos + Point(image->iOffsetX, image->iOffsetY)));		
		sprite->runAction(Sequence::create(action, CallFunc::create(std::bind(&ResManager::ReleaseSprite,this,sprite,0)), NULL));	
		if (Parent == NULL)
		{
			g_SceneManager->m_CurrLayer->addChild(sprite, layer);
		} 
		else
		{
			Parent->addChild(sprite, layer);
		}		
	}
}

Sprite* ResManager::CreateSprite(int ImageID, Point pos)
{
	char path1[64];
	const ImageInfo *image = g_CSVFileManager->GetImageByID(ImageID);		
	if (image)
	{
		if (image->iSoundID > 0)
		{
			PlaySound(image->iSoundID);
		}
		Sprite* sprite = NULL;	
		switch ((int)image->bIsList)
		{
		case 0:
			{
				sprintf(path1, "Image/%s", image->strPath.c_str());
				sprite = Sprite::create(path1);
				if (!sprite)
				{
					CCLOG("Load Err：%s", image->strPath.c_str());
					return NULL;
				}
			}
			break;
		case 1:
			{
				sprite = Sprite::create();
				
				char path2[64];
				sprintf(path1, "Image/%s.plist", image->strPath.c_str());
				sprintf(path2, "Image/%s.png", image->strPath.c_str());
				SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                sprintf(path1, "Image/%s~1.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~1.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~2.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~2.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~3.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~3.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }

				Vector<SpriteFrame*> animFrames(image->iFrameCount);
				char str[64] = {0};
				for(int i = 0; i < image->iFrameCount; i++) 
				{
					sprintf(str, "%s_%d.png", image->strPath.c_str(), i);
					auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( str );
					if (!frame)
					{
						CCLOG("Load Err：%s", str);
						break;
					}
					animFrames.pushBack(frame);				
				}
				Animation* animation = Animation::createWithSpriteFrames(animFrames, image->fPlayTime/image->iFrameCount);
				sprite->runAction(RepeatForever::create( Animate::create(animation)));
			}
			break;
		default:
			return NULL;
		}
		sprite->setAnchorPoint(Point(0.5f, 0.5f));
		sprite->setPosition(pos + Point(image->iOffsetX, image->iOffsetY));
		return sprite;
	}
	return NULL;
}

cc2d::Sprite* ResManager::CreateBuffSprite(int ImageID, bool& isLoop)
{
    char path1[64];
    const ImageInfo *image = g_CSVFileManager->GetImageByID(ImageID);
    if (image)
    {
        if (image->iSoundID > 0)
        {
            PlaySound(image->iSoundID);
        }
        Sprite* sprite = NULL;
        switch ((int)image->bIsList)
        {
        case 0:
            {
                return NULL;
            }
            break;
        case 1:
            {
                sprite = Sprite::create();
                char path2[64];
                sprintf(path1, "Image/%s.plist", image->strPath.c_str());
                sprintf(path2, "Image/%s.png", image->strPath.c_str());
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                sprintf(path1, "Image/%s~1.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~1.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~2.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~2.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }
                sprintf(path1, "Image/%s~3.plist", image->strPath.c_str());
                if (FileUtils::getInstance()->isFileExist(path1))
                {
                    sprintf(path2, "Image/%s~3.png", image->strPath.c_str());
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(path1, path2);
                }

                Vector<SpriteFrame*> animFrames(image->iFrameCount);
                char str[64] = {0};
                for(int i = 0; i < image->iFrameCount; i++) 
                {
                    sprintf(str, "%s_%d.png", image->strPath.c_str(), i);
                    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( str );
                    if (frame != NULL)
                    {
                        animFrames.pushBack(frame);
                    }else
                    {
                        log("can't find animation key=%s when loading plist=%s ", str, image->strPath.c_str());
                    }
                }
                Animation* animation = Animation::createWithSpriteFrames(animFrames, image->fPlayTime/image->iFrameCount);
                if (!image->bLoop)
                {
                    sprite->runAction(Sequence::create(Animate::create(animation), CallFunc::create(std::bind(&ResManager::ReleaseSprite,this,sprite,0)), NULL));
                }
                else
                {
                    sprite->runAction(RepeatForever::create( Animate::create(animation)));
                }
            }
            break;
        default:
            return NULL;
        }

        isLoop = image->bLoop;
        sprite->setAnchorPoint(Point(0.5f, 0.5f));
        sprite->setPosition(Point(image->iOffsetX, image->iOffsetY));

        return sprite;
    }

    return NULL;
}

Sprite* ResManager::CreatePlayerPartAsync(const string &path)
{

    std::string fullpath = ResObjectDir+path;
    SpriteFrameCacheAsync::getInstance()->addSpriteFramesWithFile(fullpath+".plist", fullpath+".png", path, m_CreateCallback);
    //一张放不下的情况
    if (FileUtils::getInstance()->isFileExist(fullpath+"~1.plist"))
        SpriteFrameCacheAsync::getInstance()->addSpriteFramesWithFile(fullpath+"~1.plist", fullpath+"~1.png", path, m_CreateCallback);
    
    if (FileUtils::getInstance()->isFileExist(fullpath+"~2.plist"))
        SpriteFrameCacheAsync::getInstance()->addSpriteFramesWithFile(fullpath+"~2.plist", fullpath+"~2.png", path, m_CreateCallback);

    if (FileUtils::getInstance()->isFileExist(fullpath+"~3.plist"))
        SpriteFrameCacheAsync::getInstance()->addSpriteFramesWithFile(fullpath+"~3.plist", fullpath+"~3.png", path, m_CreateCallback);

    return NULL;
}

Sprite* ResManager::CreatePlayerPart(const string &path)
{
    std::string fullpath = ResObjectDir+path;
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fullpath+".plist", fullpath+".png");
    //一张放不下的情况
    if (FileUtils::getInstance()->isFileExist(fullpath+"~1.plist"))
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fullpath+"~1.plist", fullpath+"~1.png");

    if (FileUtils::getInstance()->isFileExist(fullpath+"~2.plist"))
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fullpath+"~2.plist", fullpath+"~2.png");

    if (FileUtils::getInstance()->isFileExist(fullpath+"~3.plist"))
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fullpath+"~3.plist", fullpath+"~3.png");	

    m_CreateCallback(path);

    return NULL;
}

Sprite* ResManager::CreateNPCPart(const string &path)
{	
	static string wjj("Object/");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(wjj+path+".plist", wjj+path+".png");

	char str[64] = {0};
	char png[64] = {0};
	int i = 0, j = 0, k = 1;
	for (i = 0; i < ACTION_MAX; i++)
	{
		int j = DIRECTION_RIGHT_DOWN;
		k = 1;
		Vector<SpriteFrame*> animFrames;
		sprintf(str, "%s_%d_%d", path.c_str(), i, j);

        //>避免重复加载
        std::map<string, Vector<SpriteFrame*>>::iterator it = m_AnimFramesList.find(str);
        if (it != m_AnimFramesList.end() && (false == it->second.empty()))
            continue;

		while(1)
		{
			sprintf(png, "%s_%d.png", str, k);
			auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( png );
			if (!frame)
			{					
				break;
			}
			animFrames.pushBack(frame);	
			k++;
		}
		if (animFrames.size() > 0)
		{
			m_AnimFramesList.insert(std::map<string, Vector<SpriteFrame*>>::value_type(str, animFrames));
		}


		j = DIRECTION_LEFT_DOWN;
		k = 1;
		animFrames.clear();
		sprintf(str, "%s_%d_%d", path.c_str(), i, j);
		while(1)
		{
			sprintf(png, "%s_%d.png", str, k);
			auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName( png );
			if (!frame)
			{					
				break;
			}
			animFrames.pushBack(frame);	
			k++;
		}
		if (animFrames.size() > 0)
		{
			m_AnimFramesList.insert(std::map<string, Vector<SpriteFrame*>>::value_type(str, animFrames));
		}

	}
	return NULL;
}
int ResManager::PlaySound(int SoundId, float gain) 
{
 	const SoundInfo *info = g_CSVFileManager->GetSoundByID(SoundId);
	if (!info) return -1;
 	bool b = false;
 	if (info->iPlayCount <0)
 	{
 		b = true;
 	}
 
 	if (info->iType == 0)
 	{
 		SimpleAudioEngine::getInstance()->playBackgroundMusic(info->strPath.c_str(),b);
 		return -1;
 	}
 	else if (info->iType == 1)
 	{
 		return SimpleAudioEngine::getInstance()->playEffect(info->strPath.c_str(),b, 1.0f, 0.0f, gain);
 	}
	return -1;
}

void ResManager::PreloadAllSound()
{
 	/*SoundInfo *info = g_CSVFileManager->GetSoundBegin();
	int count = g_CSVFileManager->GetSoundCount();
	int num = 0;

	do 
	{
		if (info->Type == 0)
		{
			SimpleAudioEngine::getInstance()->preloadBackgroundMusic(info->Path.c_str());
		}
		else if (info->Type == 1)
		{
			SimpleAudioEngine::getInstance()->preloadEffect(info->Path.c_str());
		}

		num++;
		info = g_CSVFileManager->GetSoundNext();

	} while (num<count);*/
}

void ResManager::PreloadSound( int SoundId )
{
	const SoundInfo *info = g_CSVFileManager->GetSoundByID(SoundId);

	if (info->iType == 0)
	{
		SimpleAudioEngine::getInstance()->preloadBackgroundMusic(info->strPath.c_str());
	}
	else if (info->iType == 1)
	{
		SimpleAudioEngine::getInstance()->preloadEffect(info->strPath.c_str());
	}
}

void ResManager::SetBgMusicVolume( float volume )
{
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

void ResManager::SetEffectVolume( float volume )
{
	SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

void ResManager::SetSoundVolume( float volume )
{
	SetBgMusicVolume(volume);
	SetEffectVolume(volume);
}

void ResManager::StopBgMusic()
{
	SimpleAudioEngine::getInstance()->stopBackgroundMusic(false);
}

void ResManager::StopAllEffect()
{
	SimpleAudioEngine::getInstance()->stopAllEffects();
}

void ResManager::StopAllSound()
{
	StopBgMusic();
	StopAllEffect();
}

void ResManager::pauseBgMusic()
{
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void ResManager::resumeBgMusic()
{
	SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void ResManager::pauseAllEffect()
{
	SimpleAudioEngine::getInstance()->pauseAllEffects();
}

void ResManager::resumeAllEffect()
{
	SimpleAudioEngine::getInstance()->resumeAllEffects();
}

void ResManager::pauseAllSound()
{
	pauseBgMusic();
	pauseAllEffect();
}

void ResManager::resumeAllSound()
{
	resumeBgMusic();
	resumeAllEffect();
}


