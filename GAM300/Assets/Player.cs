using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    Rigidbody rb;
    public float speed = 10f;
    public GameObject gameObj;
    public Player player2;

    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (Input.GetKey(KeyCode.Q))
        {
            gameObj.transform.localRotation.y += speed/360f;
            rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.E))
        {
            gameObj.transform.localRotation.y -= speed/360f;
            rb.mass -= speed;
        }

        if (Input.GetKey(KeyCode.W))
        {
            gameObj.transform.localPosition += gameObj.transform.forward * speed;
            rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            gameObj.transform.localPosition += gameObj.transform.back * speed;
            rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            gameObj.transform.localPosition += gameObj.transform.left * speed;
            rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            gameObj.transform.localPosition += gameObj.transform.right * speed;
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