#pragma once


#include "docking.h"




class AnmiView : public CWnd 
{
public:
    AnmiView();
    ~AnmiView();
    virtual void PreCreate(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void OnDraw(CDC* pDC);
    void ResetScrollPosition();
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetUserScale(float scale);
    float GetUserScale() const { return userScale; }

protected:
    void DrawTranslate(class AnimFrame* pFrame, CDC* pDC);
    void DrawScale(class AnimFrame* pFrame, CDC* pDC);
    void DrawRotate(class AnimFrame* pFrame, CDC* pDC);
    void DrawOpacity(class AnimFrame* pFrame, CDC* pDC);

protected:
    CFont font;
    CPen  pen;
    CPen  redPen;
    CPen  bluePan;
    CBrush blueBursh;
    CBrush redBursh;
    bool initialied;
    int indexX;
    int indexY;
    int fontHeight;
    float userScale;
    float userInvScale;
public:
    class Tranform
    {
    public:
        static inline void WorldToView(float wx, float wy, float& vx, float& vy)
        {
            WorldToView(wx, wy, vx, vy, Tranform::viewX, Tranform::viewY, Tranform::offX, Tranform::offY);
        }
        static inline void ViewToClient(float vx, float vy, float& cx, float& cy)
        {
            ViewToClient(vx, vy, cx, cy, Tranform::clinetW, Tranform::clinetH, Tranform::innerScaleX, Tranform::innerScaleY);
        }
        static inline void WorldToClient(float wx, float wy, float& cx, float& cy)
        {
            float tx, ty;
            WorldToView(wx, wy, tx, ty);
            ViewToClient(tx, ty, cx, cy);
        }
        static inline void WorldToClient(float wx, float wy, float& cx, float& cy, 
                                         float userViewX, float userViewY, float userOffX, float userOffY, 
                                         float userClinetW, float userClinetH, float userInnerScaleX, float userInnerScaleY)
        {
            float tx, ty;
            WorldToView(wx, wy, tx, ty, userViewX, userViewY, userOffX, userOffY);
            ViewToClient(tx, ty, cx, cy, userClinetW, userClinetH, userInnerScaleX, userInnerScaleY);
        }
        static void WorldToView(float wx, float wy, float& vx, float& vy, float viewX, float viewY, float offX, float offY);
        static void ViewToClient(float vx, float vy, float& cx, float& cy, float clinetW, float clinetH, float innerScaleX, float innerScaleY);

        //////////////////////////////////////////////////////////////////////////
        static inline void ClientToView(float cx, float cy, float& vx, float& vy)
        {
            ClientToView(cx, cy, vx, vy, Tranform::clinetW, Tranform::clinetH, Tranform::innerScaleX, Tranform::innerScaleY);
        }
        static inline void ViewToWorld(float vx, float vy, float& wx, float& wy)
        {
            ViewToWorld(vx, vy, wx, wy, Tranform::viewX, Tranform::viewY, Tranform::offX, Tranform::offY);
        }
        static inline void ClientToWorld(float cx, float cy, float& wx, float& wy)
        {
            float tx, ty;
            ClientToView(cx, cy, tx, ty);
            ViewToWorld(tx, ty, wx, wy);
        }

        static inline void ClientToWorld(float cx, float cy, float& wx, float& wy, 
            float userViewX, float userViewY, float userOffX, float userOffY, 
            float userClinetW, float userClinetH, float userInnerScaleX, float userInnerScaleY)
        {
            float tx, ty;
            ClientToView(cx, cy, tx, ty, userViewX, userViewY, userOffX, userOffY);
            ViewToWorld(tx, ty, wx, wy, userClinetW, userClinetH, userInnerScaleX, userInnerScaleY);
        }
        
        static void ClientToView(float cx, float cy, float& vx, float& vy, float clinetW, float clinetH, float innerScaleX, float innerScaleY);
        static void ViewToWorld(float vx, float vy, float& wx, float& wy, float viewX, float viewY, float offX, float offY);
        

        static float viewX, viewY;
        static float offX, offY;
        static float clinetW, clinetH;
        static float innerScaleX, innerScaleY;
    };
};

class AnimDocker : public CDocker
{
public:
    AnimDocker(void);
    ~AnimDocker(void);

    void OnEditAnimNode(EditClass* node, int type);
    void ViewRedraw();
    void OnExitAnim();
    void PlayAnim();

    AnmiView m_view;
};