using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;

public class ThirdPersonController : Script
{
    public float MoveSpeed = 5f;
    public float currentMoveSpeed;
    public float sprintSpeed = 30f;
    public float JumpSpeed = 3f;
    public float Gravity = 9.81f;

    public CharacterController CC;
    public Transform CamYawPivot;
    public Transform CamMovePivot;
    public Transform CamPitchPivot;
    public Transform PlayerModel;
    public Transform player;
    public GameObject playerWeaponCollider;

    List<vec3> pos = new List<vec3>(); 

    private vec3 VerticalVelocity;
    public bool IsMoving = false;

    public float RotationSpeed = 1;

    public AudioSource audioSource;

    public bool IsAttacking = false;
    public float attackTimer = 1f;
    public float currentAttackTimer;


    public float maxHealth = 5f;
    public float currentHealth;
    public bool isInvulnerable = false;
    public float invulnerableTimer = 1f;
    public float currentInvulnerableTimer;
    // Start is called before the first frame update
    void Start()
    {
        audioSource.Play();
        currentMoveSpeed = MoveSpeed;
        playerWeaponCollider.SetActive(false);
        currentAttackTimer = attackTimer;
        currentHealth = maxHealth;
        currentInvulnerableTimer = invulnerableTimer;
    }

    // Update is called once per frame
    void Update()
    {
        //Set velocity to 0 if no input is given
        vec3 dir = vec3.Zero;
        //Handle Movement Input
        if (Input.GetKey(KeyCode.W))
            dir -= (CamYawPivot.forward);

        if (Input.GetKey(KeyCode.A))
            dir -= (CamYawPivot.right);

        if (Input.GetKey(KeyCode.S))
            dir += CamYawPivot.forward;

        if (Input.GetKey(KeyCode.D))
            dir += (CamYawPivot.right);
        //Jump
        if (Input.GetKey(KeyCode.Space) && CC.isGrounded)
        {
            //dir += (CamYawPivot.up);
            dir += (player.up);
            Console.WriteLine("Jump:");
            Console.WriteLine(dir.x + "," + dir.y + "," + dir.z);
        }

        //Sprint
        if(Input.GetKey(KeyCode.LeftShift) && IsMoving && CC.isGrounded)
        {
            currentMoveSpeed = sprintSpeed;
        }
        else
        {
            currentMoveSpeed = MoveSpeed;
        }
        ////testing reset position
        //if(Input.GetKey(KeyCode.Q))
        //{
        //    Console.WriteLine("ResetPlayerPosition");
        //    player.localPosition = new vec3(-19.586f, 2.753f, 21.845f);
        //    //player.localRotation = new vec3(180, 0, 0);
        //}
        //Determine whether a movement input was given
        IsMoving = dir != vec3.Zero;

        //Adjust the rotation of the model whenever the player moves
        if (IsMoving)
        {
            dir = dir.Normalized;

            // Calculate the angle based on the direction of movement

            float angle = (float)Math.Atan2(-dir.x, -dir.z);

            quat newQuat = glm.FromEulerToQuat(new vec3(0,angle,0)).Normalized;
            quat oldQuat = glm.FromEulerToQuat(PlayerModel.localRotation).Normalized;

            // Interpolate using spherical linear interpolation (slerp)
            quat midQuat = quat.SLerp(oldQuat, newQuat, Time.deltaTime * RotationSpeed);

            vec3 rot = ((vec3)midQuat.EulerAngles);

            if (rot != vec3.NaN)
            {
                bool isNan = false;
                foreach (float val in rot)
                {
                    if (float.IsNaN(val))
                    {
                        isNan = true;
                        break;
                    }
                }
                if (!isNan)
                {
                    PlayerModel.localRotation = rot;
                }
            }
        }

        //Handle Gravity 
        if (CC.isGrounded)
        {
/*            if (pos.Count == 10)
            {
                pos.RemoveAt(0);
            }
            pos.Add(transform.localPosition);*/
            //Small gravity applied when character is grounded to ensure grounded flag stays active
            VerticalVelocity = new vec3(0,-1,0) * 0.5f;

            //Jump
            if (Input.GetKey(KeyCode.Space))
            {
                VerticalVelocity = new vec3(0, 1, 0) * JumpSpeed;
                CC.force = new vec3(0, 10, 0);
            }
        }
/*        else if (transform.localPosition.y <= -10f)
        {
            transform.localPosition = pos[0];
        }*/
        // else
        // {
        //     //Increase gravity for every frame we're not contacting the ground
        //     VerticalVelocity += new vec3(0, -1, 0) * Gravity;
        // }

        //Apply Gravity
        dir += VerticalVelocity;

        //Console.WriteLine("dir in c#:" + dir.x + "," + dir.y + "," + dir.z);


        if (IsMoving)
            //Apply movement
            CC.Move(PlayerModel.back * currentMoveSpeed + VerticalVelocity);


        //attacking
        if(Input.GetMouseDown(0) && !IsAttacking)
        {
            Console.WriteLine("Attack");
            IsAttacking = true;
            playerWeaponCollider.SetActive(true);//enable the weapon collider
        }
        if(IsAttacking)
        {
            currentAttackTimer -= Time.deltaTime;
            if(currentAttackTimer <= 0)
            {
                IsAttacking = false;
                playerWeaponCollider.SetActive(false);
                currentAttackTimer = attackTimer;
            }
        }
        //invulnerablility
        if (isInvulnerable)
        {
            currentInvulnerableTimer -= Time.deltaTime;
            if (currentInvulnerableTimer <= 0)
            {
                isInvulnerable = false;
                currentInvulnerableTimer = invulnerableTimer;
            }
        }
        //Testing taking damage
        if (Input.GetKey(KeyCode.T))
        {            
            TakeDamage(1);
            isInvulnerable = true;
        }

    }

    void TakeDamage(int amount)
    {
        if(!isInvulnerable)
        {
            //isInvulnerable = true;
            currentHealth -= amount;           
        }
        Console.WriteLine("Hit");

        if (currentHealth <= 0)
        {
            Console.WriteLine("YouDied");
            PlayerModel.gameObject.SetActive(false);//testing, remove this later
        }
    }

    void OnCollisionEnter(Rigidbody rb)
    {
        Console.WriteLine("INTO THE UNKNOWN");
    }
}
