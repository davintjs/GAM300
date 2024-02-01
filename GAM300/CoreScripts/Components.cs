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

using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using GlmSharp;
using System;


namespace BeanFactory
{
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

        new public GameObject gameObject
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
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
        new public GameObject gameObject
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
    }

    enum PhysicsBodyType : uint
    {
        RigidBody = 0, CharacterController
    };

    [StructLayout(LayoutKind.Sequential)]
    public class PhysicsComponent
    {
        uint padding;
        uint type;

        public GameObject gameObject
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class AudioSource
    {
        public void Play() { InternalCalls.AudioSourcePlay(this); }

        public GameObject gameObject
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class ParticleComponent
    {
        public void Play() { InternalCalls.ParticlesPlayer(this); }

        public GameObject gameObject
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Animator
    {
        public void Play() { InternalCalls.PlayAnimation(this); }
        public void Pause() { InternalCalls.PauseAnimation(this); }
        public void Stop() { InternalCalls.StopAnimation(this); }
        public float GetProgress() { return InternalCalls.GetProgress(this); }
        public void SetProgress(float value) { InternalCalls.SetProgress(this, value); }
        public float GetSpeed() { return InternalCalls.GetSpeed(this); }
        public void SetSpeed(float value) { InternalCalls.SetSpeed(this, value); }
        public void SetDefaultState(string defaultState) { InternalCalls.SetDefaultState(this, defaultState); }
        public void SetState(string state) { InternalCalls.SetState(this, state); }
        public string GetState() { return InternalCalls.GetState(this); }
        public void SetNextState(string nextState) { InternalCalls.SetNextState(this, nextState); }

        public GameObject gameObject
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            }
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Transform
    {
        public vec3 localPosition
        {
            get
            {
                vec3 outVec = new vec3();
                InternalCalls.GetLocalPosition(this, ref outVec);
                return outVec;
            }

            set
            {
                InternalCalls.SetLocalPosition(this, out value);
            }
        }

        public vec3 localRotation
        {
            get
            {
                vec3 outVec = new vec3();
                InternalCalls.GetLocalRotation(this, ref outVec);
                return outVec;
            }

            set
            {
                InternalCalls.SetLocalRotation(this, out value);
            }
        }

        public vec3 localScale
        {   
            get
            {
                vec3 outVec = new vec3();
                InternalCalls.GetLocalScale(this, ref outVec);
                return outVec;
            }

            set
            {
                InternalCalls.SetLocalScale(this, out value);
            }
        }

        public vec3 position
        {
            get
            {
                vec3 outVec = new vec3();
                InternalCalls.GetWorldPosition(this, ref outVec);
                return outVec;
            }
            set
            {
                InternalCalls.SetWorldPosition(this, out value);
            }
        }

        public vec3 rotation
        {
            get
            {
                vec3 outVec = new vec3();
                InternalCalls.GetWorldRotation(this, ref outVec);
                return outVec;
            }
            set
            {
                InternalCalls.SetWorldRotation(this, out value);
            }
        }

        public vec3 scale
        {
            get
            {
                vec3 outVec = new vec3();
                InternalCalls.GetWorldScale(this, ref outVec);
                return outVec;
            }
            set
            {
                InternalCalls.SetWorldScale(this, out value);
            }
        }

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

        public GameObject gameObject 
        {
            get
            {
                GameObject result;
                InternalCalls.Get(this, out result);
                return result;
            } 
        }

        public void SetParent(Transform parent)
        {
            InternalCalls.SetTransformParent(this, parent);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class SpriteRenderer
    {
        public bool IsButtonClicked()
        {
            return InternalCalls.IsButtonClicked(this);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public class NavMeshAgent
    {
        public void FindPath(vec3 pDest)
        {
            InternalCalls.FindPath(this, pDest);
        }
        public void ResetPath()
        {
            InternalCalls.ResetPather(this);
        }
        float mAgentSpeed;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class Camera
    {
        public void LookAt(GameObject gameObject)
        {
            vec3 pos = gameObject.transform.position;
            InternalCalls.SetCameraTarget(this, ref pos);
        }

        public void LookAt(vec3 position)
        {
            InternalCalls.SetCameraTarget(this, ref position);
        }

        public vec3 right
        {
            get
            {
                vec3 temp = new vec3();
                InternalCalls.GetRightVec(this, ref temp);
                return temp;
            }
        }

        public vec3 up
        {
            get
            {
                vec3 temp = new vec3();
                InternalCalls.GetUpVec(this, ref temp);
                return temp;
            }
        }

        public vec3 forward
        {
            get
            {
                vec3 temp = new vec3();
                InternalCalls.GetForwardVec(this, ref temp);
                return temp;
            }
        }

        public float distance
        {
            get
            {
                return lookatDistance;
            }

            set
            {
                lookatDistance = value;
            }
        }

        public vec3 targetPosition
        {
            get
            {
                return focalPoint;
            }
            set
            {
                focalPoint = value;
            }
        }

        public vec3 position
        {
            get
            {
                return cameraPosition;
            }
            set
            {
                cameraPosition = value;
            }
        }

        float padding;
        float padding1;
        vec4 backgroundColor;          // Default solid color when rendering
        vec3 cameraPosition;           // The location of the viewer / eye (Center of the screen, 10 units away)
        vec3 focalPoint;               // The look-at point / target point where the viewer is looking (Center of screen)
        vec2 dimension;                // The dimension of the camera in width and height defined in pixels
        
        int cameraType;              // Type of camera

        uint targetDisplay = 0;     // Target display for the camera

        float width = 0f;                  // Pixel width of the camera
        float height = 0f;                 // Pixel height of the camera
        float pitch = 0f;                  // For rotating about the x axis
        float yaw = 0f;                    // For rotating about the y axis
        float roll = 0f;                   // For rotating about the z axis
        float distanceCheck = 5f;          // The margin in which the camera will still render objects that are outside the camera's dimension

        float aspect = 0f;                  // The aspect ratio of the camera in width/height (Automatically calculated by screen's aspect ratio)
        float nearClip = 0f;                // Distance of near clipping plane from the camera
        float farClip = 0f;                 // Distance of far clipping plane from the camera
        public float fieldOfView = 0f;      // The vertical field of view in degrees
        public float lookatDistance = 0f;   // How close is the camera to the focal point
    }

    [StructLayout(LayoutKind.Sequential)]
    public class MeshRenderer
    {
        UInt32 VAO;
        UInt32 debugVAO;
        AssetID meshID;
        Int32 shaderType;
        AssetID materialID;

        public Material material
        {
            get 
            {
                Material mat = new Material();
                InternalCalls.GetMaterial(this, ref mat);
                mat.meshRenderer = this;
                return mat;
            }
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
}