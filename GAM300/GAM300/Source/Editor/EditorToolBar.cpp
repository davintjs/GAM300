/*!***************************************************************************************
\file			EditorToolBar.cpp
\project		
\author         Joseph Ho
\Coauthor       Sean Ngo

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the functions used for the Editor's Toolbar.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "EditorTemplates.h"
#include "Core/EventsManager.h"
#include "Utilities/ThreadPool.h"
#include "Graphics/TextureManager.h"
#include "IOManager/InputSystem.h"

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



    ImVec2 buttonSize = { 25.f, ImGui::GetTextLineHeight() + 10.f};
    
    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    width += buttonSize.x;
    width += style.ItemSpacing.x;
    width += buttonSize.x;
    AlignForWidth(width);

    //std::string icon = (scene_playing ? "Assets/Icons/Editorplaybuttontoggled.dds" : "Assets/Icons/Editorplaybutton.dds" );
    std::string icon = "Assets/Icons/Editorplaybutton.dds";
    size_t icon_id = GET_TEXTURE_ID(icon);

    ImVec4 tintcolor = (scene_playing ? ImVec4(0, 0, 0, 0) : ImVec4(0.6, 0.6, 0.6, 1));

    if (ImGui::ImageButton((ImTextureID)icon_id, buttonSize, { 0 , 0 }, { 1 , 1 }, 0, tintcolor)){
        if (!scene_playing)
        {
            PRINT("SCENE START\n");
            InputSystem::Instance().LockCursor(true);
            SceneStartEvent startEvent{};
            EVENTS.Publish(&startEvent);
        }
        else
        {
            InputSystem::Instance().LockCursor(false);
            PRINT("SCENE STOP\n");
            SceneStopEvent stopEvent{};
            EVENTS.Publish(&stopEvent);

        }
        scene_playing = scene_playing ? false : true;
    }

    ImGui::SameLine();
    tintcolor = (scene_paused ? ImVec4(0, 0, 0, 0) : ImVec4(0.6, 0.6, 0.6, 1));
    icon = "Assets/Icons/Editorpausebutton.dds";
    icon_id = GET_TEXTURE_ID(icon);
    if (ImGui::ImageButton((ImTextureID)icon_id, buttonSize, { 0 , 0 }, { 1 , 1 }, 0, tintcolor)) {
        scene_paused = scene_paused ? false : true;
    }
    
    ImGui::End();
}

void EditorToolBar::Exit()
{

}