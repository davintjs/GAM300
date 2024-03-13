/*!***************************************************************************************
\file			HDR.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           28/09/2023

\brief
	Does Gamma Correction, and implements HDR if it's enabled

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0)uniform sampler2D hdrBuffer;
layout (binding = 1)uniform sampler2D bloomBlur;

uniform float bloomStrength;

void main()
{        
    vec3 color = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;   

    FragColor = vec4(mix(color, bloomColor, bloomStrength),1.0);

}


