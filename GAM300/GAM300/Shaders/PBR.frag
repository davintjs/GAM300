/*!***************************************************************************************
\file			PBR.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Fragment Shader for PBR - Instancing

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core

//-------------------------
//          DECLARATIONS
//-------------------------
#define MAX_POINT_LIGHT 20
#define MAX_SPOT_LIGHT 20
#define MAX_DIRECTION_LIGHT 5

#define MAX_POINT_LIGHT_SHADOW 10
#define MAX_SPOT_LIGHT_SHADOW 10
#define MAX_DIRECTIONAL_LIGHT_SHADOW 10

#define DIRECTIONAL_SHADOW_INDEX_OFFSET 20
#define SPOT_SHADOW_INDEX_OFFSET 10

struct PointLight
{
    vec3 position;
    vec3 colour;
    float intensity;
    bool enableShadow;
};

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

struct DirectionalLight
{
    vec3 direction;
    vec3 colour;
    float intensity;
    mat4 lightSpaceMatrix;
    bool enableShadow;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 colour;
    float innerCutOff;
    float outerCutOff;
    float intensity;
    mat4 lightSpaceMatrix;
    bool enableShadow;
};

//-------------------------
//          COMING IN
//-------------------------

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;
layout (location = 2) in vec3 Normal;

layout (location = 3) in vec4 frag_Albedo;
layout (location = 4) in vec4 frag_Metal_Rough_AO_Emission_index;
layout (location = 5) in vec4 frag_Metal_Rough_AO_Emission_constant;
layout (location = 6) in vec3 frag_texture_index_isEmission;

//layout (location = 7) in vec4 frag_pos_lightspace_D;
//
//layout (location = 8) in vec4 frag_pos_lightspace_S;
//

//-------------------------
//          GOING OUT
//-------------------------

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Blooming;

//-------------------------
//          UNIFORMS
//-------------------------

// The last slot is used for shadows from Directional / SpotLight currently
// 0 -9 - Textures | 10 -19 ( Spot Shadows ) | 20 21 - (DirectionalShadows)
layout (binding = 0) uniform sampler2D myTextureSampler[22];
layout (binding = 22) uniform samplerCube PointShadows[10];
//layout (binding = 31) uniform samplerCube PointShadowBox;
uniform bool renderShadow;
uniform float farplane;

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

// Bloom
uniform float bloomThreshold;

// ambience value
uniform float ambience_multiplier;
uniform vec3 ambient_tint;

uniform float gammaCorrection;
//End

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

float GeometrySmith_New(vec3 N, vec3 L, float roughness)
{
    float NdotL = max(dot(N, L), 0.0);
    
    // GGX (Trowbridge-Reitz) distribution
    float k = (roughness * roughness) / 2.0;
    
    float GGXV = NdotL / (NdotL * (1.0 - k) + k);

    return GGXV;
}

float ShadowCalculation_Directional(vec4 fragPosLightSpace,vec3 Normal,vec3 lightDir,int index)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(myTextureSampler[index], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;  
    // check whether current frag pos is in shadow

    // Max is 0.05 , Min is 0.005 -> put min as 0.0005
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.00005);

    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 
//    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    vec2 texelSize = 1.0 / textureSize(myTextureSampler[index], 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(myTextureSampler[index], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

float ShadowCalculation_Spot(vec4 fragPosLightSpace,vec3 Normal,vec3 lightDir,int index)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(myTextureSampler[index], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;  
    // check whether current frag pos is in shadow

    // Max is 0.05 , Min is 0.005 -> put min as 0.0005
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.0005);

    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 
//    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    vec2 texelSize = 1.0 / textureSize(myTextureSampler[index], 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(myTextureSampler[index], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}


float ShadowCalculation_Point(vec3 lightpos,int index)
{

    vec3 fragToLight = WorldPos - lightpos;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(PointShadows[index],fragToLight).r; 
    // get depth of current fragment from light's perspective
    closestDepth *= farplane;  
    // check whether current frag pos is in shadow
    float currentDepth = length(fragToLight);
//    // Max is 0.05 , Min is 0.005 -> put min as 0.0005
//    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.0005);
//
//    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 
////    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
//    if(projCoords.z > 1.0)
//        shadow = 0.0;
//
//  float bias = 0.1; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(camPos - WorldPos);
//    float diskRadius = 0.05;
    float diskRadius = (1.0 + (viewDistance / farplane)) / 50.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(PointShadows[index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farplane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);  
//    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0; 
    return shadow;
}

void main()
{		
    vec3 color;
    int Tex_index = int(frag_texture_index_isEmission.x + 0.5f); // .x is texture
    int NM_index = int(frag_texture_index_isEmission.y + 0.5f);    // .y is normal map

    int Metallic_index = int(frag_Metal_Rough_AO_Emission_index.x + 0.01f); // .x is metallic texture
    int Roughness_index = int(frag_Metal_Rough_AO_Emission_index.y + 0.01f);    // .y is roughness texture
    int AO_index = int(frag_Metal_Rough_AO_Emission_index.z + 0.01f);    // .z is ao texture
    int Emission_index = int(frag_Metal_Rough_AO_Emission_index.w + 0.01f);    // .w is emission texture


    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emission ={0.f,0.f,0.f};
    bool Am_Light = false;// this whole thing is temp

    // ALBEDO
    if (Tex_index < 32)
    {
        albedo = vec3(frag_Albedo) * pow(texture(myTextureSampler[Tex_index], TexCoords).rgb, vec3(gammaCorrection));
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
            metallic = frag_Metal_Rough_AO_Emission_constant.r * texture(myTextureSampler[Metallic_index], TexCoords).b;   
        }
        else
            metallic = frag_Metal_Rough_AO_Emission_constant.r * texture(myTextureSampler[Metallic_index], TexCoords).r;   
    }
    else
    {
        metallic = frag_Metal_Rough_AO_Emission_constant.r;

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
            roughness = frag_Metal_Rough_AO_Emission_constant.g * texture(myTextureSampler[Roughness_index], TexCoords).g;   
        }
        else
            roughness = frag_Metal_Rough_AO_Emission_constant.g * texture(myTextureSampler[Roughness_index], TexCoords).r;    
    }
    else
    {
        roughness = frag_Metal_Rough_AO_Emission_constant.g;
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
        ao  = frag_Metal_Rough_AO_Emission_constant.b * texture(myTextureSampler[AO_index], TexCoords).r; 
    }
    else
    {
        ao = frag_Metal_Rough_AO_Emission_constant.b;

        int ao_test = int(ao-0.1f);
        if(ao_test == -1)
        {
            Am_Light = true;
        }
        else
             Am_Light = false;
    }

    bool hasEmissionMap = false;
    if (Emission_index < 32)
    {
        emission  = frag_Metal_Rough_AO_Emission_constant.w * texture(myTextureSampler[Emission_index], TexCoords).xyz; 
        if(emission != vec3(0.f,0.f,0.f))
            hasEmissionMap = true;

    }


    
    if(Am_Light)
    {
        FragColor = vec4(frag_Albedo.xyz,1.f);// CHANGE
        return;
    }
    
    bool toBloom = false;
    bool isEmission = (frag_texture_index_isEmission.z != 0);
    
    if( !hasEmissionMap)
    {
        if(isEmission)
        {
            toBloom = true;
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


        int PointShadowIndex = 0;
        float totalPointCount = PointLight_Count; // this is to use at the denominator which uses floats
        for(int i = 0; i < PointLight_Count; ++i)
        {
        
        /*
        vec3 lightColourStrength = pointLights[i].colour * pointLights[i].intensity;

        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColourStrength * attenuation;

        // Lambertian Reflection (Diffuse)
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = (albedo / PI) * radiance * NdotL;

        // No specular reflection in the simplified model

        // Apply shadow (if enabled)
        bool shadows = pointLights[i].enableShadow && renderShadow;
        float shadow = shadows ? ShadowCalculation_Point(pointLights[i].position, PointShadowIndex) : 0.0;

        Lo += diffuse * (1.0 - shadow);
        // If you want to include specular reflection in a non-PBR way, you can add it here
        // Lo += specular * (1.0 - shadow);

        if (shadows)
            ++PointShadowIndex; 
        */

//  vec3 lightColourStrength = pointLights[i].colour * pointLights[i].intensity;
//
//    // Calculate per-light radiance
//    vec3 L = normalize(pointLights[i].position - WorldPos);
//    vec3 V = normalize(L - WorldPos); // Use a virtual camera position relative to the light
//    vec3 H = normalize(V+L); // Use a virtual camera position relative to the light
//
//    float distance = length(pointLights[i].position - WorldPos);
//    float attenuation = 1.0 / (distance * distance);
//    vec3 radiance = lightColourStrength * attenuation;
//    float NdotV = max(dot(N, V), 0.0);
//    float NdotL = max(dot(N, L), 0.0);
//
//    // Cook-Torrance BRDF (simplified for non-camera-relative)
//    float NDF = DistributionGGX(N, H, roughness);
//    float G = GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
//    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
//
//    vec3 numerator = NDF * G * F;
//    float denominator = max(4.0 * NdotL * NdotV, 0.001); // Adjust for denominator to prevent division by zero
//    vec3 specular = numerator / denominator;
//
//    // Lambertian Reflection (Diffuse)
//    vec3 diffuse = (albedo / PI) * radiance * NdotL;
//
//    // Apply shadow (if enabled)
//    bool shadows = pointLights[i].enableShadow && renderShadow;
//    float shadow = shadows ? ShadowCalculation_Point(pointLights[i].position, PointShadowIndex) : 0.0;
//
//    Lo += (diffuse + specular) * (1.0 - shadow);
//
//    if (shadows)
//        ++PointShadowIndex;


        vec3 lightColourStrength = pointLights[i].colour * pointLights[i].intensity;

        // Calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 H = normalize(L);  // Use light direction as the halfway vector

        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColourStrength * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith_New(N, L, roughness);  // Simplify the geometry term
        vec3 F = vec3(0.04);  // Use a constant value for Fresnel (Schlick approximation)

        vec3 numerator = NDF * G * F;
        float denominator = max(4.0 * max(dot(N, L), 0.0), 0.001);  // Adjust for denominator
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
        bool shadows = pointLights[i].enableShadow && renderShadow;
        float shadow = shadows ? ShadowCalculation_Point(pointLights[i].position, PointShadowIndex) : 0.0;

        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);


        if (shadows)
        {
            ++PointShadowIndex;
            PointShadowIndex %= MAX_POINT_LIGHT_SHADOW;
        }



//            vec3 lightColourStrength =  pointLights[i].colour * pointLights[i].intensity;
//            // calculate per-light radiance
//            vec3 L = normalize(pointLights[i].position - WorldPos);
//            vec3 temp_V = abs(V);
////            vec3 H = normalize(V+L);
//            vec3 H = normalize(temp_V + L);
////            vec3 H = normalize(L);
//            float distance = length(pointLights[i].position - WorldPos);
//            
//            float attenuation = 1.0 / (distance * distance);
//    //        vec3 radiance = pointLights[i].colour * attenuation;
//            vec3 radiance = lightColourStrength * attenuation;
//
//            // Cook-Torrance BRDF
//            float NDF = DistributionGGX(N, H, roughness);   
//            float G   = GeometrySmith(N, temp_V, L, roughness);      
//            vec3 F    = fresnelSchlick(max(dot(H, temp_V), 0.0), F0);
//           
//            vec3 numerator    = NDF * G * F; 
//            float denominator = totalPointCount * max(dot(N, temp_V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
//            vec3 specular = numerator / denominator;
//        
//            // kS is equal to Fresnel
//            vec3 kS = F;
//            // for energy conservation, the diffuse and specular light can't
//            // be above 1.0 (unless the surface emits light); to preserve this
//            // relationship the diffuse component (kD) should equal 1.0 - kS.
//            vec3 kD = vec3(1.0) - kS;
//            // multiply kD by the inverse metalness such that only non-metals 
//            // have diffuse lighting, or a linear blend if partly metal (pure metals
//            // have no diffuse light).
//        
//            kD *= 1.0 - metallic;	  
//
//            // scale light by NdotL
//            float NdotL = max(dot(N, L), 0.0);        
//    //        Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
//    //        float shadow = ShadowCalculation_Point(pointLights[i].position); 
//            bool shadows = pointLights[i].enableShadow && renderShadow;
//            
//            float shadow = shadows ? ShadowCalculation_Point(pointLights[i].position,PointShadowIndex) : 0.0; // add a shadows bool
//            Lo += ( kD * albedo / PI + specular) * radiance *( NdotL * (1.f - shadow) );  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
////            Lo += ( kD * albedo / PI + specular)* radiance * NdotL;
////            Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.f - shadow);
//            // test to render the depth map
//
//    //        vec3 fragToLight = WorldPos - pointLights[i].position;
//    //
//    //        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    //        float closestDepth = texture(PointShadowBox,fragToLight).r; 
//    //        // get depth of current fragment from light's perspective
//    //        closestDepth *= farplane;  
//    //        // check whether current frag pos is in shadow
//    //        float currentDepth = length(fragToLight);
//    //        FragColor = vec4(vec3(closestDepth / farplane), 1.0);
//    //        return;
//
//            if(shadows)
//                ++PointShadowIndex;
//
        }   
   

        int DirectionalShadowIndex = 0;
        float totalDirectionalCount = DirectionalLight_Count; // this is to use at the denominator which uses floats
        for(int i = 0; i < DirectionalLight_Count; ++i)
        {
            vec4 frag_pos_lightspace_D = directionalLights[i].lightSpaceMatrix * vec4(WorldPos,1.0);

            
            vec3 lightColourStrength =  directionalLights[i].colour * directionalLights[i].intensity;

            // calculate per-light radiance
        
    //        vec3 L = normalize(pointLights[i].position - WorldPos);
            vec3 L = normalize(-directionalLights[i].direction);


            vec3 H = normalize(V + L);



    //        float distance = length(pointLights[i].position - WorldPos);
            float distance = 10.f;
        
        
            float attenuation = 1.0 / (distance * distance);
    //        vec3 radiance = directionalLights[i].colour * attenuation;
            vec3 radiance = lightColourStrength * attenuation;

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


            bool shadows = directionalLights[i].enableShadow && renderShadow;

    //        float shadow = ShadowCalculation(frag_pos_lightspace,N, -directionalLights[i].direction * distance); 
            float shadow = shadows ? ShadowCalculation_Directional(frag_pos_lightspace_D,N, 
            -directionalLights[i].direction * distance,DIRECTIONAL_SHADOW_INDEX_OFFSET + DirectionalShadowIndex) : 0.0; // add a shadows bool

        
        
            Lo += ( kD * albedo / PI + specular) * radiance * NdotL * (1.f - shadow);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
            
            if(shadows)
            {

                ++DirectionalShadowIndex;
                DirectionalShadowIndex %= MAX_DIRECTIONAL_LIGHT_SHADOW;

            }
        }   


        int SpotShadowIndex = 0;
        float totalSpotLightCount = SpotLight_Count; // this is to use at the denominator which uses floats 
        for (int i = 0; i < SpotLight_Count; ++i)// CHANGE WIP THE POSITION IS ALL FUCKED BECUASE ITS OFF THE CAM
        {
            vec3 L = normalize(spotLights[i].position - WorldPos);

            float theta  = dot(L, normalize(-spotLights[i].direction));
        
            if(theta > spotLights[i].outerCutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
            {  
                vec4 frag_pos_lightspace_S = spotLights[i].lightSpaceMatrix * vec4(WorldPos,1.0);

                float epsilon   = spotLights[i].innerCutOff - spotLights[i].outerCutOff;
                float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0); 

    //             vec3 lightColourStrength =  spotLights[i].colour * spotLights[i].intensity;
                intensity *= spotLights[i].intensity;
                vec3 lightColourStrength =  spotLights[i].colour * intensity;

            // calculate per-light radiance
        
    //        vec3 L = normalize(-spotLights[i].direction);

                vec3 H = normalize(V + L);
        
    //      float distance = length(pointLights[i].position - WorldPos);
        
                float distance = length(spotLights[i].position - WorldPos);


    //        float theta = dot(spotLights[i].position - WorldPos, normalize(-spotLights[i].direction)); 
    //        float theta = dot(camPos - WorldPos, normalize(-spotLights[i].direction)); 
    //
    //        float epsilon = (spotLights[i].innerCutOff - spotLights[i].outerCutOff);
    //        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
    //
    //        float attentuation = smoothstep(spotLights[i].outerCutOff,spotLights[i].innerCutOff,theta);
    //        vec3 radiance = spotLights[i].colour * vec3(intensity);
    //


                float attenuation = 1.0 / (distance * distance);

        //        vec3 radiance = spotLights[i].colour * attenuation;
                vec3 radiance = lightColourStrength * attenuation;
        
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
        
        
                bool shadows = spotLights[i].enableShadow && renderShadow;

        //        float shadow = ShadowCalculation(frag_pos_lightspace,N, spotLights[i].position - WorldPos); 
                float shadow = shadows ? ShadowCalculation_Spot(frag_pos_lightspace_S,N, spotLights[i].position - WorldPos,SPOT_SHADOW_INDEX_OFFSET + SpotShadowIndex) : 0.0; // add a shadows bool
        
                Lo += ( kD * albedo / PI + specular) * radiance * NdotL * (1.f - shadow);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
            
                if(shadows)
                {
                    ++SpotShadowIndex;
                    SpotShadowIndex %= MAX_SPOT_LIGHT_SHADOW;
                }

            }   

        }   


        vec3 ambient = vec3(ambience_multiplier) * albedo * ao + emission;
        color = ambient + Lo;
            
//        else
//        {
//         toBloom = true;
//         color = albedo;
//        }
    }
    else
    {
        toBloom = true;
         color = emission;
     }

//    color *= frag_Metal_Rough_AO_Emission_constant.w;
    // Done in Post Processing
//    // HDR tonemapping
//    color = color / (color + vec3(1.0));
//    // gamma correct
//    color = pow(color, vec3(1.0/2.2)); 
//    toBloom = false;

    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    brightness *= frag_Metal_Rough_AO_Emission_constant.w;

    if( (brightness > bloomThreshold) && toBloom)
        Blooming = vec4(color.rgb, 1.0);
    else
        Blooming = vec4(0.0, 0.0, 0.0, 1.0);


if(hdr)
    color = color / (color + vec3(1.0));

    if( !hasEmissionMap && !isEmission)
    {
        color *= ambient_tint;
    }
    FragColor = vec4(color, frag_Albedo.a);

//    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
//    if(brightness > 1.0)
//        Blooming = vec4(FragColor.rgb, 1.0);
//    else
//        Blooming = vec4(0.0, 0.0, 0.0, 1.0);
}