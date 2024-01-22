/*!***************************************************************************************
\file			InstancedDebugRender.frag
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           28/09/2023

\brief
    This file is the frag file for drawing debug lines. 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core

//-------------------------
//          COMING IN
//-------------------------
layout (location = 3) in vec4 aAlbedo;

out vec4 FragColor;

//End

void main()
{
    FragColor = aAlbedo;
}