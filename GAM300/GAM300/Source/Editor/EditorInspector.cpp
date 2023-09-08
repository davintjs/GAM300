/*!***************************************************************************************
\file			EditorInspector.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. Editor Inspector

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "EditorTemplates.h";

#define TEXT_BUFFER_SIZE 128

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
    ImGui::SameLine();
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
    ImGui::Text("Active");
    Display("Position", transform.translation);
    Display("Rotation", transform.rotation);
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


void DisplayComponents(const Entity& entity) {

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_Framed;


    if (curr_scene.HasComponent<Transform>(entity)) {
        if (ImGui::TreeNodeEx("Transform", NodeFlags)) {
            DisplayComponent(curr_scene.GetComponent<Transform>(entity));
            ImGui::TreePop();
            ImGui::Separator();
        }
    }

    if (curr_scene.HasComponent<Tag>(entity)) {
        if (ImGui::TreeNodeEx("Tag", NodeFlags)) {
            DisplayComponent(curr_scene.GetComponent<Tag>(entity));
            ImGui::TreePop();
            ImGui::Separator();
        }
    }

    /*if (curr_scene.HasComponent<Tag>(entity)) {
        if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
            std::cout << "ok2\n";
            DisplayComponent(curr_scene.GetComponent<Tag>(entity));
            ImGui::TreePop();
            ImGui::Separator();
        }     
    }*/

    if (curr_scene.HasComponent<Rigidbody>(entity)) {
        if (ImGui::TreeNodeEx("Rigidbody", NodeFlags)) {
            DisplayComponent(curr_scene.GetComponent<Rigidbody>(entity));
            ImGui::TreePop();
            ImGui::Separator();
        }
    }
    
   /* if (curr_scene.HasComponent<Rigidbody>(entity)) {
        bool open = ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed);
        if (open) {
            std::cout << "ok3\n";
            DisplayComponent(curr_scene.GetComponent<Rigidbody>(entity));
            ImGui::TreePop();
            ImGui::Separator();
        }
    }*/
}

void DisplayEntity(Entity& entity)
{
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    ImGui::Checkbox("##Active", &entity.is_enabled);
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

    /*ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH
        | ImGuiTableFlags_ScrollY;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);*/
    /*if (ImGui::BeginTable("Components", 1, tableFlags, ImVec2(0.f, ImGui::GetWindowSize().y * 0.8f)))
   {
       ImGui::PushID((int)entity.uuid);
       DisplayComponents(entity);
       ImGui::PopID();
       ImGui::EndTable();
   }
   ImGui::PopStyleVar();*/

    DisplayComponents(entity);

   

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

}

void EditorInspector::Update(float dt)
{
    ImGui::Begin("Inspector");
    //Get Selected Entities from mouse picking
    //List out all components in order
    //templated functionalities (input fields, checkboxes etc.)

    const ObjectIndex curr_index = EditorHierarchy::Instance().selectedEntity;

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();


    if (curr_index != NON_VALID_ENTITY) {
        Entity& curr_entity = curr_scene.entities.DenseSubscript(curr_index);
        std::string Header = "Current Entity: " + curr_scene.GetComponent<Tag>(curr_entity).name;
        ImGui::Text(Header.c_str()); ImGui::Spacing(); ImGui::Separator();
        DisplayEntity(curr_entity);

        ImGui::Spacing();
        if (CENTERED_CONTROL(ImGui::Button("Add Component"))) {

        }
    }
  

    ImGui::End();
}

void EditorInspector::Exit()
{

}