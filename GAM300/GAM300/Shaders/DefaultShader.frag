#version 450 core

layout (location = 0) in vec2 TexCoord;

uniform sampler2D albedoTex;
uniform sampler2D normalMap;

out vec4 FragColor;

void main()
{
    FragColor = texture(albedoTex, TexCoord);
}