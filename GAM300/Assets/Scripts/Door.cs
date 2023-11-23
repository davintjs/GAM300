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
    public Transform door;

    float totaltime = 0f;
    float displaced = 0f;
    CharacterController player;

    public bool back = false;
    public float floatKey = 1f;
    float keyStartY;
    public float maxTime = 5;
    float timer = 0;

    void Start()
    {
        keyStartY = transform.localPosition.y;
    }


    void Update()
    {

        vec3 positive = new vec3(transform.localPosition);
        positive.y = floatKey + keyStartY;
        vec3 negative = new vec3(transform.localPosition);
        negative.y = -floatKey + keyStartY;
        if (!back)
        {
            transform.localPosition = vec3.Lerp(negative, positive, timer / maxTime);
        }
        else
        {
            transform.localPosition = vec3.Lerp(positive, negative, timer / maxTime);
        }

        timer += Time.deltaTime;
        if (timer >= maxTime)
        {
            back = !back;
            timer = 0;
        }
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
            door.localPosition.y += currDisplacement;

            //transform.localPosition.y = 100f;
            //temp = true;
        }

    }
    void OnTriggerEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("U TOUCHED THE DOOR NOOOOO");
            //transform.localPosition.y = 100f;
            moving = true;
        }
    }
}
