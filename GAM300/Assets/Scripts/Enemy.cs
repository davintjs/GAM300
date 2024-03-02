using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Threading;
using System.Resources;

public class Enemy : Script
{
    public float moveSpeed = 2f;
    public float chaseDistance = 10f;
    public float attackDistance = 3f;
    public float attackCooldown = 0f;
    public Transform player;
    public int count;

    public float maxHealth = 3f;
    public float currentHealth;
    public bool isDead = false;
    // HealthBar
    public Transform hpBar;
    public bool inRange = false;
    public bool back = false;

    public Transform meleeEnemyPos;
    public Transform modelOffset;
    public float RotationSpeed = 6f;

    AnimationStateMachine animationManager;
    public Animator animator;
    public int state;//Example 1 is walk, 2 is attack, 3 is idle etc.
    public bool startDeathAnimationCountdown = false;
    float animationTimer = 2.5f;
    public float currentAnimationTimer;
    private float currentDeathAnimationTimer = 2.8f;
    private Coroutine damagedCoroutine = null;

    public float testingValue = 0f;

    //public GameObject spawnObject;
    public GameObject attackTrigger;
    public Transform parentTransform;
    Rigidbody rb;

    public bool isAttacking = false;
    public bool isAttackCooldown = false;
    float attackTimer = 1f;
    float currentAttackTimer;
    float currentAttackBuffer;
    public float attackCooldownTimer = 1f;
    public float currentAttackCooldownTimer;

    // NavMesh stuff
    public float duration = 0.2f;
    public float timer = 0f;
    public bool newRequest = false;

    public bool isStunned;
    public float stunDuration = 0.5f;
    public float currentStunDuration;

    // Staggering stuff
    public float staggerCooldown = 8f;
    public float staggerTimer = 0f;

    //audio
    public bool playOnce = true;
    public bool alertedOnce = true;
    int alertedRotation = 0;

    void Start()
    {
        playOnce = true;
        currentHealth = maxHealth;
        state = 0;//start with idle state
        currentStunDuration = stunDuration;
        rb = GetComponent<Rigidbody>();
        InitAnimStates();
    }
   

    void Update()
    {
        if (player == null)
        {
            //SetState("Idle", true);
            return;
        }

        //debugging state
        //Console.WriteLine(state);

        //death animation timer
        if (startDeathAnimationCountdown)
        {
            SetEnabled(GetComponent<Rigidbody>(), false);
            currentDeathAnimationTimer -= Time.deltaTime;
            if (currentDeathAnimationTimer <= 0.5f)
            {
                currentDeathAnimationTimer = animationTimer;
                startDeathAnimationCountdown = false;
                //animator.Pause();//pause the death animation to prevent it from returning to idle animation
                gameObject.SetActive(false);
                //Respawn();
                //SceneManager.LoadScene("LevelPlay2");
            }
        }

        if (isDead)
        {
            return;
        }
        if(isStunned)
        {
            state = 4;//set to stun state
        }
        if(state == 2)
        {
            isAttacking = true;
            if(isAttacking && !isAttackCooldown)
            {
                currentAttackTimer += Time.deltaTime;
                currentAttackBuffer += Time.deltaTime;
                if (currentAttackBuffer >= 0.8f) // So that the attack is not instantaneous
                {
                    if (attackTrigger != null)
                    {
                        attackTrigger.SetActive(true);
                        attackTrigger.GetComponent<Rigidbody>().linearVelocity = new vec3(modelOffset.back * 0.6f);
                        attackTrigger.transform.localPosition = new vec3(transform.localPosition + modelOffset.forward * 0.6f);
                        attackTrigger.transform.rotation = new vec3(modelOffset.rotation);
                    }
                    currentAttackBuffer = 0f;
                }
                
                //AudioManager.instance.meleeEnemyAttack.Play();
                if (currentAttackTimer >= attackTimer)
                {
                    isAttacking = false;
                    isAttackCooldown = true;
                    SetState("Attack", false);
                    currentAttackTimer = 0f;
                }
            }
            if(isAttackCooldown)
            {
                currentAttackCooldownTimer += Time.deltaTime;
                if (attackTrigger != null)
                {
                    attackTrigger.SetActive(false);
                }

                if (currentAttackCooldownTimer >= attackCooldownTimer)
                {
                    isAttackCooldown = false;
                    currentAttackCooldownTimer = 0f;
                }
            }
           
        }
        else if (state != 2)
        {
            attackTrigger.SetActive(false);
            isAttacking = false;
            isAttackCooldown = false;
            currentAttackCooldownTimer = 0f;
            currentAnimationTimer = 0f;
        }


        vec3 direction = player.localPosition - transform.position;
        direction.y = 0f;
        direction = direction.NormalizedSafe;
        if(!isDead)
        {
            // Bean: This shouldnt be here
            // animationManager.currentState = "";
            switch (state)
            {
                //idle state
                case 0:
                    //Console.WriteLine("Idle");
                    //idle animation
                    playOnce = true;//reset ability to play audio
                    alertedOnce = true;
                    SetState("Idle", true);
                    //attackTrigger.SetActive(false);
                    //player detection
                    if (vec3.Distance(player.localPosition, transform.localPosition) <= chaseDistance)
                    {
                        if (animationManager.GetState("Idle").state)
                        {
                            SetState("Idle", false);
                        }
                        //change to chase state
                        state = 1;
                    }
                    break;
                //chase state
                case 1:
                    //Console.WriteLine("Chase");
                    SetState("Run", true);
                    playOnce = true;//reset ability to play audio
                    //attackTrigger.SetActive(false);
                    //change to attack state once it has reach it is in range

                    staggerTimer += Time.deltaTime; // Start counting stagger timer

                    if (alertedOnce)
                    {
                        Random rd = new Random();
                        alertedRotation = rd.Next(0, 2);
                        alertedOnce = false;

                        switch (alertedRotation)
                        {
                            case 0:
                                AudioManager.instance.enemyAlerted1.Play();
                                break;
                            case 1:
                                AudioManager.instance.enemyAlerted2.Play();
                                break;
                            case 2:
                                AudioManager.instance.enemyAlerted3.Play();
                                break;

                        }
                    }

                    if (vec3.Distance(player.localPosition, transform.localPosition) <= attackDistance)
                    {
                        state = 2;
                        attackCooldown = 0f;
                    }
                    //return to its starting position if player is far from its chaseDistance
                    if (vec3.Distance(player.localPosition, transform.localPosition) > chaseDistance)
                    {
                        if (animationManager.GetState("Run").state)
                        {
                            SetState("Run", false);
                        }
                        //return back to its previous position state
                        state = 0;
                    }

                    NavMeshAgent check = GetComponent<NavMeshAgent>();
                    if (check != null) // Use navmesh if is navmesh agent
                    {
                        check.FindPath(player.localPosition);
                    }
                    else // Default
                    {
                        LookAt(direction);
                        GetComponent<Rigidbody>().linearVelocity = direction * moveSpeed;
                    }

                    break;
                //attack state
                case 2:
                    SetState("Attack", true); //attack animation
                    staggerTimer += Time.deltaTime; // Start counting stagger timer

                    if (playOnce)
                    {
                        playOnce = false;
                        AudioManager.instance.meleeEnemyAttack.Play();
                    }

                    LookAt(direction);
                    if(!isAttacking)
                    {
                        //change to chase state once player has reach out of range
                        if (vec3.Distance(player.localPosition, transform.localPosition) > attackDistance)
                        {
                            if (animationManager.GetState("Attack").state)
                            {
                                SetState("Attack", false);
                            }
                            state = 1;
                        }
                    }

                    break;

                //death state
                case 3:
                    //Console.WriteLine("Death");
                    SetState("Death", true);
                    animationManager.UpdateState();

                    break;
                //stun state
                case 4:
                    //Console.WriteLine("Stunned");
                    SetState("Stun", true);
                    //attackTrigger.SetActive(false);
                    currentStunDuration -= Time.deltaTime;
                    if (currentStunDuration <= 0)
                    {
                        if (animationManager.GetState("Stun").state)
                        {
                            SetState("Stun", false);
                        }
                        isStunned = false;
                        state = 0;//reset back to idle state
                        currentStunDuration = stunDuration;
                    }
                    //animationManager.UpdateState();
                    break;

            }
        }

        //timer += Time.deltaTime;

        //needed for the animation to change
        animationManager.UpdateState();
    }

    void LookAt(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;
        float angle = (float)Math.Atan2(dir.x, dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
        quat oldQuat = glm.FromEulerToQuat(transform.rotation).Normalized;

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
                transform.rotation = rot;
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
        AnimationState attack = animationManager.GetState("Attack");
        AnimationState walk = animationManager.GetState("Walk");
        AnimationState run = animationManager.GetState("Run");
        AnimationState idle = animationManager.GetState("Idle");
        //Lowest Precedence

        stun.SetConditionals(false, death);
        attack.SetConditionals(false, death, stun);
        attack.speed = 1.5f;
        attack.loop = true;
        walk.SetConditionals(true, walk);
        walk.SetConditionals(false, attack, death, stun);
        walk.loop = true;
        run.SetConditionals(false, attack, death, stun);
        run.loop = true;
    }

    bool GetState(string stateName)
    {
        return animationManager.GetState(stateName).state;
    }
    void SetState(string stateName, bool value)
    {
        animationManager.GetState(stateName).state = value;
    }

    void TakeDamage(int amount)
    {
        if(!isDead)
        {
            ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.hitShakeMag, CombatManager.instance.hitShakeDur);
            ThirdPersonCamera.instance.SetFOV(-CombatManager.instance.hitShakeMag * 150, CombatManager.instance.hitShakeDur * 4);
            AudioManager.instance.enemyHit.Play();
            AudioManager.instance.meleeEnemyInjured.Play();
            currentHealth -= amount;
            vec3 hpScale = hpBar.localScale;
            hpScale.x = currentHealth / maxHealth;
            hpBar.localScale = hpScale;
            CombatManager.instance.SpawnHitEffect(transform);
        }

        //set particle transform to enemy position
        if (currentHealth <= 0)
        {
            currentHealth = 0;
            vec3 hpScale = hpBar.localScale;
            hpScale.x = currentHealth / maxHealth;
            hpScale.y = currentHealth / maxHealth;
            hpScale.z = currentHealth / maxHealth;
            hpBar.localScale = hpScale;
            isDead = true;
            //Console.WriteLine("EnemyDead");
            SetState("Death", true);
            animationManager.UpdateState();
            startDeathAnimationCountdown = true;
            AudioManager.instance.meleeEnemyDie.Play();

            //if (spawnObject != null)
            //    spawnObject.SetActive(true);
            //Destroy(gameObject);
        }
    }

    void Exit()
    {

    }

    void OnTriggerEnter(PhysicsComponent other)
    {
        //check if the rigidbody belongs to a game object called PlayerWeaponCollider
        if (GetTag(other) == "PlayerAttack")
        {
            Transform otherT = other.gameObject.GetComponent<Transform>();
            vec3 dir = otherT.forward;
            dir = dir.NormalizedSafe;
            if (staggerTimer >= staggerCooldown)
            {
                isStunned = true;
                staggerTimer = 0f;
            }
            
            if (damagedCoroutine != null)
            {
                StopCoroutine(damagedCoroutine);
            }
            damagedCoroutine = StartCoroutine(Damaged(.5f, dir * 5));
            TakeDamage(1);
        }
    }

    IEnumerator Damaged(float duration, vec3 knockback)
    {
        duration /= 2;
        float startDuration = duration;
        while (duration > 0)
        {
            rb.linearVelocity = new vec3(knockback * (duration / startDuration));
            duration -= Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }
        duration = startDuration;
        while (duration > 0)
        {
            float val = glm.Radians(-45f);
            duration -= Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }
    }

    void OnCollisionEnter(PhysicsComponent component)
    {
        //if (GetTag(component) == "PlayerCollider")
        //{
        //    Console.WriteLine("I have been attacked!");
        //    currentHealth -= 1;
        //    if (currentHealth <= 0)
        //        Destroy(gameObject);
        //}
    }
}

