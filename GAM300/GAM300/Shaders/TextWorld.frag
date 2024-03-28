/*!***************************************************************************************
\file			TextWorld.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           05/11/2023

\brief
	Fragment Shader for Rendering UI onto WorldSpace

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#version 450 core

//-------------------------
//          COMING IN
//-------------------------

in vec2 TexCoords;

//-------------------------
//          GOING OUT
//-------------------------

out vec4 FragColor;

//-------------------------
//          UNIFORMS
//-------------------------

uniform sampler2D text;
uniform vec3 textColor;
uniform float AlphaScaler;

uniform float gammaCorrection;
//End

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    FragColor = vec4(textColor, AlphaScaler) * sampled;

    FragColor.rgb = pow(FragColor.rgb , vec3(gammaCorrection));




//	const float gamma = 2.2f;
//
//    //FragColor = vec4(spriteColor, 1.0) * texture(image, TexCoords);
//    vec4 colour;
//    if(RenderSprite)
//    {
//        colour = texture(Sprite, TexCoords);
//
//        if(RenderIcon)
//            colour *= uColor;
//
//        if(colour.a <0.05)
//        {
//            discard;
//        }
//        else
//        {
//            FragColor = colour;
//        }
//    }
//    else
//    {
//        FragColor = vec4(1.f,0.f,0.f,1.f);
//    }
//
//        FragColor.a *= AlphaScaler;
//        FragColor.rgb = pow(FragColor.rgb, vec3(gamma));



}   