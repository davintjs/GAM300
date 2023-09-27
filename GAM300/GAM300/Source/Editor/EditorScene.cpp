/*!***************************************************************************************
\file			EditorScene.cpp
\project		
\author			Sean Ngo

\par			Course : GAM300
\date           04/09/2023

\brief
	This file contains the definitions of the following :
	1. Editor Scene

All content � 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/
#include "Precompiled.h"

#include "EditorHeaders.h"
#include "ImGuizmo.h"
#include "Scene/SceneManager.h"
#include <glm/gtx/matrix_decompose.hpp>

// Bean: Need this to reference the editor camera's framebuffer
#include "../Graphics/Editor_Camera.h"
#include "Editor.h"

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
}

void EditorScene::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

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

        buttonColor = (toggled == 1) ? toggledColor : untoggledColor;          
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        ImGui::SameLine(); if (ImGui::Button("Q", btn) || (ImGui::IsKeyPressed(ImGuiKey_Q) && windowHovered))
        {
            GizmoType = ImGuizmo::UNIVERSAL;
            toggled = 1;
        }
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
        ImGui::SameLine(); if (ImGui::Button("E", btn)
            || (ImGui::IsKeyPressed(ImGuiKey_E) && windowHovered))
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
            GizmoType = (coord_selection) ? ImGuizmo::SCALEU : ImGuizmo::SCALE;
            toggled = 4;
        }
        ImGui::PopStyleColor();

        ImGui::SameLine(); ImGui::Dummy(ImVec2(30.0f, 0.f));

        //For thoe to change to toggle debug drawing
        ImGui::SameLine(); if (ImGui::Checkbox("Debug Drawing", &debug_draw));
    }
    ImGui::End();

    if (ImGui::Begin("Game")) 
    {
        // Bean: For Game Window
        //float padding = 16.f;
        //float AspectRatio = 16.f / 9.f;

        //if (_newDimension.x != 0 && _newDimension.y != 0)
        //{
        //	bool modified = false;
        //	_newDimension = glm::floor(_newDimension);

        //	glm::vec2 adjusted = sceneDimension;
        //	if (adjusted.y > _newDimension.y || adjusted.y != _newDimension.y)
        //	{
        //		modified = true;
        //		adjusted = { _newDimension.y * AspectRatio, _newDimension.y };
        //	}

        //	if (adjusted.x > _newDimension.x - padding)
        //	{
        //		modified = true;
        //		adjusted = { _newDimension.x - padding, (_newDimension.x - padding) / AspectRatio };
        //	}

        //	// If there is any changes to the dimension and modifications, return
        //	if (adjusted != sceneDimension && modified)
        //	{
        //		sceneDimension = adjusted;

        //		EditorCam.onResize(sceneDimension.x, sceneDimension.y);

        //		EditorCam.getFramebuffer().resize(sceneDimension.x, sceneDimension.y);
        //	}
        //}

        /*float indent = (viewportEditorSize.x - sceneDimension.x) * 0.5f;
        if (indent > 0)
            ImGui::Indent(indent);*/
    }
    ImGui::End();

    //Editor scene viewport
    if (ImGui::Begin("Scene"))
    {
        windowHovered = ImGui::IsWindowHovered();
        scenePosition = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
        unsigned int textureID = EditorCam.getFramebuffer().get_color_attachment_id();
        ImVec2 viewportEditorSize = ImGui::GetContentRegionAvail();
        glm::vec2 _newDimension = *((glm::vec2*)&viewportEditorSize);

        // Only if the current scene dimension is not the same as new dimension
        if (sceneDimension != _newDimension && _newDimension.x != 0 && _newDimension.y != 0)
        {
            sceneDimension = { _newDimension.x, _newDimension.y };
            EditorCam.onResize(sceneDimension.x, sceneDimension.y);

            EditorCam.getFramebuffer().resize(sceneDimension.x, sceneDimension.y);
        }

        ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)sceneDimension.x, (float)sceneDimension.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();	
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();

        // Might be wrong -> i think here is the one that need to offset the tab header if there is
        ImGuizmo::SetRect((float)ImGui::GetWindowPos().x, (float)ImGui::GetWindowPos().y+22.f, windowWidth, windowHeight-22.f);
        //
        //std::cout << "SetRect x :" << (float)ImGui::GetWindowPos().x << "\n";
        //std::cout << "SetRect y :" << (float)ImGui::GetWindowPos().y << "\n";

        //
        //ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        //ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        //std::cout << "min :" << vMin.x << " , " << vMin.y << "\n";
        //std::cout << "max :" << vMax.x << " , " << vMax.y << "\n";

        Scene& currentScene = SceneManager::Instance().GetCurrentScene();
        Entity& entity = currentScene.Get<Entity>(EDITOR.GetSelectedEntity());
        // if (EDITOR.GetSelectedEntity() != 0)
        if (&entity)
        {
            Transform& trans = currentScene.Get<Transform>(entity);
            for (int i = 0; i < 3; ++i)
            {
                if (fabs(trans.scale[i]) < 0.001f)
                    trans.scale[i] = 0.001f;
            }

            glm::mat4 transform_1 = trans.GetWorldMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(EditorCam.getViewMatrix()), glm::value_ptr(EditorCam.getPerspMatrix()),
                (ImGuizmo::OPERATION)GizmoType, (ImGuizmo::MODE)coord_selection, glm::value_ptr(transform_1));

            if (ImGuizmo::IsUsing())
            {
                EditorCam.canMove = false;
                if (trans.parent)
                {
                    Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(trans.parent);
                    glm::mat4 parentTransform = parentTrans.GetWorldMatrix();
                    transform_1 = glm::inverse(parentTransform) * transform_1;
                }
                glm::vec3 a_translation;
                glm::quat a_rot;
                glm::vec3 a_scale;
                glm::vec3 a_skew;
                glm::vec4 a_perspective;
                glm::decompose(transform_1, a_scale, a_rot, a_translation, a_skew, a_perspective);

                trans.translation = a_translation;
                trans.rotation = glm::eulerAngles(a_rot);
                trans.scale = a_scale;
             
            }
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();

    inOperation = ImGuizmo::IsOver() && EditorHierarchy::Instance().selectedEntity != NON_VALID_ENTITY;
}

void EditorScene::Exit()
{

}