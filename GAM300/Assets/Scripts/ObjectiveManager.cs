using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class ObjectiveManager : Script
{

    public static ObjectiveManager Instance;

    public GameObject tut1;
    public GameObject tut2;
    public GameObject tut3;

    void Awake()
    {
        Instance = this;
    }

    void Start()
    {
        //timer = 17f;
        //d1 = false;
    }

    void Update()
    {
        //if (d1 && (timer > 0.f))
        //{
        //    timer -= Time.deltaTime;
        //}
        //else
        //    Objective1.alpha = 0.f;
    }

    public void SetState(int i)
    {
        switch (i)
        {
            case 0:
                break;
            case 1:
                tut1.SetActive(true);
                tut2.SetActive(true);
                break;
            case 2:
                tut3.SetActive(true);
                break;
            case 3:
                break;
        }
        //play UI sound
        AudioManager.instance.itemCollected.Play();
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        //detect the player
        //if (GetTag(rb) == "StartObjective")
        //{
        //    if (!temp)
        //    {
        //        temp = true;
        //        Objective1.alpha = alphaValue;
        //        AudioManager.instance.itemCollected.Play();
        //    }
        //}

        //if (GetTag(rb) == "Dialogue1")
        //{
        //    if (!d1)
        //    {
        //        d1 = true;
        //        Dialogue1.alpha = 1f;
        //        audio1.Play();
        //    }
        //}

    }

}
