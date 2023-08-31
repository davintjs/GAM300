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
#include "FramerateController.h"
#include "Utilities/MultiThreading.h"
//#include "Physics/PhysicsSystem.h"
#include "Scene/SceneManager.h"
#include <vector>
#include "Scene/Components.h"
#include "Graphics/GraphicsSystem.h"
//#include "IOManager/Handler_GLFW.h"
#include "AI/Blackboard.h"
#include "AI/BehaviorTreeBuilder.h"
//#include "AI/Enemy.h"

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
		MyFrameRateController.Init();


		systems =
		{
			&SceneManager::Instance(),
			//&PhysicsSystem::Instance(),
			&EditorSystem::Instance(),
			&GraphicsSystem::Instance(),
			&Blackboard::Instance(),
			&BehaviorTreeBuilder::Instance()


		};


		for (ISystem* pSystem : systems)
		{
			pSystem->Init();
		}

		//Enemy tempEnemy(BehaviorTreeBuilder::Instance().GetBehaviorTree("TestTree"));
		//tempEnemy.Update(1.f); // Temporary dt lol
		Scene& scene = SceneManager::Instance().GetCurrentScene();

		//TEST ENTITY CREATION
		// for (int i = 0; i < 22; ++i)
		// {
		// 	scene.AddEntity();
		// }

		// scene.AddComponent<Rigidbody>(20);
		// scene.AddComponent<Rigidbody>(1);
		//Script& script = scene.AddComponent<Script>(20);
		// scene.AddComponent<Script>(20);
		// scene.AddComponent<Script>(20);
		// scene.AddComponent<Script>(1);
		// scene.AddComponent<Script>(20);
		//scene.AddComponent<Script>(20);
		//Entity& entity = scene.entities.DenseSubscript(20);
		//scene.RemoveComponent(entity,script);
	}

	/**************************************************************************/
	/*!
		\brief
		Calls the update function of all the systems in vector systems depending
		if it should only be update in play mode or not
	*/
	/**************************************************************************/
	void Update()
	{
		//MultiComponentsArrays arr;
		while (!glfwWindowShouldClose(GLFW_Handler::ptr_window))
		{
			

			if (state == EngineState::Run)
			{
				MyFrameRateController.Start();

				// Bean: This should be in input system (Euan go create)
				glfwPollEvents();

				//glfwSetMouseButtonCallback();

				//Start ImGui Frames
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				for (ISystem* pSystem : systems)
				{
					pSystem->Update();
				}

				//End ImGui Frames
				ImGui::EndFrame();
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwSwapBuffers(GLFW_Handler::ptr_window); // This at the end	

				MyFrameRateController.End();
			}
		}
				//MyFrameRateController.End();
			//}
		//}
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
		THREADS.Exit();
		for (auto iter = systems.rbegin(); iter != systems.rend(); ++iter)
		{
			(*iter)->Exit();
		}
	}
private:
	std::vector<ISystem*> systems;
	EngineState state = EngineState::Run;
	SystemMode mode = ENUM_SYSTEM_EDITOR;
};
#endif // !CORE_H