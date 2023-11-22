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
    public float chaseDistance = 5f;
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


    // HealthBar
    public Transform hpBar;

    Rigidbody rb;

    private float particle_duration = 0.5f;
    private float particle_timer = 0f;
    private bool particle_on = false;

    void Start()
    {
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

        vec3 direction = player.localPosition - transform.position;
        direction.y = 0f;
        direction = direction.NormalizedSafe;

        if (particle_on)
        {
            if(particle_timer > 0f)
            {
                particle_timer -= Time.deltaTime;
            }
            else
            {
                particle.gameObject.transform.localPosition = vec3{-999, -999,-999};
                -999f, -999f, -999f);
                particle_timer = 0f;
                particle_on = false;
                particle.Play();
            }
        }

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
                if (vec3.Distance(player.localPosition, transform.localPosition) <= chaseDistance)
                {
                    //change to chase state
                    state = 1;
                }
                break;
            //chase state
            case 1:
                //change to attack state once it has reach it is in range
                if(vec3.Distance(player.localPosition, transform.localPosition) <= shootDistance)
                {
                    state = 2;
                    shootCooldown = 0f;
                }
                //return to its starting position if player is far from its chaseDistance
                if(vec3.Distance(player.localPosition, transform.localPosition) > chaseDistance)
                {
                    //return back to its previous position state
                    state = 0;
                }
                LookAt(direction);
                GetComponent<Rigidbody>().linearVelocity = direction * moveSpeed;
                break;
            //attack state
            case 2:
                //spawn bullet or just look at player
                shootCooldown += Time.deltaTime;
                if (shootCooldown > maxShootCooldown)
                {
                    GameObject bulletPrefab = Instantiate(bullet, rangeEnemyPos.localPosition, rangeEnemyPos.localRotation);
                    bulletPrefab.GetComponent<Rigidbody>().linearVelocity = transform.forward * bulletSpeed;
                    shootCooldown = 0;
                }
                LookAt(direction);
                //change to chase state once player has reach out of range
                if (vec3.Distance(player.localPosition, transform.localPosition) > shootDistance)
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
    }

    void LookAt(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;
        float angle = (float)Math.Atan2(dir.x, dir.z);
        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
        quat oldQuat = glm.FromEulerToQuat(transform.localRotation).Normalized;

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
                transform.localRotation = rot;
            }
        }
    }

    void TakeDamage(int amount)
    {
        currentHealth -= amount;
        hpBar.localScale.x = (float)currentHealth/maxHealth;
        //set particle transform to enemy position
        if(currentHealth > 0)
        {
            particle.gameObject.transform.localPosition = transform.localPosition;
            particle.Play();
            particle_on = true;
            particle_timer = particle_duration;
        }
        else
        {
            Destroy(gameObject);
        }
    }


    void OnTriggerEnter(PhysicsComponent other)
    {
        //check if the rigidbody belongs to a game object called PlayerWeaponCollider
        if(GetTag(other) == "PlayerAttack")
        {
            //Console.WriteLine("MEGAHIT");
            Transform otherT = other.gameObject.GetComponent<Transform>();
            vec3 dir = transform.localPosition - otherT.localPosition;
            dir.y = 0;
            dir = dir.NormalizedSafe;
            rb.force = dir * 100f;
            TakeDamage(1);
        }
    }

}
