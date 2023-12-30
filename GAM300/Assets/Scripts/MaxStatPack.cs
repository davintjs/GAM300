using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class MaxStatPack : Script
{
    void OnTriggerEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("MaxStat Pack Collected");
            AudioManager.instance.itemCollected.Play();
            Inventory.instance.maxStatPackCount++;
            Destroy(gameObject);

        }

    }
}
