#include "stdafx.h"
#include "EditClass.h"
#include "SelectHelper.h"
#include "EditApp.h"
#include "RedoAndUndoMng.h"

USING_NS_CC;

using namespace std;

static int nodeId = 0;
static int spriteId = 0;

static int spacekeydown = 0;

// lastVersion
// 10001 基础版本
// 10002 EditSprite添加zOrder

EditClass::EditClass(void)
{
    lastVersion = 10002;
}


EditClass::~EditClass(void)
{

}

bool EditClass::Load(std::istream& in)
{
    in >> currentVersion;
    return true;
}

bool EditClass::Save(std::ostream& out)
{
    out << lastVersion << ' ';
    return true;
}

void EditClass::Destroy()
{
    Node* node = GetNode();
    if (node)
    {
        node->removeFromParentAndCleanup(true);
    }
}

void EditClass::RecordOldPos()
{
    oldX = GetNode()->getPositionX();
    oldY = GetNode()->getPositionY();
}

void EditClass::RecordNewPos()
{
    newX = GetNode()->getPositionX();
    newY = GetNode()->getPositionY();
}

void EditClass::PostMoveUodo()
{
    if (oldX != newX || oldY != newY)
    {
        EditorMove& action = RedoAndUndoMng::Instance().NewMoveAction();
        action.SetOld(oldX,oldY);
        action.SetNew(newX, newY);
        action.SetNode(Handler<EditClass>(this));
    }
}

EditNode::EditNode()
{

}

EditNode* EditNode::create()
{
    EditNode* eNode = new EditNode();
    DrawNode* node = DrawNode::create();
    eNode->node = node;
    node->setUserData(eNode);
    node->setContentSize(Size(100,100));
    node->drawSolidRect(Vec2::ZERO, node->getContentSize(), Color4F(1.0f,0,0,0.25f));
    // 设置点击回调
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event)
    {

        cc2d::Node* node = static_cast<Node*>(event->getCurrentTarget());
        Vec2 nsp = node->convertTouchToNodeSpace(touch);
        Size size = node->getContentSize();
        Rect rect(0,0,size.width, size.height);
        if (rect.containsPoint(nsp))
        {
            EditNode* eNode = reinterpret_cast<EditNode*>(node->getUserData());
            // 显示高亮框
            gSelectHepler.SetSelectNode(eNode);
            // 修改鼠标
            //TRACE("----------->onTouchBegin\n");
            gSelectHepler.SetTouchNode(eNode);

            eNode->RecordOldPos();
            return true;
        }
        return false;
    };

    listener->onTouchMoved = [](Touch* touch, Event* event)
    {
        Node* node = static_cast<Node*>(event->getCurrentTarget());
        EditNode* eNode = reinterpret_cast<EditNode*>(node->getUserData());
        if (gSelectHepler.GetTouchNode() == eNode)
        {
            Vec2 dt = touch->getDelta();
            node->setPosition(node->getPosition() + dt);
            gSelectHepler.NodeMoveUpdate();

            eNode->RecordNewPos();
        }
    };

    listener->onTouchEnded = [](Touch* touch, Event* event)
    {
        Node* node = static_cast<Node*>(event->getCurrentTarget());
        EditNode* eNode = reinterpret_cast<EditNode*>(node->getUserData());
        if (gSelectHepler.GetTouchNode() == eNode)
        {
            //GetMyApp()->m_Frame.OnNodeMoveEnd(eNode);
            gSelectHepler.SetTouchNode(NULL);
            eNode->PostMoveUodo();
        }
        //TRACE("----------->onTouchEnd\n");
    };

    stringstream name;
    name << "node_";
    name << ++nodeId;
    node->setName(name.str().c_str());

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);
    return eNode;
}

void EditNode::OnSelect()
{

}

EditSprite* EditSprite::create(const char* fileName, bool plist /*= false*/)
{
    EditSprite* eSprite = new EditSprite();
    Sprite* sprite = Sprite::create();
    eSprite->sprite = sprite;
    eSprite->fileName.assign(fileName);
    eSprite->plist = plist ? 1 : 0;

    eSprite->Setup();

    return eSprite;
}

EditSprite::EditSprite()
{

}

void EditSprite::OnSelect()
{

}

bool EditSprite::Load(std::istream& in)
{
    // 父类数据
    EditClass::Load(in);

    fileName = GetMyApp()->m_Frame.GetResourcePath();
    string str;
    in >> str;
    fileName.append(str);
    in >> plist;
    sprite = Sprite::create();
    Setup();
    Vec2 t2;
    in >> t2.x;
    in >> t2.y;
    sprite->setAnchorPoint(t2);
    float t;
    in >> t;
    sprite->setPositionX(t);
    in >> t;
    sprite->setPositionY(t);
    in >> t;
    sprite->setScaleX(t);
    in >> t;
    sprite->setScaleY(t);
    in >> t; 
    sprite->setRotation(t);
    in >> t;
    sprite->setOpacity((GLubyte)(t * 255.0f));

    if (currentVersion >= 10002)
    {
        int zOrder;
        in >> zOrder;
        sprite->setLocalZOrder(zOrder);
    }
    return true;
}

bool EditSprite::Save(std::ostream& out)
{
    if (sprite)
    {
        // 父类数据
        EditClass::Save(out);

        size_t n = GetMyApp()->m_Frame.GetResourcePath().length();
        int s = fileName.length();
        string wirteFileName = fileName.substr(n, s - n);

        out << ConvertPathFormatToUnixStyle(wirteFileName) << ' ';
        out << plist << ' ';
        Vec2 t = sprite->getAnchorPoint();
        out << t.x << ' ' << t.y << ' ';
        out << sprite->getPositionX() << ' ';
        out << sprite->getPositionY() << ' ';
        out << sprite->getScaleX() << ' ';
        out << sprite->getScaleY()<< ' ';
        out << sprite->getRotation()<< ' ';
        out << (float)sprite->getOpacity() / 255.0f << ' ';
        out << sprite->getLocalZOrder() << ' ';
        return true;
    }
    return false;
}

void EditSprite::Setup()
{
    sprite->setUserData(this);
    if (plist)
    {
        int p = fileName.rfind('.');
        string fileImage(fileName.begin(), fileName.begin() + p);
        int p2 = fileName.rfind('/');
        string keyName(fileName.begin()+p2+1, fileName.begin() + p);
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fileImage + ".plist", fileImage + ".png");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fileImage + "~1.plist", fileImage + "~1.png");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fileImage + "~2.plist", fileImage + "~2.png");
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(fileImage + "~3.plist", fileImage + "~3.png");

        int FrameCount = 0;
        ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fileName);
        FrameCount += dict["frames"].asValueMap().size();
        dict = FileUtils::getInstance()->getValueMapFromFile(fileImage + "~1.plist");
        if (!dict["frames"].isNull())
            FrameCount += dict["frames"].asValueMap().size();
        dict = FileUtils::getInstance()->getValueMapFromFile(fileImage + "~2.plist");
        if (!dict["frames"].isNull())
            FrameCount += dict["frames"].asValueMap().size();
        dict = FileUtils::getInstance()->getValueMapFromFile(fileImage + "~3.plist");
        if (!dict["frames"].isNull())
            FrameCount += dict["frames"].asValueMap().size();

        Vector<SpriteFrame*> animFrames(FrameCount);
        char str[64] = {0};
        for (int i = 0; i < FrameCount; i++)
        {
            sprintf(str, "%s_%d.png", keyName.c_str(), i);
            auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
            if (frame != NULL)
            {
                animFrames.pushBack(frame);
            }else
            {
                log("can't find animation key=%s when loading plist=%s ", str, fileName.c_str());
            }	
        }
        Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
        Animate* animate = Animate::create(animation);
        sprite->runAction(RepeatForever::create(animate));
        animate->update(0.001f);
    }
    else
    {
        sprite->setTexture(fileName);
    }

    // 设置点击回调
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event)
    {
        if (spacekeydown)
            return false;
        Sprite* sprite = static_cast<Sprite*>(event->getCurrentTarget());
        EditSprite* eSprite = reinterpret_cast<EditSprite*>(sprite->getUserData());
        Vec2 nsp = sprite->convertTouchToNodeSpace(touch);
        Size size = sprite->getContentSize();
        Rect rect(0,0,size.width, size.height);
        if (rect.containsPoint(nsp))
        {
            // 显示高亮框
            gSelectHepler.SetSelectNode(eSprite);
            // 修改鼠标
            //TRACE("----------->onTouchBegin\n");
            gSelectHepler.SetTouchNode(eSprite);

            eSprite->RecordOldPos();
            return true;
        }
        return false;
    };

    listener->onTouchMoved = [](Touch* touch, Event* event)
    {
        Sprite* sprite = static_cast<Sprite*>(event->getCurrentTarget());
        EditSprite* eSprite = reinterpret_cast<EditSprite*>(sprite->getUserData());
        if (gSelectHepler.GetTouchNode() == eSprite)
        {
            Vec2 dt = touch->getDelta();
            sprite->setPosition(sprite->getPosition() + dt);
            gSelectHepler.NodeMoveUpdate();

            eSprite->RecordNewPos();
        }
    };

    listener->onTouchEnded = [](Touch* touch, Event* event)
    {
        Sprite* sprite = static_cast<Sprite*>(event->getCurrentTarget());
        EditSprite* eSprite = reinterpret_cast<EditSprite*>(sprite->getUserData());
        if (gSelectHepler.GetTouchNode() == eSprite)
        {
            //GetMyApp()->m_Frame.OnNodeMoveEnd(eSprite);
            gSelectHepler.SetTouchNode(NULL);

            eSprite->PostMoveUodo();
        }
        //TRACE("----------->onTouchEnd\n");
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, sprite);

    stringstream name;
    name << "sprite_";
    name << ++spriteId;
    sprite->setName(name.str().c_str());
}

class MapLayer : public Layer
{
protected:
    MapLayer()
    {
        //增加触摸事件回调
        auto listener1 = EventListenerTouchOneByOne::create();
        listener1->setSwallowTouches(true);
        listener1->onTouchBegan = CC_CALLBACK_2(MapLayer::OnTouchesBegan, this);
        listener1->onTouchMoved = CC_CALLBACK_2(MapLayer::OnTouchesMoved, this);
        listener1->onTouchEnded = CC_CALLBACK_2(MapLayer::OnTouchesEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);

        //键盘监听
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed = CC_CALLBACK_2(MapLayer::OnKeyPressed, this);
        listener->onKeyReleased = CC_CALLBACK_2(MapLayer::OnKeyReleased, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
public:
    static MapLayer* create()
    {
        MapLayer *ret = new (std::nothrow) MapLayer();
        if (ret && ret->init())
        {
            ret->autorelease();
            return ret;
        }
        else
        {
            CC_SAFE_DELETE(ret);
            return nullptr;
        }
    }

    void OnKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
        {
            spacekeydown = 1;
            gSelectHepler.HideSelect();
        }
    }

    void OnKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
        {
            spacekeydown = 0;
            gSelectHepler.NodeMoveUpdate();    
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_DELETE)
        {
            if (gSelectHepler.GetSelectNode())
            {
                GetMyApp()->m_Frame.OnNodeRemove(gSelectHepler.GetSelectNode());
            }
        }

        if (keyCode == EventKeyboard::KeyCode::KEY_Z)
        {
            RedoAndUndoMng::Instance().Undo();
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_Y)
        {
            RedoAndUndoMng::Instance().Redo();
        }
    }

    bool OnTouchesBegan(cocos2d::Touch* touch, cocos2d::Event *event)
    {
        if (spacekeydown)
        {
            return true;
        }
        return false;
    }

    void OnTouchesMoved(cocos2d::Touch* touch, cocos2d::Event *event)
    {
        Node* Root = GetMyApp()->m_Frame.GetView().sceneRoot.Get()->GetNode();

        Vec2 oldPos = Root->getPosition();

        Vec2 newPos = oldPos + touch->getDelta();

        Root->setPosition(newPos);
    }
    void OnTouchesEnded(cocos2d::Touch* touch, cocos2d::Event *event)
    {

    }
};


EditRoot::EditRoot()
{

}

EditRoot* EditRoot::create()
{
    EditRoot* eRoot = new EditRoot();
    MapLayer* node = MapLayer::create();
    eRoot->node = node;
    node->setName("Root");

    return eRoot;
}

void EditRoot::OnSelect()
{

}

void EditRoot::MoveToScreenCenter()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    GetNode()->setPosition(Vec2(visibleSize / 2) + origin);
}

EditMap::EditMap()
{

}

EditMap* EditMap::create()
{
    CFile File;
    CString str = File.OpenFileDialog(0, OFN_FILEMUSTEXIST, _T("Open File"), 
        _T("tmx files(*.tmx)\0*.tmx\0\0"), &GetMyApp()->m_Frame);

    if (!str.IsEmpty())
    {
        TMXTiledMap* map = TMXTiledMap::create(str.c_str()); 
        //初始化地图阻挡
        TMXLayer *layer = (TMXLayer *)map->getLayer("zd");
        if (layer)
        {
            layer->setVisible(false);
        }
        //初始化地图遮罩
        TMXLayer *layer2 = (TMXLayer *)map->getLayer("zz");
        if (layer2)
        {
            layer2->setVisible(false);
        }

        EditMap* eNode = new EditMap();
        eNode->map = map;
        map->setUserData(eNode);
        eNode->fileName.assign(str.c_str()); 


        eNode->mapBound = DrawNode::create();
        Rect drawRect;
        drawRect.origin = Size::ZERO;
        drawRect.size = map->getContentSize();
        eNode->mapBound->drawRect(drawRect.origin, drawRect.size, Color4F::RED);

        return eNode;
    }
    return NULL;
}
