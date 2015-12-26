#pragma once

#include "ClientDef.h"
#include "CCEventListenerTouch.h"

class JoystickListener;
class CtrlJoystick : public cc2d::Node
{
    CtrlJoystick(void);
public:
    ~CtrlJoystick(void);
    static CtrlJoystick* create(const char* background, const char* touchImage);
    
    bool Init(const char* background, const char* touchImage);

    void SetListener(JoystickListener* listenter) { _listenter = listenter; }

    void SetEnabled(bool enabled);
    bool GetEnabled() const { return _enabled; }

    void SetDelayShow(float time) { _delayShowTime = time; }
    float GetDelayShow() const { return _delayShowTime; }

    void SetDampSpeed(float speed) { _dampSpeed = speed; }
    float GetDampSpeed() const { return _dampSpeed; }

	void SetJoyDir(int dir) { _joydir = dir; }
	int GetJoyDir() const { return _joydir; }
protected:
    bool onTouchBegan(cc2d::Touch *touch, cc2d::Event *unusedEvent);
    void onTouchMoved(cc2d::Touch *touch, cc2d::Event *unusedEvent);
    void onTouchEnded(cc2d::Touch *touch, cc2d::Event *unusedEvent);
    void Update(float dt);
    void NotityListener(const cc2d::Vec2& dir);
private:
    JoystickListener* _listenter;
    cc2d::EventListenerTouchOneByOne* _touchListener;
    cc2d::Sprite* _background;
    cc2d::Sprite* _touchImage;
    bool _enabled;
    float _delayShowTime;
    float _touchTime;
    cc2d::Size _halfRange;	
    cc2d::Vec2 _target;
    float _dampSpeed;
	int _joydir;			//Ò¡¸Ë·½Ïò£¬Ä¬ÈÏ-1
    bool _realdyOnMoveBigin;
#ifdef _WIN32
    cc2d::Vec2 _touchPosition;
    bool _touched;
#endif // _WIN32
};

