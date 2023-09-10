#version 450 core
layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;

layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
layout (location = 4) in vec4 aVertexColor;

layout (location = 6) in mat4 SRT;

layout (location = 0) out vec4 vColor;

uniform mat4 persp_projection;
uniform mat4 View;
// uniform mat4 SRT;

void main()
{
	vColor = aVertexColor;
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);
} 