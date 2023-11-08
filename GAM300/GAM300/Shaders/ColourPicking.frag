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

uniform sampler2D Texture;
uniform bool isTexture; // if there is a texture, this is true else false

// 0: ColourPicking for Mesh Renderer
// 1: ColourPicking for UI (Editor Mode)
// 2: ColourPicking for UI (Button Mode)
uniform int mode; 

uniform vec4 PickingColour;
//End

void main()
{    

    switch(mode)
    {

    
    case 0: // Mesh Renderer Picking
        FragColor = PickingColour;

    break;


    case 1: // UI (Editor Mode) Picking


    break;

    case 2:// UI (Button Mode) Picking
        if(isTexture)
        {
            vec4 colour = texture(Texture, TexCoords);
            if(colour.a <1.0)
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

    break;

    }

//    FragColor = vec4(spriteColor, 1.0) * texture(image, TexCoords);

    

//     FragColor = vec4(1.f,1.f,1.f,1.f);
}   