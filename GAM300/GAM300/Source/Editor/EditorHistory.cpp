#include "Precompiled.h"
#include "EditorHistory.h"

bool HistoryManager::UndoStackEmpty() {
    return UndoStack.empty();
}

bool HistoryManager::RedoStackEmpty() {
    return RedoStack.empty();
}

bool HistoryManager::UndoChange() {
    //get most recent move
    if (!UndoStack.empty()) {
        Change undochange = UndoStack.top();
        UndoStack.pop();
        property::set(*(undochange.component), undochange.property.c_str(), undochange.previousValue);
        RedoStack.push(undochange);
        return true;
    }
    return false;
}

bool HistoryManager::RedoChange() {
    //get most recent move
    if (!RedoStack.empty()) {
        Change redochange = RedoStack.top();
        RedoStack.pop();
        property::set(*(redochange.component), redochange.property.c_str(), redochange.newValue);
        UndoStack.push(redochange);
        return true;
    }
    return false;
}

void HistoryManager::ClearRedoStack() {
    while (!RedoStack.empty()) {
        RedoStack.pop();
    }
}
