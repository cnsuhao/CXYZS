#pragma once

enum NodeType
{
    ROOT,
    NODE,
    SPRITE
};

namespace cocos2d
{
    class Node;
    class Sprite;
}

class EditClass
{
public:
    EditClass(void);
    virtual ~EditClass(void);


    virtual void OnSelect() = 0;
    virtual cc2d::Node* GetNode() = 0;
    virtual int GetType() const = 0;
    
    void Destroy();
};


class EditNode : public EditClass
{
public:
    virtual void OnSelect();
    virtual cc2d::Node* GetNode() { return node; }
    virtual int GetType() const { return (int)NodeType::NODE; }
    cc2d::Node* node;
};

class EditRoot : public EditClass
{
public:
    EditRoot();

    virtual void OnSelect();
    virtual cc2d::Node* GetNode() { return node; }
    virtual int GetType() const { return (int)NodeType::ROOT; }
    cc2d::Node* node;

    void MoveToScreenCenter();
};

class EditSprite : public EditClass
{
public:
    virtual void OnSelect();
    virtual cc2d::Node* GetNode() { return (cc2d::Node*)sprite; }
    virtual int GetType() const { return (int)NodeType::SPRITE; }
    cc2d::Sprite* sprite;
};
