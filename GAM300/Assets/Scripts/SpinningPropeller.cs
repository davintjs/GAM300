using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class SpinningPropeller : Script
{
    public float rotateSpeed = 3f;
    public bool rotateClockwise = true;
    public bool rotateAntiClockwise = false;


    void Start()
    {

    }

    void Update()
    {

        if (rotateClockwise && !rotateAntiClockwise)
        {
            rotateAntiClockwise = false;

            vec3 rot = new vec3(0f, 0f, 10f) * Time.deltaTime * rotateSpeed;

            //reset rotation so it only rotates z axis
            rot.x = 0f;
            rot.y = 0f;

            transform.localRotation = rot;
        }
        if (rotateAntiClockwise && !rotateClockwise)
        {
            rotateClockwise = false;
            vec3 rot = new vec3(0f, 0f, -10f) * Time.deltaTime * rotateSpeed;

            //reset rotation so it only rotates z axis
            rot.x = 0f;
            rot.y = 0f;

            transform.localRotation = rot;
        }
    }
}
