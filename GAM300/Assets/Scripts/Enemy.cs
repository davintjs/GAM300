using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Linq;
using System.Security.Policy;

public class Enemy : Script
{
    public float moveSpeed = 2f;
    public float chaseDistance = 5f;
    public float attackDistance = 3f;
    public Transform player;
    public int count;

    public float maxHealth = 3f;
    public float currentHealth;
    // HealthBar
    public Transform hpBar;
    public bool inRange = false;
    public bool back = false;

    public Transform meleeEnemyPos;
    public float RotationSpeed = 6f;


    AnimationStateMachine animationManager;
    public Animator animator;
    public int state;//Example 1 is walk, 2 is attack, 3 is idle etc.
    public bool startDeathAnimationCountdown = false;
    float animationTimer = 3.18f;
    public float currentAnimationTimer;
    private Coroutine damagedCoroutine = null;

    public float testing = 1f;



    void Start()
    {
        currentHealth = maxHealth;
        InitAnimStates();
    }


    void Update()
    {
        if (player == null)
            return;
        //follow target
        vec3 direction = (player.localPosition - transform.localPosition).Normalized;
        direction.y = 0;
        //double angle = Math.Atan2(direction.x, direction.z);
        //vec3.Distance(player.localPosition, this.transform.localPosition);

        if (vec3.Distance(player.localPosition, transform.localPosition) <= chaseDistance)
        {
            float angle = (float)Math.Atan2(direction.x,direction.z);
            transform.localRotation = new vec3(0,angle,0);
            GetComponent<Rigidbody>().linearVelocity = direction * moveSpeed;
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
        jump.SetConditionals(false, death, stun);
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

    void TakeDamage(int amount)
    {
        currentHealth -= amount;
        hpBar.localScale.x = currentHealth / maxHealth;
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
            vec3 dir = otherT.back;
            dir = dir.NormalizedSafe;
            if (damagedCoroutine != null)
            {
                StopCoroutine(damagedCoroutine);
            }
            //damagedCoroutine = StartCoroutine(Damaged(.5f, dir * 5));
            TakeDamage(1);
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

