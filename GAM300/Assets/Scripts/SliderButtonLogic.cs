using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class SliderButtonLogic : Script
{
    public AudioSource hoverSound;
    public AudioSource clickSound;

    public GameObject button;
    private bool hovered = false;
    private bool soundPlayed = false;

    public Transform sliderFill;
    public float MinValue = 0f;
    public float MaxValue = 1f;
    public float value = 50f;
    private float sliderWidth;
    private bool onClick = false;
    private SpriteRenderer sliderButtonRenderer;


    float hoverTimer = 0f;
    public float hoverDuration = 1.0f;
    public vec3 targetScaleMultiplier = new vec3(1);
    vec3 originalScale;

    void Start()
    {
        if (button.HasComponent<SpriteRenderer>())
            sliderButtonRenderer = button.GetComponent<SpriteRenderer>();

        originalScale = transform.localScale;

        sliderWidth = sliderFill.localScale.x;

    }

    void Update()
    {
        // Hover sound
        if (sliderButtonRenderer.IsButtonHovered())
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
        if (sliderButtonRenderer.IsButtonClicked())
        {
            if (clickSound != null)
                clickSound.Play();
        }
    }
}
