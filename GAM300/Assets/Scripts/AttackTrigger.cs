using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class AttackTrigger : Script
{
    //public Transform meleeEnemy;

    void Start()
    {

    }

    void Update()
    {
        //transform.localPosition = meleeEnemy.position;
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (GetTag(rb) == "Player")
        {
            //reference the player object and let the player take damage
            rb.gameObject.GetComponent<ThirdPersonController>().TakeDamage(1);
        }

    }
}
