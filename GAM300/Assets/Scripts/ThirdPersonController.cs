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

    public bool stopJump = false;
    private bool jumpLandSound = false;


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

    //public InstantDeath instantDeath;

    List<vec3> pos = new List<vec3>();

    public float RotationSpeed = 15;

    public AudioSource audioSource;
    int jumpAudioRotation = 0;
    int damageAudioRotation = 0;
    int dodgeRollAudioRotation = 0;
    bool dodgeSound = true;

    AnimationStateMachine animationManager;

    public bool _isAttacking = false;
    public bool _isDashAttacking = false;
    public bool _isOverdrive = false;

    public int checkpointIndex = -1;
    public bool isAtCheckpoint = false;

    public vec3 spawnPoint;

    //public MeshRenderer doorTestMesh;

    public bool cutscene = false;
    public bool _isSprinting = false;

    //new particles
    public GameObject playerRespawningVFX;
    public GameObject playerHealthVFX;
    public GameObject playerStaminaVFX;
    public GameObject playerMaxVFX;

    //overdrive QQ
    public Transform overDriveUI;
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
                selectedWeaponCollider.transform.position = new vec3(10000);
                attackLight.SetActive(false);
            }
            else
            {
                selectedWeaponCollider.transform.rotation = new vec3(PlayerModel.rotation);
                SetState("Attack" + comboCount, true);
                playerSounds.PlayerSlashAttack.Play();
                playerSounds.Spark.Play();
                playerSounds.PlayerAttack.Play();
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
    private bool updateInvulnerability = false;
    public bool isDead = false;

    float maxAirTime = 2f;
    float currentAirTime = 0;

    public float colliderDist = 0;
    public float maxColliderDist = 1f;

    //health bar
    public GameObject healthBarFill;
    public Transform healthBar;
    public GameObject healthStaminaCanvas;
    vec3 initialHealthBarPos;
    bool lowHealthSound = false;
    bool playingLowHealthSound = false;

    //stamina bar
    public float maxStamina = 100f;
    public float currentStamina;
    public GameObject staminaBarFill;
    public Transform staminaBar;
    private Coroutine regen;
    public float timeBeforeRegen = 1.5f;
    public float regenRate = 60f;//the higher the number, the slower the regen is
    private WaitForSeconds regenTick = new WaitForSeconds(0.1f);
    vec3 initialStaminaBarPos;
    public float sprintStamina = 0.5f;
    public float jumpStamina = 10f;
    public float attackStamina = 10f;
    public float dodgeStamina = 20f;
    public float dashAttackStamina = 30f;
    public float overDriveStamina = 40f;

    //overdrive bar
    //public float maxOverdrive = 10f;
    //public float currentOverdrive = 0;
    public GameObject overDriveBar;
    public Transform overDriveTransform;
    public GameObject overDriveBarFill;
    public GameObject overDriveVFX;
    vec3 initialOverdriveBarPos;
    //for overdrive chip purposes
    public bool isOverdriveEnabled = false;
    //used to check if in overdrive for dmg boost, regen and stamina reset
    public bool currentlyOverdriven = false;
    public bool playOverdrivePowerUpOnce = true;
    public float maxOverdriveCharge = 15f;
    public float currentOverdriveCharge = 0f;
    public float currentOverdriveHealthTimer = 0f;
    public float chargeOverdriveTimer = 0f;
    public float currentOverdriveScaleTimer = 0f;
    public bool overDriveUIScaleBool = true;

    public Animator animator;
    public bool startDeathAnimationCountdown = false;
    float animationTimer = 3.18f;
    public float currentAnimationTimer;

    public float attack1duration = 1f;
    public float attack2duration = 2f;
    public float attack3duration = 2f;

    public float animCancelPercentage = 1f;

    int comboCount = 1;

    public PlayerAudioManager playerSounds;
    private bool noInterpolate = true;

    public ParticleComponent dashVFX;
    public bool healthRegenEnabled = false;

    bool _wasMoving = false;
    bool wasMoving
    {
        get { return _wasMoving; }
        set
        {
            if (value == false && _wasMoving)
            {
                walkSoundTimer = 0f;
                playerSounds.PlayerFootstep.Play();
            }

            _wasMoving = value;
            if (_wasMoving)
            {
                walkSoundTimer += Time.deltaTime;
            }
            if (walkSoundTimer > walkSoundTime)
            {
                playerSounds.PlayerFootstep.Play();
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
        AnimationState dashAttack = animationManager.GetState("DashAttack");
        AnimationState sprint = animationManager.GetState("Sprint");
        AnimationState run = animationManager.GetState("Run");
        AnimationState dodge = animationManager.GetState("Dodge");
        AnimationState overdrive = animationManager.GetState("Overdrive");
        AnimationState idle = animationManager.GetState("Idle");
        //Lowest Precedence

        stun.SetConditionals(false, death);
        falling.SetConditionals(false, death, stun);
        jump.SetConditionals(false, death,stun);
        attack1.SetConditionals(false, jump, death, stun);
        attack2.SetConditionals(false, jump, death, stun);
        attack3.SetConditionals(false, jump, death, stun);
        attack1.speed = 2.2f;
        attack2.speed = 2.8f;
        attack3.speed = 3.1f;
        sprint.SetConditionals(true, run);
        sprint.SetConditionals(false, attack1, jump, death, stun);
        run.SetConditionals(false, sprint, attack1, jump, death, stun);
        dashAttack.SetConditionals(false, jump, death, stun, sprint, run, attack1, attack2, attack3);
        dashAttack.speed = 2.5f;
        dodge.SetConditionals(false, dashAttack, jump, death, stun, attack1, attack2, attack3, dashAttack);
        dodge.speed = 2.5f;
        overdrive.SetConditionals(false, dashAttack, dodge, attack1, attack2, attack3, jump, death, stun);
        SetState("Idle", true);
    }


    void Start()
    {
        //Material mat = doorTestMesh.material;
        //mat.color = vec4.Ones;
        //reference check

        if (playerStaminaVFX == null)
        {
            Console.WriteLine("Missing playerStaminaVFX reference in ThirdPersonController script");
        }

        if (playerMaxVFX == null)
        {
            Console.WriteLine("Missing playerMaxVFX reference in ThirdPersonController script");
        }

        if (playerHealthVFX == null)
        {
            Console.WriteLine("Missing playerHealthVFX reference in ThirdPersonController script");
        }


        if (dashVFX == null)
        {
            Console.WriteLine("Missing dashVFX reference in ThirdPersonController script");
        }

        if (overDriveUI == null)
        {
            Console.WriteLine("Missing overDriveUI reference in ThirdPersonController script");
        }


        if (overDriveTransform == null)
        {
            Console.WriteLine("Missing OverdriveBarTransform reference in ThirdPersonController script");
        }

        if (overDriveBar == null)
        {
            Console.WriteLine("Missing Overdrive Bar reference in ThirdPersonController script");
        }

        if (PlayerCamera == null)
        {
            Console.WriteLine("Missing Player camere reference in ThirdPersonController script");
        }
        if(PlayerModel == null)
        {
            Console.WriteLine("Missing PlayerModel reference in ThirdPersonController script");
        }
        if(player == null)
        {
            Console.WriteLine("Missing player reference in ThirdPersonController script");
        }
        if(attackLight == null)
        {
            Console.WriteLine("Missing attack light reference in ThirdPersonController script");
        }
        if(overDriveCollider == null)
        {
            Console.WriteLine("Missing overDriveCollider reference in ThirdPersonController script");
        }
        if(playerWeaponCollider1 == null)
        {
            Console.WriteLine("Missing playerWeaponCollider1 reference in ThirdPersonController script");
        }
        if(playerWeaponCollider2 == null)
        {
            Console.WriteLine("Missing playerWeaponCollider2 reference in ThirdPersonController script");
        }
        if(playerWeaponCollider3 == null)
        {
            Console.WriteLine("Missing playerWeaponCollider3 reference in ThirdPersonController script");
        }
        if(audioSource == null)
        {
            Console.WriteLine("Missing audioSource reference in ThirdPersonController script");
        }
        if(healthBarFill == null)
        {
            Console.WriteLine("Missing healthBarFill reference in ThirdPersonController script");
        }
        if(healthStaminaCanvas == null)
        {
            Console.WriteLine("Missing healthStaminaCanvas reference in ThirdPersonController script");
        }
        if(staminaBarFill == null)
        {
            Console.WriteLine("Missing staminaBarFill reference in ThirdPersonController script");
        }
        if (overDriveBarFill == null)
        {
            Console.WriteLine("Missing overDriveBarFill reference in ThirdPersonController script");
        }
        if (animator == null)
        {
            Console.WriteLine("Missing animator reference in ThirdPersonController script");
        }

        StartCoroutine(Invulnerability());

        playerSounds = PlayerAudioManager.instance;
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

        initialHealthBarPos = healthBarFill.transform.localPosition;
        initialStaminaBarPos = staminaBarFill.transform.localPosition;
        initialOverdriveBarPos = overDriveBarFill.transform.localPosition;
        walkSoundTime = walkStepsInterval;
        InitAnimStates();
        spawnPoint = transform.position;

        //Overdrive start
        if (isOverdriveEnabled == true)
        {
            overDriveBar.SetActive(true);
            UpdateOverdriveBar();
        }
        else
        {
            overDriveBar.SetActive(false);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager.instance.paused)
            return;

        if (ThirdPersonCamera.instance.cutscene)
        {
            animator.SetState("Idle");
            return;
        }


        //testing respawn
        if (Input.GetKey(KeyCode.R))
        {
            //Console.WriteLine("Test Respawn");
            player.localPosition = spawnPoint;
            //player.localRotation = new vec3(spawnPoint.localRotation);
            //SceneManager.LoadScene("LevelPlay2");
        }

        //death animation timer
        if (startDeathAnimationCountdown)
        {
            playerRespawningVFX.transform.position = new vec3(transform.localPosition.x, transform.localPosition.y - 1, transform.localPosition.z);
            playerRespawningVFX.SetActive(true);
            currentAnimationTimer -= Time.deltaTime;
            if (currentAnimationTimer <= 0.2)
            {
                //ie numerator to delay playeRespawnVFX setactive(false0
                currentAnimationTimer = animationTimer;
                startDeathAnimationCountdown = false;
                //animator.Pause();//pause the death animation to prevent it from returning to idle animation
                Respawn();
                //SceneManager.LoadScene("LevelPlay2");
            }
        }

        if (isDead || GetState("Stun")) return;

        if (CC.velocity.y > JumpSpeed)
        {
            CC.velocity.y = JumpSpeed;
        }

        vfxChecker();

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

        if (lowHealthSound)
        {
            playerSounds.LowHealthSound.Play();
            playerSounds.LowHealthHeartbeatSound.Play();
            playingLowHealthSound = true;
        }
        else
        {
            playerSounds.LowHealthSound.Pause();
            playerSounds.LowHealthHeartbeatSound.Pause();
            playingLowHealthSound = false;
        }

        if (currentStamina >= 20f)
        {
            playerSounds.LowStaminaSound.Pause();
        }

        if (currentOverdriveCharge >= maxOverdriveCharge && isOverdriveEnabled == true)
        {
            overDriveUI.gameObject.SetActive(true);
        }
        else
        {
            overDriveUI.gameObject.SetActive(false);
        }

        if (overDriveUI.gameObject.activeSelf == true)
        {
            currentOverdriveScaleTimer += Time.deltaTime;

            if (currentOverdriveScaleTimer >= 0.25f)
            {
                currentOverdriveScaleTimer = 0f;
                overDriveUIScaleBool = !overDriveUIScaleBool;
            }

            if (overDriveUIScaleBool == true)
            {
                vec3 target = vec3.Lerp(overDriveUI.localScale, 0.307f, Time.deltaTime * 2);
                overDriveUI.localScale = target;
            }
            else
            {
                vec3 target = vec3.Lerp(overDriveUI.localScale, 0.203f, Time.deltaTime * 2);
                overDriveUI.localScale = target;
            }

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
            //dir = vec3.Zero;
            if (currentAttackTimer / attackTimer < 0.2f)
                movement = PlayerModel.forward * attackMoveSpeed * Time.deltaTime;
            else
            {
                if (currentAttackTimer / attackTimer > 0.5f) 
                {
                    selectedWeaponCollider.transform.position = new vec3(10000);
                    attackLight.SetActive(false);
                }
                else if (currentAttackTimer / attackTimer > 0.3f )
                {
                    selectedWeaponCollider.transform.position = new vec3(transform.position + PlayerModel.forward * 1.1f);
                    attackLight.SetActive(true);
                    selectedWeaponCollider.SetActive(true);//enable the weapon collider
                }
                movement = vec3.Zero;
            }
            attackLight.transform.localPosition = new vec3(selectedWeaponCollider.transform.position);
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
                CC.force = PlayerModel.forward * dashAttackSpeed * Time.deltaTime;//dash player forward

                selectedWeaponCollider.transform.position = new vec3(transform.position + PlayerModel.forward * 1.1f);
                attackLight.SetActive(true);
                selectedWeaponCollider.SetActive(true);//enable the weapon collider

                movement = CC.force;//set the movement to be the dash force
            }
            if(currentDashAttackTimer <= 0.5f)
            {
                movement = vec3.Zero;
                selectedWeaponCollider.transform.position = new vec3(10000);
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
            if (dodgeSound)
            {
                Random rd = new Random();
                dodgeRollAudioRotation = rd.Next(0, 1);
                dodgeSound = false;

                //Plays Dodge Roll Sound
                switch (dodgeRollAudioRotation)
                {
                    case 0:
                        playerSounds.DodgeRoll1.Play();
                        break;
                    case 1:
                        playerSounds.DodgeRoll2.Play();
                        break;
                }
            }
            startDodgeCooldown = true;
            CC.force = PlayerModel.forward * dodgeSpeed * Time.deltaTime;//dash player forward
            movement = CC.force;//set the movement to be the dash force
            currentDodgeTimer -= Time.deltaTime;

            if (currentDodgeTimer <= 0)
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
                dodgeSound = true;
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
            overDriveVFX.transform.position = new vec3(transform.localPosition.x, transform.localPosition.y -2, transform.localPosition.z);
            overDriveVFX.SetActive(true);

            //this stops the animation only
            if (currentOverdriveTimer <= 0)
            {
                SetState("Overdrive", false);
                overDriveCollider.transform.localPosition = new vec3(10000);
                overDriveCollider.SetActive(false);
                attackLight.SetActive(false);
                overDriveVFX.transform.localPosition = new vec3(1000);
                overDriveVFX.SetActive(false);
                _isOverdrive = false;
                currentOverdriveTimer = overdriveTimer;
            }
        }

        //cooldown changed to OverdriveDuration
        if(startOverdriveCooldown)
        {
            //change cooldown to duration
            currentOverdriveCooldown -= Time.deltaTime;

            //timer to reduce charge here
            chargeOverdriveTimer += Time.deltaTime;

            if (chargeOverdriveTimer >= 0.95f)
            {
                chargeOverdriveTimer = 0f;
                currentOverdriveCharge -= 1.5f;
                if (currentOverdriveCharge <= 0f)
                {
                    currentOverdriveCharge = 0f;
                }
                UpdateOverdriveBar();
            }

            //health regen code
            currentOverdriveHealthTimer += Time.deltaTime;
            if(currentOverdriveHealthTimer >= 1.8f)
            {
                currentOverdriveHealthTimer = 0f;
                HealtHealthOverTime();
            }

            if(currentOverdriveCooldown <= 0)
            {
                //SetState("Overdrive", false);
                startOverdriveCooldown = false;
                currentOverdriveCooldown = overDriveCooldown;
                UpdateOverdriveBar();

                //remove regen, stamina reset and double dmg here
                healthRegenEnabled = false;
                currentlyOverdriven = false;
            }
        }

        //invulnerability
        if (CC.isGrounded)
        {
            //Console.WriteLine("\nGROUNDED!");
            if (GetState("Falling"))
            {
                SetState("Falling", false);
                playerSounds.PlayerFootstep.Play();
            }
            //DASH ATTACK
            //if(Input.GetMouseDown(1) && !_isDashAttacking && !IsAttacking && !startDashCooldown)
            if (Input.GetMouseDown(1) && !_isDashAttacking && !startDashCooldown && !isDodging && currentStamina >= dashAttackStamina)
            {
                dashVFX.gameObject.SetActive(true);
                //Console.WriteLine("DashAttack");
                IsAttacking = false;
                UseStamina(dashAttackStamina);
                playerSounds.DashAttack.Play();
                playerSounds.PlayerAttack.Play();
                _isDashAttacking = true;

                // No interpolation for dash attack
                noInterpolate = false;

                // Snap the direction to wasd keys
                if (Input.GetKey(KeyCode.W))
                {
                    dir = (PlayerCamera.back);
                }
                else if (Input.GetKey(KeyCode.A))
                {
                    dir = (PlayerCamera.left);
                }
                else if (Input.GetKey(KeyCode.S))
                {
                    dir = (PlayerCamera.forward);
                }
                else if (Input.GetKey(KeyCode.D))
                {
                    dir = (PlayerCamera.right);
                }

                SetState("Run", false);
                SetState("Sprint", false);
                SetState("Attack1", false);
                SetState("Attack2", false);
                SetState("Attack3", false);
                SetState("DashAttack", true);
            }
            //DODGE
            if(Input.GetKey(KeyCode.LeftControl) && !isDodging && !startDodgeCooldown && !_isOverdrive && !_isDashAttacking && currentStamina >= dodgeStamina)
            {
                //Console.WriteLine("Dodging");
                UseStamina(dodgeStamina);
                isDodging = true;
                updateInvulnerability = true;

                // No interpolation for dodging
                noInterpolate = false;

                // Snap the direction to wasd keys
                if (Input.GetKey(KeyCode.W))
                {
                    dir = (PlayerCamera.back);
                }
                else if (Input.GetKey(KeyCode.A))
                {
                    dir = (PlayerCamera.left);
                }
                else if (Input.GetKey(KeyCode.S))
                {
                    dir = (PlayerCamera.forward);
                }
                else if (Input.GetKey(KeyCode.D))
                {
                    dir = (PlayerCamera.right);
                }

                SetState("Run", false);
                SetState("Sprint", false);
                SetState("Attack1", false);
                SetState("Attack2", false);
                SetState("Attack3", false);
                SetState("Dodge", true);
            }
            //OVERDRIVE
            if(Input.GetKeyDown(KeyCode.Q) && !_isOverdrive && !_isDashAttacking && !IsAttacking && !startDashCooldown && currentOverdriveCharge == maxOverdriveCharge && isOverdriveEnabled == true && currentlyOverdriven == false)
            {
                Inventory.instance.useStaminaForScript();
                healthRegenEnabled = true;
                overDriveUI.gameObject.SetActive(false);
                playerSounds.PlayerOverdrive.Play();
                playerSounds.OverdriveVFXSound.Play();
                //Overdrive doesn't need stamina to use
                //UseStamina(overDriveStamina);
                //Console.WriteLine("Overdrive");

                //set the charge to 0, so it can't be used again immediately
                //currentOverdriveCharge = 0;

                //reset powerupPlayOnce
                playOverdrivePowerUpOnce = true;

                //reset health regen timer so it doesn't stack in the next overdrive mode use.
                currentOverdriveHealthTimer = 0f;

                //reset stamina
                currentStamina = maxStamina;
                UpdateStaminaBar();

                currentlyOverdriven = true;
                _isOverdrive = true;
                SetState("Run", false);
                SetState("Sprint", false);
                SetState("Dodge", false);
                SetState("Overdrive", true);

            }

            bool combo = IsAttacking && currentAttackTimer/attackTimer > animCancelPercentage;
            if (Input.GetMouseDown(0) && (combo || !IsAttacking))
            {
                //Normal attacks won't use stamina.
                //UseStamina(attackStamina);

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
            else if (Input.GetKeyDown(KeyCode.Space) && !IsAttacking && !_isOverdrive && !_isDashAttacking && !isDodging)
            {
                //Console.WriteLine("JUMP KEY PRESSED!");
                StartCoroutine(StopJump());
                SetState("Jump", true);
                playerSounds.JumpOffGroundSound.Play();
                jumpLandSound = true;

                //Jump will not require stamina
                //UseStamina(jumpStamina);

                Random rd = new Random();
                jumpAudioRotation = rd.Next(0, 2);

                switch (jumpAudioRotation)
                {
                    case 0:
                        playerSounds.JumpVoice.Play();
                        break;
                    case 1:
                        playerSounds.JumpVoice2.Play();
                        break;
                    case 2:
                        playerSounds.JumpVoice3.Play();
                        break;
                }
                
                movement += vec3.UnitY * JumpSpeed;
            }
            else if (!IsAttacking)
            {
                //Console.WriteLine("Stopped Jumping");
                if (stopJump)
                {
                    SetState("Jump", false);
                    if (jumpLandSound == true)
                    {
                        playerSounds.HitGroundSound.Play();
                        jumpLandSound = false;
                    }
                }
                    
                //SPRINT
                if (Input.GetKey(KeyCode.LeftShift) && isMoving && currentStamina >= sprintStamina)
                {
                    _isSprinting = true;
                    UseStamina(sprintStamina);
                    walkSoundTime = runStepsInterval;
                    SetState("Sprint", true);
                    SetState("Run", true);
                    movement *= sprintModifier;
                }
                else
                {
                    _isSprinting = false;
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
            //Console.WriteLine("NOT GROUNDED!");
            if (animationManager.GetState("Jump").state)
            {
                //Console.WriteLine("Jump");
                if (currentAirTime >= maxAirTime * .4f)
                {
                    //Console.WriteLine("Air time");
                    SetState("Falling", true);
                }

            }
            else if (currentAirTime >= maxAirTime * .2f)
            {
                SetState("Falling", true);
            }
            currentAirTime += Time.deltaTime;
            movement += vec3.UnitY * -Gravity;
        }
        wasMoving = moved;
        if (noInterpolate) // Update rotation of the player as normal
        {
            UpdateRotation(dir);
        }
        else
        {
            UpdateRotationNoInterpolate(dir);
            noInterpolate = true;
        }
        noInterpolate = true;
        CC.Move(movement);


        //attacking
    }

    void LateUpdate()
    {
        if (cutscene)
            return;
        animationManager.UpdateState();
    }

    public void enableOverdrive()
    {
        overDriveBar.SetActive(true);
        isOverdriveEnabled = true;
    }

    public void vfxChecker()
    {
        //checks whether its dashing vfx should be enabled
        if (_isDashAttacking == false)
        {
            dashVFX.gameObject.SetActive(false);
        }

        //checks whether healthregen sfx should be enabled
        if (healthRegenEnabled == true)
        {
            playerHealthVFX.SetActive(true);
        }
        else
        {
            playerHealthVFX.SetActive(false);
        }

    }

    IEnumerator delayRespawnVFX()
    {
        yield return new WaitForSeconds(2f);

        playerRespawningVFX.SetActive(false);
    }

    public void Respawn()
    {
        PlayerAudioManager.instance.playerRespawn.Play();
        //Console.WriteLine("Respawn");
        isDead = false;
        healthStaminaCanvas.SetActive(true);
        SetState("Death", false);
        //animator.Play();
        player.localPosition = spawnPoint;
        playerRespawningVFX.transform.position = new vec3(transform.localPosition.x, transform.localPosition.y - 1, transform.localPosition.z);
        HealHealth(maxHealth);
        healthBarFill.transform.localPosition = initialHealthBarPos;
        UpdatehealthBar();
        staminaBarFill.transform.localPosition = initialStaminaBarPos;
        UpdateStaminaBar();
        overDriveBarFill.transform.localPosition = initialOverdriveBarPos;
        UpdateOverdriveBar();

        StartCoroutine(delayRespawnVFX());
    }

    public void restoreStamina(float staminaPackAmount)
    {
        currentStamina += staminaPackAmount;
        if (currentStamina > maxStamina)
        {
            currentStamina = maxStamina;
        }

        UpdateStaminaBar();
    }

    public void UpdateOverdriveBar()
    {
        vec3 overDriveScale = overDriveTransform.localScale;
        overDriveScale.x = currentOverdriveCharge / maxOverdriveCharge;
        overDriveTransform.localScale = overDriveScale;
    }

    public void UpdatehealthBar()
    {
        vec3 hpScale = healthBar.localScale;
        hpScale.x = currentHealth / maxHealth;
        healthBar.localScale = hpScale;
        if (currentHealth <= 0)
        {
            //Console.WriteLine("GameOver");
            isDead = true;
            healthStaminaCanvas.SetActive(false);
            startDeathAnimationCountdown = true;
            currentHealth = 0;
            //healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.8f, 0.857f, 3f);
            //healthBarFill.GetComponent<Transform>().localScale = new vec3(-0f, -0.035f, -1f);
        }
    }

    public void UpdateStaminaBar()
    {
        vec3 staminaScale = staminaBar.localScale;
        staminaScale.x = currentStamina / maxStamina;
        staminaBar.localScale = staminaScale;
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
                playerSounds.LowStaminaSound.Play();
                //Debug.Log("Not enough stamina");
                //Console.WriteLine("Not enough stamina");
                UpdateStaminaBar();
            }

            regen = StartCoroutine(RegenStamina());
        }
        else
        {
            playerSounds.LowStaminaSound.Play();
            //Debug.Log("Not enough stamina");
            //Console.WriteLine("Not enough stamina");
            UpdateStaminaBar();

        }

        return amount;
    }

    //gradually regenerate stamina 
    private IEnumerator RegenStamina()
    {
        //Console.WriteLine("Regenerate Stamina");
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

    private IEnumerator Invulnerability()
    {
        while (true)
        {
            if (updateInvulnerability)
            {
                isInvulnerable = true;
                yield return new WaitForSeconds(invulnerableTimer);
                isInvulnerable = false;
                SetState("Stun", false);
            }

            updateInvulnerability = false;

            yield return null;
        }
    }

    bool GetState (string stateName)
    {
        return animationManager.GetState(stateName).state;
    }
    void SetState(string stateName, bool value)
    {
        animationManager.GetState(stateName).state = value;
        animationManager.UpdateState(); 
    }


    public void TakeDamage(float amount)
    {
        if (!isInvulnerable)
        {
            IsAttacking = false;
            //dmg noise
            playerSounds.PlayerInjured.Play();

            CombatManager.instance.SpawnHitEffect(gameObject.transform);

            Random rd = new Random();
            damageAudioRotation = rd.Next(0, 1);

            switch (damageAudioRotation)
            {
                case 0:
                    playerSounds.Thump1.Play();
                    break;
                case 1:
                    playerSounds.Thump2.Play();
                    break;
            }

            ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.damagedShakeMag, CombatManager.instance.damagedShakeDur);
            ThirdPersonCamera.instance.SetFOV(CombatManager.instance.damagedShakeMag * 100, CombatManager.instance.damagedShakeDur);
            updateInvulnerability = true;
            currentHealth -= amount;
            if (currentHealth <= 3f && playingLowHealthSound == false)
            {
                lowHealthSound = true;
            }
            UpdatehealthBar();

            if (currentHealth <= 0)
            {
                playerSounds.LowHealthSound.Pause();
                playerSounds.LowHealthHeartbeatSound.Pause();
                //Console.WriteLine("YouDied");
                isDead = true;
                SetState("Death", true);
            }
            else
            {
                SetState("Stun", true);
                animator.Stop();
                animator.Play();
            }
        }   
    }

    public void HealHealth(float amount)
    {
        currentHealth += amount;
        UpdatehealthBar();

        if (currentHealth > 3f)
        {
            //playerSounds.LowHealthHeartbeatSound.Pause();
            //playerSounds.LowHealthSound.Pause();
            lowHealthSound = false;
            playingLowHealthSound = false;
        }

        if(currentHealth > maxHealth)
        {
            currentHealth = maxHealth;
        }
        if(currentHealth == maxHealth)
        {
            //Console.WriteLine("Health is Full");
        }
    }

    public void HealtHealthOverTime()
    {
        HealHealth(1);
        //play audio here
        playerSounds.UseItem.Play();
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

    public void UpdateRotationNoInterpolate(vec3 dir)
    {
        if (dir == vec3.Zero)
        {
            return;
        }

        float angle = (float)Math.Atan2(-dir.x, -dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;

        vec3 rot = (vec3)newQuat.EulerAngles;

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

    public void SetRotation(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;

        float angle = (float)Math.Atan2(-dir.x, -dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;

        vec3 rot = ((vec3)newQuat.EulerAngles);

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
        if (GetTag(rb) == "EnemyAttack" && !isDead)
        {
            EnemyBullet bullet = rb.gameObject.GetComponent<EnemyBullet>();
            if (bullet != null)
                TakeDamage(bullet.damage);
            else
                TakeDamage(1);
        }
        if(GetTag(rb) == "SpinningPropeller" && !isDead)
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

    IEnumerator StopJump()
    {
        stopJump = false;
        yield return null;
        yield return null;
        stopJump = true;
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
