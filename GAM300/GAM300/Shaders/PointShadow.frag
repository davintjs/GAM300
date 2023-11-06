/*!***************************************************************************************
\file			PointShadow.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           05/11/2023

\brief
	Fragment Shader for Point Shadows

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

//End

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
} 