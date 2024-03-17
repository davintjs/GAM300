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


namespace BeanFactory
{
    public static class Time
    {
        //static float timeScale_ = 1f;
        static float deltaTime_ = 0f;
        public static float unscaledDeltaTime { get { return deltaTime_; } }
        public static float deltaTime { get { return deltaTime_ * timeScale; } }
        public static float timeScale { get { return InternalCalls.GetTimeScale(); } set { InternalCalls.SetTimeScale(value); } }
    }
}
