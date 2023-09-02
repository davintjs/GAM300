#ifndef Debugger_H
#define Debugger_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


struct Debugger
{
    ImGuiTextBuffer     Buffer;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    int                 Debug_Counter;
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    Debugger();

    void Clear();

    void AddLog(const char* fmt, ...) IM_FMTARGS(2);

    void Draw();

    void Debug_Window(bool open);
};

#endif //Debugger_H