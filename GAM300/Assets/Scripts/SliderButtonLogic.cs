using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class SliderButtonLogic : Script
{
    public GameObject buttonObj;
    //public GameObject sliderFillObj;
    public GameObject sliderTextObj;

    public GameObject decrementbuttonObj;
    public GameObject incrementbuttonObj;

    public float MinValue = 0f;
    public float MaxValue = 1f;
    public float value = 0.5f;

    private float sliderWidth;
    private float mouse = 0f; 
    private bool onClick = false;
    public bool inverted = false;
    public bool screenSpace = false;

    private SpriteRenderer sliderButtonRenderer;
    private TextRenderer sliderTextRenderer;

    private SpriteRenderer decrementButtonRenderer;
    private SpriteRenderer incrementButtonRenderer;


    public float minX = 0f;
    public float maxX = 0f;
    public float slideSpeed = 800f;


    
    public AudioSource hoverSound;  
    public AudioSource clickSound;

    public float hoverDuration = 1.0f;
    float decHoverTimer = 0f;
    float incHoverTimer = 0f;

    private bool hovered = false;

    public vec3 targetScaleMultiplier = new vec3(2f);
    private bool soundPlayed = false;


    vec3 decrementOriginalScale;
    vec3 incrementOriginalScale;
    void Awake()
    {
        sliderButtonRenderer = buttonObj.GetComponent<SpriteRenderer>();
        sliderTextRenderer = sliderTextObj.GetComponent<TextRenderer>();
        decrementButtonRenderer = decrementbuttonObj.GetComponent<SpriteRenderer>();
        incrementButtonRenderer = incrementbuttonObj.GetComponent<SpriteRenderer>();
    }

    void Start()
    {
        //sliderWidth = sliderFillObj.transform.scale.x;
        //minX = -sliderWidth * 0.5f;
        //maxX = sliderWidth * 0.5f;
        sliderWidth = Mathf.Abs(minX) + maxX;
        mouse = buttonObj.transform.localPosition.x;

        decrementOriginalScale = decrementbuttonObj.transform.localScale;
        incrementOriginalScale = incrementbuttonObj.transform.localScale;

    }

    void Update()
    {
        if (decrementButtonRenderer.IsButtonHovered() || incrementButtonRenderer.IsButtonHovered())
        {
            hovered = true;

            if (decrementButtonRenderer.IsButtonHovered())
            {
                if (decHoverTimer < hoverDuration)
                {
                    decHoverTimer += Time.unscaledDeltaTime;
                    if (decHoverTimer > hoverDuration)
                    {
                        decHoverTimer = hoverDuration;
                    }

                    decrementbuttonObj.transform.localScale = vec3.Lerp(decrementOriginalScale, decrementOriginalScale * targetScaleMultiplier,
                        decHoverTimer / hoverDuration);

                }

            }
            else
            {

                if (incHoverTimer < hoverDuration)
                {
                    incHoverTimer += Time.unscaledDeltaTime;
                    if (incHoverTimer > hoverDuration)
                    {
                        incHoverTimer = hoverDuration;
                    }

                    incrementbuttonObj.transform.localScale = vec3.Lerp(incrementOriginalScale, incrementOriginalScale * targetScaleMultiplier,
                        incHoverTimer / hoverDuration);

                }

            }

        }
        else
        {

            hovered = false;
            soundPlayed = false;
            if (decHoverTimer > 0f)
            {
                decHoverTimer -= Time.unscaledDeltaTime;
                if (decHoverTimer < 0f)
                {
                    decHoverTimer = 0f;
                }
                //transform.localScale = vec3.Lerp(originalScale, originalScale * targetScaleMultiplier, hoverTimer / hoverDuration);
                decrementbuttonObj.transform.localScale = vec3.Lerp(decrementOriginalScale, decrementOriginalScale * targetScaleMultiplier,
                    decHoverTimer / hoverDuration);
            }

            if (incHoverTimer > 0f)
            {
                incHoverTimer -= Time.unscaledDeltaTime;
                if (incHoverTimer < 0f)
                {
                    incHoverTimer = 0f;
                }

                incrementbuttonObj.transform.localScale = vec3.Lerp(incrementOriginalScale, incrementOriginalScale * targetScaleMultiplier,
                    incHoverTimer / hoverDuration);
            }

        }
        
        
        
        if (hovered && soundPlayed == false)
        {
            if (hoverSound != null)
                hoverSound.Play();
            soundPlayed = true;
        }

        //if (sliderButtonRenderer.IsButtonClicked())
        //{
        //    onClick = true;
        //}

        //if (Input.GetMouseHolding(0) && onClick)
        //{
        //    //Update the slider value 
        //    UpdateSliderValue();
        //}
        //else if (!Input.GetMouseHolding(0))
        //{
        //    onClick = false;
        //}
        if (decrementButtonRenderer.IsButtonClicked() || incrementButtonRenderer.IsButtonClicked())
        {
            if (clickSound != null)
                clickSound.Play();
            UpdateSliderValue();
        }




    }

    public void UpdateSliderValue()
    {
        vec3 slider = buttonObj.transform.localPosition;



        if (decrementButtonRenderer.IsButtonClicked())
        {
            value -= 0.1f;
        }
        else
        {
            value += 0.1f;
        }
         

        value = Mathf.Clamp(value, MinValue, MaxValue);

        //buttonObj.transform.localPosition = slider;


        ////Calculating the new value based on the mouse position
        //vec3 slider = buttonObj.transform.localPosition;
        //float mouseDelta = Input.GetMouseDelta().x * slideSpeed * Time.unscaledDeltaTime;

        ////Console.WriteLine("Pos: " + Input.GetMousePosition().x);
        //if (screenSpace)
        //    mouseDelta = Input.GetMousePosition().x;

        //mouse += (inverted) ? -mouseDelta : mouseDelta;

        ////Console.WriteLine("Mouse: " + mouse + " " + maxX + " " + minX);



        //slider.x = Mathf.Clamp(value, minX, maxX);
        //////Console.WriteLine("Slider: " + slider.x); 

        //float newValue = (inverted) ? -slider.x : slider.x;
        //value = (newValue + Mathf.Abs(minX)) / sliderWidth;
        //value = value * (MaxValue - MinValue) + MinValue;
        //buttonObj.transform.localPosition = slider;

        //vec3 slider = buttonObj.transform.localPosition;

        float tempWidth = Mathf.Abs(minX) + maxX;
        float sliderValue = (value - MinValue) / (MaxValue - MinValue);
        sliderValue = (sliderValue * tempWidth) - Mathf.Abs(minX);
        sliderValue = (inverted) ? -sliderValue : sliderValue;
        slider.x = Mathf.Clamp(sliderValue, minX, maxX);
        mouse = slider.x;

        buttonObj.transform.localPosition = slider;
        sliderTextRenderer.text = value.ToString("0.00");

    }

    public void InitializeSliderValue(float startValue)
    {
        value = startValue;

        float tempWidth = Mathf.Abs(minX) + maxX;

        vec3 slider = buttonObj.transform.localPosition;
        float sliderValue = (value - MinValue) / (MaxValue - MinValue);
        sliderValue = (sliderValue * tempWidth) - Mathf.Abs(minX);
        sliderValue = (inverted) ? -sliderValue : sliderValue;
        slider.x = Mathf.Clamp(sliderValue, minX, maxX);
        mouse = slider.x;

        buttonObj.transform.localPosition = slider;
        sliderTextRenderer.text = value.ToString("0.00"); 
    }
}
