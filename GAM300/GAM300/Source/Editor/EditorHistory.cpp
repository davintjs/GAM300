#include "Precompiled.h"
#include "EditorHistory.h"


bool HistoryManager::UndoStackEmpty() {
    return UndoStack.empty();
}

bool HistoryManager::RedoStackEmpty() {
    return RedoStack.empty();
}

template <typename T, typename... Ts>
void ScriptSetbackHelper(Field& rhs, property::data data)
{
    //YAML::Emitter out;
    //out << node;
    //PRINT(out.c_str(), '\n');
    using DataVariant = decltype(data);
    using DataPack = decltype(TemplatePack(DataVariant()));
    if constexpr (DataPack::Has<T>())
    {
        if (rhs.fType == GetFieldType::E<T>())
        {
            //if constexpr (AllObjectTypes::Has<T>())
            //{
            //    // Storing EUID and UUID of Objects (Gameobject, Components etc)
            //    Scene& scene{ MySceneManager.GetCurrentScene() };
            //    T*& object = *reinterpret_cast<T**>(rhs.data);
            //    if constexpr (std::is_same<T, Entity>())
            //    {
            //        Engine::UUID euid = node["fileID"].as<Engine::UUID>();
            //        if (euid)
            //            object = &scene.Get<T>(euid);
            //    }
            //    else
            //    {
            //        Engine::UUID uuid = node["fileID"].as<Engine::UUID>();
            //        if (uuid)
            //        {
            //            if constexpr (std::is_same_v<T, Transform> || std::is_same_v<T, Tag>)
            //            {
            //                object = &scene.Get<T>(uuid);
            //            }
            //            else
            //            {
            //                object = &scene.GetByUUID<T>(uuid);
            //            }
            //        }
            //    }
            //}
            //else
            //{
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
            //}
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
            //undochange.previousValue.
            char buffer[2048];
            Field field{ AllFieldTypes::Size(),2048,buffer };
            Script* script = reinterpret_cast<Script*>(undochange.component);
            ScriptGetFieldEvent getFieldEvent{*script,fieldName.c_str(),field};
            EVENTS.Publish(&getFieldEvent);
            ScriptSetback(field, undochange.previousValue, AllFieldTypes());
            ScriptSetFieldEvent setFieldEvent{ *script ,fieldName.c_str(), field};
            EVENTS.Publish(&setFieldEvent);
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
