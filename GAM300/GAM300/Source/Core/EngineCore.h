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
//#include "Physics/PhysicsSystem.h"
#include "Scene/SceneManager.h"
#include "Scene/Components.h"
#include "Graphics/GraphicsSystem.h"
//#include "IOManager/Handler_GLFW.h"
#include "AI/Blackboard.h"
#include "AI/BehaviorTreeBuilder.h"
//#include "AI/Enemy.h"
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
#include "Scripting/LogicSystem.h"

#define MyEngineCore EngineCore::Instance()

enum class EngineState
{
	Run,
	Editor,
	Paused,
	Quit
};

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
		RegisterComponents();
		systems =
		{
			&InputSystem::Instance(),
			&SceneManager::Instance(),
			//&ScriptingSystem::Instance(),
			&EditorSystem::Instance(),
			//&LogicSystem::Instance(),
			//&PhysicsSystem::Instance(),
			&GraphicsSystem::Instance(),
			&Blackboard::Instance(),
			&BehaviorTreeBuilder::Instance(),
			&AssetManager::Instance(),
		};

		for (ISystem* pSystem : systems)
		{
			pSystem->Init();
		}

		EVENTS.Subscribe(this, &EngineCore::CallbackSceneStart);
		//Enemy tempEnemy(BehaviorTreeBuilder::Instance().GetBehaviorTree("TestTree"));
		//tempEnemy.Update(1.f); // Temporary dt lol
		Scene& scene = SceneManager::Instance().GetCurrentScene();

		//SceneStartEvent startEvent{};
		//ACQUIRE_SCOPED_LOCK("Assets");
		//EVENTS.Publish(&startEvent);

		PRINT("SIZEOF: ", sizeof(Entity));
		//ThreadPool mThreadP;
		//for (int i = 0; i < 10; ++i)
		//{
		//	mThreadP.EnqueueTask([i]
		//		{
		//			std::cout << "Task " << i << " is being executed by thread " << std::this_thread::get_id() << std::endl;
		//			std::this_thread::sleep_for(std::chrono::seconds(1));
		//			std::cout << "Task " << i << " completed" << std::endl;
		//		});
		//}

		//std::this_thread::sleep_for(std::chrono::seconds(10));

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
		if (state == EngineState::Run)
		{
			//Start ImGui Frames

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			for (ISystem* pSystem : systems)
			{
				if (pSystem->GetMode() & mode)
					pSystem->Update(dt);
			}

			//End ImGui Frames
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(GLFW_Handler::ptr_window); // This at the end	
		}
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
		for (auto iter = systems.rbegin(); iter != systems.rend(); ++iter)
		{
			(*iter)->Exit();
		}
		THREADS.Exit();
	}

	void CallbackSceneStart(SceneStartEvent* pEvent)
	{
		mode = ENUM_SYSTEM_RUNTIME;
	}
private:
	std::vector<ISystem*> systems;
	EngineState state = EngineState::Run;
	SystemMode mode = ENUM_SYSTEM_EDITOR;
	FileWatcher watcher;
};
#endif // !CORE_H