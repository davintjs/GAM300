using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class PlatformScript : Script
{
    public Transform startPoint;
    public Transform endPoint;
    public float duration = 2f;
    float timer = 0f;
    bool back = false;

    void Update()
    {
        if (!back)
        {
            transform.localPosition = vec3.Lerp(startPoint.localPosition,endPoint.localPosition , timer / duration);
        }
        else
        {
            transform.localPosition = vec3.Lerp(endPoint.localPosition, startPoint.localPosition, timer / duration);
        }
        timer += Time.deltaTime;
        if (timer >= duration)
        {
            back = !back;
            timer = 0f;
        }
    }
}
