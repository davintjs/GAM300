using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using BeanFactory;
using GlmSharp;

class FlashMaterial : Script
{
    public MeshRenderer materialOwner;
    vec4 origColor;
    public vec3 color;
    public float interval = 1f;
    public int numOfFlashes = 4;
    bool flashed = false;

    bool done = false;

    void Start()
    {
        origColor = materialOwner.material.color;
    }

    void Update()
    {
        //materialOwner.material.SetRawColor(new vec4(color, 1));
    }

    public void OnTriggerEnter(PhysicsComponent rb)
    {
        if (done) return;
        if (GetTag(rb) == "Player")
            StartCoroutine(Flash());
    }

    IEnumerator Flash()
    {
        done = true;
        for (int i = 0; i < numOfFlashes; i++)
        {
            if (flashed)
            {
                materialOwner.material.SetRawColor(origColor);
                flashed = false;
            }
            else
            {
                materialOwner.material.SetRawColor(new vec4(color, 1));
                flashed = true;
            }
            yield return new WaitForSeconds(interval);
        }
    }
}