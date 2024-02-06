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

    void Start()
    {
    }

    void Update()
    {

        rotation();
    }

    void rotation()
    {
        vec3 rot = transform.localRotation;
        //Rotation of the propellor
        if (rotateClockwise)
            rot += new vec3(0f, 0f, 10f) * Time.deltaTime * rotateSpeed;
        else
            rot += new vec3(0f, 0f, -10f) * Time.deltaTime * rotateSpeed;
        transform.localRotation = rot;
    }
}
