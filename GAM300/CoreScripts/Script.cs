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


namespace BeanFactory
{
    public class Script
    {
        private void Initialize(GameObject gameObj)
        {
            _gameObject = gameObj;
            _transform = InternalCalls.GetTransformFromGameObject(gameObj);
        }
        public static T Instantiate<T>(T original, Vector3 pos, Vector3 rotation)
        {
            return original;
        }

        /*        public GameObject Instantiate(GameObject original)
                {
                    GameObject gameObject = new GameObject();
                    gameObject.ID = InternalCalls.CloneGameObject(original.ID);
                    return gameObject;
                }

                public GameObject Instantiate()
                {
                    GameObject gameObject = new GameObject();
                    gameObject.ID = InternalCalls.InstantiateGameObject();
                    return gameObject;
                }

                public static void Destroy(GameObject gameObj)
                {
                    InternalCalls.DestroyGameObject(gameObj.ID);
                }*/

        public Transform transform{ get{return _transform; } }
        public GameObject gameObject{ get { return _gameObject; }}

        private Transform _transform;
        private GameObject _gameObject;
    }
}