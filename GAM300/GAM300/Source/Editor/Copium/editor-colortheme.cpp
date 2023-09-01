/*!***************************************************************************************
\file			editor-colortheme.cpp
\project
\author			Shawn Tanary

\par			Course: GAM250
\par			Section:
\date			25/11/2022

\brief
    Contains the function declarations of the editor-colortheme

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
#include "pch.h"
#include "editor-colortheme.h"
#include <filesystem>
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>
#include "Utilities/thread-system.h"


namespace Copium
{
        Copium::ThreadSystem& threadSystem{ *Copium::ThreadSystem::Instance() };

		void EditorColorTheme::init()
		{
            //Force hotdog stand theme on them
            /*
                Window::ColorTheme::setTheme(Window::ColorTheme::color_for_text, Window::ColorTheme::color_for_head,
                                             Window::ColorTheme::color_for_area, Window::ColorTheme::color_for_body,
                                             Window::ColorTheme::color_for_pops);
             */
            isColorThemeOpen = true;
		}

		void EditorColorTheme::update()
		{
            if (!isColorThemeOpen)
            {
                return;
            }

            ImGui::Begin("Theme generator",&isColorThemeOpen);
            ImGui::ColorEdit3("Text Color", (float*)&color_for_text, ImGuiColorEditFlags_PickerHueBar);
            ImGui::ColorEdit3("Head Color", (float*)&color_for_head, ImGuiColorEditFlags_PickerHueBar);
            ImGui::ColorEdit3("Area Color", (float*)&color_for_area, ImGuiColorEditFlags_PickerHueBar);
            ImGui::ColorEdit3("Body Color", (float*)&color_for_body, ImGuiColorEditFlags_PickerHueBar);
            ImGui::ColorEdit3("Pop Color", (float*)&color_for_pops, ImGuiColorEditFlags_PickerHueBar);
            if (ImGui::Button("Apply Theme")) 
            {
                printf("Applied Theme");
                setTheme(color_for_text,color_for_head,color_for_area,color_for_body,color_for_pops);
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Theme"))
            {
                //Serialize
                setTheme(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
                while (!threadSystem.acquireMutex(Copium::MutexType::FileSystem));
                std::string fp = Copium::FileDialogs::save_file("Copium Theme (*.theme)\0.theme\0");
                threadSystem.returnMutex(Copium::MutexType::FileSystem);
                serialize(fp);

            }
            ImGui::SameLine();
            if (ImGui::Button("Load Theme"))
            {
                //DeSerialize
                while (!threadSystem.acquireMutex(Copium::MutexType::FileSystem));
                std::string fp = Copium::FileDialogs::open_file("Copium Theme (*.theme)\0*.theme\0");
                threadSystem.returnMutex(Copium::MutexType::FileSystem);
                deserialize(fp);
                if (fp.size())
                {
                    setTheme(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
                }
            }
            ImGui::End();
            
		}

		void EditorColorTheme::setTheme(Copium::Math::Vec3 _color_for_text, Copium::Math::Vec3 _color_for_head, Copium::Math::Vec3 _color_for_area, Copium::Math::Vec3 _color_for_body, Copium::Math::Vec3 _color_for_pops)
		{
            ImGuiStyle& style = ImGui::GetStyle();

            style.Colors[ImGuiCol_Text] = ImVec4(_color_for_text.x, _color_for_text.y, _color_for_text.z, 1.00f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(_color_for_text.x, _color_for_text.y, _color_for_text.z, 0.58f);
            style.Colors[ImGuiCol_WindowBg] = ImVec4(_color_for_body.x, _color_for_body.y, _color_for_body.z, 0.95f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 0.58f);
            style.Colors[ImGuiCol_Border] = ImVec4(_color_for_body.x, _color_for_body.y, _color_for_body.z, 0.00f);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(_color_for_body.x, _color_for_body.y, _color_for_body.z, 0.00f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.78f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 0.75f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 0.47f);
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.21f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.78f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_CheckMark] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.0f);
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.50f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_Button] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.50f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.86f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.76f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.86f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_Tab] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(_color_for_body.x, _color_for_body.y, _color_for_body.z, 1.00f);            
            style.Colors[ImGuiCol_TabActive] = ImVec4(_color_for_body.x, _color_for_body.y, _color_for_body.z, 1.00f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(_color_for_body.x, _color_for_body.y, _color_for_body.z, 1.00f);
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.15f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.78f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_PlotLines] = ImVec4(_color_for_text.x, _color_for_text.y, _color_for_text.z, 0.63f);
            style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_PlotHistogram] = ImVec4(_color_for_text.x, _color_for_text.y, _color_for_text.z, 0.63f);
            style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 1.00f);
            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(_color_for_head.x, _color_for_head.y, _color_for_head.z, 0.43f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(_color_for_pops.x, _color_for_pops.y, _color_for_pops.z, 0.92f);
            style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(_color_for_area.x, _color_for_area.y, _color_for_area.z, 0.73f);
		}


        void EditorColorTheme::serialize(const std::string& name)
        {
            std::string fp(name);
            if (fp.find(".theme") == std::string::npos)
            {
                return;
            }
            std::ofstream ofs(fp);  
            rapidjson::Document doc;
            doc.SetObject();
            rapidjson::Value txt(rapidjson::kObjectType), head(rapidjson::kObjectType), body(rapidjson::kObjectType), area(rapidjson::kObjectType), pops(rapidjson::kObjectType);
            txt.AddMember("R", color_for_text.x, doc.GetAllocator());
            txt.AddMember("G", color_for_text.y, doc.GetAllocator());
            txt.AddMember("B", color_for_text.z, doc.GetAllocator());
            doc.AddMember("Text", txt, doc.GetAllocator());

            head.AddMember("R", color_for_head.x, doc.GetAllocator());
            head.AddMember("G", color_for_head.y, doc.GetAllocator());
            head.AddMember("B", color_for_head.z, doc.GetAllocator());
            doc.AddMember("Head", head, doc.GetAllocator());

            body.AddMember("R", color_for_body.x, doc.GetAllocator());
            body.AddMember("G", color_for_body.y, doc.GetAllocator());
            body.AddMember("B", color_for_body.z, doc.GetAllocator());
            doc.AddMember("Body", body, doc.GetAllocator());

            area.AddMember("R", color_for_area.x, doc.GetAllocator());
            area.AddMember("G", color_for_area.y, doc.GetAllocator());
            area.AddMember("B", color_for_area.z, doc.GetAllocator());
            doc.AddMember("Area", area, doc.GetAllocator());

            pops.AddMember("R", color_for_pops.x, doc.GetAllocator());
            pops.AddMember("G", color_for_pops.y, doc.GetAllocator());
            pops.AddMember("B", color_for_pops.z, doc.GetAllocator());
            doc.AddMember("Pops", pops, doc.GetAllocator());

            rapidjson::StringBuffer sb;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
            doc.Accept(writer);
            ofs << sb.GetString();
            ofs.close();

        }
        void EditorColorTheme::deserialize(const std::string& _filepath)
        {
            std::ifstream ifs(_filepath);
            if (!ifs)
                return;

            rapidjson::Document doc;
            rapidjson::IStreamWrapper isw(ifs);
            if (doc.ParseStream(isw).HasParseError())
            {
                ifs.close();
                return;
            }

            if (doc.HasMember("Text"))
            {
                rapidjson::Value& txt = doc["Text"].GetObj();
                color_for_text.x = txt["R"].GetFloat();
                color_for_text.y = txt["G"].GetFloat();
                color_for_text.z = txt["B"].GetFloat();
            }
            if (doc.HasMember("Head"))
            {
                rapidjson::Value& head = doc["Head"].GetObj();
                color_for_head.x = head["R"].GetFloat();
                color_for_head.y = head["G"].GetFloat();
                color_for_head.z = head["B"].GetFloat();
            }
            if (doc.HasMember("Body"))
            {
                rapidjson::Value& body = doc["Body"].GetObj();
                color_for_body.x = body["R"].GetFloat();
                color_for_body.y = body["G"].GetFloat();
                color_for_body.z = body["B"].GetFloat();
            }
            if (doc.HasMember("Area"))
            {
                rapidjson::Value& area = doc["Area"].GetObj();
                color_for_area.x = area["R"].GetFloat();
                color_for_area.y = area["G"].GetFloat();
                color_for_area.z = area["B"].GetFloat();
            }
            if (doc.HasMember("Pops"))
            {
                rapidjson::Value& pops = doc["Pops"].GetObj();
                color_for_pops.x = pops["R"].GetFloat();
                color_for_pops.y = pops["G"].GetFloat();
                color_for_pops.z = pops["B"].GetFloat();
            }

        }

}