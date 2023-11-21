#version 450 core

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;

layout (binding = 0) uniform sampler2D albedoTexture;

uniform float AoConstant;
uniform float EmissionConstant;
uniform float bloomThreshold;
uniform float ambience_multiplier;
uniform vec4 frag_Albedo;
uniform bool hasTexture;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Blooming;

void main()
{
	//FragColor = vec4(uColor, 1.f);
//	if(!hasTexture){
//		FragColor = vec4(1.f);
//		return;
//	}
//
//	FragColor = texture(albedoTexture, TexCoords);
//
//	if(FragColor.z < 0.1){
//		discard;
//	}
    vec3 albedo = vec3(frag_Albedo);

    float ao = AoConstant;

	if(hasTexture){
		albedo = vec3(frag_Albedo) * pow(texture(albedoTexture, TexCoords).rgb, vec3(2.2));
	}
	
    vec3 color = vec3(ambience_multiplier) * albedo * ao + EmissionConstant;
    color = color / (color + vec3(1.0));
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

    FragColor = vec4(color, frag_Albedo.a);

    if(brightness > bloomThreshold)
        Blooming = vec4(color.rgb, 1.0);
    else
        Blooming = vec4(0.0, 0.0, 0.0, 1.0);
}