using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Runtime.InteropServices;
public class ThirdPersonCamera : Script
{
    public static ThirdPersonCamera instance;
    private Camera camera;

    public float yawRotSpeed = 1f;
    public float pitchRotSpeed = 1f;
    public float maxPitchAngle = 170f;
    public float minPitchAngle = -85f;
    public bool invertPitch = false;

    private float yawAngle;
    public float pitchAngle;
    private const float yawSM = 1800f;
    private const float pitchSM = 1800f;

    public GameObject target;

    public float zoom = 1f;
    private float initialZoom = 0f;
    public float defaultZoom = 4f;
    public float zoomSpeed = 50f;
    public float zoomInSpeed = 3f;
    public float closestZoom = 1f;
    public float furthestZoom = 10f;
    public bool isZooming = false;
    private bool zoomReset = false;

    public float timer = 0f;
    private float duration = 1.0f;
    private float bufferTimer = 0f;
    private float bufferDuration = 3.0f;

    public bool cutscene = false;

    private vec3 targetPosition;

    void Awake()
    {
        targetPosition = target.transform.localPosition;
        camera = GetComponent<Camera>();
        instance = this;
    }

    // Update is called once per frame
    void LateUpdate()
    {
        if (cutscene)
        {
            return;
        }
        Zoom();

        FocusOnTarget();

        UpdateCameraRotation();

        AvoidColliders();
    }

    void OnTriggerEnter(PhysicsComponent other)
    {
        if (GetTag(other) != "PlayerCollider")
        {
            StartZoom();
        }
    }

    void OnTriggerExit(PhysicsComponent other)
    {
        if (GetTag(other) != "PlayerCollider")
        {
            StopZoom();
        }
    }

    void StartZoom()
    {
        isZooming = true;
        zoomReset = false;
        timer = bufferTimer = 0f;
    }

    void StopZoom()
    {
        isZooming = false;
    }

    void AvoidColliders()
    {
        if (isZooming)
        {
            zoom -= Time.deltaTime * zoomInSpeed;
            if (zoom < closestZoom)
            {
                zoom = closestZoom;
            }
            else if (zoom > furthestZoom)
            {
                zoom = furthestZoom;
            }

            camera.lookatDistance = zoom;
            initialZoom = zoom;
            zoomReset = true;
        }
        else if (!isZooming && zoomReset)
        {
            // Wait x seconds then attemp to return to initialZoom
            ResetZoom();
        }
    }

    void UpdateCameraRotation()
    {
        vec2 mouseDelta = Input.GetMouseDelta();
        yawAngle -= mouseDelta.x * yawRotSpeed * yawSM * Time.deltaTime * 3.14f / 180f;

        //Pitch Camera Rotation
        pitchAngle -= mouseDelta.y * (invertPitch ? -1.0f : 1.0f) * pitchRotSpeed * pitchSM * Time.deltaTime * 3.14f / 180f;
        if (pitchAngle > maxPitchAngle * 3.14f / 180f)
            pitchAngle = maxPitchAngle * 3.14f / 180f;
        else if (pitchAngle < minPitchAngle * 3.14f / 180f)
            pitchAngle = minPitchAngle * 3.14f / 180f;

        transform.localRotation = new vec3(pitchAngle, yawAngle, 0f);
    }

    void Zoom()
    {
        zoom += Input.GetScroll() * zoomSpeed;
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
        bufferTimer += Time.deltaTime;

        if (bufferTimer >= bufferDuration)
        {
            timer += Time.deltaTime;
            zoom = Lerp(initialZoom, defaultZoom, timer, duration);

            if (timer >= duration)
            {
                zoomReset = false;
                timer = bufferTimer = 0f;
            }
        }
    }

    void FocusOnTarget()
    {
        if (target != null)
        {
            camera.LookAt(target);
            camera.position = target.transform.position - (camera.forward * camera.distance);
            transform.position = camera.position;
        }
    }

    float Lerp(float start, float end, float value, float duration)
    {
        value /= duration;
        return (1.0f - value) * start + value * end;
    }

    public void ShakeCamera(float magnitude, float duration)
    {
        StartCoroutine(ShakeCoroutine(magnitude, duration));
    }

    IEnumerator ShakeCoroutine(float magnitude, float duration)
    {
        vec3 originalPosition = transform.localPosition;
        float elapsed = 0f;
        while (elapsed < duration)
        {
            float x = RNG.Range(-1f, 1f) * magnitude;
            float y = RNG.Range(-1f, 1f) * magnitude;

            transform.localPosition = new vec3(originalPosition.x + x, originalPosition.y + y, originalPosition.z);

            elapsed += Time.deltaTime;
            yield return null;
        }
        transform.localPosition = originalPosition; // Reset position after shaking
    }

    public void SetFOV(float targetFOV, float duration)
    {
        StartCoroutine(FOVLerp(targetFOV, duration));
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
