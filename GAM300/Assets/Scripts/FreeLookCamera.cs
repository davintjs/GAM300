using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Threading;

public class FreeLookCamera : Script
{
    public static FreeLookCamera instance;
    private Camera camera;

    public float yawRotSpeed = 1f;
    public float pitchRotSpeed = 1f;
    public float maxPitchAngle = 170f;
    public float minPitchAngle = -85f;
    public bool invertPitch = false;

    private float yawAngle;
    private float pitchAngle;
    private const float yawSM = 1800f;
    private const float pitchSM = 1800f;

    public GameObject target;

    public float zoom = 1f;
    private float initialZoom = 0f;
    public float defaultZoom = 4f;
    public float zoomSpeed = 50f;
    public float zoomInSpeed = 2f;
    public float closestZoom = 1f;
    public float furthestZoom = 10f;
    private bool isZooming = false;
    private bool zoomReset = false;

    private float timer = 0f;
    private float duration = 0.5f;
    private float bufferTimer = 0f;
    private float bufferDuration = 3.0f;

    void Awake()
    {
        camera = GetComponent<Camera>();
        instance = this;
    }

    void Update()
    {
        Zoom();

        MoveTarget();

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
    float Lerp(float start, float end, float value, float duration)
    {
        value /= duration;
        return (1.0f - value) * start + value * end;
    }
    vec3 GetDirection()
    {
        vec3 dir = vec3.Zero;
        if (Input.GetKey(KeyCode.W))
        {
            dir -= (transform.forward);
        }


        if (Input.GetKey(KeyCode.A))
        {
            dir -= (transform.right);
        }


        if (Input.GetKey(KeyCode.S))
        {
            dir += transform.forward;
        }


        if (Input.GetKey(KeyCode.D))
        {
            dir += (transform.right);
        }

        return dir.NormalizedSafe;
    }

    void MoveTarget()
    {
        if(target != null)
        {
            vec3 dir = GetDirection();
            vec3 movement = dir * Time.deltaTime;
            movement.y = 0f;
            target.transform.localPosition += movement;
        }
    }

    void FocusOnTarget()
    {
        if(target != null)
        {
            camera.LookAt(target);
            camera.position = target.transform.position - (camera.forward * camera.distance);
            
            transform.localPosition = camera.position;
        }
    }

    void UpdateCameraRotation()
    {
        vec2 mouseDelta = Input.GetMouseDelta();
        yawAngle -= mouseDelta.x * yawRotSpeed * yawSM * Time.deltaTime * 3.14f / 180f;
        
        //Pitch Camera Rotation
        pitchAngle -= mouseDelta.y * (invertPitch ? -1.0f : 1.0f) * pitchRotSpeed * pitchSM * Time.deltaTime * 3.14f / 180f;
        if (pitchAngle > maxPitchAngle)
            pitchAngle = maxPitchAngle * 3.14f / 180f;
        else if (pitchAngle < minPitchAngle)
            pitchAngle = minPitchAngle * 3.14f / 180f;
        transform.localRotation = new vec3(pitchAngle, yawAngle, 0f);
    }
}