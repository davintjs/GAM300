#version 450 core

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;
layout (location = 2) in vec2 lifeTimes;

layout (binding = 0) uniform sampler2D albedoTexture;

uniform float AoConstant;
uniform float EmissionConstant;
uniform float bloomThreshold;
uniform float ambience_multiplier;
uniform vec4 frag_Albedo;
uniform bool hasTexture;

uniform bool fadeToColor;
uniform vec3 colorToFadeTo;
uniform float maxLifetime;

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

     // LERP the colors
    if(fadeToColor)
    {
        
        float lerp = 1.f - (lifeTimes.x / lifeTimes.y);
//        float lerp = lifeTimes / maxLifetime;
//            lerp = abs(lerp);
        albedo = mix(albedo,colorToFadeTo,lerp);
//        albedo.r = mix(albedo.r,colorToFadeTo.r,lerp);
//
//        albedo.g = mix(albedo.g,colorToFadeTo.g,lerp);
//
//        albedo.b = mix(albedo.b,colorToFadeTo.b,lerp);
        
//        albedo.r = mix(colorToFadeTo.r, albedo.r,lerp);
//
//        albedo.g = mix(colorToFadeTo.g, albedo.g,lerp);
//
//        albedo.b = mix(colorToFadeTo.b, albedo.b,lerp);

//        albedo = vec3(lerp,lerp,lerp);
    }


    vec3 color = vec3(ambience_multiplier) * albedo * AoConstant + EmissionConstant;
    //color /= (color + vec3(1.0));
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

    FragColor = vec4(color, alpha);
    Blooming = FragColor;
//    FragColor.a *= alpha;
//    FragColor.rgb = pow(FragColor.rgb, vec3(2.2f));

    


//    if(brightness > bloomThreshold)
//        Blooming = vec4(color.rgb, 1.f);
//    else
//        Blooming = vec4(0.0, 0.0, 0.0, 1.f);
}
