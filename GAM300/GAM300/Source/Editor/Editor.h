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
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/Events.h"

#define EDITOR EditorSystem::Instance()

struct Entity;
ENGINE_SYSTEM(EditorSystem)
{
public:
	void Init();
	void Update(float dt);
	void Exit();

	void Editor_Dockspace();

	Engine::UUID GetSelectedEntity();
	void SetSelectedEntity(Entity*);


	void CallbackSelectedEntity(SelectedEntityEvent* pEvent);
private:
	std::vector<ISystem*> editorSystems;
	Engine::UUID selectedEntity{0};
};