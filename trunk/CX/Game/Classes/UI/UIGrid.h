#pragma once

namespace cc2d
{
    class Vec2;
    class Size;
    namespace ui
    {
        class Widget;
    }
    namespace extension
    {
        class TableView;
        class TableViewCell;
    }
}

class UIGrid
{
    friend class GridDataSource; 
    friend class MyTableView;
public:
    UIGrid(void);
    ~UIGrid(void);
    //>初始化
    bool InitGrid(const cc2d::Size& size, const cc2d::Vec2& position, cc2d::ui::Widget* cellTemplate);
    
    unsigned int GetRowNumber();    //>获得行数
    
    unsigned int GetUnlockNextNumer(); //>获得解锁下一行数
    
    void AddLockedRow(bool reload = false);  //>新增一行
    
    void UnlockNextRow(bool reload = false); //>解锁下一行
    
    void ClearRow();    //>清空行,清空所有锁定和未锁定的.

    void Reload();  //>整个内容重构建. 比如有100行,100行从新计算位置和数据.
    
    void UpdataView();  //>显示的区域从新更新,仅仅是显示的行更新.

    bool IsClippingContainsTouch(cc2d::Touch* touch, cc2d::Node* node);
protected:
    virtual void Reload(unsigned int rowIndex,  cc2d::ui::Widget* cellTemplate) = 0;   //>子类覆盖此方法来设置显示数据. 
private:
    void Reload(cc2d::extension::TableViewCell *cell); //>请不要使用cell的getTag/setTag函数.
protected:
    class MyTableView* _scrollView;
private:
    class GridDataSource* _dataSrc;

    unsigned int _rowNumber;
    unsigned int _unlockNextNumber;
};

