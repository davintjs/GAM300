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

    bool moving = false;
    bool stoppedPermanent = false;
    CharacterController player;

    void Start()
    {
        currentRestTimer = restTimer;
        startinglocation = transform.localPosition;
    }

    void Update()
    {
        vec3 target = transform.localPosition;
        vec3 diff = vec3.Zero;
        //if (!back && !rest)
        //{
        //    target = vec3.Lerp(startinglocation, endPoint.localPosition, timer / duration);
        //    diff = target - transform.localPosition;
        //}
        

        if (player != null && moving && !stoppedPermanent)
        {

            target = vec3.Lerp(startinglocation, endPoint.localPosition, timer / duration);
            diff = target - transform.localPosition;
            transform.localPosition = target;
            player.gameObject.transform.localPosition += diff ;
            timer += Time.deltaTime;
        }

        if (timer >= duration)
        {
            moving = false;
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
            moving = true;
            AudioManager.instance.elevator.Play();
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
