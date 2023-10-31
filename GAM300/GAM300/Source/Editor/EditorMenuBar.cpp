/*!***************************************************************************************
\file			EditorMenuBar.cpp
\project
\author         Sean Ngo
\co-author      Joseph Ho

\par			Course: GAM300
\date           04/09/2023

\brief
    This file contains the definitions of the following:
    1. Editor Menubar which is mainly for engine utility 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "Editor.h"
#include "EditorHeaders.h"
#include "Core/EventsManager.h"
#include "Graphics/GraphicsHeaders.h"
#include "../Utilities/PlatformUtils.h"
#include "Utilities/ThreadPool.h"

static bool graphics_settings = false;

void EditorMenuBar::Init()
{

}

void EditorMenuBar::Update(float dt)
{
    UNREFERENCED_PARAMETER(dt);
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_N))
            NewScene();

        if (ImGui::IsKeyPressed(ImGuiKey_S))
            SaveScene();

        if (ImGui::IsKeyPressed(ImGuiKey_O))
            OpenFile();
    }

    if (ImGui::BeginMainMenuBar())
    {
        //File Menu functionality
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
                NewScene();

            // Opening of files using file dialogs
            if (ImGui::MenuItem("Open", "Ctrl+O"))
                OpenFile();

            // Saving of scene files only if an active scene is open, using file dialogs
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                SaveScene();
            }
            //else
            //    //Do not allow the user to save when there's no file loaded!
            //    ImGui::TextDisabled("Save", "Ctrl+S");

            // Save as functionality using file dialogs
            if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
            {
                IsNewSceneEvent newScene;
                EVENTS.Publish(&newScene);
                std::string filepath = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
                //Save File
                SaveSceneEvent saveScene(filepath);
                EVENTS.Publish(&saveScene);
            }
            //else
            //    //Do not allow the user to save when there's no file loaded!
            //    ImGui::TextDisabled("Save As..", "Ctrl+Shift+S");

            // Quit application, opens a pop-up dialog to confirm exit
            if (ImGui::MenuItem("Quit", "Ctrl+Q"))
            {
                exitApp = true;
            }
            ImGui::EndMenu();
        }

        // Popup Modal for exiting application
        if (exitApp)
        {
            ImGui::OpenPopup("Exit?");
            exitApp = false;
        }
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Exit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to exit the application?\n\n");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                //Close Application
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            // copy functionality
            if (ImGui::MenuItem("Copy", "Ctrl+C"))
            {
                //Copy Current Item
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V"))
            {
                //Creates a copy of the entity copieds
            }


            // Undo Functionality
            if(EDITOR.History.UndoStackEmpty())
                ImGui::TextDisabled("Undo");
            else {
                if (ImGui::MenuItem("Undo", "Ctrl+Z"))
                    EDITOR.History.UndoChange();
            }
               
            // Redo Functionality
            if(EDITOR.History.RedoStackEmpty())
                ImGui::TextDisabled("Redo");
            else {
                if (ImGui::MenuItem("Redo", "Ctrl+Y"))
                    EDITOR.History.RedoChange();         
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Preferences"))
        {
            if (ImGui::MenuItem("Graphics"))
                graphics_settings = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (graphics_settings) {
        //Formating spacing between menu items
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiStyle origStyle = style;
        style.CellPadding = ImVec2(0.0f, 6.0f);

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(450.f, 600.f));

        auto hdr = &RENDERER.IsHDR();

        ImGui::Begin("Graphics Settings", &graphics_settings, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTable("GraphicSetting", 2, ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH)) {

            ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Enable HDR lighting", hdr);
            ImGui::TableNextRow();

            if (*hdr) {
                ImGui::TableNextColumn();
                ImGui::Text("Light Exposure"); ImGui::SameLine();
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragFloat("##", &RENDERER.GetExposure(), 0.01f, 0.f, 5.f);
                ImGui::TableNextRow();              
            }

            ImGui::TableNextColumn();         
            auto shadows = &RENDERER.enableShadows();
            ImGui::Checkbox("Enable Shadows", shadows);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            auto bloom = &RENDERER.enableBloom();
            ImGui::Checkbox("Enable Bloom", bloom);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Bloom count and threshold available only when Bloom Enabled");
            ImGui::TableNextRow();

            if (*bloom) {
                ImGui::TableNextColumn();
                int& bloomcount = reinterpret_cast<int&>(RENDERER.GetBloomCount());
                ImGui::Text("Bloom Count"); ImGui::SameLine();
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragInt("##bloomcount", &bloomcount, 0.05, 0, 20);
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("Bloom Threshold"); ImGui::SameLine();
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragFloat("##bloomthreshold", &RENDERER.GetBloomThreshold(), 0.01f, 1.f, 10.f);
                ImGui::TableNextRow();
            }

            ImGui::TableNextColumn();
            ImGui::Text("Ambient"); ImGui::SameLine();
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::DragFloat("##ambient", &RENDERER.getAmbient(), 0.02f, 0.f, 20.f);
            ImGui::TableNextRow();

            ImGui::EndTable();
        }

    
        ImGui::GetStyle() = origStyle;
   
        ImGui::End();
    }
    

    
}

void EditorMenuBar::NewScene()
{
    //New Scene
    CreateSceneEvent createScene(nullptr);
    EVENTS.Publish(&createScene);

    // Load this new scene if there was a previously loaded one
    SceneChangingEvent changeScene(*createScene.scene);
    EVENTS.Publish(&changeScene);
}

void EditorMenuBar::SaveScene()
{
    IsNewSceneEvent newScene;
    EVENTS.Publish(&newScene);

    // Check if it is a new scene
    if (newScene.data)
    {
        std::string filepath = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");

        // If the user does not want to save the file and exit file dialog
        if (filepath.empty())
            return;

        //Save File
        SaveSceneEvent saveScene(filepath);
        EVENTS.Publish(&saveScene);
    }
    else
    {
        SaveSceneEvent saveScene;
        EVENTS.Publish(&saveScene);
    }
}

void EditorMenuBar::OpenFile()
{
    // Bean: open specific file types, for now this function only open scene files
    //std::string Filename = FileDialogs::OpenFile("All Files (*.*)\0*.*\0");
    std::string Filename = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
    if (!Filename.empty())
    {
        //Open File

        // Open Scene File
        if (Filename.find(".scene") != std::string::npos)
        {
            LoadSceneEvent loadScene(Filename);
            EVENTS.Publish(&loadScene);
        }
    }
}

void EditorMenuBar::Exit()
{

}