/*!***************************************************************************************
\file			EditorTemplates.h
\project		GAM300
\author			Joseph Ho
\co-author      Sean Ngo

\par			Course: GAM300
\date           31/08/2023

\brief
    This file contains the declarations of the following:
    1. Editor Templates

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#ifndef EDITORTEMPLATES_H
#define EDITORTEMPLATES_H

#include "Scene/Components.h"
#include "EditorHeaders.h"

// Includes all imgui templates to be used in developing the editor, the purpose of this
// file is to modulate the components of the editor to allow for easy-er development of the 
// editor
class OurGUI
{
    // Flags
    template <typename T>
    void Flags(); // Pre-initialize flags for all different editor settings


};

//FormatMiddleWrapper and FormatButtonMiddle formats a button to render in the middle of the current window
class FormatMiddleWrapper {
public:
    explicit FormatMiddleWrapper(bool result) : result_(result) {}

    operator bool() const {
        return result_;
    }

private:
    bool result_;
};

class FormatButtonMiddle {
public:
    FormatButtonMiddle(ImVec2 windowSize) : windowSize_(windowSize) {}

    template<typename Func>
    FormatMiddleWrapper operator()(Func control) const {
        ImVec2 originalPos = ImGui::GetCursorPos();

        // Draw offscreen to calculate size
        ImGui::SetCursorPos(ImVec2(-10000.0f, -10000.0f));
        control();
        ImVec2 controlSize = ImGui::GetItemRectSize();

        // Draw at centered position
        ImGui::SetCursorPos(ImVec2((windowSize_.x - controlSize.x) * 0.5f, originalPos.y));
        control();

        return FormatMiddleWrapper(ImGui::IsItemClicked());
    }

private:
    ImVec2 windowSize_;
};

#define CENTERED_CONTROL(control) FormatButtonMiddle{ImGui::GetWindowSize()}([&]() { control; })

//void DisplayType(const char* name, bool& val);
//
//void DisplayType(const char* name, int& val);
//
//template <size_t SZ>
//void DisplayType(const char* name, char(&val)[SZ]);
//
//void DisplayType(const char* name, char*& val);
//
//void DisplayType(const char* name, float& val);
//
//void DisplayType(const char* name, double& val);
//
//void DisplayType(const char* name, Vector3& val);
//
//void DisplayType(const char* name, Vector2& val);

//void DisplayType(const char* name, AABB& val);

//template <typename T>
//void Display(const char* name, T& val);

//template <typename T>
//void DisplayComponent(T& component);

//template <>
//void DisplayComponent<Transform>(Transform& transform);
//
//template <>
//void DisplayComponent<BoxCollider>(BoxCollider& boxCollider2D);

//template <>
//void DisplayComponent<SpriteRenderer>(SpriteRenderer& spriteRenderer);

//template <>
//void DisplayComponent<Rigidbody>(Rigidbody& rb);

//template <>
//void DisplayComponent<Script>(Script& script);



//void Display(const char* name, Script*& val, const char* scriptName);
//
//void Display(const char* string);

//void Display(const char* name, Field& field);

//template <typename T>
//void DisplayColor(const char* name, T& val);

//template <>
//void DisplayComponent<Button>(Button& btn);

//template <>
//void DisplayComponent<Animator>(Animator& animator);


//template <>
//void DisplayComponent<AudioSource>(AudioSource& audioSource);

//template <>
//void DisplayComponent<Camera>(Camera& camera);


#endif // !EDITORTEMPLATES_H

