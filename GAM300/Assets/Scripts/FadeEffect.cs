using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;


public class FadeEffect : Script
{
    public float inDuration = 1f;
    public float outDuration = 1f;

    SpriteRenderer spriteRenderer;

    public bool blink = false;

    float timer = 0f;

    float startVal = 0f;

    public float waitDelay = 0f;

    public float targetVal = 1f;

    public bool inAndOut = false;

    public bool finished = false;

    //If currently inverted
    bool inverted = false;

    public bool start = true;

    public void Start()
    {
        spriteRenderer = GetComponent<SpriteRenderer>();
        if (spriteRenderer != null)
        {
            startVal = spriteRenderer.alpha;
        }
    }

    void Update()
    {
        if (spriteRenderer == null)
            return;
        if (!start)
            return;

        if (inverted)
        {
            timer -= Time.deltaTime;
            if (timer <= -waitDelay)
            {
                timer = 0;
                inverted = false;
                if (blink)
                    spriteRenderer.alpha = 1;
                finished = true;
            }
            else
            {
                if (!blink)
                    spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, timer / outDuration);
            }
        }
        else
        {
            timer += Time.deltaTime;
            if (timer >= inDuration + waitDelay) 
            {
                if (inAndOut)
                {
                    timer = outDuration;
                    inverted = true;
                    
                }
                else
                {
                    finished = true;
                }
                if (blink)
                    spriteRenderer.alpha = 0;
            }
            else
            {
                if (!blink)
                    spriteRenderer.alpha = Mathf.Lerp(startVal, targetVal, timer / inDuration);
            }
        }
    }

    public void Reset()
    {
        timer = 0;
    }

}
