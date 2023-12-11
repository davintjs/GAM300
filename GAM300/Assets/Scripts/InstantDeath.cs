using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;


public class InstantDeath : Script
{
    public ThirdPersonController player;
    public bool hit = false;
    //CharacterController player;

    void Update()
    {
        //if(hit)
        //{
        //    //player = GetComponent<ThirdPersonController>();
        //    //player.currentHealth = 0;//set player health to 0 to trigger player death animation
        //    player.TakeDamage(player.maxHealth);
        //    player.UpdatehealthBar();
        //}
    }
    void OnCollisionEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            hit = true;
            Console.WriteLine("InstantDeath");
            player.TakeDamage(player.maxHealth);
            player.UpdatehealthBar();
            //player = rb.gameObject.GetComponent<ThirdPersonController>();
            //player.currentHealth = 0;//set player health to 0 to trigger player death animation


        }
    }
}
