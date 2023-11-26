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
    public GameObject attackLight;
    public GameObject playerWeaponCollider1;
    public GameObject playerWeaponCollider2;
    public GameObject playerWeaponCollider3;
    GameObject selectedWeaponCollider;
    public Transform checkpoint2;

    //public InstantDeath instantDeath;

    List<vec3> pos = new List<vec3>();

    public float RotationSpeed = 1;

    public AudioSource audioSource;

    AnimationStateMachine animationManager;

    bool _isAttacking = false;

    bool IsAttacking 
    {
        get { return _isAttacking; }
        set
        {
            _isAttacking = value;
            currentAttackTimer = 0f;
            for (int i = 1; i <= 3; ++i)
            {
                SetState("Attack" + i, false);
            }
            if (_isAttacking == false)
            {
                playerWeaponCollider1.SetActive(false);
                playerWeaponCollider2.SetActive(false);
                playerWeaponCollider3.SetActive(false);
                comboCount = 1;
                selectedWeaponCollider.transform.localPosition = new vec3(10000);
                attackLight.SetActive(false);
            }
            else
            {
                selectedWeaponCollider.transform.localRotation = new vec3(PlayerModel.localRotation);
                SetState("Attack" + comboCount, true);
                AudioManager.instance.playerSlashAttack.Play();
                AudioManager.instance.spark.Play();
                AudioManager.instance.playerAttack.Play();
                ++comboCount;
                if (comboCount > 3) { comboCount = 1; }
            }
        }
    }
    public float attackTimer = 1f;
    float currentAttackTimer;

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

    public float animCancelPercentage = 1f;

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

        stun.SetConditionals(false, death);
        falling.SetConditionals(false, death, stun);
        falling.loop = true;
        jump.SetConditionals(false, death,stun);
        attack1.SetConditionals(false, jump, death, stun);
        attack2.SetConditionals(false, jump, death, stun);
        attack3.SetConditionals(false, jump, death, stun);
        attack1.speed = 1.5f;
        attack2.speed = 1.5f;
        attack3.speed = 1.5f;
        sprint.SetConditionals(true, run);
        sprint.SetConditionals(false, attack1, jump, death, stun);
        sprint.loop = true;
        run.SetConditionals(false, sprint, attack1, jump, death, stun);
        run.loop = true;
    }


    void Start()
    {
        audioSource.Play();
        playerWeaponCollider1.SetActive(false);
        playerWeaponCollider2.SetActive(false);
        playerWeaponCollider3.SetActive(false);
        selectedWeaponCollider = playerWeaponCollider1;
        currentAttackTimer = 0;
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
            if (currentAttackTimer / attackTimer < 0.2f)
                movement = PlayerModel.back * MoveSpeed * Time.deltaTime;
            else
            {
                if (currentAttackTimer / attackTimer > 0.5f)
                {
                    selectedWeaponCollider.transform.localPosition = new vec3(10000);
                    attackLight.SetActive(false);
                }
                else if (currentAttackTimer / attackTimer > 0.3f )
                {
                    selectedWeaponCollider.transform.localPosition = new vec3(transform.localPosition + PlayerModel.back * 0.6f);
                    attackLight.SetActive(true);
                    selectedWeaponCollider.SetActive(true);//enable the weapon collider
                }
                movement = vec3.Zero;
            }
            attackLight.transform.localPosition = new vec3(selectedWeaponCollider.transform.localPosition);
            currentAttackTimer += Time.deltaTime;
            if (currentAttackTimer >= attackTimer)
            {
                IsAttacking = false;
            }
        }

        //invulnerability
        if (isInvulnerable)
        {
            currentInvulnerableTimer -= Time.deltaTime;
            if (currentInvulnerableTimer <= 0)
            {
                isInvulnerable = false;
                currentInvulnerableTimer = invulnerableTimer;
                SetState("Stun", false);
            }
            return;
        }
        else if (CC.isGrounded)
        {
            if (GetState("Falling"))
            {
                SetState("Falling", false);
                AudioManager.instance.playerFootstep.Play();
            }

            bool combo = IsAttacking && currentAttackTimer/attackTimer > animCancelPercentage;
            if (Input.GetMouseDown(0) && (combo || !IsAttacking))
            {
                switch (comboCount)
                {
                    case 1:
                        attackTimer = attack1duration;
                        selectedWeaponCollider = playerWeaponCollider1;
                        playerWeaponCollider2.SetActive(false);
                        playerWeaponCollider3.SetActive(false);
                        break;
                    case 2:
                        selectedWeaponCollider = playerWeaponCollider2;
                        attackTimer = attack2duration;
                        playerWeaponCollider1.SetActive(false);
                        playerWeaponCollider3.SetActive(false);
                        break;
                    case 3:
                        selectedWeaponCollider = playerWeaponCollider3;
                        attackTimer = attack3duration;
                        playerWeaponCollider2.SetActive(false);
                        playerWeaponCollider1.SetActive(false);
                        break;
                }
                IsAttacking = true;
            }

            //JUMP
            else if (Input.GetKeyDown(KeyCode.Space) && !IsAttacking)
            {
                SetState("Jump", true);
                AudioManager.instance.jumpVoice.Play();
                movement += vec3.UnitY * JumpSpeed;
            }
            else if (!IsAttacking)
            {
                SetState("Jump", false);
                //SPRINT
                if (Input.GetKey(KeyCode.LeftShift) && isMoving)
                {
                    walkSoundTime = runStepsInterval;
                    SetState("Sprint", true);
                    SetState("Run", true);
                    movement *= sprintModifier;
                }
                else
                {
                    walkSoundTime = walkStepsInterval;
                    SetState("Sprint", false);
                }
                SetState("Run", isMoving);
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
            player.localPosition = new vec3(checkpoint2.localPosition);
        }

        CC.Move(movement);

        //attacking


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
        if (!isInvulnerable)
        {
            IsAttacking = false;
            AudioManager.instance.playerInjured.Play();
            ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.damagedShakeMag, CombatManager.instance.damagedShakeDur);
            ThirdPersonCamera.instance.SetFOV(CombatManager.instance.damagedShakeMag * 100, CombatManager.instance.damagedShakeDur);
            isInvulnerable = true;
            currentInvulnerableTimer = invulnerableTimer;
            currentHealth -= amount;
            UpdatehealthBar();
        }
        
        if (currentHealth <= 0)
        {
            Console.WriteLine("YouDied");
            isDead = true;
            SetState("Death", true);
            animationManager.UpdateState();
        }
        else
        {
            SetState("Stun", true);
            animator.Stop();
            animationManager.UpdateState();
            animator.Play();
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
