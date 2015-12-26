#include "stdafx.h"
#include "cocos2d.h"
#include "RedoAndUndoMng.h"


RedoAndUndoMng::RedoAndUndoMng(void)
{
}


RedoAndUndoMng::~RedoAndUndoMng(void)
{
}

void RedoAndUndoMng::Initization()
{

}

void RedoAndUndoMng::Shutdown()
{
    Clear();
}

void RedoAndUndoMng::Clear()
{
    size_t size = _undoList.size(); 
    for (size_t i = 0; i < size; ++i)
    {
        delete _undoList[i];
    }
    _undoList.clear();

    size = _redoList.size();
    for (size_t i = 0; i < size; ++i)
    {
        delete _redoList[i];
    }
    _redoList.clear();
}

EditorMove& RedoAndUndoMng::NewMoveAction()
{
    int oldsize = _undoList.size();
    _undoList.resize(oldsize + 1);
    EditorMove* action = new EditorMove();
    _undoList[oldsize] = action;
    return *action;
}

void RedoAndUndoMng::Undo()
{
    if (!_undoList.empty())
    {
        EditorAction* action = _undoList[_undoList.size() - 1];
        if (action)
        {
            _undoList.pop_back();
            action->Undo();

            _redoList.push_back(action);
        }
    }
}

void RedoAndUndoMng::Redo()
{
    if (!_redoList.empty())
    {
        EditorAction* action = _redoList[_redoList.size() - 1];
        if (action)
        {
            _redoList.pop_back();
            action->Redo();

            _undoList.push_back(action);
        }
    }
}

void EditorMove::Undo()
{
    if (eNode.Get())
    {
        eNode.Get()->GetNode()->setPosition(oldX, oldY);
    }
}

void EditorMove::Redo()
{
    if (eNode.Get())
    {
        eNode.Get()->GetNode()->setPosition(newX, newY);
    }
}
