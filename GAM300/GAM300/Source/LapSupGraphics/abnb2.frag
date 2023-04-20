/*!
@file    abnb.frag
@author  tan.e@digipen.edu
@date    7/6/2022

This file contains a fragment shader.

*//*__________________________________________________________________________*/
#version 450 core


layout(location=0) in vec3 vColor;
layout (location=0) out vec4 fFragColor;

void main() {
	fFragColor = vec4(vColor, 1.0f);
}
