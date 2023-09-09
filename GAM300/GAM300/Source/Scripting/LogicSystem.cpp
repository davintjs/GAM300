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
#include "LogicSystem.h"
#include "Scene/SceneManager.h"
#include <Core/EventsManager.h>
#include "glm/gtc/matrix_transform.hpp"
#include "ScriptingSystem.h"

void LogicSystem::Init()
{
	EVENTS.Subscribe(this, &LogicSystem::CallbackSceneStart);
}

void LogicSystem::Update(float dt)
{
	Scene& scene = MySceneManager.GetCurrentScene();
	PRINT("THE LOGICS RUNNING\n");
	auto& scripts = scene.GetComponentsArray<Script>();
	for (auto it = scripts.begin(); it != scripts.end(); ++it)
	{
		Script& script = *it;
		Entity& entity = scene.GetEntity(script);
		if (!scene.IsActive(entity))
			continue;
		if (!it.IsActive())
			continue;
		SCRIPTING.InvokeMethod(script,"Update");
		if (&scene != &MySceneManager.GetCurrentScene())
			return;

			//for (size_t j = 0; j < MyFrameRateController.getSteps(); ++j)
			//{
			//	MyEventSystem->publish(new ScriptInvokeMethodEvent(*pScript,"FixedUpdate"));
			//}
			//if (pScene != sceneManager.get_current_scene())
			//	return
	}

	//GameObject* selected = GetSelectedGameObject();

	/*if (selected)
		PRINT("SELECTED: " << selected->name);*/
	//for (Button& button : pScene->componentArrays.GetArray<Button>())
	//{
	//	if (!button.enabled || !button.gameObj.IsActive())
	//	{
	//		button.state = ButtonState::None;
	//		if (&button == pHoveredBtn)
	//			pHoveredBtn = nullptr;
	//		continue;
	//	}
	//	button.state = GetButtonState(button,selected);
	//	ButtonBehavior(button);
	//}

	//if (!MyInputSystem.is_mousebutton_pressed(0))
	//{
	//	mouseHeld = false;
	//}
}

void LogicSystem::Exit()
{

}

void LogicSystem::CallbackSceneStart(SceneStartEvent* pEvent)
{
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& scripts = scene.GetComponentsArray<Script>();
	for (auto it = scripts.begin(); it != scripts.end(); ++it)
	{
		Script& script = *it;
		Entity& entity = scene.GetEntity(script);
		if (!scene.IsActive(entity))
			continue;
		if (!it.IsActive())
			continue;
		SCRIPTING.InvokeMethod(script, "Awake");
		if (&scene != &MySceneManager.GetCurrentScene())
			return;
	}

	for (auto it = scripts.begin(); it != scripts.end(); ++it)
	{
		Script& script = *it;
		Entity& entity = scene.GetEntity(script);
		if (!scene.IsActive(entity))
			continue;
		if (!it.IsActive())
			continue;
		SCRIPTING.InvokeMethod(script, "Start");
		if (&scene != &MySceneManager.GetCurrentScene())
			return;
	}
}