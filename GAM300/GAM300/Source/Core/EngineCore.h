/*!***************************************************************************************
\file			EngineCore.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			01/08/2023

\brief
	NOTE: DO NOT INCLUDE IN ANY OTHER PLACE OTHER THAN MAIN
	This file manages all the systems as a super system itself to call them in sequence

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "Precompiled.h"

#include "FramerateController.h"
#include "Editor/Editor.h"
#include "SystemInterface.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/SceneManager.h"
#include "Scene/Components.h"
#include "Graphics/GraphicsSystem.h"
#include "Audio/AudioSystem.h"
#include "AI/Blackboard.h"
#include "AI/BehaviorTreeBuilder.h"
#include "AI/NavMeshBuilder.h"
#include "IOManager/InputSystem.h"
#include "AssetManager/AssetManager.h"
#include "Utilities/FileWatcher.h"
#include "ImGuizmo.h"
#include "Utilities/ThreadPool.h"
#include "Scripting/ScriptingSystem.h"
#include "Utilities/YAMLUtils.h"
#include "EventsManager.h"
#include "Debugging/Debugger.h"
#include "SystemsGroup.h"
#include "Debugging/DemoSystem.h"
#include "Audio/AudioManager.h"
#include "Graphics/MeshManager.h"

#define MyEngineCore EngineCore::Instance()
#define UPDATE_TIME 1.f;

#if defined(_BUILD)
	using AllSystemsPack =
	TemplatePack
	<
		AssetManager,
		InputSystem,
		SceneManager,
		ScriptingSystem,
		DemoSystem,//RUN AFTER EDITOR
		AudioSystem,
		PhysicsSystem, //AFTER SCRIPTING
		GraphicsSystem,
		Blackboard
	>;
#else
	using AllSystemsPack =
	TemplatePack
	<
		AssetManager,
		InputSystem,
		EditorSystem,
		SceneManager,
		ScriptingSystem,
		DemoSystem,//RUN AFTER EDITOR
		AudioSystem,
		PhysicsSystem, //AFTER SCRIPTING
		GraphicsSystem,
		Blackboard
	>;
#endif

using AllSystems = decltype(SystemsGroup(AllSystemsPack()));

ENGINE_SYSTEM(EngineCore)
{
public:

	/**************************************************************************/
	/*!
		\brief
		Initializes all the systems that are Instantiated under vector systems
	*/
	/**************************************************************************/
	void Init()
	{
		THREADS.Init();
		RegisterComponents(AllObjectTypes());
		NAVMESHBUILDER.Init();
		BEHAVIORTREEBUILDER.Init();
		//#ifndef _BUILD
		//	BEHAVIORTREEBUILDER.Init();
		//#endif
		AUDIOMANAGER.InitAudioManager();
		MeshManager.Init();
		TextureManager.Init();
		AllSystems::Init();

		#if defined(_BUILD)
			InputSystem::Instance().LockCursor(true);
			SceneStartEvent startEvent{};
			EVENTS.Publish(&startEvent);
		#else
			EVENTS.Subscribe(this, &EngineCore::CallbackSceneStart);
			EVENTS.Subscribe(this, &EngineCore::CallbackSceneStop);
		#endif
		update_timer = 0.f;
		app_time = 0.f;
	}

	/**************************************************************************/
	/*!
		\brief
		Calls the update function of all the systems in vector systems depending
		if it should only be update in play mode or not
	*/
	/**************************************************************************/
	void Update(float dt)
	{
		//Start ImGui Frames
		#if defined(_BUILD)
			AllSystems::Update(dt);
		#else
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
				
			double starttime = 0;
			float elapsedtime = 0;
			bool update = false;

			//performance viewer update timer (1s)
			if (update_timer > 0.f) {
				update_timer -= dt;
			}
			else {
				update_timer = UPDATE_TIME;
				update = true;
			}

			auto func =
			[&](ISystem* sys)
			{
				if (sys->GetMode() & mode)
				{
					starttime = glfwGetTime();
					//Update performance viewer every 2s
					sys->Update(dt);
					if (update) {
						float timetaken = (float)(glfwGetTime() - starttime);
						elapsedtime += timetaken;
						system_times[typeid(*sys).name() + strlen("Class ")] = timetaken;
					}	
				}
			};

			AllSystems::Update(dt, func);

			if (update) {
				systemtotaltime = elapsedtime;
				update = false;
			}
				

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			app_time += dt;
		#endif

		//End ImGui Frames
	}

	/**************************************************************************/
	/*!
		\brief
		Calls the exit function of all the systems in vector systems, to be
		called at engine exit
	*/
	/**************************************************************************/
	void Exit()
	{
		AllSystems::Exit();
		THREADS.Exit();
		//#ifndef _BUILD
		//	BEHAVIORTREEBUILDER.Exit();
		//#endif
		NAVMESHBUILDER.Exit();
	}

	void CallbackSceneStart(SceneStartEvent* pEvent)
	{
		(void)pEvent;
		mode = ENUM_SYSTEM_RUNTIME;
	}
	void CallbackSceneStop(SceneStopEvent* pEvent) 
	{
		(void)pEvent;
		mode = ENUM_SYSTEM_EDITOR;
	}

	std::map<std::string, float>system_times;
	float systemtotaltime;
	float app_time;

private:
	float update_timer;
	SystemMode mode = ENUM_SYSTEM_EDITOR;
	FileWatcher watcher;
};
#endif // !CORE_H