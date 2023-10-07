/*!***************************************************************************************
\file			Skybox.vert
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Vertex shader for skybox

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
//    vec4 pos = projection * view * mat4(vec4(1000, 0, 0, 0),vec4(0, 1000, 0, 0),vec4(0, 0, 1000, 0),vec4(0, 0, 0, 1)) * vec4(aPos, 1);
//    
//    gl_Position = pos.xyww;
}  