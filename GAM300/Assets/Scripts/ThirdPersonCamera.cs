using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
public class ThirdPersonCamera : Script
{
    public static ThirdPersonCamera instance;
    public float YawRotSpeed = 180f;
    public float PitchRotSpeed = 180f;
    public float MaxPitchAngle = 85f;
    public float MinPitchAngle = -85f;
    public bool InvertPitch = false;

    public Transform CamYawPivot;
    public Transform CamMovePivot;
    public Transform CamPitchPivot;

    private float YawAngle = 0f;
    private float PitchAngle = 0f;

    public bool exitTrigger = false;
    public float closestZoom = 4f;
    public float furthestZoom = 10f;
    public float zoomSpeed = 3f;
    public float zoom = 0f;
    public float timer = 0f;
    private float duration = 1.0f;
    private float bufferTimer = 0f;
    private float bufferDuration = 3.0f;
    private float initialZoom;

    // Start is called before the first frame update
    

    float fov = 0;
    private Camera camera;

    void Awake()
    {
        camera = GetComponent<Camera>();
        instance = this;
        fov = camera.fieldOfView;
    }
    void Start()
    {
        //Lock and hide mouse cursor
        /*        Cursor.lockState = CursorLockMode.Locked;
                Cursor.visible = false;*/

        zoom = -transform.localPosition.z;
    }

    // Update is called once per frame
    void Update()
    {
        //Yaw Camera Rotation
        UpdateCameraRotation();

        if (exitTrigger)
            ResetZoom();

        Zoom();
    }

    void OnTriggerEnter(PhysicsComponent other)
    {
        if(GetTag(other) != "PlayerCollider")
        {
            zoom += zoomSpeed * Time.deltaTime;
        }
    }

    void OnTriggerExit(PhysicsComponent other)
    {
        if (GetTag(other) != "PlayerCollider")
        {
            initialZoom = zoom;
            exitTrigger = true;
        }
    }

    void UpdateCameraRotation()
    {
        vec2 mouseDelta = Input.GetMouseDelta();
        YawAngle -= mouseDelta.x * YawRotSpeed * Time.deltaTime * 3.14f / 180f;
        CamYawPivot.localRotation = new vec3(0f, YawAngle, 0f);
        //Pitch Camera Rotation
        PitchAngle -= mouseDelta.y * (InvertPitch ? -1.0f : 1.0f) * PitchRotSpeed * Time.deltaTime * 3.14f / 180f;
        //Console.WriteLine("{0},{1}",mouseDelta.x, mouseDelta.y);
        if (PitchAngle > MaxPitchAngle)
            PitchAngle = MaxPitchAngle * 3.14f / 180f;
        else if (PitchAngle < MinPitchAngle)
            PitchAngle = MinPitchAngle * 3.14f / 180f;
        CamPitchPivot.localRotation = new vec3(PitchAngle, 0f, 0f);
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
        transform.localPosition.z = -zoom;
    }
    void ResetZoom()
    {
        // Give it a buffer before reseting the camera's zoom
        bufferTimer += Time.deltaTime;

        if (bufferTimer >= bufferDuration)
        {
            timer += Time.deltaTime;
            zoom = Lerp(initialZoom, furthestZoom, timer, duration);

            if (timer >= duration)
            {
                exitTrigger = false;
                timer = bufferTimer = 0f;
            }
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
