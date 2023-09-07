/*!
@file    debugAABB.frag
@author  tan.e@digipen.edu
@date    1/9/2023

This file contains a fragment shader.

*//*__________________________________________________________________________*/
#version 450 core

layout(location=0) in vec4 vColor;

layout (location=0) out vec4 fFragColor;

void main() {
	fFragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
