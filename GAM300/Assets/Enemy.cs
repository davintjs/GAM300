﻿using BeanFactory;
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

    //public float ching;


    void Start()
    {
        
    }

    void Update()
    {
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
}
