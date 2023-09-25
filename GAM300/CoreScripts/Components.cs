/*!***************************************************************************************
****
\file			Components.cs
\author			Zacharie Hong

\par			Course: GAM25s0
\par			Section:
\date			10/03/2023

\brief
    Components reflections to C++ components

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace BeanFactory
{
    public class Component
    {
        virtual public Transform transform 
        {
            get
            {
                return InternalCalls.Get<Transform>(this);
            }
        }
        virtual public GameObject gameObject 
        {
            get
            {
                return InternalCalls.Get<GameObject>(this);
            } 
        }
        public bool HasComponent<T>()
        {
            return gameObject.HasComponent<T>();
        }
        virtual public T GetComponent<T>() where T : Component
        {
            return InternalCalls.Get<T>(this);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Rigidbody : Component
    {
        public Vector3 linearVelocity;         //velocity of object
        public Vector3 angularVelocity;              //acceleration of object
        public Vector3 force;
        public float friction = 0.1f;                //friction of body (0 <= x <= 1)
        public float mass = 1f;                 //mass of object
        public bool isStatic = true;             //is object static? If true will override isKinematic!
        public bool isKinematic = true;          //is object simulated?
        public bool useGravity = true;           //is object affected by gravity?
        public bool is_trigger = false;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Transform : Component
    {
        public Vector3 localPosition;
        public Vector3 localRotation;
        public Vector3 localScale;
        public Transform parent;
    }


/*
    public class Camera : Component
    {
    }

    public class SpriteRenderer : Component
    {
        public Color color
        {
            get
            {
                InternalCalls.GetSpriteRendererColor(ID, out Color color);
                return color;
            }
            set
            {
                InternalCalls.SetSpriteRendererColor(ID, ref value);
            }
        }
    }

    public class BoxCollider2D : Component
    {
    }

    public class Button : Component
    {
        public ButtonState state
        {
            get { return (ButtonState)InternalCalls.GetButtonState(ID); }
        }
        public Color hoverColor
        {
            get
            {
                InternalCalls.GetButtonHoverColor(ID, out Color color);
                return color;
            }
            set
            {
                InternalCalls.SetButtonHoverColor(ID, ref value);
            }
        }
        public Color clickedColor
        {
            get
            {
                InternalCalls.GetButtonClickedColor(ID, out Color color);
                return color;
            }
            set
            {
                InternalCalls.SetButtonClickedColor(ID, ref value);
            }
        }

    }

    public class Text : Component
    {
        public string text
        {
            get
            {
                InternalCalls.GetTextString(ID,out string text);
                return text;
            }
            set
            {
                InternalCalls.SetTextString(ID, value);
            }
        }

        public Color color
        {
            get
            {
                InternalCalls.GetTextColor(ID, out Color color);
                return color;
            }
            set
            {
                InternalCalls.SetTextColor(ID, ref value);
            }
        }

    }

    public class AudioSource : Component
    {
        public void Play()
        {
            InternalCalls.AudioSourcePlay(ID);
        }
        public void Stop()
        {
            InternalCalls.AudioSourceStop(ID);
        }

        public float volume
        {
            get
            {
                return InternalCalls.AudioSourceGetVolume(ID);
            }
            set
            {
                InternalCalls.AudioSourceSetVolume(ID,value);
            }
        }
    }

    public class Image : Component
    {
        public Color color
        {
            get
            {
                InternalCalls.GetImageColor(ID, out Color color);
                return color;
            }
            set
            {
                InternalCalls.SetImageColor(ID, ref value);
            }
        }
    }

    public class SortingGroup : Component
    {

    }

    public class Animator : Component
    {
        public float delay
        {
            get
            {
                return InternalCalls.GetAnimatorDelay(ID);
            }
            set
            {
                InternalCalls.SetAnimatorDelay(ID,value);
            }
        }

        public bool play
        {
            set
            {
                if (value)
                    InternalCalls.PlayAnimation(ID);
                else
                    InternalCalls.PauseAnimation(ID);
            }
        }

        public Color color
        {
            get
            {
                InternalCalls.GetAnimationColor(ID, out Color color);
                return color;
            }
            set
            {
                InternalCalls.SetAnimationColor(ID, ref value);
            }
        }


        public void setFrame(int _frame)
        {

            InternalCalls.SetFrame(ID, _frame);
        }

        public void stop()
        {
            InternalCalls.StopAnimation(ID);
        }
    }*/
}