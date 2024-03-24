using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Settings : Script
{
    public GameObject mainMenuBGImage;

    public GameObject backButton;

    public GameObject masterVolumeButton;

    public GameObject bgmVolumeButton;

    public GameObject sfxVolumeButton;

    public GameObject sliderButton;

    public Transform sliderFill;
    public float MinValue = 0f;
    public float MaxValue = 1f;
    public float value = 50f;
    private float sliderWidth;
    private bool onClick = false;



    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;
    public bool test = false;
    float mouse;

    public float sizeMultiplier = 1.5f;

    //sounds
    public AudioSource bgm;
    //public AudioSource uibutton;

    private SpriteRenderer backButtonRenderer;
    private SpriteRenderer masterVolumeButtonRenderer;
    private SpriteRenderer bgmVolumeButtonRenderer;
    private SpriteRenderer sfxVolumeButtonRenderer;

    private SpriteRenderer sliderButtonRenderer; 


    //vec3 startGridTextSize;

    void Start()
    {
        //bgm.Play();
        //startGridTextSize = new vec3(mainMenuTitle.transform.localScale);
        //currentRestTimer = restTimer;

        bgm.Play();
        test = true;


        if (backButton.HasComponent<SpriteRenderer>())
            backButtonRenderer = backButton.GetComponent<SpriteRenderer>();
        if (masterVolumeButton.HasComponent<SpriteRenderer>())
            masterVolumeButtonRenderer = masterVolumeButton.GetComponent<SpriteRenderer>();
        if (bgmVolumeButton.HasComponent<SpriteRenderer>())
            bgmVolumeButtonRenderer = bgmVolumeButton.GetComponent<SpriteRenderer>();
        if (sfxVolumeButton.HasComponent<SpriteRenderer>())
            sfxVolumeButtonRenderer = sfxVolumeButton.GetComponent<SpriteRenderer>();

        if (sliderButton.HasComponent<SpriteRenderer>())
            sliderButtonRenderer = sliderButton.GetComponent<SpriteRenderer>();

        sliderButton.SetActive(true);


        sliderWidth = sliderFill.localScale.x;
        Console.WriteLine("Slider Width: " + sliderWidth);


    }

    void Update()
    {
        // Get refto Button
        if (backButtonRenderer != null && backButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Scene");
            SceneManager.LoadScene("MainMenu", true);
        }
        if (masterVolumeButtonRenderer != null && masterVolumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Music");
            //InternalCalls.PauseMusic();
            //bgm.Pause();
            //SceneManager.LoadScene("MainMenu");
            if(test == true)
            {
                bgm.Pause();
                test = false;
            }
            else{
                bgm.Play();
                test = true;

            }
        }
        if (bgmVolumeButtonRenderer != null && bgmVolumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Music");
            //InternalCalls.PauseMusic();
            //bgm.Pause();
            //SceneManager.LoadScene("MainMenu");
            if(test == true)
            {
                bgm.Pause();
                test = false;
            }
            else{
                bgm.Play();
                test = true;

            }
        }
        if (sfxVolumeButtonRenderer != null && sfxVolumeButtonRenderer.IsButtonClicked())
        {
            //LoadScene(1.0f);
            Console.WriteLine("Music");
            //InternalCalls.PauseMusic();
            //bgm.Pause();
            //SceneManager.LoadScene("MainMenu");
            if(test == true)
            {
                bgm.Pause();
                test = false;
            }
            else{
                bgm.Play();
                test = true;

            }
        }

        if(sliderButtonRenderer.IsButtonClicked())
        {
            onClick = true;
        }

        // Get refto Button
        if (Input.GetMouseHolding(0) && onClick)
        {
            
            //Update the slider value 
            UpdateSliderValue();
            Console.WriteLine("Slider Width: " + sliderWidth);

            //Update the slider's visual representation
            UpdateSliderVisual();
            Console.WriteLine(Input.GetMouseDelta());
        }
        else if(Input.GetMouseHolding(0) != true)
        {
            onClick = false;
        }

        //if(Input.GetMouseDown(0))
        //{
        //    if(IsMouseOverSlider())
        //    {
        //        isDragging = true;
        //        previousMousePosition = Input.GetMousePosition().x;
        //    }
        //}

        //if(isDragging && Input.GetMouseDown(0))
        //{

        //}

        //if(!Input.GetMouseDown(0))
        //{
        //    isDragging=false;
        //}

        //if(Input.GetMouseDown(0))
        //{

        //}

        ////Update the slider value 
        //UpdateSliderValue();
        //Console.WriteLine("Slider Width: " + sliderWidth);

        ////Update the slider's visual representation
        //UpdateSliderVisual();
        //Console.WriteLine(Input.GetMouseDelta());


    }
    void UpdateSliderValue()
    {
  
        //Calculating the new value based on the mouse position
        mouse = Input.GetMouseDelta().x;
        vec3 slider = sliderButton.transform.localPosition;
        //float normalisedX = (mouse - slider.x) / sliderWidth;
        slider.x += mouse * 16;
        sliderButton.transform.localPosition = slider;
        Console.WriteLine("Update");


        //if (normalisedX < 0)
        //{
        //    normalisedX = 0;
        //}
        //else if (normalisedX > 1)
        //{
        //    normalisedX = 1;
        //}

        //value = MinValue + normalisedX * (MaxValue - MinValue);
        
    }

    void UpdateSliderVisual()
    {
        vec3 newScale = sliderFill.localScale;
        newScale.x = (value - MinValue) / (MaxValue - MinValue) * sliderWidth;
        sliderFill.localScale = newScale;
        Console.WriteLine("Slider Width: " + sliderWidth);

    }

    //private bool IsMouseOverSlider()
    //{
    //    vec3 sliderButtonPos = sliderButton.transform.position;
    //    vec2 mousePos = Input.GetMouseDelta();
    //    float distance = vec3.Distance(sliderButtonPos, mousePos);
    //    float radius = 
    //}




}
