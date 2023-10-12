using BeanFactory;
using System;
using System.Collections.Generic;
using System.Collections;

public class Player : Script
{
    //Rigidbody rb;
    public GameObject gameObj = null;
    public float sad;
    public Transform otherT;

    IEnumerator DoSomething()
    {
        while (true)
        {
            yield return new WaitForSeconds(5f);
            Console.WriteLine("DoSomething2");
        }
    }

    void Start()
    {
        Console.WriteLine("STARTED");
        StartCoroutine(DoSomething());
        AddComponent<Rigidbody>();
        //rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (otherT == null)
            return;
        if (Input.GetKey(KeyCode.Q))
        {
            otherT.localRotation.y += sad / 360f;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.E))
        {
            otherT.localRotation.y -= sad / 360f;
            //rb.mass -= speed;
        }

        if (Input.GetKey(KeyCode.W))
        {
            otherT.localPosition += otherT.forward * sad;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            otherT.localPosition += otherT.back * sad;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            otherT.localPosition += otherT.left * sad;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            otherT.localPosition += otherT.right * sad;
            //rb.mass += speed;
        }
        //Console.WriteLine("Hello");
        //enabled = false;
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}