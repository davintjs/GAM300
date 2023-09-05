/*!***************************************************************************************
\file			EditorContentBrowser.cpp
\project		GAM300
\author

\par			Course: GAM300
\date           04/09/2023

\brief
    This file contains the definitions of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"

static const std::string AssetDirectory = "Assets";
static std::string entityType = "default", entityName = "default";
static std::filesystem::path CurrentDirectory = AssetDirectory;
static std::string currentFolder = "Assets";

void EditorContentBrowser::Init()
{

}

void EditorContentBrowser::Update(float dt)
{
    ImGui::Begin("Content Browser");
    ImGui::Text("Current Folder: %s", currentFolder.c_str()); ImGui::Spacing();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    /*ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::Text(currentFolder.c_str());
    ImGui::PopFont();
    ImGui::Separator();*/

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
        ImGui::PushID(i++);
        const auto& path = it.path();
        auto relativepath = std::filesystem::relative(path, AssetDirectory);
        std::string pathStr = relativepath.filename().string();

        //Draw the file / folder icon based on whether it is a directory or not
        std::string icon = it.is_directory() ? "foldericon" : "fileicon";

        size_t icon_id = 0;
        //render respective file icon textures
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
        ImGui::ImageButton((ImTextureID)icon_id, { iconsize, iconsize }, { 0 , 1 }, { 1 , 0 });

        //Drag drop logic for files
        /*if (ImGui::BeginDragDropSource()) {
            std::string filepath = relativepath.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filepath.c_str(), filepath.size() + 1);
            ImGui::EndDragDropSource();
        }*/

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

        //double click logic to open scene file
        /*if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (!it.is_directory()) {

            }
        }*/

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