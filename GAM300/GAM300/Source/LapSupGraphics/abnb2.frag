/*!
@file    abnb.frag
@author  tan.e@digipen.edu
@date    7/6/2022

This file contains a fragment shader.

*//*__________________________________________________________________________*/
#version 450 core

//test
layout(location=0) in vec4 vColor;
layout(location=1) in vec2 vTexCoord;

layout (location=0) out vec4 fFragColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main() {
	//fFragColor = vColor;
	fFragColor = texture( myTextureSampler, vTexCoord );
	//fFragColor = vec4(vTexCoord,0,1);
}
