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

    public SpriteRenderer button;
    public bool isText = false;
    private bool hovered = false;
    private bool soundPlayed = false;

    float hoverTimer = 0f;
    public float hoverDuration = 1.0f;
    public vec3 targetScaleMultiplier = new vec3(1);
    vec3 originalScale;

    private vec3 initialColor;
    private vec3 finalColor;
    private TextRenderer textRenderer;

    void Start()
    {
        originalScale = transform.localScale;
        if(!isText)
            button = GetComponent<SpriteRenderer>();
        else
        {
            textRenderer = GetComponent<TextRenderer>();
            initialColor = textRenderer.color.xyz;
            finalColor = textRenderer.color.xyz * 2f;
        }
    }

    void Update()
    {
        ButtonUpdate();
    }

    void ButtonUpdate()
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

                if(isText)
                    textRenderer.color.xyz = vec3.Lerp(textRenderer.color.xyz, finalColor, hoverTimer / hoverDuration);
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
                transform.localScale = vec3.Lerp(originalScale, originalScale * targetScaleMultiplier, hoverTimer / hoverDuration);

                if (isText)
                    textRenderer.color.xyz = vec3.Lerp(textRenderer.color.xyz, initialColor, hoverTimer / hoverDuration);
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
