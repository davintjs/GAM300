using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    Rigidbody rb;
    public float babei;

    void Start()
    {
        Console.WriteLine("STARTED");
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (transform.parent != null)
            Console.WriteLine("Has parent");
        float speed = 0.4f;
        if (Input.GetKey(KeyCode.A))
        {
            transform.localPosition.x -= speed;
            rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            transform.localPosition.x += speed;
            rb.mass += speed;
        }
        //Console.WriteLine("Sup");
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}