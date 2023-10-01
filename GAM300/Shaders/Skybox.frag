/*!***************************************************************************************
\file			Skybox.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Fragment shader for skybox

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#version 450 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);

//    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));


//    float gamma = 2.2;
//    vec3 diffuseColor = pow(texture(skybox, TexCoords).rgb, vec3(gamma));
//    FragColor.rgb = diffuseColor;
}