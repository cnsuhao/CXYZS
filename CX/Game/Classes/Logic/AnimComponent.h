/********************************************************************
created:	2015/11/30
author:		lishihai
desc:		显示动画类
*********************************************************************/

#ifndef __ANIMCOMPOENT_H__
#define __ANIMCOMPOENT_H__

#include "ClientDef.h"
#include "math/Vec2.h"

namespace cc2d
{
    class Sprite;
    namespace ui
    {
        class LoadingBar;
    }
}

class MapObj;
struct DropHpMode;

class AnimComponent
{
public:
	AnimComponent(MapObj* Owner);
	virtual ~AnimComponent();

    virtual void LoadResouce(bool asyncLoad = true);        //>加载更新
	//动作相关
    virtual void ChangeDirection(DirectionType direction);  //改变方向
	virtual void ChangeAction(ActionType action);			//改变动作
	virtual void UpdataAction();							//更新动作
	virtual void StopAction();				                //停止动作

    virtual void SetOpacityWhenEnterMapTile(int inMask);    //>inMask=1在阴影中,  inMask=0不在阴影中.
    virtual bool IsSelection(const cc2d::Vec2 &point);      //>选中检测
    virtual cc2d::Node* GetBody();                          //>获得显示身体
    
    void EnableShadow(bool enable);
    void EnableHpBar(bool enable);
    void SetHpBarPercent(float percent);
    void PlayDropHpAnim(const DropHpMode& mode);
    void PlayAddHpAnmi(const AddHpMode& mode);
    void PlayEffect(cc2d::Sprite* pEffect, int where = 0);

    virtual void Update(float dt);

    inline DirectionType GetShowDirection(DirectionType direction);
protected:
    void UpdateShadowAndHpBar();
    void ReleaseHpAnim(cc2d::Node *sender);
protected:
    MapObj*      m_Owner;
    DirectionType   m_Direction;
    ActionType      m_ActionType;

    void CheckAsyncLoading(const std::string& resourceKey);
    std::string m_ResourceKey;  //>异步资源名
    bool  m_ResouceLoading;     //>异步资源加载中
    bool  m_AsyncLoad;          //>是否异步加载
    int   m_FrameCount;
    cc2d::Sprite* m_Shadow;     //>影子
    cc2dui::LoadingBar* m_HpBar;//>血条
    float m_DropHPDelay;		//>掉血等待时间,有事会同时多次掉血，多次掉血间需要一个延时时间
};


//npc_动作_方向_编号_帧数.png
//monster_动作_方向_编号_帧数.png
class NpcAnimComp : public AnimComponent
{
public:
    class Interface
    {
    public:
        virtual const char* GetModelName() = 0;
        virtual void        OnAttcakAnimEnd() = 0;
    };

public:
    NpcAnimComp(MapObj* pObject,  Interface* pInterface);

    virtual void LoadResouce(bool asyncLoad = true);        //>加载更新
    void        SetBodyAnchorPoint(const cc2d::Vec2& anchor);
    void        SetBodySacle(float sacle);

    virtual void UpdataAction();							//更新动作
    virtual void StopAction();				                //停止动作
    virtual void SetOpacityWhenEnterMapTile(int inMask);    //>inMask=1在阴影中,  inMask=0不在阴影中.
    virtual bool IsSelection(const cc2d::Vec2 &point);      //>选中检测
    virtual cc2d::Node* GetBody();                          //>获得显示身体

    void AttcakAnimEnd();

private:
    Interface*  m_Interfece;    //接口
    cc2d::Sprite *m_Body;		//身体
};

//player_性别_部位_编号_动作_方向_帧数.png
class PlayerAnimComp : public AnimComponent
{
public:
    class Interface
    {
    public:
        virtual float   AC_GetMoveSpeed() = 0;
        virtual void    GetFaces(int face[EQUIPTYPE_MAX]) = 0;
        virtual int     GetSex() = 0;
        virtual void    OnAttcakAnimEnd() = 0;
    };
public:
    PlayerAnimComp(MapObj* pObject,  Interface* pInterface);

    static cc2d::Node* CreateFaceCopyNode(int equipId[EQUIPTYPE_MAX], int sex);   //>创建一个外观节点

    virtual void LoadResouce(bool asyncLoad = true);        //>加载更新

    virtual void ChangeAction(ActionType action);			//>改变动作
    virtual void UpdataAction();							//>更新动作
    virtual void StopAction();				                //>停止动作
    virtual void SetOpacityWhenEnterMapTile(int inMask);    //>inMask=1在阴影中,  inMask=0不在阴影中.
    virtual bool IsSelection(const cc2d::Vec2 &point);      //>选中检测
    virtual cc2d::Node* GetBody();                          //>获得显示身体

    void ResetDieActionToLastFrame() { m_DieActionToLastFrame = true; } //>死亡动画直接设置到最后一帧,生效一次
    void AttcakAnimEnd();
private:
    Interface*  m_Interfece;    //接口
    cc2d::Sprite *m_Body;	//身体
    cc2d::Sprite *m_Head;	//头
    cc2d::Sprite *m_Weapon;	//武器
    cc2d::Sprite *m_Wing;	//翅膀
    cc2d::Sprite *m_Horse;	//坐骑
    bool          m_DieActionToLastFrame;
};


inline DirectionType AnimComponent::GetShowDirection(DirectionType direction)
{
    if (direction == DIRECTION_LEFT_UP)
    {
        return DIRECTION_RIGHT_UP;
    }

    else if(direction == DIRECTION_LEFT)
    {
        return DIRECTION_RIGHT;
    }

    else if(direction == DIRECTION_LEFT_DOWN)
    {
        return DIRECTION_RIGHT_DOWN;
    }

    return direction;
}

//extern const std::string ObjectLoadImage;

#endif	//__ANIMCOMPOENT_H__
