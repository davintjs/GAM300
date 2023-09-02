/**************************************************************************************/
/*!
//    \file			Editor.cpp
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
#include "Editor.h"
#include "../Utilities/PlatformUtils.h"

// Bean: Need this to reference the editor camera's framebuffer
#include "../Graphics/Editor_Camera.h"

static bool exitapp = false;

void EditorSystem::Init()
{
    //Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    //io.FontDefault = io.Fonts->Fonts[0];

    // Enable this for dockspace capabilities in dockspace mode		
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(GLFW_Handler::ptr_window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

}

void EditorSystem::Update(float dt)
{

    //bool demo = true;
    //ImGui::ShowDemoWindow(&demo);

    

    //Editor Functions
   /* Editor_Dockspace();
    Editor_MenuBar();
    Editor_Content_Browser();
    Editor_SceneViewport();
    Editor_Toolbar();
    Editor_Inspector();
    Editor_Hierarchy();

    ImGui::StyleColorsDark();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);*/
    

}

void EditorSystem::Exit()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void EditorSystem::Editor_MenuBar(){

    if (ImGui::BeginMainMenuBar())
    {
        //File Menu functionality
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                //New Scene
            }

            // Opening of files using file dialogs
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                std::string Filename =  FileDialogs::OpenFile("Text Files (*.txt)\0*.txt\0");
                if (!Filename.empty()) {
                    //Open File
                }
            }

            // Saving of scene files only if an active scene is open, using file dialogs
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                //Save File
            }
            //else
            //    //Do not allow the user to save when there's no file loaded!
            //    ImGui::TextDisabled("Save", "Ctrl+S");

            // Save as functionality using file dialogs
            if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {
                std::string Filename = FileDialogs::SaveFile("Text Files (*.txt)\0*.txt\0");
                if (!Filename.empty()) {
                    //DEBUG->Debugger.AddLog("[%i]{Info} File saved successfully!\n", DEBUG->debugcounter++);
                }
            }
            //else
            //    //Do not allow the user to save when there's no file loaded!
            //    ImGui::TextDisabled("Save As..", "Ctrl+Shift+S");

            // Quit application, opens a pop-up dialog to confirm exit
            if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
                exitapp = true;
            }
            ImGui::EndMenu();
        }

        // Popup Modal for exiting application
        if (exitapp) {
            ImGui::OpenPopup("Exit?");
            exitapp = false;
        }
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Exit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to exit the application?\n\n");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                //Close Application
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            // copy functionality
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                //Copy Current Item
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                //Creates a copy of the entity copieds
            }
            

            // Undo Functionality using Editor_Undo()
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
            }

            // Do not allow the user to undo when history buffer is empty or when file is not loaded
            ImGui::TextDisabled("Undo");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}



/**************************************************************************/
   /*!
       Implementation of the docking functionality in the Editor.
   */
/**************************************************************************/
void EditorSystem::Editor_Dockspace() {

    static bool dockopen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // setting of docking space settings in ImGui
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoBackground;
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

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &dockopen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
}

static const std::string AssetDirectory = "Data";
static std::string entityType = "default", entityName = "default";
static std::filesystem::path CurrentDirectory = AssetDirectory;
static std::string currentFolder = "Data";

void EditorSystem::Editor_Content_Browser() {
    ImGui::Begin("Content Browser");

    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    /*ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::Text(currentFolder.c_str());
    ImGui::PopFont();
    ImGui::Separator();*/

    // Back button to return to parent directory
    /*if (CurrentDirectory != std::filesystem::path(AssetDirectory)) {
        if (ImGui::Button("Back", ImVec2{ 50.f, 30.f })) {
            CurrentDirectory = CurrentDirectory.parent_path();
            currentFolder = CurrentDirectory.string();
        }
    }*/


    /*static float padding = 12.0f;
    static float iconsize = 100.f;
    float cellsize = iconsize + padding;

    float window_width = ImGui::GetContentRegionAvail().x;
    int columncount = (int)(window_width / cellsize);
    if (columncount < 1) { columncount = 1; }

    ImGui::Columns(columncount, 0, false);*/

    //int i = 0;
    //using filesystem to iterate through all folders/files inside the "/Data" directory
    //for (auto& it : std::filesystem::directory_iterator{ CurrentDirectory }) {
    //  ImGui::PushID(i++);
    //  const auto& path = it.path();
    //  auto relativepath = std::filesystem::relative(path, AssetDirectory);
    //    std::string pathStr = relativepath.filename().string();

        //Draw the file / folder icon based on whether it is a directory or not
        //std::string icon = it.is_directory() ? "foldericon" : "fileicon";

        // render respective file icon textures
       

        // Drag drop logic for files
        /*if (ImGui::BeginDragDropSource()) {
            std::string filepath = relativepath.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filepath.c_str(), filepath.size() + 1);
            ImGui::EndDragDropSource();
        }*/

        //ImGui::PopStyleColor();
        // Change directory into the folder clicked
        /*if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (it.is_directory()) {
                currentFolder = pathStr;
                CurrentDirectory /= path.filename();
            }
        }*/

        // double click logic to open scene file
        /*if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (!it.is_directory()) {
                
            }
        }*/

        // render file name below icon
        /*ImGui::TextWrapped(pathStr.c_str());
        ImGui::NextColumn();
        ImGui::PopID();*/
    //}
    //ImGui::Columns(1);

    ImGui::End();
}

// Bean: Temporary add a viewport
void EditorSystem::Editor_SceneViewport() {
    
    if (ImGui::Begin("Scene"))
    {
        static glm::vec2 _newDimension, sceneDimension{ 1600, 900 };

        unsigned int textureID = EditorCam.getFramebuffer().get_color_attachment_id();
        ImVec2 viewportEditorSize = ImGui::GetContentRegionAvail();
        _newDimension = *((glm::vec2*) &viewportEditorSize);

        // Only if the current scene dimension is not the same as new dimension
        if (sceneDimension != _newDimension && _newDimension.x != 0 && _newDimension.y != 0)
        {
            sceneDimension = { _newDimension.x, _newDimension.y };
            EditorCam.onResize(sceneDimension.x, sceneDimension.y);

            EditorCam.getFramebuffer().resize(sceneDimension.x, sceneDimension.y);
        }

        ImGui::Image((void*) (size_t) textureID, ImVec2{ (float) sceneDimension.x, (float) sceneDimension.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });

    }
    ImGui::End();
}

void EditorSystem::Editor_Toolbar() {
    ImGui::Begin("Toolbar");
    ImGui::End();
}

void EditorSystem::Editor_Inspector() {
    ImGui::Begin("Inspector");
    ImGui::End();
}

void EditorSystem::Editor_Hierarchy() {
    ImGui::Begin("Hierarchy");
    ImGui::End();
}
