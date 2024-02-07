using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

class CutsceneTrigger : Script
{
    public Transform cameraPosition;
    public Transform camera;

    bool triggered = false;

    void Update()
    {
        if (triggered)
        {
            camera.position = cameraPosition.position;
            camera.rotation = cameraPosition.rotation;
        }
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (triggered)
            return;
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("Cutscene Triggered!");
            triggered = true;

        }
    }

    void OnTriggerExit(PhysicsComponent rb)
    {
        if (GetTag(rb) == "Player")
        {
            triggered = false;
        }
    }
}
