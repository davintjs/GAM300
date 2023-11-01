#version 450 core

//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexNormal;
layout (location = 2) in vec3 aVertexTangent;
layout (location = 3) in vec2 aVertexTexCoord; //UVs info
layout (location = 4) in vec4 aVertexColor; // This can throw

layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;


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

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
//uniform bool isAnim;

//End

void main()
{
    TexCoords = aVertexTexCoord;
    
    //    WorldPos = vec3(SRT * vec4(aVertexPosition, 1.0f));
    //    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    //    {
    //        if(boneIds[i] == -1) 
    //            continue;
    //        if(boneIds[i] >=MAX_BONES) 
    //        {
    //            WorldPos = vec3(SRT * vec4(aPos, 1.0));
    //        }
    //        else
    //        {
    //            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f) * weights[i];
    //            WorldPos = vec3(SRT * localPosition);
    //        }
    //    }

    bool isAnim = true;
    if (isAnim)
    {
        vec4 totalPosition = vec4(0.0f);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >=MAX_BONES) 
            {
                totalPosition = vec4(aVertexPosition, 1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aVertexPosition,1.0f);
            totalPosition += localPosition * weights[i];
       }
	
	    WorldPos = vec3(SRT * totalPosition);
    }
    else
	    WorldPos = vec3(SRT * vec4(aVertexPosition, 1.0f));

	gl_Position = persp_projection * View * vec4(WorldPos,1.0);

    frag_pos_lightspace_D = lightSpaceMatrix_Directional * vec4(WorldPos, 1.0);

    frag_pos_lightspace_S = lightSpaceMatrix_Spot * vec4(WorldPos, 1.0);


    Normal = mat3(transpose(inverse(SRT))) * aVertexNormal;

}