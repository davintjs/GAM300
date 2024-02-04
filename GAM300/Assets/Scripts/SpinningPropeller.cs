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

    public bool originalRotation = true;

    vec3 tempTransform;

    void Start()
    {
        tempTransform = transform.up;
    }

    void Update()
    {

        rotation();
    }

    void rotation()
    {
        if (originalRotation == true)
        {
            //ORIGINAL ROTATION
            if (rotateClockwise && !rotateAntiClockwise)
            {
                rotateAntiClockwise = false;
                //GetComponent<Rigidbody>().angularVelocity.z = gameObject.transform.position.z * -rotateSpeed;
                vec3 rot = transform.rotation + new vec3(0f, 0f, 10f) * Time.deltaTime * rotateSpeed;

                //reset rotation so it only rotates z axis

                transform.rotation = rot;
            }
            if (rotateAntiClockwise && !rotateClockwise)
            {
                rotateClockwise = false;
                //GetComponent<Rigidbody>().angularVelocity = new vec3(gameObject.transform.position) * rotateSpeed;
                vec3 rot = transform.rotation + new vec3(0f, 0f, -10f) * Time.deltaTime * rotateSpeed;

                transform.rotation = rot;
            }
        }

        if (originalRotation == false)
        {
            //ORIGINAL ROTATION
            if (rotateClockwise && !rotateAntiClockwise)
            {
                rotateAntiClockwise = false;
                //GetComponent<Rigidbody>().angularVelocity.z = gameObject.transform.position.z * -rotateSpeed;
                vec3 rot = transform.rotation + tempTransform * Time.deltaTime * rotateSpeed;

                //reset rotation so it only rotates z axis

                transform.rotation = rot;
            }
            if (rotateAntiClockwise && !rotateClockwise)
            {
                rotateClockwise = false;
                //GetComponent<Rigidbody>().angularVelocity = new vec3(gameObject.transform.position) * rotateSpeed;
                vec3 rot = transform.rotation + tempTransform * Time.deltaTime * -rotateSpeed;

                transform.rotation = rot;
            }
        }
    }
}
