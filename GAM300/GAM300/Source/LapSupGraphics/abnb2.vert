/*!
@file    abnb2.vert
@author  tan.e@digipen.edu
@date    7/6/2022

This file contains a vertex shader.

*//*__________________________________________________________________________*/
#version 450 core

layout (location = 0) in vec3 aVertexPosition;

//testing
//layout (location=1) in vec3 aVertexColor;
layout (location=1) in vec2 aVertexTexture;

//layout (location = 0) out vec3 vColor;
layout (location = 0) out vec2 vTexture;

uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;


void main() {
				// Proj * View() * model(SRT)  * vertex pos
	gl_Position = persp_projection *View * SRT * vec4(aVertexPosition, 1.0f);

	//vColor = aVertexColor;
	//testing
	//vColor = vec3(1.0f, 1.0f, 1.0f);
	vTexture = aVertexTexture;
	//vTexture.x = 1.f - aVertexTexture.x;
}