using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class CompanionMovement : Script
{
    ThirdPersonController player;

    vec3 offset = vec3.Ones;

    public float speed = 2f;

    public float distanceThreshold = 4f;

    public float acceleration = 1f;
    vec3 velocity;

    void Start () 
    { 
        player = ThirdPersonController.instance;
    }

    void Update ()
    {
        if (glm.DistanceSqr(transform.position, player.transform.position) > distanceThreshold*distanceThreshold)
        {
            vec3 dir = player.transform.position - transform.position;
            dir = glm.NormalizedSafe(dir);
            velocity += dir * acceleration * Time.deltaTime;
            if (velocity.LengthSqr > speed*speed)
            {
                velocity = glm.NormalizedSafe(velocity);
                velocity *= speed;
            }
        }
        else if (velocity != vec3.Zero)
        {
            vec3 dir = glm.NormalizedSafe(velocity);
            velocity -= dir * acceleration * 2 * Time.deltaTime;
        }
        transform.position += velocity * Time.deltaTime;
    }
}