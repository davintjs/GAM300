/**************************************************************************************/
/*!
//    \file			InstancedDebugRender.vert
//    \author(s) 	Euphrasia Theophelia Tan Ee Mun
//
//    \date   	    15th September 2023
//    \brief		This file is the vert file for drawing debug lines.
//
//    \Percentage   Theophelia 100%
//
//    Copyright (C) 2022 DigiPen Institute of Technology.
//    Reproduction or disclosure of this file or its contents without the
//    prior written consent of DigiPen Institute of Technology is prohibited.
*/
/**************************************************************************************/

#version 450 core
layout (location = 0) in vec3 aVertexPosition;

layout (location = 6) in mat4 SRT;

uniform mat4 persp_projection;
uniform mat4 View;

void main()
{
	//vColor = aVertexColor;
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);

} 