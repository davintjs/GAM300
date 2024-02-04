using BeanFactory;
using GlmSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class TransformEffect : Script
{
    public float duration;
    float timer = 0f;

    public vec3 translationEffect;
    public vec3 rotateEffect;
    public vec3 scaleEffect;

    vec3 startPos;
    vec3 startRot;
    vec3 startScale;
    vec3 finalPos;
    vec3 finalRot;
    vec3 finalScale;

    public bool finished = false;

    public void Start()
    {    
        startPos = transform.localPosition;
        startRot = transform.localRotation;
        startScale = transform.localScale;

        finalPos = transform.localPosition + translationEffect;
        finalRot = transform.localRotation + rotateEffect;
        finalScale = transform.localScale + scaleEffect;
    }

    void Update()
    {
        vec3 pos = transform.localPosition;
        vec3 rot = transform.localRotation;
        vec3 scale = transform.localScale;

        if (timer < duration)
        {
            transform.localPosition = vec3.Lerp(startPos, finalPos,timer/duration);
            transform.localRotation = vec3.Lerp(startRot, finalRot, timer / duration);
            transform.localScale = vec3.Lerp(startScale, finalScale, timer / duration);
            timer += Time.deltaTime;
        }
        else
        {
            finished = true;
        }
    }
}
