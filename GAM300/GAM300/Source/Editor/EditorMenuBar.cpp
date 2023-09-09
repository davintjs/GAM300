#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"
#include "Core/EventsManager.h"

#include "../Utilities/PlatformUtils.h"

void EditorMenuBar::Init()
{

}

void EditorMenuBar::Update(float dt)
{
    if (ImGui::BeginMainMenuBar())
    {
        //File Menu functionality
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                //New Scene
            }

            // Opening of files using file dialogs
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                std::string Filename = FileDialogs::OpenFile("Text Files (*.txt)\0*.txt\0");
                if (!Filename.empty())
                {
                    //Open File
                }
            }

            // Saving of scene files only if an active scene is open, using file dialogs
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                //Save File
                SaveSceneEvent saveScene;
                EVENT.Publish(&saveScene);
            }
            //else
            //    //Do not allow the user to save when there's no file loaded!
            //    ImGui::TextDisabled("Save", "Ctrl+S");

            // Save as functionality using file dialogs
            if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
            {
                std::string Filename = FileDialogs::SaveFile("Text Files (*.txt)\0*.txt\0");
                if (!Filename.empty())
                {
                    //DEBUG->Debugger.AddLog("[%i]{Info} File saved successfully!\n", DEBUG->debugcounter++);
                }
            }
            //else
            //    //Do not allow the user to save when there's no file loaded!
            //    ImGui::TextDisabled("Save As..", "Ctrl+Shift+S");

            // Quit application, opens a pop-up dialog to confirm exit
            if (ImGui::MenuItem("Quit", "Ctrl+Q"))
            {
                exitApp = true;
            }
            ImGui::EndMenu();
        }

        // Popup Modal for exiting application
        if (exitApp)
        {
            ImGui::OpenPopup("Exit?");
            exitApp = false;
        }
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Exit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to exit the application?\n\n");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
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
            if (ImGui::MenuItem("Copy", "Ctrl+C"))
            {
                //Copy Current Item
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V"))
            {
                //Creates a copy of the entity copieds
            }


            // Undo Functionality using Editor_Undo()
            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
            }

            // Do not allow the user to undo when history buffer is empty or when file is not loaded
            ImGui::TextDisabled("Undo");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorMenuBar::Exit()
{

}