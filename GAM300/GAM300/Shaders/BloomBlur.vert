/*!***************************************************************************************
\file			BloomBlur.vert
\project
\author         Euan Lim

\par			Course: GAM300
\date           05/11/2023

\brief
	Vertex Shader for Blooming Effect

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
//End
void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}