/*!***************************************************************************************
\file			PBR.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Fragment Shader for PBR

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#version 450 core

//-------------------------
//          DECLARATIONS
//-------------------------
#define MAX_POINT_LIGHT 20
#define MAX_SPOT_LIGHT 20
#define MAX_DIRECTION_LIGHT 5

struct PointLight
{
    vec3 position;
    vec3 colour;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 colour;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 colour;
    float innerCutOff;
    float outerCutOff;
};

//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;
layout (location = 2) in vec3 Normal;

layout (location = 3) in vec4 frag_Albedo;
layout (location = 4) in vec4 frag_Metal_Rough_AO_index;
layout (location = 5) in vec3 frag_Metal_Rough_AO_constant;
layout (location = 6) in vec2 frag_texture_index;

//-------------------------
//          GOING OUT
//-------------------------

out vec4 FragColor;

//-------------------------
//          UNIFORMS
//-------------------------

layout (binding = 0) uniform sampler2D myTextureSampler[32];
//layout(std140, binding = 1) uniform PointLightBuffer {
//    PointLight pointLights[NR_POINT_LIGHTS];
//};
//uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform vec3 camPos;
uniform bool hdr;

// Point Light 
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform int PointLight_Count;

// Directional Light 
uniform DirectionalLight directionalLights[MAX_DIRECTION_LIGHT];
uniform int DirectionalLight_Count;

// Spot Light 
uniform SpotLight spotLights[MAX_SPOT_LIGHT];
uniform int SpotLight_Count;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------

vec3 getNormalFromMap(int NM_index)
{
    vec3 normal = texture(myTextureSampler[NM_index], TexCoords).xyz;
    normal.z = normal.z == 0 ? 1 : normal.z;

    vec3 tangentNormal = (normal * 2.0) - 1.0;

     // transform normal vector to range [-1,1]

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);


    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------


void main()
{		

    int Tex_index = int(frag_texture_index.x + 0.5f); // .x is texture
    int NM_index = int(frag_texture_index.y + 0.5f);    // .y is normal map

    int Metallic_index = int(frag_Metal_Rough_AO_index.x + 0.01f); // .x is metallic texture
    int Roughness_index = int(frag_Metal_Rough_AO_index.y + 0.01f);    // .y is roughness texture
    int AO_index = int(frag_Metal_Rough_AO_index.z + 0.01f);    // .z is ao texture
    int Emission_index = int(frag_Metal_Rough_AO_index.w + 0.01f);    // .w is emission texture


    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emission ={0.f,0.f,0.f};
    bool Am_Light = false;// this whole thing is temp

    // ALBEDO
    if (Tex_index < 32)
    {
        albedo = pow(texture(myTextureSampler[Tex_index], TexCoords).rgb, vec3(2.2));
    }
    else
    {
        albedo = vec3(frag_Albedo);
    }

    // METALLIC 
    if (Metallic_index < 32)
    {
        if(Metallic_index == Roughness_index)
        {
            metallic = texture(myTextureSampler[Metallic_index], TexCoords).b;   
        }
        else
            metallic = texture(myTextureSampler[Metallic_index], TexCoords).r;   
    }
    else
    {
        metallic = frag_Metal_Rough_AO_constant.r;

        int metal_test = int(metallic-0.1f);
        if(metal_test == -1)
        {
//                FragColor = vec4(1.f,1.f,1.f,1.f);
//                return;
//
            Am_Light = true;
        }
        else
             Am_Light = false;

    }
    // ROUGHNESS
    if (Roughness_index < 32)
    {
        if (Metallic_index == Roughness_index)
        {
            roughness = texture(myTextureSampler[Roughness_index], TexCoords).g;   
        }
        else
            roughness = texture(myTextureSampler[Roughness_index], TexCoords).r;    
    }
    else
    {
        roughness = frag_Metal_Rough_AO_constant.g;
        int rough_test = int(roughness-0.1f);
        if(rough_test == -1)
        {
            Am_Light = true;
        }
        else
             Am_Light = false;
    }
    // AO
    if (AO_index < 32)
    {
        ao  = texture(myTextureSampler[AO_index], TexCoords).r; 
    }
    else
    {
        ao = frag_Metal_Rough_AO_constant.b;

        int ao_test = int(ao-0.1f);
        if(ao_test == -1)
        {
            Am_Light = true;
        }
        else
             Am_Light = false;
    }


    if (Emission_index < 32)
    {
        emission  = texture(myTextureSampler[Emission_index], TexCoords).xyz; 
    }


    
    if(Am_Light)
    {
        FragColor = vec4(frag_Albedo.xyz,1.f);// CHANGE
        return;
    }

    vec3 N ;
    if (NM_index < 32)
    {
        N = getNormalFromMap(NM_index);
    }
    else
    {
        N = normalize(Normal);
    }

    
   

    vec3 V = normalize(camPos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);



    float totalPointCount = PointLight_Count; // this is to use at the denominator which uses floats
    for(int i = 0; i < PointLight_Count; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].colour * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = totalPointCount * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        
        Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
   


    float totalDirectionalCount = DirectionalLight_Count; // this is to use at the denominator which uses floats
    for(int i = 0; i < DirectionalLight_Count; ++i)
    {
        // calculate per-light radiance
        
//        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 L = normalize(-directionalLights[i].direction);


        vec3 H = normalize(V + L);



//        float distance = length(pointLights[i].position - WorldPos);
        float distance = 1000.f;
        
        
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = directionalLights[i].colour * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = totalDirectionalCount * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        
        Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   

    float totalSpotLightCount = SpotLight_Count; // this is to use at the denominator which uses floats
    for(int i = 0; i < SpotLight_Count; ++i)// CHANGE WIP THE POSITION IS ALL FUCKED BECUASE ITS OFF THE CAM
    {
        float theta = dot(camPos - WorldPos, normalize(-spotLights[i].direction)); 
    
        if(theta > spotLights[i].innerCutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
        {  

        // calculate per-light radiance
        
//        vec3 L = normalize(spotLights[i].position - WorldPos);
        vec3 L = normalize(-spotLights[i].direction);


        vec3 H = normalize(V + L);



//        float distance = length(pointLights[i].position - WorldPos);
        float distance = length(camPos - WorldPos);


//        float theta = dot(spotLights[i].position - WorldPos, normalize(-spotLights[i].direction)); 
//        float theta = dot(camPos - WorldPos, normalize(-spotLights[i].direction)); 
//
//
//
//        float epsilon = (spotLights[i].innerCutOff - spotLights[i].outerCutOff);
//        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
//
//        float attentuation = smoothstep(spotLights[i].outerCutOff,spotLights[i].innerCutOff,theta);
//        vec3 radiance = spotLights[i].colour * vec3(intensity);
//


        float attenuation = 1.0 / (distance * distance);

        vec3 radiance = spotLights[i].colour * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = totalSpotLightCount * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        
        Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        }    
    }   





    vec3 ambient = vec3(0.03) * albedo * ao + emission;
    
    vec3 color = ambient + Lo;

    // Done in Post Processing
//    // HDR tonemapping
//    color = color / (color + vec3(1.0));
//    // gamma correct
//    color = pow(color, vec3(1.0/2.2)); 


if(hdr)
    color = color / (color + vec3(1.0));

    FragColor = vec4(color, 1.0);
}