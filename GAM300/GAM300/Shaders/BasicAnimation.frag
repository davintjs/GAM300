// robbed from opengl

#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = texture(texture_diffuse1, vec2(TexCoords.x, 1.0 - TexCoords.y));
;
}