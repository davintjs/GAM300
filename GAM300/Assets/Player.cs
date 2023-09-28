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
        if (gameObj == null)
            return;
        if (Input.GetKey(KeyCode.Q))
        {
            gameObj.transform.localRotation.y += speed/360f;
        }
        if (Input.GetKey(KeyCode.E))
        {
            gameObj.transform.localRotation.y -= speed/360f;
        }

        if (Input.GetKey(KeyCode.W))
        {
            gameObj.transform.localPosition += gameObj.transform.forward * speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            gameObj.transform.localPosition += gameObj.transform.back * speed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            gameObj.transform.localPosition += gameObj.transform.left * speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            gameObj.transform.localPosition += gameObj.transform.right * speed;
        }
        //Console.WriteLine("Sup");
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}