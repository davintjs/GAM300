#version 450 core
layout (location = 0) in vec3 aVertexPosition;

layout (location = 6) in mat4 SRT;

uniform mat4 persp_projection;
uniform mat4 View;

void main()
{
	//vColor = aVertexColor;
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);

} 