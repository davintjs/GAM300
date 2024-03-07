using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class EnemyBullet : Script
{
    public bool destroyOnTrigger = true;

    void OnTriggerEnter(PhysicsComponent other)
    {
        if (!destroyOnTrigger)
            return;
        if (GetTag(other) != "Enemy" && GetTag(other) != "EnemyAttack" && GetTag(other) != "PlayerAttack")
        {
            Destroy(gameObject);
        }
    }
}
