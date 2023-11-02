using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
public class ThirdPersonCamera : Script
{
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

    // Start is called before the first frame update
    void Start()
    {
        //Lock and hide mouse cursor
/*        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;*/
    }

    // Update is called once per frame
    void Update()
    {
        //Yaw Camera Rotation

        vec2 mouseDelta = Input.GetMouseDelta();
        YawAngle += mouseDelta.x * YawRotSpeed * Time.deltaTime * 3.14f / 180f;
        CamYawPivot.localRotation = new vec3(0f, YawAngle, 0f);
        //Pitch Camera Rotation
        PitchAngle += mouseDelta.y * (InvertPitch ? -1.0f : 1.0f) * PitchRotSpeed * Time.deltaTime * 3.14f / 180f;
        //Console.WriteLine("{0},{1}",mouseDelta.x, mouseDelta.y);
        if (PitchAngle > MaxPitchAngle)
            PitchAngle = MaxPitchAngle * 3.14f / 180f;
        else if (PitchAngle < MinPitchAngle)
            PitchAngle = MinPitchAngle * 3.14f / 180f;
        CamPitchPivot.localRotation = new vec3(PitchAngle, 0f, 0f);
    }

}