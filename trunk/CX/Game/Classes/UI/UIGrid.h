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
    //>��ʼ��
    bool InitGrid(const cc2d::Size& size, const cc2d::Vec2& position, cc2d::ui::Widget* cellTemplate);
    
    unsigned int GetRowNumber();    //>�������
    
    unsigned int GetUnlockNextNumer(); //>��ý�����һ����
    
    void AddLockedRow(bool reload = false);  //>����һ��
    
    void UnlockNextRow(bool reload = false); //>������һ��
    
    void ClearRow();    //>�����,�������������δ������.

    void Reload();  //>���������ع���. ������100��,100�д��¼���λ�ú�����.
    
    void UpdataView();  //>��ʾ��������¸���,��������ʾ���и���.

    bool IsClippingContainsTouch(cc2d::Touch* touch, cc2d::Node* node);
protected:
    virtual void Reload(unsigned int rowIndex,  cc2d::ui::Widget* cellTemplate) = 0;   //>���า�Ǵ˷�����������ʾ����. 
private:
    void Reload(cc2d::extension::TableViewCell *cell); //>�벻Ҫʹ��cell��getTag/setTag����.
protected:
    class MyTableView* _scrollView;
private:
    class GridDataSource* _dataSrc;

    unsigned int _rowNumber;
    unsigned int _unlockNextNumber;
};

