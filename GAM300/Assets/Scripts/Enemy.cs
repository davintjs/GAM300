using BeanFactory;
using System;
using System.Collections.Generic;
using System.Collections;
using GlmSharp;

public class Enemy : Script
{
    public float moveSpeed = 2f;
    public float chaseDistance = 5f;
    public Transform player;
    public int count;

    public int Health = 1;

    //public float ching;


    void Start()
    {
        
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

    void Exit()
    {

    }

    void OnCollisionEnter(Rigidbody rb)
    {
/*        Console.WriteLine("Enemy collided tag: " + GetTag(rb));
        if (GetTag(rb) == "Test2")
        {
            Console.WriteLine("I have been attacked!");
            Health -= 1;
            if (Health <= 0)
                Destroy(gameObject);
        }*/
    }
}

