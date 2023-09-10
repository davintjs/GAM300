using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    void Start()
    {
        Console.WriteLine("Start from Player");
    }

    void Update()
    {
        Console.WriteLine(gameObject.name);
        Vector3 newPos = transform.localPosition;
        float speed = 0.4f;
        if (Input.GetKey(KeyCode.W))
        {
            newPos.z += speed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            newPos.x += speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            newPos.z -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            newPos.x -= speed;
        }
        transform.localPosition = newPos;
    }

    void Exit()
    {
        Console.WriteLine("Exit from Player");
    }
}