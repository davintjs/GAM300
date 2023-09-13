/*!***************************************************************************************
\file			EditorInspector.cpp
\project		GAM300
\author

\par			Course: GAM300
\date           00/00/2023

\brief
    This file contains the definitions of the following:
    1.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "EditorTemplates.h";
#include "Scene/Components.h"

#define TEXT_BUFFER_SIZE 2048

static ImGuiTableFlags windowFlags =
ImGuiTableFlags_Resizable |
ImGuiTableFlags_NoBordersInBody |
ImGuiTableFlags_NoSavedSettings |
ImGuiTableFlags_SizingStretchProp;

template <typename T>
void Display(const char* name, T& val);

void DisplayType(const char* name, bool& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    ImGui::Checkbox(idName.c_str(), &val);
}

void DisplayType(const char* name, std::string& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    static char buffer[2048];
    strcpy_s(buffer, val.c_str());
    ImGui::InputText(idName.c_str(), buffer,2048);
}

void DisplayType(const char* name, int& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    ImGui::DragInt(idName.c_str(), &val);
}

template <size_t SZ>
void DisplayType(const char* name, char(&val)[SZ])
{
    static std::string idName{};
    idName = "##";
    idName += name;
    ImGui::InputTextMultiline(idName.c_str(), val, SZ, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
}

void DisplayType(const char* name, char*& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    ImGui::InputTextMultiline(idName.c_str(), val, TEXT_BUFFER_SIZE, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
}

void DisplayType(const char* name, float& val)
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
    ImGui::DragFloat(cIdName, &val, 0.15f);
}

void DisplayType(const char* name, double& val)
{
    static std::string idName{};
    idName = "##";
    idName += name;
    float temp{ (float)val };
    ImGui::DragFloat(idName.c_str(), &temp, 0.15f);
    val = temp;
}

void DisplayType(const char* name, Vector3& val)
{
    static float temp{};
    static std::string idName{};
    idName = "##";
    idName += name;
    if (ImGui::BeginTable("Vector3", 3, windowFlags))
    {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        idName += 'X';
        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        DisplayType(idName.c_str(), val.x);

        ImGui::TableNextColumn();
        idName.back() = 'Y';
        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        DisplayType(idName.c_str(), val.y);

        ImGui::TableNextColumn();
        idName.back() = 'Z';
        ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        DisplayType(idName.c_str(), val.z);
        ImGui::EndTable();
    }
}

void DisplayType(const char* name, Vector2& val)
{
    static float temp{};
    static std::string idName{};
    idName = "##";
    idName += name;
    if (ImGui::BeginTable("Vector2", 2, windowFlags))
    {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        idName += 'X';
        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        DisplayType(idName.c_str(), val.x);

        ImGui::TableNextColumn();
        idName.back() = 'Y';
        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        DisplayType(idName.c_str(), val.y);

        ImGui::EndTable();
    }
}

//void DisplayType(const char* name, AABB& val)
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//
//    if (ImGui::BeginTable("AABB", 2, windowFlags))
//    {
//        ImGui::AlignTextToFramePadding();
//        Display("Max", val.max);
//        Display("Min", val.min);
//        ImGui::EndTable();
//    }
//}


template <typename T>
void Display(const char* name, T& val)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    DisplayType(name, val);
}

void Display(const char* name, Script*& val, const char* scriptName)
{
    ImGui::AlignTextToFramePadding();
    ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();
    //DisplayType(name, val, scriptName);
}

void Display(const char* string)
{
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text(string);
}

template <typename T>
void DisplayComponent(T& component)
{
    //PRINT("Component of type: " << GetComponentType<T>::name << " does not exist yet! ");
}

template <>
void DisplayComponent<Transform>(Transform& transform)
{
    //ImGui::Checkbox("##Active", &transform.is_enabled); ImGui::SameLine();
    //ImGui::Text("Active");
    Display("Position", transform.translation);
    glm::vec3 rotation = glm::degrees(transform.rotation);
    Display("Rotation", rotation);
    transform.rotation = glm::radians(rotation);
    Display("Scale", transform.scale);
}

template <>
void DisplayComponent<BoxCollider>(BoxCollider& boxCollider2D)
{
    //Display("Bounds", boxCollider2D.bounds);
}

template <>
void DisplayComponent<Rigidbody>(Rigidbody& rb)
{

    //DisplayDragDrop();
    //spriteRenderer.sprite.set_name()
    //ImGui::Checkbox("##Active", &rb.is_enabled);
    ImGui::SameLine();
    ImGui::Text("Active");
    Display("Mass", rb.mass);
    Display("Velocity", rb.velocity);
    Display("Acceleration", rb.acceleration);
    Display("Force", rb.force);
    Display("Use Gravity", rb.useGravity);
    Display("Is Kinematic", rb.isKinematic);
}

template <>
void DisplayComponent<Tag>(Tag& tag)
{
    //DisplayDragDrop();
    //spriteRenderer.sprite.set_name()
    Display("Entity Name", tag.name);
}

template<>
void DisplayComponent<AudioSource>(AudioSource& as) {
    Display("Loop", as.loop);
    Display("Volume", as.volume);
}

//template <>
//void DisplayComponent<SpriteRenderer>(SpriteRenderer& spriteRenderer)
//{
//
//    //DisplayDragDrop();
//    //spriteRenderer.sprite.set_name()
//    Display("Sprite", spriteRenderer.sprite.refTexture);
//    DisplayColor("Color", spriteRenderer.sprite.color);
//
//    ImGui::TableNextColumn();
//    ImGui::Text("Flip");
//    ImGui::TableNextColumn();
//    ImGui::Checkbox("X", &spriteRenderer.sprite.flip.x);
//    ImGui::SameLine(0.f, 16.f);
//    ImGui::Checkbox("Y", &spriteRenderer.sprite.flip.y);
//
//    if (spriteRenderer.sprite.refTexture)
//    {
//        std::string filePath = spriteRenderer.sprite.refTexture->get_file_path();
//        uint64_t pathID = std::hash<std::string>{}(filePath);
//        MetaID metaID = MyAssetSystem.GetMetaID(pathID);
//        spriteRenderer.sprite.spriteID = metaID.uuid;
//        size_t pos = filePath.find_last_of('\\');
//        spriteRenderer.sprite.sprite_name = filePath.substr(pos + 1, filePath.length() - pos);
//    }
//}

//template <>
//void DisplayComponent<Text>(Text& text)
//{
//    Display("Font", text.font);
//    text.fontName = text.font->GetName();
//
//    Display("Font Size", text.fSize);
//    Display("Content", text.content);
//    Display("Wrapping", text.wrapper);
//
//    ImGui::TableNextColumn();
//    ImGui::AlignTextToFramePadding();
//    ImGui::Text("Horizontal Alignment");
//    ImGui::TableNextColumn();
//    static const char* const horizontal[] = { "Left", "Center", "Right" };
//    ImGui::PushItemWidth(-1);
//    ImGui::Combo("hAlign", reinterpret_cast<int*>(&text.hAlignment), horizontal, 3);
//    ImGui::PopItemWidth();
//
//    ImGui::TableNextColumn();
//    ImGui::AlignTextToFramePadding();
//    ImGui::Text("Vertical Alignment");
//    ImGui::TableNextColumn();
//    static const char* const vertical[] = { "Top", "Center", "Bottom" };
//    ImGui::PushItemWidth(-1);
//    ImGui::Combo("vAlign", reinterpret_cast<int*>(&text.vAlignment), vertical, 3);
//    ImGui::PopItemWidth();
//
//    DisplayColor("Color", text.color);
//    //DisplayDragDrop();
//    //spriteRenderer.sprite.set_name()
//}

//template <>
//void DisplayComponent<Script>(Script& script)
//{
//    static Field buffer(FieldType::None, 128);
//    for (auto pair : script.fieldDataReferences)
//    {
//        const char* name = pair.first.c_str();
//        Field& field{ pair.second };
//        //    //Component Enum + ComponentType Enum
//        if (field.fType >= FieldType::Component)
//        {
//            ComponentType cType = (ComponentType)field.fType;
//            switch (cType) {
//            case ComponentType::Animator:
//                Display(name, (Animator*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Animator*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::AudioSource:
//                Display(name, (AudioSource*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (AudioSource*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::BoxCollider2D:
//                Display(name, (BoxCollider2D*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (BoxCollider2D*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Button:
//                Display(name, (Button*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Button*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Camera:
//                Display(name, (Camera*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Camera*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Image:
//                Display(name, (Image*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Image*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Rigidbody2D:
//                Display(name, (Rigidbody2D*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Rigidbody2D*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::SpriteRenderer:
//                Display(name, (SpriteRenderer*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (SpriteRenderer*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Script:
//                Display(name, (Script*&)script.fieldComponentReferences[name], field.typeName.c_str());
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Script*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Transform:
//                Display(name, (Transform*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Transform*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::Text:
//                Display(name, (Text*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (Text*)script.fieldComponentReferences[name]));
//                break;
//            case ComponentType::SortingGroup:
//                Display(name, (SortingGroup*&)script.fieldComponentReferences[name]);
//                MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, (SortingGroup*)script.fieldComponentReferences[name]));
//                break;
//            default:
//                // handle invalid case
//                break;
//            }
//        }
//        else if (field.fType == FieldType::GameObject)
//        {
//            Display(name, script.fieldGameObjReferences[name]);
//            MyEventSystem->publish(new ScriptSetFieldReferenceEvent(script, name, script.fieldGameObjReferences[name]));
//        }
//        else
//        {
//            buffer.fType = field.fType;
//            MyEventSystem->publish(new ScriptGetFieldEvent(script, name, field.data));
//            Display(name, field);
//            MyEventSystem->publish(new ScriptSetFieldEvent(script, name, field.data));
//        }
//    }
//}
//template <>
//void DisplayComponent<Image>(Image& image)
//{
//    Display("Image", image.sprite.refTexture);
//    DisplayColor("Color", image.sprite.color);
//
//    ImGui::TableNextColumn();
//    ImGui::Text("Flip");
//    ImGui::TableNextColumn();
//    ImGui::Checkbox("X", &image.sprite.flip.x);
//    ImGui::SameLine(0.f, 16.f);
//    ImGui::Checkbox("Y", &image.sprite.flip.y);
//
//    //Update sprite data
//    if (image.sprite.refTexture)
//    {
//        std::string filePath = image.sprite.refTexture->get_file_path();
//        uint64_t pathID = std::hash<std::string>{}(filePath);
//        MetaID metaID = MyAssetSystem.GetMetaID(pathID);
//        image.sprite.spriteID = metaID.uuid;
//        size_t pos = filePath.find_last_of('\\');
//        image.sprite.sprite_name = filePath.substr(pos + 1, filePath.length() - pos);
//    }
//}

template <typename T>
void DisplayComponentHelper(T& component)
{
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth;
    static std::string name{};
    if constexpr (std::is_same<T, Script>())
    {
        name = (component.name + " [Script]");
    }
    else if constexpr (AllComponentTypes::Has<T>())
    {
        name = GetComponentType::Name<T>();
    }
    else
    {
        //This means T is not a component
        PRINT(typeid(T).name());
    }
    if (ImGui::CollapsingHeader(name.c_str(), nodeFlags))
    {
        /*if (ImGui::BeginDragDropSource())
        {
            void* container = &component;
            if constexpr (std::is_same<T, Script>())
            {
                ImGui::SetDragDropPayload(component.Name().c_str(), &container, sizeof(void*));
                ImGui::EndDragDropSource();
            }
            else
            {
                ImGui::SetDragDropPayload(GetTypeName<T>(), &container, sizeof(void*));
                ImGui::EndDragDropSource();
            }
        }*/

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

            DisplayComponent(component);

            //To do: Deletion of component in inspector
            //ImGui::Spacing();
            //if (CENTERED_CONTROL(ImGui::Button("Delete ", ImVec2(ImGui::GetWindowSize().x * 0.3, ImGui::GetTextLineHeightWithSpacing()))))
            //{
            //    //To do: delete component, get container of all component types and compare,
            //    PRINT("DELETING");
            //}

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


template<typename T, typename... Ts>
struct DisplayComponentsStruct
{
public:
    constexpr DisplayComponentsStruct(TemplatePack<T, Ts...> pack) {}
    DisplayComponentsStruct() = delete;
    DisplayComponentsStruct(Entity& entity)
    {
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
        ImGui::TableNextColumn();
        //DisplayComponentHelper(curr_scene.GetComponent<Transform>(entity));

        DisplayNext<T, Ts...>(entity);
    }
private:
    template<typename T1, typename... T1s>
    void DisplayNext(Entity& entity)
    {
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

        if constexpr (SingleComponentTypes::Has<T1>()) {
            if (curr_scene.HasComponent<T1>(entity)) {
                //dont display tag component as it is already on top of the inspector
                if constexpr (!std::is_same<T1, Tag>())
                {
                    auto& component = curr_scene.GetComponent<T1>(entity);
                    DisplayComponentHelper(component);
                }              
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>()) {

            auto components = curr_scene.GetComponents<T1>(entity);
            for (T1* component : components)
            {
                Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
                //DisplayType("Enabled", component->is_enabled); ImGui::SameLine();
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

template<typename T, typename... Ts>
struct AddComponentsStruct
{
public:
    constexpr AddComponentsStruct(TemplatePack<T, Ts...> pack) {}
    AddComponentsStruct(Entity& entity)
    {
        AddNext<T, Ts...>(entity);
    }
private:
    template<typename T1, typename... T1s>
    void AddNext(Entity& entity)
    {
        if constexpr (SingleComponentTypes::Has<T1>()) {
            if (!entity.pScene->HasComponent<T1>(entity))
            {
                if (CENTERED_CONTROL(ImGui::Button(GetComponentType::Name<T1>(), ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
                {
                    entity.pScene->AddComponent<T1>(entity);
                    EditorInspector::Instance().isAddComponentPanel = false;
                }
            }
        }
        else
        {
            if (CENTERED_CONTROL(ImGui::Button(GetComponentType::Name<T1>(), ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
            {
                entity.pScene->AddComponent<T1>(entity);
                EditorInspector::Instance().isAddComponentPanel = false;
            }
        }

        if constexpr (sizeof...(T1s) != 0)
        {
            AddNext<T1s...>(entity);
        }
    }
};
using AddComponentsDisplay = decltype(AddComponentsStruct(DisplayableComponentTypes()));

void AddComponentPanel(Entity& entity) {
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 500));

    //press esc to exit add component window
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        EditorInspector::Instance().isAddComponentPanel = false;
    }
    ImGui::OpenPopup("Add Component");
    if (ImGui::BeginPopupModal("Add Component", &EditorInspector::Instance().isAddComponentPanel, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {

        (void)AddComponentsDisplay(entity);
        ImGui::EndPopup();
    }
}

void DisplayEntity(Entity& entity)
{
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    bool enabled = curr_scene.IsActive(entity);
    ImGui::Checkbox("##Active", &enabled);
    curr_scene.SetActive(entity, enabled);
    ImGui::SameLine();
    static char buffer[256];
    std::string entity_name = curr_scene.GetComponent<Tag>(entity).name;
    strcpy_s(buffer, entity_name.c_str());
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##gameObjName", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
        curr_scene.GetComponent<Tag>(entity).name = buffer;
    }

    ImGui::PopItemWidth();
    curr_scene.GetComponent<Tag>(entity).name = buffer;

    ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH
        | ImGuiTableFlags_ScrollY;


    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

    if (ImGui::BeginTable("Components", 1, tableFlags))
    {
        ImGui::PushID((int)entity.uuid);
        DisplayComponents(entity);
        ImGui::PopID();
        ImGui::Separator();
        if (CENTERED_CONTROL(ImGui::Button("Add Component", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetTextLineHeightWithSpacing())))) {
            EditorInspector::Instance().isAddComponentPanel = true;
        }

        ImGui::EndTable();
        
    }
    ImGui::PopStyleVar();

    /*static const float buttonSizeY = ImGui::CalcTextSize("Add Component").y;
    ImVec2 buttonSize(ImGui::GetWindowSize().x, buttonSizeY * 2);;
    ImGui::SetCursorPosY(ImGui::GetWindowSize().y - buttonSize.y);*/

    /*if (ImGui::Button("Add Component", buttonSize)) {
        isAddingComponent = true;
    }
    AddComponentPanel(gameObject, isAddingComponent);*/
}

void EditorInspector::Init()
{
    isAddComponentPanel = false;
}

void EditorInspector::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
    ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin("Inspector");
    //Get Selected Entities from mouse picking
    //List out all components in order
    //templated functionalities (input fields, checkboxes etc.)

    const ObjectIndex curr_index = EditorHierarchy::Instance().selectedEntity;

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();


    if (curr_index != NON_VALID_ENTITY) {
        ImGui::Spacing();
        Entity& curr_entity = curr_scene.entities.DenseSubscript(curr_index);
        std::string Header = "Current Entity: " + curr_scene.GetComponent<Tag>(curr_entity).name;
        ImGui::Text(Header.c_str()); ImGui::Spacing(); ImGui::Separator();
        DisplayEntity(curr_entity);
    }

    if (isAddComponentPanel) {
        AddComponentPanel(curr_scene.entities.DenseSubscript(curr_index));
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void EditorInspector::Exit()
{

}