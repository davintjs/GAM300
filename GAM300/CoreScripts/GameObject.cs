/*!***************************************************************************************
\file			GameObject.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			1/11/2022

\brief
	This file encapsulates the GameObject of the Engine

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

using System.Collections.Generic;
using System;
using System.Collections;

namespace BeanFactory
{
    public class GameObject
    {
        public bool activeSelf
        {
            get
            {
                return InternalCalls.GetActive(this,typeof(GameObject));
            }
        }

        public string name
        {
            get
            {
                return "Yes";
            }
            set
            {

            }
        }

        //Checks if a gameObject has a component by calling back to c++
        public bool HasComponent<T>()
        {
            bool output;
            InternalCalls.HasComponent(this, typeof(T),out output);
            return output;
        }

        //Gets a component by calling back to c++
        public T GetComponent<T>() where T : class
        {
            if (HasComponent<T>())
            {
                T component;
                InternalCalls.Get(this, out component);
                return component;
            }
            Console.WriteLine("Component does not exist");
            return null;
        }

        public T AddComponent<T>() where T : class
        {
            
            return InternalCalls.AddComponent<T>(this);
        }

        public void SetActive(bool _active)
        {
            InternalCalls.SetActive(this, typeof(GameObject), _active);
        }

        public Transform transform
        {
            get
            {
                Transform component;
                InternalCalls.Get(this, out component);
                return component;
            }
        }
    }
}
