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

using System.Runtime.InteropServices;
using GlmSharp;
using System;


namespace BeanFactory
{
    public class Component
    {
        virtual public Transform transform 
        {
            get
            {
                Transform result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
        virtual public GameObject gameObject 
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            } 
        }
        virtual public CharacterController charactercontroller
        {
            get
            {
                CharacterController result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }

        virtual public bool HasComponent<T>()
        {
            GameObject gameObj;
            InternalCalls.Get(this,out gameObj);
            bool output;
            InternalCalls.HasComponent(gameObj, typeof(T), out output);
            return output;
        }

        virtual public T GetComponent<T>() where T : Component
        {
            T result;
            InternalCalls.Get(this, out result);
            return result;
        }

        virtual public T AddComponent<T>() where T : Component
        {
            return InternalCalls.AddComponent<T>(gameObject);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Rigidbody : PhysicsComponent
    {
        public vec3 linearVelocity;         //velocity of object
        public vec3 angularVelocity;              //acceleration of object
        public vec3 force;
        public float friction = 0.1f;                //friction of body (0 <= x <= 1)
        public float mass = 1f;                 //mass of object
        public bool isStatic = true;             //is object static? If true will override isKinematic!
        public bool isKinematic = true;          //is object simulated?
        public bool useGravity = true;           //is object affected by gravity?
        public bool is_trigger = false;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class CharacterController : PhysicsComponent
    {
        public vec3 velocity;
        public vec3 force;
        private vec3 direction;

        public float mass = 1f;
        public float friction = 0.1f;
        public float gravityFactor = 1f;
        public float slopeLimit = 45f;
        public bool isGrounded = false;


        public void Move(vec3 dir)
        {
            direction += dir;
        }

    }

    enum PhysicsBodyType : uint
    {
        RigidBody = 0, CharacterController
    };

    [StructLayout(LayoutKind.Sequential)]
    public class PhysicsComponent : Component
    {
        uint padding;
        uint type;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class AudioSource : Component
    {
        public void Play() { InternalCalls.AudioSourcePlay(this); }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Animator : Component
    {
        public void Play() { InternalCalls.PlayAnimation(this); }
        public void Pause() { InternalCalls.PauseAnimation(this); }
        public void Stop() { InternalCalls.StopAnimation(this); }
        public float GetProgress() { return InternalCalls.GetProgress(this); }
        public bool IsCurrentState(string state) { return InternalCalls.IsCurrentState(this, state); }
        public void SetDefaultState(string defaultState) { InternalCalls.SetDefaultState(this, defaultState); }
        public void SetState(string state) { InternalCalls.SetState(this, state); }
        public void SetNextState(string nextState) { InternalCalls.SetNextState(this, nextState); }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Transform : Component
    {
        public vec3 localPosition;
        public vec3 localRotation;
        public vec3 localScale;
        public vec3 forward
        {
            get
            {
                mat4 mat = glm.ToMat4(glm.FromEulerToQuat(localRotation));
                mat *= glm.Translate(new vec3(0,0,1));
                return glm.Normalized(glm.GetTranslation(mat));
            }
        }
        public vec3 back
        {
            get
            {
                return -forward;
            }
        }
        public vec3 right
        {
            get
            {
                mat4 mat = glm.ToMat4(glm.FromEulerToQuat(localRotation));
                mat *= glm.Translate(new vec3(1, 0, 0));
                return glm.Normalized(glm.GetTranslation(mat));
            }
        }

        public vec3 left
        {
            get
            {
                return -right;
            }
        }

        public vec3 up
        {
            get
            {
                mat4 mat = glm.ToMat4(glm.FromEulerToQuat(localRotation));
                mat *= glm.Translate(new vec3(0, 1, 0));
                return glm.Normalized(glm.GetTranslation(mat));
            }
        }

        public vec3 down
        {
            get
            {
                return -up;
            }
        }

        public void SetParent(Transform parent)
        {
            InternalCalls.SetTransformParent(this, parent);
        }
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

    public class Collider : Component
    {
        Rigidbody rigidbody;
    }
}