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
using System.Diagnostics.Tracing;
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
        internal extern static void SetAnimatorDelay(ulong ID, float timeDelay);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetAnimatorDelay(ulong ID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetFrame(ulong ID, int frame);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetProgress(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetProgress(Animator animator, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static float GetSpeed(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetSpeed(Animator animator, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetDefaultState(Animator animator, string defaultState);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetState(Animator animator, string state);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetNextState(Animator animator, string state);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string GetState(Animator animator);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetAnimationColor(ulong ID, out vec4 color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetAnimationColor(ulong ID, ref vec4 color);
        #endregion

        #region COMPONENT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool GetActive(Object obj, Type cType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetActive(Object obj, Type cType, bool val);

        #endregion

        #region GAMEOBJECT
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool HasComponent(GameObject gameObject, Type componentType, out bool output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void CloneGameObject(GameObject gameObject, out GameObject newGameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static ulong InstantiateGameObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DestroyGameObject(GameObject gameObject);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void DestroyComponent(Object component, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static T AddComponent<T>(GameObject gameObject, Type componentType);
        internal static T AddComponent<T>(GameObject gameObject) { return AddComponent<T>(gameObject, typeof(T)); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Get<T>(Object owner, Type objType, out T newObject);

        public static void Get<T>(Object owner, out T newObject) { Get(owner, typeof(T), out newObject); }

        #endregion

        #region TRANSFORM
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTransformParent(Transform gameObject, Transform parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetChild(Transform owner, out Transform child);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetLocalPosition(Transform gameObject, out vec3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetLocalRotation(Transform gameObject, out vec3 rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetLocalScale(Transform gameObject, out vec3 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetWorldPosition(Transform gameObject, out vec3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetWorldRotation(Transform gameObject, out vec3 rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetWorldScale(Transform gameObject, out vec3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetLocalPosition(Transform gameObject, ref vec3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetLocalRotation(Transform gameObject, ref vec3 rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetLocalScale(Transform gameObject, ref vec3 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetWorldPosition(Transform gameObject, ref vec3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetWorldRotation(Transform gameObject, ref vec3 rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetWorldScale(Transform gameObject, ref vec3 scale);
        #endregion

        #region SCENE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadScene(string sceneName, bool loadDirect);
        #endregion

        #region AUDIO_SOURCE
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourcePlay(AudioSource audioSource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StopMusic(float fadetime);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PauseMusic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ResumeMusic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMusicFade(AudioSource audioSource, float fadeIn, float fadeOut);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void EnableSFX(bool toggle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMasterVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetMasterVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSFXVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetSFXVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMusicVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetMusicVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourceStop(AudioSource audioSource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSourceSetVolume(AudioSource audioSource, float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float AudioSourceGetVolume(AudioSource audioSource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern UInt32 GetSoundLength(AudioSource audioSource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetAllVolume(float volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioMute(bool mute);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PauseComponent(AudioSource audioSource);
        #endregion	

        #region INPUT_SYSTEM
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LockCursor(bool toggle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseHolding(int mouseCode);
        #endregion

        #region PHYSICS_SYSTEM
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Raycast(vec3 position, vec3 direction, float distance, ref bool hit, ref vec3 point, ref GameObject gameObj);
        #endregion

        #region TEXT
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string GetTextString(TextRenderer renderer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetTextString(TextRenderer renderer, string str);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTextColor(ulong ID, out vec4 color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void SetTextColor(ulong ID, ref vec4 color);
        #endregion

        #region IDENTIFIERS
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetLayer(string layerName);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetTag(Object owner);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetLayerName(int layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetName(GameObject gameObject);
        #endregion

        #region PARTICLES
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ParticlesPlayer(ParticleComponent particleComp);
        #endregion

        #region CAMERA
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetCameraTarget(Camera camera, ref vec3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRightVec(Camera camera, ref vec3 temp);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetUpVec(Camera camera, ref vec3 temp);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetForwardVec(Camera camera, ref vec3 temp);
        #endregion

        #region SPRITERENDERER
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsButtonClicked(SpriteRenderer spriteRenderer);

        #endregion

        #region MESH_RENDERER
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetMaterial(MeshRenderer meshRenderer, Material mat);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMaterial(MeshRenderer meshRenderer, Material mat);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMaterialRaw(MeshRenderer meshRenderer, Material mat);

        #endregion

        #region GRAPHICS
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetGamma();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetGamma(float gammaValue);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetBloom();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBloom(bool bloomValue);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetShadow();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetShadow(bool shadowValue);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetFullScreen();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFullscreenMode(bool fullscreenMode);

        #endregion

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SaveData(string key, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetData(string key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadNext();

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetScrollState();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void FindPath(NavMeshAgent pEnemy, vec3 pDest);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ResetPather(NavMeshAgent pEnemy);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsButtonHovered(SpriteRenderer spriteRenderer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTimeScale(float timescale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetTimeScale();
    }
}
