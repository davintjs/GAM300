using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Policy;
using System.Threading;

public class DoorPrompt : Script
{
    public float promptDistance = 1f;

    public Transform doorPrompt_yellow;
    public Transform doorPrompt_white;

    public Transform player;

    public Transform yellowDoor1;
    public Door yellowDoor1Script;

    public Transform yellowDoor2;
    public Door yellowDoor2Script;

    public Transform whiteDoor1;
    public Door whiteDoor1Script;

    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        //set initial values out of canvas
        vec3 pos = doorPrompt_yellow.position;
        pos.x = 1999f;
        doorPrompt_yellow.position = doorPrompt_white.position = pos;

        if (yellowDoor1 != null)
        {
            vec3 door_pos = yellowDoor1.position;
            if ( !yellowDoor1Script.moving && vec3.Distance(door_pos, player.position) < promptDistance)
            {
                pos.x = 0;
                doorPrompt_yellow.position = pos;
            }            
        }

        if (yellowDoor2 != null)
        {
            vec3 door_pos = yellowDoor2.position;
            if (!yellowDoor2Script.moving && vec3.Distance(door_pos, player.position) < promptDistance)
            {
                pos.x = 0;
                doorPrompt_yellow.position = pos;
            }
        }

        if (whiteDoor1 != null)
        {
            vec3 door_pos = whiteDoor1.position;
            if (!whiteDoor1Script.moving && vec3.Distance(door_pos, player.position) < promptDistance)
            {
                pos.x = 0;
                doorPrompt_white.position = pos;
            }
        }


    }

}
