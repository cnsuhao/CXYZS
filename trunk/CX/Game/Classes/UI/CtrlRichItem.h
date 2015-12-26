#ifndef __RICHITEM_H__
#define __RICHITEM_H__

#include "ClientDef.h"
#include "UIWidget.h"
#include "CCLabel.h"
#include "CCLayer.h"
#include "CCFontAtlas.h"


typedef enum 
{
    RICH_ITEM_TEXT,
    RICH_ITEM_NEWLINE,
    RICH_ITEM_IMAGE,
    RICH_ITEM_CUSTOM
}
RichItemType;

class RichItem : public cc2d::Ref
{
public:
    RichItem(){};
    virtual ~RichItem(){};
    bool init(int tag, const cc2d::Color3B& color, GLubyte opacity);

    std::string getContext() {return context;}
    void setContext(std::string cxt) {context = cxt;}
    RichItemType GetType() { return _type; }

    RichItemType _type;
    int _tag;
    cc2d::Color3B _color;
    GLubyte _opacity;
    std::string context;
};

class RichItemNewLine : public RichItem
{
public:
    RichItemNewLine(){_type = RICH_ITEM_NEWLINE;};
    virtual ~RichItemNewLine(){};
    bool init(int tag);
    static RichItemNewLine* create(int tag);
};

class RichItemText : public RichItem
{
public:
    RichItemText(){_type = RICH_ITEM_TEXT;};
    virtual ~RichItemText(){};
    bool init(int tag, const cc2d::Color3B& color, GLubyte opacity, const char* text, const char* fontName, float fontSize);
    static RichItemText* create(int tag, const cc2d::Color3B& color, GLubyte opacity, const char* text, const char* fontName, float fontSize);
    static RichItemText* create(int tag, const cc2d::Color3B& color, const char* text);
    static RichItemText* create(int tag, const cc2d::Color3B& color, const char* text, const cc2d::Color4B& linkcolor);

    void enableOutLine(const cc2d::Color4B& outcolor, GLubyte outlinesize) 
    {
        _outcolor = outcolor;
        _outlinesize = outlinesize;
    }
    void enableLinkLine(const cc2d::Color4B& linkcolor, GLubyte linksize) 
    {
        _linkcolor = linkcolor;
        _linksize = linksize;
    }

    std::string _text;
    std::string _fontName;
    float _fontSize;
    cc2d::Color4B _outcolor;
    cc2d::Color4B _linkcolor;
    GLubyte _linksize;
    GLubyte _outlinesize;
};

class RichItemImage : public RichItem
{
public:
    RichItemImage(){_type = RICH_ITEM_IMAGE;};
    virtual ~RichItemImage(){};
    bool init(int tag, const cc2d::Color3B& color, GLubyte opacity, const char* filePath);
    static RichItemImage* create(int tag, const cc2d::Color3B& color, GLubyte opacity, const char* filePath);

    std::string _filePath;
    cc2d::Rect _textureRect;
    int _textureType;

};

class RichItemCustom : public RichItem
{
public:
    RichItemCustom(){_type = RICH_ITEM_CUSTOM;};
    virtual ~RichItemCustom(){CC_SAFE_RELEASE(_customNode);};
    bool init(int tag, const cc2d::Color3B& color, GLubyte opacity, cc2d::Node* customNode);
    static RichItemCustom* create(int tag, const cc2d:: Color3B& color, GLubyte opacity, cc2d::Node* customNode);

    cc2d::Node* _customNode;
};

class LinkLable;
class LableDelegate
{
public:
    virtual void labelClicked(LinkLable* lab) = 0;
};

class LinkLable : public cc2d::Label
{
public:
    LinkLable(cc2d::FontAtlas *atlas = nullptr, cc2d::TextHAlignment hAlignment = cc2d::TextHAlignment::LEFT, 
        cc2d::TextVAlignment vAlignment = cc2d::TextVAlignment::TOP, bool useDistanceField = false, bool useA8Shader = false);	

    virtual ~LinkLable();

    static LinkLable* create(const std::string& text, const std::string& fontFile, float fontSize, const cc2d::Size& dimensions = cc2d::Size::ZERO,
        cc2d::TextHAlignment hAlignment = cc2d::TextHAlignment::LEFT, cc2d::TextVAlignment vAlignment = cc2d::TextVAlignment::TOP);


    void setFontAndSize(const std::string& fontFile, float fontSize, const cc2d::Size& dimensions = cc2d::Size::ZERO);
    void setLableDelegate(LableDelegate* ld) { _delegate = ld; }
    void enableLinkLine(const cc2d::Color4B& linkcolor, GLubyte linksize) ;
    void disableLinkLine();
    bool onTouchBegan(cc2d::Touch *touch, cc2d::Event *unusedEvent);
    void onTouchEnded(cc2d::Touch *touch, cc2d::Event *unusedEvent);

    cc2d::LayerColor* getLinkline() { return _linkline;}

    cc2d::EventListenerTouchOneByOne* _touchListener;
    cc2d::LayerColor* _linkline;
    LableDelegate* _delegate;
    cc2d::Color4B _linkcolor;
    GLubyte _linksize;
};


#endif /* defined(__RichItem__) */

