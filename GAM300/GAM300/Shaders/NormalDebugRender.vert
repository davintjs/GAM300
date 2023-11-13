/*!***************************************************************************************
\file			NormalDebugRender.vert
\project
\author         Euan Lim Yiren

\par			Course: GAM300
\date           07/11/2023

\brief
    This file is the vert file for drawing debug VAOs. ( Currently used for Physics debug drawing )

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core
layout (location = 0) in vec3 aPosition;


uniform mat4 SRT;
uniform mat4 persp_projection;
uniform mat4 View;

//End

void main()
{
	//vColor = aVertexColor;
	gl_Position = persp_projection * View * SRT * vec4(aPosition, 1.0f);

} 