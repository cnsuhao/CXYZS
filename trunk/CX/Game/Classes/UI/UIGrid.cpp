#include "ClientDef.h"

#include "UIWidget.h"

USING_NS_CC;
using namespace cc2dui;

#include "cocos-ext.h"
#include "UIGrid.h"

const int CellTag = 999;

class GridDataSource : public extension::TableViewDataSource
{
public:
    GridDataSource(UIGrid* bag)
    {
        _bag = bag;
        _widget = NULL;
    }
    ~GridDataSource()
    {
        if (_widget)
            _widget->retain();
    }

    virtual Size tableCellSizeForIndex(extension::TableView *table, ssize_t idx) {
        return cellSizeForTable(table);
    };

    virtual Size cellSizeForTable(extension::TableView *table) {
        if (_widget){
            return _widget->getContentSize();
        }
        return Size::ZERO;
    };

    virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx)
    {
        extension::TableViewCell *cell = table->dequeueCell();
        if (!cell && _widget) {
            cell = new (std::nothrow) extension::TableViewCell();
            cell->autorelease();
            Widget* w = _widget->clone();
            w->setTag(CellTag);
            w->setVisible(true);
            cell->addChild(w);
        }

        if (cell)
        {
            // ÉèÖÃÊý¾Ý
            cell->setIdx(idx);
            _bag->Reload(cell);
        }
        return cell;
    }

    virtual ssize_t numberOfCellsInTableView(extension::TableView *table) 
    {
        if (_bag)
        {
            return _bag->GetRowNumber();
        }
        return 0;
    }

    void SetWidget(cc2dui::Widget* w)
    {
        if (w == _widget)
            return;

        if (_widget)
            _widget->release();

        _widget = w;
        _widget->retain();
    }
protected:
    UIGrid* _bag;
    cc2dui::Widget* _widget;
};

class MyTableView : public extension::TableView
{
public:
    static MyTableView* create(extension::TableViewDataSource* dataSource, Size size, UIGrid* bag)
    {
        MyTableView *table = new (std::nothrow) MyTableView();
        table->initWithViewSize(size, nullptr);
        table->autorelease();
        table->setDataSource(dataSource);
        table->_bag = bag;
        table->setName("MyTableView");
        return table;
    }

    void UpdataView()
    {
        if(!_cellsUsed.empty())
        {
            for (Vector<extension::TableViewCell*>::iterator it = _cellsUsed.begin(); it != _cellsUsed.end(); ++it)
            {
                _bag->Reload(*it);
            }
        }
    }
    UIGrid* _bag;
};


UIGrid::UIGrid(void):_scrollView(NULL)
{
    _dataSrc = new GridDataSource(this);

    _rowNumber = 0;
    _unlockNextNumber = 0;

}

UIGrid::~UIGrid(void)
{
    if (_dataSrc)
    {
        delete _dataSrc;
    }
}

bool UIGrid::InitGrid(const cc2d::Size& size, const cc2d::Vec2& position, cc2dui::Widget* cellTemplate)
{
    CCASSERT(_scrollView == NULL, "");
    _scrollView = MyTableView::create(_dataSrc, size, this);
    if (_scrollView)
    {
        _scrollView->setPosition(position);
        _scrollView->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
        _scrollView->setDirection(extension::ScrollView::Direction::VERTICAL);
        if (cellTemplate)
        {
            if (cellTemplate->getParent())
                cellTemplate->removeFromParentAndCleanup(false);

            cellTemplate->setPosition(Vec2::ZERO);
            _dataSrc->SetWidget(cellTemplate);
        }
        else
        {
            CCASSERT(0,"");
        }
        return true;
    }
    return false;
}

unsigned int UIGrid::GetRowNumber()
{
    return _rowNumber;
}

unsigned int UIGrid::GetUnlockNextNumer()
{
    return _unlockNextNumber;
}

void UIGrid::AddLockedRow(bool reload /*= false*/)
{
    if (_scrollView)
    {
        ++_rowNumber;
        if (reload)
        {
            Reload();
        }
    }
}

void UIGrid::UnlockNextRow(bool reload /*= false*/)
{
    if (_unlockNextNumber >= _rowNumber)
    {
        return;
    }
    if(_scrollView)
    {
        ++_unlockNextNumber;
        if (reload)
        {
            Reload();
        }
    }
}

void UIGrid::ClearRow()
{
    _rowNumber = 0;
    _unlockNextNumber = 0;

    Reload();
}

void UIGrid::Reload()
{
    if (_scrollView)
    {
        _scrollView->reloadData();
    }
}

void UIGrid::UpdataView()
{
    if (_scrollView)
    {
        _scrollView->UpdataView();
    }
}

bool UIGrid::IsClippingContainsTouch(Touch* pTouch, Node* node)
{
    Node* parent = node->getParent();
    while (parent)
    {
        if(parent->getName() == "MyTableView")
        {
            Rect bb = parent->getBoundingBox();
            bb.origin = parent->getParent()->convertToWorldSpace(bb.origin);
        
            if (bb.containsPoint(pTouch->getLocation()))
            {
                return true;
            }
        }
        parent = parent->getParent();
    }
    return false;
}

void UIGrid::Reload(cc2d::extension::TableViewCell *cell)
{
    if (cell)
    {
        Widget* pW = static_cast<Widget*>(cell->getChildByTag(CellTag));
        if (pW && _dataSrc)
        {
            Reload(cell->getIdx(),pW);
        }
    }
}

