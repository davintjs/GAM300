/*!***************************************************************************************
\file			InstancedDebugRender.vert
\project
\author         Euphrasia Theophelia Tan Ee Mun

\par			Course: GAM300
\date           28/09/2023

\brief
    This file is the vert file for drawing debug lines.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core
//-------------------------
//          COMING IN
//-------------------------
layout (location = 0) in vec3 aVertexPosition;
layout (location = 6) in mat4 SRT;
layout (location = 10) in vec4 aAlbedo;

//-------------------------
//          GOING OUT
//-------------------------
layout (location = 3) out vec4 vAlbedo;


uniform mat4 persp_projection;
uniform mat4 View;

//End

void main()
{
	vAlbedo = aAlbedo;
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);

} 