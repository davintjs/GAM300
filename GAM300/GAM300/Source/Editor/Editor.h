/**************************************************************************************/
/*!
//    \file			Editor.h
//    \author(s) 	Joseph Ho
//
//    \date   	    07/09/2023
//    \brief
        This file contains the declarations of the following:
        1. Contains the declaration of the Editor system in the architecture.
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