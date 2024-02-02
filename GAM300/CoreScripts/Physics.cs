/*!***************************************************************************************
\file			Physics.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			03/04/2022

\brief
	This file contains definitions for Physics related features

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

using GlmSharp;
using System.Collections;
using System.Collections.Generic;


namespace BeanFactory
{
    public struct RayCastResult
    {
        GameObject obj;
        vec3 hitPosition;
        bool hit;
    }

    public class Physics
    {
        public RayCastResult Raycast(vec3 position, vec3 direction, float distance)
        {
            RayCastResult tmp = InternalCalls.Raycast(position, direction, distance);

            return tmp;
        }
    }



    public enum ForceMode2D
    {
        Impulse = 0,
    }
}
