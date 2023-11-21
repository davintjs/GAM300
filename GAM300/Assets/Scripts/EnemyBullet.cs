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
        Console.WriteLine("Enemy bullet hit object with tag: " + GetTag(other));
        if (GetTag(other) != "Enemy")
        {
            Console.WriteLine("Self destructed bullet");
            Destroy(gameObject);
        }
    }
}
