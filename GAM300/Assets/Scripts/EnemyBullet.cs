using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class EnemyBullet : Script
{
    void OnTriggerEnter(PhysicsComponent other)
    {
        if (GetTag(other) != "Enemy" && GetTag(other) != "EnemyAttack")
        {
            Destroy(gameObject);
        }
    }
}
