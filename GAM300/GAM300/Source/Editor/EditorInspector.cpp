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
#include "Graphics/MeshManager.h"
#include <variant>
#include "PropertyConfig.h"

#define BUTTON_HEIGHT .1 //Percent
#define BUTTON_WIDTH .6 //Percent
#define TEXT_BUFFER_SIZE 2048

static ImGuiTableFlags windowFlags =
ImGuiTableFlags_Resizable |
ImGuiTableFlags_NoBordersInBody |
ImGuiTableFlags_NoSavedSettings |
ImGuiTableFlags_SizingStretchProp;

bool isAddingReference = false;
void* pEditedContainer{ nullptr };

template <typename T>
void Display(const char* name, T& val);

template <typename T>
void DisplayType(const char* name, T& val)
{
    //PRINT(name," ", typeid(T).name(), '\n');
}



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

void DisplayType(const char* name, Vector4& val)
{
    //static_assert(sizeof(T) == sizeof(float) * 4);
    static float temp{};
    static std::string idName{};
    idName = "##";
    idName += name;

    //std::string var = name;
    //if (var.find("Albedo") != std::string::npos) {}
    ImGui::ColorEdit4("MyColor##2", (float*)&val, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine(); 
    /*ImGui::TableNextColumn();
    ImGui::Text(name);
    ImGui::TableNextColumn();*/
    //static ImGuiColorEditFlags miscFlags = ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview;

    if (ImGui::BeginTable("Color4", 4, windowFlags))
    {
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        idName += 'W';
        ImGui::Text("w"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
        DisplayType(idName.c_str(), val.w);

        ImGui::TableNextColumn();
        idName.back() = 'X';
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

    //ImVec4 color = ImVec4(val.w, val.x, val.y, val.z);


     //if (ImGui::ColorEdit4("MyColor##2", (float*)&color, miscFlags))
     //{
     //    //editedColor = reinterpret_cast<float*>(&val);
     //    val = color;
     //}
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

template <typename T>
void AddReferencePanel(T* container)
{
    //ZACH: If no one is adding reference or the container does not match
    if (!isAddingReference || (T*)pEditedContainer != container)
    if (!isAddingReference || (T*)pEditedContainer != container)
    {
        return;
    }
    Scene& scene = MySceneManager.GetCurrentScene();
    static ImGuiTextFilter filter;
    static std::string windowName;
    windowName = "Add ";
    windowName += GetType::Name<T>();
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100.f, 100.f), ImGuiCond_FirstUseEver);
    windowName += " Reference";
    if (ImGui::Begin(windowName.c_str(), &isAddingReference))
    {
        ImGui::PushItemWidth(-1);
        filter.Draw("##References");
        ImGui::PopItemWidth();
        static std::string buttonName{};
        for (T& object : scene.GetArray<T>())
        {
            ImVec2 buttonSize = ImGui::GetWindowSize();
            buttonSize.y *= (float)BUTTON_HEIGHT;
            Tag& tag = scene.Get<Tag>(object);
            buttonName = tag.name;
            if (filter.PassFilter(tag.name.c_str()) && ImGui::Button(buttonName.c_str(), buttonSize))
            {
                isAddingReference = false;
                Handle* value = (Handle*)container;
                *value = Handle(object.EUID(),object.UUID());
                break;
            }
        }
        ImGui::End();
    }
    //Reset the edited container back to false
    if (isAddingReference == false)
    {
        pEditedContainer = nullptr;
    }
}

template <typename T>
void DisplayType(const char* name, T* container, const char* altName = nullptr)
{
    if constexpr (AllObjectTypes::Has<T>())
    {
        static std::string btnName;
        Handle* value = (Handle*)container;
        if (value->euid != 0)
        {
            btnName = MySceneManager.GetCurrentScene().Get<Tag>(value->euid).name;
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
            pEditedContainer = reinterpret_cast<void*>(container);
        }
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetType::Name<T>());
            if (payload)
            {
                *value = *(Handle*)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        AddReferencePanel(container);
    }
}

template <typename T, typename... Ts>
void DisplayField(const char* name, Field& field)
{
    if (GetFieldType::E<T>() == field.fType)
    {
        if (field.fType < AllObjectTypes::Size())
        {
            T* value = reinterpret_cast<T*>(field.data);
            if constexpr (std::is_same<T, Script>())
            {
                DisplayType(name, value,field.typeName.c_str());
            }
            else
            {
                DisplayType(name, value);
            }
        }
        else
        {
            DisplayType(name, field.Get<T>());
        }
        return;
    }
    if constexpr (sizeof...(Ts) != 0)
    {
        DisplayField<Ts...>(name, field);
    }
}

template <typename T, typename... Ts>
void DisplayField(const char* name, Field& field, TemplatePack<T,Ts...>)
{
    DisplayField<T,Ts...>(name,field);
}

void DisplayType(const char* name, Field& val)
{
    DisplayField(name, val, AllFieldTypes());
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
void Display_Property(T& comp) {
    if constexpr (std::is_same<T, MeshRenderer>()) {
        //Combo field for mesh renderer
        ImGui::AlignTextToFramePadding();
        ImGui::TableNextColumn();
        ImGui::Text("MeshName");
        ImGui::TableNextColumn();
        std::vector<const char*> meshNames;
        int number = 0;
        bool found = false;
        for (auto& pair : MeshManager.mContainer)
        {
            if (pair.first == comp.MeshName)
                found = true;
            meshNames.push_back(pair.first.c_str());
            if (!found)
            {
                ++number;
            }
        }
        ImGui::PushItemWidth(-1);
        ImGui::Combo("Mesh Name", &number, meshNames.data(), meshNames.size(), 5);
        ImGui::PopItemWidth();
        comp.MeshName = meshNames[number];
    }
    std::vector<property::entry> List;
    property::SerializeEnum(comp, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
        {
            // If we are dealing with a scope that is not an array someone may have change the SerializeEnum to a DisplayEnum they only show up there.
            assert(Flags.m_isScope == false || PropertyName.back() == ']');
            List.push_back(property::entry { PropertyName, Data });
        });

    int id = 0;
    for (auto& [Name, Data] : List)
    {
        //find out how to use flags
        /*property::flags::type Flags;
        if (Flags.m_isDontShow) continue;*/

        if((Name.find("EUID") != std::string::npos) || (Name.find("UUID") != std::string::npos)) continue;

        std::visit([&](auto& Value) {
                using T1 = std::decay_t<decltype(Value)>;
                //Edit name
                std::string DisplayName = Name;
                auto it = DisplayName.begin() + DisplayName.find_last_of("/");
                DisplayName.erase(DisplayName.begin(), ++it);
                DisplayName[0] = toupper(DisplayName[0]); //Make first letter uppercase
                ImGui::PushID(Name.c_str());
                Display<T1>(DisplayName.c_str(), Value);

                //temporary implementation
                if (DisplayName == "AlbedoTexture") {
                    ImGui::SameLine();
                    ImGuiWindowFlags win_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar;
                    static const std::string AssetDirectory = "Assets/Icons";
                    static std::filesystem::path CurrentDirectory = AssetDirectory;
                    static std::string currentFolder = "Assets/Icons";

                    static std::string button_name = "Texture";

                    if (ImGui::Button(button_name.c_str())) {
                        ImGui::OpenPopup("Texture");
                    }

                    //Component Settings window
                    ImGui::SetNextWindowSize(ImVec2(250.f, 150.f));
                    if (ImGui::BeginPopup("Texture", win_flags)) {
                        static float padding = 15.f;
                        static float iconsize = 50.f;
                        float cellsize = iconsize + padding;

                        float window_width = ImGui::GetContentRegionAvail().x;
                        int columncount = (int)(window_width / cellsize);
                        if (columncount < 1) { columncount = 1; }

                        ImGui::Columns(columncount, 0, false);

                        int i = 0;
                        //using filesystem to iterate through all folders/files inside the "/Data" directory
                        for (auto& it : std::filesystem::directory_iterator{ CurrentDirectory })
                        {
                            const auto& path = it.path();
                            if (path.string().find("meta") != std::string::npos) continue;

                            ImGui::PushID(i++);

                            auto relativepath = std::filesystem::relative(path, AssetDirectory);
                            std::string pathStr = relativepath.filename().string();

                            //Draw the file / folder icon based on whether it is a directory or not
                            std::string icon = it.is_directory() ? "foldericon" : "fileicon";

                            size_t icon_id = 0;

                            std::string filename = relativepath.string();

                            auto it = filename.begin() + filename.find_first_of(".");
                            filename.erase(it, filename.end());
                            //PRINT(filename);
                            icon = filename;

                            //render respective file icon textures
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
                            icon_id = GET_TEXTURE_ID(icon);
                            ImGui::ImageButton((ImTextureID)icon_id, { iconsize, iconsize }, { 0 , 1 }, { 1 , 0 });

                            ImGui::PopStyleColor();

                            //Change directory into the folder clicked
                            /*if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                if (it.is_directory())
                                {
                                    currentFolder = pathStr;
                                    CurrentDirectory /= path.filename();
                                }
                            }*/

                            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                button_name = filename;
                            }

                            //render file name below icon
                            ImGui::TextWrapped(pathStr.c_str());
                            ImGui::NextColumn();
                            ImGui::PopID();
                        }
                        ImGui::Columns(1);
                        ImGui::EndPopup();
                    }

                }

                ImGui::PopID();          
            }
        , Data);
        property::set(comp, Name.c_str(), Data);
    }

}

//template <typename T>
//void DisplayComponent(T& component)
//{
    //PRINT("Component of type: " << typeid(component).name() << " does not exist yet! ");
    //DisplayProperties(component);
//}
////Cant use reflection system due to glm::vec3
//template <>
//void DisplayComponent<Transform>(Transform& transform)
//{
    //DisplayProperties(transform);
    //ImGui::Checkbox("##Active", &transform.is_enabled); ImGui::SameLine();
    //ImGui::Text("Active");
//}
//
//template <>
//void DisplayComponent<BoxCollider>(BoxCollider& boxCollider2D)
//{
//    //Display("Bounds", boxCollider2D.bounds);
//}
//template <>
//void DisplayComponent<Rigidbody>(Rigidbody& rb)
//{
//
//    //DisplayDragDrop();
//    //spriteRenderer.sprite.set_name()
//    //ImGui::Checkbox("##Active", &rb.is_enabled);
//    ImGui::SameLine();
//    ImGui::Text("Active");
//    Display("Mass", rb.mass);
//    Display("Linear Velocity", rb.linearVelocity);
//    Display("Angular Velocity", rb.angularVelocity);
//    Display("Force", rb.force);
//    Display("Use Gravity", rb.useGravity);
//    Display("Is Kinematic", rb.isKinematic);
//}
//template <>
//void DisplayComponent<Tag>(Tag& tag)
//{
//    //DisplayDragDrop();
//    //spriteRenderer.sprite.set_name()
//    Display("Entity Name", tag.name);
//}
//
//
//
//template <typename T>
//void Display_Property(T& comp) {
//
//    std::vector<property::entry> List;
//    property::SerializeEnum(comp, [&](std::string_view PropertyName, property::data&& Data, const property::table&, std::size_t, property::flags::type Flags)
//        {
//            // If we are dealing with a scope that is not an array someone may have change the SerializeEnum to a DisplayEnum they only show up there.
//            assert(Flags.m_isScope == false || PropertyName.back() == ']');
//            List.push_back(property::entry { PropertyName, Data });
//        });
//
//    for (auto& [Name, Data] : List)
//    {
//        std::visit([&](auto& Value) {
//
//            using T1 = std::decay_t<decltype(Value)>;
//
//            //PRINT(typeid(T).name(),'\n');
//            Display<T1>(Name.c_str(), Value);
//            //ReflectedTypes::DisplayHelper(Name,Value);
//            }
//        , Data);
//        property::set(comp, Name.c_str(), Data);
//    }
//}
//template<>
//void DisplayComponent<AudioSource>(AudioSource& as) {
//
//    Display_Property(as);
//
//  
//}
//
//template <>
//void DisplayComponent<MeshRenderer>(MeshRenderer& meshyRendy)
//{
//    //ImGui::Checkbox("##Active", &transform.is_enabled); ImGui::SameLine();
//    //ImGui::Text("Active");
//    Display("Mesh Name", meshyRendy.MeshName);
//    ImGui::AlignTextToFramePadding();
//    ImGui::TableNextColumn();
//    ImGui::Text("MeshName");
//    ImGui::TableNextColumn();
//    std::vector<const char*> meshNames;
//    int number = 0;
//    bool found = false;
//    for (auto& pair : MeshManager.mContainer)
//    {
//        if (pair.first == meshyRendy.MeshName)
//            found = true;
//        meshNames.push_back(pair.first.c_str());
//        if (!found)
//        {
//            ++number;
//        }
//    }
//    ImGui::PushItemWidth(-1);
//    ImGui::Combo("Mesh Name", &number, meshNames.data(), meshNames.size(), 5);
//    ImGui::PopItemWidth();
//    meshyRendy.MeshName = meshNames[number];
//}
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

void DisplayComponent(Script& script)
{
    for (auto& pair : script.fields)
    {
        const char* name = pair.first.c_str();
        Field& field{ pair.second };
        Display(name, field);
        ScriptSetFieldEvent e{ script,name };
        EVENTS.Publish(&e);
    }
}
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
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
    static std::string name{};
    if constexpr (std::is_same<T, Script>())
    {
        name = (component.name + " [Script]");
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

    //For Zac to change to component is_enabled
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

    ImGui::PushID(GetType::E<T>());

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

                curr_scene.Destroy(curr_scene.Get<T>(EditorHierarchy::Instance().selectedEntity));
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

           

            //ImGui::PushID(component.UUID);
           
            //ImGui::SameLine();

            if constexpr (std::is_same_v<T,Script>)
            {
                DisplayComponent(component);
            }
            else
            {
                Display_Property(component);
            }

            //ImGui::PopID();

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
        //DisplayComponentHelper(curr_scene.Get<Transform>(entity));

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
                    DisplayComponentHelper(component);
                }              
            }
        }
        else if constexpr (MultiComponentTypes::Has<T1>()) {

            auto components = curr_scene.GetMulti<T1>(entity);
            for (T1* component : components)
            {
                //Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
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
                    scene.Add<T1>(entity);
                    EditorInspector::Instance().isAddPanel = false;
                }
            }
        }
        else
        {
            if constexpr (std::is_same_v<T1, Script>)
            {
                GetScriptNamesEvent nameEvent;
                EVENTS.Publish(&nameEvent);

                for (size_t i = 0; i < nameEvent.count; ++i)
                {
                    if (CENTERED_CONTROL(ImGui::Button(nameEvent.arr[i], ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
                    {
                        scene.Add<T1>(entity, nameEvent.arr[i]);
                        EditorInspector::Instance().isAddPanel = false;
                    }
                }
            }
            else
            {
                if (CENTERED_CONTROL(ImGui::Button(GetType::Name<T1>(), ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetTextLineHeightWithSpacing()))))
                {
                    scene.Add<T1>(entity);
                    EditorInspector::Instance().isAddPanel = false;
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

void AddPanel(Entity& entity) {
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 500));

    //press esc to exit add component window
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        EditorInspector::Instance().isAddPanel = false;
    }
    ImGui::OpenPopup("Add Component");
    if (ImGui::BeginPopupModal("Add Component", &EditorInspector::Instance().isAddPanel, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {

        (void)AddsDisplay(entity);
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
    std::string entity_name = curr_scene.Get<Tag>(entity).name;
    strcpy_s(buffer, entity_name.c_str());
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##gameObjName", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
        curr_scene.Get<Tag>(entity).name = buffer;
    }

    ImGui::PopItemWidth();
    curr_scene.Get<Tag>(entity).name = buffer;

    ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH
        | ImGuiTableFlags_ScrollY;


    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

    if (ImGui::BeginTable("Components", 1, tableFlags))
    {
        ImGui::PushID((int)entity.EUID());
        DisplayComponents(entity);
        ImGui::PopID();
        ImGui::Separator();
        if (CENTERED_CONTROL(ImGui::Button("Add Component", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetTextLineHeightWithSpacing())))) {
            EditorInspector::Instance().isAddPanel = true;
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
    AddPanel(gameObject, isAddingComponent);*/
}

void EditorInspector::Init()
{
    isAddPanel = false;
}

void EditorInspector::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
    ImGui::SetNextWindowSizeConstraints(ImVec2(320, 180), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin("Inspector");
    //Get Selected Entities from mouse picking
    //List out all components in order
    //templated functionalities (input fields, checkboxes etc.)

    Engine::UUID curr_index = EditorHierarchy::Instance().selectedEntity;

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();


    if (curr_index != NON_VALID_ENTITY) {
        ImGui::Spacing();
        Entity& curr_entity = curr_scene.Get<Entity>(curr_index);
        std::string Header = "Current Entity: " + curr_scene.Get<Tag>(curr_index).name;
        ImGui::Text(Header.c_str()); ImGui::Spacing(); ImGui::Separator();
        DisplayEntity(curr_entity);
    }

    if (isAddPanel) {
        AddPanel(curr_scene.Get<Entity>(curr_index));
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void EditorInspector::Exit()
{

}