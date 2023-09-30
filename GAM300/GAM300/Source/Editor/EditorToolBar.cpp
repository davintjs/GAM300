/*!***************************************************************************************
\file			EditorToolBar.cpp
\project		
\author         Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. Editor ToolBar

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "EditorTemplates.h"
#include "Core/EventsManager.h"
#include "Utilities/ThreadPool.h"

void AlignForWidth(float width, float alignment = 0.5f)
{
    //ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void EditorToolBar::Init()
{

}


static bool scene_playing = false;
static bool scene_paused = false;

void EditorToolBar::Update(float dt)
{
    UNREFERENCED_PARAMETER(dt);
    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoResizeY;
    ImGui::SetNextWindowClass(&window_class);

    static int Guizmo_world = 0;

    ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
    //Play, Pause, Step(Optional)
    // 
    
    //To do: change button to image button with play/pause icons



    ImVec2 buttonSize = { 50.f, ImGui::GetTextLineHeight() + 10.f};
    
    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    width += buttonSize.x;
    width += style.ItemSpacing.x;
    width += buttonSize.x;
    AlignForWidth(width);

    std::string play_status = (scene_playing ? "Stop" : "Play"); 
    if (ImGui::Button(play_status.c_str(), buttonSize)) {
        if (!scene_playing)
        {
            PRINT("SCENE START\n");
            SceneStartEvent startEvent{};
            EVENTS.Publish(&startEvent);
        }
        else
        {
            PRINT("SCENE STOP\n");
            SceneStopEvent stopEvent{};
            EVENTS.Publish(&stopEvent);

        }
        scene_playing = scene_playing ? false : true;
    }

    ImGui::SameLine();
    std::string pause_status = (scene_paused ? "Resume" : "Pause");
    if (ImGui::Button(pause_status.c_str(), buttonSize)) {
        scene_paused = scene_paused ? false : true;
    }
    
    ImGui::End();
}

void EditorToolBar::Exit()
{

}