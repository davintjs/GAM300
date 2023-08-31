#version 450 core
layout (location = 0) in vec3 aVertexPosition;

uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;

void main()
{
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);
} 