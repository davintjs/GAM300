using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    //Rigidbody rb;
    public GameObject gameObj = null;
    public float sad;
    public Transform otherT;

    void Start()
    {
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
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}