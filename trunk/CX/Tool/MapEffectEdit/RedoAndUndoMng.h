#pragma once

#include "EditClass.h"

class EditorAction
{
public:
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

class EditorMove : public EditorAction
{
    float oldX;
    float oldY;
    float newX;
    float newY;
    Handler<EditClass> eNode;
public:
    void Undo();

    void Redo();

    void SetOld(float x, float y) { oldX = x; oldY = y; }
    void SetNew(float x, float y) { newX = x; newY = y; }
    void SetNode(Handler<EditClass> hNode) { eNode = hNode; }
};


class RedoAndUndoMng
{
    RedoAndUndoMng(void);
public:
    
    ~RedoAndUndoMng(void);

    static RedoAndUndoMng& Instance()
    {
        static RedoAndUndoMng mng;
        return mng;
    }

    void Initization();

    void Shutdown();

    void Clear();

    EditorMove& NewMoveAction();

    void Undo();

    void Redo();
protected:
    std::vector<EditorAction*> _undoList;
    std::vector<EditorAction*> _redoList;
};

