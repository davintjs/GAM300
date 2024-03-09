#version 450 core

//-------------------------
//          COMING IN
//-------------------------
layout (location = 0) in vec3 aVertexPosition;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
// layout (location = 4) in vec4 aVertexColor; // This can throw
layout (location = 6) in mat4 SRT;

layout (location = 0) out vec2 texCoord;
layout (location = 1) out vec3 WorldPos;

uniform mat4 persp_projection;
uniform mat4 View;

void main()
{
	WorldPos = vec3(SRT * vec4(aVertexPosition, 1.0f));
	gl_Position = persp_projection * View * vec4(WorldPos,1.0);
	texCoord = aVertexTexCoord;
}