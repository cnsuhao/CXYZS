#pragma once

#include <iostream>

static inline std::string ConvertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    int len = ret.length();
    for (int i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}

static inline CString ConvertPathFormatToUnixStyle(CString& path)
{
    CString ret = path;
    int len = ret.GetLength();
    for (int i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}

namespace cocos2d
{
    class Node;
    class Sprite;
    class TMXTiledMap;
    class DrawNode;
}

enum NodeType
{
    ROOT,
    NODE,
    MAP,
    SPRITE,
};

class EditClass
{
public:
    EditClass(void);
    virtual ~EditClass(void);


    virtual void OnSelect() = 0;
    virtual cc2d::Node* GetNode() = 0;
    virtual int GetType() const = 0;

    virtual bool Load(std::istream& in);
    virtual bool Save(std::ostream& out);

    void Destroy();


    void RecordOldPos();

    void RecordNewPos();

    void PostMoveUodo();
protected:
    int   currentVersion;   // 当前文件版本
    int   lastVersion;      // 最新版本
private:
    float oldX, oldY;
    float newX, newY;
};


class EditNode : public EditClass
{
public:
    EditNode();
    static EditNode* create();
    virtual void OnSelect();
    virtual cc2d::Node* GetNode() { return node; }
    virtual int GetType() const { return (int)NodeType::NODE; }

    //virtual bool Load(std::istream& in) { return true; }
    //virtual bool Save(std::ostream& out){ return true; }
    cc2d::Node* node;
};

class EditRoot : public EditClass
{
public:
    EditRoot();
    static EditRoot* create();
    virtual void OnSelect();
    virtual cc2d::Node* GetNode() { return node; }
    virtual int GetType() const { return (int)NodeType::ROOT; }
    cc2d::Node* node;

    //virtual bool Load(std::istream& in) { return true; }
    //virtual bool Save(std::ostream& out){ return true; }
    void MoveToScreenCenter();
};

class EditMap : public EditClass
{
public:
    EditMap();
    static EditMap* create();
    virtual void OnSelect() {}
    virtual cc2d::Node* GetNode() { return (cc2d::Node*)map; }
    virtual int GetType() const { return (int)NodeType::MAP; }

    //virtual bool Load(std::istream& in) { return true; }
    //virtual bool Save(std::ostream& out){ return true; }
    cc2d::TMXTiledMap* map;
    std::string fileName;
    cc2d::DrawNode* mapBound;
};

class EditSprite : public EditClass
{
public:
    EditSprite();
    static EditSprite* create(const char* fileName, bool plist = false);
    virtual void OnSelect();
    virtual cc2d::Node* GetNode() { return (cc2d::Node*)sprite; }
    virtual int GetType() const { return (int)NodeType::SPRITE; }
    cc2d::Sprite* sprite;

    virtual bool Load(std::istream& in);
    virtual bool Save(std::ostream& out);
    std::string fileName;
    int plist;
protected:
    void Setup();
};
