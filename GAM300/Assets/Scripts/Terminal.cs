using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Terminal : Script
{
    public int index;

    void Start()
    {

    }

    void Update()
    {
        CheckCheckpoint();
    }

    void OnCollisionEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("AtCheckpoint");
            ThirdPersonController.instance.checkpointIndex = index;
            //CheckCheckpoint();
        }

    }

    public void CheckCheckpoint()
    {
        if(index == ThirdPersonController.instance.checkpointIndex)
        {
            //save checkpoint
            if (Input.GetKeyDown(KeyCode.E))
            {
                Console.WriteLine("Save Checkpoint");
                //shift the spawn point to where the current termainal position where the player save
                ThirdPersonController.instance.spawnPoint.localPosition = new vec3(transform.position + vec3.Ones);
                ThirdPersonController.instance.spawnPoint.localRotation = new vec3(transform.rotation);
            }
        }

    }

    void OnCollisionExit(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("AwayFromCheckpoint");
            ThirdPersonController.instance.isAtCheckpoint = false;
        }
    }
}
