#version 450 core
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec3 FragmentPos;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec2 frag_TexCoord;

layout (location = 10) in vec4 frag_albedo;
layout (location = 11) in vec4 frag_specular;
layout (location = 12) in vec4 frag_diffuse;
layout (location = 13) in vec4 frag_ambient;
layout (location = 14) in float frag_shininess;
layout (location = 15) in float texture_index;


out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

layout (binding = 0) uniform sampler2D myTextureSampler[32];

void main()
{

    if(lightColor == vec3(0.f,0.f,0.f))
    {
        FragColor = vec4(vColor); // set all 4 vector values to 1.0
    }

    // Initial   
    //    FragColor = vec4(lightColor * objectColor, 1.0);

    // ambient lighting
//    float ambientStrength = 0.1;
    vec3 ambience = vec3(frag_ambient) * lightColor;

//    vec3 result = ambient * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffusion = lightColor * (diff * vec3(frag_diffuse));

//    vec3 result = (ambient + diffuse) * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // specular
    vec3 viewDir = normalize(camPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), frag_shininess); // look at basic lighting for the 32
    vec3 speculation = lightColor * (vec3(frag_specular) * spec) ;  
        
    vec3 result = (ambience + diffusion + speculation) * vec3(vColor);
    
    int index = int(texture_index - 0.5f);
    if (index < 32){
        result = (ambience + diffusion + speculation) * vec3(texture(myTextureSampler[index], frag_TexCoord));
    }else{
        result = (ambience + diffusion + speculation) * vec3(frag_albedo);
    }
    FragColor = vec4(result, 1.0);
}