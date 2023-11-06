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
