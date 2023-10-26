using BeanFactory;
using System;
using System.Collections.Generic;
using System.Collections;

public class Player : Script
{
    //Rigidbody rb;
    public GameObject gameObj = null;
    public float sad;
    public float jumpSpeed;
    public Transform otherT;
    public string onamae = "HERRO";
    public CharacterController characterController;

    public float ching;
    
    IEnumerator DoSomething()
    {
        while (true)
        {
            yield return new WaitForSeconds(5f);
            Console.WriteLine("Coroutine Every 5 seconds");
        }
    }

    void Start()
    {
        StartCoroutine(DoSomething());
        Console.WriteLine("START");
        //AddComponent<Rigidbody>();
        //rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        //
        if (otherT == null)
            return;
        if (Input.GetKey(KeyCode.Q))
        {
            Console.WriteLine("Q");
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
            //otherT.localPosition += otherT.forward * sad;
            characterController.Move(otherT.forward * sad);
            //rb.mass -= speed;
        }
        if (Input.GetKey(KeyCode.S))
        {
            //otherT.localPosition += otherT.back * sad;
            characterController.Move(otherT.back * sad);
        }
        if (Input.GetKey(KeyCode.A))
        {
            //otherT.localPosition += otherT.left * sad;
            characterController.Move(otherT.left * sad);
        }
        if (Input.GetKey(KeyCode.D))
        {
            //otherT.localPosition += otherT.right * sad;
            characterController.Move(otherT.right * sad);
        }
        // Jump
        if (Input.GetKey(KeyCode.Space))
        {
            characterController.Move(otherT.up * jumpSpeed);
        }
        //Console.WriteLine("Hello");
        //enabled = false;
        //Destroy(this);
        //transform.localPosition = newPos;
    }

    void Exit()
    {
    }

    void OnCollisionEnter(Rigidbody rb)
    {
        Console.WriteLine("COLLISION ENTER FROM SCRIPTING!");
    }
}