/*!***************************************************************************************
\file			Editor.cpp
\project		GAM300
\author			Joseph Ho

\par			Course: GAM300
\date           07/09/2023

\brief
    This file contains the definitions of the following:
    1. Contains the definitions of the Editor system in the architecture.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "AppEntry/Application.h"
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
    EditorGame,
    EditorScene,
    EditorInspector,
    EditorDebugger,
    EditorHierarchy,
    EditorToolBar,
    EditorPerformanceViewer,
    EditorBehaviourTreeEditor
>;

//helper function to align imgui objects
void AlignForWidth(float width, float alignment)
{
    //ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

using EditorSystems = decltype(SystemsGroup(EditorSystemsPack()));

void EditorSystem::Init()
{
    //Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    //io.FontDefault = io.Fonts->Fonts[0];

    // Enable this for dockspace capabilities in dockspace mode		
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(Application::GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    EVENTS.Subscribe(this, &EditorSystem::CallbackSelectedEntity);
    EVENTS.Subscribe(this, &EditorSystem::CallbackGetSelectedEntity);

    EditorSystems::Init();
}

void DeleteEntity(Engine::UUID entity) {
    if (entity != NON_VALID_ENTITY)
    {
        Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
        Entity& ent = curr_scene.Get<Entity>(entity);
        Change newchange;
        newchange.entity = &ent;
        EDITOR.History.AddEntityChange(newchange);
        //curr_scene.Destroy(ent);
        SelectedEntityEvent selectedEvent{ 0 };
        EVENTS.Publish(&selectedEvent);
    }
}

void EditorSystem::Update(float dt)
{
    if (!MySceneManager.HasScene() || !MySceneManager.GetCurrentScene().HasHandle<Entity>({ selectedEntity }))
        selectedEntity = 0;
    Scene& currScene{ MySceneManager.GetCurrentScene() };
    //Editor Functions
    Editor_Dockspace();
    EditorSystems::Update(dt);

    //macros for undo and redo functionality
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
        if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
            History.UndoChange();    
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
        if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
            History.RedoChange();
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
        if (ImGui::IsKeyPressed(ImGuiKey_D, false))
        {
            PRINT("SELECTED ENTITY: ", selectedEntity, "\n");
            if (selectedEntity)
            {
                currScene.Clone(currScene.Get<Entity>(selectedEntity));
                
            }
        }
    }

    //Need to press ctrl before delete to avoid accidental deletion
    if (ImGui::IsKeyReleased(ImGuiKey_Delete)) {
        DeleteEntity(selectedEntity);
    }
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
        if (ImGui::IsKeyReleased(ImGuiKey_Backspace)) {
            DeleteEntity(selectedEntity);
        }
    }

    //demo
    //bool demo = true;
    //ImGui::ShowDemoWindow(&demo);
    //ImPlot::ShowDemoWindow(&demo);

    ImGui::StyleColorsDark();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
}

void EditorSystem::Exit()
{
    EditorSystems::Exit();

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
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

void EditorSystem::CallbackGetSelectedEntity(GetSelectedEntityEvent* pEvent)
{
    if (selectedEntity != 0)
        pEvent->pEntity = &MySceneManager.GetCurrentScene().Get<Entity>(selectedEntity);
    else
        pEvent->pEntity = nullptr;
}