using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class ButtonLogic : Script
{
    public AudioSource hoverSound;
    public AudioSource clickSound;

    private SpriteRenderer button;
    private bool hovered = false;
    private bool soundPlayed = false;

    void Start()
    {
        button = GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        // Hover sound
        if (button.IsButtonHovered())
        {
            hovered = true;
        }
        else
        {
            hovered = false;
            soundPlayed = false;
        }

        if (hovered && soundPlayed == false)
        {
            hoverSound.Play();
            soundPlayed = true;
        }

        // Click sound
        if (button.IsButtonClicked())
        {
            clickSound.Play();
        }
    }
}
