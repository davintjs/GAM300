using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;

public class ThirdPersonController : Script
{
    public float MoveSpeed = 5f;
    public float JumpSpeed = 3f;
    public float Gravity = 9.81f;

    public CharacterController CC;
    public Transform CamYawPivot;
    public Transform CamMovePivot;
    public Transform CamPitchPivot;
    public Transform PlayerModel;

    private vec3 VerticalVelocity;
    private bool IsMoving = false;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        //Set velocity to 0 if no input is given
        vec3 dir = vec3.Zero;
        //Handle Movement Input
        if (Input.GetKey(KeyCode.W))
            dir += (CamYawPivot.forward * MoveSpeed);

        if (Input.GetKey(KeyCode.A))
            dir += (CamYawPivot.right * MoveSpeed);

        if (Input.GetKey(KeyCode.S))
            dir -= (CamYawPivot.forward * MoveSpeed);

        if (Input.GetKey(KeyCode.D))
            dir -= (CamYawPivot.right * MoveSpeed);


        //Determine whether a movement input was given
        IsMoving = dir != vec3.Zero;

        //Adjust the rotation of the model whenever the player moves
        if (IsMoving)
        {
            
            PlayerModel.localRotation.y = CamYawPivot.localRotation.y;
        }

        //Handle Gravity 
        if (CC.isGrounded)
        {
            //Small gravity applied when character is grounded to ensure grounded flag stays active
            VerticalVelocity = new vec3(0,-1,0) * 0.5f;

            //Jump
            if (Input.GetKeyDown(KeyCode.Space))
                VerticalVelocity = new vec3(0, 1, 0) * JumpSpeed;
        }
        else
        {
            //Increase gravity for every frame we're not contacting the ground
            VerticalVelocity += new vec3(0, -1, 0) * Gravity;
        }

        //Apply Gravity
        dir += VerticalVelocity;

        //Console.WriteLine("dir in c#:" + dir.x + "," + dir.y + "," + dir.z);


        //Apply movement
        CC.Move(dir);
    }
}
