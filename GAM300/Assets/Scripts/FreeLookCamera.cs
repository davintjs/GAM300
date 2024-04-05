using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Threading;

public class FreeLookCamera : Script
{
    public GameObject mainMenuObj;
    public GameObject htpObj;
    public GameObject settingsObj;
    public GameObject exitObj;
    public GameObject cameraObj;
    public GameObject playObj;
    public GameObject finalExitObj;

    private Camera camera;
    private float timer = 0f;
    private float duration = 1f;

    private Coroutine mainMenuCoroutine;
    private Coroutine htpCoroutine;
    private Coroutine settingCoroutine;
    private Coroutine exitCoroutine;

    void Awake()
    {
        camera = cameraObj.GetComponent<Camera>();
    }

    void Start()
    {

    }

    void Update()
    {

    }
    public void GoToPlay()
    {
        StopAllCoroutine();
        duration = 30f;
        StartCoroutine(Panning(playObj.transform));
    }

    public void GoToMainMenu()
    {
        StopAllCoroutine();
        mainMenuCoroutine = StartCoroutine(Panning(mainMenuObj.transform));
    }

    public void GoToHTP()
    {
        StopAllCoroutine();
        htpCoroutine = StartCoroutine(Panning(htpObj.transform));
    }

    public void GoToSettings()
    {
        StopAllCoroutine();
        settingCoroutine = StartCoroutine(Panning(settingsObj.transform));
    }

    public void GoToExit()
    {
        StopAllCoroutine();
        exitCoroutine = StartCoroutine(Panning(exitObj.transform));
    }

    public void GoToFinalExit()
    {
        StopAllCoroutine();
        duration = 40f;
        StartCoroutine(Panning(finalExitObj.transform));
    }

    void StopAllCoroutine()
    {
        StopCoroutine(mainMenuCoroutine);
        StopCoroutine(htpCoroutine);
        StopCoroutine(settingCoroutine);
        StopCoroutine(exitCoroutine);
    }

    IEnumerator Panning(Transform finalTransform)
    {
        Transform initialTransform = cameraObj.transform;
        
        //angle = angle * Mathf.Deg2Rad;

        timer = 0f;
        float t = 0f;
        while(timer < duration)
        {
            cameraObj.transform.position = vec3.Lerp(initialTransform.position, finalTransform.position, timer, duration, vec3.EasingType.BEZIER);
            //cameraObj.transform.rotation = vec3.Lerp(initialTransform.rotation, finalTransform.rotation, timer, duration, vec3.EasingType.BEZIER);
            //float yValue = 0f;
            //if(angle !=  0f)
            //{
            //    yValue = Mathf.Lerp(initialTransform.rotation.y, angle, timer, duration, Mathf.EasingType.BEZIER);
            //}
            //else
            //{
            //    yValue = Mathf.Lerp(initialTransform.rotation.y, finalTransform.rotation.y, timer, duration, Mathf.EasingType.BEZIER);
            //}

            t = timer / duration;
            t = t * t * (3.0f - 2.0f * t);
            quat oldQuat = glm.FromEulerToQuat(initialTransform.rotation).Normalized;
            quat newQuat = glm.FromEulerToQuat(finalTransform.rotation).Normalized;
            quat midQuat = quat.SLerp(oldQuat, newQuat, t);
            if (midQuat != quat.NaN)
            {
                camera.rotation = midQuat;
            }

            //vec3 rotation = cameraObj.transform.rotation;
            //rotation.y = yValue;
            //cameraObj.transform.rotation = rotation;

            timer += Time.deltaTime;
            yield return null;
        }
    }
}