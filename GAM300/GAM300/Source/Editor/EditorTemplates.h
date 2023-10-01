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
// file is to modulate the components of the editor to allow for easyer development of the 
// editor

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

#endif // !EDITORTEMPLATES_H

