/*!***************************************************************************************
\file			EditorPerformanceViewer.cpp
\project
\author		    Joseph Ho

\par			Course : GAM300
\date           04/09/2023

\brief
    This file contains the definitions of the following :
        1. Performance Viewer Window

All content � 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/
#include "Precompiled.h"

#include "EditorHeaders.h"
#include "Core/EngineCore.h"




void EditorPerformanceViewer::Init()
{

}



void EditorPerformanceViewer::Update(float dt)
{
    if (ImGui::Begin("Performance Viewer")) {
        ImGui::Text("FPS: %1.f fps", EngineCore::Instance().get_FPS());
        for (auto system : EngineCore::Instance().system_times) {
            //render only if system takes up resources
            ImGui::Text(system.first.c_str());
            ImGui::SameLine();
            float time = (system.second / EngineCore::Instance().systemtotaltime) * 100.0f;
            ImGui::Text(": %.1f%%", time);
        } 
    }
    ImGui::End();

}

void EditorPerformanceViewer::Exit()
{

}