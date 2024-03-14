using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class ButtonHover : Script
{
    public AudioSource hoverSound;

    private SpriteRenderer button;

    void Start()
    {
        button = GetComponent<SpriteRenderer>();
    }
    
    void Update()
    {
        if (button.IsButtonHovered())
        {
            hoverSound.Play();
        }
    }
}
