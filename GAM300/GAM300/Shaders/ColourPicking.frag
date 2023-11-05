/*!***************************************************************************************
\file			ColourPicking.frag
\project
\author         Euan Lim

\par			Course: GAM300
\date           05/11/2023

\brief
	Fragment Shader for ColourPicking

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
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

uniform sampler2D Sprite;
uniform bool RenderSprite; // if there is a texture, this is true else false

uniform vec4 PickingColour;
//End

void main()
{    


//    FragColor = vec4(spriteColor, 1.0) * texture(image, TexCoords);

    if(RenderSprite)
    {
        vec4  colour = texture(Sprite, TexCoords);
        if(colour.a <0.7)
        {
            discard;
        }
        else
        {
            FragColor = PickingColour;
        }


    }
    else
    {
        FragColor = PickingColour;
    }

//     FragColor = vec4(1.f,1.f,1.f,1.f);
}   