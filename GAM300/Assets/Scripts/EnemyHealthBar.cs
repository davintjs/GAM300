using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class EnemyHealthBar : Script
{
    
    Transform camera;

    void Start()
    {
        camera = ThirdPersonCamera.instance.transform;
    }

    void Update()
    {
        vec3 dir = transform.forward;
        if (camera != null)
        {
            dir = (camera.position - transform.position).Normalized;
        }
        // Get direction from health bar to camera

        UpdateRotation(dir);

    }
    void UpdateRotation(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;

        float angle = (float)Math.Atan2(-dir.x, -dir.z);

        transform.rotation = vec3.UnitY * angle;
    }

}
