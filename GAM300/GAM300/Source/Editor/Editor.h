/**************************************************************************************/
/*!
//    \file			Editor.h
//    \author(s) 	Joseph Ho Jun Jie
//
//    \date   	    9th May 2023
//    \brief		This file contains the prototypes for all the functions that are used
//					to create the Level Editor interface for the program.
//
//    \Percentage   Joseph 100%
//
//    Copyright (C) 2022 DigiPen Institute of Technology.
//    Reproduction or disclosure of this file or its contents without the
//    prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /**************************************************************************************/

#include "Precompiled.h"
#include "IOManager/Handler_GLFW.h"
#include "Core/SystemInterface.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



ENGINE_SYSTEM(EditorSystem)
{
	void Init();
	void Update(float dt);
	void Exit();

	void Editor_Dockspace();
	void Editor_MenuBar();
	void Editor_Content_Browser();
	void Editor_SceneViewport();
	void Editor_Toolbar();
	void Editor_Inspector();
	void Editor_Hierarchy();

};



//Inspector Functions
//void Inspector_Transform(int index);
//void Inspector_Renderer(int index);
//void Inspector_GameLogic(std::string type, int index);
//void Inspector_Camera(int index);
//void Inspector_Body(int index);
//void Inspector_General(int index);

// Editor Member functions
//void Editor_Entity_Creator();

//void Editor_PerformanceViewer();

//void Editor_Update();
//void Editor_New_Scene();
//void Editor_PickEntity();
//void Editor_Macros();
//void Editor_Undo();
//void Editor_Redo();