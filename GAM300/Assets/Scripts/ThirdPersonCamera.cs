using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Runtime.InteropServices;
using System.Diagnostics.Tracing;

public class ThirdPersonCamera : Script
{
    public static ThirdPersonCamera instance;
    public Camera camera;

    public float yawRotSpeed = 1f;
    public float pitchRotSpeed = 1f;
    public float maxPitchAngle = 170f;
    public float minPitchAngle = -85f;
    public bool invertPitch = false;

    public float yawAngle;
    public float pitchAngle;
    private float setYawAngle;
    private const float yawSM = 1800f;
    private const float pitchSM = 1800f;

    public GameObject target;

    public float zoom = 1f;
    private float finalZoom = 0f;
    private float zoomTimer = 0f;
    public float defaultZoom = 5f;
    public float zoomSpeed = 50f;
    public float zoomInSpeed = 3f;
    public float closestZoom = 1f;
    public float furthestZoom = 10f;
    public bool isZooming = false;
    private bool zoomReset = false;
    private bool settingYaw = false;

    public float timer = 0f;
    //private float duration = 1.0f;
    //private float bufferTimer = 0f;
    //private float bufferDuration = 3.0f;
    //private float distance = 0f;

    public bool cutscene = false;

    float shakeMagnitude = 0f;
    float shakeDuration = 0f;

    void Awake()
    {
        //targetPosition = target.transform.localPosition;
        camera = GetComponent<Camera>();
        instance = this;
    }

    void Update()
    {
        if (cutscene)
        {
            return;
        }
        if (GameManager.instance.paused)
            return;
        UpdateCameraRotation();
    }

    // Update is called once per frame
    void LateUpdate()
    {
        if (cutscene)
        {
            return;
        }

        FocusOnTarget();

        if (GameManager.instance.paused)
            return;

        Zoom();

        AvoidColliders();

        ShakeCoroutine();
    }

    void OnTriggerEnter(PhysicsComponent other)
    {
        //Console.WriteLine("Enter");
        if (GetTag(other) != "Player" && GetTag(other) != "Enemy")
        {
            zoomReset = false;
        }
    }

    void OnTriggerExit(PhysicsComponent other)
    {
        //Console.WriteLine("Exit");
        if (GetTag(other) != "Player" && GetTag(other) != "Enemy")
        {
            timer = 0f;
            zoomReset = true;
        }
    }

    void AvoidColliders()
    {
        vec3 tempDirection = transform.position - target.transform.position;
        RaycastHit raycast = Physics.Raycast(target.transform.position, tempDirection, 1.01f);
        
        if (raycast.hit && raycast.gameObj != null)
        {
            isZooming = true;
            zoomReset = false;
            string tagName = GetTag(raycast.gameObj);
            if (tagName != "Camera" && tagName != "Enemy")
            {
                //Console.WriteLine("Name: " +  raycast.gameObj.name);
                finalZoom = vec3.Distance(target.transform.position, raycast.point) * 0.95f;
                finalZoom = Mathf.Clamp(finalZoom, closestZoom, furthestZoom);
                zoomTimer = 0f;
            }
        }
        else
        {
            isZooming = false;
        }

        if (zoomReset)
            ResetZoom();
    }

    public void SetYaw(float yaw)
    {
        setYawAngle = yaw;
        settingYaw = true;
    }
        
    void UpdateCameraRotation()
    {
        vec2 mouseDelta = Input.GetMouseDelta();
        if (mouseDelta.LengthSqr > 1.0f)
            return;

        if(settingYaw)
        {
            settingYaw = false;
            yawAngle = setYawAngle;
            Console.WriteLine("Angle: " + setYawAngle);
        }
        else
        {
            yawAngle -= mouseDelta.x * yawRotSpeed * yawSM * Time.deltaTime * 3.14f / 180f;
        }

        //Pitch Camera Rotation
        pitchAngle -= mouseDelta.y * (invertPitch ? -1.0f : 1.0f) * pitchRotSpeed * pitchSM * Time.deltaTime * 3.14f / 180f;
        if (pitchAngle > maxPitchAngle * 3.14f / 180f)
            pitchAngle = maxPitchAngle * 3.14f / 180f;
        else if (pitchAngle < minPitchAngle * 3.14f / 180f)
            pitchAngle = minPitchAngle * 3.14f / 180f;

        //transform.localRotation = new vec3(pitchAngle, yawAngle, 0f);

        quat newQuat = glm.FromEulerToQuat(new vec3(pitchAngle, yawAngle, 0f)).Normalized;
        //quat oldQuat = glm.FromEulerToQuat(transform.localRotation).Normalized;

        // Interpolate using spherical linear interpolation (slerp)
        quat midQuat = quat.SLerp(camera.rotation, newQuat, Time.deltaTime * 15f);
        if (midQuat != quat.NaN)
        {
            camera.rotation = midQuat;
        }
    }

    void Zoom()
    {
        float scroll = Input.GetScroll();

        if (scroll == 0f || isZooming) // Dont allow manual zoom if it is doing wall collision
            return;

        zoomReset = false;
        zoom += scroll * zoomSpeed;
        if (zoom < closestZoom)
        {
            zoom = closestZoom;
        }
        else if (zoom > furthestZoom)
        {
            zoom = furthestZoom;
        }

        camera.lookatDistance = zoom;
    }
    void ResetZoom()
    {
        // Give it a buffer before reseting the camera's zoom
        //bufferTimer += Time.deltaTime;

        //if (bufferTimer >= bufferDuration)
        //{
        //    timer += Time.deltaTime;
        //    zoom = Mathf.Lerp(zoom, defaultZoom, timer);

        //    if (timer >= duration)
        //    {
        //        isZooming = true;
        //        timer = bufferTimer = 0f;
        //    }
        //}

        if (timer > 1.0f)
            timer = 0f;

        zoom = Mathf.Lerp(zoom, defaultZoom, timer);
        timer += Time.deltaTime;
    }

    void FocusOnTarget()
    {
        if (target != null)
        {
            if (finalZoom != 0f)
            {
                zoomTimer += Time.deltaTime;
                zoom = Mathf.Lerp(zoom, finalZoom, zoomTimer, 0.16f, Mathf.EasingType.EASEOUT);
            }
            vec3 finalPosition = target.transform.position - (camera.forward * zoom);
            finalZoom = 0f;
            transform.localPosition = finalPosition;

            camera.LookAt(target);
        }
    }

    public void ShakeCamera(float magnitude, float duration)
    {
        shakeDuration = duration;
        shakeMagnitude = magnitude;
    }

    void ShakeCoroutine()
    {
        if (shakeDuration > 0)
        {
            shakeDuration -= Time.deltaTime;
            float x = RNG.Range(-1f, 1f) * shakeMagnitude;
            float y = RNG.Range(-1f, 1f) * shakeMagnitude;
            transform.localPosition += new vec3(x,y,0);
        }
    }

    public void SetFOV(float targetFOV, float duration)
    {
        //StartCoroutine(FOVLerp(targetFOV, duration));
    }

    IEnumerator FOVLerp(float targetFOV, float duration)
    {
        float elapsed = 0f;
        float startFOV = camera.fieldOfView;

        while (elapsed < duration/2)
        {
            camera.fieldOfView = glm.Lerp(startFOV, startFOV+targetFOV, elapsed / duration);

            elapsed += Time.deltaTime;
            yield return null;
        }

        while (elapsed < duration)
        {
            camera.fieldOfView = glm.Lerp(startFOV + targetFOV, startFOV, elapsed / duration);

            elapsed += Time.deltaTime;
            yield return null;
        }

        camera.fieldOfView = startFOV; // Ensure it reaches the exact target value
    }
}
