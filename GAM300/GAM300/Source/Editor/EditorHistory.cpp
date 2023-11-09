/*!***************************************************************************************
\file			EditorHistory.cpp
\project
\author         Joseph Ho

\par			Course: GAM300
\date           07/10/2023

\brief
    This file contains the definitions of functions in the History system that handles the
    undo and redo functionality in the editor. 

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "EditorHistory.h"
#include "Scene/SceneManager.h"
#include "Core/Events.h"


#pragma warning( disable : 4996)

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

        if (undochange.type == VARIABLE) {
            property::set(*(undochange.component), undochange.property.c_str(), undochange.previousValue);
        }
        //Set script variables
        if (undochange.type == SCRIPT) {
            SetScriptField(undochange, UNDO);
        }
        //Set script references
        else if (undochange.type == REFERENCE) {
            SetScriptReference(undochange, UNDO);
        }
        else if (undochange.type == COMPONENT) {
            AmendComponent(undochange, UNDO);
        }
        else if (undochange.type == ENTITY) {
            AmendEntity(undochange, UNDO);
        }
        
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

        if (redochange.type == VARIABLE) {
            property::set(*(redochange.component), redochange.property.c_str(), redochange.newValue);
        }
        //set script variables
        else if (redochange.type == SCRIPT) {
            SetScriptField(redochange, REDO);
        }
        //Set script references
        else if (redochange.type == REFERENCE) {
            SetScriptReference(redochange, REDO);
        }
        else if (redochange.type == COMPONENT) {
            AmendComponent(redochange, REDO);
        }
        else if (redochange.type == ENTITY) {
            AmendEntity(redochange, REDO);
        }
   
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

void HistoryManager::ClearUndoStack() {
    while (!UndoStack.empty()) {
        UndoStack.pop();
    }
}

//add a change in reference action to the undo stack
void HistoryManager::AddReferenceChange(Change& change, Component oldRef, Component newRef) {
    change.oldreference = oldRef;
    change.newreference = newRef;
    change.type = REFERENCE;
    UndoStack.push(change);
}

void HistoryManager::AddComponentChange(Change& change) {
    if(change.action == DELETING)
        change.component->state = DELETED;
        change.type = COMPONENT;

    UndoStack.push(change);
}

void HistoryManager::AmendComponent(Change& change, ChangeType type) {
    Scene& curr_scene = MySceneManager.GetCurrentScene();

    if (type == UNDO) {
        if (change.action == CREATING) 
            change.component->state = DELETED;
        else
            change.component->state = NORMAL;
    }
        
    else {
        if(change.action == CREATING)
            change.component->state = NORMAL;
        else
            change.component->state = DELETED;
    }
        
}

//Helper function that displays all relevant fields and types in a component
template <typename T>
void DisplayComponentHelper(T& component, ChangeType type)
{
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    if (type == UNDO)
        component.state = NORMAL;
    else
        component.state = DELETED;
}

//Template recursive function to set all components in the deleted entity to deleted as well
template<typename T, typename... Ts>
struct SetComponentsDeleteStruct
{
public:
    constexpr SetComponentsDeleteStruct(TemplatePack<T, Ts...> pack) {}
    SetComponentsDeleteStruct() = delete;
    SetComponentsDeleteStruct(Entity& entity, ChangeType type)
    {
        SetNext<T, Ts...>(entity, type);
    }
private:
    template<typename T1, typename... T1s>
    void SetNext(Entity& entity, ChangeType type)
    {
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

        if constexpr (SingleComponentTypes::Has<T1>()) {
            if (curr_scene.Has<T1>(entity)) {

                auto& component = curr_scene.Get<T1>(entity);

                if (type == UNDO)
                    component.state = NORMAL;
                else
                    component.state = DELETED;
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>()) {

            auto components = curr_scene.GetMulti<T1>(entity);
            for (T1* component : components) {
                if (type == UNDO)
                    component->state = NORMAL;
                else
                    component->state = DELETED;
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            SetNext<T1s...>(entity, type);
        }
    }
};
using SetAllComponentsDeleteStruct = decltype(SetComponentsDeleteStruct(AllComponentTypes()));
void SetComponentsDelete(Entity& entity, ChangeType type) { SetAllComponentsDeleteStruct obj{ entity, type}; }

void SetStateRecursive(Entity& entity, STATE state, ChangeType type) {
    entity.state = state;

    auto& curr_scene = MySceneManager.GetCurrentScene();
    auto children = curr_scene.Get<Transform>(entity).child;

    for (auto& child : children) {
        Entity& child_entity = curr_scene.Get<Entity>(child);
        child_entity.state = state;
        SetComponentsDelete(child_entity, type);
        SetStateRecursive(child_entity, state, type);
    }
}

void HistoryManager::AddEntityChange(Change& change) {
    if (change.action == DELETING) {
        SetStateRecursive(*change.entity, DELETED, REDO);
        //change.entity->state = DELETED;
        //Scene& curr_scene = MySceneManager.GetCurrentScene();
        //auto children = curr_scene.Get<Transform>(*change.entity).child;

        //for (auto& child : children) {
        //    Entity& ent = curr_scene.Get<Entity>(child);
        //    ent.state = DELETED;
        //    //set all children component to delete too
        //    SetComponentsDelete(ent, REDO);
        //}

        //set components of the entity to deleted as well
        SetComponentsDelete(*change.entity, REDO);
    }
  
    change.type = ENTITY;
    UndoStack.push(change);
}

void HistoryManager::AmendEntity(Change& change, ChangeType type) {
    Scene& curr_scene = MySceneManager.GetCurrentScene();

    if (type == UNDO) {
        if (change.action == CREATING) {
            change.entity->state = DELETING;
            SetComponentsDelete(*change.entity, REDO);
            SelectedEntityEvent selectedEvent{ 0 };
            EVENTS.Publish(&selectedEvent);
        }  
        else {
            SetStateRecursive(*change.entity, NORMAL, UNDO);
            //change.entity->state = NORMAL;
            //auto children = curr_scene.Get<Transform>(*change.entity).child;
            //for (auto& child : children) {
            //    Entity& ent = curr_scene.Get<Entity>(child);
            //    ent.state = NORMAL;
            //    //set all children component to normal too
            //    SetComponentsDelete(ent, UNDO);
            //}
            

            SetComponentsDelete(*change.entity, UNDO);
            SelectedEntityEvent selectedEvent{ change.entity };
            EVENTS.Publish(&selectedEvent);
        }
    }
    else {
        if (change.action == CREATING) {
            change.entity->state = NORMAL;
            SetComponentsDelete(*change.entity, UNDO);
            SelectedEntityEvent selectedEvent{ change.entity };
            EVENTS.Publish(&selectedEvent);
        }
        else {
            change.entity->state = DELETING;
            SetComponentsDelete(*change.entity, REDO);   
            SelectedEntityEvent selectedEvent{ 0 };
            EVENTS.Publish(&selectedEvent);
        }
    }
}