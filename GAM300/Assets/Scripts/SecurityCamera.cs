using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Threading;

public class SecurityCamera : Script
{
    public float moveSpeed = 2f;
    public float chaseDistance = 8f;
    public float shootDistance = 3f;
    public bool inRange = false;
    public bool back = false;
    public int state;//Example 1 is walk, 2 is attack, 3 is idle etc.
    public Transform player;
    public Transform rangeEnemyPos;
    public float RotationSpeed = 6f;

    public Transform modelOffset;

    //vec3 startingPos;
    public float duration = 2f;
    public float timer = 0f;

    public float floatingY = .5f;

    //audio
    public bool playOnce = true;

    //sounds

    public bool onOnce = true;

    void Start()
    {
        playOnce = true;
        //startingPos = GetComponent<Transform>().localPosition;//get its starting position
        state = 0;//start with idle state

    }

    void Update()
    {
        if (player == null)
            return;

        vec3 direction = player.localPosition - transform.position;
        direction.y = 0f;
        direction = direction.NormalizedSafe;

        switch (state)
        {
            case 0:
                //player detection
                if (vec3.Distance(player.position, transform.position) <= chaseDistance)
                {
                    //change to chase state
                    state = 1;

                    if(onOnce)
                    {
                        onOnce = false;
                        
                    }    
                }
                break;
            //chase state
            case 1:
                //return to its starting position if player is far from its chaseDistance
                if (vec3.Distance(player.position, transform.position) > chaseDistance)
                {
                    //return back to its previous position state
                    state = 0;
                }
                LookAt(direction);
                break;
        }

        void LookAt(vec3 dir)
        {
            if (dir == vec3.Zero)
                return;
            float angle = (float)Math.Atan2(dir.x, dir.z);
            quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
            quat oldQuat = glm.FromEulerToQuat(transform.rotation).Normalized;

            // Interpolate using spherical linear interpolation (slerp)
            quat midQuat = quat.SLerp(oldQuat, newQuat, Time.deltaTime * RotationSpeed);

            vec3 rot = ((vec3)midQuat.EulerAngles);

            if (rot != vec3.NaN)
            {
                bool isNan = false;
                foreach (float val in rot)
                {
                    if (float.IsNaN(val))
                    {
                        isNan = true;
                        break;
                    }
                }
                if (!isNan)
                {
                    transform.rotation = rot;
                }
            }
        }
    }
}
