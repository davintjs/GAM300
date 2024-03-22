#version 450 core

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;
layout (location = 2) in float lifeTimes;

layout (binding = 0) uniform sampler2D albedoTexture;

uniform float AoConstant;
uniform float EmissionConstant;
uniform float bloomThreshold;
uniform float ambience_multiplier;
uniform vec4 frag_Albedo;
uniform bool hasTexture;

uniform bool fadeToColor;
uniform vec3 colorToFadeTo;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Blooming;

void main()
{
    vec3 albedo = vec3(frag_Albedo);
    float alpha = frag_Albedo.a;

    if(hasTexture){
        vec4 texColor = texture(albedoTexture, TexCoords);
        albedo *= pow(texColor.rgb * frag_Albedo.rgb, vec3(2.2));
        alpha *= texColor.a;
    }
    if(alpha < 0.05f){
        discard;
    }

    if(fadeToColor)
    {
        FragColor = vec4(colorToFadeTo * lifeTimes, alpha);
    }


    vec3 color = vec3(ambience_multiplier) * albedo * AoConstant + EmissionConstant;
    //color /= (color + vec3(1.0));
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

    FragColor = vec4(color, alpha);
//    FragColor.a *= alpha;
//    FragColor.rgb = pow(FragColor.rgb, vec3(2.2f));

    


    if(brightness > bloomThreshold)
        Blooming = vec4(color.rgb, 1.f);
    else
        Blooming = vec4(0.0, 0.0, 0.0, 1.f);
}
