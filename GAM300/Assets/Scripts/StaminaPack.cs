using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class StaminaPack : Script
{
    void OnTriggerEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("Stamina Pack Collected");
            AudioManager.instance.itemCollected.Play();
            Inventory.instance.staminaPackCount++;
            Destroy(gameObject);

        }

    }
}
