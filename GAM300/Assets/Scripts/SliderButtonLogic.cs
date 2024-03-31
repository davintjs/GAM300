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

    public float MinValue = 0f;
    public float MaxValue = 1f;
    public float value = 0.5f;

    private float sliderWidth;
    private float mouse = 0f; 
    private bool onClick = false;
    public bool inverted = false;

    private SpriteRenderer sliderButtonRenderer;
    private TextRenderer sliderTextRenderer;

    public float minX = 0f;
    public float maxX = 0f;
    public float slideSpeed = 800f;

    void Awake()
    {
        sliderButtonRenderer = buttonObj.GetComponent<SpriteRenderer>();
        sliderTextRenderer = sliderTextObj.GetComponent<TextRenderer>();
    }

    void Start()
    {
        //sliderWidth = sliderFillObj.transform.scale.x;
        //minX = -sliderWidth * 0.5f;
        //maxX = sliderWidth * 0.5f;
        sliderWidth = Mathf.Abs(minX) + maxX;
        mouse = buttonObj.transform.localPosition.x;
    }

    void Update()
    {
        if (sliderButtonRenderer.IsButtonClicked())
        {
            onClick = true;
        }

        if (Input.GetMouseHolding(0) && onClick)
        {
            //Update the slider value 
            UpdateSliderValue();
        }
        else if (!Input.GetMouseHolding(0))
        {
            onClick = false;
        }
    }

    public void UpdateSliderValue()
    {
        //Calculating the new value based on the mouse position
        vec3 slider = buttonObj.transform.localPosition;
        float mouseDelta = Input.GetMouseDelta().x * slideSpeed * Time.unscaledDeltaTime;
        mouse += (inverted) ? -mouseDelta : mouseDelta;

        //Console.WriteLine("Mouse: " + mouse + " " + maxX + " " + minX);

        slider.x = Mathf.Clamp(mouse, minX, maxX);
        //Console.WriteLine("Slider: " + slider.x); 

        float newValue = (inverted) ? -slider.x : slider.x;
        value = (newValue + Mathf.Abs(minX)) / sliderWidth;
        value = value * (MaxValue - MinValue) + MinValue;
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
