using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Linq;
using System.Security.Policy;

public class ThirdPersonController : Script
{
    public float MoveSpeed = 5f;
    public float sprintModifier = 1.5f;
    public float JumpSpeed = 3f;
    public float Gravity = 9.81f;
    private float walkSoundTimer = 0f;
    private float walkSoundTime = 0.5f;

    public float runStepsInterval = 0.25f;
    public float walkStepsInterval = 0.5f;


    public CharacterController CC;
    public Transform CamYawPivot;
    public Transform CamMovePivot;
    public Transform CamPitchPivot;
    public Transform PlayerModel;
    public Transform player;
    public GameObject playerWeaponCollider;
    public Transform checkpoint2;

    //public InstantDeath instantDeath;

    List<vec3> pos = new List<vec3>();

    public float RotationSpeed = 1;

    public AudioSource audioSource;

    AnimationStateMachine animationManager;

    public bool IsAttacking = false;
    public float attackTimer = 1f;
    public float currentAttackTimer;


    public float maxHealth = 4f;
    public float currentHealth;
    public bool isInvulnerable = false;
    public float invulnerableTimer = 1f;
    public float currentInvulnerableTimer;
    public bool isDead = false;

    float maxAirTime = 1f;
    float currentAirTime = 0;

    public float colliderDist = 0;
    public float maxColliderDist = 1f;

    //health bar
    public GameObject healthBarFill;
    public GameObject healthStaminaCanvas;
    vec3 initialHealthBarPos;
    float initialHealthBarXpos;
    float initialHealthBarXScale;

    public Animator animator;
    public bool startDeathAnimationCountdown = false;
    float animationTimer = 3.18f;
    public float currentAnimationTimer;

    public float attack1duration = 1f;
    public float attack2duration = 2f;
    public float attack3duration = 2f;

    float attackDisplacement = 0f;

    int comboCount = 1;

    bool _wasMoving = false;
    bool wasMoving
    {
        get { return _wasMoving; }
        set
        {
            if (value == false && _wasMoving)
            {
                walkSoundTimer = 0f;
                AudioManager.instance.playerFootstep.Play();
            }

            _wasMoving = value;
            if (_wasMoving)
            {
                walkSoundTimer += Time.deltaTime;
            }
            if (walkSoundTimer > walkSoundTime)
            {
                AudioManager.instance.playerFootstep.Play();
                walkSoundTimer = 0;
            }
        }
    }

    // Start is called before the first frame update
    void InitAnimStates()
    {
        animationManager = new AnimationStateMachine(animator);
        currentAnimationTimer = animationTimer;

        //Highest Precedence
        AnimationState death = animationManager.GetState("Death");
        AnimationState stun = animationManager.GetState("Stun");
        AnimationState falling = animationManager.GetState("Falling");
        AnimationState jump = animationManager.GetState("Jump");
        AnimationState attack1 = animationManager.GetState("Attack1");
        AnimationState attack2 = animationManager.GetState("Attack2");
        AnimationState attack3 = animationManager.GetState("Attack3");
        AnimationState sprint = animationManager.GetState("Sprint");
        AnimationState run = animationManager.GetState("Run");
        //Lowest Precedence

        death.stall = true;
        stun.SetConditionals(false, death);
        stun.stall = true;
        falling.SetConditionals(false, death, stun);
        falling.loop = true;
        jump.SetConditionals(false, death,stun);
        attack1.SetConditionals(false, jump, death, stun);
        attack1.stall = true;
        attack2.SetConditionals(false, jump, death, stun);
        attack2.stall = true;
        attack3.SetConditionals(false, jump, death, stun);
        attack3.stall = true;
        sprint.SetConditionals(true, run);
        sprint.SetConditionals(false, attack1, jump, death, stun);
        sprint.loop = true;
        run.SetConditionals(false, sprint, attack1, jump, death, stun);
        run.loop = true;
    }


    void Start()
    {
        audioSource.Play();
        playerWeaponCollider.SetActive(false);
        currentAttackTimer = attackTimer;
        currentHealth = maxHealth;
        currentInvulnerableTimer = invulnerableTimer;

        initialHealthBarPos = healthBarFill.GetComponent<Transform>().localPosition;
        initialHealthBarXpos = healthBarFill.GetComponent<Transform>().localPosition.x;
        initialHealthBarXScale = healthBarFill.GetComponent<Transform>().localScale.x;
        walkSoundTime = walkStepsInterval;
        InitAnimStates();
    }

    // Update is called once per frame
    void Update()
    {
        //restart game
        if (Input.GetKey(KeyCode.R))
        {
            Console.WriteLine("Restart");
            SceneManager.LoadScene("LevelPlay2");
        }
        if (isDead) return;

        if (CC.velocity.y > JumpSpeed)
        {
            CC.velocity.y = JumpSpeed;
        }

        //Testing taking damage
        if (Input.GetKeyDown(KeyCode.T))
        {
            TakeDamage(1);
            isInvulnerable = true;
        }
        vec3 dir = GetDirection();
        vec3 movement = dir * MoveSpeed * Time.deltaTime;

        bool isMoving = dir != vec3.Zero;
        bool moved = false;
        //Jump



        if (IsAttacking)
        {
            dir = vec3.Zero;
            if (currentAttackTimer / attackTimer < 0.1f)
                movement = PlayerModel.back * attackDisplacement * Time.deltaTime;
            else
                movement = vec3.Zero;
            playerWeaponCollider.transform.localPosition = transform.localPosition + PlayerModel.back * 2f;
            currentAttackTimer += Time.deltaTime;
            if (currentAttackTimer >= attackTimer)
            {
                IsAttacking = false;
                playerWeaponCollider.SetActive(false);
                currentAttackTimer = attackTimer;
            }
        }
        else if (CC.isGrounded)
        {
            if (GetState("Falling"))
            {
                SetState("Falling", false);
                AudioManager.instance.playerFootstep.Play();
            }




            bool combo = IsAttacking && currentAttackTimer/attackTimer > .9f;
            if (Input.GetMouseDown(0) && (combo || !IsAttacking))
            {
                IsAttacking = true;
                playerWeaponCollider.SetActive(true);//enable the weapon collider
                currentAttackTimer = 0f;
                playerWeaponCollider.transform.localRotation = PlayerModel.localRotation;
                SetState("Attack"+comboCount, true);
                AudioManager.instance.playerSlashAttack.Play();
                AudioManager.instance.spark.Play();
                AudioManager.instance.playerAttack.Play();
                attackDisplacement = MoveSpeed * comboCount;
                switch (comboCount)
                {
                    case 1:
                        attackTimer = attack1duration;
                        break;
                    case 2:
                        attackTimer = attack2duration;
                        break;
                    case 3:
                        attackTimer = attack3duration;
                        break;
                }
                ++comboCount;
                if (comboCount > 3) { comboCount = 1; }
            }

            //JUMP
            else if (Input.GetKeyDown(KeyCode.Space))
            {
                SetState("Jump", true);
                AudioManager.instance.jumpVoice.Play();
                movement += vec3.UnitY * JumpSpeed;
            }
            else
            {
                SetState("Jump", false);
                //SPRINT
                if (Input.GetKey(KeyCode.LeftShift))
                {
                    walkSoundTime = runStepsInterval;
                    SetState("Sprint", true);
                    movement *= sprintModifier;
                }
                else
                {
                    walkSoundTime = walkStepsInterval;
                    SetState("Sprint", false);
                    SetState("Run", isMoving);
                }
                if (isMoving)
                    moved = isMoving;
            }

            currentAirTime = 0;
        }
        else
        {
            if (animationManager.GetState("Jump").state)
            {
                if (currentAirTime >= maxAirTime)
                {
                    SetState("Falling", true);
                }
            }
            else if (currentAirTime >= maxAirTime * .5f)
            {
                SetState("Falling", true);
            }
            currentAirTime += Time.deltaTime;
            movement += vec3.UnitY * -Gravity;
        }
        wasMoving = moved;

        UpdateRotation(dir);
        //testing reset position
        if (Input.GetKey(KeyCode.P))
        {
            Console.WriteLine("ResetPlayerPosition");
            player.localPosition = new vec3(-19.586f, 2.753f, 21.845f);
            player.localRotation = vec3.Radians(new vec3(0, 180, 0));
        }
        if (Input.GetKey(KeyCode.L))
        {
            Console.WriteLine("ResetPlayerPosition");
            player.localPosition = checkpoint2.localPosition;
        }

        CC.Move(movement);

        //attacking

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


        animationManager.UpdateState();

        //death animation timer
        if(startDeathAnimationCountdown)
        {
            currentAnimationTimer -= Time.deltaTime;
            if(currentAnimationTimer <= 0)
            {
                animator.Pause();//pause the death animation to prevent it from returning to idle animation
            }
        }
    }

    public void UpdatehealthBar()
    {
        //NOTE: tempoary disabled, not working currently
        //float scaleFactor = (float)currentHealth / (float)maxHealth;
        //float newXScale = initialHealthBarXScale * scaleFactor;
        //float xOffset = (initialHealthBarXScale - newXScale) * 0.5f;
        //vec3 currentPos = healthBarFill.GetComponent<Transform>().localPosition;
        //vec3 currentScale = healthBarFill.GetComponent<Transform>().localScale;
        //currentPos.x = initialHealthBarXpos - xOffset;
        //currentScale.x = newXScale;
        //healthBarFill.GetComponent<Transform>().localPosition = currentPos;
        //healthBarFill.GetComponent<Transform>().localScale = currentScale;

        //hard code the health bar for now
        if(currentHealth == 3)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.7f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(0.15f, -0.035f, 1f);
        }
        if (currentHealth == 2)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.74f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(0.11f, -0.035f, 1f);
        }
        if (currentHealth == 1)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.8f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(0.05f, -0.035f, 1f);
        }
        if(currentHealth <= 0)
        {
            Console.WriteLine("GameOver");
            isDead = true;
            healthStaminaCanvas.SetActive(false);
            startDeathAnimationCountdown = true;
            currentHealth = 0;
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.8f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(0f, -0.035f, 1f);
        }
    }

    bool GetState (string stateName)
    {
        return animationManager.GetState(stateName).state;
    }
    void SetState(string stateName, bool value)
    {
        animationManager.GetState(stateName).state = value;
    }
    public void TakeDamage(float amount)
    {
        AudioManager.instance.playerInjured.Play();

        if (!isInvulnerable)
        {
            isInvulnerable = true;
            currentInvulnerableTimer = invulnerableTimer;
            currentHealth -= amount;
            UpdatehealthBar();
        }
        Console.WriteLine("Hit");
        
        if (currentHealth <= 0)
        {
            Console.WriteLine("YouDied");
            isDead = true;
            SetState("Death", true);
            
        }
        else
        {
            SetState("Stun", true);
        }
    }

    //Handle Movement Input
    vec3 GetDirection()
    {
        vec3 dir = vec3.Zero;
        if (Input.GetKey(KeyCode.W))
        {
            dir -= (CamYawPivot.forward);
        }


        if (Input.GetKey(KeyCode.A))
        {
            dir -= (CamYawPivot.right);
        }


        if (Input.GetKey(KeyCode.S))
        {
            dir += CamYawPivot.forward;
        }


        if (Input.GetKey(KeyCode.D))
        {
            dir += (CamYawPivot.right);
        }

        return dir.NormalizedSafe;
    }

    void UpdateRotation(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;
        float angle = (float)Math.Atan2(-dir.x, -dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
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

    public void Hello(string yes)
    {
        Console.WriteLine(yes);
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "EnemyAttack")
        {
            //AudioManager.instance.playerInjured.Play();
            TakeDamage(1);
        }
        ////Not working
        //if(GetTag(rb) == "PuzzleKey")
        //{
        //    Console.WriteLine("Collected");
        //    AudioManager.instance.itemCollected.Play();//play audio sound
        //}
    }

    void OnCollisionEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "InstantDeath")
        {
            currentHealth = 0;
            Console.WriteLine("InstantDeath");
            TakeDamage(maxHealth);
        }
    }
}
