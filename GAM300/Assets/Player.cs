using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    Rigidbody rb;

    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        float speed = 0.4f;
        if (Input.GetKey(KeyCode.A))
        {
            rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            rb.mass += speed;
        }
        Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}c