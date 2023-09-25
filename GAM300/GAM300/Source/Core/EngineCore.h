/*!***************************************************************************************
\file			EngineCore.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			27/09/2022

\brief
	NOTE: DO NOT INCLUDE IN ANY OTHER PLACE OTHER THAN MAIN
	This file manages all the systems as a super system itself. 

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
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
//#include "IOManager/Handler_GLFW.h"
#include "AI/Blackboard.h"
#include "AI/BehaviorTreeBuilder.h"
//#include "AI/Enemy.h"
#include "AI/NavMeshBuilder.h"
#include "IOManager/InputSystem.h"
#include "IOManager/Handler_GLFW.h"
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

#define MyEngineCore EngineCore::Instance()
#define UPDATE_TIME 2.f;

#if defined(_BUILD)
#else
	using AllSystemsPack =
	TemplatePack
	<
		AssetManager,
		InputSystem,
		SceneManager,
		DemoSystem,
		ScriptingSystem,
		EditorSystem,
		PhysicsSystem,
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
		AllSystems::Init();

		EVENTS.Subscribe(this, &EngineCore::CallbackSceneStart);
		//Enemy tempEnemy(BehaviorTreeBuilder::Instance().GetBehaviorTree("TestTree"));
		//tempEnemy.Update(1.f); // Temporary dt lol
		update_timer = 0.f;

		SceneStartEvent startEvent{};
		ACQUIRE_SCOPED_LOCK(Assets);
		EVENTS.Publish(&startEvent);

		// NavMesh testing
		std::vector<glm::vec3> GroundVertices{glm::vec3(1.f, 1.f, 1.f)};
		std::vector<glm::ivec3> GroundIndices{glm::ivec3(0, 0, 0)};
		NAVMESHBUILDER.BuildNavMesh(GroundVertices, GroundIndices); // Build the navmesh

		// Bean: Serialization Tests
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

			//performance viewer update timer (2s)
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
						float timetaken = glfwGetTime() - starttime;
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
		#endif

		//End ImGui Frames

		glfwSwapBuffers(GLFW_Handler::ptr_window); // This at the end	
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
		BEHAVIORTREEBUILDER.Exit();
		NAVMESHBUILDER.Exit();
	}

	void CallbackSceneStart(SceneStartEvent* pEvent)
	{
		mode = ENUM_SYSTEM_RUNTIME;
	}

	std::map<std::string, float>system_times;
	float systemtotaltime;

private:
	float update_timer;
	SystemMode mode = ENUM_SYSTEM_EDITOR;
	FileWatcher watcher;
};
#endif // !CORE_H