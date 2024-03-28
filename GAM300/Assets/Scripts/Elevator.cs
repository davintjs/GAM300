using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Elevator : Script
{
    //public Transform startPoint;
    vec3 startinglocation;
    public Transform endPoint;
    public float duration = 10f;
    public float timer = 0f;
    public bool back = false;

    public bool rest = false;
    public float restTimer = 2f;
    public float currentRestTimer;

    bool movingUp = false;
    bool playerOnPlatform = false;
    bool stoppedPermanent = false;
    CharacterController player;
    PlayerAudioManager playerSounds; 

    vec3 returningLocation;
    // Things to return back down
    bool stopped = false;
    vec3 stoppedLocation;
    public float returnDuration = 5f;
    float returnTimer = 0f;
    public float hangTimerCap = 1f;
    float hangTimer= 0f;
    //bool 
    void Start()
    {
        playerSounds = PlayerAudioManager.instance;
        currentRestTimer = restTimer;
        startinglocation = transform.localPosition;
        returningLocation = startinglocation;
        hangTimer = 0f;
    }

    void Update()
    {
        vec3 target = transform.localPosition;
        vec3 diff = vec3.Zero;

        //if(movingUp)// This is only ever turned on when u step onto the platform
        //{
        //    // Now we check if the player's Y value is above the platform. if it is we still keep going up
        //    // This is so that if the player jumps whilst on the platform. the platform still goes up

        //    if(player!= null) // just a incase check
        //    {
        //        //else // just to print out that we are fine
        //        //{

        //        //}
        //    }
        //}

        

        // Move the platform up
        if (player != null && movingUp && !stoppedPermanent)
        {
            if(!playerOnPlatform) 
            {
                if(hangTimer >= hangTimerCap)
                {
                    movingUp = false;
                    returningLocation = transform.localPosition;
                    returnTimer = duration - timer;
                    return;
                }
                hangTimer += Time.deltaTime;
            }
            else
            { 
                //Console.WriteLine("Timer is "+ timer);
                target = vec3.Lerp(startinglocation, endPoint.localPosition, timer / duration);
                diff = target - transform.localPosition;

                transform.localPosition = target;
                player.gameObject.transform.localPosition += diff;
                hangTimer = 0f;
                timer += Time.deltaTime;
                stopped = false;
                returnTimer = 0f;
            }
            returnTimer = 0f;
        }
        else // Not going up
        {
            if(transform.localPosition ==  startinglocation)
            {
                //Console.WriteLine("in here");
                timer = 0f; 
                returnTimer = 0f;

            }
            else // Start Lerping it down
            {
                //Console.WriteLine("Return is " + returnTimer);


                target = vec3.Lerp(endPoint.localPosition, startinglocation, returnTimer / duration);
                transform.localPosition = target;

                returnTimer += Time.deltaTime;
                timer -= Time.deltaTime;

                if(timer < 0f)
                {
                    //Console.WriteLine("out there");

                    timer = 0f;
                }
                if (returnTimer >= duration )
                {
                    timer = 0f;
                    returnTimer = 0f;
                    transform.localPosition = startinglocation;
                }
                //Console.WriteLine("YIKES is " + timer);
                //transform.localPosition = startinglocation;
            }

            //if(movingUp) // Means that player has already stepped on it before -> its coming down
            //{
            //    if(!stopped)
            //    {
            //        stopped = true;
            //        stoppedLocation = transform.localPosition;
            //    }
            //    target = vec3.Lerp(stoppedLocation, startinglocation, returnTimer / returnDuration);
            //    transform.localPosition = target;

            //    returnTimer += Time.deltaTime;
            //    timer -= Time.deltaTime;
            //    if( (returnTimer >= returnDuration) || (transform.localPosition.y < startinglocation.y) )// Hit the Ground
            //    {
            //        stopped = true;
            //        transform.localPosition = startinglocation;

            //    }
            //    startinglocationfrfr = transform.localPosition;
            //}
            //else // Stay still on the ground
            //{
            //    timer = 0f; 

            //    returnTimer = 0f;
            //    transform.localPosition = startinglocation;
            //    startinglocationfrfr = transform.localPosition; 

            //}

        }
         
        if (timer >= duration)
        {
            movingUp = false;
            stoppedPermanent = true; 
        }



        //if (timer >= duration)
        //{
        //    rest = true;//stops platform from moving
        //    //Console.WriteLine("PlatformResting");
        //}
        //if (rest)
        //{
        //    currentRestTimer -= Time.deltaTime;
        //    if (currentRestTimer <= 0)
        //    {
        //        //Console.WriteLine("PlatformStartMoving");
        //        rest = false;
        //        currentRestTimer = restTimer;//rest rest timer
        //        back = !back;//change to the other movement direction
        //        timer = 0f;//rest movement timer
        //    }
        //}
    }
    void OnCollisionEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            player = rb.gameObject.GetComponent<CharacterController>();
            Console.WriteLine("PlayerOnPlatform");
            
            movingUp = true;
            playerSounds.Elevator.Play();
            playerOnPlatform = true;
        }
    }

    

    void OnCollisionExit(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            //player = null;
            Console.WriteLine("Player Exit Platform");
            playerOnPlatform = false;
             
        }
    }
}
