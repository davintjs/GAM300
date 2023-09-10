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


namespace BeanFactory
{
    public class GameObject
    {
        private UInt64 denseIndex; 
        public bool activeSelf
        {
            get
            {
                return InternalCalls.GetActive(this);
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

/*        //Checks if a gameObject has a component by calling back to c++
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.HasComponent(this, componentType);
        }

        //Gets a component by calling back to c++
        public T GetComponent<T>() where T : Component, new()
        {
            return InternalCalls.GetComponent(this, typeof(T)) as T;
        }*/
/*
        public T AddComponent<T>() where T : Component, new()
        {
            T component = new T() { gameObject = this };
            component.ID = InternalCalls.AddComponent(this, typeof(T));
            return component;
        }*/

        public void SetActive(bool _active)
        {
            InternalCalls.SetActive(this, _active);
        }

        public Transform transform
        {
            get
            {
                return InternalCalls.GetTransformFromGameObject(this);
            }
        }
    }
}
