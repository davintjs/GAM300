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

using System.Collections;
using System.Collections.Generic;

namespace CopiumEngine
{
    public struct Color
    {
        public float r, g, b;
        public float a;
        public Color(float _r,float _g, float _b, float _a = 1f)
        {
            r = _r;
            g = _g;
            b = _b;
            a = _a;
        }

        public static Color operator +(Color a, Color b)
        {
            return new Color(a.r + b.r, a.g + b.g, a.b + b.b, a.a+b.a);
        }

        public static Color operator -(Color a, Color b)
        {
            return new Color(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
        }

        public static Color operator *(Color color, float scalar)
        {
            return new Color(color.r * scalar, color.g * scalar, color.b * scalar, color.a * scalar);
        }

        public static Color Lerp(Color startColor, Color endColor, float t)
        {
            float r = startColor.r + t * (endColor.r - startColor.b);
            float g = startColor.g + t * (endColor.g - startColor.g);
            float b = startColor.b + t * (endColor.b - startColor.b);
            float a = startColor.a + t * (endColor.a - startColor.a);
            return new Color(r, g, b,a);
        }

        static public Color red = new Color(1f, 0f, 0f, 1f);
        static public Color white = new Color(1f, 1f, 1f, 1f);
        static public Color gray = new Color(.5f, .5f, .5f, 1f);
        static public Color green = new Color(0f, 1f, 0f, 1f);
    }
}
