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

    //vec3 startingPos;
    public Transform maxUpPos;
    public Transform maxBottomPos;
    public Transform startingPos;
    public float duration = 2f;
    public float timer = 0f;

    public GameObject bullet;
    public float bulletSpeed = 3f;

    public Transform testSetParent;


    // HealthBar
    public Transform hpBar;


    void Start()
    {
        currentHealth = maxHealth;
        //startingPos = GetComponent<Transform>().localPosition;//get its starting position
        state = 0;//start with idle state
        if (testSetParent != null)
        {
            testSetParent.SetParent(transform);
        }
    }

    void Update()
    {
        if (player == null)
            return;


        //ensure the moving animation reference continues to follow the it
        //maxUpPos.localPosition.x = GetComponent<Transform>().localPosition.x;
        //maxUpPos.localPosition.z = GetComponent<Transform>().localPosition.z;
        //maxBottomPos.localPosition.x = GetComponent<Transform>().localPosition.x;
        //maxBottomPos.localPosition.z = GetComponent<Transform>().localPosition.z;

        switch (state)
        {
            //idle state
            case 0:
                //Console.WriteLine("Idle");
                if (!back)
                {
                    transform.localPosition = vec3.Lerp(maxUpPos.localPosition, maxBottomPos.localPosition, timer / duration);
                }
                else
                {
                    transform.localPosition = vec3.Lerp(maxBottomPos.localPosition, maxUpPos.localPosition, timer / duration);
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
                //Console.WriteLine("Chase");
                //follow target
                vec3 direction = (player.localPosition - transform.localPosition).Normalized;
                direction.y = 0;
                float angle = (float)Math.Atan2(direction.x, direction.z);
                transform.localRotation = new vec3(0, angle, 0);
                GetComponent<Rigidbody>().linearVelocity = direction * moveSpeed;

                //change to attack state once it has reach it is in range
                if(vec3.Distance(player.localPosition, transform.localPosition) <= shootDistance)
                {
                    state = 2;
                }
                //return to its starting position if player is far from its chaseDistance
                if(vec3.Distance(player.localPosition, transform.localPosition) > chaseDistance)
                {
                    //return back to its previous position state
                    state = 4;

                    //NOTE: temporary return to its idle state
                    //state = 1;
                }
                break;
            //attack state
            case 2:
                //spawn bullet
                //Console.WriteLine("Shoot Start");
                GameObject bulletPrefab = Instantiate(bullet, rangeEnemyPos.localPosition, rangeEnemyPos.localRotation);
                //You should set the rigid body velocity here
                //Console.WriteLine("Shoot Start");
                bulletPrefab.GetComponent<Rigidbody>().linearVelocity = transform.forward * moveSpeed;
                //bulletPrefab.transform.localPosition = new vec3(0, 0, 1) * bulletSpeed;//add movement to the bullet based on its forward direction
                //Console.WriteLine("Shoot End");
                //change to chase state once player has reach out of range
                if (vec3.Distance(player.localPosition, transform.localPosition) > shootDistance)
                {
                    state = 1;
                }
                break;
            //death state
            case 3:
                Destroy(this.gameObject);
                break;
            //return to previous position state
            case 4:
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
                break;
        }

        ////player detection
        //if (vec3.Distance(player.localPosition, transform.localPosition) <= chaseDistance)
        //{
        //    //change to chase state
        //    state = 1;
        //}
        //else
        //{
        //    //return back to its previous position state
        //    state = 4;
        //}
    }

    void TakeDamage(int amount)
    {
        currentHealth -= amount;
        Console.WriteLine("Hit");

        hpBar.localScale.x = (float)currentHealth/maxHealth;

        if(currentHealth <= 0)
        {
            state = 3;
            //Destroy(this.gameObject);
        }
    }

    void OnTriggerEnter(PhysicsComponent other)
    {
        Console.WriteLine("Enemy Triggered " + GetTag(other));

        //check if the rigidbody belongs to a game object called PlayerWeaponCollider
        if(GetTag(other) == "PlayerAttack")
        {
            Console.WriteLine("Hit");
            TakeDamage(1);
        }
    }

}
