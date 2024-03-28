using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics.PerformanceData;
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

    BossState state = 0;

    public float chaseSpeed = 10f;
    public float rotationSpeed = 10f;

    public float projectileSpeed = 10f;

    public Animator animator;

    public float basicAttackDistance = 3f;

    public float chaseDuration = 3f;

    public float dodgeDuration = 2f;

    public float ultimateSize = 66f;

    public float jumpAttackDuration = 3f;

    public float ultiChargeDuration = 10f;

    public float projectileAttackDuration = 4f;

    public float ultiExplodeDuration = 2f;

    public float slamAttackRadius = 10f;

    public float projectileDistance = 5f;

    public int projectileCount = 16;

    


    public Transform attack1;
    public Transform attack2;
    public Transform attack2Left;
    public Transform attack2Right;
    public Transform attack3;
    public Transform attack4;

    public ParticleComponent dashVFX;
    public ParticleComponent ultimateVFX;

    public Transform ultimateCollider;

    public Transform ultiDomeVFX;
    public Transform ultiLaserVFX;
    public Transform ultiLight;
    public Transform ultiForceField;

    public Transform openingCameraStartTarget;
    public Transform openingCameraEndTarget;
    public GameObject deathAnimation;

    //public AudioSource bossOpeningSFX;
    //public AudioSource bossPhase2SFX;
    //public AudioSource bossDeathSpeechSFX;

    vec3 indicatorLocal = new vec3();

    public Transform ultiSphere;

    AnimationStateMachine animationManager;

    bool _forceFieldUp;

    bool forceFieldUp
    {
        get
        {
            return _forceFieldUp;
        }
        set
        {
            if (value)
            {
                StartCoroutine(StartForceField(0.3f));
            }
            else
            {
                StartCoroutine(StopForceField(1f));
            }
        }
    }

    ThirdPersonController player;

    public GameObject bullet;

    public Transform model;

    private DialogueManagerBoss instance;

    public int maxHealth = 100;

    int _health;

    bool isDead = false;
    bool isDashPlayed = false;
    bool isSlamplayed = false;
    bool halfHealth1 = false;
    bool halfHealth2 = false;

    int health
    {
        get 
        { 
            return _health;
        }
        set 
        {
            if (forceFieldUp)
            {
                bossSounds.ultimateForceField.Play();
                return;
            }

            _health = value;
            if (_health <= maxHealth / 2)
            {
                if (phase == 1 && !halfHealth1)
                {
                    halfHealth1 = true;
                    instance.SetState(6); //half health, phase 1
                }
                else if (phase == 2 && !halfHealth2) {
                    halfHealth2 = true;
                    instance.SetState(7); //half health, phase 2
                }
            }

            if (_health <= 0)
            {
                if (phase == 1)
                {
                    phase = 2;
                    _health = maxHealth;
                    StopAllCoroutines();
                    rb.linearVelocity = vec3.Zero;
                    animationManager.ResetAllStates();
                    SetState("Idle", true);
                    StartCoroutine(UltimateAttack());
                }
                else if(!isDead)
                {
                    isDead = true;
                    StopAllCoroutines();
                    rb.linearVelocity = vec3.Zero;
                    animationManager.ResetAllStates();
                    
                    SetEnabled(rb,false);

                    SetState("Death", true);

                    //if (animator.GetProgress() >= 0.5)
                    StartCoroutine(Death());
                    //DIE
                }
            }
            vec3 scale = vec3.Ones;
            scale.x = _health / (float)maxHealth;
            bossHealthPivot.localScale = scale;
        }
    }

    

    float yPos;

    Rigidbody rb;

    int phase = 1;

    bool dashed = false;

    public Transform center;

    public Transform bossHealthPivot;

    BossAudioManager bossSounds;
    PlayerAudioManager playerSounds;

    void Awake()
    {
        yPos = transform.position.y;
        rb = GetComponent<Rigidbody>();
        indicatorLocal = ultiSphere.localPosition;
        health = maxHealth;       
    }

    void Start()
    {
        instance = DialogueManagerBoss.Instance;
        bossSounds = BossAudioManager.instance;
        playerSounds = PlayerAudioManager.instance;

        player = ThirdPersonController.instance;
        StartCoroutine(EnterBossCutscene());
        InitAnimStates();
        ultiSphere.gameObject.SetActive(false);
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Z)) 
        {
            health = 0;
        }
    }

    IEnumerator Victory()
    {
        yield return new WaitForSeconds(1);
        SceneManager.LoadScene("VictoryScreenMenu",true);
    }

    IEnumerator Death()
    {
        instance.SetState(3); //Death dialogue
        deathAnimation.SetActive(true);
        model.gameObject.SetActive(false);
        yield return new WaitForSeconds(6);
        deathAnimation.SetActive(false);
        model.gameObject.SetActive(true);
        yield return new WaitForSeconds(3);
        animator.SetSpeed(0);
        StartCoroutine(GameManager.instance.GetComponent<SceneTransitionTrigger>().StartFadeOut());
    }

    IEnumerator EnterBossCutscene()
    {
        instance.SetState(1); //apex opening dialogue
        ThirdPersonCamera.instance.cutscene = true;
        Transform camera = ThirdPersonCamera.instance.transform;

        vec3 bossStartPos = center.localPosition + vec3.UnitY / 2f + vec3.UnitX * 5f;
        transform.localPosition = bossStartPos;

        float cutsceneDuration = 8f;
        float timer = cutsceneDuration;
        while (timer > 0)
        {
            animator.SetState("Walking");
            float percentage = (float)timer / cutsceneDuration;
            transform.localPosition = vec3.Lerp(bossStartPos - vec3.UnitX * 12f, bossStartPos, percentage);
            vec3 targetPos = vec3.Lerp(openingCameraEndTarget.position, openingCameraStartTarget.position, percentage * percentage);
            vec3 targetRot = vec3.Lerp(openingCameraEndTarget.rotation, openingCameraStartTarget.rotation, percentage * percentage);
            camera.localPosition = targetPos;
            camera.localRotation = targetRot;
            if (Input.GetKeyDown(KeyCode.X))
            {
                //Skip cutscen
                break;
            }
            timer -= Time.deltaTime;
            yield return null;
        }

        ThirdPersonCamera.instance.SetYaw(openingCameraEndTarget.rotation.y);
        ThirdPersonCamera.instance.transform.localRotation = new vec3(openingCameraEndTarget.rotation.x, openingCameraEndTarget.rotation.y, 0f);
        ThirdPersonCamera.instance.cutscene = false;

        StartCoroutine(Chase());
    }

    //Decision making is here
    IEnumerator Chase()
    {
        float timer = chaseDuration;
        while (timer > 0)
        {
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
                StartCoroutine(BasicAttackSequence());
                yield break;
            }
            yield return null;
        }
        SetState("Running", false);
        StartCoroutine(DashAttack());
        rb.linearVelocity = vec3.Zero;
    }

    IEnumerator RotateAndMoveToPlayer(float duration, float speed, float rotSpeed)
    {
        while (duration > 0f)
        {
            float dist = vec3.Distance(transform.position, player.transform.position);
            vec3 dir = (player.transform.position - transform.position) / dist;
            dir.y = 0;
            UpdateRotation(dir, rotSpeed);
            rb.linearVelocity = transform.back * speed * Time.deltaTime;
            duration -= Time.deltaTime;
            yield return null;
        }
        rb.linearVelocity = vec3.Zero;
    }

    IEnumerator BasicAttackSequence()
    {

        SetState("Attack1", true);
        yield return new WaitForSeconds(0.65f);
        StartCoroutine(RotateAndMoveToPlayer(0.1f, 800f, rotationSpeed / 2f));
        bossSounds.attack1SFX.Play();
        bossSounds.slashRelease.Play();
        StartCoroutine(EnableAttackCollider(attack1, transform.back * 800f, .6f, vec3.UnitY + transform.back * 2f + transform.left * 0.5f));

        yield return new WaitForSeconds(0.55f);
        StartCoroutine(RotateAndMoveToPlayer(0.3f, 300f, rotationSpeed));
        bossSounds.attack2SFX.Play();
        bossSounds.slashRelease.Play();
        StartCoroutine(EnableAttackCollider(attack2Right, transform.back * 800f, .8f, vec3.UnitY + transform.back * 2f));
        StartCoroutine(EnableAttackCollider(attack2Left, transform.back * 800f, .8f, vec3.UnitY + transform.back * 2f));

        yield return new WaitForSeconds(0.45f);
        StartCoroutine(RotateAndMoveToPlayer(0.1f, 0f, rotationSpeed / 2f));
        bossSounds.attack3SFX.Play();
        bossSounds.slashRelease.Play();
        StartCoroutine(EnableAttackCollider(attack2, transform.back * 500f, .8f, vec3.UnitY + transform.back ));

        yield return new WaitForSeconds(0.35f);
        StartCoroutine(RotateAndMoveToPlayer(1f, 0f, rotationSpeed * 8f));
        yield return new WaitForSeconds(0.1f);
        bossSounds.attack4SFX.Play();
        bossSounds.slashRelease.Play();
        StartCoroutine(EnableAttackCollider(attack3, transform.back * 800f, 1.4f, vec3.UnitY + transform.back * 2f));

        yield return new WaitForSeconds(1.2f);

        //COOLDOWN
        yield return new WaitForSeconds(1.5f);

        SetState("Attack1", false);
        if (vec3.Distance(transform.position, player.transform.position) > basicAttackDistance)
        {
            StartCoroutine(DashAttack());
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
        if(!isDashPlayed)
        {
            instance.SetState(4); //apex dash attack
            isDashPlayed = true;
        }

        SetState("DashChargeUp", true);
        BossAudioManager.instance.dashChargeUpVoice.Play();
        vec3 startPos = transform.localPosition;
        yield return StartCoroutine(RotateAndMoveToPlayer(1f,0f,rotationSpeed*2f));
        animator.SetSpeed(0f);
        yield return StartCoroutine(RotateAndMoveToPlayer(0.5f, 0f, rotationSpeed * 2f));
        dashVFX.particleLooping = true;
        SetState("DashChargeUp", false);

        SetState("DashAttack", true);
        yield return StartCoroutine(RotateAndMoveToPlayer(1.3f, 0f, rotationSpeed/8f));
        BossAudioManager.instance.dashShoutVoiceOver.Play();
        BossAudioManager.instance.dashLaserSound.Play();
        BossAudioManager.instance.dashWhoosh.Play();
        animator.SetSpeed(1f);

        dashVFX.particleLooping = false;
        //Spawn colliders
        float remainingTime = 0.2f;
        float timer = remainingTime;

        float dist = vec3.Distance(startPos, player.transform.localPosition);
        vec3 targetPos = startPos + transform.back * (5f + dist);

        //StartCoroutine(EnableAttackCollider(attack2Right, transform.back * 9600f, .8f, vec3.UnitY + transform.back * 2f));
        StartCoroutine(EnableAttackCollider(attack4, transform.back * 4200, .8f, vec3.UnitY + transform.back * 2f));

        while (timer > 0f)
        {
            transform.position = vec3.Lerp(targetPos, startPos, timer / remainingTime);
            timer -= Time.deltaTime;
            yield return null;
        }

        //Cooldown
        rb.linearVelocity = vec3.Zero;
        yield return new WaitForSeconds(2f);
        StartCoroutine(Chase());
        SetState("DashAttack", false);
    }

    IEnumerator SlamAttack()
    {
        if (!isSlamplayed)
        {
            isSlamplayed = true;
            instance.SetState(5); //Apex slam attack
        }
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
        bossSounds.jumpAttackSFX.Play();
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
        ThirdPersonCamera.instance.ShakeCamera(0.6f, 0.2f);
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

    IEnumerator UltimateLaser()
    {
        ultiLaserVFX.localPosition = transform.localPosition;
        vec3 startScale = ultiLaserVFX.localScale;
        vec3 targetScale = vec3.Zero;

        float vfxDuration = 3f;
        float timer = 0;
        ThirdPersonCamera.instance.ShakeCamera(0.2f, timer);
        bossSounds.ultimateLaser.Play();
        while (timer < vfxDuration)
        {
            yield return null;
            timer += Time.deltaTime;
            ultiLaserVFX.localScale = vec3.Lerp(startScale,targetScale,timer/vfxDuration);
        }

        yield return null;
        ultiLaserVFX.localScale = vec3.Zero;
    }

    IEnumerator UltimateLightVFX()
    {
        float dur = 2f;
        float timer = 0;
        ultiLight.gameObject.SetActive(true);
        ultiLight.localPosition = transform.localPosition;

        LightSource lightSource = ultiLight.gameObject.GetComponent<LightSource>();
        float intensity = lightSource.intensity;

        lightSource.intensity = 0f;
        while (timer < dur)
        {
            yield return null;
            timer += Time.deltaTime;
            float percentage = timer / dur;
            if (percentage > 1)
                percentage = 1;
            lightSource.intensity = glm.Lerp(0f, intensity, percentage * percentage);
        }

        timer = 0;
        dur /= 2f;
        ultiDomeVFX.localPosition = transform.localPosition;
        while (timer < dur)
        {
            ultiDomeVFX.localScale = vec3.Lerp(100f, 0f, timer / dur);
            yield return null;
            timer += Time.deltaTime;
            float percentage = timer / dur;
            if (percentage > 1)
                percentage = 1;
            lightSource.intensity = glm.Lerp(intensity, 0f, percentage * percentage);
        }
        ultiLight.gameObject.SetActive(false);
        yield return null;
    }

    IEnumerator UltimateAttack()
    {
        instance.SetState(2); //apex phase 2 dialogue

        float timer = ultiChargeDuration;

        vec3 sphereScale = ultiSphere.localScale;

        ultiSphere.gameObject.SetActive(true);
        forceFieldUp = true;

        ThirdPersonCamera.instance.ShakeCamera(0.05f, timer);
        SetState("Ultimate", true);

        animator.SetSpeed(0.2f);
        while (timer > ultiChargeDuration - 0.05f)
        {
            ultiSphere.localScale = vec3.Lerp(ultimateSize, sphereScale, timer / ultiChargeDuration);
            timer -= Time.deltaTime;
            yield return null;
        }
        bossSounds.ultimateEnergy.Play();
        float timeDiff = ultiChargeDuration - 2.3f;
        while (timer > 2.3f)
        {
            ultiSphere.localScale = vec3.Lerp(ultimateSize, sphereScale, timer / ultiChargeDuration);
            timer -= Time.deltaTime;
            yield return null;
        }
        bossSounds.ultimateRumbling.Play();
        animator.SetSpeed(0f);
        while (timer > 0f)
        {
            ultiSphere.localScale = vec3.Lerp(ultimateSize, sphereScale, timer / ultiChargeDuration);
            timer -= Time.deltaTime;
            yield return null;
        }
        yield return StartCoroutine(UltimateLaser());
        yield return StartCoroutine(UltimateLightVFX());
        forceFieldUp = false;

        timer = ultiExplodeDuration;
        float explode = ultiExplodeDuration / 3f * 2f;
        while (timer > explode)
        {
            timer -= Time.deltaTime;
            yield return null;
        }

        animator.SetSpeed(1f);
        bossSounds.ultimateExplosion.Play();
        bossSounds.screamSFX.Play();
        yield return new WaitForSeconds(0.2f);
        bossSounds.ultimateRinging.Play();

        ultimateVFX.particleLooping = true;
        while (timer > 0)
        {
            ThirdPersonCamera.instance.ShakeCamera(0.8f, ultiExplodeDuration /2f);
            ultiDomeVFX.localScale = vec3.Lerp(100f, 0f, timer / explode);
            ultimateCollider.localPosition = transform.position;
            ultimateCollider.gameObject.GetComponent<Rigidbody>().linearVelocity = vec3.UnitY * 0.0001f;
            ultimateCollider.gameObject.SetActive(true);
            timer -= Time.deltaTime;
            yield return null;
        }
        ultimateVFX.particleLooping = false;
        ultiDomeVFX.gameObject.SetActive(false);
        
        

        ultimateCollider.gameObject.SetActive(false);

        SetState("Ultimate", false);
        ultiSphere.gameObject.SetActive(false);
        ultiSphere.localScale = sphereScale;
        StartCoroutine(Rest((ultiChargeDuration + ultiExplodeDuration) / 2f, SlamAttack()));
    }

    void StartBullet(GameObject bullet, float duration, float waitTime, vec3 velocity)
    {
        EnemyBullet bulletScript = bullet.GetComponent<EnemyBullet>();
        bulletScript.StartCoroutine(bulletScript.StartBullet(duration,waitTime, velocity));
    }

    IEnumerator ProjectileAttack()
    {
        int cycles = 4;

        int directions = projectileCount;
        float angle = 360 / (float)(directions);
        float offset = 10f;

        vec3 posOffset = vec3.UnitY * 2f;

        float intervals = projectileAttackDuration / 2 / cycles;
        float timer;
        for (int i = 0; i < cycles; i++)
        {
            SetState("RangeAttack", true);

            vec3 bossRot = transform.localRotation;
            quat bossQuat = glm.FromEulerToQuat(bossRot);

            float yaw = (float)bossQuat.Yaw;

            float waitTime = intervals * 1.15f;

            for (int d = 0; d < directions; d++)
            {
                timer = intervals / directions;
                vec3 rot = new vec3(0, glm.Radians(offset * i + angle * d + 90f) + yaw, 0) ;
                GameObject obj = Instantiate(bullet, transform.localPosition + posOffset, rot);
                obj.transform.position += obj.transform.right * projectileDistance;
                StartBullet(obj, intervals, waitTime, obj.transform.right * projectileSpeed);
                while (timer > 0)
                {
                    waitTime -= Time.deltaTime;
                    timer -= Time.deltaTime;
                    yield return null;
                }
            }
            timer = intervals;
            while (timer > 0)
            {
                vec3 dir = (player.transform.localPosition - transform.localPosition)/vec3.Distance(player.transform.localPosition, transform.localPosition);
                UpdateRotation(dir,rotationSpeed);
                timer -= Time.deltaTime;
                yield return null;
            }

            SetState("RangeAttack", false);
        }
        SetState("Idle", true);
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

    IEnumerator EnableAttackCollider(Transform attackTrans, vec3 vel, float duration, vec3 offset)
    {
        if (attackTrans == null)
            yield break;
        if (duration == 0)
            yield break;

        attackTrans.localRotation = transform.localRotation;
        attackTrans.localPosition = transform.localPosition + offset;
        Transform child = attackTrans.GetChild();

        Rigidbody rb = child.gameObject.GetComponent<Rigidbody>();
        MeshRenderer mesh = child.GetChild().gameObject.GetComponent<MeshRenderer>();

        Material mat = mesh.material;


        rb.linearVelocity = vel;
        float timer = duration;


        vec4 color = mat.color;

        float emission = mat.emission;

        vec4 targetColor = color;

        targetColor.a = 0;
        while (timer > 0)
        {
            timer -= Time.deltaTime;
            mat.color = vec4.Lerp(targetColor, color, timer/ duration);
            mat.emission = glm.Lerp(0,emission,timer/ duration);
            rb.linearVelocity = vel * Time.deltaTime;
            attackTrans.localScale = vec3.Lerp(2f , 1f,timer/duration);
            yield return null;
        }
        mat.emission = emission;
        mat.color = color;
        attackTrans.localPosition = vec3.Zero;
        rb.gameObject.transform.localPosition = vec3.Zero;
        rb.linearVelocity = vec3.Zero;
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

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "PlayerAttack")
        {
            //While being in overdrive, deals double dmg.
            if (ThirdPersonController.instance.currentlyOverdriven == true)
            {
                ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.hitShakeMag, CombatManager.instance.hitShakeDur);
                ThirdPersonCamera.instance.SetFOV(-CombatManager.instance.hitShakeMag * 150, CombatManager.instance.hitShakeDur * 4);
                //CombatManager.instance.SpawnHitEffect(transform);
                //AudioManager.instance.enemyHit.Play();
                //AudioManager.instance.playerInjured.Play();
                health -= 20;
            }
            else
            {
                ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.hitShakeMag, CombatManager.instance.hitShakeDur);
                ThirdPersonCamera.instance.SetFOV(-CombatManager.instance.hitShakeMag * 150, CombatManager.instance.hitShakeDur * 4);
                //CombatManager.instance.SpawnHitEffect(transform);
                //AudioManager.instance.enemyHit.Play();
                //AudioManager.instance.playerInjured.Play();
                health -= 10;

                //This allows the player to charge his overdrive while ONLY NOT BEING IN OVERDRIVE
                if (ThirdPersonController.instance.currentOverdriveCharge >= ThirdPersonController.instance.maxOverdriveCharge)
                {
                    ThirdPersonController.instance.currentOverdriveCharge = ThirdPersonController.instance.maxOverdriveCharge;
                    ThirdPersonController.instance.UpdateOverdriveBar();

                    if (ThirdPersonController.instance.playOverdrivePowerUpOnce == true)
                    {
                        ThirdPersonController.instance.playOverdrivePowerUpOnce = false;
                        playerSounds.PowerUp.Play();
                    }
                }
                else
                {
                    ThirdPersonController.instance.currentOverdriveCharge++;
                    ThirdPersonController.instance.UpdateOverdriveBar();

                    if (ThirdPersonController.instance.playOverdrivePowerUpOnce == true && ThirdPersonController.instance.currentOverdriveCharge >= ThirdPersonController.instance.maxOverdriveCharge)
                    {
                        ThirdPersonController.instance.playOverdrivePowerUpOnce = false;
                        playerSounds.PowerUp.Play();
                    }
                }
            }
        }
        ////Not working
        //if(GetTag(rb) == "PuzzleKey")
        //{
        //    Console.WriteLine("Collected");
        //    AudioManager.instance.itemCollected.Play();//play audio sound
        //}
    }

    IEnumerator StartForceField(float duration)
    {
        MeshRenderer mr = ultiForceField.gameObject.GetComponent<MeshRenderer>();
        float timer = 0;
        ultiForceField.gameObject.SetActive(true);
        while (timer < duration)
        {
            timer += Time.deltaTime;
            float percentage = timer / duration;
            mr.material.emission = Mathf.Lerp(0, 2, percentage);
            vec4 color = mr.material.color;
            color.a = percentage/4f;
            mr.material.color = color;
            ultiForceField.localScale = vec3.Lerp(0, 3, percentage);
            yield return null;
        }
        _forceFieldUp = true;
    }

    IEnumerator StopForceField(float duration)
    {
        float timer = 0;
        MeshRenderer mr = ultiForceField.gameObject.GetComponent<MeshRenderer>();
        while (timer < duration)
        {
            timer += Time.deltaTime;
            float percentage = timer / duration;
            mr.material.emission = Mathf.Lerp(2, 0, percentage);
            vec4 color = mr.material.color;
            color.a = (1 - percentage) / 4f;
            mr.material.color = color;
            yield return null;
        }
        ultiForceField.gameObject.SetActive(false);
        _forceFieldUp = false;
    }
}