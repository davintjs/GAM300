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
using System.Runtime.CompilerServices;

namespace CopiumEngine
{
    public static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetDeltaTime();

        #region ANIMATION
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PauseAllAnimation();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PlayAllAnimation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PlayAnimation(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void PauseAnimation(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetAnimatorDelay(ulong ID,float timeDelay);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetAnimatorDelay(ulong ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetFrame(ulong ID, int frame);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void StopAnimation(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetAnimationColor(ulong ID, out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetAnimationColor(ulong ID, ref Color color);
        #endregion

        #region COMPONENT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool GetComponentEnabled(ulong compId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetComponentEnabled(ulong compId, bool enabled);

        #endregion

        #region GAMEOBJECT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool HasComponent(ulong ID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetActive(ulong ID, bool _active);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool GetActive(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static ulong CloneGameObject(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static ulong InstantiateGameObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void DestroyGameObject(ulong ID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static ulong AddComponent(ulong gameObjID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static Component GetComponent(ulong gameObjID, Type componentType);

        #endregion

        #region TRANSFORM

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void GetLocalScale(ulong ID, out Vector3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetLocalScale(ulong ID, ref Vector3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void GetTranslation(ulong ID, out Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetTranslation(ulong ID, ref Vector3 translation);


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void GetGlobalScale(ulong ID, out Vector3 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void GetGlobalPosition(ulong ID, out Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void GetRotation(ulong ID, out Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetRotation(ulong ID, ref Vector3 translation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetParent(ulong newParentID, ulong childID);
        #endregion

        #region RIGIDBODY2D

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyAddForce(ulong ID, ref Vector2 force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodyGetVelocity(ulong entIDityID, out Vector2 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void RigidbodySetVelocity(ulong ID, ref Vector2 velocity);
        #endregion

        #region SPRITE_RENDERER
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetSpriteRendererColor(ulong ID, out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetSpriteRendererColor(ulong ID, ref Color color);

        #endregion

        #region IMAGE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetImageColor(ulong ID, out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetImageColor(ulong ID, ref Color color);

        #endregion

        #region SCENE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(string sceneName);
        #endregion

        #region AUDIO_SOURCE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourcePlay(ulong ID);
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

        #region UI
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetUIInteractable(bool isInteractable);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetUIInteractable();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern char GetButtonState(ulong buttonID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetButtonHoverColor(ulong ID, out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetButtonHoverColor(ulong ID, ref Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetButtonClickedColor(ulong ID, out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetButtonClickedColor(ulong ID, ref Color color);
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
        public static extern void GetMousePosition(out Vector2 pos);
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFullscreenMode(bool fullscreenMode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetUICallback(string funcName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitGame();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetFPS();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetGameNDC(out Vector2 pos);
    }
}
