#version 450 core

layout (location = 0) in vec3 FragmentPos;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

layout (location = 2) out vec3 gPos;
layout (location = 3) out vec3 gNormal;
layout (location = 4) out vec4 gAlbedoSpec;

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_spec;

void main(){
	gPos = FragmentPos;
	gNormal = normalize(Normal);
	gAlbedoSpec.rgb = texture(texture_diffuse, TexCoord).rgb;
	gAlbedoSpec.a = texture(texture_spec, TexCoord).r;
}