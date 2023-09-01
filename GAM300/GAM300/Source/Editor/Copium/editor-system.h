/*!***************************************************************************************
\file			editor-system.h
\project
\author			Sean Ngo
\co-author		Shawn Tanary
				Matthew Lau

\par			Course: GAM250
\par			Section:
\date			16/09/2022

\brief
	This file holds the declaration of functions for editor-system.cpp.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#ifndef EDITOR_SYSTEM_H
#define EDITOR_SYSTEM_H

#include "CopiumCore/system-interface.h"
#include "Editor/editor-sceneview.h"
#include "Editor/editor-game.h"
#include "Editor/editor-content-browser.h"
#include "Editor/editor-camera.h"
#include "Editor/editor-colortheme.h"
#include "Editor/editor-undoredo.h"
#include "Editor/editor-hierarchy-list.h"
#include "Editor/editor-layers.h"
#include "Editor/editor-consolelog.h"
#include "Editor/inspector.h"
#include "Editor/editor-performance.h"
#include "Messaging/message-system.h"

#define MyEditorSystem (*Copium::EditorSystem::Instance())

namespace Copium
{
	CLASS_SYSTEM(EditorSystem), IReceiver
	{
	public:
		/***************************************************************************/
		/*!
		\brief
			Initialises the editor for the engine. It creates the ImGui context for
			the engine
		*/
		/***************************************************************************/
		void init();

		/***************************************************************************/
		/*!
		\brief
			Updates the editors in the engine
		*/
		/***************************************************************************/
		void update();

		/***************************************************************************/
		/*!
		\brief
			Renders all Imgui in the engine
		*/
		/***************************************************************************/
		void draw();

		/***************************************************************************/
		/*!
		\brief
			Shutdown Imgui and its components
		*/
		/***************************************************************************/
		void exit();

		/**************************************************************************/
		/*!
		\brief
			Interface function for MessageSystem to call for IReceivers to handle
			a messageType
		*/
		/**************************************************************************/
		void handleMessage(MESSAGE_TYPE _mType);

		/***************************************************************************/
		/*!
		\brief
			Adds a log to the imgui console
		*/
		/***************************************************************************/
		void imguiConsoleAddLog(std::string value);

		/*******************************************************************************
		/*!
		*
		\brief
			Set the game in windowed fullscreen
		*/
		/*******************************************************************************/
		void playMode(bool _enabled);

		/*******************************************************************************
		/*!
		*
		\brief
			A getter function for the command manager

		\return
			a pointer to the undo redo command manager
		*/
		/*******************************************************************************/
		UndoRedo::CommandManager* get_commandmanager();
		
		bool is_enabled() { return enableEditor; }

		EditorSceneView* get_scene_view() { return &sceneView; }
		EditorGame* get_game_view() { return &game; }
		EditorContentBrowser* get_content_browser() { return &contentBrowser; }
		EditorCamera* get_camera() { return &camera; }
		EditorHierarchyList* get_hierarchy_list() { return &hierarchyList; }
		EditorInspector* get_inspector() { return &inspector; }
		EditorLayers* getLayers() { return &layers; }

		GameObject* pSelectedGameObject{};
		std::string sceneChangeName{};


		std::vector<Texture> buttons;

	private:
		

	private:
		void PreviewButton();
		bool previewFlag = false;
		bool enableEditor = true;

		EditorSceneView sceneView;
		EditorGame game;
		EditorContentBrowser contentBrowser;
		EditorCamera camera;
		EditorColorTheme colorTheme;
		EditorHierarchyList hierarchyList;
		EditorInspector inspector;
		EditorLayers layers;
		EditorPerformance performanceViewer;

		UndoRedo::CommandManager commandManager; //for undo and redo

		void CallbackSceneChanging(SceneChangingEvent* pEvent);
		void CallbackEditorConsoleLog(EditorConsoleLogEvent* pEvent);
	};
}
#endif // !EDITOR_SYSTEM_H
