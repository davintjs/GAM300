using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Terminal : Script
{
    Transform terminalPos;

    void Start()
    {
        terminalPos = gameObject.GetComponent<Transform>();
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
            ThirdPersonController.instance.isAtCheckpoint1 = true;
            //CheckCheckpoint();
        }

    }

    public void CheckCheckpoint()
    {
        if (ThirdPersonController.instance.isAtCheckpoint1)
        {
            if (Input.GetKeyDown(KeyCode.E))
            {
                Console.WriteLine("Save Checkpoint");
                //shift the spawn point to where the current termainal position where the player save
                ThirdPersonController.instance.spawnPoint.localPosition = new vec3(terminalPos.localPosition.x + 1, terminalPos.localPosition.y + 1, terminalPos.localPosition.z);
                ThirdPersonController.instance.spawnPoint.localRotation = new vec3(terminalPos.localRotation.x, terminalPos.localRotation.y, terminalPos.localRotation.z);
            }
        }
    }

    void OnCollisionExit(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("AwayFromCheckpoint");
            ThirdPersonController.instance.isAtCheckpoint1 = false;
        }
    }
}
