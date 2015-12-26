#pragma once

#include "EditClass.h"

class SelectHelper
{
public:
    SelectHelper(void);
    ~SelectHelper(void);

    bool Init(cc2d::Node* scene);

    void SetSelectNode(EditClass* selectNode);
    inline EditClass* GetSelectNode() { return selectNode; }

    inline void SetTouchNode(EditClass* touchNode_) { touchNode = touchNode_; };
    inline EditClass* GetTouchNode() { return touchNode; }

    void NodeMoveUpdate();
    void HideSelect();
private:
    cc2d::DrawNode* drawNode;
    EditClass* selectNode;
    EditClass* touchNode;
};

extern SelectHelper gSelectHepler;

