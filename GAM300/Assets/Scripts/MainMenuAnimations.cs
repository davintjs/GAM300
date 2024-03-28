using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class MainMenuAnimations : Script
{
    public GameObject objectToAnimate;
    public GameObject initial;
    public GameObject final;
    public int easingType = 0;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;

    private vec3 initialPos;
    private vec3 finalPos;

    void Start()
    {
        initialPos = initial.transform.position;
        finalPos = final.transform.position;
    }

    void Update()
    {
        movement(objectToAnimate);
    }

    void movement(GameObject gameObj)
    {
        if (!back)
        {
            gameObj.transform.position = vec3.Lerp(finalPos, initialPos, timer, duration, (vec3.EasingType)easingType);
        }
        else
        {
            gameObj.transform.position = vec3.Lerp(initialPos, finalPos, timer, duration, (vec3.EasingType)easingType);
        }
        timer += Time.deltaTime;
        if (timer >= duration)
        {
            timer = 0f;
            back = !back;
        }
    }
}
