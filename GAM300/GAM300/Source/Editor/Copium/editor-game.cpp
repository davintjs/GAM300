/*!***************************************************************************************
\file			editor-game.cpp
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			21/11/2022

\brief
	This file holds the definitions of the functions in the editor game gui where the
	user can play the game in this GUI

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#include "pch.h"

#include "Editor/editor-game.h"
#include "Editor/editor-system.h"
#include "SceneManager/scene-manager.h"
#include "Windows/windows-system.h"

#include "Graphics/graphics-system.h"

#define AspectRatio (16.f/9.f)

namespace Copium
{
	namespace
	{
		Camera* gameCamera = nullptr;

		float padding = 16.f;
	}

	void EditorGame::init()
	{
		sceneDimension = { sceneWidth, sceneHeight };
	}

	void EditorGame::update()
	{
		// Game view settings
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

		// Begin Game View
		if (ImGui::Begin("Game", 0, windowFlags))
		{
			windowFocused = ImGui::IsWindowFocused();
			windowHovered = ImGui::IsWindowHovered();
			scenePosition = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

			unsigned int textureID = 0;
			Scene* currScene{ MySceneManager.get_current_scene() };
			if (currScene && !currScene->componentArrays.GetArray<Camera>().empty())
			{
				gameCamera = &*currScene->componentArrays.GetArray<Camera>().begin();
				//gameCamera->SetCameraPosition(gameCamera->gameObj.transform.position);
				textureID = gameCamera->get_framebuffer()->get_color_attachment_id();
			}

			ImVec2 viewportEditorSize = ImGui::GetContentRegionAvail();
			resize_game(*((glm::vec2*)&viewportEditorSize));
			indent = (viewportEditorSize.x - sceneWidth) * 0.5f;
			if (indent > 0)
				ImGui::Indent(indent);
			ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)sceneWidth, (float)sceneHeight }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });

			// Game Camera
			Scene* scene = MySceneManager.get_current_scene();
			if (scene && !scene->componentArrays.GetArray<Camera>().empty())
			{
				scene->componentArrays.GetArray<Camera>()[0].update();
			}
		}
		
		ImGui::End();
		ImGui::PopStyleVar();
		// End Game View
	}

	void EditorGame::exit()
	{

	}

	void EditorGame::resize_game(glm::vec2 _newDimension)
	{
		// Only resize if the window is focused
		if (!WindowsSystem::Instance()->get_window_focused())
			return;

		// Only if the current scene dimension is not the same as new dimension
		if (_newDimension.x != 0 && _newDimension.y != 0)
		{
			bool modified = false;
			_newDimension = glm::floor(_newDimension);

			glm::vec2 adjusted  = sceneDimension;
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

			// If there isnt any changes to the dimension or no modifications, return
			if (adjusted == sceneDimension || !modified)
				return;

			sceneDimension = adjusted;
			sceneWidth = (int) sceneDimension.x;
			sceneHeight = (int) sceneDimension.y;

			if(gameCamera)
				gameCamera->on_resize(sceneDimension.x, sceneDimension.y);
		}
	}
}