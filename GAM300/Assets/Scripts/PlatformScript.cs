using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class PlatformScript : Script
{
    public Transform startPoint;
    public Transform endPoint;
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;

    public bool rest = false;
    public float restTimer = 2f;
    public float currentRestTimer;

    void Start()
    {
        currentRestTimer = restTimer;
    }

    void Update()
    {
        if (!back && !rest)
        {
            transform.localPosition = vec3.Lerp(startPoint.localPosition,endPoint.localPosition , timer / duration);
        }
        else if(back && !rest)
        {
            transform.localPosition = vec3.Lerp(endPoint.localPosition, startPoint.localPosition, timer / duration);
        }
        timer += Time.deltaTime;
        if (timer >= duration)
        {
            rest = true;//stops platform from moving
            //Console.WriteLine("PlatformResting");
        }
        if(rest)
        {
            currentRestTimer -= Time.deltaTime;
            if(currentRestTimer <= 0)
            {
                //Console.WriteLine("PlatformStartMoving");
                rest = false;
                currentRestTimer = restTimer;//rest rest timer
                back = !back;//change to the other movement direction
                timer = 0f;//rest movement timer
            }
        }
    }
    void OnCollisionEnter(PhysicsComponent rb)
    {
        //detect the player
        if(rb.HasComponent<ThirdPersonController>())
        {
            Console.WriteLine("PlayerOnPlatform");
        }
    }

    void OnTrigger(PhysicsComponent other)
    {
        //check if player landed on it
        if (other.HasComponent<ThirdPersonController>())
        {
            Console.WriteLine("PlayerOnPlatform");
        }
    }
}
