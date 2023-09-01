/*!***************************************************************************************
****
\file			Collision.cs
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			28/11/2022

\brief
    Defines a struct to store collision data

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

namespace CopiumEngine
{
    public struct Collision2D
    {
        /*******************************************************************************
        /*!
        * 
        \brief
            Constructor that takes in ID to know which gameObject was collided
        \param _gameObjectID
            ID of gameObject that was collided
        */
        /*******************************************************************************/
        Collision2D(ulong _gameObjectID)
        {
            gameObject = null;
/*            foreach (GameObject gameObj in Instances.gameObjects)
            {
                if (gameObj.ID == _gameObjectID)
                {
                    gameObject = gameObj;
                    break;
                }
            }*/
            if (gameObject == null)
            {
                gameObject = new GameObject();
            }
            //rigidbody = gameObject.GetComponent<Rigidbody2D>();
            transform = gameObject.transform;
        }
        public GameObject gameObject;
        public Transform transform;
        //public Rigidbody2D rigidbody;
    }
}
