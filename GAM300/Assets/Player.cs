using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    Rigidbody rb;

    void Start()
    {
        rb = Get<Rigidbody>();
    }

    void Update()
    {
        float speed = 0.4f;
        if (Input.GetKey(KeyCode.A))
        {
            rb.linearVelocity.x -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            rb.linearVelocity.x += speed;
        }
        //Console.WriteLine("Sup");
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}