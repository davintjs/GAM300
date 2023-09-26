/**************************************************************************************/
/*!
//    \file			Editor.cpp
//    \author(s) 	Joseph Ho Jun Jie
//
//    \date   	    9th May 2023
//    \brief		This file contains the prototypes for all the functions that are used
//					to create the Level Editor interface for the program.
//
//    \Percentage   Joseph 100%
//
//    Copyright (C) 2022 DigiPen Institute of Technology.
//    Reproduction or disclosure of this file or its contents without the
//    prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /**************************************************************************************/

#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"
#include "Core/SystemsGroup.h"

using EditorSystemsPack =
TemplatePack
<
    EditorMenuBar,
    EditorContentBrowser,
    EditorScene,
    EditorInspector,
    EditorDebugger,
    EditorHierarchy,
    EditorToolBar,
    EditorPerformanceViewer
>;

using EditorSystems = decltype(SystemsGroup(EditorSystemsPack()));

void EditorSystem::Init()
{
    //Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    //io.FontDefault = io.Fonts->Fonts[0];

    // Enable this for dockspace capabilities in dockspace mode		
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(GLFW_Handler::ptr_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    EVENTS.Subscribe(this, &EditorSystem::CallbackSelectedEntity);

    EditorSystems::Init();
}

void EditorSystem::Update(float dt)
{
    //Editor Functions
    Editor_Dockspace();

    

    //for (ISystem* pSystem : editorSystems)
    //{   
    //    pSystem->Update(dt);
    //}
    EditorSystems::Update(dt);

    /*bool demo = true;
    ImGui::ShowDemoWindow(&demo);*/

    ImGui::StyleColorsDark();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
}

void EditorSystem::Exit()
{
    //for (auto iter = editorSystems.rbegin(); iter != editorSystems.rend(); ++iter)
    //{
    //    (*iter)->Exit();
    //}
    EditorSystems::Exit();

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}



/**************************************************************************/
   /*!
       Implementation of the docking functionality in the Editor.
   */
/**************************************************************************/
void EditorSystem::Editor_Dockspace() {

    static bool dockopen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // setting of docking space settings in ImGui
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoBackground;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &dockopen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
}


Engine::UUID EditorSystem::GetSelectedEntity()
{
    return selectedEntity;
}

void EditorSystem::SetSelectedEntity(Entity* pEntity)
{
    selectedEntity = pEntity->EUID();
}

void EditorSystem::CallbackSelectedEntity(SelectedEntityEvent* pEvent)
{
    if (pEvent->pEntity)
        selectedEntity = pEvent->pEntity->EUID();
    else
        selectedEntity = 0;
}