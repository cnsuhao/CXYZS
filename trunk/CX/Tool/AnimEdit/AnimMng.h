#pragma once

#include "GL/glew.h"
#include "2d/CCAction.h"

enum class FrameType
{
    Position = 0,
    Rotation,
    Scale,
    Opacity,
};

struct PositionData
{
    unsigned int frameId;
    cc2d::Vec2 position;
    unsigned char tween;
};

struct RotateData
{
    unsigned int frameId;
    float dstAngle;
    unsigned char tween;
};

struct ScaleData
{
    unsigned int frameId;
    cc2d::Vec2 scale;
    unsigned char tween;
};

struct OpacityData
{
    unsigned int frameId;
    GLubyte opacity;
    unsigned char tween;
};

class AnimFrame
{
public:
    AnimFrame();
    ~AnimFrame();

    PositionData* GetPositionData(unsigned int frameId, bool create = true);
    RotateData* GetRotateData(unsigned int frameId, bool create = true);
    ScaleData* GetScaleData(unsigned int frameId, bool create = true);
    OpacityData* GetOpacityData(unsigned int frameId, bool create = true);


    template <class T>
    T* GetData(unsigned int frameId, std::list<T*>& container, bool create = true);

    void DelPositionData(unsigned int frameId);
    void DelRotateData(unsigned int frameId);
    void DelScaleData(unsigned int frameId);
    void DelOpacityData(unsigned int frameId);

    template <class T>
    void DelData(unsigned int frameId, std::list<T*>& container);

    PositionData* FindPositionData(unsigned int frameId);
    RotateData* FindRotateData(unsigned int frameId);
    ScaleData* FindScaleData(unsigned int frameId);
    OpacityData* FindOpacityData(unsigned int frameId);

    std::list<PositionData*> m_position_array; //MoveTo::create(float duration, const Vec2& position)
    std::list<RotateData*> m_rotation_array; //RotateTo::create(float duration, const Vec3& dstAngle3D)
    std::list<ScaleData*> m_scale_array;    //ScaleTo::create(float duration, float sx, float sy)
    std::list<OpacityData*> m_opacity_array;     //FadeTo::create(float duration, GLubyte opacity)    

};

class AnimMng
{
public:
    AnimMng(void);
    ~AnimMng(void);

    
    void EnterEditNode(EditClass* node, int type);
    void ExitEidtNode();
    bool IsEditingNode() const { return m_currentNode != NULL; }
    EditClass* GetEditingNode() { return m_currentNode; }
    int GetLastEditingFrameId() { return m_lastEditingFrameId; }
    int GetEditingType() { return m_type; }
    void SetFps(float fps) { m_fps = fps; }
    float GetFps() const { return m_fps; }
    void OnPlay();

    bool HasFrame(unsigned int frame);
    void SetFrameData(AnimFrame* frame, int type, unsigned char interpolation, unsigned int frameId, float newValue1, float newValue2 = 0.0f, float newValue3 = 0.0f);
    void SetFrameData(const std::string& name, int type, unsigned char interpolation, unsigned int frameId, float newValue1, float newValue2 = 0.0f, float newValue3 = 0.0f);
    void SetCurrentFrameData(int type, unsigned char interpolation, unsigned int frameId, float newValue1, float newValue2 = 0.0f, float newValue3 = 0.0f);
    void SetCurrentFrameData(int type, unsigned char interpolation, unsigned int frameId);
    void DelCurrentFrameData(int type, unsigned int frameId);
    bool HasCurrentFrameData(int type, unsigned int frameId);
    //float GetFrameData(int type, unsigned int frame);

    AnimFrame* Find(const std::string& name);
protected:
    AnimFrame* AddFrame(const std::string& name);
    void RemoveFrame(const std::string& name);
    void ResetFrame();
    cc2d::FiniteTimeAction* GetInterpolation(unsigned char interpolation, cc2d::ActionInterval* action);

    cc2d::FiniteTimeAction* MakeTranslateAction(const std::list<PositionData*>& PosArray);
    cc2d::FiniteTimeAction* MakeScaleAction(const std::list<ScaleData*>& ScaleArray);
    cc2d::FiniteTimeAction* MakeRotationAction(const std::list<RotateData*>& RotationArray);
    cc2d::FiniteTimeAction* MakeOpacityAction(const std::list<OpacityData*>& OpacityArray);

protected:
    EditClass* m_currentNode;
    int m_type;
    int m_lastEditingFrameId;

    std::map<std::string, AnimFrame*> m_frames;
    float m_fps;
};

extern AnimMng gAnimMng;

