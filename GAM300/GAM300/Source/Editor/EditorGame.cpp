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

All content � 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/
#include "Precompiled.h"

#include "EditorHeaders.h"
#include "Graphics/BaseCamera.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"

extern glm::vec2 windowPos; // In ColourPicker.cpp
extern glm::vec2 windowDimension; // In ColourPicker.cpp

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
    EVENTS.Subscribe(this, &EditorGame::CallbackSetCamera);
    EVENTS.Subscribe(this, &EditorGame::CallbackDeleteCamera);
    EVENTS.Subscribe(this, &EditorGame::CallbackSceneChange);
    EVENTS.Subscribe(this, &EditorGame::CallbackSceneStop);
}

void EditorGame::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

    GameView();

    ImGui::PopStyleVar();
}

void EditorGame::ToolBar()
{
    if (ImGui::BeginMenuBar())
    {
        static std::string display = displayTargets[0].name.c_str();
        ImGui::SetNextItemWidth(76.f + 20.f);
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

        ImGui::EndMenuBar();
    }
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
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;
    windowOpened = ImGui::Begin("Game", nullptr, flags);
    if (windowOpened)
    {
        ToolBar();

        if (!camera) // If the camera does not exist
        {
            ImGui::End();
            return;
        }

        windowHovered = ImGui::IsWindowHovered();
        windowFocused = ImGui::IsWindowFocused();
        ImRect sceneRect = ImGui::GetCurrentWindow()->InnerRect;
        position = glm::vec2(sceneRect.Min.x, sceneRect.Min.y);
        //std::cout << "editor game position:" << position.x << " , " << position.y << "\n";

        ImVec2 viewportEditorSize = sceneRect.GetSize();

        // Check if it needs to resize the game view
        camera->TryResize(*((glm::vec2*)&viewportEditorSize));

        // Indentation for the game view to be in the center of the game window
        float indent = (viewportEditorSize.x - dimension.x) * 0.5f;
        if (indent > 0.f)
            ImGui::Indent(indent);
        float x_offset = indent;

        indent = (viewportEditorSize.y - dimension.y) * 0.5f;
        float y_offset = indent;

        dimension = camera->GetViewportSize();

        windowPos.x = position.x + x_offset; // Colour Picking.cpp
        windowPos.y = position.y + y_offset; // Colour Picking.cpp

        windowDimension.x = dimension.x; // Colour Picking.cpp
        windowDimension.y = dimension.y; // Colour Picking.cpp

        if(indent > 1.f)
            ImGui::Dummy({ 0.f, indent });

        unsigned int textureID = FRAMEBUFFER.GetTextureID(camera->GetFramebufferID(), camera->GetAttachment());
        //unsigned int textureID = camera->GetFramebuffer().GetColorAttachmentId();

        ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)dimension.x, (float)dimension.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });
    }
    ImGui::End();
}

void EditorGame::Exit()
{

}

void EditorGame::CallbackEditorWindow(EditorWindowEvent* pEvent)
{
    // If not game window
    if (pEvent->name.compare("Game"))
        return;

    pEvent->isOpened = WindowOpened();
    pEvent->isHovered = WindowHovered();
    pEvent->isFocused = WindowFocused();
}

void EditorGame::CallbackSetCamera(ObjectCreatedEvent<Camera>* pEvent)
{
    if (pEvent->pObject->GetTargetDisplay() == targetDisplay)
        UpdateTargetDisplay();

}

void EditorGame::CallbackDeleteCamera(ObjectDestroyedEvent<Camera>* pEvent)
{
    if (pEvent->pObject->GetTargetDisplay() == targetDisplay)
        camera = nullptr;
}

void EditorGame::CallbackSceneChange(SceneChangingEvent* pEvent)
{
    UpdateTargetDisplay();
}

void EditorGame::CallbackSceneStop(ScenePostCleanupEvent* pEvent)
{
    UpdateTargetDisplay();
}