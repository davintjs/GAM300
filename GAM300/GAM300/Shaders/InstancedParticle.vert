#version 450 core

//-------------------------
//          COMING IN
//-------------------------
layout (location = 0) in vec3 aVertexPosition;
//layout (location = 1) in vec3 aVertexNormal;
//layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
// layout (location = 4) in vec4 aVertexColor; // This can throw
layout (location = 6) in mat4 SRT;

layout (location = 0) out vec2 texCoord;

uniform mat4 persp_projection;
uniform mat4 View;
uniform bool is2D;

void main()
{
//	if(is2D){
//		gl_Position = persp_projection * SRT * vec4(aVertexPosition, 1.0f);
//		return;
//	}
	gl_Position = persp_projection * View * SRT * vec4(aVertexPosition, 1.0f);
	texCoord = aVertexTexCoord;
}