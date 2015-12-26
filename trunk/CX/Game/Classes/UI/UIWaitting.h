#pragma once

#include "UIBase.h"
#include "GameDef.h"


class UIWaitting : public UIBase
{
public:
    UIWaitting(void);
    ~UIWaitting(void);
    virtual bool init();
    CREATE_FUNC(UIWaitting);

    virtual void OnShow();      //>��ʾ�ص�,�������ؼǵõ��ø��෽��

    void SetTextID(uint32 id);
    void SetText(std::string& content);
private:
    Node*   m_Panel;
    cc2dui::Text*   m_Text;
};

