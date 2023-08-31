#version 450 core

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragmentPos; 

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
    // Initial   
    //    FragColor = vec4(lightColor * objectColor, 1.0);

    
    // ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

//    vec3 result = ambient * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

//    vec3 result = (ambient + diffuse) * objectColor;
//    FragColor = vec4(result, 1.0);
//
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(camPos - FragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // look at basic lighting for the 32
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);

}