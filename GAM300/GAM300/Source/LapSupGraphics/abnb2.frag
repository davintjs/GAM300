/*!
@file    abnb.frag
@author  tan.e@digipen.edu
@date    7/6/2022

This file contains a fragment shader.

*//*__________________________________________________________________________*/
#version 450 core

//test
layout(location=0) in vec2 vTexture;

//layout(location=0) in vec3 vColor;
layout (location=0) out vec4 fFragColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main() {
	//fFragColor = vec4(vColor, 1.0f);
	fFragColor = texture( myTextureSampler, vTexture );
}
