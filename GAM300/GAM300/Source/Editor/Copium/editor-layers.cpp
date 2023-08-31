/*!***************************************************************************************
\file			editor-layers.cpp
\project
\author			Sean Ngo
\co-author      Matthew Lau

\par			Course: GAM250
\par			Section:
\date			13/01/2023

\brief
	This file holds the definition of the functions in the editor layers gui

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#include "pch.h"

#include "Editor/editor-layers.h"

namespace Copium
{
	void EditorLayers::init()
	{
        
	}

	void EditorLayers::update()
	{
        
        ImGui::Begin("Tags and Layers");
			
        if (ImGui::CollapsingHeader("Tags"))
		{

		}

		if (ImGui::CollapsingHeader("Sorting Layers"))
		{
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));

            // Bean: Add a add and remove layer button

            if (ImGui::Button("Add Layer"))
            {
                std::string newLayer = "New Layer " + std::to_string(sortingLayers.GetLayerCount());
                sortingLayers.CreateNewLayer(newLayer);
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove Top Layer"))
            {
                sortingLayers.RemoveLayer(sortingLayers.GetSortingLayers().back().layerID);
            }

            ImGui::Indent();

            for (int i = 0; i < sortingLayers.GetSortingLayers().size(); i++)
            {
                ImGuiSelectableFlags flags = ImGuiSelectableFlags_AllowItemOverlap;
                char* name = sortingLayers.GetSortingLayers()[i].name.data();
                int id = sortingLayers.GetSortingLayers()[i].layerID + 1;
                std::string label = "##" + std::to_string(id);
                ImGui::Selectable(label.c_str(), false, flags);
                if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                {
                    int next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (next >= 0 && next < sortingLayers.GetLayerCount())
                    {
                        sortingLayers.SwapLayers(i, next);


                        ImGui::ResetMouseDragDelta();
                    }
                }
                ImGui::SameLine();
                std::string layer = "Layer                                  ";
                ImGui::Text(layer.c_str());
                ImGui::SameLine();

                ImGuiInputTextFlags textFlags = 0;
                if (!sortingLayers.GetSortingLayers()[i].name.compare("Default"))
                    textFlags = ImGuiInputTextFlags_ReadOnly;

                label = "##" + std::to_string(id * 100);
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputText(label.c_str(), name, (size_t) sortingLayers.GetCharLength(), textFlags);
                sortingLayers.GetSortingLayers()[i].name = name;
            }

            ImGui::Unindent();

            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
		}

		if (ImGui::CollapsingHeader("Layers"))
		{

		}

		ImGui::End();
	}

	void EditorLayers::exit()
	{
        sortingLayers.ClearAllLayer(true);
	}
}