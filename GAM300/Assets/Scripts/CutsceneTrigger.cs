using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

class CutsceneTrigger : Script
{
    public Transform cameraPosition;

    bool triggered = false;

    public float cutsceneDuration = 2f;

    ThirdPersonCamera camera;

    void Start()
    {
        camera = ThirdPersonCamera.instance;
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (triggered) return;
        if (GetTag(rb) == "Player")
        {
            StartCoroutine(stopCutscene());
            Console.WriteLine("Cutscene Triggered!");
        }
    }

    IEnumerator stopCutscene()
    {
        camera.cutscene = true;
        triggered = true;
        camera.transform.position = cameraPosition.position;
        camera.transform.rotation = cameraPosition.rotation;
        yield return new WaitForSeconds(cutsceneDuration);
        camera.cutscene = false;
    }
}
