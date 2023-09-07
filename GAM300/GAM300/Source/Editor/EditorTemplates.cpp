/*!***************************************************************************************
\file			EditorTemplates.cpp
\project		GAM300
\author			Joseph Ho
\co-author      Sean Ngo

\par			Course: GAM300
\date           31/08/2023

\brief
    This file contains the definitions of the following:
    1. Editor Templates

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "EditorTemplates.h"
#include "Editor.h"

#define TEXT_BUFFER_SIZE 128



//static ImGuiTableFlags windowFlags =
//ImGuiTableFlags_Resizable |
//ImGuiTableFlags_NoBordersInBody |
//ImGuiTableFlags_NoSavedSettings |
//ImGuiTableFlags_SizingStretchProp;
//
//template <typename T>
//void Display(const char* name, T& val);
//
//void DisplayType(const char* name, bool& val)
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    ImGui::Checkbox(idName.c_str(), &val);
//}
//
//void DisplayType(const char* name, int& val)
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    ImGui::DragInt(idName.c_str(), &val);
//}
//
//template <size_t SZ>
//void DisplayType(const char* name, char(&val)[SZ])
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    ImGui::InputTextMultiline(idName.c_str(), val, SZ, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
//}
//
//void DisplayType(const char* name, char*& val)
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    ImGui::InputTextMultiline(idName.c_str(), val, TEXT_BUFFER_SIZE, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
//}
//
//void DisplayType(const char* name, float& val)
//{
//    static float temp{};
//    static std::string idName{};
//    const char* cIdName{};
//    if (name[0] == '#' && name[1] == '#')
//    {
//        cIdName = name;
//    }
//    else
//    {
//        idName = "##";
//        idName += name;
//        cIdName = idName.c_str();
//    }
//    ImGui::DragFloat(cIdName, &val, 0.15f);
//}
//
//void DisplayType(const char* name, double& val)
//{
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    float temp{ (float)val };
//    ImGui::DragFloat(idName.c_str(), &temp, 0.15f);
//    val = temp;
//}
//
//void DisplayType(const char* name, Vector3& val)
//{
//    static float temp{};
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    if (ImGui::BeginTable("Vector3", 3, windowFlags))
//    {
//        ImGui::TableNextColumn();
//        ImGui::AlignTextToFramePadding();
//        idName += 'X';
//        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
//        DisplayType(idName.c_str(), val.x);
//
//        ImGui::TableNextColumn();
//        idName.back() = 'Y';
//        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
//        DisplayType(idName.c_str(), val.y);
//
//        ImGui::TableNextColumn();
//        idName.back() = 'Z';
//        ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
//        DisplayType(idName.c_str(), val.z);
//        ImGui::EndTable();
//    }
//}
//
//void DisplayType(const char* name, Vector2& val)
//{
//    static float temp{};
//    static std::string idName{};
//    idName = "##";
//    idName += name;
//    if (ImGui::BeginTable("Vector2", 2, windowFlags))
//    {
//        ImGui::TableNextColumn();
//        ImGui::AlignTextToFramePadding();
//        idName += 'X';
//        ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
//        DisplayType(idName.c_str(), val.x);
//
//        ImGui::TableNextColumn();
//        idName.back() = 'Y';
//        ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
//        DisplayType(idName.c_str(), val.y);
//
//        ImGui::EndTable();
//    }
//}
//
////void DisplayType(const char* name, AABB& val)
////{
////    static std::string idName{};
////    idName = "##";
////    idName += name;
////
////    if (ImGui::BeginTable("AABB", 2, windowFlags))
////    {
////        ImGui::AlignTextToFramePadding();
////        Display("Max", val.max);
////        Display("Min", val.min);
////        ImGui::EndTable();
////    }
////}
//
//
//template <typename T>
//void Display(const char* name, T& val)
//{
//    ImGui::AlignTextToFramePadding();
//    ImGui::TableNextColumn();
//    ImGui::Text(name);
//    ImGui::TableNextColumn();
//    DisplayType(name, val);
//}
//
//void Display(const char* name, Script*& val, const char* scriptName)
//{
//    ImGui::AlignTextToFramePadding();
//    ImGui::TableNextColumn();
//    ImGui::Text(name);
//    ImGui::TableNextColumn();
//    //DisplayType(name, val, scriptName);
//}
//
//void Display(const char* string)
//{
//    ImGui::TableNextColumn();
//    ImGui::AlignTextToFramePadding();
//    ImGui::Text(string);
//}
//
////void Display(const char* name, Field& field)
////{
////    switch (field.fType)
////    {
////    case FieldType::Float:
////        Display(name, field.Get<float>());
////        break;
////    case FieldType::Double:
////        Display(name, field.Get<double>());
////        break;
////    case FieldType::Bool:
////        Display(name, field.Get<bool>());
////        break;
////    case FieldType::Char:
////        //Display(name, field.Get<char>());
////        break;
////    case FieldType::Short:
////        Display(name, field.Get<int>());
////        break;
////    case FieldType::Int:
////        Display(name, field.Get<int>());
////        break;
////    case FieldType::Long:
////        Display(name, field.Get<int>());
////        break;
////    case FieldType::UShort:
////        break;
////    case FieldType::UInt:
////        break;
////    case FieldType::ULong:
////        break;
////    case FieldType::String:
////    {
////        Display(name, (char*&)field.data);
////        break;
////    }
////    case FieldType::Vector2:
////    {
////        Display(name, field.Get<Math::Vec2>());
////        break;
////    }
////    case FieldType::Vector3:
////    {
////        Display(name, field.Get<Math::Vec3>());
////        break;
////    }
////    }
////}
//
////template <typename T>
////void DisplayColor(const char* name, T& val)
////{
////    static_assert(sizeof(T) == sizeof(float) * 4);
////    ImGui::TableNextColumn();
////    ImGui::Text(name);
////    ImGui::TableNextColumn();
////    static ImGuiColorEditFlags miscFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip
////        | ImGuiColorEditFlags_NoLabel;
////    if (ImGui::ColorButton(name, reinterpret_cast<ImVec4&>(val), miscFlags, ImVec2(FLT_MAX, 0)))
////    {
////        editedColor = reinterpret_cast<float*>(&val);
////    }
////
////}
//
////template <>
////void DisplayComponent<Button>(Button& btn)
////{
////    DisplayDragDrop();
////    spriteRenderer.sprite.set_name()
////    Display("Target Graphic", btn.targetGraphic);
////    Display("Bounds", btn.bounds);
////    DisplayColor("Normal Color", btn.normalColor);
////    DisplayColor("Hover Color", btn.hoverColor);
////    DisplayColor("Clicked Color", btn.clickedColor);
////    Display("Bounds", btn.fadeDuration);
////}
//
////template <>
////void DisplayComponent<Animator>(Animator& animator)
////{
////
////    DisplayDragDrop();
////    spriteRenderer.sprite.set_name()
////    Display("Loop", animator.loop);
////    Display("Freeze", animator.freeze);
////
////    ImGui::TableNextColumn();
////    ImGui::Text("Play Animation");
////    std::string toggleAnimation = "Play";
////    if (animator.status == Animator::AnimatorStatus::playing)
////        toggleAnimation = "Stop";
////    ImGui::TableNextColumn();
////    if (ImGui::Button(toggleAnimation.c_str(), ImVec2(ImGui::GetColumnWidth() * 0.2f, 0.f)))
////    {
////        Animation* anim{ animator.GetCurrentAnimation() };
////
////        if (animator.status == Animator::AnimatorStatus::idle)
////            animator.status = Animator::AnimatorStatus::playing;
////        else
////        {
////            animator.status = Animator::AnimatorStatus::idle;
////            anim->ResetFrame();
////        }
////
////    }
////    Display("Number of Animations:");
////    Display(std::to_string(animator.get_animation_vector().size()).c_str());
////    if (ImGui::Button("Add Animation"))
////    {
////        animator.AddAnimation();
////    }
////
////     For each animation display appropriate things
////    for (int i{ 0 }; i < animator.animations.size(); ++i)
////    {
////        ImGui::PushID(i);
////        ImGui::TableNextRow();
////        ImGui::TableNextColumn();
////        ImGui::Text("Animation %d", i + 1);
////        ImGui::TableNextRow();
////        Display("Number of Frames", animator.animations[i].frameCount);
////        Display("Current Frame", animator.animations[i].currentFrameIndex);
////        Display("Columns", animator.animations[i].spriteSheet.columns);
////        Display("Rows", animator.animations[i].spriteSheet.rows);
////        Display("Time Delay", animator.animations[i].timeDelay);
////        Display("Sprite", animator.animations[i].spriteSheet.texture);
////
////        ImGui::TableNextColumn();
////        ImGui::Text("Flip");
////        ImGui::TableNextColumn();
////        ImGui::Checkbox("X", &animator.animations[i].spriteSheet.flip.x);
////        ImGui::SameLine(0.f, 16.f);
////        ImGui::Checkbox("Y", &animator.animations[i].spriteSheet.flip.y);
////
////         Update sprite data
////        if (animator.animations[i].spriteSheet.texture)
////        {
////            std::string filePath = animator.animations[i].spriteSheet.texture->get_file_path();
////            uint64_t pathID = std::hash<std::string>{}(filePath);
////            MetaID metaID = MyAssetSystem.GetMetaID(pathID);
////            animator.animations[i].spriteSheet.spriteID = metaID.uuid;
////            size_t pos = filePath.find_last_of('\\');
////            animator.animations[i].spriteSheet.name = filePath.substr(pos + 1, filePath.length() - pos);
////        }
////
////        ImGui::PopID();
////    }
////}
//
//
////template <>
////void DisplayComponent<AudioSource>(AudioSource& audioSource)
////{
////    ImGui::Indent();
////    // Sprite
////    // Extern source file
////
////    ImGui::TableNextRow();
////    ImGui::TableNextColumn();
////    ImGui::Text("Audio File");
////    ImGui::TableNextColumn();
////    ImGui::Button(audioSource.alias.c_str(), ImVec2(-FLT_MIN, 0.f));
////    if (ImGui::BeginDragDropTarget())
////    {
////        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ContentBrowserItem"))
////        {
////            std::string str = (const char*)(payload->Data);
////            size_t fileDot = str.find_last_of(".");
////            std::string extension = str.substr(fileDot, str.size());
////
////            if (extension == ".wav")
////            {
////                audioSource.stop_sound();//stop any currently playing audio
////
////                size_t lastSlash = str.find_last_of("\\");
////                std::string temp = str.substr(lastSlash + 1);
////                size_t lastDot = temp.find_last_of(".");
////                audioSource.alias = temp.substr(0, lastDot);
////                //std::cout << "Alias: " << temp << "\n";
////
////
////                if (MySoundSystem.soundList.find(audioSource.alias) == MySoundSystem.soundList.end())//if its true it means file doesnt exist yet
////                {
////                    PRINT("New sound file detected: " << str << " / Alias (" << audioSource.alias << ")");
////                    SoundSystem::Instance()->CreateSound(str, audioSource.alias);
////                }
////                else
////                {
////                    MySoundSystem.soundList[audioSource.alias].first->getVolume(&audioSource.volume);
////                }
////            }
////            else
////            {
////                PRINT("Wrong file type");
////                Window::EditorConsole::editorLog.add_logEntry("AudioSource only accepts the.wav file format");
////                Window::EditorConsole::editorLog.bring_to_front();
////            }
////
////        }
////        ImGui::EndDragDropTarget();
////    }
////
////
////    //drop down for channel sellection
////    if (ImGui::BeginCombo("##combo", audioSource.channel.c_str()))
////    {
////        for (int n = 0; n < IM_ARRAYSIZE(audioSource.channelName); n++)
////        {
////            bool is_selected = (audioSource.channel == audioSource.channelName[n]);
////            if (ImGui::Selectable(audioSource.channelName[n], is_selected))
////            {
////                audioSource.channel = audioSource.channelName[n];
////            }
////            if (is_selected)
////                ImGui::SetItemDefaultFocus();
////        }
////        ImGui::EndCombo();
////    }
////    ImGui::SameLine();
////    ImGui::Text("Channel");
////
////    //volume slider
////    ImGui::SliderFloat("Volume", &audioSource.volume, 0.0f, 1.0f, "%.2f");
////    SoundSystem::Instance()->soundList[audioSource.alias].first->setVolume(audioSource.volume);
////
////    ImGui::Checkbox("Overlap", &audioSource.overLap);
////    ImGui::Checkbox("Loop", &audioSource.loop);
////
////    if (ImGui::Button("Preview")) //play
////    {
////        if (audioSource.alias.size())
////        {
////            audioSource.play_sound();
////        }
////        else
////        {
////            Window::EditorConsole::editorLog.add_logEntry("Error: No audio file to preview");
////        }
////    }
////    ImGui::SameLine();
////    if (ImGui::Button("Stop")) //stop
////    {
////        if (audioSource.alias.size())
////        {
////            MySoundSystem.Stop(audioSource.alias);
////        }
////        else
////        {
////            Window::EditorConsole::editorLog.add_logEntry("Error: No audio file to stop");
////        }
////    }
////
////
////    if (ImGui::Button("Stop ALL"))
////    {
////        if (audioSource.alias.size())
////        {
////            PRINT("Stopping all audio");
////            MySoundSystem.StopAll();
////        }
////    }
////
////    ImGui::Unindent();
////    //ImGui::EndTable();
////
////}
//
////template <>
////void DisplayComponent<Camera>(Camera& camera)
////{
////    bool openPopup = false;
////
////    glm::vec4 clrGLM = camera.GetBackgroundColor();
////
////    ImVec4 color = { clrGLM.r, clrGLM.g, clrGLM.b, clrGLM.a };
////
////    static ImVec4 backupColor;
////
////    ImGuiColorEditFlags miscFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip
////        | ImGuiColorEditFlags_NoLabel;
////
////    ImGui::Indent();
////    // Sprite
////    // Extern source file
////
////    // Background Color
////    ImGui::TableNextRow();
////    ImGui::TableNextColumn();
////    ImGui::Text("Color");
////    ImGui::TableNextColumn();
////    openPopup = ImGui::ColorButton("Color", color, miscFlags, ImVec2(FLT_MAX, 0));
////
////    // Projection
////    ImGui::TableNextRow();
////    ImGui::TableNextColumn();
////    ImGui::Text("Projection");
////    ImGui::TableNextColumn();
////
////    ImGui::Text("Orthographic"); ImGui::SameLine();
////    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
////    bool temp = camera.IsOrthographic();
////    if (ImGui::Checkbox("", &temp))
////    {
////        camera.SetOrthographic(temp);
////    }
////    ImGui::PopItemFlag();
////
////    // Clipping Planes
////    ImGui::TableNextRow();
////    ImGui::TableNextColumn();
////    ImGui::Text("Clipping Planes");
////    ImGui::TableNextColumn();
////
////    ImGui::PushID(0);
////    ImGui::Text("Near"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
////    float temp2 = camera.GetNearClip();
////    if (ImGui::InputFloat("", &temp2))
////    {
////        camera.SetNearClip(temp2);
////    }
////    if (ImGui::IsItemEdited())
////        camera.update_ortho_projection();
////    ImGui::PopID();
////
////    ImGui::PushID(1);
////    ImGui::Text("Far"); ImGui::SameLine(); ImGui::SetNextItemWidth(-FLT_MIN);
////    float temp3 = camera.GetFarClip();
////    if (ImGui::InputFloat("", &temp3))
////    {
////        camera.SetFarClip(temp3);
////    }
////    if (ImGui::IsItemEdited())
////        camera.update_ortho_projection();
////    ImGui::PopID();
////
////    ImGui::Unindent();
////    //ImGui::EndTable();
////}