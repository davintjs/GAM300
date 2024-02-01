/*!***************************************************************************************
****
\file			Graphics.cs
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
    Reflection for graphic types like colors

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using GlmSharp;

namespace GlmSharp
{
    public partial struct vec4
    {
        static public vec4 red = new vec4(1f, 0f, 0f, 1f);
        static public vec4 white = new vec4(1f, 1f, 1f, 1f);
        static public vec4 gray = new vec4(.5f, .5f, .5f, 1f);
        static public vec4 green = new vec4(0f, 1f, 0f, 1f);
    }
}


namespace BeanFactory
{
    public struct AssetID
    {
        Int64 id;
        Int64 id2;
    }

    public struct Texture
    {
        AssetID asset;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Material
    {
        private int shaderType;

        vec4 color;
        float metallic;
        float roughness;
        float ambientOcclusion;
        float emission;

        Texture albedoTexture;
        Texture normalMapTexture;
        Texture metallicTexture;
        Texture roughnessTexture;
        Texture ambientOcclusionTexture;
        Texture emissionTexture;
    }

}
