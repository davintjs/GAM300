/*!***************************************************************************************
\file			Debug.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			1/11/2022

\brief
	This file encapsulates the debugging features of the Engine

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/
using System;

namespace CopiumEngine
{
    public static class Debug
    {

        /*******************************************************************************
        /*!
        *
            \brief
                Logs a message to Engine Console
        */
        /*******************************************************************************/
        public static void Log(string message)
        {
            InternalCalls.Log(message);
        }

        /*******************************************************************************
        /*!
        *
            \brief
                Logs a warning message to Engine Console
        */
        /*******************************************************************************/
        public static void LogWarning()
        {

        }

        /*******************************************************************************
        /*!
        *
            \brief
                Logs an error message to Engine Console and stops/prevents play mode
        */
        /*******************************************************************************/
        public static void LogError()
        {

        }
    }
}
