/*!***************************************************************************************
\file			BasicLighting.vert
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Basic Shader - NOT IN USE CURRENTLY

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core
layout (location = 0) in vec3 aVertexPosition;

uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;

void main()
{
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);
} 