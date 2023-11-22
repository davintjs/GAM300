using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Door : Script
{
    public bool moving = false;
    public bool temp = false;
    public float movingTimer = 2f;// takes 3 seconds to move the door down
    public float displacement = -3.8f;
    float totaltime = 0f;
    float displaced = 0f;
    CharacterController player;

    void Start()
    {
    }

    void Update()
    {
        
        if(moving && !temp)
        {
            //totaltime += Time.deltaTime;
            float currDisplacement = displacement * (Time.deltaTime / movingTimer);

            displaced += currDisplacement;

            // Check to make sure i dont overshoot
            if (Math.Abs(displaced) >= Math.Abs(displacement))
            {
                currDisplacement -= displacement - displaced;
                temp = true;
            }
            transform.localPosition.y += currDisplacement;

            //transform.localPosition.y = 100f;
            //temp = true;
        }

    }
    void OnCollisionEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            player = rb.gameObject.GetComponent<CharacterController>();
            Console.WriteLine("U TOUCHED THE DOOR NOOOOO");
            //transform.localPosition.y = 100f;
            moving = true;
        }
    }

    void OnCollisionExit(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            player = null;
            Console.WriteLine("Player Exit Platform");
        }
    }
}
