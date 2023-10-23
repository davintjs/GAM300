#version 450 core

//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
layout (location = 4) in vec4 aVertexColor; // This can throw

uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;

//-------------------------
//          GOING OUT
//-------------------------

layout (location = 0) out vec2 TexCoords;
layout (location = 1) out vec3 WorldPos;
layout (location = 2) out vec3 Normal;

void main()
{
	vec4 worldPos = SRT * vec4(aVertexPosition, 1.0f);
	gl_Position = persp_projection * View * worldPos;

    Normal = mat3(transpose(inverse(SRT))) * aVertexNormal;
}