/*!***************************************************************************************
\file			Scene.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	Subscribes objects created from C++ into a C# scene

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

using System.Collections;
using System.Collections.Generic;
using System;

namespace CopiumEngine
{
    public static class SceneManager
    {
        public static void LoadScene(string sceneName)
        {
            InternalCalls.LoadScene(sceneName);
        }
    }
}