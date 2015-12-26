#include "CtrlRichText.h"

USING_NS_CC;
using namespace cc2dui;

#include "CCDirector.h"
#include "CCTextureCache.h"
#include "ChatSystem.h"
#include "UIManager.h"
#include "UIChat.h"
#include "UIHelper.h"
#include "deprecated/CCString.h"
#include "ccUTF8.h"


class RichTextTableCell : public extension::TableViewCell
{
public:
    ~RichTextTableCell()
    {
        for (size_t ti = 0; ti < texts.size(); ++ti)
        {
            texts[ti]->release();
        }
        for (size_t i = 0; i < images.size(); ++i)
        {
            images[i]->release();
        }
    }

    std::vector<LinkLable*> texts;
    std::vector<Sprite*> images;

    void Reset()
    {
        textCount = 0;
        imageCount = 0;
        height = 0;

        for (size_t ti = 0; ti < texts.size(); ++ti)
        {
            texts[ti]->removeFromParentAndCleanup(false);
            texts[ti]->disableLinkLine();
        }
        for (size_t i = 0; i < images.size(); ++i)
        {
            images[i]->removeFromParentAndCleanup(false);
        }
    }

    LinkLable* GetText(const string& text, const string& fontName, unsigned int fontSize)
    {
        if (textCount < texts.size())
        {
            texts[textCount]->setString(text);
            texts[textCount]->setFontAndSize(fontName, fontSize);
            return texts[textCount++];
        }
        
        texts.push_back(LinkLable::create(text, fontName, fontSize));
        texts[textCount]->retain();
        return texts[textCount++];
    }

    Sprite* GetImage(const string& imagePath)
    {
        if (textCount < images.size())
        {
            images[imageCount]->setTexture(imagePath);
            return images[imageCount++];
        }

        images.push_back(Sprite::create(imagePath));
        images[imageCount]->retain();
        return images[imageCount++];
    }

    size_t textCount;
    size_t imageCount;
    float height;
};


class RichTextDataSource : public extension::TableViewDataSource
{
public:
    RichTextDataSource(CtrlRichText*  richText)
    {
        _richText = richText;
        _lable = LinkLable::create("", "", 10);
        _lable->retain();
    }
    ~RichTextDataSource()
    {
        if (_lable)
        {
            _lable->release();
        }
    }

    ChatSystem::RichItemSet& GetCellIndexRichItemSet(ssize_t idx)
    {
        _itemSet.clear();
        UIChat* pUIChat = static_cast<UIChat*>(g_UIManager->GetUI(UI_CHAT));
        if (pUIChat)
        {
            const ChatMessage* msg = g_ChatSystem.GetMsgOfIndex((int)pUIChat->GetSeletedChannel(), idx);
            if (msg)
            {
                g_ChatSystem.AppendixChannelAndUser(msg, _itemSet);
                g_ChatSystem.Parse(msg, _itemSet);
            }
        }
        return _itemSet;
    }

    void HeightOfTextItem(const string& text, const string& fontName, float fontSize)
    {
        _lable->setFontAndSize(fontName, fontSize);
        _lable->setString(text);
        float textWidth = _lable->getContentSize().width;
        _leftSpaceWidth -= textWidth;
        if (_leftSpaceWidth < 0.0f)
        {
            float overstepPercent = (-_leftSpaceWidth) / textWidth;
            size_t stringLength = StringUtils::getCharacterCountInUTF8String(text);
            int leftLength = stringLength * (1.0f - overstepPercent);
            std::string leftWords = ui::Helper::getSubStringOfUTF8String(text,0,leftLength);
            std::string cutWords = ui::Helper::getSubStringOfUTF8String(text, leftLength, stringLength - leftLength);
            _lable->setString(ui::Helper::getSubStringOfUTF8String(leftWords, 0, leftLength).c_str());
            AddHight(_lable->getContentSize().height);
            NewLine();
            HeightOfTextItem(cutWords, fontName, fontSize);
        }
        else
        {
            AddHight(_lable->getContentSize().height);
        }
    }

    void HeightOfImageItem(const string& filePath)
    {
        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filePath);
        if (texture)
        {
            Size size = texture->getContentSize();
            _leftSpaceWidth -= size.width;
            if (_leftSpaceWidth < 0.0f)
            {
                NewLine();
                AddHight(size.width);
                _leftSpaceWidth -= size.width;
            }
            else
            {
                AddHight(size.width);
            }
        }
    }

    virtual Size tableCellSizeForIndex(extension::TableView *table, ssize_t idx) {
        float height = 0.0f;
        std::map<ssize_t, float>::iterator it = _cacheHeights.find(idx);
        if (it != _cacheHeights.end())
        {
            height = it->second;
        }
        else
        {
            ChatSystem::RichItemSet& itemSet = GetCellIndexRichItemSet(idx);
            NewLine();
            for (size_t i = 0; i < itemSet.size(); i++)
            {
                RichItem* element = static_cast<RichItem*>(itemSet.at(i));
                switch (element->GetType())
                {
                case RICH_ITEM_NEWLINE:
                    {
                        NewLine();
                    }
                    break;
                case RICH_ITEM_TEXT:
                    {
                        RichItemText* elmtText = static_cast<RichItemText*>(element);
                        HeightOfTextItem(elmtText->_text, elmtText->_fontName, elmtText->_fontSize);
                    }
                    break;
                case RICH_ITEM_IMAGE:
                    {
                        RichItemImage* elmtImage = static_cast<RichItemImage*>(element);	
                        HeightOfImageItem(elmtImage->_filePath);
                    }
                    break;
                case RICH_ITEM_CUSTOM:
                    {
                    }
                    break;
                default:
                    break;
                }
            }

            float *maxHeights = new float[_linesHeights.size()];
            for (size_t i = 0; i < _linesHeights.size(); i++)
            {
                std::vector<float>* row = (_linesHeights[i]);
                float maxHeight = 0.0f;
                for (size_t j = 0; j < row->size(); j++)
                {
                    float height = row->at(j);
                    maxHeight = MAX(height, maxHeight);
                }
                maxHeights[i] = maxHeight + _richText->GetLineSpace();
                height += maxHeights[i];
            }

            delete [] maxHeights;

            size_t length = _linesHeights.size();
            for (size_t i = 0; i<length; i++)
            {
                std::vector<float>* l = _linesHeights[i];
                l->clear();
                delete l;
            }
            _linesHeights.clear();
            _cacheHeights[idx] = height;
        }
        return Size(_richText->getContentSize().width, height);
    };

    virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx)
    {
        RichTextTableCell *cell = static_cast<RichTextTableCell*>(table->dequeueCell());
        if (!cell) {
            cell = new (std::nothrow) RichTextTableCell();
            cell->autorelease();
        }
        
        if (cell)
        {
            // ÉèÖÃÊý¾Ý
            cell->setIdx(idx);
            cell->Reset();
            ChatSystem::RichItemSet& itemSet = GetCellIndexRichItemSet(idx);
            _richText->Format(cell, itemSet);
        }
        
        return cell;
    }

    virtual ssize_t numberOfCellsInTableView(extension::TableView *table) 
    {
        UIChat* pUIChat = static_cast<UIChat*>(g_UIManager->GetUI(UI_CHAT));
        if (pUIChat)
        {
            return g_ChatSystem.GetMsgCount((int)pUIChat->GetSeletedChannel());
        }
        return 0;
    }

    void NewLine()
    {
        _leftSpaceWidth = _richText->getViewSize().width;
        _linesHeights.push_back(new std::vector<float>());
    }

    void AddHight(float height)
    {
        if (_linesHeights.size() <= 0)
        {
            return;
        }
        _linesHeights[_linesHeights.size()-1]->push_back(height);
    }

    void ClearCacheHeight()
    {
        _cacheHeights.clear();
    }
protected:
    CtrlRichText*  _richText;
    ChatSystem::RichItemSet _itemSet;
    float _leftSpaceWidth;
    std::vector<std::vector<float>*> _linesHeights;
    std::map<ssize_t, float> _cacheHeights;
    LinkLable* _lable;
};



CtrlRichText::CtrlRichText()
{
    _dataSource = new (std::nothrow) RichTextDataSource(this);
    _lineSpace = 6;
    _leftSpaceWidth = 0;
}
    
CtrlRichText::~CtrlRichText()
{
    if (_dataSource)
    {
        delete _dataSource;
    }
}

CtrlRichText* CtrlRichText::create(const cc2d::Size& size, const cc2d::Vec2& position)
{
    CtrlRichText *richText = new (std::nothrow) CtrlRichText();
    richText->initWithViewSize(size, NULL);
    richText->setPosition(position);
    richText->autorelease();
    richText->setDataSource(richText->_dataSource);
    richText->_updateCellPositions();
    richText->_updateContentSize();
    return richText;
}

void CtrlRichText::Format(extension::TableViewCell* cell, std::vector<class RichItem*>& RichItemSet)
{
    ChangeLine();
    for (size_t i = 0; i < RichItemSet.size(); i++)
    {
        RichItem* element = static_cast<RichItem*>(RichItemSet.at(i));
        switch (element->GetType())
        {
        case RICH_ITEM_NEWLINE:
            {
                ChangeLine();			
            }
            break;
        case RICH_ITEM_TEXT:
            {
                RichItemText* elmtText = static_cast<RichItemText*>(element);
                FormatText(cell, elmtText, elmtText->_text.c_str(), elmtText->_fontName.c_str(), elmtText->_fontSize, elmtText->_color, elmtText->_opacity);						
            }
            break;
        case RICH_ITEM_IMAGE:
            {
                RichItemImage* elmtImage = static_cast<RichItemImage*>(element);
                FormatImage(cell, elmtImage->_filePath, elmtImage->_color, elmtImage->_opacity);					
            }
            break;
        case RICH_ITEM_CUSTOM:
            {
            }
            break;
        default:
            break;
        }
    }
    FormarRenderers(cell);

}

void CtrlRichText::FormatText(extension::TableViewCell* cell, RichItemText* item, const string& text, const string& fontName, float fontSize, const cc2d::Color3B& color, GLubyte opacity)
{
    LinkLable* textRenderer = CreateLable(cell, item, text);
    if (textRenderer)
    {
        textRenderer->setColor(color);
        textRenderer->setOpacity(opacity);
        float textWidth = textRenderer->getContentSize().width;
        _leftSpaceWidth -= textWidth;
        if (_leftSpaceWidth < 0.0f)
        {
            float overstepPercent = (-_leftSpaceWidth) / textWidth;
            size_t stringLength = StringUtils::getCharacterCountInUTF8String(text);
            int leftLength = stringLength * (1.0f - overstepPercent);
            std::string leftWords = ui::Helper::getSubStringOfUTF8String(text,0,leftLength);
            std::string cutWords = ui::Helper::getSubStringOfUTF8String(text, leftLength, stringLength - leftLength);
            if (leftLength > 0)
            {
                textRenderer->setString(ui::Helper::getSubStringOfUTF8String(leftWords, 0, leftLength).c_str());
                PushToContainer(textRenderer);
            }
            else
            {
                CCASSERT(0,"");
            }
            ChangeLine();
            FormatText(cell, item, cutWords.c_str(), fontName, fontSize, color, opacity);
        }
        else
        {
            PushToContainer(textRenderer);
        }   
    }
}

void CtrlRichText::FormatImage(extension::TableViewCell* cell, const string& fileParh, const cc2d::Color3B& color, GLubyte opacity)
{
    RichTextTableCell* tableCell = static_cast<RichTextTableCell*>(cell);
    if (tableCell)
    {
        Sprite* renderer = tableCell->GetImage(fileParh);
        if (renderer)
        {
            Size imgSize = renderer->getContentSize();
            _leftSpaceWidth -= imgSize.width;
            if (_leftSpaceWidth < 0.0f)
            {
                ChangeLine();
                PushToContainer(renderer);
                _leftSpaceWidth -= imgSize.width;
            }
            else
            {
                PushToContainer(renderer);
            }
        }
    }
}

LinkLable* CtrlRichText::CreateLable(extension::TableViewCell* cell, RichItemText* item, const string& text)
{
    RichTextTableCell* tableCell = static_cast<RichTextTableCell*>(cell);
    LinkLable* textRenderer = NULL;
    if (tableCell)
    {
        textRenderer = tableCell->GetText(text, item->_fontName, item->_fontSize);
        if (textRenderer)
        {
            if (item->_outlinesize > 0)
            {
                textRenderer->enableOutline(item->_outcolor, item->_outlinesize);			
            }	
            if (item->_linksize > 0)
            {
                textRenderer->enableLinkLine(item->_linkcolor, item->_linksize);			
            }	
        }
    }
    return textRenderer;
}

void CtrlRichText::PushToContainer(Node* renderer)
{
    if (_elementRenders.size() <= 0)
    {
        return;
    }
    _elementRenders[_elementRenders.size()-1]->pushBack(renderer);
}

void CtrlRichText::ChangeLine()
{
    _leftSpaceWidth = getViewSize().width;
    _elementRenders.push_back(new Vector<Node*>());
}

void CtrlRichText::FormarRenderers(extension::TableViewCell* cell)
{
    float newContentSizeHeight = 0.0f;
    float *maxHeights = new float[_elementRenders.size()];

    for (size_t i = 0; i < _elementRenders.size(); i++)
    {
        Vector<Node*>* row = (_elementRenders[i]);
        float maxHeight = 0.0f;
        for (ssize_t j = 0; j < row->size(); j++)
        {
            Node* l = row->at(j);
            maxHeight = MAX(l->getContentSize().height, maxHeight);
        }
        maxHeights[i] = maxHeight + _lineSpace;
        newContentSizeHeight += maxHeights[i];
    }

    float nextPosY = newContentSizeHeight;
    static_cast<RichTextTableCell*>(cell)->height = newContentSizeHeight;
    for (size_t i = 0; i < _elementRenders.size(); i++)
    {
        Vector<Node*>* row = (_elementRenders[i]);
        float nextPosX = 0.0f;
        nextPosY -= maxHeights[i];

        for (ssize_t j=0; j < row->size(); j++)
        {
            Node* l = row->at(j);
            l->setAnchorPoint(Point::ZERO);
            l->setPosition(Point(nextPosX, nextPosY));
            cell->addChild(l, 1, (int)(i * 10 + j));
            LinkLable* la = dynamic_cast<LinkLable*>(l);
            if (la && la->getLinkline())
            {
                la->getLinkline()->setPosition(nextPosX, nextPosY - 1);
                cell->addChild(la->getLinkline(), 1, (int)(i * 10 + j));	
            }				
            nextPosX += l->getContentSize().width;
        }
    }
    delete [] maxHeights;

    size_t length = _elementRenders.size();
    for (size_t i = 0; i<length; i++)
    {
        Vector<Node*>* l = _elementRenders[i];
        l->clear();
        delete l;
    }
    _elementRenders.clear();
}

void CtrlRichText::ReloadData()
{
    for(const auto &cell : _cellsUsed) {
        if(_tableViewDelegate != nullptr) {
            _tableViewDelegate->tableCellWillRecycle(this, cell);
        }

        _cellsFreed.pushBack(cell);

        cell->reset();
        if (cell->getParent() == this->getContainer())
        {
            this->getContainer()->removeChild(cell, true);
        }
    }

    _indices->clear();
    _cellsUsed.clear();

    this->_updateCellPositions();
    this->_updateContentSize();
}

