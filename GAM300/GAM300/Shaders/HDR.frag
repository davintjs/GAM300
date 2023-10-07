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

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    
    if(hdr)
    {
        // reinhard
        // vec3 result = hdrcolor / (hdrcolor + vec3(1.0));
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}