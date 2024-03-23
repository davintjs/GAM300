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

    public void GoToMainMenu()
    {
        StopCoroutine(htpCoroutine);
        StopCoroutine(settingCoroutine);
        StopCoroutine(exitCoroutine);
        mainMenuCoroutine = StartCoroutine(Panning(mainMenuObj.transform));
    }

    public void GoToHTP()
    {
        StopCoroutine(mainMenuCoroutine);
        StopCoroutine(settingCoroutine);
        StopCoroutine(exitCoroutine);
        htpCoroutine = StartCoroutine(Panning(htpObj.transform));
    }

    public void GoToSettings()
    {
        StopCoroutine(mainMenuCoroutine);
        StopCoroutine(htpCoroutine);
        StopCoroutine(exitCoroutine);
        settingCoroutine = StartCoroutine(Panning(settingsObj.transform));
    }

    public void GoToExit()
    {
        StopCoroutine(mainMenuCoroutine);
        StopCoroutine(htpCoroutine);
        StopCoroutine(settingCoroutine);
        exitCoroutine = StartCoroutine(Panning(exitObj.transform, 180f));
    }

    IEnumerator Panning(Transform finalTransform, float angle = 0f)
    {
        Transform initialTransform = cameraObj.transform;
        
        angle = angle * Mathf.Deg2Rad;

        timer = 0f;
        while(timer < duration)
        {
            cameraObj.transform.position = vec3.Lerp(initialTransform.position, finalTransform.position, timer, duration, vec3.EasingType.BEZIER);
            //cameraObj.transform.rotation = vec3.Lerp(initialTransform.rotation, finalTransform.rotation, timer, duration, vec3.EasingType.BEZIER);
            float yValue = 0f;
            if(angle !=  0f)
            {
                yValue = Mathf.Lerp(initialTransform.rotation.y, angle, timer, duration, Mathf.EasingType.BEZIER);
            }
            else
            {
                yValue = Mathf.Lerp(initialTransform.rotation.y, finalTransform.rotation.y, timer, duration, Mathf.EasingType.BEZIER);
            }

            vec3 rotation = cameraObj.transform.rotation;
            rotation.y = yValue;
            cameraObj.transform.rotation = rotation;

            timer += Time.deltaTime;
            yield return null;
        }
    }
}