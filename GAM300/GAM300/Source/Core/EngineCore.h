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
#include "Utilities/MultiThreading.h"
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


		systems =
		{
			&InputSystem::Instance(),
			&SceneManager::Instance(),
			//&PhysicsSystem::Instance(),
			&EditorSystem::Instance(),
			&GraphicsSystem::Instance(),
			&Blackboard::Instance(),
			&BehaviorTreeBuilder::Instance(),
			&AssetManager::Instance()
		};


		for (ISystem* pSystem : systems)
		{
			pSystem->Init();
		}

		//Enemy tempEnemy(BehaviorTreeBuilder::Instance().GetBehaviorTree("TestTree"));
		//tempEnemy.Update(1.f); // Temporary dt lol
		Scene& scene = SceneManager::Instance().GetCurrentScene();

		//TEST ENTITY CREATION
		//for (int i = 0; i < 22; ++i)
		//{
		//	scene.AddEntity();
		//}

		//Script& script3 = scene.AddComponent<Script>(14);
		//scene.AddComponent<Script>(14);
		//scene.AddComponent<Script>(13);
		//scene.AddComponent<Script>(3);
		//scene.AddComponent<Script>(21);
		//scene.Destroy(script3);
		//scene.AddComponent<Script>(14);
		//scene.AddComponent<Script>(14);
		//Script& script = scene.AddComponent<Script>(0);
		//scene.AddComponent<Script>(0);
		//scene.AddComponent<Script>(0);
		//scene.Destroy(script);
		//scene.AddComponent<Script>(0);
		//scene.AddComponent<Script>(0);
		//scene.AddComponent<Script>(0);
		//scene.AddComponent<Script>(0);
		//Script& script2 = scene.AddComponent<Script>(10);
		//scene.multiComponentsArrays.GetArray<Script>().SetActive(script2,false);
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
		//MultiComponentsArrays arr;
		if (state == EngineState::Run)
		{
			//Start ImGui Frames
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

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