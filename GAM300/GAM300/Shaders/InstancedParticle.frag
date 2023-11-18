#version 450 core

layout (location = 0) in vec2 TexCoords;

out vec4 FragColor;
layout (binding = 0) uniform sampler2D textureSampler;

uniform bool hasTexture;


void main()
{
	//FragColor = vec4(uColor, 1.f);
	if(!hasTexture){
		FragColor = vec4(1.f);
		return;
	}

	FragColor = texture(textureSampler, TexCoords);

	if(FragColor.z < 0.1){
		discard;
	}
}