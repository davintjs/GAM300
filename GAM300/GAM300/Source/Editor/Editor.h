/*!***************************************************************************************
\file			Editor.h
\project		GAM300
\author			Joseph Ho

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the declarations of the following:
    1. Contains the declaration of the Editor system in the architecture.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "Core/SystemInterface.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/Events.h"
#include "Editor/EditorHistory.h"

#define EDITOR EditorSystem::Instance()

void AlignForWidth(float width, float alignment = 0.5f);

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

	void CallbackGetSelectedEntity(GetSelectedEntityEvent* pEvent);

	HistoryManager History;

private:
	std::vector<ISystem*> editorSystems;
	Engine::UUID selectedEntity{0};
};