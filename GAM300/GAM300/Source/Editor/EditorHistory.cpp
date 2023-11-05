/*!***************************************************************************************
\file			EditorHistory.cpp
\project
\author         Joseph Ho

\par			Course: GAM300
\date           07/10/2023

\brief
    This file contains the definitions of functions in the History system that handles the
    undo and redo functionality in the editor. 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "EditorHistory.h"
#include "Scene/SceneManager.h"

//Check if the undo stack is empty
bool HistoryManager::UndoStackEmpty() {
    return UndoStack.empty();
}

//Check if the redo stack is empty
bool HistoryManager::RedoStackEmpty() {
    return RedoStack.empty();
}

//Function to undo reference fields from scripts
template <typename T, typename... Ts>
void ReferenceSetBackHelper(Field& rhs, Object*& data)
{
    if (rhs.fType == GetFieldType::E<T>())
    {
        if constexpr (AllObjectTypes::Has<T>())
        {
            Scene& scene{ MySceneManager.GetCurrentScene() };
            T*& object = *reinterpret_cast<T**>(rhs.data);
            object = reinterpret_cast<T*&>(data);
        }
    }

    if constexpr (sizeof...(Ts) != 0)
    {
        ReferenceSetBackHelper<Ts...>(rhs, data);
    }
}

//Main function thats calls the helper function to undo reference fields from scripts
template <typename T, typename... Ts>
void ReferenceSetBack(Field& rhs, Object* & data, TemplatePack<T, Ts...>)
{
    ReferenceSetBackHelper<T, Ts...>(rhs, data);
}

template <typename T, typename... Ts>
void ScriptSetbackHelper(Field& rhs, property::data data)
{
    using DataVariant = decltype(data);
    using DataPack = decltype(TemplatePack(DataVariant()));
    if constexpr (DataPack::Has<T>())
    {
        if (rhs.fType == GetFieldType::E<T>())
        {

            if constexpr (std::is_same_v<char*, T>)
            {
                std::string value = std::get<std::string>(data);
                strcpy((char*)rhs.data, value.data());
            }
            else
            {
                // Store Basic Types
                rhs.Get<T>() = std::get<T>(data);
            }
            return;
        }
    }

    if constexpr (sizeof...(Ts) != 0)
    {
        ScriptSetbackHelper<Ts...>(rhs, data);
    }
}

template <typename T, typename... Ts>
void ScriptSetback(Field& rhs, property::data& data, TemplatePack<T, Ts...>)
{
    ScriptSetbackHelper<T, Ts...>(rhs, data);
}

//undo and redo logic for script fields
void HistoryManager::SetScriptField(Change& change, ChangeType type) {

    std::string fieldName = change.property;

    //remove Script/ for fieldname
    if (fieldName.find_last_of("/") != std::string::npos) {
        auto it = fieldName.begin() + fieldName.find_last_of("/") + 1;
        fieldName.erase(fieldName.begin(), it);
    }

    char buffer[2048];
    Field field{ AllFieldTypes::Size(),2048,buffer };
    Script* script = reinterpret_cast<Script*>(change.component);
    ScriptGetFieldEvent getFieldEvent{ *script, fieldName.c_str(), field };
    EVENTS.Publish(&getFieldEvent);

    if(type == UNDO)
        ScriptSetback(field, change.previousValue, AllFieldTypes());
    else
        ScriptSetback(field, change.newValue, AllFieldTypes());

    ScriptSetFieldEvent setFieldEvent{ *script , fieldName.c_str(), field };
    EVENTS.Publish(&setFieldEvent);
}

//change a reference in a script
void HistoryManager::SetScriptReference(Change& change, ChangeType type) {

    //set previous reference back to field
    char buffer[2048];
    Field field{ AllFieldTypes::Size(),2048, buffer };
    std::string fieldName = change.property;
    Script* script = reinterpret_cast<Script*>(change.component);

    //Get script reference
    ScriptGetFieldEvent getFieldEvent{ *script, fieldName.c_str(), field };
    EVENTS.Publish(&getFieldEvent);

    //Set reference back
    if (type == UNDO)
        ReferenceSetBack(field, change.oldreference, AllObjectTypes());
    else //REDO
        ReferenceSetBack(field, change.newreference, AllObjectTypes());

    ScriptSetFieldEvent setFieldEvent{ *script , fieldName.c_str(), field };
    EVENTS.Publish(&setFieldEvent);
}

//undo a change for a field or action in the editor
bool HistoryManager::UndoChange() {
    //get most recent move
    if (!UndoStack.empty()) {
        Change undochange = UndoStack.top();
        UndoStack.pop();

        //Set script variables
        if (undochange.property.find("Script/") != std::string::npos) {
            SetScriptField(undochange, UNDO);
        }   
        //Set script references
        else if(undochange.isreference) {
            SetScriptReference(undochange, UNDO);
        }
        else //Set normal editor variables
            property::set(*(undochange.component), undochange.property.c_str(), undochange.previousValue);
        
        RedoStack.push(undochange);
        return true;
    }
    return false;
}

//redo an undo move for a field or action in the editor
bool HistoryManager::RedoChange() {
    //get most recent move
    if (!RedoStack.empty()) {
        Change redochange = RedoStack.top();
        RedoStack.pop();

        //Set script variables
        if (redochange.property.find("Script/") != std::string::npos) {
            SetScriptField(redochange, REDO);
        }
        //Set script references
        else if (redochange.isreference) {
            SetScriptReference(redochange, REDO);
        }
        else //Set normal editor variables
            property::set(*(redochange.component), redochange.property.c_str(), redochange.newValue);
   
        UndoStack.push(redochange);
        return true;
    }
    return false;
}

//Clear the redo stack
void HistoryManager::ClearRedoStack() {
    while (!RedoStack.empty()) {
        RedoStack.pop();
    }
}

//add a change in reference action to the undo stack
void HistoryManager::AddReferenceChange(Change& change, Component oldRef, Component newRef) {
    change.oldreference = oldRef;
    change.newreference = newRef;
    change.isreference = true;
    UndoStack.push(change);
}
