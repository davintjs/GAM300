using BeanFactory;
using GlmSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class LoadingIcon : Script
{
    SpriteRenderer spriteRenderer;

    public float speed = 1f;

    void Awake()
    {
        spriteRenderer = GetComponent<SpriteRenderer>();
    }

    void Update ()
    {
        if (spriteRenderer == null)
            return;
        transform.localRotation -= new vec3(0, 0, 1) * speed * Time.deltaTime;
    }
}
