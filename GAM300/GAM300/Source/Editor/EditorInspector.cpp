/*!***************************************************************************************
\file			EditorInspector.cpp
\project
\author         Joseph Ho
\coauthor       Zachary Hong

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the functions that renders the inspector window in
    Editor. These functionalities include:
    1. Displaying Components of the selected entity
    2. Display the individual types and fields in the editor

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "Editor.h"
#include "EditorHeaders.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "EditorTemplates.h"
#include "Scene/Components.h"
#include "Graphics/MeshManager.h"
#include <variant>
#include "PropertyConfig.h"
#include "Utilities/ThreadPool.h"
#include "Scene/Identifiers.h"
#include "Graphics/GraphicsHeaders.h"
#include "PropertyConfig.h"
#include "Utilities/Serializer.h"

#include "Core/Events.h"
#include "Core/EventsManager.h"

#define BUTTON_HEIGHT .1 //Percent
#define BUTTON_WIDTH .6 //Percent
#define TEXT_BUFFER_SIZE 2048
#define PI 3.1415927
#define EPSILON 0.01f

enum ComponentType {
    AUDIO, MESHRENDERER, SPRITERENDERER
};

//Flags for inspector headers/windows
static ImGuiTableFlags windowFlags =
ImGuiTableFlags_Resizable |
ImGuiTableFlags_NoBordersInBody |
ImGuiTableFlags_NoSavedSettings |
ImGuiTableFlags_SizingStretchProp;

bool referenceChanged = false;
Object* previousReference = nullptr;
Object* newReference = nullptr;
std::string refFieldName{};
bool isAddingReference = false;
size_t editedContainer{};

//for checking of drag fields
bool valueChanged = false;
float initialvalue = 0.f;
float changedvalue = 0.f;
Vector3 initialVector;
Vector4 intialColor;

template <typename T>
void Display(const char* name, T& val);

// DisplayType contains overloads that display the respective fields based on the type passed into the function
template <typename T>
void DisplayType(Change& change, const char* name, T& val)
{
    UNREFERENCED_PARAMETER(change);
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(val);
}

template <typename T>
void DisplayType(const char* name, T& val)
{
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(val);
}

void DisplayType(Change& change, const char* name, bool& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    bool buf = val;
    if (ImGui::Checkbox(idName.c_str(), &buf)) {
        EDITOR.History.SetPropertyValue(change, val, buf);
    }
}

//for texture picker strings (read-only)
void DisplayType(const char* name, std::string& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    PRINT(val, '\n');
    static char buffer[TEXT_BUFFER_SIZE];
    strcpy(buffer, val.c_str());
    ImGui::InputText(idName.c_str(), buffer, ImGuiInputTextFlags_ReadOnly);
    val = buffer;
}

void DisplayType(Change& change, const char* name, std::string& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    static char buffer[TEXT_BUFFER_SIZE];
    strcpy(buffer, val.c_str());
    if(ImGui::InputText(idName.c_str(), buffer, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::string newString = buffer;
        EDITOR.History.SetPropertyValue(change, val, newString);
    }
    val = buffer;
}

void DisplayType(Change& change, const char* name, int& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    int buf = val;
    if (ImGui::DragInt(idName.c_str(), &buf)) {
        EDITOR.History.SetPropertyValue(change, val, buf);
    }
}

template <size_t SZ>
void DisplayType(Change& change, const char* name, char(&val)[SZ])
{
    static std::string idName{};
    idName = "##";
    idName += name;
    char buf[SZ] = val;
    if (ImGui::InputTextMultiline(idName.c_str(), buf, SZ, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16))) {
        EDITOR.History.SetPropertyValue(change, val, buf);
    }
}

void DisplayType(Change& change, const char* name, char*& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    char* buf = val; 
    if (ImGui::InputTextMultiline(idName.c_str(), buf, TEXT_BUFFER_SIZE, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16))) {
        EDITOR.History.SetPropertyValue(change, val, buf);
    }
}

void DisplayAssetPicker(Change& change,const fs::path& fp, Engine::GUID& guid)
{
    static ImGuiTextFilter filter;
    fs::path extension = fp.extension();
    size_t extensionType = GetAssetType::E<Asset>();
    if (AssetExtensionTypes.contains(extension))
        extensionType = AssetExtensionTypes[extension];

    ImGui::SameLine();
    ImGuiWindowFlags win_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar;


    if (ImGui::Button("Edit")) {
        filter.Clear();
        ImGui::OpenPopup("Texture");
    }

    GLuint defaultFileIcon = TextureManager.GetTexture("Assets/Icons/fileicon.dds");

    //Component Settings window
    ImGui::SetNextWindowSize(ImVec2(250.f, 300.f));

    if (ImGui::BeginPopup("Texture", win_flags)) {
        
        ImGui::Dummy(ImVec2(0, 10.f));
        ImGui::Text("Filter: "); ImGui::SameLine();
        filter.Draw();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -20.f), false);
        // Back button to return to parent directory
        static float padding = 15.f;
        static float iconsize = 50.f;
        float cellsize = iconsize + padding;

        float window_width = ImGui::GetContentRegionAvail().x;
        int columncount = (int)(window_width / cellsize);
        if (columncount < 1) { columncount = 1; }

        ImGui::Columns(columncount, 0, false);

        //remove texture icon
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
        size_t id = (size_t)GET_TEXTURE_ID("Assets/Icons/Cancel_Icon.dds");
        if (ImGui::ImageButton((ImTextureID)id, { iconsize, iconsize }, { 0 , 1 }, { 1 , 0 })) {
            for (auto& pair : DEFAULT_ASSETS)
            {
                if (pair.first.extension() != extension)
                    continue;

                if (pair.first.string().starts_with("None"))
                {
                    EDITOR.History.SetPropertyValue(change, guid, pair.second);
                    break;
                }
            }

            ImGui::PopStyleColor();
            ImGui::EndChild();
            ImGui::EndPopup();
            ImGui::CloseCurrentPopup();
            return;
        };
        ImGui::PopStyleColor();
        //render file name below icon
        ImGui::TextWrapped("None");
        ImGui::NextColumn();

        int i = 0;

        for (auto& pair : DEFAULT_ASSETS)
        {
            if (pair.first.extension() != extension)
                continue;

            if (pair.first.string().starts_with("None"))
                continue;
            
            if (!filter.PassFilter(pair.first.string().c_str()))
                continue;


            fs::path icon = "Assets/Icons/fileicon.dds";

            //if not png or dds file, dont show

            ImGui::PushID(i++);

            //render respective file icon textures
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
            ImTextureID icon_id = (ImTextureID)TextureManager.GetTexture(icon);
            if (ImGui::ImageButton(icon_id, { iconsize, iconsize }, { 0 , 0 }, { 1 , 1 }))
            {
                EDITOR.History.SetPropertyValue(change, guid, pair.second);
            }
            ImGui::PopStyleColor();
            ImGui::TextWrapped(pair.first.stem().string().c_str());

            //render file name below icon
            ImGui::NextColumn();
            ImGui::PopID();
        }

        //using filesystem to iterate through all folders/files inside the "/Data" directory

        if (extension == ".geom")
        //for (auto& it : std::filesystem::recursive_directory_iterator{ "Assets" })
        {
            // Bean: Put this publish event in the open popup in the future
            GetAssetsEvent<MeshAsset> e1;
            EVENTS.Publish(&e1);
            fs::path icon = "Assets/Icons/fileicon.dds";
            auto iconID = GET_TEXTURE_ID(icon);

            for (auto& meshAsset : *e1.pAssets)
            {                    
                std::string path = meshAsset.second.mFilePath.stem().string().c_str();

                if (!filter.PassFilter(path.c_str()))
                    continue;

                //render respective file icon textures
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
                ImGui::PushID(i++);
                ImTextureID textureID = (ImTextureID)iconID;
                if (ImGui::ImageButton(textureID, { iconsize, iconsize }, { 0 , 0 }, { 1 , 1 }))
                {
                    Engine::GUID currentGUID = meshAsset.first;
                    EDITOR.History.SetPropertyValue(change, guid, currentGUID);
                    //PRINT("Using guid: ", currentGUID.ToHexString(), " name: ", meshAsset.second.mFilePath.stem().string(), "\n");
                }
                ImGui::PopID();
                ImGui::PopStyleColor();
                ImGui::TextWrapped(path.c_str());

                //render file name below icon
                ImGui::NextColumn();
                
            }
        }
        else if(extension == ".anim")
            //for (auto& it : std::filesystem::recursive_directory_iterator{ "Assets" })
        {
            // Bean: Put this publish event in the open popup in the future
            GetAssetsEvent<AnimationAsset> e2;
            EVENTS.Publish(&e2);
            fs::path icon = "Assets/Icons/fileicon.dds";
            auto iconID = GET_TEXTURE_ID(icon);

            for (auto& animAsset : *e2.pAssets)
            {
                std::string path = animAsset.second.mFilePath.stem().string().c_str();

                if (!filter.PassFilter(path.c_str()))
                    continue;

                //render respective file icon textures
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
                ImGui::PushID(i++);
                ImTextureID textureID = (ImTextureID)iconID;
                if (ImGui::ImageButton(textureID, { iconsize, iconsize }, { 0 , 0 }, { 1 , 1 }))
                {
                    Engine::GUID currentGUID = animAsset.first;
                    EDITOR.History.SetPropertyValue(change, guid, currentGUID);
                }
                ImGui::PopID();
                ImGui::PopStyleColor();
                ImGui::TextWrapped(path.c_str());

                //render file name below icon
                ImGui::NextColumn();

            }
        }
        else
        {
            for (auto& it : std::filesystem::recursive_directory_iterator{ "Assets" })
            {
                const auto& path = it.path();
                if (path.extension() != extension)
                    continue;

                if (!filter.PassFilter(path.string().c_str()))
                    continue;
                if (!AssetExtensionTypes.contains(path.extension()))
                    continue;
                if (AssetExtensionTypes[path.extension()] != extensionType)
                    continue;
                //if (path.extension() != extension)

                GetAssetEvent e { path };
                EVENTS.Publish(&e);
                Engine::GUID currentGUID = e.guid;

                //if not png or dds file, dont show

                //Draw the file / folder icon based on whether it is a directory or not
                GLuint icon_id = GET_TEXTURE_ID(currentGUID);
                if (icon_id == 0) {
                    icon_id = defaultFileIcon;
                }

                ImGui::PushID(i++);
                ImTextureID textureID = (ImTextureID)icon_id;
                //render respective file icon textures
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
                if (ImGui::ImageButton(textureID, { iconsize, iconsize }, { 0 , 0 }, { 1 , 1 }))
                {
                    EDITOR.History.SetPropertyValue(change, guid, currentGUID);
                }
            
                ImGui::PopStyleColor();
                ImGui::TextWrapped(path.stem().string().c_str());

                ImGui::NextColumn();
                ImGui::PopID();
            }
        }
       
        ImGui::Columns(1);
        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

template <typename... Ts>
fs::path DisplayGUIDHelper(TemplatePack<Ts...>, Engine::GUID& guid)
{
    fs::path path;
    if (([&](auto type)
    {
        using T = decltype(type);
        GetFilePathEvent<T> e{ guid };
        EVENTS.Publish(&e);
        if (e.filePath == "")
            return false;
        path = e.filePath;
        return true;
    }
    (Ts{}) || ...))
    {
        return path;
    }
    return path;
}

void DisplayType(Change& change, const char* name, Engine::GUID& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    //Val is a default asset guid
    fs::path fp;
    for (auto& pair : DEFAULT_ASSETS)
    {
        if (pair.second == val)
        {
            fp = pair.first;
            break;
        }
    }
    if (fp.empty())
    {
        fp = DisplayGUIDHelper(AssetTypes(),val);
    }
    const std::string& pathStr = fp.stem().string();
    ImGui::InputText(idName.c_str(), (char*)pathStr.c_str(), pathStr.size(), ImGuiInputTextFlags_ReadOnly);
    DisplayAssetPicker(change,fp,val);
}



bool DisplayType(const char* name, float& val)
{
    static float temp{};
    static std::string idName{};
    const char* cIdName{};
    if (name[0] == '#' && name[1] == '#')
    {
        cIdName = name;
    }
    else
    {
        idName = "##";
        idName += name;
        cIdName = idName.c_str();
    }
    bool ischanged = false;
    float buf = val;

    if (ImGui::DragFloat(cIdName, &buf, 0.15f)) {
        if (!valueChanged) {
            initialvalue = val;
        }
        valueChanged = true;
        val = buf;
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        ischanged = true;
        valueChanged = false;
    }

    return ischanged;
}


void DisplayType(Change& change, const char* name, float& val)
{
    static float temp{};
    static std::string idName{};
    const char* cIdName{};
    if (name[0] == '#' && name[1] == '#')
    {
        cIdName = name;
    }
    else
    {
        idName = "##";
        idName += name;
        cIdName = idName.c_str();
    }
    //ImGui::DragFloat(cIdName, &val, 0.15f);

    float buf = val;
   
    if (ImGui::DragFloat(cIdName, &buf, 0.01f)) {
        if (!valueChanged) {
            initialvalue = val;
        }
        valueChanged = true;
        val = buf;
        changedvalue = buf;
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        valueChanged = false;
        val = initialvalue;
        EDITOR.History.SetPropertyValue(change, val, changedvalue);
    }
}

//void DisplayType(Change& change, const char* name, double& val)
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    float buf{ (float)val };
//    if (ImGui::DragFloat(idName.c_str(), &buf, 0.15f)) {
//        double temp = buf;
//        /EDITOR.History.SetPropertyValue(change, val, temp);
//    }
//}

void DisplayType(Change& change, const char* name, Vector3& val)
{
    static float temp{};
    static std::string idName{};
    idName = "##";
    idName += name;

    Vector3 buf = val;
    if (!std::strcmp(name, "Rotation")) {
        buf *= (180.f / PI);
    }
    bool changed = false;
    if (ImGui::BeginTable("Vector3", 3, windowFlags))
    {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        idName += 'X';
        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);

        changed = DisplayType(idName.c_str(), buf.x);
        if (changed) {
            initialVector = val;

            initialVector.x = initialvalue;
            val = initialVector;
        }

        ImGui::TableNextColumn();
        idName.back() = 'Y';
        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        if (!changed && DisplayType(idName.c_str(), buf.y)) {
            changed = true;
            initialVector = val;
            initialVector.y = initialvalue;
            val = initialVector;
        }
           


        ImGui::TableNextColumn();
        idName.back() = 'Z';
        ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        if (!changed && DisplayType(idName.c_str(), buf.z)) {
            changed = true;
            initialVector = val;
            initialVector.z = initialvalue;
            val = initialVector;
        }

       

        //convert rotation from degree back to radians
        if (!std::strcmp(name, "Rotation")) {
            buf *= (PI / 180.f);
            val *= (PI / 180.f);
            //Check whether value is within epsilon range (to avoid negative 0)
            for (int i = 0; i < 3; ++i) {
                if (std::fabs(buf[i] - 0.f) < EPSILON)
                    buf[i] = 0;
            }
        }

        if (!changed)
            val = buf;

        ImGui::EndTable();
    }

    if (changed) {
        EDITOR.History.SetPropertyValue(change, val, buf);
    }
}

void DisplayType(Change& change, const char* name, Vector4& val)
{
    static float temp{};
    static std::string idName{};
    idName = "##";
    idName += name;

    Vector4 buf = val;
    ImVec4 color = ImVec4(buf.x, buf.y, buf.z, buf.w);

    bool ischanged = false;

    if (ImGui::ColorButton("##color", color, 0, ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
        ImGui::OpenPopup("colorpicker");

    if (ImGui::BeginPopup("colorpicker"))
    {
        if (ImGui::ColorPicker4("##picker", (float*)&buf, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueWheel)) {
            ischanged = true;
            if (!valueChanged)
                intialColor = val;

            valueChanged = true;
            val = buf;
        }

        if (ImGui::IsItemDeactivatedAfterEdit()) {
            valueChanged = false;
            buf = val;
            val = intialColor;
            EDITOR.History.SetPropertyValue(change, val, buf);
        }
        ImGui::EndPopup();
    }

}

void DisplayType(Change& change, const char* name, Vector2& val)
{
    static float temp{};
    static std::string idName{};
    idName = "##";
    idName += name;
    bool changed = false;
    Vector2 buf = val;
    if (ImGui::BeginTable("Vector2", 2, windowFlags))
    {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        idName += 'X';
        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        changed = DisplayType(idName.c_str(), buf.x);

        ImGui::TableNextColumn();
        idName.back() = 'Y';
        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);

        if (!changed && DisplayType(idName.c_str(), buf.y))
            changed = true;

        if (changed)
            //EDITOR.History.SetPropertyValue(change, val, buf);

        ImGui::EndTable();
    }
}

template <typename T>
void AddReferencePanel(T*& container)
{
    Scene& scene = MySceneManager.GetCurrentScene();
    static ImGuiTextFilter filter;
    static std::string windowName;
    windowName = "Add ";
    windowName += GetType::Name<T>();
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100.f, 100.f), ImGuiCond_FirstUseEver);
    windowName += " Reference";
    bool open = true;
    if (ImGui::Begin(windowName.c_str(), &open))
    {
        ImGui::PushItemWidth(-1);
        ImGui::Text("Filter"); ImGui::SameLine();
        filter.Draw("##References");
        ImGui::PopItemWidth();
        static std::string buttonName{};
        ImVec2 buttonSize = ImGui::GetWindowSize();
        buttonSize.y *= (float)BUTTON_HEIGHT;
        if (ImGui::Button("None", buttonSize))
        {
            editedContainer = 0;
            container = nullptr;
        }
        for (T& object : scene.GetArray<T>())
        {
            Tag& tag = scene.Get<Tag>(object);
            buttonName = tag.name;
            if constexpr (std::is_same_v<T, Entity>)
            {
                ImGui::PushID((int)object.EUID());
            }
            else
            {
                ImGui::PushID((int)object.UUID());
            }
            if (filter.PassFilter(tag.name.c_str()) && ImGui::Button(buttonName.c_str(), buttonSize))
            {
                //if reference is changed, add it to reference buffer
                if (container != &object) {
                    previousReference = container;
                    newReference = &object;
                    referenceChanged = true;
                }
                editedContainer = 0;
                container = &object;
            }
            ImGui::PopID();
        }
        ImGui::End();
    }
    if (!open)
        editedContainer = 0;
}


GENERIC_RECURSIVE
(
    void, 
    AddReferencePanel, 
    AddReferencePanel(((Field*)pObject )->Get<T*>())
)

template <typename T>
void DisplayType(Change& change, const char* name, T*& container, const char* altName = nullptr)
{
    UNREFERENCED_PARAMETER(name);
    if constexpr (AllObjectTypes::Has<T>())
    {
        static std::string btnName;
        if (container)
        {
            btnName = MySceneManager.GetCurrentScene().Get<Tag>(container->EUID()).name;
        }
        else
        {
            btnName = "None";
        }
        btnName += "("; 
        if constexpr (std::is_same_v<T,Entity>)
        {
            btnName += "GameObject";
        }
        else
        {
            if (altName)
            {
                btnName += altName;
            }
            else
            {
                btnName += GetType::Name<T>();
            }
        }
        btnName += ")";
        ImGui::Button(btnName.c_str(), ImVec2(-FLT_MIN, 0.f));
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            isAddingReference = true;
        }
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetType::Name<T>());
            if (payload)
            {
                Handle handle = *(Handle*)payload->Data;
                container = &MySceneManager.GetCurrentScene().Get<T>(handle);
            }
            ImGui::EndDragDropTarget();
        }
    }
}

template <typename T, typename... Ts>
void DisplayField(Change& change, const char* name, Field& field)
{
    if (GetFieldType::E<T>() == field.fType)
    {
        if (field.fType < AllObjectTypes::Size())
        {
            Scene& scene{ MySceneManager.GetCurrentScene() };
            T*& value = *reinterpret_cast<T**>(field.data);
            if constexpr (std::is_same<T, Script>())
            {
                DisplayType(change, name, value, field.typeName.c_str());
            }
            else
            {
                
                DisplayType(change, name, value);
            }
        }
        else
        {
            if constexpr (std::is_same_v<T, char*>)
            {
                char* str = (char*)field.data;
                std::string val = str;
                DisplayType(change, name, val);
                if (val.size() >= TEXT_BUFFER_SIZE - 1)
                {
                    memcpy(str, val.data(), TEXT_BUFFER_SIZE - 1);
                    str[TEXT_BUFFER_SIZE - 1] = 0;
                }
                else
                {
                    strcpy(str, val.data());
                }
            }
            else
            {
                DisplayType(change,name, field.Get<T>());
            }
        }
        return;
    }
    if constexpr (sizeof...(Ts) != 0)
    {
        DisplayField<Ts...>(change, name, field);
    }
}

template <typename T, typename... Ts>
void DisplayField(Change& change, const char* name, Field& field, TemplatePack<T,Ts...>)
{
    DisplayField<T,Ts...>(change, name,field);
}

void DisplayType(Change& change, const char* name, Field& val)
{
    DisplayField(change, name, val, AllFieldTypes());
}

template <typename T>
void Display(const char* name, T& val)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    DisplayType(name, val);
}

template <typename T>
void Display(Change& change, const char* name, T& val)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    DisplayType(change, name, val);
}

void Display(const char* string)
{
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text(string);
}

//Function to display and edit textures of a given property.

template <typename T>
void DisplayLightTypes(Change& change, T& value) {
    if constexpr (std::is_same<T, int>()) {
        ImGui::AlignTextToFramePadding();
        ImGui::TableNextColumn();
        ImGui::Text("Type");
        ImGui::TableNextColumn();

        Engine::UUID curr_index = EditorHierarchy::Instance().selectedEntity;
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
        Entity& curr_entity = curr_scene.Get<Entity>(curr_index);

        std::vector<const char*> layers;
        layers.push_back("Spot"); layers.push_back("Directional"); layers.push_back("Point");
        int index = value;
        ImGui::PushItemWidth(100.f);
        if (ImGui::Combo("##LightType", &index, layers.data(), (int)layers.size(), 5)) {
            EDITOR.History.SetPropertyValue(change, value, index);
        }
        ImGui::PopItemWidth();
    }
}

//Display all available audio channels inside the system for user to choose
template <typename T>
void DisplayAudioChannels(Change& change, T& value) {
    if constexpr (std::is_same<T, int>()) {
        ImGui::AlignTextToFramePadding();
        ImGui::TableNextColumn();
        ImGui::Text("Channel");
        ImGui::TableNextColumn();

        Engine::UUID curr_index = EditorHierarchy::Instance().selectedEntity;
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
        Entity& curr_entity = curr_scene.Get<Entity>(curr_index);

        std::vector<const char*> layers;
        layers.push_back("Music"); layers.push_back("SFX"); layers.push_back("Loop FX");
        int index = value;
        ImGui::PushItemWidth(100.f);
        if (ImGui::Combo("##AudioChannel", &index, layers.data(), (int)layers.size(), 5)) {
            EDITOR.History.SetPropertyValue(change, value, index);
        }
        ImGui::PopItemWidth();
    }
}

int shader_id;

//Display all available shaders inside the system for user to choose
template <typename T>
void DisplayShaders(Change& change, T& value) {
    if constexpr (std::is_same_v<T, int>) {
        ImGui::AlignTextToFramePadding();
        ImGui::TableNextColumn();
        ImGui::Text("Shader");
        ImGui::TableNextColumn();

        Engine::UUID curr_index = EditorHierarchy::Instance().selectedEntity;
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
        Entity& curr_entity = curr_scene.Get<Entity>(curr_index);
        MeshRenderer& rend = static_cast<MeshRenderer&>(*change.component);

        std::vector<const char*> layers;
        shader_id = value;
        //Get all materials inside PBR shader
        for (auto& shader : MATERIALSYSTEM.available_shaders) {
            layers.push_back(shader.name.c_str());
        }


        //ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::Combo("##Shader", &shader_id, layers.data(), (int)layers.size(), 5)) {
            EDITOR.History.SetPropertyValue(change, value, shader_id);
        }
    }
}


//Displays all the properties of an given entity
template <typename T>
void Display_Property(T& comp) {

    std::vector<property::entry> List;
    property::SerializeEnum(comp, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
        {
            if (!Flags.m_isDontShow){
                auto entry = property::entry { PropertyName, Data };
                std::visit([&](auto& Value) {
                    using T1 = std::decay_t<decltype(Value)>;

                    std::string DisplayName = entry.first;
                    auto it = DisplayName.begin() + DisplayName.find_last_of("/");
                    DisplayName.erase(DisplayName.begin(), ++it);

                    Change newchange(&comp, entry.first);

                    ImGui::PushID(entry.first.c_str());

                    //Temporary implementation for materials
                    if (entry.first.find("Shader") != std::string::npos) {
                        DisplayShaders(newchange, Value);
                    }
                    else if (entry.first.find("AudioChannel") != std::string::npos) {
                        DisplayAudioChannels(newchange, Value);
                    }
                    else {
                        Display<T1>(newchange, DisplayName.c_str(), Value);
                    }
                    ImGui::PopID();

                    }
                , Data);
                property::set(comp, entry.first.c_str(), Data);

                // If we are dealing with a scope that is not an array someone may have change the SerializeEnum to a DisplayEnum they only show up there.
                //assert(Flags.m_isScope == false || PropertyName.back() == ']');
            }
           
        });
}

//Display all fields from a script component
void DisplayComponent(Script& script)
{
    static char buffer[2048]{};
    ScriptGetFieldNamesEvent getFieldNamesEvent{script};
    EVENTS.Publish(&getFieldNamesEvent);
    for (size_t i = 0; i < getFieldNamesEvent.count; ++i)
    {
        const char* fieldName = getFieldNamesEvent.pStart[i];
        Field field{ AllFieldTypes::Size(),2048, buffer};
        ScriptGetFieldEvent getFieldEvent{script,fieldName,field};
        EVENTS.Publish(&getFieldEvent);
        if (field.fType < AllFieldTypes::Size())
        {
            std::string fieldproperty = "Script/" + std::string(fieldName);
            Change change(&script, fieldproperty);
            change.type = SCRIPT;
            Display(change, fieldName, field);
            if (isAddingReference)
            {
                //Hash
                editedContainer = script.UUID() ^ i;
                isAddingReference = false;
            }
            if (editedContainer == (script.UUID() ^ i))
            {
                AddReferencePanel(field.fType, &field);
                //add reference change to undo stack
                if (referenceChanged) {
                    Change refChange(&script, fieldName);
                    EDITOR.History.AddReferenceChange(refChange, previousReference, newReference);
                    referenceChanged = false;
                }
            }
            ScriptSetFieldEvent setFieldEvent{ script,fieldName,field};
            EVENTS.Publish(&setFieldEvent);
        }
        
    }
}

//Display all available light types inside the system for user to choose
void DisplayLightProperties(LightSource& source) {

    Change enableshadow(&source, "LightSource/EnableShadow");
    Display<bool>(enableshadow, "Enable Shadow", source.enableShadow);

    Change lighttypes(&source, "LightSource/lightType");
    DisplayLightTypes(lighttypes, source.lightType);
    
    Change intensity(&source, "LightSource/Intensity");
    Display<float>(intensity, "Intensity", source.intensity);

    Change color(&source, "LightSource/Color");
    Display<Vector3>(color, "Color", source.lightingColor);

    Change lightpos(&source, "LightSource/lightpos");
    Change lightdir(&source, "LightSource/Direction");
    Change innercutoff(&source, "LightSource/Inner Cutoff");
    Change outercutoff(&source, "LightSource/Outer Cutoff");
    if (source.lightType == (int)SPOT_LIGHT) {
        Display<Vector3>(lightpos, "Light Position", source.lightpos);
        Display<Vector3>(lightdir, "Direction", source.direction);
        Display<float>(innercutoff, "Inner Cutoff", source.inner_CutOff);
        Display<float>(outercutoff, "Outer Cutoff", source.outer_CutOff);
    }
    else if(source.lightType == (int)DIRECTIONAL_LIGHT){
        Display<Vector3>(lightdir, "Direction", source.direction);
    }
    else { //POINT LIGHT
        Display<Vector3>(lightpos, "Light Position", source.lightpos);
    }
}

//Helper function that displays all relevant fields and types in a component
template <typename T>
void DisplayComponentHelper(T& component)
{
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
    static std::string name{};
    if constexpr (std::is_same<T, Script>())
    {
        GetFilePathEvent<ScriptAsset> e{component.scriptId};
        EVENTS.Publish(&e);
        if(&component)
            name = (e.filePath.stem().string() + " (Script)");
    }
    else if constexpr (AllComponentTypes::Has<T>())
    {
        name = GetType::Name<T>();
    }
    else
    {
        //This means T is not a component
        //PRINT(typeid(T).name());
    }
    
    ImVec4 check_color = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.3f, 1.0f)); // set color of checkbox

    bool checkbox = curr_scene.IsActive(component);
    std::string label = "##" + name;
    ImGui::Checkbox(label.c_str(), &checkbox);
    curr_scene.SetActive(component,checkbox);

    ImGui::PopStyleColor(); 
    
    ImGui::SameLine(ImGui::GetItemRectSize().x + 10.f);

    bool windowopen = ImGui::CollapsingHeader(name.c_str(), nodeFlags);

    ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 30.f);

    ImGuiWindowFlags win_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    const char* popup = GetType::Name<T>();

    ImGui::PushID((int)component.UUID());

    if (ImGui::Button("...")) {
        ImGui::OpenPopup(popup);
    }

    //Component Settings window
    ImGui::SetNextWindowSize(ImVec2(150.f, 180.f));
    if (ImGui::BeginPopup(popup, win_flags)) {

        if (ImGui::MenuItem("Reset")) {

        }

        if constexpr (!std::is_same<T, Transform>()) {
            if (ImGui::MenuItem("Remove Component")) {
                //Destroy current component of current selected entity in editor
                Change newchange;
                newchange.component = &component;
                newchange.property = popup;
                EDITOR.History.AddComponentChange(newchange);
            }
        }
        else {
            ImGui::TextDisabled("Remove Component");
        }

        if (ImGui::MenuItem("Copy Component")) {

        }

        ImGui::EndPopup();
    }

    ImGui::PopID();

    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Component Settings");
    
    if (windowopen)
    {
        ImGuiWindowFlags winFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchProp
            | ImGuiTableFlags_PadOuterX;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 2));

        if (ImGui::BeginTable("Component", 2, winFlags))
        {
            ImGui::Indent();
            ImGui::TableSetupColumn("Text", 0, 0.4f);
            ImGui::TableSetupColumn("Input", 0, 0.6f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 0));

            if constexpr (std::is_same_v<T,Script>)
            {
                DisplayComponent(component);
            }
            else if constexpr (std::is_same_v<T, LightSource>) {
                DisplayLightProperties(component);
            }
            else
            {
                Display_Property(component);
            }

            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();

            ImGui::Unindent();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }

}

//Template recursive function to display the components in an entity
template<typename T, typename... Ts>
struct DisplayComponentsStruct
{
public:
    constexpr DisplayComponentsStruct(TemplatePack<T, Ts...> pack) {}
    DisplayComponentsStruct() = delete;
    DisplayComponentsStruct(Entity& entity)
    {
        ImGui::TableNextColumn();
        DisplayNext<T, Ts...>(entity);
    }
private:
    template<typename T1, typename... T1s>
    void DisplayNext(Entity& entity)
    {
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

        if constexpr (SingleComponentTypes::Has<T1>()) {
            if (curr_scene.Has<T1>(entity)) {
                //dont display tag component as it is already on top of the inspector
                if constexpr (!std::is_same<T1, Tag>())
                {   
                    auto& component = curr_scene.Get<T1>(entity);
                    if (component.state != DELETED)
                        DisplayComponentHelper(component);
                }              
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>()) {

            auto components = curr_scene.GetMulti<T1>(entity);
            for (T1* component : components){
                if (component->state != DELETED)
                    DisplayComponentHelper(*component);
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            DisplayNext<T1s...>(entity);
        }
    }
};
using DisplayAllComponentsStruct = decltype(DisplayComponentsStruct(AllComponentTypes()));

void DisplayComponents(Entity& entity) { DisplayAllComponentsStruct obj{ entity }; }

//Lists out all available components to add in the add component panel
template<typename T, typename... Ts>
struct AddsStruct
{
public:
    constexpr AddsStruct(TemplatePack<T, Ts...> pack) {}
    AddsStruct(Entity& entity)
    {
        AddNext<T, Ts...>(entity,MySceneManager.GetCurrentScene());
    }
private:
    template<typename T1, typename... T1s>
    void AddNext(Entity& entity, Scene& scene)
    {
        if constexpr (SingleComponentTypes::Has<T1>()) {
            if (!scene.Has<T1>(entity))
            {
               
                if (CENTERED_CONTROL(ImGui::Button(GetType::Name<T1>(), ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
                {
                    T1* pObject =  scene.Add<T1>(entity);                   

                    if constexpr (std::is_same<T1, BoxCollider>())
                    {
                        geometryDebugData temp;
                        if (scene.Has<MeshRenderer>(entity))
                        {
                            MeshRenderer& mr = scene.Get<MeshRenderer>(entity);

                            temp = MESHMANAGER.offsetAndBoundContainer.find(mr.meshID)->second;
                        }
                        else
                        {
                            temp = MESHMANAGER.offsetAndBoundContainer.find(DEFAULT_MESH)->second;
                        }

                        pObject->x = temp.scalarBound.x;
                        pObject->y = temp.scalarBound.y;
                        pObject->z = temp.scalarBound.z;
                        pObject->offset = temp.offset;
                    }

                    Change newchange(pObject);
                    newchange.action = CREATING;
                    EDITOR.History.AddComponentChange(newchange);

                    EditorInspector::Instance().isAddComponentPanel = false;
                }
            }
        }
        else
        {
            if constexpr (std::is_same_v<T1, Script>)
            {
                GetAssetsEvent<ScriptAsset> e;
                EVENTS.Publish(&e);

                for (auto& pair : *e.pAssets)
                {
                    if (CENTERED_CONTROL(ImGui::Button(pair.second.mFilePath.stem().string().c_str(), ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
                    {                      
                        T1* comp = scene.Add<T1>(entity, pair.first);
                        Change newchange(comp);
                        newchange.action = CREATING;
                        EDITOR.History.AddComponentChange(newchange);
                        EditorInspector::Instance().isAddComponentPanel = false;
                    }
                }
            }
            else
            {
                if (CENTERED_CONTROL(ImGui::Button(GetType::Name<T1>(), ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
                {
                    T1* comp = scene.Add<T1>(entity);
                    Change newchange(comp);
                    newchange.action = CREATING;
                    EDITOR.History.AddComponentChange(newchange);
                    EditorInspector::Instance().isAddComponentPanel = false;
                }
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            AddNext<T1s...>(entity,scene);
        }
    }
};
using AddsDisplay = decltype(AddsStruct(DisplayableComponentTypes()));

//Implementation for the panel to add a component to the current entity
void AddComponentPanel(Entity& entity) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 500));

    //press esc to exit add component window
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        EditorInspector::Instance().isAddComponentPanel = false;
    }


    ImGui::OpenPopup("Add Component");
    if (ImGui::BeginPopupModal("Add Component", &EditorInspector::Instance().isAddComponentPanel, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {

        (void)AddsDisplay(entity);
        ImGui::EndPopup();
    }
}

void AddTagPanel() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 500));

    //press esc to exit add component window
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        EditorInspector::Instance().isAddTagPanel = false;
    }
    ImGui::OpenPopup("Tags");
    if (ImGui::BeginPopupModal("Tags", &EditorInspector::Instance().isAddTagPanel, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {


        Tags& tags = IDENTIFIERS.GetTags();

        if (ImGui::BeginTable("Tags", 3)) {
            ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthFixed, 250.0f);
            float cellpadding = 15.f;
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(cellpadding, cellpadding));
            int i = 0;
            for (auto& tag : tags) {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                std::string tag_id = "Tag " + std::to_string(i++);
                ImGui::Text(tag_id.c_str());

                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                if (tag.second == -1) //deleted tag
                    ImGui::Text("(Removed)");
                else
                    ImGui::Text(tag.first.c_str());

                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                std::string tag_name = tag.first;
                if (tag_name != "Untagged") {
                    ImGui::PushID(i);
                    if (ImGui::Button("-")) {
                        IDENTIFIERS.DeleteTag(tag.first);
                    }
                    ImGui::PopID();
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
        std::string newname;
        ImGui::Text("New Tag Name"); ImGui::SameLine();
        ImGui::InputText("##tag", &newname);
        if (CENTERED_CONTROL(ImGui::Button("Add"))) {
            IDENTIFIERS.CreateTag(newname);
        };
        ImGui::EndPopup();
    }
}

void AddLayerPanel() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(480, 680));

    //press esc to exit add component window
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        EditorInspector::Instance().isAddLayerPanel = false;
    }
    ImGui::OpenPopup("Layers");
    if (ImGui::BeginPopupModal("Layers", &EditorInspector::Instance().isAddLayerPanel, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        std::string newname;
        /*ImGui::Text("New Layer Name"); ImGui::SameLine();
        ImGui::InputText("##layer", &newname);*/
        if (ImGui::BeginTable("Layers", 2)) {
            ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            float cellpadding = 15.f;
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(cellpadding, cellpadding));
            for (int i = 0; i < MAX_PHYSICS_LAYERS; i++) {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                std::string layer = "Layer " + std::to_string(i);      
                if (i <= 4)//default layers
                    layer += " (Default)";
                ImGui::Text(layer.c_str());

                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                std::string label = "##" + layer;
                std::string& layername = IDENTIFIERS.physicsLayers[i].name;
                ImGui::SetNextItemWidth(-FLT_MIN);
                if(i <= 4)
                    ImGui::Text(layername.c_str());
                else
                    ImGui::InputText(label.c_str(), &layername);
                ImGui::TableNextRow();
            }
            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
        
        /*if (CENTERED_CONTROL(ImGui::Button("Add"))) {
            IDENTIFIERS.CreateLayer(newname);
            EditorInspector::Instance().isAddLayerPanel = false;
        };*/
        ImGui::EndPopup();
    }
}

void DisplayLayers(Entity& entity) {
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    //Entity current tag
    auto& tag = curr_scene.Get<Tag>(entity);

    std::vector<const char*> layers;
    int i = 0;
    for (auto& it : IDENTIFIERS.physicsLayers) {
        if (it.name.empty()) {
            i++;
            continue;
        }        
        layers.push_back(it.name.c_str());
    }

    int index = (int)tag.physicsLayerIndex;
    ImGui::Text("Layer"); ImGui::SameLine();
    ImGui::PushItemWidth(100.f);
    Change change(&tag, "Tag/Layer Index");
    if (ImGui::Combo("##Layer", &index, layers.data(), (int)layers.size(), 5))
    {
        size_t buf = (size_t)index;
        EDITOR.History.SetPropertyValue(change, tag.physicsLayerIndex, buf);
    }
        //tag.physicsLayerIndex = index;
    ImGui::PopItemWidth();
}

void DisplayTags(Entity& entity) {
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    //Entity current tag
    auto& tag = curr_scene.Get<Tag>(entity);

    //tags in the project
    auto& Tags = IDENTIFIERS.GetTags();

    std::vector<const char*> layers;
    int i = 0;
    int index = 0; //default
    bool found = false;

    for (auto& it : Tags) {
        if (it.second == tag.tagName) {
            index = i;
        }
        layers.push_back(it.first.c_str());
        i++;
    }

    ImGui::Text("Tags"); ImGui::SameLine();
    ImGui::PushItemWidth(100.f);
    Change change(&tag, "Tag/Tag Name");
    if(ImGui::Combo("##Tags", &index, layers.data(), (int)layers.size(), 5)){

        Engine::UUID buf = Tags[layers[index]];
        EDITOR.History.SetPropertyValue(change, tag.tagName, buf);
        //tag.tagName = Tags[layers[index]];
    }

    ImGui::PopItemWidth();
}

//Display all the components as well as the name and whether the entity is enabled in the scene.
void DisplayEntity(Entity& entity)
{
    ImGui::PushID((int)entity.EUID());

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    bool enabled = curr_scene.IsActive(entity);
    ImGui::Checkbox("##Active", &enabled);
    curr_scene.SetActive(entity, enabled);
    ImGui::SameLine();
    static char buffer[256];

    auto& tag = curr_scene.Get<Tag>(entity);
    Change change (&tag, "Tag/name");

    strcpy_s(buffer, tag.name.c_str());
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##gameObjName", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::string buf = buffer;
        EDITOR.History.SetPropertyValue(change, tag.name, buf);
    }

    ImGui::PopItemWidth();
  
    //display tags
    DisplayTags(entity);
    ImGui::SameLine();
    ImGui::PushID(1);
    if(ImGui::Button("+")){ EditorInspector::Instance().isAddTagPanel = true; }
    ImGui::PopID();
    ImGui::SameLine(); ImGui::Dummy(ImVec2(22.f, 0.f)); ImGui::SameLine();
    
    //display layers
    DisplayLayers(entity); ImGui::SameLine();
    if (ImGui::Button("+")) { EditorInspector::Instance().isAddLayerPanel = true; }

   
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

    if (ImGui::BeginTable("Components", 1, tableFlags))
    {
        
        DisplayComponents(entity);
        
        ImGui::Separator();
        if (CENTERED_CONTROL(ImGui::Button("Add Component", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetTextLineHeightWithSpacing())))) {
            EditorInspector::Instance().isAddComponentPanel = true;
        }

        ImGui::EndTable();
        
    }
    ImGui::PopID();
    ImGui::PopStyleVar();
}

void EditorInspector::Init()
{
    isAddComponentPanel = false;
    //default layers (same as unity)
    IDENTIFIERS.physicsLayers[0] = Layer("Default");
    IDENTIFIERS.physicsLayers[1] = Layer("TransparentFX");
    IDENTIFIERS.physicsLayers[2] = Layer("Ignore Physics");
    IDENTIFIERS.physicsLayers[3] = Layer("UI");
    IDENTIFIERS.physicsLayers[4] = Layer("Water");
    IDENTIFIERS.physicsLayers[5] = Layer("NavMesh");

    //create default tag
    IDENTIFIERS.GetTags()["Untagged"] = Engine::CreateUUID();
    IDENTIFIERS.GetTags()["Test"] = Engine::CreateUUID();
    IDENTIFIERS.GetTags()["Test2"] = Engine::CreateUUID();
}

void EditorInspector::Update(float dt)
{
    UNREFERENCED_PARAMETER(dt);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
    ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin("Inspector");
    //Get Selected Entities from mouse picking
    //List out all components in order
    //templated functionalities (input fields, checkboxes etc.)

    Engine::UUID curr_index = EditorHierarchy::Instance().selectedEntity;

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    Entity& curr_entity = curr_scene.Get<Entity>(curr_index);
    //if (curr_index != NON_VALID_ENTITY) {
    if (&curr_entity) {
        ImGui::Spacing();
        std::string Header = "Current Entity: " + curr_scene.Get<Tag>(curr_index).name;
        ImGui::Text(Header.c_str()); ImGui::Spacing(); ImGui::Separator();
        DisplayEntity(curr_entity);
    }

    if (isAddComponentPanel) {
        AddComponentPanel(curr_scene.Get<Entity>(curr_index));
    }

    if (material_inspector) {

        ImGui::Begin("Material", &material_inspector);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 2));
        
        //auto& material = MATERIALSYSTEM._material[SHADERTYPE::PBR][mat_id];

        ImGuiWindowFlags tableflags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchProp
            | ImGuiTableFlags_PadOuterX;

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(450.f, 600.f));

        auto& material = MATERIALSYSTEM.getMaterialInstance(EditorContentBrowser::Instance().selectedAss);

        if (ImGui::BeginTable("Mats", 2, tableflags))
        {
            ImGui::Indent();
            ImGui::TableSetupColumn("Text", 0, 0.4f);
            ImGui::TableSetupColumn("Input", 0, 0.6f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 0));

            property::SerializeEnum(material, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
                {
                    if (!Flags.m_isDontShow) {
                        auto entry = property::entry { PropertyName, Data };
                        std::visit([&](auto& Value) {

                            using T1 = std::decay_t<decltype(Value)>;

                            //Edit name
                            std::string DisplayName = entry.first;
                            
                            auto it = DisplayName.begin() + DisplayName.find_last_of("/");
                            DisplayName.erase(DisplayName.begin(), ++it);
                            Change newchange(&material, entry.first);
                            ImGui::PushID(entry.first.c_str());

                            if (entry.first.find("Shader") != std::string::npos) {
                                DisplayShaders(newchange, Value);
                            }
                            else {                            
                                 Display<T1>(newchange, DisplayName.c_str(), Value);     
                            }

                            ImGui::PopID();
                            }
                        , Data);
                        property::set(material, entry.first.c_str(), Data);

                        // If we are dealing with a scope that is not an array someone may have change the SerializeEnum to a DisplayEnum they only show up there.
                        //assert(Flags.m_isScope == false || PropertyName.back() == ']');
                    }

                });

            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();

            ImGui::Unindent();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();

        ImGui::Separator();

        //if (CENTERED_CONTROL(ImGui::Button("Duplicate Material", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetTextLineHeightWithSpacing()))))
        //{
        //    Engine::UUID curr_index = EditorHierarchy::Instance().selectedEntity;
        //    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
        //    MeshRenderer& rend = curr_scene.Get<MeshRenderer>(curr_index);
        //    //Material_instance& new_mat = MATERIALSYSTEM.DuplicateMaterial(material);
        //    
        //    Change newchange(&rend, "MeshRenderer/material");
        //    //int new_id = (int)MATERIALSYSTEM._material[SHADERTYPE::PBR].size()-1;
        //    //EDITOR.History.SetPropertyValue(newchange, rend.material, new_id);
        //}

        if (CENTERED_CONTROL(ImGui::Button("Save Material", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetTextLineHeightWithSpacing()))))
        {
            GetFilePathGenericEvent e{ EditorContentBrowser::Instance().selectedAss };
            EVENTS.Publish(&e);
            std::string fPathStr{ e.filePath.string() };
            size_t strPos = fPathStr.find_last_of("\\");
            if (strPos == std::string::npos)
                strPos = fPathStr.find_last_of("/");
            fs::path fPath;
            if (strPos != std::string::npos)
            {
                fPathStr = std::string(fPathStr.begin(), fPathStr.begin() + strPos);
                fPath = fPathStr;
            }
            if (!fPath.empty())
            {
                std::string directory = fPathStr;
                fPath += "\\";
                fPath += material.name.c_str();
                fPath += ".material";
                fs::rename(e.filePath, fPath);
                Serialize(material, fPathStr);
            }
            else
            {
                fPath = "Assets\\";
                fPath += material.name.c_str();
                fPath += ".material";
                Serialize(material);
                GetAssetEvent pathEvent(fPath);
                EVENTS.Publish(&pathEvent);
                EditorContentBrowser::Instance().selectedAss = e.guid;
            }
            //PRINT(fPath);
            //material.name
        }

        ImGui::End();
    }

    if (isAddTagPanel) {
        AddTagPanel();
    }

    if (isAddLayerPanel) {
        AddLayerPanel();
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void EditorInspector::Exit()
{

}