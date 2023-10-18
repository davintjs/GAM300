#version 450 core

#define MAX_POINT_LIGHT 20
#define MAX_SPOT_LIGHT 20
#define MAX_DIRECTION_LIGHT 5

struct PointLight
{
    vec3 position;
    vec3 colour;
    float intensity;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 colour;
    float intensity;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 colour;
    float innerCutOff;
    float outerCutOff;
    float intensity;
};

out vec4 FragColor;
  
layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 WorldPos;
layout (location = 2) in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

uniform vec4 Albedo;
uniform vec4 Specular;
uniform vec4 Diffuse;
uniform vec4 Ambient;

uniform float Shininess;
uniform float Metal;
uniform float Roughness;
uniform float Ao;

uniform int hasTexture;
uniform int hasNormal;
uniform int hasRoughness;
uniform int hasMetallic;
uniform int hasAO;
uniform int hasEmission;

uniform bool hdr;
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform int PointLight_Count;
uniform DirectionalLight directionalLights[MAX_DIRECTION_LIGHT];
uniform int DirectionalLight_Count;
uniform SpotLight spotLights[MAX_SPOT_LIGHT];
uniform int SpotLight_Count;

const float PI = 3.14159265359;

layout (binding = 0) uniform sampler2D AlbedoTexture;
layout (binding = 1) uniform sampler2D NormalMap;
layout (binding = 2) uniform sampler2D RoughnessMap;
layout (binding = 3) uniform sampler2D MetallicMap;
layout (binding = 4) uniform sampler2D AoMap;
layout (binding = 5) uniform sampler2D EmmisionMap;

vec3 getNormalFromMap()
{
    vec3 normal = texture(NormalMap, TexCoords).xyz;
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
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emission ={0.f,0.f,0.f};
    bool Am_Light = false;// this whole thing is temp

    // ALBEDO
    if (hasTexture != 0)
    {
        albedo = pow(texture(AlbedoTexture, TexCoords).rgb, vec3(2.2));
    }
    else
    {
        albedo = vec3(Albedo);
    }

    // METALLIC 
    if (hasMetallic != 0)
    {
        if(hasMetallic == hasRoughness)
        {
            metallic = texture(MetallicMap, TexCoords).b;   
        }
        else
            metallic = texture(MetallicMap, TexCoords).r;   
    }
    else
    {
        metallic = Metal;

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
    if (hasRoughness != 0)
    {
        if (hasMetallic == hasRoughness)
        {
            roughness = texture(RoughnessMap, TexCoords).g;   
        }
        else
            roughness = texture(RoughnessMap, TexCoords).r;    
    }
    else
    {
        roughness = Roughness;
        int rough_test = int(roughness-0.1f);
        if(rough_test == -1)
        {
            Am_Light = true;
        }
        else
            Am_Light = false;
    }
    // AO
    if (hasAO != 0)
    {
        ao  = texture(AoMap, TexCoords).r; 
    }
    else
    {
        ao = Ao;

        int ao_test = int(ao-0.1f);
        if(ao_test == -1)
        {
            Am_Light = true;
        }
        else
             Am_Light = false;
    }


    if (hasEmission != 0)
    {
        emission  = texture(EmmisionMap, TexCoords).xyz; 
    }


    
    if(Am_Light)
    {
        FragColor = vec4(Albedo.xyz,1.f);// CHANGE
        return;
    }

    vec3 N ;
    if (hasNormal != 0)
    {
        N = getNormalFromMap();
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
        vec3 lightColourStrength =  pointLights[i].colour * pointLights[i].intensity;
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColourStrength * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = totalPointCount * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        
        Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    float totalDirectionalCount = DirectionalLight_Count; // this is to use at the denominator which uses floats
    for(int i = 0; i < DirectionalLight_Count; ++i)
    {

        vec3 lightColourStrength =  directionalLights[i].colour * directionalLights[i].intensity;
        vec3 L = normalize(-directionalLights[i].direction);
        vec3 H = normalize(V + L);
        float distance = 1000.f;
        float attenuation = 1.0 / (distance * distance);
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
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        
        Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   

    float totalSpotLightCount = SpotLight_Count; // this is to use at the denominator which uses floats
    for(int i = 0; i < SpotLight_Count; ++i)// CHANGE WIP THE POSITION IS ALL FUCKED BECUASE ITS OFF THE CAM
    {
        float theta = dot(spotLights[i].position - WorldPos, normalize(-spotLights[i].direction)); 
        if(theta > spotLights[i].innerCutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
        {  
            vec3 lightColourStrength =  spotLights[i].colour * spotLights[i].intensity;
            vec3 L = normalize(-spotLights[i].direction);
            vec3 H = normalize(V + L);
            float distance = length(spotLights[i].position - WorldPos);

            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = lightColourStrength * attenuation;
            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);   
            float G   = GeometrySmith(N, V, L, roughness);      
            vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
            vec3 numerator    = NDF * G * F; 
            float denominator = totalSpotLightCount * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;	  

            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);        
            Lo += ( kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        }    
    }   

    vec3 ambient = vec3(0.03) * albedo * ao + emission;
    vec3 color = ambient + Lo;

    if(hdr)
        color = color / (color + vec3(1.0));

    FragColor = vec4(color, 1.0);
}  