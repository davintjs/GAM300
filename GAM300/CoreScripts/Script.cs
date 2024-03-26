/*!***************************************************************************************
\file			CopiumScript.cs
\project
\author			Zacharie Hong
\co-authors

\par			Course: GAM250
\par			Section:
\date			26/09/2022

\brief
    BaseClass for other C# scripts to inherit off

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

using System;
using GlmSharp;
using System.Collections;
using System.Collections.Generic;

namespace BeanFactory
{
    public class Script
    {
        UInt64 euid;
        UInt64 uuid;

        public string GetTag<T>(T component)
        {
            return InternalCalls.GetTag(component);
        }
        public virtual bool enabled
        {
            get
            {
                return InternalCalls.GetActive(this, GetType());
            }
            set
            {
                InternalCalls.SetActive(this, GetType(), value);
            }
        }
        private void Initialize(GameObject gameObj,UInt64 _euid, UInt64 _uuid)
        {
            _gameObject = gameObj;
            if (_gameObject == null)
                Console.WriteLine("Unable to find gameobject");
            euid = _euid;
            uuid = _uuid;
            InternalCalls.Get(gameObject,out _transform);
            if (_transform == null)
                Console.WriteLine("Unable to find transform");
        }
        public GameObject Instantiate(GameObject gameObject, vec3 pos, vec3 rot)
        {
            GameObject newGameObject;
            InternalCalls.CloneGameObject(gameObject, out newGameObject);
            newGameObject.transform.localPosition = pos;
            newGameObject.transform.localRotation = rot;
            return newGameObject;

        }

        public void Destroy(GameObject gameObject)
        {
            InternalCalls.DestroyGameObject(gameObject);
        }

        public static void Destroy<T>(T component) where T : class
        {
            InternalCalls.DestroyComponent(component,typeof(T));
        }

        public CharacterController charactercontroller
        {
            get
            {
                CharacterController result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }

        public bool HasComponent<T>()
        {
            bool output;
            InternalCalls.HasComponent(gameObject, typeof(T), out output);
            return output;
        }

        public T GetComponent<T>() where T : class
        {
            if (HasComponent<T>())
            {
                T component;
                InternalCalls.Get(gameObject, out component);
                return component;
            }
            Console.WriteLine("Component" + typeof(T) + "does not exist");
            return null;
        }

        public T AddComponent<T>() where T : class
        {
            return InternalCalls.AddComponent<T>(gameObject);
        }

        public Transform transform{ get{return _transform; } }
        public GameObject gameObject{ get { return _gameObject; }}

        private Transform _transform;
        private GameObject _gameObject;

        public Coroutine StartCoroutine(IEnumerator enumerator)
        {
            Coroutine coroutine = new Coroutine(enumerator);
            coroutinesBuffer.Add(coroutine);
            return coroutine;
        }

        public void StopCoroutine(Coroutine coroutine)
        {
            //WARN IF coroutine is null
            coroutines.Remove(coroutine);
            //ERROR IF not found
        }

        public void StopAllCoroutines()
        {
            coroutines.Clear();
            coroutinesBuffer.Clear();
        }

        public void ExecuteCoroutines()
        {
            foreach (Coroutine coroutine in coroutinesBuffer)
            {
                coroutines.Add(coroutine);
            }
            coroutinesBuffer.Clear();

            foreach (Coroutine coroutine in coroutines)
            {
                //Finished operation
                if (coroutine.MoveNext() == false)
                {
                    //Remove from list
                    endedCoroutines.Add(coroutine);
                }
            }
            foreach (Coroutine coroutine in endedCoroutines)
            {
                //Remove from list
                coroutines.Remove(coroutine);
            }
            endedCoroutines.Clear();
        }

        public static bool IsEnabled<T>(T component)
        {
            return InternalCalls.GetActive(component, typeof(T));
        }

        public static void SetEnabled<T>(T component, bool val = true)
        {
            InternalCalls.SetActive(component, typeof(T), val);
        }

        static List<Coroutine> endedCoroutines = new List<Coroutine>();
        List<Coroutine> coroutines = new List<Coroutine>();
        List<Coroutine> coroutinesBuffer = new List<Coroutine>();
    }

    public class InstanceData
    {
        public static void SaveData(string key, float value)
        {
            InternalCalls.SaveData(key, value);
        }

        public static float GetData(string key)
        {
            return InternalCalls.GetData(key);
        }
    }
}