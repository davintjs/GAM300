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
#ifndef EDITORHEADERS_H
#define EDITORHEADERS_H

#include <glm/vec2.hpp>
#include <unordered_map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include <implot.h>
#include <implot_internal.h>

#include "Core/SystemInterface.h"
#include "Utilities/SparseSet.h"
#include "Scene/Entity.h"
#include "Core/Events.h"

#define NON_VALID_ENTITY 0
#define GET_TEXTURE_ID(filepath) TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(filepath));
#define FIND_TEXTURE(filepath) TextureManager.FindTexture()

// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset = 0;
        }
    }
};

struct layer {
    layer(std::string _name) : name(_name) {}
    std::string name;

};

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
    // Initializing the Scene View
    void Init();

    // Updating and displaying of the Scene View
    void Update(float dt);

    bool SelectEntity();

    void ToolBar();
    
    void SceneView();
    
    void DisplayGizmos();

    // Exit the system
    void Exit();

    // Getters for the data members
    glm::vec2 const GetDimension() { return sceneDimension; }
    glm::vec2 const GetPosition() { return scenePosition; }
    bool const WindowHovered() { return windowHovered; }
    bool const WindowFocused() { return windowFocused; }
    bool const UsingGizmos() { return inOperation; }
    bool const DebugDraw() { return debug_draw; }

    void CallbackEditorWindow(EditorWindowEvent* pEvent);

private:
    glm::vec2 sceneDimension{}; // Dimensions of the viewport
    glm::vec2 scenePosition{};  // Position of the viewport relative to the engine
    glm::vec2 min{}, max{};     // Minimum and maximum position of the viewport
    bool windowHovered = false;
    bool windowFocused = false;
    bool inOperation = false;
    bool debug_draw = false;
};

ENGINE_EDITOR_SYSTEM(EditorGame)
{
public:
    // Initializing the Game View
    void Init();

    // Updating and displaying of the Game View
    void Update(float dt);

    void ToolBar();

    void UpdateTargetDisplay();

    void GameView();

    void ResizeGameView(glm::vec2 _newDimension);

    // Exit the system
    void Exit();

    // Getters for the data members
    glm::vec2 const GetDimension() { return dimension; }
    glm::vec2 const GetPosition() { return position; }
    bool const WindowHovered() { return windowHovered; }
    bool const WindowFocused() { return windowFocused; }
    bool const DebugDraw() { return debug_draw; }

    void CallbackEditorWindow(EditorWindowEvent * pEvent);

private:
    glm::vec2 dimension{}; // Dimensions of the viewport
    glm::vec2 position{};  // Position of the viewport relative to the engine
    glm::vec2 min{}, max{};     // Minimum and maximum position of the viewport
    unsigned int targetDisplay = 0;
    float padding = 16.f;
    float AspectRatio = 16.f / 9.f;
    bool windowHovered = false;
    bool windowFocused = false;
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

    std::vector<layer> Layers;
    std::vector<std::string> Tags;

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

    ImGuiTextBuffer& GetBuffer() { return Buffer; }
    ImVector<int>& GetLineOffset() { return LineOffsets; }

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

    std::map<std::string, ScrollingBuffer>system_plots;
};

#endif // !EDITORHEADERS_H