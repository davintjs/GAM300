#version 450 core

//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
layout (location = 4) in vec4 aVertexColor; // This can throw

//-------------------------
//          GOING OUT
//-------------------------

layout (location = 0) out vec3 FragmentPos;
layout (location = 1) out vec2 TexCoord;
layout (location = 2) out vec3 Normal;

uniform mat4 SRT;
uniform mat4 persp_projection;
uniform mat4 View;
//End
void main()
{
	vec4 worldPos = SRT * vec4(aVertexPosition, 1.0f);
	mat3 normalMatrix = transpose(inverse(mat3(SRT)));

	FragmentPos = worldPos.xyz;
	TexCoord = aVertexTexCoord;
	Normal = normalMatrix * aVertexNormal;

	gl_Position = persp_projection * View * worldPos;
} 