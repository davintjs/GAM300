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

        if (undochange.property.find("Script/") != std::string::npos) {
            std::string fieldName = undochange.property;

            //remove Script/ for fieldname
            if (fieldName.find_last_of("/") != std::string::npos) {
                auto it = fieldName.begin() + fieldName.find_last_of("/") + 1;
                fieldName.erase(fieldName.begin(), it);
                std::cout << "undoing " << fieldName << " from the undo stack.\n";
            }
            //to convert back to field and 
            /*Field field{ AllFieldTypes::Size(), 2048, undochange.previousValue };
            Script& script = reinterpret_cast<Script&>(undochange.component);
            ScriptSetFieldEvent setFieldEvent{ script ,fieldName.c_str(), field};
            EVENTS.Publish(&setFieldEvent);*/
        }
        else
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

        if (redochange.property.find("Script/") != std::string::npos) {
            std::string fieldName = redochange.property;

            //remove Script/ for fieldname
            if (fieldName.find_last_of("/") != std::string::npos) {
                auto it = fieldName.begin() + fieldName.find_last_of("/") + 1;
                fieldName.erase(fieldName.begin(), it);
                std::cout << "redoing " << fieldName << " from the undo stack.\n";
            }
        }
        else
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
