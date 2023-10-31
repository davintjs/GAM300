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

uniform vec3 uColor;

out vec4 FragColor;

//End

void main()
{
	FragColor = vec4(uColor, 1.f);
}