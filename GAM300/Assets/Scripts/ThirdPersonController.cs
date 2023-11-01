using System.Collections;
using System.Collections.Generic;
using BeanFactory;

public class ThirdPersonController : Script
{
  /*  public float MoveSpeed = 5f;
    public float JumpSpeed = 3f;
    public float Gravity = 9.81f;

    public CharacterController CC;
    public Transform CamYawPivot;
    public Transform CamMovePivot;
    public Transform CamPitchPivot;
    public Transform PlayerModel;


    private Vector3 VerticalVelocity;
    private bool IsMoving = false;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //Set velocity to 0 if no input is given
        Vector3 dir = Vector3.zero;

        //Handle Movement Input
        if (Input.GetKey(KeyCode.W))
            dir += CamYawPivot.forward;

        if (Input.GetKey(KeyCode.A))
            dir -= CamYawPivot.right;

        if (Input.GetKey(KeyCode.S))
            dir -= CamYawPivot.forward;

        if (Input.GetKey(KeyCode.D))
            dir += CamYawPivot.right;


        //Determine whether a movement input was given
        IsMoving = dir != Vector3.zero;

        //Adjust the rotation of the model whenever the player moves
        if(IsMoving)
        {
            PlayerModel.forward = CamYawPivot.forward;
        }

        //Handle Gravity 
        if(CC.isGrounded)
        {
            //Small gravity applied when character is grounded to ensure grounded flag stays active
            VerticalVelocity = Vector3.down * 0.5f;

            //Jump
            if (Input.GetKeyDown(KeyCode.Space))
                VerticalVelocity = Vector3.up * JumpSpeed;
        }
        else
        {
            //Increase gravity for every frame we're not contacting the ground
            VerticalVelocity += Vector3.down * Gravity * Time.deltaTime;
        }

        //Apply Gravity
        dir += VerticalVelocity;

        //Apply movement
        CC.Move(dir * MoveSpeed * Time.deltaTime);
    }*/
}
