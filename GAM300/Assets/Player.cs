using BeanFactory;
using System;
using System.Collections.Generic;

public class Player : Script
{
    //Rigidbody rb;
    public GameObject gameObj = null;
    public float sad;
    public Player player;
    public float yes;

    void Start()
    {
        //rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (Input.GetKey(KeyCode.Q))
        {
            gameObj.transform.localRotation.y += sad / 360f;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.E))
        {
            gameObj.transform.localRotation.y -= sad / 360f;
            //rb.mass -= speed;
        }

        if (Input.GetKey(KeyCode.W))
        {
            gameObj.transform.localPosition += gameObj.transform.forward * sad;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            gameObj.transform.localPosition += gameObj.transform.back * sad;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.A))
        {
            gameObj.transform.localPosition += gameObj.transform.left * sad;
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.D))
        {
            gameObj.transform.localPosition += gameObj.transform.right * sad;
            //rb.mass += speed;
        }
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }
}