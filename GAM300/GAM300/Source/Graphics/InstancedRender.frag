#version 450 core
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec3 FragmentPos;
layout (location = 2) in vec3 Normal;

out vec4 FragColor;



//in vec3 Normal;  
//in vec3 FragmentPos; 
//
// uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

uniform vec4 Specular;
uniform vec4 Diffuse;
uniform vec4 Ambient;
uniform float Shininess;
uniform vec4 Albedos;



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
    vec3 ambience = vec3(Ambient) * lightColor;

//    vec3 result = ambient * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffusion = lightColor * (diff * vec3(Diffuse));

//    vec3 result = (ambient + diffuse) * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // specular
    vec3 viewDir = normalize(camPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess); // look at basic lighting for the 32
    vec3 speculation = lightColor * (vec3(Specular) * spec) ;  
        
    vec3 result = (ambience + diffusion + speculation) * vec3(vColor);
    result = (ambience + diffusion + speculation) * vec3(Albedos);
    FragColor = vec4(result, 1.0);

}