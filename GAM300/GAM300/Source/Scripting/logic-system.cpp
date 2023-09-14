/*!***************************************************************************************
\file			logic-system.cpp
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			30/10/2022

\brief
	This file defines the functions for the Logic System

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "Precompiled.h"
#include "Scripting/logic-system.h"
#include <Windows/windows-input.h>
#include "Scene/SceneManager.h"
#include <Windows/windows-system.h>
#include <Debugging/frame-rate-controller.h>
#include <Events/events-system.h>
#include <GameObject/components.h>
#include <Utilities/easing.h>
#include "Editor/editor-system.h"
#include <Graphics/graphics-system.h>
#include "glm/gtc/matrix_transform.hpp"

	namespace
	{
		SceneManager& sceneManager {SceneManager::Instance()};
		double timeElasped;
		bool inPlayMode{ false };
		//Button* pHoveredBtn{ nullptr };
		//bool mouseHeld = false;
	}

	//ButtonState GetButtonState(Button& btn, GameObject* selectedGameObject)
	//{
	//	if (selectedGameObject != &btn.gameObj)
	//	{
	//		if (btn.state == ButtonState::None || btn.state == ButtonState::OnRelease || btn.state == ButtonState::OnHover)
	//		{
	//			return ButtonState::None;
	//		}
	//	}
	//	glm::vec2 scenePos = MySceneManager.mainCamera->get_game_ndc();
	//	Transform& transform{ btn.gameObj.transform };

	//	//if (MyInputSystem.is_mousebutton_pressed(0) && pHoveredBtn == nullptr)
	//	//{
	//	//	return ButtonState::None;
	//	//}
	//	//else
	//	if (pHoveredBtn == nullptr)
	//	{
	//		//PRINT("GETTING BUTTON STATE");
	//		//PRINT(scenePos.x << " , " << scenePos.y);
	//		AABB relativeBounds = btn.bounds.GetRelativeBounds(transform.GetWorldPosition(), transform.GetWorldScale());
	//		//PRINT("X: " << relativeBounds.max.x << " , " << relativeBounds.min.x);
	//		//PRINT("Y: " << relativeBounds.max.y << " , " << relativeBounds.min.y);

	//		if (static_collision_pointrect(scenePos, relativeBounds))
	//		{
	//			//PRINT("COLLIDED");
	//			if (!mouseHeld && MyInputSystem.is_mousebutton_pressed(0))
	//			{
	//				if (pHoveredBtn)
	//				{
	//					pHoveredBtn = nullptr;
	//				}
	//				pHoveredBtn = &btn;
	//				mouseHeld = true;
	//				if (btn.state == ButtonState::OnClick || btn.state == ButtonState::OnHeld)
	//					return ButtonState::OnHeld;
	//				return ButtonState::OnClick;
	//			}
	//			return ButtonState::OnHover;
	//		}
	//	}
	//	else if (pHoveredBtn == &btn)
	//	{
	//		if (!MyInputSystem.is_mousebutton_pressed(0))
	//		{
	//			AABB relativeBounds = btn.bounds.GetRelativeBounds(transform.GetWorldPosition(), transform.GetWorldScale());
	//			pHoveredBtn = nullptr;
	//			if (static_collision_pointrect(scenePos, relativeBounds) && selectedGameObject == &btn.gameObj)
	//				return ButtonState::OnRelease;
	//			return ButtonState::None;
	//		}
	//		if (btn.state == ButtonState::OnClick || btn.state == ButtonState::OnHeld)
	//			return ButtonState::OnHeld;
	//		return ButtonState::OnClick;
	//	}
	//	return ButtonState::None;
	//}

	//void ButtonBehavior(Button& btn)
	//{
	//	if (btn.previousState != btn.state)
	//	{
	//		btn.timer = 0;
	//		btn.previousState = btn.state;
	//		if (btn.targetGraphic)
	//			btn.previousColor = btn.targetGraphic->layeredColor;
	//	}

	//	switch (btn.state)
	//	{
	//	case ButtonState::OnClick:
	//	{
	//		//PRINT("UI: Clicked " << btn.gameObj.name);
	//		if (btn.targetGraphic)
	//		{
	//			btn.targetGraphic->layeredColor = Linear(btn.previousColor, btn.clickedColor, btn.timer / btn.fadeDuration);
	//		}
	//		break;
	//	}
	//	case ButtonState::OnHover:
	//	{
	//		//PRINT("UI: Hover " << btn.gameObj.name);
	//		if (btn.targetGraphic)
	//		{
	//			btn.targetGraphic->layeredColor = Linear(btn.previousColor, btn.hoverColor, btn.timer / btn.fadeDuration);
	//			//PRINT("R " << btn.targetGraphic->layeredColor.r);
	//			//PRINT("G " << btn.targetGraphic->layeredColor.g);
	//			//PRINT("B " << btn.targetGraphic->layeredColor.b);
	//			//PRINT("A " << btn.targetGraphic->layeredColor.a);
	//		}
	//		break;
	//	}
	//	case ButtonState::OnHeld:
	//	{
	//		//PRINT("UI: Held " << btn.gameObj.name);
	//		if (btn.targetGraphic)
	//		{
	//			btn.targetGraphic->layeredColor = Linear(btn.previousColor, btn.clickedColor, btn.timer / btn.fadeDuration);
	//		}
	//		break;
	//	}
	//	case ButtonState::OnRelease:
	//	{
	//		//PRINT("UI: Released " << btn.gameObj.name);
	//		break;
	//	}
	//	default:
	//	{
	//		//PRINT("UI: NONE " << btn.gameObj.name);
	//		if (btn.targetGraphic)
	//		{
	//			btn.targetGraphic->layeredColor = Linear(btn.previousColor, btn.normalColor, btn.timer / btn.fadeDuration);
	//		}
	//		break;
	//	}
	//	}
	//	if (btn.targetGraphic == nullptr)
	//		return;
	//	if (btn.timer < btn.fadeDuration)
	//		btn.timer += (float)MyFrameRateController.getDt();
	//	else if (btn.timer > btn.fadeDuration)
	//		btn.timer = btn.fadeDuration;
	//}

	//GameObject* GetSelectedGameObject()
	//{
	//	GameObject* selectedGameObject{nullptr};
	//	SortingGroup* selectedSg{nullptr};
	//	Scene* pScene = sceneManager.get_current_scene();
	//	if (!pScene)
	//		return nullptr;
	//	if (pScene->componentArrays.GetArray<Camera>().empty())
	//		return nullptr;

	//	Camera& gameCamera{pScene->componentArrays.GetArray<Camera>()[0]};
	//	glm::vec2 mousePosition = gameCamera.get_game_ndc();
	//	for (Image& image : pScene->componentArrays.GetArray<Image>())
	//	{
	//		GameObject& gameObject {image.gameObj};
	//		if (!gameObject.IsActive() || !image.enabled)
	//			continue;
	//		Transform& t = gameObject.transform;
	//		Math::Vec3 worldPos{ t.GetWorldPosition() };
	//		Math::Vec3 worldScale{ t.GetWorldScale() };
	//		Texture* texture = image.sprite.refTexture;
	//		AABB bound{};
	//		if (texture != nullptr)
	//		{
	//			Math::Vec2 max = { texture->get_pixel_width() / 2.f,texture->get_pixel_height() / 2.f };
	//			Math::Vec2 min = { -texture->get_pixel_width() / 2.f , -texture->get_pixel_height() / 2.f };
	//			bound = AABB(min, max);
	//		}
	//		bound = bound.GetRelativeBounds(worldPos, worldScale);
	//		
	//		if (static_collision_pointrect(mousePosition, bound))
	//		{ 
	//			//If currently no selected Gameobject
	//			if (!selectedGameObject)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = selectedGameObject->GetComponent<SortingGroup>();
	//				continue;
	//			}
	//			//If somehow they are on the same object
	//			if (selectedGameObject == &gameObject)
	//				continue;
	//			SortingGroup* sg = gameObject.GetComponent<SortingGroup>();
	//			//If there is no sorting group, dont bother
	//			if (!sg)
	//				continue;
	//			//If there is a sorting group but selectedSg has none, override
	//			if (!selectedSg)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = sg;
	//				continue;
	//			}
	//			//If current sorting layer is higher
	//			if (sg->sortingLayer > selectedSg->sortingLayer)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = sg;
	//				continue;
	//			}
	//			//If same layer but higher in order
	//			else if (sg->sortingLayer == selectedSg->sortingLayer && sg->orderInLayer > selectedSg->orderInLayer)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = sg;
	//				continue;
	//			}
	//		}
	//	}
	//	for (Button& button : pScene->componentArrays.GetArray<Button>())
	//	{
	//		GameObject& gameObject{ button.gameObj };
	//		if (!gameObject.IsActive() || !button.enabled)
	//			continue;
	//		Transform& t = gameObject.transform;
	//		Math::Vec3 worldPos{ t.GetWorldPosition()};
	//		Math::Vec3 worldScale{ t.GetWorldScale() };
	//		AABB bound = button.bounds.GetRelativeBounds(worldPos, worldScale);
	//		if (static_collision_pointrect(mousePosition, bound))
	//		{
	//			//If currently no selected Gameobject
	//			if (!selectedGameObject)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = selectedGameObject->GetComponent<SortingGroup>();
	//				continue;
	//			}
	//			//If somehow they are on the same object
	//			if (selectedGameObject == &gameObject)
	//				continue;
	//			SortingGroup* sg = gameObject.GetComponent<SortingGroup>();
	//			//If there is no sorting group, dont bother
	//			if (!sg)
	//				continue;
	//			//If there is a sorting group but selectedSg has none, override
	//			if (!selectedSg)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = sg;
	//				continue;
	//			}
	//			//If current sorting layer is higher
	//			if (sg->sortingLayer > selectedSg->sortingLayer)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = sg;
	//				continue;
	//			}
	//			//If same layer but higher in order
	//			else if (sg->sortingLayer == selectedSg->sortingLayer && sg->orderInLayer > selectedSg->orderInLayer)
	//			{
	//				selectedGameObject = &gameObject;
	//				selectedSg = sg;
	//				continue;
	//			}
	//		}
	//	}
	//	// Ensure that the container is not empty
	//	// Sort base on depth value
	//	// If there is no selected gameobject
	//	// Update button behaviour for layered game objects
	//	// Update from back to front within layer
	//	//std::vector<Layer>& sortingLayers{ MyEditorSystem.getLayers()->SortLayers()->GetSortingLayers() };
	//	//Iterate through list of possible gameObjects
	//	return selectedGameObject;
	//}


	void LogicSystem::init()
	{
		MyEventSystem->subscribe(this, &LogicSystem::CallbackSceneLinked);
	}

	void LogicSystem::update()
	{
		Scene* pScene = sceneManager.get_current_scene();
		if (pScene == nullptr)
			return;


		for (Script& script : pScene->componentArrays.GetArray<Script>())
		{
			if (!script.enabled)
				continue;
			if (!script.gameObj.IsActive())
				continue;
			MyEventSystem->publish(new ScriptInvokeMethodEvent(script, "Update"));
			if (pScene != sceneManager.get_current_scene())
				return;

				//for (size_t j = 0; j < MyFrameRateController.getSteps(); ++j)
				//{
				//	MyEventSystem->publish(new ScriptInvokeMethodEvent(*pScript,"FixedUpdate"));
				//}
				//if (pScene != sceneManager.get_current_scene())
				//	return
		}

		GameObject* selected = GetSelectedGameObject();

		/*if (selected)
			PRINT("SELECTED: " << selected->name);*/
		for (Button& button : pScene->componentArrays.GetArray<Button>())
		{
			if (!button.enabled || !button.gameObj.IsActive())
			{
				button.state = ButtonState::None;
				if (&button == pHoveredBtn)
					pHoveredBtn = nullptr;
				continue;
			}
			button.state = GetButtonState(button,selected);
			ButtonBehavior(button);
		}

		if (!MyInputSystem.is_mousebutton_pressed(0))
		{
			mouseHeld = false;
		}
	}

	void LogicSystem::exit()
	{
	}

	void LogicSystem::CallbackSceneLinked(SceneLinkedEvent* pEvent)
	{
		//MT_START_PREVIEW
		if (!inPlayMode)
			return;
		PRINT("CALLED START AGAIN!");
		Scene& scene = pEvent->scene;
		pHoveredBtn = nullptr;
		for (Script& script : scene.componentArrays.GetArray<Script>())
		{
			MyEventSystem->publish(new ScriptInvokeMethodEvent(script, "Awake"));
		}

		for (Script& script : scene.componentArrays.GetArray<Script>())
		{
			MyEventSystem->publish(new ScriptInvokeMethodEvent(script, "Start"));
		}
		timeElasped = MyFrameRateController.getDt();
	}

	void LogicSystem::handleMessage(MESSAGE_TYPE mType)
	{
		//MT_START_PREVIEW
		if (mType == MESSAGE_TYPE::MT_START_PREVIEW)
		{
			inPlayMode = true;
			Scene* pScene = sceneManager.get_current_scene();
			if (pScene == nullptr)
				return;
			for (Script& script : pScene->componentArrays.GetArray<Script>() )
			{
				MyEventSystem->publish(new ScriptInvokeMethodEvent(script, "Awake"));
			}

			for (Script& script : pScene->componentArrays.GetArray<Script>())
			{
				MyEventSystem->publish(new ScriptInvokeMethodEvent(script, "Start"));
			}
			timeElasped = MyFrameRateController.getDt();
		}
		else
		{
			inPlayMode = false;
		}
	}
}