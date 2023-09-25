#version 450 core

// IN
// layout (location = 0) in vec4 vColor;
layout (location = 0) in vec2 frag_texture_index;
layout (location = 1) in vec3 FragmentPos;
layout (location = 2) in vec3 Normal;
//layout (location = 3) in vec2 frag_TexCoord;

layout (location = 4) in vec2 Tex_Coord;


layout (location = 10) in vec4 frag_albedo;
layout (location = 11) in vec4 frag_specular;
layout (location = 12) in vec4 frag_diffuse;
layout (location = 13) in vec4 frag_ambient;
layout (location = 14) in float frag_shininess;
//layout (location = 0) in vec2 texture_index;

layout (location = 5) in vec3 TangentLightPos;
layout (location = 15) in vec3 TangentViewPos;
layout (location = 16) in vec3 TangentFragPos;



//    vec3 TangentLightPos;
//    vec3 TangentViewPos;
//    vec3 TangentFragPos;

//layout (location = 15) in VS_OUT {
//    vec3 TangentLightPos;
//    vec3 TangentViewPos;
//    vec3 TangentFragPos;
//} fs_in;

// OUT



out vec4 FragColor;


uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;


//layout(binding=0) uniform sampler2D myTextureSampler;

//layout(binding=1) uniform sampler2D normalSampler;


layout (binding = 0) uniform sampler2D myTextureSampler[32];

void main()
{

    float gamma = 2.2;
    int Tex_index = int(frag_texture_index.x + 0.5f); // .x is texture
    int NM_index = int(frag_texture_index.y + 0.5f);    // .y is normal map
     FragColor = vec4(frag_albedo);

if(Tex_index<32)
{

    // COLOR CODE

    if(lightColor == vec3(0.f,0.f,0.f))
    {
        FragColor = vec4(frag_albedo); // set all 4 vector values to 1.0
    }

    vec3 colour = vec3(pow(texture(myTextureSampler[Tex_index], Tex_Coord).rgb, vec3(gamma)));

    // Initial   
    //    FragColor = vec4(lightColor * objectColor, 1.0);

    // ambient lighting
    // float ambientStrength = 0.1;
//   vec3 ambience = vec3(frag_ambient) * lightColor;
   vec3 ambience = vec3(frag_ambient) * colour;
//   vec3 ambience = 0.1f * ;
//    vec3 result = ambient * objectColor;
//    FragColor = vec4(result, 1.0);

    // diffuse 
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragmentPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffusion = (diff * colour) * vec3(frag_diffuse);
//
//    vec3 result = (ambient + diffuse) * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // specular
    vec3 viewDir = normalize(camPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);  

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), frag_shininess); // look at basic lighting for the 32

//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), frag_shininess); // look at basic lighting for the 32
    vec3 speculation = (vec3(frag_specular) * spec) ;  
        
    vec3 result = (ambience + diffusion + speculation) ;
//    result = (ambience + diffusion + speculation) * vec3(frag_albedo);
    FragColor = vec4(result, 1.0);
//    FragColor = vec4(lightDir, 1.0);


//


if(NM_index < 32)
{

 // NORMAL MAPPING

      // obtain normal from normal map in range [0,1]
     vec3 normal = texture(myTextureSampler[NM_index], Tex_Coord).rgb;
    
     // transform normal vector to range [-1,1]
     normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
 //    vec3 normal = normalize(Normal);

 //
     // get diffuse color

     vec3 color = pow(texture(myTextureSampler[Tex_index], Tex_Coord).rgb, vec3(gamma)); // Undoing Gamma... i think its kind of stupid actually
 //    vec3 color = texture(myTextureSampler, Tex_Coord).rgb;

     // ambient
//     vec3 ambient = 0.1 * color;
     vec3 ambient = vec3(frag_ambient) * color;
     // diffuse
     vec3 lightDir = normalize(-TangentLightPos + TangentFragPos);
//     vec3 tempdir = normalize(TangentLightPos - TangentFragPos);
 //    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
     float diff = max(dot(lightDir, normal), 0.0);
//     vec3 diffuse = lightColor * (diff *  vec3(frag_diffuse));
     vec3 diffuse =  (diff *  colour)  * vec3(frag_diffuse);
     // specular
     vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
 //    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
     vec3 reflectDir = reflect(-lightDir, normal);
     vec3 halfwayDir = normalize(lightDir + viewDir);  
     float spec = pow(max(dot(normal, halfwayDir), 0.0), frag_shininess);
//     float spec = pow(max(dot(normal, halfwayDir), 0.0), frag_shininess);
////     float spec = pow(max(dot(viewDir, reflectDir), 0.0), frag_shininess); // look at basic lighting for the 32

//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), frag_shininess); // look at basic lighting for the 32
//    vec3 speculation = lightColor * (vec3(frag_specular) * spec) ;  
//



     vec3 specular = vec3(frag_specular) * spec;
//     vec3 specular = vec3(frag_specular) * spec;
//    vec3 specular = (vec3(frag_specular) * spec) ;  

     FragColor = vec4(ambient + diffuse + specular, 1.0);
//     FragColor = vec4(lightDir, 1.0);
//     FragColor = vec4(ambient + diffuse + speculation, 1.0);

// FragColor = texture(myTextureSampler,Tex_Coord);


 }
 }

// FragColor = texture(myTextureSampler,Tex_Coord);


//
//// --------------------------------------------------
//// COMBINED NORMAL MAPPING & COLOUR
//// --------------------------------------------------
//
//// ----------------- COLOURS PORTION -----------------
//
//
//vec3 color;
//if( Tex_index < 32)
//{
//    color = pow(texture(myTextureSampler[Tex_index], Tex_Coord).rgb, vec3(gamma)); // Undoing Gamma... i think its kind of stupid actually
//        // Probably need to blend in the frag_albedo
//}
//else
//{
//// If no texture
//    color = vec3(frag_albedo);
//}
//
//
//
//// ----------------- NORMALS PORTION -----------------
//vec3 normal;
//if( NM_index < 32)
//{
//// If have Normal Map
//     // obtain normal from normal map in range [0,1]
//    normal = texture(myTextureSampler[NM_index], Tex_Coord).rgb;
//  
////    // transform normal vector to range [-1,1]
//    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
//}
//else
//{
//// No Normal Map
//    normal = normalize(Normal);
//
//}
//    vec3 ambience = vec3(frag_ambient) * lightColor;
//
//
//vec3 X_variable;
//vec3 Y_variable;
//vec3 camera_position;
//vec3 fragment_position;
//
//// If have normal map
//if( NM_index < 32)
//{
//    X_variable = -TangentLightPos;
//    Y_variable = -TangentFragPos;
//    camera_position = TangentViewPos;
//    fragment_position = TangentFragPos;
//}
//else// If dont have
//{
//    X_variable = lightPos;
//    Y_variable = FragmentPos;
//    camera_position = camPos;
//    fragment_position = FragmentPos;
//}
//
//// Universal
//vec3 lightDir = normalize(X_variable - Y_variable);
//
//float diff = max(dot(normal, lightDir), 0.0);
//
////vec3 diffusion = lightColor * (diff * vec3(frag_diffuse));// either 1
//vec3 diffusion = lightColor * diff * color;// either 1
////
//vec3 viewDir = normalize(camera_position - fragment_position);
////vec3 reflectDir = reflect(-lightDir, normal);
////
////float spec = pow(max(dot(viewDir, reflectDir), 0.0), frag_shininess); // no normal map
//////
//vec3 halfwayDir = normalize(lightDir + viewDir);  // Normal Map
//float spec = pow(max(dot(normal, halfwayDir), 0.0), frag_shininess); // Normal Map
////
////
////vec3 speculation = lightColor * (vec3(frag_specular) * spec) ;  // either 1
//vec3 speculation =  (vec3(frag_specular) * spec) ;  // either 1
////
//vec3 result = (ambience + diffusion + speculation) * vec3(color);
//
////FragColor = vec4(ambience + diffusion + speculation, 1.0);
//FragColor = vec4(result, 1.0);
////



// --------------------------------------------------
// HDR GONE WRONG
// --------------------------------------------------


//        vec3 color = pow(texture(myTextureSampler, Tex_Coord).rgb, vec3(gamma)); // Undoing Gamma... i think its kind of stupid actually
//        vec3 normal = texture(normalSampler, Tex_Coord).rgb;
//        // ambient
//        vec3 ambient = 0.0 * color;
//        // lighting
//        vec3 lighting = vec3(0.0);
//   
//        // diffuse
//        vec3 lightDir = normalize(FragmentPos - lightPos);
//        float diff = max(dot(lightDir, normal), 0.0);
//        vec3 diffuse = lightColor * diff * color;      
//        vec3 result = diffuse;        
//
//        // attenuation (use quadratic as we have gamma correction)
//        float distance = length(FragmentPos - lightPos);
//        result *= 1.0 / (distance * distance);
//        
//        
//        
//        lighting += result;
//                
//    FragColor = vec4(ambient + lighting, 1.0);
////

// Gamma Correction -> Currently this is moved to the post processing
//    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));

//    vec3 viewDir = normalize(camPos - FragmentPos);
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), frag_shininess); // look at basic lighting for the 32
//    vec3 speculation = lightColor * (vec3(frag_specular) * spec) ;  
//        
//    vec3 result = (ambience + diffusion + speculation) * vec3(frag_albedo);
//    
//    // int index = int(texture_index.x + 0.5f); // .x is texture
//    int index = int(texture_index.y + 0.5f);    // .y is normal map
//    if (index < 32){
//        result = (ambience + diffusion + speculation) * vec3(texture(myTextureSampler[index], frag_TexCoord));
//    }else{
//        result = (ambience + diffusion + speculation) * vec3(frag_albedo);
//    }
//    FragColor = vec4(result, 1.0);
//}
}