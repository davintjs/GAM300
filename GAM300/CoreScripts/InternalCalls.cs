/*!***************************************************************************************
\file			InternalCalls.cs
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
	This file encapsulates the InternalCalls of the Engine

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

using System;
using System.Reflection.Emit;
using System.Runtime.CompilerServices;
using GlmSharp;

namespace BeanFactory
{
    public static class InternalCalls
    {

        #region ANIMATION
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PauseAllAnimation();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PlayAllAnimation();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PlayAnimation(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PauseAnimation(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void StopAnimation(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetAnimatorDelay(ulong ID,float timeDelay);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetAnimatorDelay(ulong ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetFrame(ulong ID, int frame);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetProgress(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool IsCurrentState(Animator animator, string state);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetDefaultState(Animator animator, string defaultState);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetState(Animator animator, string state);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetNextState(Animator animator, string state);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetAnimationColor(ulong ID, out Color color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetAnimationColor(ulong ID, ref Color color);
        #endregion

        #region COMPONENT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool GetActive(Object obj, Type cType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetActive(Object obj, Type cType, bool val);

        #endregion

        #region GAMEOBJECT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool HasComponent(GameObject gameObject, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void CloneGameObject(GameObject gameObject, out GameObject newGameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static ulong InstantiateGameObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DestroyGameObject(GameObject gameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void DestroyComponent(Component component, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static T AddComponent<T>(GameObject gameObject, Type componentType);
        internal static T AddComponent<T>(GameObject gameObject) { return AddComponent<T>(gameObject, typeof(T));}

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Get<T>(Object owner, Type objType, out T newObject);

        public static void Get<T>(Object owner, out T newObject) { Get(owner,typeof(T), out newObject); }

        #endregion

        #region TRANSFORM


        #endregion

        #region SCENE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(string sceneName);
        #endregion

        #region AUDIO_SOURCE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourcePlay(AudioSource audioSource);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourceStop(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourceSetVolume(ulong ID, float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float AudioSourceGetVolume(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern UInt32 GetSoundLength(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetAllVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioMute(bool mute);
        #endregion	

        #region INPUT
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKey(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyDown(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKeyUp(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseDown(int mouseCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetMousePosition(out GlmSharp.vec2 pos);
        #endregion

        #region TEXT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void GetTextString(ulong compID,out string str);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetTextString(ulong compID,string str);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTextColor(ulong ID, out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetTextColor(ulong ID, ref Color color);
        #endregion

        #region IDENTIFIERS
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetLayer(string layerName);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetTag(Object owner);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetLayerName(int layer);
        #endregion

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFullscreenMode(bool fullscreenMode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitGame();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetFPS();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetGameNDC(out GlmSharp.vec2 pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetMouseDelta(out GlmSharp.vec2 pos);
    }
}
