using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Policy;

public class ThirdPersonController : Script
{
    public static ThirdPersonController instance;
    public float MoveSpeed = 5f;
    public float attackMoveSpeed = 3f;
    public float sprintModifier = 1.5f;
    public float JumpSpeed = 3f;
    public float dashAttackSpeed = 20f;
    public float dodgeSpeed = 20f;
    public float Gravity = 9.81f;
    private float walkSoundTimer = 0f;
    private float walkSoundTime = 0.5f;

    public float runStepsInterval = 0.25f;
    public float walkStepsInterval = 0.5f;

    public float dashAttackTimer = 0.9f;
    public float currentDashAttackTimer;
    public float dashAttackCooldown = 0.9f;
    public float currentDashAttackCooldown;
    public bool startDashCooldown = false;


    public float dodgeTimer = 1f;
    public float currentDodgeTimer;
    public float dodgeCooldown = 2f;
    public float currentDodgeCooldown;
    public bool isDodging = false;
    public bool startDodgeCooldown = false;

    public float overdriveTimer = 2f;
    public float currentOverdriveTimer;
    public float overDriveCooldown = 6f;
    public float currentOverdriveCooldown;
    public bool startOverdriveCooldown = false;


    public CharacterController CC;
    public Transform PlayerCamera;
    public Transform PlayerModel;
    public Transform player;
    public GameObject attackLight;
    public GameObject overDriveCollider;
    public GameObject playerWeaponCollider1;
    public GameObject playerWeaponCollider2;
    public GameObject playerWeaponCollider3;
    GameObject selectedWeaponCollider;
    //public Transform checkpoint2;

    //public InstantDeath instantDeath;

    List<vec3> pos = new List<vec3>();

    public float RotationSpeed = 1;

    public AudioSource audioSource;

    AnimationStateMachine animationManager;

    public bool _isAttacking = false;
    public bool _isDashAttacking = false;
    public bool _isOverdrive = false;

    public int checkpointIndex = -1;
    public bool isAtCheckpoint = false;
    public Transform spawnPoint;
    public Transform terminal1;
    public Transform terminal2;

    void Awake()
    {
        if (instance != null)
        {
            //Debug.LogError("More than one AudioManager in the scene.");
        }
        else
        {
            instance = this;
        }
    }

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

    //stamina bar
    public float maxStamina = 100f;
    public float currentStamina;
    public GameObject staminaBarFill;
    public Transform staminaBarPos;
    private Coroutine regen;
    public float timeBeforeRegen = 1.5f;
    public float regenRate = 60f;//the higher the number, the slower the regen is
    private WaitForSeconds regenTick = new WaitForSeconds(0.1f);
    vec3 initialStaminaBarPos;
    float initialStaminaBarXpos;
    float initialStaminaBarXScale;
    public float sprintStamina = 0.5f;
    public float jumpStamina = 10f;
    public float attackStamina = 10f;
    public float dodgeStamina = 20f;
    public float dashAttackStamina = 30f;
    public float overDriveStamina = 40f;

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
        AnimationState dashAttack = animationManager.GetState("DashAttack");
        AnimationState dodge = animationManager.GetState("Dodge");
        AnimationState overdrive = animationManager.GetState("Overdrive");
        //Lowest Precedence

        stun.SetConditionals(false, death);
        falling.SetConditionals(false, death, stun);
        falling.loop = true;
        jump.SetConditionals(false, death,stun);
        attack1.SetConditionals(false, jump, death, stun);
        attack2.SetConditionals(false, jump, death, stun);
        attack3.SetConditionals(false, jump, death, stun);
        attack1.speed = 2.2f;
        attack2.speed = 2.8f;
        attack3.speed = 3.1f;
        sprint.SetConditionals(true, run);
        sprint.SetConditionals(false, attack1, jump, death, stun);
        sprint.loop = true;
        run.SetConditionals(false, sprint, attack1, jump, death, stun);
        run.loop = true;
        dashAttack.SetConditionals(false, jump, death, stun, sprint, run, attack1, attack2, attack3);
        dashAttack.speed = 2.5f;
        dodge.SetConditionals(false, dashAttack, jump, death, stun, attack1, attack2, attack3, dashAttack);
        dodge.speed = 2.5f;
        overdrive.SetConditionals(false, dashAttack, dodge, attack1, attack2, attack3, jump, death, stun);
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
        currentStamina = maxStamina;
        currentInvulnerableTimer = invulnerableTimer;
        currentDashAttackTimer = dashAttackTimer;
        currentDashAttackCooldown = dashAttackCooldown;
        currentOverdriveTimer = overdriveTimer;
        currentOverdriveCooldown = overDriveCooldown;

        initialHealthBarPos = healthBarFill.GetComponent<Transform>().localPosition;
        initialHealthBarXpos = healthBarFill.GetComponent<Transform>().localPosition.x;
        initialHealthBarXScale = healthBarFill.GetComponent<Transform>().localScale.x;

        initialStaminaBarPos = staminaBarFill.GetComponent<Transform>().localPosition;
        initialStaminaBarXpos = staminaBarFill.GetComponent<Transform>().localPosition.x;
        initialStaminaBarXScale = staminaBarFill.GetComponent<Transform>().localScale.x;
        walkSoundTime = walkStepsInterval;
        InitAnimStates();
    }

    // Update is called once per frame
    void Update()
    {



        //testing respawn
        if (Input.GetKey(KeyCode.R))
        {
            Console.WriteLine("Test Respawn");
            player.localPosition = new vec3(spawnPoint.localPosition);
            //player.localRotation = new vec3(spawnPoint.localRotation);
            //SceneManager.LoadScene("LevelPlay2");
        }

        //death animation timer
        if (startDeathAnimationCountdown)
        {
            currentAnimationTimer -= Time.deltaTime;
            if (currentAnimationTimer <= 0.2)
            {
                currentAnimationTimer = animationTimer;
                startDeathAnimationCountdown = false;
                //animator.Pause();//pause the death animation to prevent it from returning to idle animation
                Respawn();
                //SceneManager.LoadScene("LevelPlay2");
            }
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
        //testing heal
        if(Input.GetKeyDown(KeyCode.Y))
        {
            HealHealth(1);
        }
        vec3 dir = GetDirection();
        vec3 movement = dir * MoveSpeed * Time.deltaTime;

        bool isMoving = dir != vec3.Zero && !_isDashAttacking;
        bool moved = false;

        if(!isMoving)
        {
            SetState("Run", false);
            SetState("Sprint", false);
        }


        //melee attack check
        if (IsAttacking)
        {
            SetState("DashAttack", false);
            dir = vec3.Zero;
            if (currentAttackTimer / attackTimer < 0.2f)
                movement = PlayerModel.back * attackMoveSpeed * Time.deltaTime;
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
        //dash attack check
        if(_isDashAttacking)
        {
            startDashCooldown = true;
            //CC.force = PlayerModel.back * dashAttackSpeed;//dash player forward

            //selectedWeaponCollider.transform.localPosition = new vec3(transform.localPosition + PlayerModel.back * 0.6f);
            //attackLight.SetActive(true);
            //selectedWeaponCollider.SetActive(true);//enable the weapon collider

            //movement = CC.force;//set the movement to be the dash force

            //movement = transform.forward * dashAttackSpeed;//dash player forward
            currentDashAttackTimer -= Time.deltaTime;

            if(currentDashAttackTimer > 0.5f)
            {
                CC.force = PlayerModel.back * dashAttackSpeed;//dash player forward

                selectedWeaponCollider.transform.localPosition = new vec3(transform.localPosition + PlayerModel.back * 0.6f);
                attackLight.SetActive(true);
                selectedWeaponCollider.SetActive(true);//enable the weapon collider

                movement = CC.force;//set the movement to be the dash force
            }
            if(currentDashAttackTimer <= 0.5f)
            {
                movement = vec3.Zero;
                selectedWeaponCollider.transform.localPosition = new vec3(10000);
                attackLight.SetActive(false);//disable weapon collider
            }
            if(currentDashAttackTimer <= 0)
            {
                SetState("DashAttack", false);
                //selectedWeaponCollider.transform.localPosition = new vec3(10000);
                //attackLight.SetActive(false);//disable weapon collider

                //movement = vec3.Zero;
                _isDashAttacking = false;
                currentDashAttackTimer = dashAttackTimer;

            }
        }
        //ensure the dash attack animation is not snapped while performing dash attack
        if(startDashCooldown)
        {
            currentDashAttackCooldown -= Time.deltaTime;
            if(currentDashAttackCooldown <= 0)
            {
                //SetState("DashAttack", false);
                startDashCooldown = false;
                currentDashAttackCooldown = dashAttackCooldown;

            }
        }
        //dodge check
        if(isDodging)
        {
            startDodgeCooldown = true;
            CC.force = PlayerModel.back * dodgeSpeed;//dash player forward
            movement = CC.force;//set the movement to be the dash force
            currentDodgeTimer -= Time.deltaTime;
            if(currentDodgeTimer <= 0)
            {
                movement = vec3.Zero;
                isDodging = false;
                currentDodgeTimer = dodgeTimer;
            }
        }
        if(startDodgeCooldown)
        {
            currentDodgeCooldown -= Time.deltaTime;
            if(currentDodgeCooldown <= 0)
            {
                SetState("Dodge", false);
                startDodgeCooldown = false;
                currentDodgeCooldown = dodgeCooldown;
            }
        }
        //overdrive check
        if(_isOverdrive)
        {
            isMoving = false;
            startOverdriveCooldown = true;
            currentOverdriveTimer -= Time.deltaTime;
            overDriveCollider.transform.localPosition = new vec3(transform.localPosition);
            overDriveCollider.SetActive(true);
            attackLight.transform.localPosition = new vec3(transform.localPosition);
            attackLight.SetActive(true);
            
            if (currentOverdriveTimer <= 0)
            {
                SetState("Overdrive", false);
                overDriveCollider.transform.localPosition = new vec3(10000);
                overDriveCollider.SetActive(false);
                attackLight.SetActive(false);
                _isOverdrive = false;
                currentOverdriveTimer = overdriveTimer;
            }
        }
        if(startOverdriveCooldown)
        {
            currentOverdriveCooldown -= Time.deltaTime;
            if(currentOverdriveCooldown <= 0)
            {
                //SetState("Overdrive", false);
                startOverdriveCooldown = false;
                currentOverdriveCooldown = overDriveCooldown;
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
            //DASH ATTACK
            //if(Input.GetMouseDown(1) && !_isDashAttacking && !IsAttacking && !startDashCooldown)
            if (Input.GetKeyDown(KeyCode.LeftAlt) && !_isDashAttacking && !startDashCooldown && currentStamina >= dashAttackStamina)
            {
                Console.WriteLine("DashAttack");
                UseStamina(dashAttackStamina);
                AudioManager.instance.playerAttack.Play();
                _isDashAttacking = true;
                SetState("Run", false);
                SetState("Sprint", false);
                SetState("DashAttack", true);
            }
            //DODGE
            if(Input.GetKey(KeyCode.C) && !isDodging && !startDodgeCooldown && !_isOverdrive && currentStamina >= dodgeStamina)
            {
                Console.WriteLine("Dodging");
                UseStamina(dodgeStamina);
                isDodging = true;
                SetState("Run", false);
                SetState("Sprint", false);
                SetState("Dodge", true);
            }
            //OVERDRIVE
            if(Input.GetKeyDown(KeyCode.Q) && !_isOverdrive && !_isDashAttacking && !IsAttacking && !startDashCooldown && !startOverdriveCooldown && currentStamina >= overDriveStamina)
            {
                AudioManager.instance.playerOverdrive.Play();
                UseStamina(overDriveStamina);
                Console.WriteLine("Overdrive");
                _isOverdrive = true;
                SetState("Run", false);
                SetState("Sprint", false);
                SetState("Dodge", false);;
                SetState("Overdrive", true);

            }

            bool combo = IsAttacking && currentAttackTimer/attackTimer > animCancelPercentage;
            if (Input.GetMouseDown(0) && (combo || !IsAttacking) && currentStamina >= attackStamina)
            {
                UseStamina(attackStamina);

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
            else if (Input.GetKeyDown(KeyCode.Space) && !IsAttacking && !_isOverdrive && !_isDashAttacking && currentStamina >= jumpStamina)
            {
                SetState("Jump", true);
                UseStamina(jumpStamina);
                AudioManager.instance.jumpVoice.Play();
                movement += vec3.UnitY * JumpSpeed;
            }
            else if (!IsAttacking)
            {
                SetState("Jump", false);
                //SPRINT
                if (Input.GetKey(KeyCode.LeftShift) && isMoving && currentStamina >= sprintStamina)
                {
                    UseStamina(sprintStamina);
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


        CC.Move(movement);


        //attacking

        animationManager.UpdateState();
    }

    public void Respawn()
    {
        Console.WriteLine("Respawn");
        isDead = false;
        healthStaminaCanvas.SetActive(true);
        SetState("Death", false);
        //animator.Play();
        player.localPosition = new vec3(spawnPoint.localPosition);
        HealHealth(maxHealth);
        healthBarFill.GetComponent<Transform>().localPosition = initialHealthBarPos;
        UpdatehealthBar();
        staminaBarFill.GetComponent<Transform>().localPosition = initialStaminaBarPos;
        UpdateStaminaBar();


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
        if(currentHealth == maxHealth)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.65f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(-0.2f, -0.035f, -1f);
        }
        if(currentHealth == 3)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.7f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(-0.15f, -0.035f, -1f);
        }
        if (currentHealth == 2)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.74f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(-0.11f, -0.035f, -1f);
        }
        if (currentHealth == 1)
        {
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.8f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(-0.05f, -0.035f, -1f);
        }
        if(currentHealth <= 0)
        {
            Console.WriteLine("GameOver");
            isDead = true;
            healthStaminaCanvas.SetActive(false);
            startDeathAnimationCountdown = true;
            currentHealth = 0;
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.8f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(-0f, -0.035f, -1f);
        }
    }

    public void UpdateStaminaBar()
    {

        float scaleFactor = (float)currentStamina / (float)maxStamina;
        float newXScale = initialStaminaBarXScale * scaleFactor;
        float xOffset = (initialStaminaBarXScale - newXScale) * 0.5f;
        //vec3 currentPos = staminaBarFill.GetComponent<Transform>().localPosition;
        vec3 currentPos = staminaBarPos.localPosition;
        vec3 currentScale = staminaBarFill.GetComponent<Transform>().localScale;
        currentPos.x = initialStaminaBarXpos - xOffset;
        currentScale.x = newXScale;
        staminaBarFill.GetComponent<Transform>().localPosition = currentPos;
        staminaBarFill.GetComponent<Transform>().localScale = currentScale;
    }

    public float UseStamina(float amount)
    {
        //if player has enough stamina, reduce the stamina amount used
        if (currentStamina - amount >= 0)
        {
            currentStamina -= amount;
            UpdateStaminaBar();
            //staminaBar.value = currentStamina;
            //lerp timer reset the lerp effect
            //lerpTimer = 0f;

            //checks if player is currently regenerating stamina, this prevents player from continuing
            //regenerating while perfroming actions that consume stamina
            if (regen != null)
            {
                StopCoroutine(regen);
            }
            if (currentStamina - amount <= 0)
            {
                //Debug.Log("Not enough stamina");
                Console.WriteLine("Not enough stamina");
                UpdateStaminaBar();
            }


            regen = StartCoroutine(RegenStamina());
        }
        else
        {
            //Debug.Log("Not enough stamina");
            Console.WriteLine("Not enough stamina");
            UpdateStaminaBar();

        }

        return amount;
    }

    //gradually regenerate stamina 
    private IEnumerator RegenStamina()
    {
        Console.WriteLine("Regenerate Stamina");
        yield return new WaitForSeconds(timeBeforeRegen);

        regenRate = 60f;
        while (currentStamina < maxStamina)
        {
            //changed 100 to 60 for faster regen
            currentStamina += maxStamina / regenRate;
            //staminaBar.value = currentStamina;
            UpdateStaminaBar();
            yield return regenTick;
        }

        //if (ThirdPersonController.instance.overdriveRegen == true)
        //{
        //    regenRate = 10f;
        //    while (currentStamina < maxStamina)
        //    {
        //        changed 100 to 60 for faster regen
        //        currentStamina += maxStamina / regenRate;
        //        staminaBar.value = currentStamina;
        //        yield return regenTick;
        //    }
        //}
        //else
        //{
        //    regenRate = 60f;
        //    while (currentStamina < maxStamina)
        //    {
        //        changed 100 to 60 for faster regen
        //        currentStamina += maxStamina / regenRate;
        //        staminaBar.value = currentStamina;
        //        yield return regenTick;
        //    }
        //}

        regen = null;
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

    public void HealHealth(float amount)
    {
        currentHealth += amount;
        UpdatehealthBar();

        if(currentHealth > maxHealth)
        {
            currentHealth = maxHealth;
        }
        if(currentHealth == maxHealth)
        {
            Console.WriteLine("Health is Full");
        }
    }


    //Handle Movement Input
    vec3 GetDirection()
    {
        vec3 dir = vec3.Zero;
        if(!_isOverdrive && !isDodging && !_isDashAttacking)
        {
            if (Input.GetKey(KeyCode.W))
            {
                dir -= (PlayerCamera.forward);
            }


            if (Input.GetKey(KeyCode.A))
            {
                dir -= (PlayerCamera.right);
            }


            if (Input.GetKey(KeyCode.S))
            {
                dir += PlayerCamera.forward;
            }


            if (Input.GetKey(KeyCode.D))
            {
                dir += (PlayerCamera.right);
            }
        }
       

        dir.y = 0f;
        return dir.NormalizedSafe;
    }

    public void UpdateRotation(vec3 dir)
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
        if(GetTag(rb) == "SpinningPropeller")
        {
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

    void OnCollisionExit(PhysicsComponent rb)
    {

    }

}
