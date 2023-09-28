/*!***************************************************************************************
\file			EditorHeaders.h
\project
\author         Sean Ngo
\co-author      Joseph Ho

\par			Course: GAM300
\date           04/09/2023

\brief
    This file contains the declarations of the following:
    1. All windows in the editor system

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include <glm/vec2.hpp>
#include <unordered_map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"

#include "Core/SystemInterface.h"
#include "Utilities/SparseSet.h"
#include "Scene/Entity.h"
#include "Core/Events.h"

#define NON_VALID_ENTITY 0
#define GET_TEXTURE_ID(filepath) TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(filepath));
#define FIND_TEXTURE(filepath) TextureManager.FindTexture()

ENGINE_EDITOR_SYSTEM(EditorMenuBar)
{
public:
    // Initializing the Menu Bar
    void Init();

    // Updating and displaying of the Menu Bar
    void Update(float dt);

    // Exit the system
    void Exit();

    // Open a scene file
    void OpenFile();

    // Create a new scene
    void NewScene();

    // Save the current loaded scene
    void SaveScene();

private:
    bool exitApp = false;
};

ENGINE_EDITOR_SYSTEM(EditorToolBar)
{
public:
    // Initializing the Menu Bar
    void Init();

    // Updating and displaying of the Menu Bar
    void Update(float dt);

    // Exit the system
    void Exit();

private:
};

ENGINE_EDITOR_SYSTEM(EditorHierarchy)
{
public:
    // Initializing the Hierarchy
    void Init();

    // Updating and displaying of the Hierarchy
    void Update(float dt);

    // Exit the system
    void Exit();

    void DisplayEntity(Engine::UUID euid);
    //void DisplayChildren(const ObjectIndex& Parent);
    Engine::UUID selectedEntity;

    bool initLayer = true;
private:
    void CallbackSelectedEntity(SelectedEntityEvent* pEvent);
};


ENGINE_EDITOR_SYSTEM(EditorContentBrowser)
{
public:
    // Initializing the Content Browser
    void Init();

    // Updating and displaying of the Content Browser
    void Update(float dt);

    // Exit the system
    void Exit();

private:
};


ENGINE_EDITOR_SYSTEM(EditorScene)
{
public:
    // Initializing the Scene & Game
    void Init();

    // Updating and displaying of the Scene & Game
    void Update(float dt);

    // Exit the system
    void Exit();

    // Getters for the data members
    glm::vec2 const GetDimension() { return sceneDimension; }
    glm::vec2 const GetPosition() { return scenePosition; }
    bool const WindowHovered() { return windowHovered; }
    bool const UsingGizmos() { return inOperation; }
    bool const DebugDraw() { return debug_draw; }

private:
    glm::vec2 sceneDimension{};   // Dimensions of the viewport
    glm::vec2 scenePosition{};    // Position of the viewport relative to the engine
    glm::vec2 min{}, max{};         // Minimum and maximum position of the viewport
    bool windowHovered = false;
    bool inOperation = false;
    bool debug_draw = false;
};

ENGINE_EDITOR_SYSTEM(EditorInspector)
{
public:
    // Initializing the Inspector
    void Init();

    // Updating and displaying of the Inspector
    void Update(float dt);

    // Exit the system
    void Exit();

    bool isAddPanel;
private:
};

ENGINE_EDITOR_SYSTEM(EditorDebugger)
{
public:

    // Initializing the Debugger
    void Init();

    // Updating and displaying of the Debugger
    void Update(float dt);

    // Exit the system
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

ENGINE_EDITOR_SYSTEM(EditorPerformanceViewer)
{
public:
    // Initializing the Performance Viewer
    void Init();

    // Updating and displaying of the Performance Viewer
    void Update(float dt);

    // Exit the system
    void Exit();

    float update_time;
    std::vector<float>times;
};

#endif // !EDITORTOOLBAR_H