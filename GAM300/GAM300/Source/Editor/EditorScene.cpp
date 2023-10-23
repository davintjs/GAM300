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

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "IOManager/InputHandler.h"
#include "Editor.h"
#include "Editor/EditorCamera.h"
#include "ImGuizmo.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"
#include "Graphics/MeshManager.h"
#include "Graphics/GraphicsHeaders.h"

namespace
{
    const char* GizmoWorld[] = { "Local", "Global" };
    int GizmoType = ImGuizmo::TRANSLATE;
    int coord_selection = 1;
}

void EditorScene::Init()
{
    sceneDimension = glm::vec2(1600.f, 900.f);
    scenePosition = glm::vec2(0.f, 0.f);

    EVENTS.Subscribe(this, &EditorScene::CallbackEditorWindow);
}

void EditorScene::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

    ToolBar();

    SceneView();

    ImGui::PopStyleVar();

    inOperation = ImGuizmo::IsOver() && EditorHierarchy::Instance().selectedEntity != NON_VALID_ENTITY;
}

void EditorScene::ToolBar()
{
    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoResizeY;

    ImGui::SetNextWindowClass(&window_class);

    //Scene toolbar
    if (ImGui::Begin("Scene Toolbar"))
    {
        ImGui::Dummy(ImVec2(0.0f, 3.f));
        ImGui::Dummy(ImVec2(15.0f, 0.f)); ImGui::SameLine();
        ImGui::SetNextItemWidth(68.f);
        ImGui::Combo("Coord Space", &coord_selection, GizmoWorld, 2, 2);
        ImGui::SameLine(); ImGui::Dummy(ImVec2(15.0f, 0.f));

        float buttonSize = 20.f;
        ImVec2 btn = ImVec2(buttonSize, buttonSize);

        static int toggled = 2;
        ImVec4 untoggledColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Dark grey
        ImVec4 toggledColor = ImVec4(0.0f, 0.447f, 0.898f, 1.0f);
        ImVec4 buttonColor = untoggledColor;

        bool shouldPan = false; // Bean: This is for panning the camera using the Q key
        buttonColor = (toggled == 1) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        bool condition = !EditorCam.IsFlying() && (ImGui::IsKeyPressed(ImGuiKey_Q) && windowHovered);
        ImGui::SameLine(); if (ImGui::Button("Q", btn) || condition)
        {
            toggled = 1;
        }

        // Pan the editor camera
        shouldPan = (toggled == 1) ? true : false;
        EditorPanCameraEvent e(shouldPan);
        EVENTS.Publish(&e);

        ImGui::PopStyleColor();

        buttonColor = (toggled == 2) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        ImGui::SameLine(); if (ImGui::Button("W", btn)
            || (ImGui::IsKeyPressed(ImGuiKey_W) && windowHovered))
        {
            GizmoType = ImGuizmo::TRANSLATE;
            toggled = 2;
        }
        ImGui::PopStyleColor();

        buttonColor = (toggled == 3) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        condition = !EditorCam.IsFlying() && (ImGui::IsKeyPressed(ImGuiKey_E) && windowHovered);
        ImGui::SameLine(); if (ImGui::Button("E", btn) || condition)
        {
            GizmoType = ImGuizmo::ROTATE;
            toggled = 3;
        }
        ImGui::PopStyleColor();

        buttonColor = (toggled == 4) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        ImGui::SameLine(); if (ImGui::Button("R", btn)
            || (ImGui::IsKeyPressed(ImGuiKey_R) && windowHovered))
        {
            GizmoType = ImGuizmo::SCALE;
            toggled = 4;
        }
        ImGui::PopStyleColor();

        ImGui::SameLine(); ImGui::Dummy(ImVec2(20.0f, 0.f));

        //For thoe to change to toggle debug drawing
        ImGui::SameLine(); if (ImGui::Checkbox("Debug Drawing", &debug_draw)) {}
        ImGui::SameLine(); if (ImGui::Checkbox("Render Shadows", &Renderer::Instance().RenderShadow)) {}
    }
    ImGui::End();
}

void EditorScene::SceneView()
{
    //Editor scene viewport
    if (windowOpened = ImGui::Begin("Scene"))
    {
        windowHovered = ImGui::IsWindowHovered();
        windowFocused = ImGui::IsWindowFocused();
        ImRect sceneRect = ImGui::GetCurrentWindow()->InnerRect;
        scenePosition = glm::vec2(sceneRect.Min.x, sceneRect.Min.y);
        unsigned int textureID = EditorCam.GetFramebuffer().GetColorAttachmentId();
        ImVec2 viewportEditorSize = sceneRect.GetSize();
        glm::vec2 _newDimension = *((glm::vec2*)&viewportEditorSize);

        // Only if the current scene dimension is not the same as new dimension
        if (sceneDimension != _newDimension && _newDimension.x != 0 && _newDimension.y != 0)
        {
            sceneDimension = { _newDimension.x, _newDimension.y };
            EditorUpdateSceneGeometryEvent e(scenePosition, sceneDimension);
            EVENTS.Publish(&e);
            EditorCam.OnResize(sceneDimension.x, sceneDimension.y);
        }

        ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)sceneDimension.x, (float)sceneDimension.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });

        // Display the gizmos for the selected entity
        DisplayGizmos();
    }
    ImGui::End();
}

bool EditorScene::SelectEntity()
{
    if (!inOperation && !EditorCam.IsMoving() && InputHandler::isMouseButtonPressed_L())
    {
        // Bean: Click within the scene imgui window
        if (!windowHovered)
            return false;

        EditorCam.GetRay() = EditorCam.Raycasting();
        if (EditorCam.GetIntersect() == FLT_MAX)
        {
            // This means that u double clicked, wanted to select something, but THERE ISNT ANYTHING
            SelectedEntityEvent selectedEvent{ 0 };
            EVENTS.Publish(&selectedEvent);
        }
        return true;
    }

    return false;
}

void EditorScene::DisplayGizmos()
{
    Scene& currentScene = MySceneManager.GetCurrentScene();
    if (SelectEntity())
    {
        for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
        {
            Entity& entity = currentScene.Get<Entity>(renderer);
            Transform& transform = currentScene.Get<Transform>(entity);

            // I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
            glm::mat4 transMatrix = transform.GetWorldMatrix();

            glm::vec3 translation;
            glm::vec3 rot;
            glm::vec3 scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transMatrix), &translation[0], &rot[0], &scale[0]);

            glm::vec3 mins = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_min;
            glm::vec3 maxs = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_max;
            rot = glm::radians(rot);
            glm::mat4 rotMat = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

            float& intersect = EditorCam.GetIntersect();
            float& tempIntersect = EditorCam.GetTempIntersect();
            Ray3D temp = EditorCam.GetRay();
            if (temp.TestRayOBB(glm::translate(glm::mat4(1.0f), translation) * rotMat, mins, maxs, tempIntersect))
            {
                if (tempIntersect < intersect)
                {
                    SelectedEntityEvent SelectingEntity(&entity);
                    EVENTS.Publish(&SelectingEntity);
                    intersect = tempIntersect;
                }
            }
        }

        for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
        {
            Entity& entity = currentScene.Get<Entity>(Sprite);
            Transform& transform = currentScene.Get<Transform>(entity);

            // I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
            glm::mat4 transMatrix = transform.GetWorldMatrix();

            glm::vec3 translation;
            glm::vec3 rot;
            glm::vec3 scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transMatrix), &translation[0], &rot[0], &scale[0]);

            //glm::vec3 mins = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_min;
            //glm::vec3 maxs = scale * MeshManager.DereferencingMesh(renderer.MeshName)->vertices_max;
            glm::vec3 mins = -scale;
            glm::vec3 maxs = scale;
            rot = glm::radians(rot);
            glm::mat4 rotMat = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

            float& intersect = EditorCam.GetIntersect();
            float& tempIntersect = EditorCam.GetTempIntersect();
            Ray3D temp = EditorCam.GetRay();
            if (temp.TestRayOBB(glm::translate(glm::mat4(1.0f), translation) * rotMat, mins, maxs, tempIntersect))
            {
                if (tempIntersect < intersect)
                {
                    SelectedEntityEvent SelectingEntity(&entity);
                    EVENTS.Publish(&SelectingEntity);
                    intersect = tempIntersect;
                }
            }



        }
    }

    // Display gizmos for selected entity
    if (EDITOR.GetSelectedEntity() != 0)
    {
        Entity& entity = currentScene.Get<Entity>(EDITOR.GetSelectedEntity());
        Transform& trans = currentScene.Get<Transform>(entity);
        for (int i = 0; i < 3; ++i)
        {
            if (fabs(trans.scale[i]) < 0.001f)
                trans.scale[i] = 0.001f;
        }

        glm::mat4 transform_1 = trans.GetWorldMatrix();

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(scenePosition.x, scenePosition.y, sceneDimension.x, sceneDimension.y);
        ImGuizmo::Manipulate(glm::value_ptr(EditorCam.GetViewMatrix()), glm::value_ptr(EditorCam.GetProjMatrix()),
            (ImGuizmo::OPERATION)GizmoType, (ImGuizmo::MODE)coord_selection, glm::value_ptr(transform_1));

        static bool firstmove = true;
        static Transform origTransform;

        if (ImGuizmo::IsUsing())
        {
            if (firstmove) {
                origTransform = trans;
                firstmove = false;
            }

            if (trans.parent)
            {
                Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(trans.parent);
                glm::mat4 parentTransform = parentTrans.GetWorldMatrix();
                transform_1 = glm::inverse(parentTransform) * transform_1;
            }
            glm::vec3 a_translation;
            glm::vec3 a_rot;
            glm::vec3 a_scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform_1), &a_translation[0], &a_rot[0], &a_scale[0]);
            trans.translation = a_translation;
            trans.rotation = glm::radians(a_rot);
            trans.scale = a_scale;
        }
        else if (!firstmove) {
            if (trans.translation != origTransform.translation) {
                Change translate(&trans, "Transform/Translation");
                EDITOR.History.SetPropertyValue(translate, origTransform.translation, trans.translation);
            }
            if (trans.rotation != origTransform.rotation) {
                Change rotate(&trans, "Transform/Rotation");
                EDITOR.History.SetPropertyValue(rotate, origTransform.rotation, trans.rotation);
            }
            if (trans.scale != origTransform.scale) {
                Change scale(&trans, "Transform/Scale");
                EDITOR.History.SetPropertyValue(scale, origTransform.scale, trans.scale);
            }
            firstmove = true;
        }
    }
}

void EditorScene::Exit()
{

}

void EditorScene::CallbackEditorWindow(EditorWindowEvent* pEvent)
{
    // If not scene window
    if (pEvent->name.compare("Scene"))
        return;

    pEvent->isOpened = WindowOpened();
    pEvent->isHovered = WindowHovered();
    pEvent->isFocused = WindowFocused();
}