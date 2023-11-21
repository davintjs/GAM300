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
    public class Script : Component
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

        public static void Destroy<T>(T component) where T : Component
        {
            InternalCalls.DestroyComponent(component,typeof(T));
        }

        override public Transform transform{ get{return _transform; } }
        override public GameObject gameObject{ get { return _gameObject; }}

        private Transform _transform;
        private GameObject _gameObject;

        override public T GetComponent<T>()
        {

            if (HasComponent<T>())
            {
                T component;
                InternalCalls.Get(_gameObject, out component);
                return component;
            }
            Console.WriteLine("Component does not exist");
            return null;
        }

        override public bool HasComponent<T>()
        {
            bool output;
            InternalCalls.HasComponent(_gameObject, typeof(T), out output);
            return output;
        }

        public Coroutine StartCoroutine(IEnumerator enumerator)
        {
            Coroutine coroutine = new Coroutine(enumerator);
            coroutines.Add(coroutine);
            return coroutine;
        }

        void StopCoroutine(Coroutine coroutine)
        {
            //WARN IF coroutine is null
            coroutines.Remove(coroutine);
            //ERROR IF not found
        }

        void ExecuteCoroutines()
        {
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

        static List<Coroutine> endedCoroutines = new List<Coroutine>();
        List<Coroutine> coroutines = new List<Coroutine>();
    }
}