#version 450 core

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;

layout (binding = 0) uniform sampler2D albedoTexture;

uniform vec4 frag_Albedo;
uniform bool hasTexture;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Blooming;

void main()
{
    vec3 albedo = vec3(frag_Albedo);
    float alpha = frag_Albedo.a;
    vec4 texColor;
    if(hasTexture){
        texColor = texture(albedoTexture, TexCoords);
        albedo *= pow(texColor.rgb * frag_Albedo.rgb, vec3(2.2f));
        alpha *= texColor.a;
        FragColor = texColor;
        if(alpha < 0.05){
            discard;
        }
    }else
    {
        FragColor = vec4(albedo,alpha);
    }
    
    FragColor.a *= alpha;
    FragColor.rgb = pow(FragColor.rgb, vec3(2.2f));

}
