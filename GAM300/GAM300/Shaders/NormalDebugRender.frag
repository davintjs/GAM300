/*!***************************************************************************************
\file			NormalDebugRender.frag
\project
\author         Euan Lim Yiren

\par			Course: GAM300
\date           07/11/2023

\brief
    This file is the frag file for drawing debug VAOs. ( Currently used for Physics debug drawing )

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core

uniform vec3 uColor;

out vec4 FragColor;

//End

void main()
{
	FragColor = vec4(uColor, 1.f);
}