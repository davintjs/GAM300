/*!
@file    abnb2.vert
@author  tan.e@digipen.edu
@date    7/6/2022

This file contains a vertex shader.

*//*__________________________________________________________________________*/
#version 450 core

layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
layout (location = 4) in vec4 aVertexColor;

//testing
//layout (location=1) in vec3 aVertexColor;

layout (location = 0) out vec4 vColor;
layout (location = 1) out vec2 vTexCoord;

uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;


void main() {
				// Proj * View() * model(SRT)  * vertex pos
	gl_Position = persp_projection *View * SRT * vec4(aVertexPosition, 1.0f);

	//TODO use normals and tangents

	vTexCoord = aVertexTexCoord;
	vColor = aVertexColor;
}