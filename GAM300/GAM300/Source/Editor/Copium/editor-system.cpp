/*!***************************************************************************************
\file			editor-system.cpp
\project
\author			Sean Ngo
\co-author		Shawn Tanary
				Matthew Lau

\par			Course: GAM250
\par			Section:
\date			16/09/2022

\brief
	This file holds the definition of functions for the editor system.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#include "pch.h"
#include "Windows/windows-system.h"
#include "Windows/windows-input.h"
#include "Editor/editor-system.h"
#include "Files/assets-system.h"
#include "Utilities/thread-system.h"
#include "Events/events-system.h"
#include <SceneManager/scene-manager.h>
#include <ImGuizmo.h>


namespace Copium
{
	namespace
	{
		// Our state
		bool show_demo_window = false;
		ThreadSystem& threadSystem{ *ThreadSystem::Instance() };
		bool tempMode = true;
	}

	void EditorSystem::init()
	{
		MyMessageSystem.subscribe(MESSAGE_TYPE::MT_START_PREVIEW, this);
		MyMessageSystem.subscribe(MESSAGE_TYPE::MT_STOP_PREVIEW, this);
		MyEventSystem->subscribe(this, &EditorSystem::CallbackSceneChanging);
		MyEventSystem->subscribe(this, &EditorSystem::CallbackEditorConsoleLog);
		systemFlags |= FLAG_RUN_ON_EDITOR | FLAG_RUN_ON_PLAY;

		//PRINT("FLAGS: " << systemFlags);

		//imgui
		ImGui::CreateContext();
		ImGuiIO & io = ImGui::GetIO(); (void) io;
		io.Fonts->AddFontFromFileTTF("Data\\arial.ttf", 32.f);

		// Only move window from title bar
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		// Global Font Size
		io.FontGlobalScale = 0.5f;
		
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(MyWindowSystem.get_window(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::GetIO().ConfigDockingWithShift = true;

		Window::EditorConsole::init();
		sceneView.init();
		game.init();
		contentBrowser.init();
		colorTheme.init();
		hierarchyList.init();
		inspector.init();
		layers.init();
		performanceViewer.init();
		// Initialize a new editor camera
		camera.init((float) sceneView.get_width(), (float) sceneView.get_height());


		buttons = MyAssetSystem.GetResources();
		previewFlag = false;
	}

	void EditorSystem::update()
	{
		if (MyInputSystem.is_key_held(GLFW_KEY_LEFT_SHIFT) && MyInputSystem.is_key_pressed(GLFW_KEY_F7))
		{
			tempMode = !tempMode;
		}
		static bool loadOnce = true;
		if (tempMode != enableEditor && loadOnce)
		{
			enableEditor = tempMode;
			if (MySceneManager.startPreview())
			{
				playMode(enableEditor);
				MyMessageSystem.dispatch(MESSAGE_TYPE::MT_START_PREVIEW);
			}
			else if(MySceneManager.inPlayMode)
				playMode(false);
		}

		if (!sceneChangeName.empty())
		{
			MySceneManager.load_scene(sceneChangeName);
			sceneChangeName.clear();
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		//Dockspace
		if(enableEditor)
		{
            static bool p_open = true;
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &p_open, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO& io = ImGui::GetIO();

			ImGuiID dockspace_id = 0;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

			//top menu bar
			if (ImGui::BeginMenuBar())
			{

				// File Dropdown
				if (ImGui::BeginMenu("File"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows, 
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
					if (ImGui::MenuItem("New", "Ctrl+N"))
					{
						//create new scene
						MySceneManager.create_scene();
					}

					if (ImGui::MenuItem("Open...", "Ctrl+O"))
					{
						//open scene
						while (!threadSystem.acquireMutex(MutexType::FileSystem));
						std::string filepath = FileDialogs::open_file("Copium Scene (*.scene)\0*.scene\0");
						threadSystem.returnMutex(MutexType::FileSystem);
						if (!filepath.empty())
						{
							PRINT(filepath);

							if (MySceneManager.load_scene(filepath))
								PRINT("loading success");
							else
								PRINT("loading fail");
						}
						else
						{
							PRINT("file failed to open");
						}

					}

					if (ImGui::MenuItem("Save", "Ctrl+S"))
					{
						//save scene
						if (MySceneManager.get_scenefilepath().empty()) {
							//save sceen as
							while (!threadSystem.acquireMutex(MutexType::FileSystem));
							std::string filepath = FileDialogs::save_file("Copium Scene (*.scene)\0.scene\0");
							threadSystem.returnMutex(MutexType::FileSystem);
							PRINT(filepath);

							size_t pos = filepath.find_last_of("/\\") + 1;
							std::string sceneName = filepath.substr(pos);
							MySceneManager.save_scene(filepath, sceneName, true);
						}
						else 
						{
							MySceneManager.save_scene();
						}
					}

					if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					{
						
						if (MySceneManager.get_current_scene())
						{
							//save sceen as
							while (!threadSystem.acquireMutex(MutexType::FileSystem));
							std::string filepath = FileDialogs::save_file("Copium Scene (*.scene)\0.scene\0");
							threadSystem.returnMutex(MutexType::FileSystem);
							PRINT(filepath);
							size_t pos = filepath.find_last_of("/\\") + 1;
							std::string sceneName = filepath.substr(pos);
							MySceneManager.save_scene(filepath, sceneName);
						}
						else
						{
							PRINT("There is no scene to save...\n");
						}

					}

					if (ImGui::MenuItem("Exit"))
					{
						Scene* s = MySceneManager.get_storage_scene();
						if (s)
						{
							MySceneManager.endPreview();
						}
						//exit game engine
						MyEventSystem->publish(new QuitEngineEvent);
						PRINT("Copium has been huffed, Engine shutting down");

					}

					ImGui::EndMenu();
				}

				// Preview Options
				if (ImGui::BeginMenu("Preview"))
				{
					if (ImGui::MenuItem("Play Scene"))
					{
						printf("Starting scene\n");
						UUID selectedID{};
						if (pSelectedGameObject)
							selectedID = pSelectedGameObject->uuid;
						if (MySceneManager.startPreview())
						{
							pSelectedGameObject = MySceneManager.FindGameObjectByID(selectedID);
							MyMessageSystem.dispatch(MESSAGE_TYPE::MT_START_PREVIEW);
						}
					}
					if (ImGui::MenuItem("Stop Scene"))
					{
						UUID selectedID{};
						if (pSelectedGameObject)
							selectedID = pSelectedGameObject->uuid;
						if (MySceneManager.endPreview())
						{						
							pSelectedGameObject = MySceneManager.FindGameObjectByID(selectedID);
							MyMessageSystem.dispatch(MESSAGE_TYPE::MT_STOP_PREVIEW);
						}
					}

					ImGui::EndMenu();
				}

				//if your IMGUI window can be closed,you should make the bool inline in the header and include it here to be able to reopen it
				if (ImGui::BeginMenu("Windows"))
				{
					if (ImGui::MenuItem("Hierarchy"))
					{
						hierarchyList.status() = true;
					}
					if (ImGui::MenuItem("Console Log"))
					{
						//Window::EditorConsole::isConsoleLogOpen = true;
					}
					if (ImGui::MenuItem("Theme generator"))
					{
						isColorThemeOpen = true;
					}
					if (ImGui::MenuItem("Inspector"))
					{
						inspector.status() = true;
					}

					ImGui::EndMenu();
				}


				ImGui::EndMenuBar();
			}

			//top menu shortcuts
			if (MyInputSystem.is_key_held(GLFW_KEY_LEFT_CONTROL))
			{
				if (MyInputSystem.is_key_pressed(GLFW_KEY_N))
				{
					//create new scene
					MySceneManager.create_scene();
				}
				else if (MyInputSystem.is_key_pressed(GLFW_KEY_O))
				{
					//open scene
					while (!threadSystem.acquireMutex(MutexType::FileSystem));
					std::string filepath = FileDialogs::open_file("Copium Scene (*.scene)\0*.scene\0");
					threadSystem.returnMutex(MutexType::FileSystem);
					if (!filepath.empty())
					{
						PRINT(filepath);


						if (MySceneManager.load_scene(filepath))
							PRINT("loading success");
						else
							PRINT("loading fail");

					}
					else
					{
						PRINT("file failed to open");
					}
				}
				else if (MyInputSystem.is_key_pressed(GLFW_KEY_S))
				{

					if (MyInputSystem.is_key_held(GLFW_KEY_LEFT_SHIFT))
					{
						if (MySceneManager.get_current_scene())
						{
							//save sceen as
							while (!threadSystem.acquireMutex(MutexType::FileSystem));
							std::string filepath = FileDialogs::save_file("Copium Scene (*.scene)\0.scene\0");
							threadSystem.returnMutex(MutexType::FileSystem);
							PRINT(filepath);
							size_t pos = filepath.find_last_of("/\\") + 1;
							std::string sceneName = filepath.substr(pos);
							MySceneManager.save_scene(filepath, sceneName);
						}
						else
						{
							PRINT("There is no scene to save...\n");
						}
					}
					else
					{
						//save scene
						if (MySceneManager.get_scenefilepath().empty()) {
							//save sceen as
							while (!threadSystem.acquireMutex(MutexType::FileSystem));
							std::string filepath = FileDialogs::save_file("Copium Scene (*.scene)\0.scene\0");
							threadSystem.returnMutex(MutexType::FileSystem);
							PRINT(filepath);
							size_t pos = filepath.find_last_of("/\\") + 1;
							std::string sceneName = filepath.substr(pos);
							MySceneManager.save_scene(filepath, sceneName, true);
						}
						else
						{
							MySceneManager.save_scene();
						}
					}


				}
			}

            //Call all the editor layers updates here
			PreviewButton();
			colorTheme.update();
			hierarchyList.update();
			layers.update();
			inspector.update();
			Window::EditorConsole::update();
			game.update();
			sceneView.update();
			contentBrowser.update();
			performanceViewer.update();
			
			// demo update
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::End();
		}
		
		ImGui::EndFrame();

		if (!loadOnce)
			loadOnce = true;
	}

	void EditorSystem::draw()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EditorSystem::exit()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		sceneView.exit();
		game.exit();
		contentBrowser.exit();
		hierarchyList.exit();
		inspector.exit();
		layers.exit();
		performanceViewer.exit();

		PRINT("Before deleting, Undo stack: " << commandManager.undoStack.size() << ", Redo stack:" << commandManager.redoStack.size());
		while (commandManager.undoStack.size() > 0)
		{
			UndoRedo::Command* temp = get_commandmanager()->undoStack.top();
			delete temp;
			get_commandmanager()->undoStack.pop();
			
		}

		while (commandManager.redoStack.size() > 0)
		{
			UndoRedo::Command* temp = get_commandmanager()->redoStack.top();
			delete temp;
			get_commandmanager()->redoStack.pop();
			
		}
		PRINT("After deleting, Undo stack: " << commandManager.undoStack.size() << ", Redo stack:" << commandManager.redoStack.size());
		
		camera.exit();
	}

	void EditorSystem::handleMessage(MESSAGE_TYPE _mType)
	{
		if (_mType == MESSAGE_TYPE::MT_START_PREVIEW)
		{
			previewFlag = true;
		}
		else if (_mType == MESSAGE_TYPE::MT_STOP_PREVIEW)
		{
			previewFlag = false;
			Scene* scene = MySceneManager.get_current_scene();
			if (scene && !scene->componentArrays.GetArray<Camera>().empty())
				for (Camera& cam : scene->componentArrays.GetArray<Camera>())
				{
					// Bean: Reset all the framebuffers for now, next time only need to reset active main camera
					cam.get_framebuffer()->init();
				}
		}
	}

	void EditorSystem::imguiConsoleAddLog(std::string value)
	{
		Window::EditorConsole::editorLog.add_logEntry(value);
	}

	void EditorSystem::playMode(bool _enabled)
	{
		if (!_enabled)
		{
			// Swap camera
			//camera.get_framebuffer()->exit();
			glm::vec2 dimension = { MyWindowSystem.get_window_width(), MyWindowSystem.get_window_height() };
			// Game Camera
			Scene* scene = MySceneManager.get_current_scene();
			if (scene && !scene->componentArrays.GetArray<Camera>().empty())
			{
				game.resize_game(dimension);
				glViewport(0, 0, (GLsizei)dimension.x, (GLsizei)dimension.y);
			}
		}
		else if(_enabled)
			camera.get_framebuffer()->init();
	}

	void EditorSystem::CallbackEditorConsoleLog(EditorConsoleLogEvent* pEvent)
	{
		PRINT(pEvent->message);
		imguiConsoleAddLog(pEvent->message);
	}

	void EditorSystem::CallbackSceneChanging(SceneChangingEvent* pEvent)
	{
		pSelectedGameObject = nullptr;
	}

	UndoRedo::CommandManager* EditorSystem::get_commandmanager()
	{
		return &commandManager;
	}

	void EditorSystem::PreviewButton()
	{
		int i{ 3 };
		if (previewFlag && MySceneManager.get_current_scene())
		{
			i = 4;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		// Preview Button
		ImTextureID previewBtn = (ImTextureID)(size_t)buttons[i].get_object_id();
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);             // Black background
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint
		
		ImVec2 sz = { 20.f, 20.f };

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;

		ImGui::Begin("Preview Toolbar", nullptr, windowFlags);
		ImGui::PushID(10000);

		if (ImGui::IsWindowDocked())
		{
			ImGuiDockNode* node = ImGui::GetWindowDockNode();
			ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize 
				| ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe;
			node->LocalFlags |= dockFlags;
		}

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.5f - sz.x * 0.5f - 2.f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (ImGui::ImageButton("", previewBtn, sz, {0,1}, {1,0}, bg_col, tint_col))
		{
			previewFlag = previewFlag ? false : true;

			if (MySceneManager.get_current_scene())
			{
				if (MySceneManager.GetSceneState() == Scene::SceneState::play)
				{
					UUID selectedID{};
					if (pSelectedGameObject)
						selectedID = pSelectedGameObject->uuid;
					if (MySceneManager.endPreview())
					{
						pSelectedGameObject = MySceneManager.FindGameObjectByID(selectedID);
						MyMessageSystem.dispatch(MESSAGE_TYPE::MT_STOP_PREVIEW);
					}
				}
				else if (MySceneManager.GetSceneState() == Scene::SceneState::edit)
				{
					UUID selectedID{};
					if (pSelectedGameObject)
						selectedID = pSelectedGameObject->uuid;
					if (MySceneManager.startPreview())
					{
						pSelectedGameObject = MySceneManager.FindGameObjectByID(selectedID);
						MyMessageSystem.dispatch(MESSAGE_TYPE::MT_START_PREVIEW);
					}
				}

			}

		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::PopID();
		ImGui::End();
	}
}
