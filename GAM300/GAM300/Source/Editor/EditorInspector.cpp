/*!***************************************************************************************
\file			EditorInspector.cpp
\project		GAM300
\author

\par			Course: GAM300
\date           00/00/2023

\brief
    This file contains the definitions of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "Editor.h"
#include "EditorHeaders.h"

void EditorInspector::Init()
{

}

void EditorInspector::Update(float dt)
{
    ImGui::Begin("Inspector");
    //Get Selected Entities from mouse picking
    //List out all components in order
    //templated functionalities (input fields, checkboxes etc.)
    ImGui::End();
}

void EditorInspector::Exit()
{

}