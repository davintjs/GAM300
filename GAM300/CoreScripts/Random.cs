/*!***************************************************************************************
\file			Random.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			1/11/2022

\brief
	This file contains definitions for C# Random

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

using System;

namespace CopiumEngine
{

    public static class RNG
    {

        private static System.Random rnd = new System.Random();

        //Generates a random float within min and max 
        public static float Range (float min, float max)
        {
            return (float)((max - min) * rnd.NextDouble() + min);
        }

        //Generates a random int within min and max(inclusive) 
        public static int Range(int min, int max)
        {
            return rnd.Next(min, max + 1);
        }
    }
}
