#include "CtrlRichItem.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCFileUtils.h"
#include "CCEventListenerTouch.h"
#include "CCEventDispatcher.h"

bool RichItem::init(int tag, const Color3B &color, GLubyte opacity)
{
    _tag = tag;
    _color = color;
    _opacity = opacity;
    return true;
}    

RichItemNewLine* RichItemNewLine::create(int tag)
{
    RichItemNewLine* element = new RichItemNewLine();
    if (element && element->init(tag))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}

bool RichItemNewLine::init(int tag)
{
    if (RichItem::init(tag, Color3B(0,0,0), 0))
    {
        return true;
    }
    return false;
}

RichItemText* RichItemText::create(int tag, const Color3B &color, GLubyte opacity, const char *text, const char *fontName, float fontSize)
{
    RichItemText* element = new RichItemText();
    if (element && element->init(tag, color, opacity, text, fontName, fontSize))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}

RichItemText* RichItemText::create(int tag, const cc2d::Color3B& color, const char* text)
{
    RichItemText* element = new RichItemText();
    if (element && element->init(tag, color, 255, text, "", 22))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}

RichItemText* RichItemText::create(int tag, const cc2d::Color3B& color, const char* text, const cc2d::Color4B& linkcolor)
{
    RichItemText* element = new RichItemText();
    if (element && element->init(tag, color, 255, text, "", 22))
    {
        element->autorelease();
        element->enableLinkLine(linkcolor, 2);
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}

bool RichItemText::init(int tag, const Color3B &color, GLubyte opacity, const char *text, const char *fontName, float fontSize)
{
    if (RichItem::init(tag, color, opacity))
    {
        _text = text;
        _fontName = fontName;
        _fontSize = fontSize;
        _outcolor = Color4B::WHITE;
        _linkcolor = Color4B::WHITE;
        _outlinesize = 0;
        _linksize = 0;
        return true;
    }
    return false;
}

RichItemImage* RichItemImage::create(int tag, const Color3B &color, GLubyte opacity, const char *filePath)
{
    RichItemImage* element = new RichItemImage();
    if (element && element->init(tag, color, opacity, filePath))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}

bool RichItemImage::init(int tag, const Color3B &color, GLubyte opacity, const char *filePath)
{
    if (RichItem::init(tag, color, opacity))
    {
        _filePath = filePath;
        return true;
    }
    return false;
}

RichItemCustom* RichItemCustom::create(int tag, const Color3B &color, GLubyte opacity, cocos2d::Node *customNode)
{
    RichItemCustom* element = new RichItemCustom();
    if (element && element->init(tag, color, opacity, customNode))
    {
        element->autorelease();
        return element;
    }
    CC_SAFE_DELETE(element);
    return NULL;
}

bool RichItemCustom::init(int tag, const Color3B &color, GLubyte opacity, cocos2d::Node *customNode)
{
    if (RichItem::init(tag, color, opacity))
    {
        _customNode = customNode;
        _customNode->retain();
        return true;
    }
    return false;
}

LinkLable::LinkLable(FontAtlas *atlas, TextHAlignment hAlignment, TextVAlignment vAlignment, bool useDistanceField, bool useA8Shader)
    : Label(atlas, hAlignment, vAlignment, useDistanceField, useA8Shader), _touchListener(NULL),
    _delegate(NULL), _linkcolor(Color4B::WHITE), _linksize(0), _linkline(NULL)
{

}

LinkLable::~LinkLable()
{
    disableLinkLine();	
}

LinkLable* LinkLable::create(const std::string& text, const std::string& fontFile, float fontSize, const Size& dimensions,
                             TextHAlignment hAlignment, TextVAlignment vAlignment)
{
    auto ret = new LinkLable(nullptr, hAlignment, vAlignment);
    if (ret)
    {
        if (FileUtils::getInstance()->isFileExist(fontFile))
        {
            TTFConfig ttfConfig(fontFile.c_str(), fontSize, GlyphCollection::DYNAMIC);
            if (ret->setTTFConfig(ttfConfig))
            {
                ret->setDimensions(dimensions.width, dimensions.height);
                ret->setString(text);
                ret->autorelease();
                return ret;
            }
        }
        else
        {
            ret->setSystemFontName(fontFile.c_str());
            ret->setSystemFontSize(fontSize);
            ret->setDimensions(dimensions.width, dimensions.height);
            ret->setString(text);
            ret->autorelease();
            return ret;
        }
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

void LinkLable::setFontAndSize(const std::string& fontFile, float fontSize, const Size& dimensions)
{
    if (FileUtils::getInstance()->isFileExist(fontFile))
    {
        TTFConfig ttfConfig(fontFile.c_str(), fontSize, GlyphCollection::DYNAMIC);
        if (setTTFConfig(ttfConfig))
        {
            setDimensions(dimensions.width, dimensions.height);
        }
    }
    else
    {
        setSystemFontName(fontFile.c_str());
        setSystemFontSize(fontSize);
        setDimensions(dimensions.width, dimensions.height);
    }
}

void LinkLable::enableLinkLine(const Color4B& linkcolor, GLubyte linksize) 
{
    _linkcolor = linkcolor;
    _linksize = linksize;
    if (_linksize > 0)
    {
        _touchListener = EventListenerTouchOneByOne::create();
        CC_SAFE_RETAIN(_touchListener);
        _touchListener->setSwallowTouches(false);
        _touchListener->onTouchBegan = CC_CALLBACK_2(LinkLable::onTouchBegan, this);
        _touchListener->onTouchEnded = CC_CALLBACK_2(LinkLable::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

        _linkline = LayerColor::create(_linkcolor);
        _linkline->setContentSize(Size(getContentSize().width, _linksize));
    }
}

bool LinkLable::onTouchBegan(Touch *touch, Event *unusedEvent)
{
    if (!isVisible())
    {
        return false;
    }
    Point _touchStartPos = touch->getLocation();
    Point nsp = convertToNodeSpace(_touchStartPos);
    Rect bb;
    bb.size = _contentSize;
    if (bb.containsPoint(nsp))
    {
        return true;
    }
    return false;
}

void LinkLable::onTouchEnded(Touch *touch, Event *unusedEvent)
{
    Point _touchStartPos = touch->getLocation();
    Point nsp = convertToNodeSpace(_touchStartPos);
    Rect bb;
    bb.size = _contentSize;
    if (bb.containsPoint(nsp))
    {
        if (_delegate)
        {
            _delegate->labelClicked(this);
        }
    }
}

void LinkLable::disableLinkLine()
{
    if (_linksize)
    {
        _eventDispatcher->removeEventListener(_touchListener);
        CC_SAFE_RELEASE_NULL(_touchListener);
        if (_linkline)
        {
            _linkline->removeFromParent();
            _linkline = NULL;
        }
        _linksize = 0;
    }
}



