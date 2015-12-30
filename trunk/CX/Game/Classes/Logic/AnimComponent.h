/********************************************************************
created:	2015/11/30
author:		lishihai
desc:		��ʾ������
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

    virtual void LoadResouce(bool asyncLoad = true);        //>���ظ���
	//�������
    virtual void ChangeDirection(DirectionType direction);  //�ı䷽��
	virtual void ChangeAction(ActionType action);			//�ı䶯��
	virtual void UpdataAction();							//���¶���
	virtual void StopAction();				                //ֹͣ����

    virtual void SetOpacityWhenEnterMapTile(int inMask);    //>inMask=1����Ӱ��,  inMask=0������Ӱ��.
    virtual bool IsSelection(const cc2d::Vec2 &point);      //>ѡ�м��
    virtual cc2d::Node* GetBody();                          //>�����ʾ����
    
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
    std::string m_ResourceKey;  //>�첽��Դ��
    bool  m_ResouceLoading;     //>�첽��Դ������
    bool  m_AsyncLoad;          //>�Ƿ��첽����
    int   m_FrameCount;
    cc2d::Sprite* m_Shadow;     //>Ӱ��
    cc2dui::LoadingBar* m_HpBar;//>Ѫ��
    float m_DropHPDelay;		//>��Ѫ�ȴ�ʱ��,���»�ͬʱ��ε�Ѫ����ε�Ѫ����Ҫһ����ʱʱ��
};


//npc_����_����_���_֡��.png
//monster_����_����_���_֡��.png
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

    virtual void LoadResouce(bool asyncLoad = true);        //>���ظ���
    void        SetBodyAnchorPoint(const cc2d::Vec2& anchor);
    void        SetBodySacle(float sacle);

    virtual void UpdataAction();							//���¶���
    virtual void StopAction();				                //ֹͣ����
    virtual void SetOpacityWhenEnterMapTile(int inMask);    //>inMask=1����Ӱ��,  inMask=0������Ӱ��.
    virtual bool IsSelection(const cc2d::Vec2 &point);      //>ѡ�м��
    virtual cc2d::Node* GetBody();                          //>�����ʾ����

    void AttcakAnimEnd();

private:
    Interface*  m_Interfece;    //�ӿ�
    cc2d::Sprite *m_Body;		//����
};

//player_�Ա�_��λ_���_����_����_֡��.png
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

    static cc2d::Node* CreateFaceCopyNode(int equipId[EQUIPTYPE_MAX], int sex);   //>����һ����۽ڵ�

    virtual void LoadResouce(bool asyncLoad = true);        //>���ظ���

    virtual void ChangeAction(ActionType action);			//>�ı䶯��
    virtual void UpdataAction();							//>���¶���
    virtual void StopAction();				                //>ֹͣ����
    virtual void SetOpacityWhenEnterMapTile(int inMask);    //>inMask=1����Ӱ��,  inMask=0������Ӱ��.
    virtual bool IsSelection(const cc2d::Vec2 &point);      //>ѡ�м��
    virtual cc2d::Node* GetBody();                          //>�����ʾ����

    void ResetDieActionToLastFrame() { m_DieActionToLastFrame = true; } //>��������ֱ�����õ����һ֡,��Чһ��
    void AttcakAnimEnd();
private:
    Interface*  m_Interfece;    //�ӿ�
    cc2d::Sprite *m_Body;	//����
    cc2d::Sprite *m_Head;	//ͷ
    cc2d::Sprite *m_Weapon;	//����
    cc2d::Sprite *m_Wing;	//���
    cc2d::Sprite *m_Horse;	//����
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
