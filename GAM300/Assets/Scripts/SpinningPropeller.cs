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
            //GetComponent<Rigidbody>().angularVelocity.z = gameObject.transform.position.z * -rotateSpeed;
            vec3 rot = transform.localRotation + new vec3(0f, 0f, 10f) * Time.deltaTime * rotateSpeed;

            //reset rotation so it only rotates z axis

            transform.localRotation = rot;
        }
        if (rotateAntiClockwise && !rotateClockwise)
        {
            rotateClockwise = false;
            //GetComponent<Rigidbody>().angularVelocity = new vec3(gameObject.transform.position) * rotateSpeed;
            vec3 rot = transform.localRotation + new vec3(0f, 0f, -10f) * Time.deltaTime * rotateSpeed;

            transform.localRotation = rot;
        }
    }
}
