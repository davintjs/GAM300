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
using System;
namespace GlmSharp
{
	public static partial class glm
	{
		public static quat FromEulerToQuat(vec3 eulerAngle)
		{
			quat q = new quat();
			vec3 c = Cos(eulerAngle * 0.5f);
			vec3 s = Sin(eulerAngle * 0.5f);
			q.w = c.x* c.y * c.z + s.x* s.y* s.z;
			q.x = s.x* c.y * c.z - c.x* s.y* s.z;
			q.y = c.x* s.y * c.z + s.x* c.y* s.z;
			q.z = c.x* c.y * s.z - s.x* s.y* c.z;
			return q;
		}

		public static mat4 Translate(vec3 position)
		{
			mat4 m = mat4.Identity;
			m.m30 = position.x;
			m.m31 = position.y;
			m.m32 = position.z;
			return m;
		}

		public static vec3 GetTranslation(mat4 m)
		{
			return new vec3(m.m30,m.m31,m.m32);
		}

	}


}

namespace BeanFactory
{
    public static class Mathf
    {
        public static float PI = 3.1415926f;
        public static float Rad2Deg = 180f / PI;
        public static float Abs(float value)
        {
            return value < 0 ? -value : value;
        }
        public static float Lerp(float start, float end, float percentage)
        {
            return start + (end - start) * percentage;
        }

        public static T Clamp<T>(T value, T min, T max) where T : IComparable<T>
        {
            if (value.CompareTo(min) < 0)
            {
                return min;
            }
            else if (value.CompareTo(max) > 0)
            {
                return max;
            }
            else
            {
                return value;
            }
        }

        public static float CalculateYAngle(float directionX, float directionZ)
        {
            // Calculate the angle in radians
            float radians = (float)Math.Atan2(directionZ, directionX);

            // Convert radians to degrees
            float degrees = radians * Rad2Deg;

            // Ensure the angle is in the range [0, 360)
            if (degrees < 0)
            {
                degrees += 360;
            }

            return degrees;
        }

        public static float SmoothDampAngle(float current, float target, ref float currentVelocity, float smoothTime, float maxSpeed)
        {
            // Calculate the delta time
            float deltaTime = Time.deltaTime;

            // Damping factor
            float omega = 2.0f / smoothTime;

            // Calculate the angular velocity
            float x = omega * deltaTime;
            float dampedFactor = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
            float deltaTheta = current - target;
            float theta = deltaTheta * dampedFactor;

            // Calculate the angular velocity
            currentVelocity = currentVelocity - omega * theta * deltaTime;
            current = target + (current - target) * (float)Math.Exp(-omega * deltaTime);

            // Clamp the velocity to avoid overshooting
            float maxDelta = maxSpeed * smoothTime;
            currentVelocity = Mathf.Clamp(currentVelocity, -maxDelta, maxDelta);

            // Return the smoothed angle
            return current;
        }
    }
}