/*!***************************************************************************************
\file			PBR.vert
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Vertex Shader for PBR

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#version 450 core
//-------------------------
//          COMING IN
//-------------------------


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangents;
layout (location = 3) in vec2 aTexCoords;
//layout (location = 4) in vec2 aColor;

layout (location = 6) in mat4 SRT;

layout (location = 10) in vec4 Albedo;
layout (location = 11) in vec4 Metal_Rough_AO_Emission_index; // Texture Index
layout (location = 12) in vec4 Metal_Rough_AO_Emission_constant; // Material Instance Constants
layout (location = 15) in vec2 texture_index;


//-------------------------
//          GOING OUT
//-------------------------
layout (location = 0) out vec2 TexCoords;
layout (location = 1) out vec3 WorldPos;
layout (location = 2) out vec3 Normal;

layout (location = 3) out vec4 frag_Albedo;
layout (location = 4) out vec4 frag_Metal_Rough_AO_Emission_index; // Texture Index
layout (location = 5) out vec4 frag_Metal_Rough_AO_Emission_constant; // Material Instance Constants

layout (location = 6) out vec2 frag_texture_index;

layout (location = 7) out vec4 frag_pos_lightspace;




//-------------------------
//          UNIFORMS
//-------------------------

uniform mat4 persp_projection;
uniform mat4 View;
uniform mat4 lightSpaceMatrix;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(SRT * vec4(aPos, 1.0));

    frag_Albedo = Albedo;
    frag_Metal_Rough_AO_Emission_index = Metal_Rough_AO_Emission_index;
    frag_Metal_Rough_AO_Emission_constant = Metal_Rough_AO_Emission_constant;
    frag_texture_index = texture_index;

    
    Normal = mat3(transpose(inverse(SRT))) * aNormal;

    frag_pos_lightspace = lightSpaceMatrix * vec4(WorldPos, 1.0);
    
    
    
    
    gl_Position =  persp_projection * View * vec4(WorldPos, 1.0);
}