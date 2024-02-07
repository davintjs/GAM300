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

    public Transform doortoSec3;
    public Door doortoSec3Script;

    public Transform doortoBossSec2;
    public Door doortoBossSec2Script;

    public Transform doortoBoss;
    public Door doortoBossScript;

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

        if (doortoSec3 != null)
        {
            vec3 door_pos = doortoSec3.position;
            if ( !doortoSec3Script.moving && vec3.Distance(door_pos, player.position) < promptDistance)
            {
                pos.x = 0;
                doorPrompt_yellow.position = pos;
            }            
        }

        if (doortoBossSec2 != null)
        {
            vec3 door_pos = doortoBossSec2.position;
            if (!doortoBossSec2Script.moving && vec3.Distance(door_pos, player.position) < promptDistance)
            {
                pos.x = 0;
                doorPrompt_yellow.position = pos;
            }
        }

        if (doortoBoss != null)
        {
            vec3 door_pos = doortoBoss.position;
            if (!doortoBossScript.moving && vec3.Distance(door_pos, player.position) < promptDistance)
            {
                pos.x = 0;
                doorPrompt_white.position = pos;
            }
        }


    }

}
