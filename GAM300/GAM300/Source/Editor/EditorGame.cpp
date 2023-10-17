/*!***************************************************************************************
\file			EditorScene.cpp
\project
\author			Sean Ngo
\Coauthor       Joseph Ho

\par			Course : GAM300
\date           04/09/2023

\brief
    This file contains the definitions of the following:
    1. Editor Scene
    2. Editor Game

All content © 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/
#include "Precompiled.h"

#include "EditorHeaders.h"
#include "Graphics/BaseCamera.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"

namespace
{
    BaseCamera* camera = nullptr;

    struct DisplayTarget
    {
        unsigned int targetDisplay = 0;
        std::string name;
    };

    DisplayTarget displayTargets[8];
}

void EditorGame::Init()
{
    dimension = glm::vec2(1600.f, 900.f);
    position = glm::vec2(0.f, 0.f);

    for (int i = 0; i < 8; i++)
    {
        displayTargets[i].name = "Display " + std::to_string(i + 1);
        displayTargets[i].targetDisplay = i;
    }

    EVENTS.Subscribe(this, &EditorGame::CallbackEditorWindow);
}

void EditorGame::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

    ToolBar();

    GameView();

    ImGui::PopStyleVar();
}

void EditorGame::ToolBar()
{
    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoResizeY;

    ImGui::SetNextWindowClass(&window_class);

    //Scene toolbar
    if (ImGui::Begin("Game Toolbar"))
    {
        static std::string display = displayTargets[0].name.c_str();
        if (ImGui::BeginCombo("##Target Display", display.c_str()))
        {
            for (int n = 0; n < IM_ARRAYSIZE(displayTargets); n++)
            {
                const bool is_selected = (targetDisplay == (unsigned int)n);
                if (ImGui::Selectable(displayTargets[n].name.c_str(), is_selected))
                {
                    targetDisplay = n;
                    display = displayTargets[n].name.c_str();
                    UpdateTargetDisplay();
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::End();
}

void EditorGame::UpdateTargetDisplay()
{
    Scene& currentScene = MySceneManager.GetCurrentScene();
    for (Camera& cameraComponent : currentScene.GetArray<Camera>())
    {
        if (cameraComponent.GetTargetDisplay() == targetDisplay)
        {
            camera = &cameraComponent;
            return;
        }
    }

    // No camera match the display target
    camera = nullptr;
}

void EditorGame::GameView()
{
    if (ImGui::Begin("Game"))
    {
        if (!camera) // If the camera does not exist
        {
            ImGui::End();
            return;
        }

        windowHovered = ImGui::IsWindowHovered();
        windowFocused = ImGui::IsWindowFocused();
        ImRect sceneRect = ImGui::GetCurrentWindow()->InnerRect;
        position = glm::vec2(sceneRect.Min.x, sceneRect.Min.y);
        ImVec2 viewportEditorSize = sceneRect.GetSize();

        // Check if it needs to resize the game view
        ResizeGameView(*((glm::vec2*)&viewportEditorSize));

        float indent = (viewportEditorSize.x - dimension.x) * 0.5f;
        if (indent > 0)
            ImGui::Indent(indent);

        unsigned int textureID = camera->GetFramebuffer().colorBuffer;
        ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)dimension.x, (float)dimension.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });
    }
    ImGui::End();
}

void EditorGame::ResizeGameView(glm::vec2 _newDimension)
{
    if (_newDimension.x != 0 && _newDimension.y != 0)
    {
        bool modified = false;
        _newDimension = glm::floor(_newDimension);

        glm::vec2 adjusted = dimension;
        if (adjusted.y > _newDimension.y || adjusted.y != _newDimension.y)
        {
            modified = true;
            adjusted = { _newDimension.y * AspectRatio, _newDimension.y };
        }

        if (adjusted.x > _newDimension.x - padding)
        {
            modified = true;
            adjusted = { _newDimension.x - padding, (_newDimension.x - padding) / AspectRatio };
        }

        // If there is any changes to the dimension and modifications, return
        if (adjusted != dimension && modified)
        {
            dimension = adjusted;

            camera->OnResize(dimension.x, dimension.y);
        }
    }
}

void EditorGame::Exit()
{

}

void EditorGame::CallbackEditorWindow(EditorWindowEvent* pEvent)
{
    // If not game window
    if (pEvent->name.compare("Game"))
        return;

    pEvent->isHovered = WindowHovered();
    pEvent->isFocused = WindowFocused();
}