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

    public CharacterController CC;
    public Transform CamYawPivot;
    public Transform CamMovePivot;
    public Transform CamPitchPivot;
    public Transform PlayerModel;
    public Transform player;
    public GameObject playerWeaponCollider;

    List<vec3> pos = new List<vec3>();

    class AnimationManager
    {
        public AnimationManager(Animator _animator)
        {
            animator = _animator;
            animator.SetDefaultState("Idle");
            animator.Play();
        }

        Animator animator;
        Dictionary<string,AnimationState> animationStates = new Dictionary<string,AnimationState>();
        public void UpdateState()
        {
            AnimationState currState = GetState(currentState);
            if (currState != null && currState.stall)
            {
                if (animator.GetProgress() <= 0.95f)
                {
                    return;
                }
                else
                {
                    currState.state = false;
                }
                currentState = "";
            }
            foreach (var pair in animationStates)
            {
                if (pair.Value.canTransition)
                {
                    if (currentState == pair.Key)
                    {
                        if (currState.loop)
                        {
                            animator.SetNextState(currentState);
                        }
                        return;
                    }
                    currentState = pair.Key;
                    animator.SetState(currentState);
                    return;
                }
            }
            if (currentState != "")
            {
                currentState = "";
            }
        }

        public AnimationState GetState(string name)
        {
            if (name == "")
                return null;
            if (!animationStates.ContainsKey(name))
                animationStates.Add(name, new AnimationState());
            return animationStates[name];
        }

        public AnimationState GetCurrentState()
        {
            return animationStates[currentState];
        }

        string currentState = "";
    }

    class AnimationState
    {
        public bool canTransition
        {
            get
            {
                foreach (AnimationState animState in trueConditionals)
                {
                    if (animState.state == false)
                    {
                        return false;
                    }
                }

                foreach (AnimationState animState in falseConditionals)
                {
                    if (animState.state == true)
                    {
                        return false;
                    }
                }

                return state;
            }
        }

        public void SetConditionals(bool condition, params AnimationState[] animationStates)
        {
            foreach (var state in animationStates)
            {
                if (condition)
                {
                    trueConditionals.Add(state);
                }
                else
                {
                    falseConditionals.Add(state);
                }
            }
        }

        public bool stall = false;
        public bool state = false;
        public bool loop = false;
        List<AnimationState> trueConditionals = new List<AnimationState>();
        List<AnimationState> falseConditionals = new List<AnimationState>();
    }

    public float RotationSpeed = 1;

    public AudioSource audioSource;

    AnimationManager animationManager;

    public bool IsAttacking = false;
    public float attackTimer = 1f;
    public float currentAttackTimer;


    public float maxHealth = 4f;
    public float currentHealth;
    public bool isInvulnerable = false;
    public float invulnerableTimer = 1f;
    public float currentInvulnerableTimer;

    //health bar
    public GameObject healthBarFill;
    vec3 initialHealthBarPos;
    float initialHealthBarXpos;
    float initialHealthBarXScale;

    public Animator animator;
    // Start is called before the first frame update
    void InitAnimStates()
    {
        animationManager = new AnimationManager(animator);

        //Highest Precedence
        AnimationState death = animationManager.GetState("Death");
        AnimationState stun = animationManager.GetState("Stun");
        AnimationState jump = animationManager.GetState("Jump");
        AnimationState attack1 = animationManager.GetState("Attack1");
        AnimationState sprint = animationManager.GetState("Sprint");
        AnimationState run = animationManager.GetState("Run");
        //Lowest Precedence

        death.stall = true;
        stun.SetConditionals(false, death);
        stun.stall = true;
        jump.SetConditionals(false, death,stun);
        attack1.SetConditionals(false, jump, death, stun);
        attack1.stall = true;
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

        InitAnimStates();
    }

    // Update is called once per frame
    void Update()
    {
        if (CC.velocity.y > JumpSpeed)
        {
            CC.velocity.y = JumpSpeed;
            Console.WriteLine("Limit velocity");
        }

        //Testing taking damage
        if (Input.GetKeyDown(KeyCode.T))
        {
            TakeDamage(1);
            isInvulnerable = true;
            UpdatehealthBar();
        }

        vec3 dir = GetDirection();
        SetState("Run", dir == vec3.Zero ? false : true);
        UpdateRotation(dir);
        vec3 movement = dir * MoveSpeed;

        //Jump
        if (CC.isGrounded)
        {
            //JUMP
            if (Input.GetKeyDown(KeyCode.Space))
            {
                SetState("Jump", true);
                AudioManager.instance.jumpVoice.Play();
                movement += player.up * JumpSpeed;
            }
            else
            {
                SetState("Jump", false);
                //SPRINT
                if (Input.GetKey(KeyCode.LeftShift))
                {
                    SetState("Sprint", true);
                    movement *= sprintModifier;
                }
                else
                {
                    SetState("Sprint", false);
                }
            }
        }
        else
        {
            movement += vec3.UnitY * -Gravity;
        }

        //testing reset position
        if(Input.GetKey(KeyCode.Q))
        {
            Console.WriteLine("ResetPlayerPosition");
            player.localPosition = new vec3(-19.586f, 2.753f, 21.845f);
            player.localRotation = vec3.Radians(new vec3(0, 180, 0));
        }

        CC.Move(movement);

        //attacking
        if(Input.GetMouseDown(0) && !IsAttacking)
        {
            Console.WriteLine("Attack");
            IsAttacking = true;
            playerWeaponCollider.SetActive(true);//enable the weapon collider
            SetState("Attack1", true);
            AudioManager.instance.playerSlashAttack.Play();
            AudioManager.instance.spark.Play();
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

        animationManager.UpdateState();
    }

    void UpdatehealthBar()
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
            currentHealth = 0;
            healthBarFill.GetComponent<Transform>().localPosition = new vec3(-0.8f, 0.857f, 3f);
            healthBarFill.GetComponent<Transform>().localScale = new vec3(0f, -0.035f, 1f);
        }
    }

    void SetState(string stateName, bool value)
    {
        if (stateName == "Jump")
        Console.WriteLine($"{stateName} {value}");
        animationManager.GetState(stateName).state = value;
    }
    void TakeDamage(int amount)
    {
        if(!isInvulnerable)
        {
            currentHealth -= amount;
            //UpdatehealthBar();
        }
        Console.WriteLine("Hit");
        
        if (currentHealth <= 0)
        {
            Console.WriteLine("YouDied");
            PlayerModel.gameObject.SetActive(false);//testing, remove this later
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
            dir -= (CamYawPivot.forward);

        if (Input.GetKey(KeyCode.A))
            dir -= (CamYawPivot.right);

        if (Input.GetKey(KeyCode.S))
            dir += CamYawPivot.forward;

        if (Input.GetKey(KeyCode.D))
            dir += (CamYawPivot.right);
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

    void OnCollisionEnter(PhysicsComponent rb)
    {
        Console.WriteLine("INTO THE UNKNOWN");
    }
}
