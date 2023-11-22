using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class EnemyHealthBar : Script
{
    
    public Transform player;
    public float RotationSpeed = 2.0f;
    public Transform parentTransform;

    private vec3 pDir;

    void Start()
    {
    }

    void Update()
    {
        // Get direction from health bar to player
        vec3 direction = (player.localPosition - transform.localPosition).Normalized;

        pDir = (player.localPosition - parentTransform.localPosition);
        pDir.y = 0;
        pDir = pDir.Normalized;
        UpdateRotation(pDir);

        transform.localRotation.x = 0.0f;
        transform.localRotation.z = 0.0f;

    }
    void UpdateRotation(vec3 dir)
    {
        if (dir == vec3.Zero)
            return;

        float angle = (float)Math.Atan2(-dir.x, -dir.z);
        float pAngle = parentTransform.localRotation.y;

        angle -= pAngle;

        quat newQuat = glm.FromEulerToQuat(new vec3(0, angle, 0)).Normalized;
        quat oldQuat = glm.FromEulerToQuat(transform.localRotation).Normalized;

        // Interpolate using spherical linear interpolation (slerp)
        //quat midQuat = quat.SLerp(oldQuat, newQuat, Time.deltaTime * RotationSpeed);

        vec3 rot = ((vec3)newQuat.EulerAngles);


        if (rot != vec3.NaN)
        {
            bool isNan = false;
            foreach (float val in rot)
            {
                if (float.IsNaN(val))
                {
                    isNan = true;
                    break;
                }
            }
            if (!isNan)
            {
                transform.localRotation = rot;
            }
        }
    }

}
