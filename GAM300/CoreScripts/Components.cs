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

namespace CopiumEngine
{
    public enum ButtonState 
    {
        OnHover,
		OnClick,
		OnHeld,
		OnRelease,
		None,
	};

    public class Component
    {
        public ulong ID;
        public GameObject gameObject;
        public Transform transform;
        public void Initialize (GameObject _gameObject, ulong _ID)
        {
            gameObject = _gameObject;
            transform = gameObject.transform;
            ID = _ID;
        }

        public bool enabled
        {
            get
            {
                return InternalCalls.GetComponentEnabled(ID);
            }
            set
            {
                Type componentType = GetType();
                InternalCalls.SetComponentEnabled(ID,value);
            }
        }

        void SetID(ulong _ID)
            { ID = _ID; }

        public T GetComponent<T>() where T : Component, new()
        {
            return gameObject.GetComponent<T>();
        }
    }

    public class Transform : Component
    {
        public Vector3 position
        {
            get
            {
                InternalCalls.GetTranslation(gameObject.ID, out Vector3 vec3);
                return vec3;
            }
            set
            {
                InternalCalls.SetTranslation(gameObject.ID, ref value);
            }
        }

        public Vector3 localScale
        {
            get
            {
                InternalCalls.GetLocalScale(gameObject.ID, out Vector3 vec3);
                return vec3;
            }
            set
            {
                InternalCalls.SetLocalScale(gameObject.ID, ref value);
            }
        }

        public Vector3 worldPosition
        {
            get
            {
                InternalCalls.GetGlobalPosition(gameObject.ID, out Vector3 vec3);
                return vec3;
            }
        }

        public Vector3 worldScale
        {
            get
            {
                InternalCalls.GetGlobalScale(gameObject.ID, out Vector3 vec3);
                return vec3;
            }
        }

        public Vector3 localPosition
        {
            get
            {
                InternalCalls.GetTranslation(gameObject.ID, out Vector3 vec3);
                return vec3;
            }
            set
            {
                InternalCalls.SetTranslation(gameObject.ID, ref value);
            }
        }

        public Vector3 localRotation 
        {
            get
            {
                InternalCalls.GetRotation(gameObject.ID, out Vector3 vec3);
                return vec3;
            }
            set
            {
                InternalCalls.SetRotation(gameObject.ID, ref value);
            }
        }

        public Transform parent
        {
            set
            {
                if (value == null)
                    InternalCalls.SetParent(0, gameObject.ID);
                else
                    InternalCalls.SetParent(value.gameObject.ID, gameObject.ID);
            }
        }
    }

    public class Rigidbody2D : Component
    {
        public Vector2 velocity
        {
            get
            {
                InternalCalls.RigidbodyGetVelocity(ID, out Vector2 vec2);
                return vec2;
            }
            set
            {
                InternalCalls.RigidbodySetVelocity(ID, ref value);
            }
        }

        public void AddForce(Vector2 force, ForceMode2D forceMode)
        {
            InternalCalls.RigidbodyAddForce(ID, ref force);
        }
    }

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
    }
}