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
static std::filesystem::path CurrentDirectory = AssetDirectory;
static std::string currentFolder = "Assets";

void EditorContentBrowser::Init()
{

}

void EditorContentBrowser::Update(float dt)
{
    UNREFERENCED_PARAMETER(dt);
    ImGui::Begin("Content Browser");
    ImGui::Text("Current Folder: %s", currentFolder.c_str()); ImGui::Spacing();

    // Back button to return to parent directory
    if (CurrentDirectory != std::filesystem::path(AssetDirectory))
    {
        if (ImGui::Button("Back", ImVec2{ 50.f, 30.f }))
        {
            CurrentDirectory = CurrentDirectory.parent_path();
            currentFolder = CurrentDirectory.string();
        }
    }


    static float padding = 12.0f;
    static float iconsize = 100.f;
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
        std::string parentpath = relativepath.parent_path().string();

        std::string filename = relativepath.string();
        
        if (!it.is_directory()) {

            auto it2 = filename.begin();


            if (filename.find_last_of("\\") != std::string::npos) {
                it2 = filename.begin() + filename.find_last_of("\\") + 1;
                filename.erase(filename.begin(), it2);
            }
            it2 = filename.begin() + filename.find_first_of(".");
            filename.erase(it2, filename.end());
            GLint tex = GET_TEXTURE_ID(filename);
            if (tex != UINT_MAX) {
                icon = filename;
            }
        }
       
        //render respective file icon textures
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
        icon_id = GET_TEXTURE_ID(icon);
        ImGui::ImageButton((ImTextureID)icon_id, { iconsize, iconsize }, { 0 , 0 }, { 1 , 1 });

        ImGui::PopStyleColor();
        //Change directory into the folder clicked
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (it.is_directory())
            {
                currentFolder = pathStr;
                CurrentDirectory /= path.filename();
            }
        }

        //double click logics
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

            //Open script file in VSCode
            if ((path.string().find(".cs") != std::string::npos)) {
                std::string command = "code " + path.string();
                system(command.c_str());
            }

            //Open Scene file
            if ((path.string().find(".scene") != std::string::npos)) {
                //Open scene file logic here
                LoadSceneEvent loadScene(path.string());
                EVENTS.Publish(&loadScene);
                EditorDebugger::Instance().AddLog("[%i]{Scene}Scene File Opened!\n", EditorDebugger::Instance().debugcounter++);
            }
        }

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