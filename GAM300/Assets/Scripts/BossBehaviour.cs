using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class BossBehaviour : Script
{

    private enum BossState
    {
        Chase,
        BasicAttack,
        DashAttack,
        SlamAttack,
        UltimateAttack,
        UltimateCharge,
        ProjectileAttack,
    }

    //Start boss fight - DashAttacks towards you to close the distance
    //1st phase rotation:
    //  Chase 3s, if within distance, basic attack, else dash attack
    //  If after 3 basic attacks, jump back, dash attack

    //Transition:
    //  Health drops to 0 for the first time, enters sphere shield, doesnt take damage, does big aoe damage after charging up

    //2nd phase rotation
    //  Starts of with slam attack
    //  Then throws projectile in a 360* angle

    BossState state = 0;

    public float chaseSpeed = 10f;
    public float rotationSpeed = 10f;

    public float projectileSpeed = 10f;

    public Animator animator;

    public float basicAttackDistance = 3f;

    public float chaseDuration = 3f;

    public float dodgeDuration = 2f;

    public float basicAttackDuration = 3f;

    public float dashAttackDuration = 3f;

    public float jumpAttackDuration = 3f;

    public float ultiChargeDuration = 10f;

    public float projectileAttackDuration = 4f;

    public float ultiExplodeDuration = 2f;

    public Transform ultiSphere;

    AnimationStateMachine animationManager;

    ThirdPersonController player;

    public GameObject bullet;

    float yPos;

    Rigidbody rb;

    int phase = 1;

    void Awake()
    {
        yPos = transform.position.y;
        rb = GetComponent<Rigidbody>();
    }

    void Start()
    {
        player = ThirdPersonController.instance;
        StartCoroutine(Chase());
        InitAnimStates();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Z)) 
        {
            phase = 2;
        }
    }

    //Decision making is here
    IEnumerator Chase()
    {
        float timer = chaseDuration;
        while (timer > 0)
        {
            if (phase == 2)
            {
                rb.linearVelocity = vec3.Zero;
                SetState("Run", false);
                StartCoroutine(UltimateAttack());
                yield break;
            }
            SetState("Run", true);
            float dist = vec3.Distance(transform.position, player.transform.position);
            vec3 dir = (player.transform.position - transform.position) / dist;
            dir.y = 0;
            UpdateRotation(dir);
            rb.linearVelocity = dir * chaseSpeed * Time.deltaTime;
            timer -= Time.deltaTime;
            if (dist < basicAttackDistance)
            {
                rb.linearVelocity = vec3.Zero;
                SetState("Run", false);
                StartCoroutine(BasicAttack());
                yield break;
            }
            yield return null;
        }
        SetState("Run", false);
        StartCoroutine(DashAttack());
        rb.linearVelocity = vec3.Zero;
    }

    IEnumerator BasicAttack()
    {
        float timer = basicAttackDuration;
        float dist = 0;
        SetState("Attack1", true);
        while (timer > 0)
        {
            dist = vec3.Distance(transform.position, player.transform.position);
            vec3 dir = (player.transform.position - transform.position) / dist;
            dir.y = 0;
            if (timer > basicAttackDuration / 2)
            {
                UpdateRotation(dir);
            }
            timer -= Time.deltaTime;
            yield return null;
        }
        SetState("Attack1", false);
        if (dist > basicAttackDistance)
        {
            StartCoroutine(DashAttack());
            yield break;
        }
        else
        {
            StartCoroutine(Dodge());
        }
    }

    IEnumerator Dodge()
    {
        float timer = dodgeDuration;
        float dist = 0;
        vec3 targetPos = vec3.Zero;
        targetPos.y = yPos;
        vec3 startPos = transform.position;
        SetState("Dodge", true);
        while (timer > 0)
        {
            dist = vec3.Distance(startPos, targetPos);
            vec3 dir = (targetPos - startPos) / dist;
            UpdateRotation(dir);
            timer -= Time.deltaTime;
            vec3 pos = vec3.Lerp(startPos+dir* 5f, startPos, timer / dodgeDuration);
            transform.position = pos;
            yield return null;
        }
        SetState("Dodge", false);
        StartCoroutine(Chase());
    }

    IEnumerator DashAttack()
    {
        float timer = dashAttackDuration;
        float chargeUpDuration = dashAttackDuration / 4f;

        float dist = 0;
        vec3 dir = vec3.Zero;
        SetState("DashAttack", true);
        vec3 startPos = transform.localPosition;
        vec3 targetPos = vec3.Zero;
        while (timer > 0)
        {
            if (timer > chargeUpDuration)
            {
                targetPos = player.transform.position;
                targetPos.y = transform.position.y;
                dist = vec3.Distance(transform.position, targetPos);
                dir = (targetPos - transform.position) / dist;
                UpdateRotation(dir);
            }
            else
            {
                vec3 pos = vec3.Lerp(targetPos + dir * 5f, startPos, timer / chargeUpDuration);
                transform.position = pos;
            }
            timer -= Time.deltaTime;
            yield return null;
        }
        animator.SetSpeed(0);
        //Cooldown
        timer = chargeUpDuration;
        rb.linearVelocity = vec3.Zero;
        while (timer > 0)
        {
            timer -= Time.deltaTime;
            yield return null;
        }
        SetState("DashAttack", false);
        StartCoroutine(Chase());
    }

    IEnumerator SlamAttack()
    {
        float jumpDur = jumpAttackDuration * 0.3f;
        float startDur = jumpAttackDuration * 0.5f;
        float slamDur = jumpAttackDuration - jumpDur - startDur;

        vec3 startPos = transform.localPosition;
        startPos.y = 0;
        vec3 targetPos = player.transform.localPosition;
        targetPos.y = 0f;

        float dist = 0;
        vec3 dir = vec3.Zero;
        float timer = startDur;
        SetState("Jump", true);
        ultiSphere.localScale = new vec3(5f);
        while (timer > 0)
        {
            targetPos = player.transform.localPosition;
            targetPos.y = 0f;
            ultiSphere.position = targetPos;
            dist = vec3.Distance(startPos, targetPos);
            dir = (targetPos - startPos) / dist;
            UpdateRotation(dir);
            transform.position = vec3.Lerp(startPos, startPos + transform.back * dist + vec3.UnitY * 50f, (1 - timer / startDur) * 0.5f);
            timer -= Time.deltaTime;
            yield return null;
        }
        startPos = transform.localPosition;
        targetPos = startPos + transform.back * dist * 0.5f;
        targetPos.y = yPos;
        timer = jumpDur;
        while (timer > 0)
        {
            ultiSphere.position = targetPos;
            transform.localPosition = vec3.Lerp(startPos, targetPos, (1 - timer / jumpDur));
            timer -= Time.deltaTime;
            yield return null;
        }
        ultiSphere.localPosition = vec3.Zero;
        ultiSphere.scale = vec3.Ones;
        transform.localPosition = targetPos;
        timer = slamDur;
        //Freeze
        while (timer > 0)
        {
            timer -= Time.deltaTime;
            yield return null;
        }
        StartCoroutine(ProjectileAttack());
    }

    IEnumerator UltimateAttack()
    {
        float timer = ultiChargeDuration;

        vec3 sphereScale = ultiSphere.localScale;

        while (timer > 0)
        {
            SetState("Idle", true);
            ultiSphere.localScale = vec3.Lerp(66f, sphereScale, timer / ultiChargeDuration);
            timer -= Time.deltaTime;
            yield return null;
        }
        timer = ultiExplodeDuration;
        //ACTUAL BOOM EXPANDS
        while (timer > 0)
        {
            SetState("Idle", true);
            timer -= Time.deltaTime;
            yield return null;
        }
        ultiSphere.localScale = sphereScale;
        //BOOM
        StartCoroutine(SlamAttack());
    }

    IEnumerator ProjectileAttack()
    {
        int cycles = 8;

        int directions = 16;
        float angle = 360 / directions;
        float offset = 10f;

        List<GameObject> bullets;

        for (int i = 0; i < cycles; i++)
        {
            float timer = projectileAttackDuration / 2 / cycles;
            for (int d = 0; d < directions; d++)
            {
                vec3 rot = new vec3(0, glm.Radians(offset * i + angle * d), 0) ;
                GameObject obj = Instantiate(bullet, transform.localPosition, rot);
                obj.transform.position += obj.transform.back * 2f;
                obj.GetComponent<Rigidbody>().linearVelocity = obj.transform.back * projectileSpeed;
/*                while()
                {

                }*/
                yield return null;
            }
            timer = projectileAttackDuration / 2 / cycles;
            while (timer > 0)
            {
                vec3 dir = (player.transform.localPosition - transform.localPosition)/vec3.Distance(player.transform.localPosition, transform.localPosition);
                UpdateRotation(dir);
                SetState("Idle", true);
                timer -= Time.deltaTime;
                yield return null;
            }

        }
        StartCoroutine(SlamAttack());
    }

    public void UpdateRotation(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;
        float angle = (float)Math.Atan2(-dir.x, -dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
        quat oldQuat = glm.FromEulerToQuat(transform.localRotation).Normalized;

        // Interpolate using spherical linear interpolation (slerp)
        quat midQuat = quat.SLerp(oldQuat, newQuat, Time.deltaTime * rotationSpeed);

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
                transform.localRotation = rot;
            }
        }
    }

    void InitAnimStates()
    {
        animationManager = new AnimationStateMachine(animator);

        //Highest Precedence
        AnimationState death = animationManager.GetState("Death");
        AnimationState stun = animationManager.GetState("Stun");
        AnimationState falling = animationManager.GetState("Falling");
        AnimationState jump = animationManager.GetState("Jump");
        AnimationState overdrive = animationManager.GetState("Overdrive");
        AnimationState dashAttack = animationManager.GetState("DashAttack");
        AnimationState dodge = animationManager.GetState("Dodge");
        AnimationState attack1 = animationManager.GetState("Attack1");
        AnimationState attack2 = animationManager.GetState("Attack2");
        AnimationState attack3 = animationManager.GetState("Attack3");
        AnimationState sprint = animationManager.GetState("Sprint");
        AnimationState run = animationManager.GetState("Run");
        //Lowest Precedence

        attack1.speed = 1.2f;
    }

    bool GetState(string stateName)
    {
        return animationManager.GetState(stateName).state;
    }
    void SetState(string stateName, bool value)
    {
        animationManager.GetState(stateName).state = value;
        animationManager.UpdateState();
    }
}