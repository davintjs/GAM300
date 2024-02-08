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
    public float alphaValue = 0.8f;
    public Transform player;
    public SpriteRenderer Objective1;


    bool temp = false;

    //vec3 startGridTextSize;

    void Start()
    {
    }

    void Update()
    {
        if(player.position.z < -43f)
        {
            if (!temp)
            {
                temp = true;
                Objective1.alpha = alphaValue;
                AudioManager.instance.itemCollected.Play();
            }
            
        }
            
    }

   


}
