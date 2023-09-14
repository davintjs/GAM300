/*!***************************************************************************************
\file			Time.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	Time related functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CopiumEngine
{
    public static class Time
    {
        static float timeScale_ = 1f;
        public static float deltaTime{ get { return InternalCalls.GetDeltaTime() * timeScale; } }
        public static float rawDeltaTime { get { return InternalCalls.GetDeltaTime(); } }
        public static float timeScale { get { return timeScale_; } set { timeScale_ = value; } }
    }
}
