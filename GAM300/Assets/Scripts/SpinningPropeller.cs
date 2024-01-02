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
            transform.localRotation = new vec3(0f, 0f, 10f) * Time.deltaTime * rotateSpeed;

            //reset rotation so it only rotates z axis
            transform.localRotation.x = 0f;
            transform.localRotation.y = 0f;
        }
        if (rotateAntiClockwise && !rotateClockwise)
        {
            rotateClockwise = false;
            transform.localRotation = new vec3(0f, 0f, -10f) * Time.deltaTime * rotateSpeed;

            //reset rotation so it only rotates z axis
            transform.localRotation.x = 0f;
            transform.localRotation.y = 0f;
        }
    }
}
