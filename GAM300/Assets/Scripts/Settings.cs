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


    public bool isStartActive = true;
    public float flickerTimer = 0f;

    //movement variables
    public float duration = 2f;
    public float timer = 0f;
    public bool back = false;
    public bool test = false;

    public float sizeMultiplier = 1.5f;

    //sounds
    public AudioSource bgm;
    //public AudioSource uibutton;

    private SpriteRenderer backButtonRenderer;
    private SpriteRenderer masterVolumeButtonRenderer;
    private SpriteRenderer bgmVolumeButtonRenderer;
    private SpriteRenderer sfxVolumeButtonRenderer;


    vec3 startGridTextSize;

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
   


        // Check if button state is clicked

        // Do something(Load)
        //goToPlay();

        //code not working atm
        //selfFlicker();
       // movement();
    }

   


}
