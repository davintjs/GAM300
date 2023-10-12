#version 450 core
layout (location = 0) in vec3 aPos;

layout (location = 6) in mat4 SRT;


uniform mat4 lightSpaceMatrix;

uniform mat4 model; // This is SRT from above

void main()
{
//    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    gl_Position = lightSpaceMatrix * SRT * vec4(aPos, 1.0);

}