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

    float hoverTimer = 0f;
    public float hoverDuration = 1.0f;
    public vec3 targetScaleMultiplier = new vec3(1);
    vec3 originalScale;

    void Start()
    {
        originalScale = transform.localScale;
        button = GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        // Hover sound
        if (button.IsButtonHovered())
        {
            hovered = true;
            if (hoverTimer < hoverDuration)
            {
                hoverTimer += Time.unscaledDeltaTime;
                if (hoverTimer > hoverDuration)
                {
                    hoverTimer = hoverDuration;
                }
                transform.localScale = vec3.Lerp(originalScale, originalScale * targetScaleMultiplier, hoverTimer / hoverDuration);
            }
        }
        else
        {
            hovered = false;
            soundPlayed = false;
            if (hoverTimer > 0f)
            {
                hoverTimer -= Time.unscaledDeltaTime;
                if (hoverTimer < 0f)
                {
                    hoverTimer = 0f;
                }
                transform.localScale = vec3.Lerp(originalScale,originalScale*targetScaleMultiplier,hoverTimer/hoverDuration);
            }
        }

        if (hovered && soundPlayed == false)
        {
            if (hoverSound != null)
                hoverSound.Play();
            soundPlayed = true;
        }

        // Click sound
        if (button.IsButtonClicked())
        {
            if (clickSound != null)
                clickSound.Play();
        }
    }
}
