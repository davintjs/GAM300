/*!***************************************************************************************
\file			EditorContentBrowser.cpp
\project		
\author         Joseph Ho

\par			Course: GAM300
\date           04/09/2023

\brief
    This file contains the definitions of the functions for the Editor's Content Browser.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "Graphics/TextureManager.h"
#include "AssetManager/AssetManager.h"
#include "Core/EventsManager.h"

static const std::string AssetDirectory = "Assets";
static std::string currentFolder = "Assets";



void EditorContentBrowser::Init()
{
    currentDirectory = AssetDirectory;
    EVENTS.Subscribe(this, &EditorContentBrowser::CallbackGetCurrentDirectory);
}

void EditorContentBrowser::Update(float dt)
{
    UNREFERENCED_PARAMETER(dt);
    bool isOpened = ImGui::Begin("Content Browser");
    if (!isOpened)
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Current Folder: %s", currentFolder.c_str()); ImGui::Spacing();

    // Back button to return to parent directory
    if (currentDirectory != std::filesystem::path(AssetDirectory))
    {
        if (ImGui::Button("Back", ImVec2{ 50.f, 30.f }))
        {
            currentDirectory = currentDirectory.parent_path();
            currentFolder = currentDirectory.string();
        }
    }


    static float padding = 20.0f;
    static float iconsize = 100.f;
    float cellsize = iconsize + padding + 20.f;

    float window_width = ImGui::GetContentRegionAvail().x;
    int columncount = (int)(window_width / cellsize);
    if (columncount < 1) { columncount = 1; }

    ImGui::Columns(columncount, 0, false);

    int i = 0;
    //using filesystem to iterate through all folders/files inside the "/Data" directory
    for (auto& it : std::filesystem::directory_iterator{ currentDirectory })
    {
        const auto& path = it.path();
        if (path.string().find("meta") != std::string::npos) continue;

        ImGui::PushID(i++);

        auto relativepath = std::filesystem::relative(path, AssetDirectory);
        std::string pathStr = relativepath.filename().string();

        //Draw the file / folder icon based on whether it is a directory or not
        fs::path icon = it.is_directory() ? "Assets/Icons/foldericon.dds" : "Assets/Icons/fileicon.dds";

        size_t icon_id = 0;
        
        if (!it.is_directory()) {

            GLint tex = GET_TEXTURE_ID(path);
            if (tex != 0) {
                icon = path;
            }
        }

        ImGui::BeginGroup();

        //render respective file icon textures
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
        icon_id = GET_TEXTURE_ID(icon);
        ImGui::ImageButton((ImTextureID)icon_id, { iconsize, iconsize }, { 0 , 0 }, { 1 , 1 });

        //Drag drop logic for content browser
        if (!it.is_directory() && ImGui::BeginDragDropSource()) {
            std::string filepath = relativepath.string();
            std::string ext = filepath;
            //check what extension is the file
            ext.erase(0, ext.find_last_of(".") + 1);

            ContentBrowserPayload payload;
            /*
            GetAssetEvent e{ it.path() };
            EVENTS.Publish(&e);
            Engine::GUID currentGUID = e.guid;
            payload.payload = &currentGUID;
            payload.type = MESH;*/
            
            if (ext == "geom") { //mesh files
                GetAssetEvent e{ it.path() };
                EVENTS.Publish(&e);
                Engine::GUID currentGUID = e.guid;
                payload.guid = currentGUID;
                payload.type = MESH;
            }
            else if (ext == "prefab") { //prefab files

            }
           
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &payload, filepath.size() + 1);
            ImGui::EndDragDropSource();

        }

        ImGui::PopStyleColor();
        //Change directory into the folder clicked
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (it.is_directory())
            {
                currentFolder = pathStr;
                currentDirectory /= path.filename();
            }
        }

        //double click logics
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

            //Open script file in VSCode
            if ((path.string().find(".cs") != std::string::npos)) {
                system("ScriptCore.sln");
                system(path.string().c_str());
            }

            //Open Scene file
            if ((path.string().find(".scene") != std::string::npos)) {
                //Open scene file logic here
                LoadSceneEvent loadScene(path.string());
                EVENTS.Publish(&loadScene);
                EditorDebugger::Instance().AddLog("[%i]{Scene}Scene File Opened!\n", EditorDebugger::Instance().debugcounter++);
            }
        }
    
        //render extension button
        if (!it.is_directory() && (path.string().find(".fbx") != std::string::npos)) {
            ImGui::SameLine();

            // Calculate the position to align the buttons to the center of the row height
            float rowHeight = ImGui::GetTextLineHeightWithSpacing();
            float buttonPosY = ImGui::GetCursorPos().y + 50.f;

            // Set the cursor position to center the buttons vertically
            ImGui::SetCursorPosY(buttonPosY);

            icon_id = GET_TEXTURE_ID("Assets/Icons/Editorplaybutton.dds");
            ImGui::ImageButton((ImTextureID)icon_id, {20.f, 20.f}, {0 , 0}, {1 , 1}, 0);
        }
       
        ImGui::EndGroup();

        //render file name below icon
        ImGui::TextWrapped(pathStr.c_str());
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::Columns(1);

    ImGui::End();
}

void EditorContentBrowser::Exit()
{

}

void EditorContentBrowser::CallbackGetCurrentDirectory(EditorGetCurrentDirectory* pEvent)
{
    pEvent->path = currentDirectory.string();
}