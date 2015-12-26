#pragma once

class NodeFactory
{
public:
    static EditRoot* CreateRoot();
    static EditNode*  CreateNode();
    static EditSprite* CreateSprite();

    static void DeleateEditClass(EditClass* editClass);
};