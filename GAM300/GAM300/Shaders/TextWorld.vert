/*!***************************************************************************************
\file			TextWorld.vert
\project
\author         Euan Lim

\par			Course: GAM300
\date           05/11/2023

\brief
	Vertex Shader for Rendering UI onto WorldSpace

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core


//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

//-------------------------
//          GOING OUT
//-------------------------

out vec2 TexCoords;

//-------------------------
//          UNIFORMS
//-------------------------

uniform mat4 SRT;
uniform mat4 projection;
uniform mat4 view;

//End

void main()
{
    TexCoords = vertex.zw;

	gl_Position = projection * view * SRT * vec4(vertex.xy, 0.0,1.0);
}
