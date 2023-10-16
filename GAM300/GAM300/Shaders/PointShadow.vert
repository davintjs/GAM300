#version 450 core
layout (location = 0) in vec3 aPos;

layout (location = 6) in mat4 SRT;

//uniform mat4 model;

void main()
{
    gl_Position = SRT * vec4(aPos, 1.0);
}