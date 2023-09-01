/*!***************************************************************************************
\file			Input.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			1/11/2022

\brief
	This file encapsulates the Input of the Engine

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

namespace CopiumEngine
{
    public static class Input
    {
        /*******************************************************************************
        /*!
        *
            \brief
                Checks if a key is held

            \param keyCode
                KeyCode to listen for

            \return
                True if key was held
        */
        /*******************************************************************************/
        public static bool GetKey(KeyCode keyCode) { return InternalCalls.GetKey((int)keyCode); }

        /*******************************************************************************
        /*!
        *
            \brief
                Checks if a key just pressed

            \param keyCode
                KeyCode to listen for

            \return
                True if key was just pressed
        */
        /*******************************************************************************/
        public static bool GetKeyDown(KeyCode keyCode) { return InternalCalls.GetKeyDown((int)keyCode); }

        /*******************************************************************************
        /*!
        *
            \brief
                Checks if a key just released

            \param keyCode
                KeyCode to listen for

            \return
                True if key was just released
        */
        /*******************************************************************************/
        public static bool GetKeyUp(KeyCode keyCode) { return InternalCalls.GetKeyUp((int)keyCode); }

        public static bool GetMouseDown(int mouseCode) { return InternalCalls.GetMouseDown(mouseCode); }

        public static Vector2 GetMousePosition() 
        { 
            InternalCalls.GetMousePosition(out Vector2 pos);
            return pos;
        }

        public static Vector2 GetGameNDC()
        {
            InternalCalls.GetGameNDC(out Vector2 pos);
            return pos;
        }
    }
}
