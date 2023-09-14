/*!***************************************************************************************
\file			EditorScene.cpp
\project		GAM300
\author			Sean Ngo

\par			Course : GAM300
\date           04/09/2023

\brief
	This file contains the definitions of the following :
	1. Editor Scene

All content © 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/
#include "Precompiled.h"

#include "EditorHeaders.h"

// Bean: Need this to reference the editor camera's framebuffer
#include "../Graphics/Editor_Camera.h"

void EditorScene::Init()
{
    sceneDimension = glm::vec2(1600.f, 900.f);
    scenePosition = glm::vec2(0.f, 0.f);
}

void EditorScene::Update(float dt)
{
    if (ImGui::Begin("Scene"))
    {
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

    }
    ImGui::End();
}

void EditorScene::Exit()
{

}