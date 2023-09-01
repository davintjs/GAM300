/*!***************************************************************************************
\file			editor-sceneview.cpp
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			16/09/2022

\brief
	This file holds the definition of functions for the scene view of the editor, where
	the user can view the scene.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#include "pch.h"
#include <GL/glew.h>
#include "Editor/editor-sceneview.h"
#include "Editor/editor-system.h"
#include "Windows/windows-system.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>
#include "Animation/animation-system.h"
#include <GameObject/game-object.h>

namespace Copium
{
	namespace
	{
		EditorCamera& camera = *(MyEditorSystem.get_camera());
		bool inOp = false;
	}

	void EditorSceneView::init()
	{
		sceneDimension = { sceneWidth, sceneHeight };
	}

	glm::vec2 update_position(Transform* _t, glm::vec2 const& _pos)
	{
		glm::vec2 tempPos = _pos;
		if (_t->HasParent())
		{
			tempPos += glm::vec2(_t->parent->position.x, _t->parent->position.y);
			tempPos = update_position(_t->parent, tempPos);
		}
		//PRINT("Cur : " << tempPos.x << " " << tempPos.y);
		return tempPos;
	}

	void EditorSceneView::update()
	{
		// Scene view settings
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

		Scene* scene = nullptr;

		// Begin Scene View
		if (ImGui::Begin("Scene View", 0, windowFlags))
		{
			windowFocused = ImGui::IsWindowFocused();
			windowHovered = ImGui::IsWindowHovered();
			scenePosition = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

			unsigned int textureID = camera.get_framebuffer()->get_color_attachment_id();
			ImVec2 viewportEditorSize = ImGui::GetContentRegionAvail();
			resize_sceneview(*((glm::vec2*)&viewportEditorSize));
			ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)sceneWidth, (float)sceneHeight }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });

			// Gizmos
			scene = MySceneManager.get_current_scene();
			update_gizmos();

			// Editor Camera
			camera.update();

		}
		ImGui::End();
		
		ImGui::PopStyleVar();
		// End Scene View

		// Render stats settings
		windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

		// Begin Render Stats
		ImGui::Begin("Renderer Stats", 0, windowFlags);
		
		size_t gameobjectCount = 0;
		if (scene != nullptr)
			gameobjectCount = scene->gameObjects.size();

		ImGui::Text("Render Stats");
		char buffer[64];
		sprintf(buffer, "GameObject Count: %zd", (size_t)gameobjectCount);
		ImGui::Text(buffer);

		sprintf(buffer, "Viewport Dimensions: %d by %d", sceneWidth, sceneHeight);
		ImGui::Text(buffer);

		sprintf(buffer, "Draw Count: %d", camera.getDraw()->get_renderer()->getDrawCount());
		ImGui::Text(buffer);

		sprintf(buffer, "Quad Count: %d", camera.getDraw()->get_renderer()->getQuadCount());
		ImGui::Text(buffer);

		sprintf(buffer, "Line Count: %d", camera.getDraw()->get_renderer()->getLineCount());
		ImGui::Text(buffer);

		sprintf(buffer, "Circle Count: %d", camera.getDraw()->get_renderer()->getCircleCount());
		ImGui::Text(buffer);

		sprintf(buffer, "Text Count: %d", camera.getDraw()->get_renderer()->getTextCount());
		ImGui::Text(buffer);

		/*sprintf(buffer, "Vertex Count: %d", (int)camera.getDraw()->get_renderer()->getVertices());
		ImGui::Text(buffer);*/

		// End Render Stats
		ImGui::End();

		// Mouse picking
		bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
		if (scene && !inOp && mouseReleased && windowHovered)
		{
			GameObjectsPtrArray pGameObjs; // Possible selectable gameobjects
			for (GameObject& gameObject : scene->gameObjects)
			{
				if (!gameObject.IsActive())
					continue;

				Transform& t = gameObject.transform;
				glm::vec2 mousePosition = camera.get_ndc();
				glm::vec3 tempPos = t.position;
				glm::vec3 tempScale = t.scale;

				if (t.HasParent())
				{
					Transform* tempObj = t.parent;

					glm::mat4 pTranslate, pRotate, pScale, pTransform;

					while (tempObj)
					{
						pTranslate = glm::translate(glm::mat4(1.f), tempObj->position.glmVec3);

						float pRot = glm::radians(tempObj->rotation.z);
						pRotate = {
							glm::vec4(cos(pRot), sin(pRot), 0.f, 0.f),
							glm::vec4(-sin(pRot), cos(pRot), 0.f, 0.f),
							glm::vec4(0.f, 0.f, 1.f, 0.f),
							glm::vec4(0.f, 0.f, 0.f, 1.f)
						};

						glm::vec3 scale = tempObj->scale.glmVec3;
						pScale = {
							glm::vec4(scale.x, 0.f, 0.f, 0.f),
							glm::vec4(0.f, scale.y, 0.f, 0.f),
							glm::vec4(0.f, 0.f, 1.f, 0.f),
							glm::vec4(0.f, 0.f, 0.f, 1.f)
						};

						pTransform = pTranslate * pRotate * pScale;
						tempPos = glm::vec3(pTransform * glm::vec4(tempPos, 1.f));
						tempScale *= tempObj->scale.glmVec3;
						tempObj = tempObj->parent;
					}
				}

				glm::vec2 objPosition = { tempPos.x, tempPos.y };

				// Not Within bounds
				if (glm::distance(objPosition, mousePosition)
					> glm::length(camera.get_dimension()))
					continue;

				glm::vec2 min, max;
				AABB bound;
					
				min = glm::vec2(objPosition.x - tempScale.x * 0.5f, objPosition.y - tempScale.y * 0.5f);
				max = glm::vec2(objPosition.x + tempScale.x * 0.5f, objPosition.y + tempScale.y * 0.5f);
				float tempX = 0.f, tempY = 0.f;

				if (gameObject.HasComponent<SpriteRenderer>())
				{
					SpriteRenderer* spriteRenderer = gameObject.GetComponent<SpriteRenderer>();

					if (!spriteRenderer->enabled)
						continue;

					Texture* texture = spriteRenderer->sprite.refTexture;
					
					if (texture != nullptr)
					{
						tempX = tempScale.x * texture->get_pixel_width();
						tempY = tempScale.y * texture->get_pixel_height();
						
						min = glm::vec2(objPosition.x - tempX * 0.5f, objPosition.y - tempY * 0.5f);
						max = glm::vec2(objPosition.x + tempX * 0.5f, objPosition.y + tempY * 0.5f);
					}
				}

				if (gameObject.HasComponent<Image>())
				{
					Image* image = gameObject.GetComponent<Image>();

					if (!image->enabled)
						continue;
					Texture* texture = image->sprite.refTexture;

					if (texture != nullptr)
					{
						tempX = tempScale.x * texture->get_pixel_width();
						tempY = tempScale.y * texture->get_pixel_height();

						min = glm::vec2(objPosition.x - tempX * 0.5f, objPosition.y - tempY * 0.5f);
						max = glm::vec2(objPosition.x + tempX * 0.5f, objPosition.y + tempY * 0.5f);
					}

				}

				//for (Animator* animator : gameObject.GetComponents<Animator>())
				//{
				//	if (!animator->enabled)
				//		continue;
				//	int columns = animator->GetCurrentAnimation()->spriteSheet.columns;
				//	int rows = animator->GetCurrentAnimation()->spriteSheet.rows;
				//	float tempX = 0.f, tempY = 0.f;
				//	if (animator->GetCurrentAnimation()->spriteSheet.GetTexture() != nullptr)
				//	{
				//		float width = (float)animator->GetCurrentAnimation()->spriteSheet.GetTexture()->get_width() / (float) columns;
				//		float height = (float)animator->GetCurrentAnimation()->spriteSheet.GetTexture()->get_height() / (float) rows;
				//		float multiplier = width / (float)WindowsSystem::Instance()->get_window_width();
				//		tempX = tempScale.x * (width / (float)height) * multiplier * 0.5f;
				//		if (width == height)
				//			tempY = tempScale.y * (width / (float)height) * multiplier * 0.5f;
				//		else
				//			tempY = tempScale.y * multiplier * 0.5f;
				//	}
				//	else
				//		break;

				//	min = glm::vec2(objPosition.x - tempX, objPosition.y - tempY);
				//	max = glm::vec2(objPosition.x + tempX, objPosition.y + tempY);
				//}

				// Check AABB
				if (mousePosition.x > min.x && mousePosition.x < max.x)
				{
					if (mousePosition.y > min.y && mousePosition.y < max.y)
					{
						pGameObjs.push_back(&gameObject);
					}
				}
			}

			// Ensure that the container is not empty
			if (!pGameObjs.empty())
			{
				// Sort base on depth value
				std::sort(pGameObjs.begin(), pGameObjs.end(),
					[](GameObject* lhs, GameObject* rhs) -> bool
					{return (lhs->transform.position.z > rhs->transform.position.z); });

				bool selected = false;
				for (int i = 0; i < pGameObjs.size(); i++)
				{
					// Get the next gameobject after
					if (MyEditorSystem.pSelectedGameObject == pGameObjs[i])
					{
						if (i + 1 < pGameObjs.size())
						{
							MyEditorSystem.pSelectedGameObject = pGameObjs[i + 1];
							selected = true;
							break;
						}
						else if (i + 1 >= pGameObjs.size())
						{
							MyEditorSystem.pSelectedGameObject = pGameObjs[0];
							selected = true;
							break;
						}
					}
				}

				// If there is no selected gameobject
				if (MyEditorSystem.pSelectedGameObject == nullptr || !selected)
				{
					GameObject* selectObject = pGameObjs.front();
					for (GameObject* gameObject : pGameObjs)
					{
						// Select closest gameobject
						float depth = gameObject->transform.position.z;

						if (depth > selectObject->transform.position.z)
						{
							selectObject = gameObject;
						}
					}
					MyEditorSystem.pSelectedGameObject = (selectObject);
				}

				//PRINT("Set object to: " << MySceneManager.selectedGameObject->get_name());
			}
			else
				MyEditorSystem.pSelectedGameObject = nullptr;
		}

		inOp = ImGuizmo::IsUsing();
	}

	void EditorSceneView::exit()
	{
		
	}

	void EditorSceneView::resize_sceneview(glm::vec2 _newDimension)
	{
		// Only resize if the window is focused
		if (!WindowsSystem::Instance()->get_window_focused())
			return;

		// Only if the current scene dimension is not the same as new dimension
		if (sceneDimension != _newDimension && _newDimension.x != 0 && _newDimension.y != 0)
		{
			sceneDimension = { _newDimension.x, _newDimension.y };
			sceneWidth = (int)sceneDimension.x;
			sceneHeight = (int)sceneDimension.y;
			camera.on_resize(sceneDimension.x, sceneDimension.y);
		}
	}

	void EditorSceneView::update_gizmos()
	{
		static ImGuizmo::OPERATION currop = ImGuizmo::OPERATION::TRANSLATE;
		GameObject* currObj = MyEditorSystem.pSelectedGameObject;
		if (currObj)
		{
			Transform& trf = currObj->transform;
			glm::vec2 size(trf.scale.x, trf.scale.y);
			float rot = glm::radians(trf.rotation.z);
			const glm::vec3& pos = trf.position;

			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)sceneWidth, (float)sceneHeight + 50.f);
			//glm::mat4 camProj = camera.get_projection();

			// Offset the zl for bottom and top because of the scene view window bar height of 50.f
			float ar = camera.GetAspect();
			float nearClip = camera.GetNearClip();
			float farClip = camera.GetFarClip(); 
			float zl = camera.get_zoom();
			float x = (sceneHeight + 50.f) / (float)sceneHeight;
			glm::mat4 camProj = glm::ortho(-ar * zl, ar * zl, -zl * x, zl * x, nearClip, farClip);
			glm::mat4 camView = camera.get_view_matrix();

			glm::mat4 translate = glm::translate(glm::mat4(1.f), pos);
			glm::mat4 lRotation = {
			glm::vec4(cos(rot), sin(rot), 0.f, 0.f),
			glm::vec4(-sin(rot), cos(rot), 0.f, 0.f),
			glm::vec4(0.f, 0.f, 1.f, 0.f),
			glm::vec4(0.f, 0.f, 0.f, 1.f)
			};

			glm::mat4 lScale = {
				glm::vec4(size.x, 0.f, 0.f, 0.f),
				glm::vec4(0.f, size.y, 0.f, 0.f),
				glm::vec4(0.f, 0.f, 1.f, 0.f),
				glm::vec4(0.f, 0.f, 0.f, 1.f)
			};
			glm::mat4 transform = translate * lRotation * lScale;

			glm::mat4 pTranslate, pRotate, pScale, pTransform, totalTransform, postTransform;
			glm::vec3 postScale = { 1.f, 1.f, 1.f };
			float postRotation = 0.f;
			if (currObj->transform.HasParent())
			{
				glm::vec3 position = currObj->transform.position;
				glm::vec3 scale = currObj->transform.scale;
				float rotation = currObj->transform.rotation.z;
				totalTransform = postTransform = glm::identity<glm::mat4>();
				Transform* tempObj = currObj->transform.parent;

				while (tempObj)
				{
					pTranslate = glm::translate(glm::mat4(1.f), tempObj->position.glmVec3);

					float pRot = glm::radians(tempObj->rotation.z);
					pRotate = {
						glm::vec4(cos(pRot), sin(pRot), 0.f, 0.f),
						glm::vec4(-sin(pRot), cos(pRot), 0.f, 0.f),
						glm::vec4(0.f, 0.f, 1.f, 0.f),
						glm::vec4(0.f, 0.f, 0.f, 1.f)
					};

					pScale = {
						glm::vec4(tempObj->scale.x, 0.f, 0.f, 0.f),
						glm::vec4(0.f, tempObj->scale.y, 0.f, 0.f),
						glm::vec4(0.f, 0.f, 1.f, 0.f),
						glm::vec4(0.f, 0.f, 0.f, 1.f)
					};

					pTransform = pTranslate * pRotate * pScale;
					postTransform = postTransform * glm::inverse(pTransform);
					postScale /= tempObj->scale.glmVec3;
					postRotation += tempObj->rotation.z;

					position = glm::vec3(pTransform * glm::vec4(position, 1.f));
					scale *= tempObj->scale.glmVec3;
					rotation += tempObj->rotation.z;

					tempObj = tempObj->parent;
				}
				
				pTranslate = glm::translate(glm::mat4(1.f), position);

				float pRot = glm::radians(rotation);
				pRotate = {
					glm::vec4(cos(pRot), sin(pRot), 0.f, 0.f),
					glm::vec4(-sin(pRot), cos(pRot), 0.f, 0.f),
					glm::vec4(0.f, 0.f, 1.f, 0.f),
					glm::vec4(0.f, 0.f, 0.f, 1.f)
				};

				pScale = {
					glm::vec4(scale.x, 0.f, 0.f, 0.f),
					glm::vec4(0.f, scale.y, 0.f, 0.f),
					glm::vec4(0.f, 0.f, 1.f, 0.f),
					glm::vec4(0.f, 0.f, 0.f, 1.f)
				};

				totalTransform = pTranslate * pRotate * pScale;
			}
			else
			{
				totalTransform = transform;
			}

			if (ImGui::IsWindowHovered())
			{
				if (ImGui::IsKeyReleased(ImGuiKey_W))
				{
					currop = ImGuizmo::OPERATION::TRANSLATE;

				}
				if (ImGui::IsKeyReleased(ImGuiKey_R))
				{
					currop = ImGuizmo::OPERATION::SCALE;

				}
				if (ImGui::IsKeyReleased(ImGuiKey_E))
				{
					currop = ImGuizmo::OPERATION::ROTATE;
				}
			}
			
			ImGuizmo::Manipulate(glm::value_ptr(camView), glm::value_ptr(camProj),
				currop, ImGuizmo::LOCAL, glm::value_ptr(totalTransform));
			if (ImGuizmo::IsUsing())
			{
				float newTranslate[3]; 
				float newRotate[3];
				float newScale[3];
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(totalTransform), newTranslate, newRotate, newScale);
				glm::vec3 nTranslation = { newTranslate[0], newTranslate[1], newTranslate[2] };
				glm::vec3 nScale = { newScale[0], newScale[1], 1.f};

				if (currObj->transform.HasParent())
				{
					nTranslation = glm::vec3(postTransform * glm::vec4(nTranslation, 1.f));
					nScale *= postScale;
					newRotate[2] -= postRotation;
				}

				trf.position = nTranslation;
				trf.scale = nScale;
				trf.rotation.z = newRotate[2];
			}
		}
	}
}
