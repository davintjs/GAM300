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

layout (location = 0) out vec2 TexCoords;
layout (location = 1) out vec3 WorldPos;
layout (location = 2) out vec3 Normal;
layout (location = 3) out vec4 frag_pos_lightspace_D;
layout (location = 4) out vec4 frag_pos_lightspace_S;

//-------------------------
//          UNIFORMS
//-------------------------


uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 SRT;

uniform mat4 lightSpaceMatrix_Directional;
uniform mat4 lightSpaceMatrix_Spot;

void main()
{
    TexCoords = aVertexTexCoord;

	WorldPos = vec3(SRT * vec4(aVertexPosition, 1.0f));
	gl_Position = persp_projection * View * vec4(WorldPos,1.0);

    frag_pos_lightspace_D = lightSpaceMatrix_Directional * vec4(WorldPos, 1.0);

    frag_pos_lightspace_S = lightSpaceMatrix_Spot * vec4(WorldPos, 1.0);



    Normal = mat3(transpose(inverse(SRT))) * aVertexNormal;
}