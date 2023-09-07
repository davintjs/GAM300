/*!***************************************************************************************
\file			EditorHeaders.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           04/09/2023

\brief
    This file contains the declarations of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include <glm/vec2.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_stdlib.h>

#include "Core/SystemInterface.h"
#include "Utilities/SparseSet.h"
#include "Scene/Entity.h"

#define NON_VALID_ENTITY -1


ENGINE_EDITOR_SYSTEM(EditorMenuBar)
{
public:
    void Init();
    void Update(float dt);
    void Exit();

private:
    bool exitApp = false;
};

ENGINE_EDITOR_SYSTEM(EditorToolBar)
{
public:
    void Init();
    void Update(float dt);
    void Exit();

private:
};

ENGINE_EDITOR_SYSTEM(EditorHierarchy)
{
public:
    void Init();
    void Update(float dt);
    void Exit();

    void DisplayEntity(const ObjectIndex& Index);
    //void DisplayChildren(const ObjectIndex& Parent);
    ObjectIndex selectedEntity;

    std::vector<Entity*>layer; 

    bool initLayer = true;
private:
};


ENGINE_EDITOR_SYSTEM(EditorContentBrowser)
{
public:
    void Init();
    void Update(float dt);
    void Exit();

private:
};


ENGINE_EDITOR_SYSTEM(EditorScene)
{
public:
    void Init();
    void Update(float dt);
    void Exit();

    glm::vec2 const GetDimension() { return sceneDimension; }
    glm::vec2 const GetPosition() { return scenePosition; }

private:
    glm::vec2 sceneDimension;   // Dimensions of the viewport
    glm::vec2 scenePosition;    // Position of the viewport relative to the engine
};

ENGINE_EDITOR_SYSTEM(EditorInspector)
{
public:
    void Init();
    void Update(float dt);
    void Exit();
    bool isAddComponentPanel;
private:
};

ENGINE_EDITOR_SYSTEM(EditorDebugger)
{
public:

    void Init();
    void Update(float dt);
    void Exit();

    void Clear();

    void AddLog(const char* fmt, ...) IM_FMTARGS(2);

    void Draw();

    int debugcounter;

private:
    ImGuiTextBuffer     Buffer;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

};

#endif // !EDITORTOOLBAR_H