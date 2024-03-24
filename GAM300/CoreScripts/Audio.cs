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

namespace BeanFactory
{
    static class Audio
    {
        public static float masterVolume
        {
            get
            {
                return InternalCalls.GetMasterVolume();
            }
            set
            {
                InternalCalls.SetMasterVolume(value);
            }
        }

        public static float sfxVolume
        {
            get
            {
                return InternalCalls.GetSFXVolume();
            }
            set
            {
                InternalCalls.SetSFXVolume(value);
            }
        }

        public static float musicVolume
        {
            get
            {
                return InternalCalls.GetMusicVolume();
            }
            set
            {
                InternalCalls.SetMusicVolume(value);
            }
        }
    }
}
