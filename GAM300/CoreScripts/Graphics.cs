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
    public struct Mesh
    {
        AssetID asset;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Material
    {
        Texture _albedoTexture;
        Texture _normalMapTexture;
        Texture _metallicTexture;
        Texture _roughnessTexture;
        Texture _ambientOcclusionTexture;
        Texture _emissionTexture;
        vec4 _color = new vec4(1,2,3,4);
        int shaderType = 1;
        float _metallic = 2;
        float _roughness =3 ;
        float _ambientOcclusion = 4;
        float _emission = 5;

        bool isEmission;
        bool isVariant;

        public vec4 color { get { return _color; } set { _color = value; PropertyChangedCallBack(); } }
        public float metallic { get { return _metallic; } set { _metallic = value; PropertyChangedCallBack(); } }
        public float roughness { get { return _roughness; } set { _roughness = value; PropertyChangedCallBack(); } }
        public float ambientOcclusion { get { return _ambientOcclusion; } set { _ambientOcclusion = value; PropertyChangedCallBack(); } }
        public float emission { get { return _emission; } set { _emission = value; PropertyChangedCallBack(); } }
        public Texture albedoTexture { get { return _albedoTexture; } set { _albedoTexture = value; PropertyChangedCallBack(); } }
        public Texture normalMapTexture { get { return _normalMapTexture; } set { _normalMapTexture = value; PropertyChangedCallBack(); } }
        public Texture metallicTexture { get { return _metallicTexture; } set { _metallicTexture = value; PropertyChangedCallBack(); } }
        public Texture roughnessTexture { get { return _roughnessTexture; } set { _roughnessTexture = value; PropertyChangedCallBack(); } }
        public Texture ambientOcclusionTexture { get { return _ambientOcclusionTexture; } set { _ambientOcclusionTexture = value; PropertyChangedCallBack(); } }
        public Texture emissionTexture { get { return _emissionTexture; } set { _emissionTexture = value; PropertyChangedCallBack(); } }

        public MeshRenderer meshRenderer { get { return _meshRenderer; } set { _meshRenderer = value; PropertyChangedCallBack(); } }

        public MeshRenderer _meshRenderer;

        void PropertyChangedCallBack()
        {
            isVariant = true;
            InternalCalls.SetMaterial(meshRenderer, this);
        }
    }

}
