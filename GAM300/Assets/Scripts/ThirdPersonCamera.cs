using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
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
        //YawAngle += Input.GetAxis("Mouse X") * YawRotSpeed * Time.deltaTime;
        CamYawPivot.localRotation = new vec3(0f, YawAngle, 0f);

        //Pitch Camera Rotation
        //PitchAngle += Input.GetAxis("Mouse Y") * (InvertPitch ? -1.0f : 1.0f) * PitchRotSpeed * Time.deltaTime;

        if (PitchAngle > MaxPitchAngle)
            PitchAngle = MaxPitchAngle;
        else if (PitchAngle < MinPitchAngle)
            PitchAngle = MinPitchAngle;
        CamPitchPivot.localRotation = new vec3(PitchAngle, 0f, 0f);
    }
}
