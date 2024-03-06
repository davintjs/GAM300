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
    //public float alphaValue = 0.8f;
    //public Transform player;
    //public SpriteRenderer Objective1;


    //private bool d1;
    //public SpriteRenderer Dialogue1;
    //public AudioSource audio1;


    //bool temp = false;

    //vec3 startGridTextSize;

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
