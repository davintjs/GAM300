using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Threading;

public class RangeEnemy : Script
{
    public float maxHealth = 3f;
    public float currentHealth;
    public float moveSpeed = 2f;
    public float chaseDistance = 8f;
    public float shootDistance = 3f;
    public bool inRange = false;
    public bool back = false;
    public int state;//Example 1 is walk, 2 is attack, 3 is idle etc.
    public Transform player;
    public Transform rangeEnemyPos;
    public float RotationSpeed = 6f;

    public ParticleComponent particle;

    
    public Transform modelOffset;

    //vec3 startingPos;
    public Transform startingPos;
    public float duration = 2f;
    public float timer = 0f;

    public GameObject bullet;
    public float bulletSpeed = 3f;

    public float floatingY = .5f;

    public float maxShootCooldown = 3f;
    float shootCooldown = 0f;

    float startPoint;

    // NavMesh stuff
    public float navMeshduration = 0.5f;
    public float navMeshtimer = 0f;
    public bool newRequest = false;

    // HealthBar
    public Transform hpBar;

    Rigidbody rb;

    //audio
    public bool playOnce = true;

    Coroutine damagedCoroutine;

    public GameObject spawnObject;

    PlayerAudioManager playerSounds;

    void Start()
    {
        playerSounds = PlayerAudioManager.instance;
        playOnce = true;
        rb = GetComponent<Rigidbody>();
        currentHealth = maxHealth;
        //startingPos = GetComponent<Transform>().localPosition;//get its starting position
        state = 0;//start with idle state
        startPoint = modelOffset.localPosition.y;
    }

    void Update()
    {
        if (player == null)
            return;

        vec3 direction = player.position - transform.position;
        direction.y = 0f;
        direction = direction.NormalizedSafe;

        switch (state)
        {
            //idle state
            case 0:
                //Console.WriteLine("Idle");
                if (!back)
                {
                    modelOffset.localPosition = vec3.Lerp(vec3.UnitY * (-floatingY + startPoint), vec3.UnitY * (floatingY + startPoint), timer / duration);
                }
                else
                {
                    modelOffset.localPosition = vec3.Lerp(vec3.UnitY * (floatingY + startPoint), vec3.UnitY * (-floatingY + startPoint), timer / duration);
                }
                timer += Time.deltaTime;
                if(timer > duration)
                {
                    back = !back;
                    timer = 0f;
                }

                //player detection
                if (vec3.Distance(player.position, transform.position) <= chaseDistance)
                {
                    if (navMeshtimer >= navMeshduration)
                    {
                        newRequest = true;
                        navMeshtimer = 0f;
                    }
                    //change to chase state
                    state = 1;
                }
                if (vec3.Distance(player.position, transform.position) <= shootDistance)
                {
                    state = 2;
                    shootCooldown = 0f;
                }
                break;
            //chase state
            case 1:
                ////change to attack state once it has reach it is in range
                //if(vec3.Distance(player.position, transform.position) <= shootDistance)
                //{
                //    state = 2;
                //    shootCooldown = 0f;
                //}
                ////return to its starting position if player is far from its chaseDistance
                //if(vec3.Distance(player.position, transform.position) > chaseDistance)
                //{
                //    //return back to its previous position state
                //    state = 0;
                //}

                //NavMeshAgent check = GetComponent<NavMeshAgent>();
                //if (check != null) // Use navmesh if is navmesh agent
                //{
                //    if (newRequest)
                //    {
                //        Console.WriteLine("Moving agent...");
                //        check.FindPath(player.localPosition);
                //        newRequest = false;
                //    }
                //}
                //else
                //{
                //    LookAt(direction);
                //    GetComponent<Rigidbody>().linearVelocity = direction * moveSpeed;
                //}
                break;
            //attack state
            case 2:
                //spawn bullet or just look at player
                shootCooldown += Time.deltaTime;
                if (shootCooldown > maxShootCooldown)
                {
                    if(playOnce)
                    {
                        playOnce = false;
                        AudioManager.instance.rangeEnemyFiring.Play();
                    }
                    CombatManager.instance.SpawnHitEffect2(modelOffset);
                    GameObject bulletPrefab = Instantiate(bullet, modelOffset.position, transform.rotation);
                    vec3 dir = player.position - transform.position;
                    dir = dir.NormalizedSafe;
                    bulletPrefab.GetComponent<Rigidbody>().linearVelocity = dir * bulletSpeed;
                    shootCooldown = 0;
                    if(shootCooldown == 0)
                    {
                        playOnce = true;//reset ability to play audio
                    }
                }
                LookAt(direction);
                //change to chase state once player has reach out of range
                if (vec3.Distance(player.position, transform.position) > shootDistance)
                {
                    state = 1;
                }
                break;
            //death state
            //return to previous position state
/*            case 4:
                //Console.WriteLine("Return");
                vec3 returnDirection = (startingPos.localPosition - transform.localPosition).Normalized;
                returnDirection.y = 0;
                float angle2 = (float)Math.Atan2(returnDirection.x, returnDirection.z);
                transform.localRotation = new vec3(0, angle2, 0);
                GetComponent<Rigidbody>().linearVelocity = returnDirection * moveSpeed;
                //change to idle state once it has reach its starting position
                if(transform.localPosition == startingPos.localPosition)
                {
                    state = 0;
                }
                //player detection
                if (vec3.Distance(player.localPosition, transform.localPosition) <= chaseDistance)
                {
                    //change to chase state
                    state = 1;
                }
                break;*/
        }
        navMeshtimer += Time.deltaTime;
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

    void TakeDamage(int amount)
    {
        ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.hitShakeMag, CombatManager.instance.hitShakeDur);
        ThirdPersonCamera.instance.SetFOV(-CombatManager.instance.hitShakeMag * 150, CombatManager.instance.hitShakeDur*4);
        shootCooldown -= .5f;
        AudioManager.instance.enemyHit.Play();
        currentHealth -= amount;
        vec3 hpScale = hpBar.localScale;
        hpScale.x = currentHealth/maxHealth;
        hpBar.localScale = hpScale;
        CombatManager.instance.SpawnHitEffect(transform);
        //set particle transform to enemy position
        if(currentHealth <= 0)
        {
            AudioManager.instance.rangeEnemyDead.Play();
            if (spawnObject != null)
                spawnObject.SetActive(true);
            Destroy(gameObject);
        }
    }


    void OnTriggerEnter(PhysicsComponent other)
    {
        //check if the rigidbody belongs to a game object called PlayerWeaponCollider
        if(GetTag(other) == "PlayerAttack")
        {
            Transform otherT = other.gameObject.GetComponent<Transform>();
            vec3 dir = otherT.forward;
            dir = dir.NormalizedSafe;
            if (damagedCoroutine != null)
            {
                StopCoroutine(damagedCoroutine);
            }
            damagedCoroutine = StartCoroutine(Damaged(.5f, dir * 5));

            if (ThirdPersonController.instance.currentlyOverdriven == true)
            {
                TakeDamage(2);
            }
            else
            {
                TakeDamage(1);
                if (ThirdPersonController.instance.isOverdriveEnabled == true)
                {
                    //This allows the player to charge his overdrive while ONLY NOT BEING IN OVERDRIVE
                    if (ThirdPersonController.instance.currentOverdriveCharge >= ThirdPersonController.instance.maxOverdriveCharge)
                    {
                        ThirdPersonController.instance.currentOverdriveCharge = ThirdPersonController.instance.maxOverdriveCharge;
                        ThirdPersonController.instance.UpdateOverdriveBar();

                        if (ThirdPersonController.instance.playOverdrivePowerUpOnce == true)
                        {
                            ThirdPersonController.instance.playOverdrivePowerUpOnce = false;
                            playerSounds.PowerUp.Play();
                            ThirdPersonController.instance.overDriveUI.gameObject.SetActive(true);
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
                            ThirdPersonController.instance.overDriveUI.gameObject.SetActive(true);
                        }
                    }
                }
            }
        }
    }

    IEnumerator Damaged(float duration, vec3 knockback)
    {
        duration /= 2;
        float startDuration = duration;
        vec3 newRot = modelOffset.localRotation;
        newRot.x = glm.Radians(-45f);
        modelOffset.localRotation = newRot;
        while (duration > 0)
        {
            rb.linearVelocity = new vec3(knockback * (duration/startDuration));
            duration -= Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }
        duration = startDuration;
        while (duration > 0)
        {
            float val = glm.Radians(-45f);
            newRot.x = glm.Lerp(0, val,duration/startDuration);
            modelOffset.localRotation = newRot;
            duration -= Time.deltaTime;
            yield return new WaitForSeconds(Time.deltaTime);
        }
    }
}
