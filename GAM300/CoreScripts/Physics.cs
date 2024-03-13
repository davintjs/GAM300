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
    public static class Physics
    {
        public static RaycastHit Raycast(vec3 position, vec3 direction, float distance)
        {
            RaycastHit hit = new RaycastHit();
            InternalCalls.Raycast(position, direction, distance, ref hit.hit, ref hit.point, ref hit.gameObj);
            return hit;
        }
    }

    public struct RaycastHit
    {
        public vec3 point;
        public bool hit;
        public GameObject gameObj;
    }

    public enum ForceMode2D
    {
        Impulse = 0,
    }
}
