/*!***************************************************************************************
\file			Math.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			1/11/2022

\brief
	This file contains definitions for C# Vector2 and Vector3

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

namespace CopiumEngine
{
    public struct Vector2
    {
        public float x, y;
        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2(float scalar)
        {
            x = scalar; y = scalar;
        }
        public Vector2(float _x, float _y)
        {
            x = _x; y = _y;
        }
        public static Vector2 operator *(Vector2 vector, float scalar)
        {
            return new Vector2(vector.x * scalar, vector.y * scalar);
        }

        public static implicit operator Vector3(Vector2 lhs) => new Vector3(lhs.x, lhs.y, 0);
    }
    
    public struct Vector3
    {
        public Vector2 vec2;
        public float x
        {
            get { return vec2.x; }
            set { vec2.x = value; }
        }
        public float y
        {
            get { return vec2.y; }
            set { vec2.y = value; }
        }

        public float z;
        public static Vector3 zero => new Vector3(0.0f);
        public static Vector3 one => new Vector3(1.0f);
        public Vector3(float scalar)
        {
            vec2.x = scalar; vec2.y = scalar; z = scalar;
        }
        public Vector3(Vector2 _vec2)
        {
            vec2.x = _vec2.x; vec2.y = _vec2.y; z = 0;
        }

        public Vector3(float _x = 0, float _y = 0, float _z = 0)
        {
            vec2.x = _x; vec2.y = _y; z = _z;
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3 operator *(Vector3 vector, float scalar)
        {
            return new Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            return a + (b - a) * t;
        }

        public static implicit operator Vector2(Vector3 lhs) => lhs.vec2;
    }

    public static class Mathf
    {
        public const float Infinity = float.PositiveInfinity;

        static public float Max(float lhs, float rhs)
        {
            return (lhs<rhs) ? rhs : lhs;
        }

        static public float Min(float lhs, float rhs)
        {
            return (lhs > rhs) ? rhs : lhs;
        }

        public static float Clamp(float value, float min, float max)
        {
            if (value < min)
                return min;
            if (value > max)
                return max;
            return value;
        }
        public static float Lerp(float a, float b, float t)
        {
            return a + (b - a) * t;
        }

        static public float SmoothDamp(float current, float target, ref float currentVelocity, float smoothTime, float maxSpeed = Infinity, float deltaTime = 0)
        {
            if (deltaTime == 0)
                deltaTime = Time.deltaTime;
            smoothTime = Mathf.Max(0.0001f, smoothTime);
            float num = 2f / smoothTime;
            float num2 = num * deltaTime;
            float num3 = 1f / (1f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
            float num4 = current - target;
            float num5 = target;
            float num6 = maxSpeed * smoothTime;
            num4 = Mathf.Clamp(num4, -num6, num6);
            target = current - num4;
            float num7 = (currentVelocity + num * num4) * deltaTime;
            currentVelocity = (currentVelocity - num * num7) * num3;
            float num8 = target + (num4 + num7) * num3;
            if (num5 - current > 0f == num8 > num5)
            {
                num8 = num5;
                currentVelocity = (num8 - num5) / deltaTime;
            }
            return num8;
        }
    }
}
