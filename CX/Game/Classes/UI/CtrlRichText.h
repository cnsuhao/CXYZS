#ifndef __CTRLRICHTEXTEX_H__
#define __CTRLRICHTEXTEX_H__

#include "cocos-ext.h"
#include "CtrlRichItem.h"

class CtrlRichText : public cocos2d::extension::TableView
{
    CtrlRichText();
public:
	virtual ~CtrlRichText();
	static CtrlRichText* create(const cc2d::Size& size, const cc2d::Vec2& position);

    void Format(cocos2d::extension::TableViewCell* cell, std::vector<class RichItem*>& RichItemSet);
    void SetLineSpace(unsigned int lineSpace) { _lineSpace = (float)lineSpace; }
    unsigned int GetLineSpace() { return (unsigned int)_lineSpace; }
    void ReloadData();
protected:
    void FormatText(cocos2d::extension::TableViewCell* cell, RichItemText* item, const string& text, const string& fontName, float fontSize, const cc2d::Color3B& color, GLubyte opacity);
    void FormatImage(cocos2d::extension::TableViewCell* cell, const string& fileParh, const cc2d::Color3B& color, GLubyte opacity);
    void FormarRenderers(cocos2d::extension::TableViewCell* cell);
    LinkLable* CreateLable(cocos2d::extension::TableViewCell* cell, RichItemText* item, const string& text);
   
    void ChangeLine();
    void PushToContainer(Node* renderer);

private:
    short _lineSpace;
    float _leftSpaceWidth;
    std::vector<cc2d::Vector<Node*>*> _elementRenders;
};


#endif

