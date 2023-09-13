#version 450 core
layout (location = 0) in vec4 vColor;

out vec4 FragColor;


void main()
{
    FragColor = vec4(vColor); // set all 4 vector values to 1.0
}