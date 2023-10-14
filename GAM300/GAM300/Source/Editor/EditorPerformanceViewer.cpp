/*!***************************************************************************************
\file			EditorPerformanceViewer.cpp
\project
\author		    Joseph Ho

\par			Course : GAM300
\date           04/09/2023

\brief
    This file contains the definitions of the following :
        1. Performance Viewer Window that shows the performances of the different systems in
           the architecture

All content � 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/

#include "Precompiled.h"
#include "EditorHeaders.h"
#include "Core/EngineCore.h"

#pragma warning( disable : 4996)

ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | 
ImGuiTableFlags_NoHostExtendX;

void EditorPerformanceViewer::Init() {

}

void EditorPerformanceViewer::Update(float dt)
{
    if (ImGui::Begin("Performance Viewer")){
        //settings for collapsing headers
        static bool fpsgraph = true;
        static bool systemtable = true;
        static bool systemgraphs = true;
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;

        //settings for plot graphs
        static float time = 0;
        time = EngineCore::Instance().app_time;
        static float history = 10.0f;
        static ImPlotFlags plotflags = ImPlotFlags_NoMouseText;
        static ImPlotAxisFlags axisflags = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax;

        fpsgraph = ImGui::CollapsingHeader("FPS Graph", nodeFlags);
        if (fpsgraph) {
            bool update = false;
            static float update_timer = 0.f;
            static float fps = 0.f;

            if (update_timer > 0.f) {
                update_timer -= dt;
            }
            else {
                update_timer = UPDATE_TIME;
                update = true;
            }

            if (update) {
                fps = 1.f / dt;
                update = false;
            }

            ImGui::Text("FPS: %1.f fps", fps);

            static ScrollingBuffer fpsbuf;
            fpsbuf.AddPoint(time, fps);
            if (ImPlot::BeginPlot("FPS", ImVec2(-1, 180), plotflags)) {
                ImPlot::SetupAxes("Time (s)", "FPS", axisflags, axisflags);
                ImPlot::SetupAxisLimits(ImAxis_X1, time - history, time, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 200.f);
                //ImPlot::DragLineY(0, &highestfps, ImVec4(0.1, 0.1, 0.1, 0.1), 1.f, ImPlotDragToolFlags_NoInputs);
                ImPlot::PlotLine("FPS", &fpsbuf.Data[0].x, &fpsbuf.Data[0].y, fpsbuf.Data.size(), 0, fpsbuf.Offset, 2 * sizeof(float));
                ImPlot::EndPlot();
            }
        }
        
        systemtable = ImGui::CollapsingHeader("Systems Table", nodeFlags);
        if (systemtable) {
            if (ImGui::BeginTable("PerfViewer", 3, tableFlags)) {

                ImGui::TableSetupColumn("System");
                ImGui::TableSetupColumn("Usage");
                char times[100];
                sprintf(times, "Elasped Time (Total: %f ms)", EngineCore::Instance().systemtotaltime * 1000.f);

                ImGui::TableSetupColumn(times);
                ImGui::TableHeadersRow();
                for (auto system : EngineCore::Instance().system_times) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    // ImGui::PushID(system.first.c_str());
                     //ImGui::AlignTextToFramePadding();
                    ImGui::Text(system.first.c_str());
                    float usage = (system.second / EngineCore::Instance().systemtotaltime);
                    float percentage = usage * 100.f;
                    char percent[32];
                    sprintf(percent, "%.2f%%", percentage);

                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::ProgressBar(usage, ImVec2(0.0f, 0.0f), percent);

                    ImGui::TableNextColumn();
                    char systime[100];
                    sprintf(systime, "%f ms", system.second * 1000.f);
                    ImGui::Text(systime);
                    //ImGui::PopID();
                    //ImGui::Text(": %.1f%%", time);
                }
                ImGui::EndTable();
            }
        }
      
        systemgraphs = ImGui::CollapsingHeader("System Graphs", nodeFlags);
        if (systemgraphs) {
            if (ImPlot::BeginPlot("System Usage", ImVec2(-1, 350), plotflags)) {
                ImPlot::SetupLegend(ImPlotLocation_North, ImPlotLegendFlags_Horizontal);
                ImPlot::SetupAxes("Time (s)", "Usage (%)", axisflags, axisflags);
                ImPlot::SetupAxisLimits(ImAxis_X1, time - history, time, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100.0, ImPlotCond_Always);
                ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 1.f);
                for (auto system : EngineCore::Instance().system_times) {
                    system_plots[system.first].AddPoint(time, (EngineCore::Instance().system_times[system.first] / EngineCore::Instance().systemtotaltime) * 100.f);
                    ImPlot::PlotLine(system.first.c_str(), &system_plots[system.first].Data[0].x, &system_plots[system.first].Data[0].y,
                        system_plots[system.first].Data.size(), 0, system_plots[system.first].Offset, 2 * sizeof(float));
                }

                ImPlot::EndPlot();
            }
        }     
    }
    ImGui::End();

}

void EditorPerformanceViewer::Exit()
{

}