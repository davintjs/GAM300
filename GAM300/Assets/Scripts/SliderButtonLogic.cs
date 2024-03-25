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
    //public float MinValue = 0f;
    //public float MaxValue = 1f;
    //public float value = 0.5f;
    public vec3 MinValue = new vec3(0);
    public vec3 MaxValue = new vec3(1);
    public vec3 value = new vec3(1);
    private float sliderWidth;
    private bool onClick = false;
    private float mouse; 

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

        if (sliderButtonRenderer.IsButtonClicked())
        {
            onClick = true;
        }

        if (Input.GetMouseHolding(0) && onClick)
        {

            //Update the slider value 
            UpdateSliderValue();
            Console.WriteLine(MinValue + " " + MaxValue);

            //Update the slider's visual representation
            UpdateSliderVisual();
        }
        else if (Input.GetMouseHolding(0) != true)
        {
            onClick = false;
        }
    }

    void UpdateSliderValue()
    {

        //Calculating the new value based on the mouse position
        mouse = Input.GetMouseDelta().x;
        vec3 slider = button.transform.localPosition;
        slider.x += mouse * 16;
        button.transform.localPosition = slider;

    }

    void UpdateSliderVisual()
    {
        //vec3 slider = button.transform.localPosition;
        vec3 newScale = sliderFill.localScale;
        vec3 fillScale = (value - MinValue) / (MaxValue - MinValue);
        //newScale.x = (value - MinValue) / (MaxValue - MinValue);
        //sliderFill.localScale = newScale;
        newScale = fillScale;
    }


}
