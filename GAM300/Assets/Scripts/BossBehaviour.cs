using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
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

    public float ultimateSize = 66f;

    public float basicAttackDuration = 3f;

    public float dashAttackDuration = 3f;

    public float jumpAttackDuration = 3f;

    public float ultiChargeDuration = 10f;

    public float projectileAttackDuration = 4f;

    public float ultiExplodeDuration = 2f;

    public float slamAttackRadius = 10f;

    public float projectileDistance = 5f;

    public int projectileCount = 16;

    public ParticleComponent dashVFX;
    public ParticleComponent ultimateVFX;

    public Transform ultimateCollider;

    vec3 indicatorLocal = new vec3();

    public Transform ultiSphere;

    AnimationStateMachine animationManager;

    ThirdPersonController player;

    public GameObject bullet;

    public Transform model;

    int health = 100;

    float yPos;

    Rigidbody rb;

    int phase = 1;

    bool startShoot = false;

    bool dashed = false;

    public Transform center;

    void Awake()
    {
        yPos = transform.position.y;
        rb = GetComponent<Rigidbody>();
        indicatorLocal = ultiSphere.localPosition;
    }

    void Start()
    {
        player = ThirdPersonController.instance;
        StartCoroutine(Chase());
        InitAnimStates();
        ultiSphere.gameObject.SetActive(false);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Z)) 
        {
            health = 0;
            phase = 2;
        }
    }

    //Decision making is here
    IEnumerator Chase()
    {
        float timer = chaseDuration;
        while (timer > 0)
        {
            if (phase == 2 && health == 0)
            {
                health = 100;
                rb.linearVelocity = vec3.Zero;
                SetState("Running", false);
                StartCoroutine(UltimateAttack());
                yield break;
            }
            SetState("Running", true);
            float dist = vec3.Distance(transform.position, player.transform.position);
            vec3 dir = (player.transform.position - transform.position) / dist;
            dir.y = 0;
            UpdateRotation(dir,rotationSpeed);
            rb.linearVelocity = dir * chaseSpeed * Time.deltaTime;
            timer -= Time.deltaTime;
            if (dist < basicAttackDistance)
            {
                rb.linearVelocity = vec3.Zero;
                SetState("Running", false);
                StartCoroutine(BasicAttack());
                yield break;
            }
            yield return null;
        }
        SetState("Running", false);
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
            //First Attack
            if (timer > 2.2f && timer < 2.3f)
            {
                UpdateRotation(dir, rotationSpeed/2f);
                rb.linearVelocity = transform.back * 30f;
            }
            //Second Attack
            else if (timer > 1.5f && timer < 1.7f)
            {
                UpdateRotation(dir, rotationSpeed);
                rb.linearVelocity = transform.back * 30f;
            }
            else if (timer > 0.7f && timer < 1.2f)
            {
                UpdateRotation(dir, rotationSpeed / 2f);
                rb.linearVelocity = transform.back * 10f;
            }
            else
            {
                rb.linearVelocity = vec3.Zero;
            }
            timer -= Time.deltaTime;
            yield return null;
        }


        yield return new WaitForSeconds(basicAttackDuration / 2f);

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
        vec3 targetPos = center.position;
        targetPos.y = yPos;
        vec3 startPos = transform.position;
        while (timer > 0)
        {
            SetState("Sprinting", true);
            dist = vec3.Distance(startPos, targetPos);
            vec3 dir = (targetPos - startPos) / dist;
            rb.linearVelocity = dir * chaseSpeed * 2f * Time.deltaTime;
            UpdateRotation(dir, rotationSpeed);
            timer -= Time.deltaTime;
            yield return null;
        }

        rb.linearVelocity = vec3.Zero;

        SetState("Sprinting", false);
        timer = dodgeDuration / 2f;
        while (timer > 0)
        {
            dist = vec3.Distance(player.transform.position, transform.position);
            vec3 dir = (player.transform.position - transform.position) / dist;
            UpdateRotation(dir, rotationSpeed);
            timer -= Time.deltaTime;
            yield return null;
        }
        if (phase == 1)
        {
            StartCoroutine(Chase());
        }
        else
        {
            StartCoroutine(ProjectileAttack());
        }
    }

    IEnumerator DashAttack()
    {
        dashed = true;
        float chargeUpDuration = dashAttackDuration / 2f;
        float timer = chargeUpDuration;

        float dist = 0;
        vec3 dir = vec3.Zero;
        SetState("DashChargeUp", true);
        vec3 startPos = transform.localPosition;
        vec3 targetPos = vec3.Zero;
        while (timer > 0)
        {
            if (timer < chargeUpDuration - 0.5f)
            {
                animator.SetSpeed(0f);
            }
            targetPos = player.transform.position;
            targetPos.y = transform.position.y;
            dist = vec3.Distance(transform.position, targetPos);
            dir = (targetPos - transform.position) / dist;
            UpdateRotation(dir, rotationSpeed * 2f);
            timer -= Time.deltaTime;
            yield return null;
        }
        dashVFX.particleLooping = true;
        SetState("DashChargeUp", false);
        SetState("DashAttack", true);


        float remainingTime = 0.4f;
        float startAttackTime = chargeUpDuration - remainingTime;

        timer = startAttackTime;
        while (timer > 0)
        {
            targetPos = player.transform.position;
            targetPos.y = transform.position.y;
            dist = vec3.Distance(transform.position, targetPos);
            dir = (targetPos - transform.position) / dist;
            UpdateRotation(dir, rotationSpeed * 2f);
            timer -= Time.deltaTime;
            yield return null;
        }
        animator.SetSpeed(1f);

        timer = remainingTime;
        while (timer > 0)
        {
            vec3 pos = vec3.Lerp(targetPos + dir * 5f, startPos, timer / remainingTime);
            transform.position = pos;
            timer -= Time.deltaTime;
            yield return null;
        }
        //Cooldown
        rb.linearVelocity = vec3.Zero;
        yield return new WaitForSeconds(dashAttackDuration / 2f);
        dashVFX.particleLooping = false;
        StartCoroutine(Chase());
        SetState("DashAttack", false);
    }

    IEnumerator SlamAttack()
    {
        float jumpDur = jumpAttackDuration * 0.3f;
        float startDur = jumpAttackDuration * 0.7f;

        vec3 startPos = transform.localPosition;
        vec3 targetPos = player.transform.localPosition;
        targetPos.y = startPos.y;

        float dist = 0;
        vec3 dir = vec3.Zero;
        SetState("JumpAttack", true);

        float indicatorY = ultiSphere.position.y;
        float timer = startDur/5f;
        while (timer > 0)
        {
            timer -= Time.deltaTime;
            yield return null;
        }

        timer = startDur - timer;
        while (timer > 0)
        {
            targetPos = player.transform.localPosition;
            targetPos.y = startPos.y;
            dist = vec3.Distance(startPos, targetPos);
            dir = (targetPos - startPos) / dist;
            UpdateRotation(dir, rotationSpeed/2f);
            transform.position = vec3.Lerp(startPos, startPos + transform.back * dist + vec3.UnitY * 50f, (1 - timer / startDur) * 0.5f);

            vec3 indicatorPos = startPos + transform.back * dist;
            indicatorPos.y = indicatorY;
            ultiSphere.position = indicatorPos;
            timer -= Time.deltaTime;
            ultiSphere.localScale = vec3.Lerp(vec3.Ones * slamAttackRadius, vec3.Ones * 0.1f, timer / startDur);
            if (timer < startDur / 3f * 2f)
            {
                ultiSphere.gameObject.SetActive(true);
                animator.SetSpeed(0f);
            }
            yield return null;
        }
        startPos = transform.localPosition;
        targetPos = startPos + transform.back * dist * 0.5f;
        targetPos.y = yPos;
        timer = jumpDur;

        vec3 modelPos = model.localPosition;
        vec3 modelTargetPos = modelPos + vec3.UnitZ * 2f;
        while (timer > 0)
        {
            if (timer < jumpDur / 3f * 2f)
            {
                animator.SetSpeed(1f);
                model.localPosition = vec3.Lerp(modelPos, modelTargetPos, (1 - timer / jumpDur));
            }
            transform.localPosition = vec3.Lerp(startPos, targetPos, (1 - timer / jumpDur));
            vec3 indicatorPos = targetPos;
            indicatorPos.y = indicatorY;
            ultiSphere.position = indicatorPos;
            timer -= Time.deltaTime;
            yield return null;
        }
        animator.SetSpeed(0f);
        ultiSphere.localPosition = indicatorLocal;
        ultiSphere.scale = vec3.Ones;
        transform.localPosition = targetPos;
        ultiSphere.gameObject.SetActive(false);
        yield return new WaitForSeconds(jumpAttackDuration / 2f);
        model.localPosition = modelPos;
        SetState("JumpAttack", false);
        StartCoroutine(Dodge());
    }

    IEnumerator Rest(float duration,IEnumerator next)
    {
        vec3 startPos = transform.localPosition;
        vec3 targetPos = player.transform.localPosition;
        float timer = duration;
        while (timer > 0)
        {
            float dist = vec3.Distance(startPos, targetPos);
            vec3 dir = (targetPos - startPos) / dist;
            UpdateRotation(dir, rotationSpeed);
            timer -= Time.deltaTime;
            yield return null;
        }
        StartCoroutine(next);
    }

    IEnumerator UltimateAttack()
    {
        float timer = ultiChargeDuration;

        vec3 sphereScale = ultiSphere.localScale;

        ultiSphere.gameObject.SetActive(true);

        SetState("Ultimate", true);
        while (timer > 0)
        {
            ultiSphere.localScale = vec3.Lerp(ultimateSize, sphereScale, timer / ultiChargeDuration);

            if (timer <= ultiChargeDuration - 0.05f)
            {
                animator.SetSpeed(0.02f);
            }
            timer -= Time.deltaTime;
            yield return null;
        }
        timer = ultiExplodeDuration;
        //ACTUAL BOOM EXPANDS
        while (timer > 0)
        {
            if (timer <= ultiExplodeDuration / 3f * 2f)
            {
                ultimateCollider.localPosition = transform.position;
                ultimateCollider.gameObject.GetComponent<Rigidbody>().linearVelocity = vec3.UnitY;
                ultimateCollider.gameObject.SetActive(true);
                ultimateVFX.particleLooping = true;
                animator.SetSpeed(1f);
            }
            timer -= Time.deltaTime;
            yield return null;
        }
        ultimateVFX.particleLooping = false;
        ultimateCollider.gameObject.SetActive(false);

        SetState("Ultimate", false);
        ultiSphere.gameObject.SetActive(false);
        ultiSphere.localScale = sphereScale;
        StartCoroutine(Rest((ultiChargeDuration + ultiExplodeDuration) / 2f, SlamAttack()));
    }

    IEnumerator StartBullet(GameObject bullet)
    {
        while (!startShoot)
            yield return null;
        bullet.GetComponent<Rigidbody>().linearVelocity = bullet.transform.back * projectileSpeed;
    }

    IEnumerator ProjectileAttack()
    {
        int cycles = 4;

        int directions = projectileCount;
        float angle = 360 / (float)(directions);
        float offset = 10f;


        float intervals = projectileAttackDuration / 2 / cycles;
        float timer;
        for (int i = 0; i < cycles; i++)
        {
            SetState("RangeAttack", true);

            vec3 bossRot = transform.localRotation;
            quat bossQuat = glm.FromEulerToQuat(bossRot);

            float yaw = (float)bossQuat.Yaw;


            for (int d = 0; d < directions; d++)
            {
                timer = intervals / directions;
                vec3 rot = new vec3(0, glm.Radians(offset * i + angle * d) + yaw, 0) ;
                GameObject obj = Instantiate(bullet, transform.localPosition + vec3.UnitY * 2f, rot);
                StartCoroutine(StartBullet(obj));
                obj.transform.position += obj.transform.back * projectileDistance;
                timer = intervals/directions;
                while (timer > 0)
                {
                    timer -= Time.deltaTime;
                    yield return null;
                }
            }

            startShoot = true;
            timer = intervals;
            while (timer > 0)
            {
                vec3 dir = (player.transform.localPosition - transform.localPosition)/vec3.Distance(player.transform.localPosition, transform.localPosition);
                UpdateRotation(dir,rotationSpeed);
                timer -= Time.deltaTime;
                yield return null;
            }
            

            SetState("RangeAttack", false);
            startShoot = false;
        }
        //Cooldown
        StartCoroutine(Rest(projectileAttackDuration / 2f, SlamAttack()));
    }

    public void UpdateRotation(vec3 dir, float rotSpeed)
    {
        if (dir == vec3.Zero)
            return;
        float angle = (float)Math.Atan2(-dir.x, -dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
        quat oldQuat = glm.FromEulerToQuat(transform.localRotation).Normalized;

        // Interpolate using spherical linear interpolation (slerp)
        quat midQuat = quat.SLerp(oldQuat, newQuat, Time.deltaTime * rotSpeed);

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
        AnimationState jump = animationManager.GetState("JumpAttack");
        AnimationState overdrive = animationManager.GetState("Ultimate");
        AnimationState dashChargeUp = animationManager.GetState("DashChargeUp");
        AnimationState dashAttack = animationManager.GetState("DashAttack");
        AnimationState rangeAttack = animationManager.GetState("RangeAttack");
        dashAttack.speed = 2f;
        rangeAttack.speed = 1.5f;
        AnimationState attack1 = animationManager.GetState("Attack1");
        AnimationState attack2 = animationManager.GetState("Attack2");
        AnimationState sprint = animationManager.GetState("Sprinting");
        AnimationState run = animationManager.GetState("Running");
        AnimationState walk = animationManager.GetState("Walking");
        AnimationState ready = animationManager.GetState("Ready");

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