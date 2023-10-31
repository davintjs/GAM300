/*!***************************************************************************************
\file			LightAffected.vert
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
layout (location = 0) in vec3 aNormalPosition;

out vec3 FragmentPos;
out vec3 Normal;



uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;

//End

void main()
{
	FragmentPos = vec3(SRT * vec4(aVertexPosition, 1.0));

	// DO THIS TRANSPOSE INVERSE IN CPU THEN UNIFORM INTO SHADER
    Normal = mat3(transpose(inverse(SRT))) * aNormalPosition;

	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);
} 