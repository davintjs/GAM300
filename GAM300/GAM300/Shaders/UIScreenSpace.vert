#version 450 core


//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

//-------------------------
//          GOING OUT
//-------------------------

out vec2 TexCoords;

//-------------------------
//          UNIFORMS
//-------------------------

uniform mat4 SRT;
uniform mat4 projection;

//End

void main()
{
    TexCoords = aTexCoords;

   gl_Position = projection * SRT * vec4(aPos, 1.0);
}